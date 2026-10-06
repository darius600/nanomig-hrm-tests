; s6.i -- the program every case of this package runs (SPECIFICATION.md section 3.3).
; The test places the register list at RLIST before boot(): (register offset, value)
; word pairs in step order, terminated by an offset of $FFFF. The CPU writes each
; value to its register with one MOVE.W, in list order. Data (Copper list, sprite
; structures, bit-plane area) is part of the image the test places.
;
; Symbols a program sets before the include:
;   NOFIX   leave out the fixture store
;   NODONE  leave out step 6's write of DONE_VALUE (idle loop instead)
;   VBL     after the DONE write, poll VERTB and toggle COLOR00 in every vertical blank
;   PRO     a prologue at PROGRAM_ORG enables Copper DMA on a one-instruction list at
;           $088100 and jumps to the entry, which starts at PROGRAM_ORG+$40

        include "nanomig.i"

RLIST           equ     $0B8000
LISAID          equ     $07C
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088

        org     PROGRAM_ORG

        ifd     PRO
        lea     CUSTOM,a0
        move.w  #$0008,COP1LCH(a0)
        move.w  #$8100,COP1LCL(a0)
        move.w  d0,COPJMP1(a0)
        move.w  #$8280,DMACON(a0)
        jmp     entry
        ds.w    16,0
        org     PROGRAM_ORG+$40
        endc

entry:
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS          ; the fixture: a known constant, stored first
        endc
        lea     CUSTOM,a0
        lea     RLIST,a1
.next:  move.w  (a1)+,d0
        cmp.w   #$FFFF,d0
        beq.s   .listend
        move.w  (a1)+,d1
        move.w  d1,0(a0,d0.w)
        bra.s   .next
.listend:
        move.w  LISAID(a0),RESULTS+4    ; the chip set's identity, read once the list is done
        ifd     NODONE
.hang:  bra.s   .hang
        else
        ifd     VBL
        move.w  #DONE_VALUE,DONE_ADDR
        move.w  #$000A,d3               ; colour(0)
.vb:    move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .vb
        move.w  #$0020,INTREQ(a0)
        eori.w  #$0020,d3               ; colour(0) <-> colour(2)
        move.w  d3,COLOR00(a0)
        bra.s   .vb
        else
        DONE
        endc
        endc
