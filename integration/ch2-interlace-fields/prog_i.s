; prog_i.s -- Program I (SPECIFICATION.md section 3): interlaced PAL
; bitplane display with one Copper list per field, toggled by a level-3
; interrupt handler this program installs itself. Serves P1, P4, P5, P6.
;
; Variants (section 3.5) assemble this same file with one symbol set, the
; way prog_control.s in apparatus-vasm-program does:
;   I_NOLACE     I-a   BPLCON0 = $1200 (LACE = 0)
;   I_SAMEPTR    I-b   LISTB writes PIC, not PIC+40, into BPL1PTH/L
;   I_NOPTRLIST  I-c   both lists omit the two BPL1PT MOVEs
;   I_NOTOGGLE   I-d   the handler omits the COP1LC rewrite (still counts, still clears)
;   I_NOSPRLIST  I-e   both lists omit the two SPR0PT MOVEs
;   I_NOCOPEN    I-f   DMACON = $8320 (COPEN not set)
;   I_NOCOPINT   I-g   INTENA = $C000 (COPER not enabled)
;   I_NOBPLEN    I-h   DMACON = $82A0 (BPLEN not set)
;   I_NOSPREN    I-i   DMACON = $8380 (SPREN not set)
;   M1_DELAY     I-j   M1 waits between samples (see m1.i)
;   I_NOTAG      I-k   PIC filled with $0000, not the tags
;   I_LATEPTR    I-l   the BPL1PT MOVEs placed after WAIT $2D01,$FF00
;   I_LATESPR    I-m   the SPR0PT MOVEs placed after WAIT $2D01,$FF00
;   I_BADFIX     I-n   FIXMARK stores $DEAD, not $BEEF

        include "nanomig.i"
        include "ch2.i"

; ---- this program's own memory layout (chip RAM, identity-mapped) ----
PIC             equ     $082000         ; 512 lines x 40 bytes = 20480 bytes
SPR             equ     $087000         ; sprite 0 structure, 16 bytes
NULLSPR         equ     $087010         ; $0000,$0000, for sprites 1-7
LISTA           equ     $080800
LISTB           equ     $080900
CUR             equ     $087220         ; word: 0 = LISTA is current, 1 = LISTB
HCNT            equ     $087222         ; word

M1_K            equ     8
M1_STARTW       equ     $087224
M1_DONEW        equ     $087226
M1_RECBASE      equ     $087230         ; M1_K x 10-byte records
FIXMARK         equ     $087400         ; the fixture's own known-value proof

        org     PROGRAM_ORG

start:
        ; Release the reset-time overlay so the interrupt vector at $6C
        ; can be written below (ch2.i: CIA-A PRA bit 0 is OVL, p339/p223;
        ; DDRA is written first, as p339 gives it).
        move.b  #$03,CIAADDRA.l
        move.b  #0,CIAAPRA.l

        clr.w   HCNT.l
        move.w  #0,CUR.l               ; CUR = LISTA (0)

        ; The fixture proves it can deliver a known value before anything
        ; else reads one.
        ifd     I_BADFIX
        move.w  #$DEAD,FIXMARK.l
        else
        move.w  #$BEEF,FIXMARK.l
        endif

        move.l  #i_handler,$6C.w

        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTREQ(a0)

        ; Build PIC: 512 lines of 20 words each, tag = $8000 OR j (I-k: all
        ; zero instead).
        lea     PIC,a1
        moveq   #0,d0                   ; d0 = line index j
.picline:
        ifd     I_NOTAG
        moveq   #0,d1
        else
        move.w  d0,d1
        or.w    #$8000,d1
        endif
        moveq   #19,d2
.picword:
        move.w  d1,(a1)+
        dbra    d2,.picword
        addq.w  #1,d0
        cmp.w   #512,d0
        blt.s   .picline

        ; Build the sprite 0 structure (section 3.1, p105/p109 layout).
        lea     SPR,a1
        move.w  #$5060,(a1)+
        move.w  #$5200,(a1)+
        move.w  #$0FF0,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0FF0,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        move.l  #$00000000,NULLSPR.l

        ; Display registers (section 3.4 step 3).
        ifd     I_NOLACE
        move.w  #$1200,BPLCON0(a0)
        else
        move.w  #$1204,BPLCON0(a0)
        endif
        move.w  #$0000,BPLCON1(a0)
        move.w  #$0000,BPLCON2(a0)
        move.w  #40,BPL1MOD(a0)
        move.w  #40,BPL2MOD(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0000,COLOR00(a0)
        move.w  #$0FFF,COLOR01(a0)
        move.w  #$0F00,COLOR17(a0)
        move.w  #$00F0,COLOR18(a0)
        move.w  #$000F,COLOR19(a0)

        ; step 4: the CPU's own initial pointer writes (p110's own technique).
        move.l  #PIC,BPL1PTH(a0)
        move.l  #SPR,SPR0PTH(a0)

        ; step 6: initialise COP1LC and strobe COPJMP1 before Copper DMA is
        ; turned on (p30).
        move.l  #LISTA,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        ; step 7.
        ifd     I_NOCOPEN
        move.w  #$8320,DMACON(a0)
        else
        ifd     I_NOBPLEN
        move.w  #$82A0,DMACON(a0)
        else
        ifd     I_NOSPREN
        move.w  #$8380,DMACON(a0)
        else
        move.w  #$83A0,DMACON(a0)
        endif
        endif
        endif

        ; step 8.
        move.w  #$0010,INTREQ(a0)
        ifd     I_NOCOPINT
        move.w  #$C000,INTENA(a0)
        else
        move.w  #$C010,INTENA(a0)
        endif

        ; step 9: accept interrupt levels 3 and up.
        move.w  #$2000,sr

        ; step 10: run M1, then stop.
        bsr     m1_run
        DONE

