; prog_jump.s -- run JUMP (SPECIFICATION.md P10).
;
; Wrapper sources set one mutation symbol and include this file:
;
;   (no symbol) -- the normal run
;   MUT_P10PROP -- proposition contrast: field 4 writes COLOR01 (not a
;                  strobe) instead of COPJMP2
;   MUT_P10PRE1 -- precondition 1 contrast: DMACON without COPEN
;   MUT_P10PRE2 -- precondition 2 contrast: omit field 2's marker write
;   MUT_P10PRE3 -- precondition 3 contrast: wait until V7-0=$F4 instead of
;                  $80 before every marker
;   MUT_FIXTURE -- the fixture check's own contrast: $DEAD instead of $BEEF

        include "nanomig.i"
        include "sprreg.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL

        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)

        ; Copper lists (SPECIFICATION.md P10 step 1). The CPU fills them
        ; itself, below $80000 (3.2's anchor check).
        move.l  #COPLIST_L1,a1
        move.w  #$F001,(a1)+           ; WAIT line $F0
        move.w  #$FFFE,(a1)+
        move.w  #COLOR00,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+

        move.l  #COPLIST_L2,a1
        move.w  #COLOR00,(a1)+
        move.w  #$0F00,(a1)+
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+

        move.l  #COPLIST_L1,COP1LCH(a0)
        move.l  #COPLIST_L2,COP2LCH(a0)
        move.w  #0,COPJMP1(a0)

        ifd     MUT_P10PRE1
        move.w  #$8200,DMACON(a0)      ; P10 precondition 1 contrast: no COPEN
        else
        move.w  #$8280,DMACON(a0)      ; SET, DMAEN, COPEN
        endif

        lea     MARK,a2
        moveq   #5,d7                          ; fields 1..6, d7=f-1
.fields:
        bsr     wait_vertb

        ifd     MUT_P10PRE3
        move.w  #$00F4,d1              ; P10 precondition 3 contrast
        else
        move.w  #$0080,d1
        endif
        bsr     wait_v_ge

        move.w  d7,d6
        addq.w  #1,d6                          ; d6 = field number f

        cmp.w   #2,d6
        bne.s   .writemark
        ifd     MUT_P10PRE2
        bra.s   .nomark                        ; P10 precondition 2 contrast: omit M_2
        endif
.writemark:
        move.w  #1,(a2)
.nomark:
        addq.l  #2,a2

        btst    #0,d6
        bne.s   .oddfield                      ; fields 1,3,5: nothing further
        cmp.w   #2,d6
        bne.s   .notf2
        move.w  #$0000,COPJMP2(a0)
        bra.s   .oddfield
.notf2:
        cmp.w   #4,d6
        bne.s   .notf4
        ifd     MUT_P10PROP
        move.w  #$FFFF,COLOR01(a0)             ; P10 proposition contrast
        else
        move.w  #$FFFF,COPJMP2(a0)
        endif
        bra.s   .oddfield
.notf4:
        move.w  #$5A5A,COPJMP2(a0)              ; field 6
.oddfield:
        dbra    d7,.fields

        ifd     MUT_FIXTURE
        move.w  #$DEAD,RESULTS+0
        else
        move.w  #$BEEF,RESULTS+0
        endif
        DONE

wait_vertb:
        move.w  INTREQR(a0),d0
        and.w   #$0020,d0
        beq.s   wait_vertb
        move.w  #$0020,INTREQ(a0)
        rts

wait_v_ge:
        move.w  VPOSR(a0),d0
        and.w   #1,d0
        lsl.w   #8,d0
        move.w  VHPOSR(a0),d2
        lsr.w   #8,d2
        or.w    d2,d0
        cmp.w   d1,d0
        blt.s   wait_v_ge
        rts
