; prog_p4_base.s -- P4: loading a location register and writing its strobe
; before enabling gives a known start address (SPECIFICATION.md P4).
; Included by the run/mutation variants below, each setting symbols first.
;
;   (no symbol)         -- Run 1 (COP1): COP2LC=B, COP1LC=A, COPJMP1
;   P4_RUN2             -- Run 2 (COP2): COP1LC=A, COP2LC=B, COPJMP2
;   P4_RUN2+P4_RUN3      -- Run 3: ...COPJMP2, COP2LC=C (no strobe)
;   P4_RUN2+P4_RUN3+P4_RUN3_REORDER -- Run 3 with COP2LC=C moved before
;                           COPJMP2 (graded normally: its own "Main" case)
;   P4_MUT_A             -- precondition (a) mutation: DMACON=$8280 written
;                           before the DMACONR read
;   P4_MUT_B             -- precondition (b) mutation: the whole Run 1
;                           sequence, DMACON write included, run once early
;   P4_MUT_C             -- precondition (c) mutation: wait for a frame
;                           wrap between M1 and the DMACON write

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL
        lea     CUSTOM,a0

        ifd     P4_MUT_A
        move.w  #$8280,DMACON(a0)       ; precondition (a) mutation
        endif

        move.w  DMACONR(a0),RESULTS+4   ; the program's very first action

        ; Lists A, B, C: $0180,x ; $FFFF,$FFFE, a different x each.
        move.l  #$018000AA,LIST1        ; A
        move.l  #$FFFFFFFE,LIST1+4
        move.l  #$018000BB,LIST2        ; B
        move.l  #$FFFFFFFE,LIST2+4
        move.l  #$018000CC,LIST3        ; C
        move.l  #$FFFFFFFE,LIST3+4

        ifd     P4_MUT_B
        ; precondition (b) mutation: the whole Run 1 sequence, DMACON
        ; write included, once before the real run below.
        move.l  #LIST2,COP2LCH(a0)
        move.l  #LIST1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  #$8280,DMACON(a0)
        endif

        ifd     P4_RUN2
        ifd     P4_RUN3
        ; Run 3 (strobe, then rewrite): COP1LC=A, COP2LC=B, COPJMP2,
        ; COP2LC=C (no strobe) -- or, reordered, COP2LC=C before COPJMP2.
        move.l  #LIST1,COP1LCH(a0)
        move.l  #LIST2,COP2LCH(a0)
        ifd     P4_RUN3_REORDER
        move.l  #LIST3,COP2LCH(a0)
        move.w  #0,COPJMP2(a0)
        else
        move.w  #0,COPJMP2(a0)
        move.l  #LIST3,COP2LCH(a0)
        endif
        else
        ; Run 2 (COP2): COP1LC=A, COP2LC=B, COPJMP2
        move.l  #LIST1,COP1LCH(a0)
        move.l  #LIST2,COP2LCH(a0)
        move.w  #0,COPJMP2(a0)
        endif
        else
        ; Run 1 (COP1): COP2LC=B, COP1LC=A, COPJMP1
        move.l  #LIST2,COP2LCH(a0)
        move.l  #LIST1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        endif

        move.w  #9,MARKBUF+4             ; M0: bounds the run-specific writes,
                                          ; diagnostic only (2.3's marker
                                          ; technique; not a page value)

        lea     RECBUF,a6
        moveq   #0,d7                   ; running record count

        ; sampler until a record shows V between $40 and $C0
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

        move.w  #1,MARKBUF              ; M1

        ifd     P4_MUT_C
        ; precondition (c) mutation: wait for a frame wrap before DMACON
        move.l  -4(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        move.w  d1,d4
.wwrap: SAMPLE
        addq.l  #1,d7
        move.l  -4(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        cmp.w   d4,d1
        bge.s   .wwrap
        endif

        move.w  #$8280,DMACON(a0)
        move.w  #2,MARKBUF+2            ; M2

        ; sampler until (well past) the end of the next complete frame
        move.w  #20000-1,d3
.pd:    SAMPLE
        addq.l  #1,d7
        dbra    d3,.pd

        move.l  d7,RESULTS+8             ; record count, for the C++ side

        DONE
