; prog_p1_base.s -- P1: DMACON bit 7 (COPEN) enables Copper DMA
; (SPECIFICATION.md P1). Included by the base/contrast/mutation variants
; below, each setting one symbol first.
;
;   (no symbol)       -- the main case: DMACON = $8280 (bit 7 set)
;   P1_BIT7_OFF       -- the contrasting case graded in the normal run:
;                        DMACON = $8200 (bit 7 clear)
;   P1_MUT_EARLY_ON   -- precondition (a) mutation: DMACON = $8280 written
;                        before COP1LC/COPJMP1 (before step 3)
;   P1_MUT_NO_DMAEN   -- precondition (b) mutation: DMACON = $8080 (COPEN
;                        without DMAEN) instead of $8280 at step 5
;   P1_MUT_SLOWSAMP   -- precondition (c) mutation: a delay inserted between
;                        samples (2.4's own contrasting case for B2/B3)

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL
        lea     CUSTOM,a0
        CLEAR_DMA_INT

        ; List L1 at LIST1: $0180,$0F00 (MOVE COLOR00) ; $FFFF,$FFFE
        move.l  #$01800F00,LIST1
        move.l  #$FFFFFFFE,LIST1+4

        ifd     P1_MUT_EARLY_ON
        move.w  #$8280,DMACON(a0)      ; precondition (a) mutation: enabled
                                        ; before COP1LC/COPJMP1 are written
        endif

        ; step 3 (p.30): write COP1LC = LIST1, then write COPJMP1
        move.l  #LIST1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        lea     RECBUF,a6
        moveq   #0,d7                  ; running record count

        ; step 4: sampler until a record shows V between $40 and $C0
.w1:    FAST_SAMPLE
        addq.l  #1,d7
        move.l  -6(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1               ; d1 = V
        cmpi.w  #$40,d1
        blt.s   .w1
        cmpi.w  #$C0,d1
        bgt.s   .w1

        ; step 5: M1; DMACON = ...; M2
        move.w  #1,MARKBUF             ; M1

        ifd     P1_BIT7_OFF
        move.w  #$8200,DMACON(a0)      ; contrasting case: bit 7 clear
        else
        ifd     P1_MUT_NO_DMAEN
        move.w  #$8080,DMACON(a0)      ; precondition (b) mutation
        else
        move.w  #$8280,DMACON(a0)      ; the main case
        endif
        endif

        move.w  #2,MARKBUF+2           ; M2

        move.w  DMACONR(a0),RESULTS+4   ; step 6: read DMACONR and store it

        ; sampler for a fixed number of
        ; iterations, generous enough to span several complete frames (the
        ; C++ side finds "two complete frames after M2" itself from the
        ; wraps in the captured data -- SPECIFICATION.md 2.4). A plain
        ; `dbra` loop around FAST_SAMPLE, with no per-iteration V decode,
        ; keeps density at or above the anchor B2 (>= 7 records/line,
        ; SPECIFICATION.md 2.4); the main case unrolls eight FAST_SAMPLEs
        ; per iteration, sharing the addq/dbra overhead across them.
        ifd     P1_MUT_SLOWSAMP
        move.w  #4000-1,d3             ; precondition (c) mutation: fewer
                                        ; iterations, each slowed further
.w2:    FAST_SAMPLE
        addq.l  #1,d7
        SLOWDELAY
        dbra    d3,.w2
        else
        move.w  #3000-1,d3
.w2:    FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        addq.l  #8,d7
        dbra    d3,.w2
        endif

        move.l  d7,RESULTS+8           ; record count, for the C++ side

        DONE