; ---- level-3 handler (autovector 27, address $6C; COPER is level 3, p219) ----
i_handler:
        movem.l a1,-(a7)
        lea     CUSTOM,a1
        ifd     I_NOTOGGLE
        bra.s   .skiptoggle
        endif
        tst.w   CUR.l
        bne.s   .curb
        move.l  #LISTB,COP1LCH(a1)
        move.w  #1,CUR.l
        bra.s   .skiptoggle
.curb:
        move.l  #LISTA,COP1LCH(a1)
        clr.w   CUR.l
.skiptoggle:
        addq.w  #1,HCNT.l
        move.w  #$0010,INTREQ(a1)
        movem.l (a7)+,a1
        rte

; ---- Copper lists (section 3.2) ----
        org     LISTA
        ifnd    I_NOPTRLIST
        ifnd    I_LATEPTR
        dc.w    BPL1PTH,(PIC>>16)&$FFFF
        dc.w    BPL1PTL,PIC&$FFFF
        endif
        endif
        ifnd    I_NOSPRLIST
        ifnd    I_LATESPR
        dc.w    SPR0PTH,(SPR>>16)&$FFFF
        dc.w    SPR0PTL,SPR&$FFFF
        endif
        endif
        dc.w    SPR1PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR1PTL,NULLSPR&$FFFF
        dc.w    SPR2PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR2PTL,NULLSPR&$FFFF
        dc.w    SPR3PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR3PTL,NULLSPR&$FFFF
        dc.w    SPR4PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR4PTL,NULLSPR&$FFFF
        dc.w    SPR5PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR5PTL,NULLSPR&$FFFF
        dc.w    SPR6PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR6PTL,NULLSPR&$FFFF
        dc.w    SPR7PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR7PTL,NULLSPR&$FFFF
        dc.w    $2D01,$FF00
        ifd     I_LATEPTR
        ifnd    I_NOPTRLIST
        dc.w    BPL1PTH,(PIC>>16)&$FFFF
        dc.w    BPL1PTL,PIC&$FFFF
        endif
        endif
        ifd     I_LATESPR
        ifnd    I_NOSPRLIST
        dc.w    SPR0PTH,(SPR>>16)&$FFFF
        dc.w    SPR0PTL,SPR&$FFFF
        endif
        endif
        dc.w    INTREQ,$8010
        dc.w    $FFFF,$FFFE

        org     LISTB
        ifnd    I_NOPTRLIST
        ifnd    I_LATEPTR
        ifd     I_SAMEPTR
        dc.w    BPL1PTH,(PIC>>16)&$FFFF
        dc.w    BPL1PTL,PIC&$FFFF
        else
        dc.w    BPL1PTH,((PIC+40)>>16)&$FFFF
        dc.w    BPL1PTL,(PIC+40)&$FFFF
        endif
        endif
        endif
        ifnd    I_NOSPRLIST
        ifnd    I_LATESPR
        dc.w    SPR0PTH,(SPR>>16)&$FFFF
        dc.w    SPR0PTL,SPR&$FFFF
        endif
        endif
        dc.w    SPR1PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR1PTL,NULLSPR&$FFFF
        dc.w    SPR2PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR2PTL,NULLSPR&$FFFF
        dc.w    SPR3PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR3PTL,NULLSPR&$FFFF
        dc.w    SPR4PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR4PTL,NULLSPR&$FFFF
        dc.w    SPR5PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR5PTL,NULLSPR&$FFFF
        dc.w    SPR6PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR6PTL,NULLSPR&$FFFF
        dc.w    SPR7PTH,(NULLSPR>>16)&$FFFF
        dc.w    SPR7PTL,NULLSPR&$FFFF
        dc.w    $2D01,$FF00
        ifd     I_LATEPTR
        ifnd    I_NOPTRLIST
        ifd     I_SAMEPTR
        dc.w    BPL1PTH,(PIC>>16)&$FFFF
        dc.w    BPL1PTL,PIC&$FFFF
        else
        dc.w    BPL1PTH,((PIC+40)>>16)&$FFFF
        dc.w    BPL1PTL,(PIC+40)&$FFFF
        endif
        endif
        endif
        ifd     I_LATESPR
        ifnd    I_NOSPRLIST
        dc.w    SPR0PTH,(SPR>>16)&$FFFF
        dc.w    SPR0PTL,SPR&$FFFF
        endif
        endif
        dc.w    INTREQ,$8010
        dc.w    $FFFF,$FFFE

        include "m1.i"
