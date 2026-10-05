; prog_p8_cop_base.s -- P8 COP case: the 680x0 can poll the Copper's
; INTREQ flag (SPECIFICATION.md P8). Included by the case files below,
; each setting one symbol first (default = main).
;
;   (no symbol)     -- main
;   P8_COP_NOMOVE   -- device contrast: the MOVE replaced by $0180,$0F00
;                       (COLOR00). Bit 4 stays 0
;   P8_COP_MUT_C    -- contrast for (c): DMACON=$8200 (no COPEN), no IR2
;                       fetch occurs
;   P8_COP_MUT_A    -- precondition (a) mutation: INTENA=$C020 with a
;                       level-3 handler that clears bit 5; INTENAR is then
;                       non-zero
;   P8_COP_MUT_B    -- precondition (b) mutation: WAIT V=$20 and no clear,
;                       so bit 4 already reads 1 at the first record stored
;                       after MC
;   P8_COP_MUT_III  -- check (iii) contrast: once a poll record reads bit 4
;                       = 1, INTREQ=$0010 is written again (once); a later
;                       record then reads 0
;   P8_COP_MUT_D    -- population mutation: a delay of about 130 lines after
;                       MC, so no poll record is stored before the Copper's
;                       MOVE has executed

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL_DDR
        lea     CUSTOM,a0
        CLEAR_DMA_INT                    ; common prologue: INTENA cleared, polling only
        ifd     P8_COP_MUT_A
        ; precondition (a) mutation: a level-3 handler that clears bit 5,
        ; with INTENA's master enable and bit 5 both set -- INTENAR is then
        ; non-zero.
        move.l  #p8_h3,$6C.w
        move.w  #$C020,INTENA(a0)
        move.w  #$2000,sr                 ; accept all interrupt levels
        endif
        move.w  INTENAR(a0),RESULTS+4    ; precondition (a): INTENAR bits 14-0 = 0

        ; List: $8001,$FF00 (WAIT V=$80, H ignored) ; $009C,$8010 (MOVE
        ; INTREQ, sets bit 4) ; $FFFF,$FFFE.
        ifd     P8_COP_MUT_B
        move.l  #$2001FF00,LIST1         ; precondition (b) mutation: WAIT
                                          ; V=$20, so the MOVE executes before
                                          ; the poll starts
        else
        move.l  #$8001FF00,LIST1
        endif
        ifd     P8_COP_NOMOVE
        move.l  #$01800F00,LIST1+4       ; device contrast: COLOR00, not INTREQ
        else
        move.l  #$009C8010,LIST1+4
        endif
        move.l  #$FFFFFFFE,LIST1+8

        ; Start per 2.8.
        move.l  #LIST1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        ifd     P8_COP_MUT_C
        move.w  #$8200,DMACON(a0)        ; contrast for (c): COPEN clear
        else
        move.w  #$8280,DMACON(a0)
        endif
        move.w  #0,COPJMP1(a0)           ; strobe again (2.8 step 3)
        move.w  #3,MARKBUF+4              ; MJ

        lea     RECBUF,a6
        moveq   #0,d7

        ; When a record shows V between $40 and $60, write INTREQ=$0010; MC.
.w1:    SAMPLE
        addq.l  #1,d7
        move.l  -6(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        cmpi.w  #$40,d1
        blt.s   .w1
        cmpi.w  #$60,d1
        bgt.s   .w1

        ifnd    P8_COP_MUT_B
        move.w  #$0010,INTREQ(a0)
        endif
        move.w  #7,MARKBUF+6              ; MC

        ifd     P8_COP_MUT_D
        move.w  #6000-1,d6                ; population mutation: no record yet
.dl:    dbra    d6,.dl
        endif

        ; Poll until the end of the frame: a generous fixed span.
        ifd     P8_COP_MUT_III
        moveq   #0,d6                    ; 0 until the re-clear below has fired
        move.w  #3000-1,d3
.pd:    SAMPLE
        addq.l  #1,d7
        move.w  -8(a6),d0                ; the record's own INTREQR word
        btst    #4,d0
        beq.s   .pd_next
        tst.w   d6
        bne.s   .pd_next
        move.w  #$0010,INTREQ(a0)        ; check (iii) contrast: clear again
        moveq   #1,d6
.pd_next:
        dbra    d3,.pd
        else
        move.w  #3000-1,d3
.pd:    SAMPLE
        addq.l  #1,d7
        dbra    d3,.pd
        endif

        move.l  d7,RESULTS+8

        DONE

        ifd     P8_COP_MUT_A
; ---- level-3 handler (autovector 27, address $6C): clears bit 5. --------
p8_h3:  movem.l a1,-(a7)
        lea     CUSTOM,a1
        move.w  #$0020,INTREQ(a1)
        movem.l (a7)+,a1
        rte
        endif
