; prog_p4.s -- P4 (ECS, PAL): LPENDIS's own effect on a vertical-position
; read. See SPECIFICATION.md, "P4 (ECS)". Reuses P3's own Copper list
; (WAIT/MOVE table) verbatim through E210, then gains one more WAIT/MOVE
; pair for E250 (line 250) before the end, exactly as specified.
;
; The CPU's own "waits about 20 lines" after E250 (SPECIFICATION.md's own
; words) is built as a fixed instruction-count spin (P4_SPIN below), not a
; poll of VPOSR/VHPOSR: a Copper WAIT cannot target line 250+20=270 at all
; (its own V field is 8 bits, p.24's own ambiguity #11 -- ovs. line 255),
; and polling the very register the field 4 grades would be circular on an
; implementation that actually freezes it (unlike this one). P4_SPIN is
; tuned on this RTL; the reference-frame bound in SPECIFICATION.md (X's
; store at least 156*228 colour clocks after t(A1), Y's at least 100*228
; before the next field's own t(A1)) is the precondition that guards it,
; checked by test_ch7_system_control.cpp.
;
;   NOLPENDIS  -- the page's own contrasting case: F1's own E60 write is
;                 $0020 instead of $2020 (LPENDIS never set).
;   F3LPENDIS  -- precondition 3's own contrast: F3's own E60 write is also
;                 $2020 instead of $0020 (LPENDIS set in the control field too).

        include "nanomig.i"

BEAMCON0        equ     $1DC
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
BPLCON0         equ     $100

CIAADDRA        equ     $BFE201

COP_LIST        equ     $0C1000         ; 92 bytes (P3's list plus E250)
MAINTICK        equ     $0C1100         ; 6 words: fields F-1,F0,F1,F2,F3 (indices 0-4) plus one more
RECORDS         equ     $0C2000         ; X,Y,X',Y': 4*3 = 12 words

P4_SPIN         equ     2000            ; see the header note

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$7FFF,DMACON(a0)

        ; CIA-A DDRA fix, as in prog_lp.s.
        move.b  CIAADDRA.l,d0
        moveq   #0,d2
        move.b  d0,d2
        move.w  d2,RESULTS.l
        move.b  d0,d1
        and.b   #$C0,d1
        beq.s   .ddra_done
        and.b   #$3F,d0
        move.b  d0,CIAADDRA.l
.ddra_done:

        move.w  #$0020,BEAMCON0(a0)     ; ECS, PAL, LPENDIS=0
        move.w  #$0008,BPLCON0(a0)      ; LPEN=1
        move.w  #$0000,COLOR00(a0)

        ; ---- Copper list: P3's own WAIT/MOVE table through E210, then E250.
        move.w  #$2801,COP_LIST.l
        move.w  #$FFFE,COP_LIST+2.l
        move.w  #INTREQ,COP_LIST+4.l
        move.w  #$8010,COP_LIST+6.l

        move.w  #$3C01,COP_LIST+8.l
        move.w  #$FFFE,COP_LIST+10.l
        move.w  #INTREQ,COP_LIST+12.l
        move.w  #$8010,COP_LIST+14.l

        move.w  #$6441,COP_LIST+16.l
        move.w  #$FFFE,COP_LIST+18.l
        move.w  #COLOR00,COP_LIST+20.l
        move.w  #$0F00,COP_LIST+22.l

        move.w  #$64A1,COP_LIST+24.l
        move.w  #$FFFE,COP_LIST+26.l
        move.w  #COLOR00,COP_LIST+28.l
        move.w  #$0000,COP_LIST+30.l

        move.w  #$6E01,COP_LIST+32.l
        move.w  #$FFFE,COP_LIST+34.l
        move.w  #INTREQ,COP_LIST+36.l
        move.w  #$8010,COP_LIST+38.l

        move.w  #$8201,COP_LIST+40.l
        move.w  #$FFFE,COP_LIST+42.l
        move.w  #INTREQ,COP_LIST+44.l
        move.w  #$8010,COP_LIST+46.l

        move.w  #$B441,COP_LIST+48.l
        move.w  #$FFFE,COP_LIST+50.l
        move.w  #COLOR00,COP_LIST+52.l
        move.w  #$000F,COP_LIST+54.l

        move.w  #$B4A1,COP_LIST+56.l
        move.w  #$FFFE,COP_LIST+58.l
        move.w  #COLOR00,COP_LIST+60.l
        move.w  #$0000,COP_LIST+62.l

        move.w  #$BE01,COP_LIST+64.l
        move.w  #$FFFE,COP_LIST+66.l
        move.w  #INTREQ,COP_LIST+68.l
        move.w  #$8010,COP_LIST+70.l

        move.w  #$D201,COP_LIST+72.l
        move.w  #$FFFE,COP_LIST+74.l
        move.w  #INTREQ,COP_LIST+76.l
        move.w  #$8010,COP_LIST+78.l

        move.w  #$FA01,COP_LIST+80.l    ; WAIT line 250
        move.w  #$FFFE,COP_LIST+82.l
        move.w  #INTREQ,COP_LIST+84.l   ; E250
        move.w  #$8010,COP_LIST+86.l

        move.w  #$FFFF,COP_LIST+88.l    ; end
        move.w  #$FFFE,COP_LIST+90.l

        move.l  #COP_LIST,d3
        move.w  d3,COP1LCL(a0)
        swap    d3
        move.w  d3,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        move.w  #$8280,DMACON(a0)

        ; ---- Event loop. d0 = field index (0=F-1,1=F0,2=F1,3=F2,4=F3),
        ; d1 = per-field COPER counter (0=E40,1=E60,2=E110,3=E130,4=E190,
        ; 5=E210,6=E250). a4 = MAINTICK pointer, a5 = RECORDS pointer.
        lea     MAINTICK,a4
        lea     RECORDS,a5
        moveq   #-1,d0
main_loop:
        move.w  INTREQR(a0),d7
        btst    #5,d7
        beq.s   .check_coper
        move.w  #$0020,INTREQ(a0)       ; clear VERTB
        addq.w  #1,d0
        moveq   #0,d1
        move.w  d0,(a4)+
        bra     main_loop
.check_coper:
        btst    #4,d7
        beq.s   main_loop
        move.w  #$0010,INTREQ(a0)       ; clear COPER
        cmp.w   #1,d1                    ; E60?
        bne.s   .not_e60
        cmp.w   #2,d0                    ; F1?
        bne.s   .maybe_f3_e60
        ifd     NOLPENDIS
        move.w  #$0020,BEAMCON0(a0)      ; contrast: LPENDIS never set
        else
        move.w  #$2020,BEAMCON0(a0)      ; F1: LPENDIS=1
        endif
        bra     .coper_done
.maybe_f3_e60:
        cmp.w   #4,d0                    ; F3?
        bne.s   .coper_done
        ifd     F3LPENDIS
        move.w  #$2020,BEAMCON0(a0)      ; contrast: LPENDIS set in the control field too
        else
        move.w  #$0020,BEAMCON0(a0)      ; F3 control: re-affirm LPENDIS=0
        endif
        bra     .coper_done
.not_e60:
        cmp.w   #6,d1                    ; E250?
        bne.s   .coper_done
        cmp.w   #2,d0
        beq     .handle_e250_f1
        cmp.w   #4,d0
        beq     .handle_e250_f3
        bra     .coper_done
.coper_done:
        addq.w  #1,d1
        bra     main_loop

.handle_e250_f1:
        move.w  #P4_SPIN-1,d5
.spin1: dbra    d5,.spin1
        bsr     p4_store                 ; X
        move.w  #$0020,BEAMCON0(a0)
        bsr     p4_store                 ; Y
        addq.w  #1,d1
        bra     main_loop

.handle_e250_f3:
        move.w  #P4_SPIN-1,d5
.spin3: dbra    d5,.spin3
        bsr     p4_store                 ; X'
        move.w  #$0020,BEAMCON0(a0)
        bsr     p4_store                 ; Y'
        cmp.w   #4,d0
        beq     all_done                 ; stop right after F3's own Y'
        addq.w  #1,d1
        bra     main_loop

all_done:
        DONE

; ---- p4_store: stores one 3-word reading (VPOSR from long read, VHPOSR
; from the long read, VHPOSR from a separate word read) at (a5)+.
p4_store:
        move.l  VPOSR(a0),d2
        move.w  VHPOSR(a0),d3
        move.w  d2,d6
        swap    d2
        move.w  d2,(a5)+
        move.w  d6,(a5)+
        move.w  d3,(a5)+
        rts
