; prog_p2_base.s -- P2: Clearing COPEN stops instruction fetching and data
; placement (SPECIFICATION.md P2). Included by the base/contrast/mutation
; variants below, each setting one symbol first.
;
;   (no symbol)     -- the main case: phase 2 writes DMACON = $0080 (clears
;                      bit 7, COPEN)
;   P2_MAIN_OFF     -- the contrasting case graded in the normal run: phase
;                      2 writes DMACON = $0000 (no bit selected -- a no-op)
;   P2_MUT_A        -- precondition (a) mutation: L2's MOVE replaced by
;                      $0180,$0F00 (COLOR00, never touches INTREQ)
;   P2_MUT_B        -- precondition (b) mutation: INTENA = $C010 with a
;                      level-3 handler installed that clears bit 4
;   P2_MUT_SLOWSAMP -- precondition (c) mutation: a delay inserted between
;                      phase-2 samples

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL

        ifd     P2_MUT_B
        move.l  #p2_handler3,$6C        ; autovector 3 (COPER, p.219/p.280)
        endif

        lea     CUSTOM,a0
        CLEAR_DMA_INT

        ; List L2 at LIST1: places data once per frame, right after the
        ; Copper restarts.
        ifd     P2_MUT_A
        move.l  #$01800F00,LIST1        ; precondition (a) mutation: COLOR00
        else
        move.l  #$009C8010,LIST1        ; MOVE INTREQ,#$8010 (sets bit 4)
        endif
        move.l  #$FFFFFFFE,LIST1+4

        ; Start per 2.7
        move.l  #LIST1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  #$8280,DMACON(a0)

        ifd     P2_MUT_B
        move.w  #$C010,INTENA(a0)       ; precondition (b) mutation
        move.w  #$2000,sr               ; lower the 680x0 mask to accept it
        endif

        lea     RECBUF,a6
        moveq   #0,d7                   ; running record count

        ; Phase 1: wait for a record showing V between $40 and $C0
.w1:    SAMPLE
        addq.l  #1,d7
        move.l  -4(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        cmpi.w  #$40,d1
        blt.s   .w1
        cmpi.w  #$C0,d1
        bgt.s   .w1

        move.w  #$0010,INTREQ(a0)       ; clear bit 4 (p.218)

        ; Polling sampler until one complete frame has passed: a fixed,
        ; generous iteration count (real INTREQR each time -- observation
        ; (i)/precondition (a) need it).
        move.w  #3000-1,d3
.pb:    SAMPLE
        addq.l  #1,d7
        dbra    d3,.pb

        move.w  INTENAR(a0),RESULTS+4   ; precondition (b): read/store before phase 2

        ; Phase 2: wait for a record showing V between $80 and $C0
.w2:    SAMPLE
        addq.l  #1,d7
        move.l  -4(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        cmpi.w  #$80,d1
        blt.s   .w2
        cmpi.w  #$C0,d1
        bgt.s   .w2

        move.w  #1,MARKBUF              ; M1
        ifd     P2_MAIN_OFF
        move.w  #$0000,DMACON(a0)       ; contrasting case: no bit selected
        else
        move.w  #$0080,DMACON(a0)       ; the main case: clear bit 7 (COPEN)
        endif
        move.w  #2,MARKBUF+2            ; M2

        move.w  #$0010,INTREQ(a0)       ; clear bit 4 again

        ifd     P2_MUT_SLOWSAMP
        move.w  #4000-1,d3
        else
        move.w  #20000-1,d3
        endif
.pd:    SAMPLE
        addq.l  #1,d7
        ifd     P2_MUT_SLOWSAMP
        SLOWDELAY
        endif
        dbra    d3,.pd

        move.l  d7,RESULTS+8            ; record count, for the C++ side

        DONE

        ifd     P2_MUT_B
p2_handler3:
        movem.l d0/a0,-(sp)
        lea     CUSTOM,a0
        move.w  #$0010,INTREQ(a0)
        movem.l (sp)+,d0/a0
        rte
        endif
