; prog_t.s -- Program T (SPECIFICATION.md section 4): line timing at the
; field boundaries, one fixed Copper list (LISTT) run every field. Serves
; P2 and P3. NTSC/PAL is chosen by the harness's chipset_config, not by
; this program (section 4.3 step 1); the test driver selects it per
; variant.
;
; Variants (section 4.4) assemble this same file with one symbol set:
;   T_NOLACE       T-a  BPLCON0 = $1200 (LACE = 0)
;   (PAL is chosen by the test driver's Config, not a symbol here)  T-b
;   T_WAIT0_ZERO   T-c  the first WAIT is (0,$00), not (0,$E0)
;   T_LINE100_C0   T-d  the mark of line 100 uses (100,$C0), not (100,$E0)
;   T_DIWSTOP_NORM T-e  DIWSTOP = $F4C1 (the normal NTSC value, p59)
;   M1_DELAY       T-f  M1 waits between samples (see m1.i)
;   T_SKIP10       T-g  the WAIT and MOVE of line 10 are deleted
;   T_NOCOPEN      T-h  DMACON = $8300 (COPEN not set)
;   T_BADFIX       T-i  FIXMARK stores $DEAD, not $BEEF

        include "nanomig.i"
        include "ch2.i"

; ---- this program's own memory layout (chip RAM, identity-mapped) ----
PLANE           equ     $082000         ; 11,000 bytes of $0000
LISTT           equ     $080800

M1_K            equ     13
M1_STARTW       equ     $086000
M1_DONEW        equ     $086002
M1_RECBASE      equ     $086010         ; M1_K x 10-byte records
FIXMARK         equ     $086100         ; the fixture's own known-value proof

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTREQ(a0)

        ; The fixture proves it can deliver a known value before anything
        ; else reads one.
        ifd     T_BADFIX
        move.w  #$DEAD,FIXMARK.l
        else
        move.w  #$BEEF,FIXMARK.l
        endif

        ; PLANE: 11,000 bytes of $0000 (section 4.1).
        lea     PLANE,a1
        move.w  #(11000/2)-1,d0
.clrplane:
        move.w  #0,(a1)+
        dbra    d0,.clrplane

        ; Display registers (section 4.3 step 3).
        ifd     T_NOLACE
        move.w  #$1200,BPLCON0(a0)
        else
        move.w  #$1204,BPLCON0(a0)
        endif
        move.w  #$0000,BPLCON1(a0)
        move.w  #$0000,BPLCON2(a0)
        move.w  #0,BPL1MOD(a0)
        move.w  #0,BPL2MOD(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #$2C81,DIWSTRT(a0)
        ifd     T_DIWSTOP_NORM
        move.w  #$F4C1,DIWSTOP(a0)
        else
        move.w  #$39C1,DIWSTOP(a0)
        endif

        ; step 4.
        move.l  #LISTT,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        ; step 5.
        ifd     T_NOCOPEN
        move.w  #$8300,DMACON(a0)
        else
        move.w  #$8380,DMACON(a0)
        endif

        ; step 6: interrupts stay disabled (reset SR already masks them).

        ; step 7: run M1, then stop.
        bsr     m1_run
        DONE

; ---- Copper list LISTT (section 4.2) ----
        org     LISTT
        include "listt_body.i"
        dc.w    $FFFF,$FFFE

        include "m1.i"
