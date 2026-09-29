; prog_p10_base.s -- P10: INTREQ bit 15 decides whether bits written as 1
; are set or cleared (SPECIFICATION.md P10). Included by the case files
; below, each setting one symbol first (default = main run).
;
;   (no symbol)   -- main run
;   P10_MAIN2     -- Main contrast: 2nd MOVE data $0010 -> $8010, 3rd MOVE
;                     data $0004 -> $8004
;   P10_MUT_A     -- precondition (a) mutation: 2nd WAIT $5001 -> $5A01
;   P10_MUT_B     -- precondition (b) mutation: step 2 writes INTREQ=$8004
;   P10_MUT_C     -- precondition (c) mutation: INTENA=$C004 with a level-1
;                     handler that clears bit 2 (SOFT is level 1, p.280)

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL_DDR
        lea     CUSTOM,a0
        CLEAR_DMA_INT

        ifd     P10_MUT_C
        ; precondition (c) mutation: INTENA=$C004 (SOFT enabled, master
        ; enable set) with a level-1 handler that clears bit 2, installed
        ; and enabled before the INTENAR read below, so it reads non-zero.
        move.l  #p10_h1,$64.w
        move.w  #$C004,INTENA(a0)
        move.w  #$2000,sr                      ; accept all interrupt levels
        endif

        ; step 2.
        ifd     P10_MUT_B
        move.w  #$8004,INTREQ(a0)        ; precondition (b) mutation
        else
        move.w  #$0014,INTREQ(a0)
        endif
        move.w  INTENAR(a0),RESULTS+4     ; precondition (c): INTENAR bits 14-0

        ; step 3: list R at LIST1.
        move.l  #$4001FF00,LIST1               ; WAIT $4001,$FF00
        move.l  #$009C8014,LIST1+4             ; MOVE INTREQ,$8014
        ifd     P10_MUT_A
        move.l  #$5A01FF00,LIST1+8             ; precondition (a) mutation
        else
        move.l  #$5001FF00,LIST1+8             ; WAIT $5001,$FF00
        endif
        ifd     P10_MAIN2
        move.l  #$009C8010,LIST1+12            ; Main contrast: $0010 -> $8010
        else
        move.l  #$009C0010,LIST1+12            ; MOVE INTREQ,$0010
        endif
        move.l  #$6001FF00,LIST1+16            ; WAIT $6001,$FF00
        ifd     P10_MAIN2
        move.l  #$009C8004,LIST1+20            ; Main contrast: $0004 -> $8004
        else
        move.l  #$009C0004,LIST1+20            ; MOVE INTREQ,$0004
        endif
        move.l  #$7001FF00,LIST1+24            ; WAIT $7001,$FF00
        move.l  #$009C8010,LIST1+28            ; MOVE INTREQ,$8010
        move.l  #$FFFFFFFE,LIST1+32            ; END

        ; step 4: start per 2.8 while a record shows V < $30.
        lea     RECBUF,a6
        moveq   #0,d7
.w1:    SAMPLE
        addq.l  #1,d7
        move.l  -6(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        cmpi.w  #$30,d1
        blt.s   .started
        bra.s   .w1
.started:
        move.l  #LIST1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  #$8280,DMACON(a0)
        move.w  #0,COPJMP1(a0)             ; strobe again (2.8 step 3)
        move.w  #3,MARKBUF+4                ; MJ

        ; step 5: polling sampler (INTREQR and position); a generous fixed
        ; span covering lines $38 through $78 and well beyond.
        move.w  #3000-1,d3
.pd:    SAMPLE
        addq.l  #1,d7
        dbra    d3,.pd

        move.l  d7,RESULTS+8

        DONE

        ifd     P10_MUT_C
; ---- level-1 handler (autovector 25, address $64): clears bit 2 (SOFT). --
p10_h1: movem.l a1,-(a7)
        lea     CUSTOM,a1
        move.w  #$0004,INTREQ(a1)
        movem.l (a7)+,a1
        rte
        endif
