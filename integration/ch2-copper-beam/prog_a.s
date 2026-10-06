; prog_a.s -- Run A: beam trace and line markers (SPECIFICATION.md 5, Run A).
; List A is written by the CPU, one word per store, so every word appears on
; the Chip-memory port (precondition L1). The Copper is then started and the
; CPU repeats reading the long word at $DFF004 (VPOSR,VHPOSR) into the trace
; buffer (RF2) until SAMPLE_COUNT samples are taken.
;
; Symbols this source answers to (a variant wrapper sets one before
; including this file):
;   RUN_NTSC     -- NTSC tail loop (N=6) instead of PAL (N=$37).
;   RUN_LACE     -- A-LACE: BPLCON0 = $0204 (LACE set) instead of $0200.
;   RUN_NOPREP   -- A-noprep: the $E2 WAIT/MARK and the 16 padding MARKs
;                    are left out.
;   RUN_SLOW     -- A-slow: a delay between trace samples.
;   RUN_NOCOP    -- A-noCop: DMACON = $0080 (COPEN cleared) is written by
;                    the CPU after the third field boundary the trace loop
;                    itself observes (V decreasing between samples).
;   RUN_ANCHOR   -- A-anchor: the main loop's WAIT target is v+1 instead of
;                    v (v running $30..$FE), while MARK(v) keeps v.
;   RUN_SHIFT    -- A-shift: WAIT($80,$42) in place of WAIT($80,$40).
;   RUN_HOLD     -- A-hold: one read of $DFF004 after polling H >= $20,
;                    the same long word stored every step.
;   RUN_P2WAITED -- an extra WAIT($7F,$40) inserted right after MARK($7F):
;                    it is already satisfied when fetched (P2's "waited"
;                    precondition mutation).
;   RUN_NODONE   -- P1's completion-word contrasting case: the program
;                    starts the Copper and halts without tracing or
;                    executing DONE.
;   RUN_SHORT    -- A-short: the trace buffer holds 64 samples.
;   RUN_NOOVL    -- the ROM overlay is not cleared (step 2 left out).
;   RUN_TAILSHIFT-- the tail loop's WAIT(n,$60) becomes WAIT(n+1,$60).
;   RUN_LONGPAD  -- 16 further padding moves: WAIT(0,$60) is fetched after count $60.
;   RUN_NOFIX    -- the fixture constant is not stored (copbeam.i).

        include "nanomig.i"
        include "copbeam.i"

        ifd     RUN_SHORT
SAMPLE_COUNT    equ     64
        else
SAMPLE_COUNT    equ     50000
        endif
NOCOP_SAMPLE_COUNT equ  12000
SLOW_SAMPLE_COUNT equ   400

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        FRAME_INIT

        ifd     RUN_NODONE
        move.l  #LIST_A_BASE,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  #$0200,BPLCON0(a0)
        move.w  #$8280,DMACON(a0)
.hang:  bra.s   .hang
        endif

        ; ---- step 3: write list A, one CPU write per word --------------
        lea     LIST_A_BASE,a1

        move.w  #$0001,(a1)+           ; lambda_min: WAIT($00,$00)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+           ; MARK($A000)
        move.w  #$A000,(a1)+

        move.w  #$30,d0
.vloop:
        move.w  d0,d1
        ifd     RUN_ANCHOR
        addq.w  #1,d1
        endif
        lsl.w   #8,d1
        ifd     RUN_SHIFT
        cmpi.w  #$80,d0
        bne.s   .noshift80
        ori.w   #$43,d1                ; WAIT($80,$42)
        bra.s   .wwrite
.noshift80:
        endif
        ori.w   #$41,d1                ; WAIT(v[+1],$40)
.wwrite:
        move.w  d1,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        move.w  d0,(a1)+                ; MARK(v)

        ifd     RUN_P2WAITED
        cmpi.w  #$7F,d0
        bne.s   .nop2w
        move.w  #$7F41,(a1)+            ; extra WAIT($7F,$40): already past
        move.w  #$FFFE,(a1)+
.nop2w:
        endif

        addq.w  #1,d0
        ifd     RUN_ANCHOR
        cmpi.w  #$FF,d0
        else
        cmpi.w  #$100,d0
        endif
        bne.s   .vloop

        ifnd    RUN_NOPREP
        move.w  #$FFE1,(a1)+           ; WAIT($FF,$E0)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+           ; MARK($A0E0)
        move.w  #$A0E0,(a1)+

        move.w  #$A0F0,d1
.padloop:
        move.w  #$01FE,(a1)+
        move.w  d1,(a1)+
        addq.w  #1,d1
        cmpi.w  #$A100,d1
        bne.s   .padloop
        ifd     RUN_LONGPAD
.padloop2:
        move.w  #$01FE,(a1)+
        move.w  d1,(a1)+
        addq.w  #1,d1
        cmpi.w  #$A110,d1
        bne.s   .padloop2
        endif
        endif

        ; tail loop: n = 0 .. N, WAIT(n,$60) MARK($0100+n)
        moveq   #0,d0
.nloop:
        move.w  d0,d1
        ifd     RUN_TAILSHIFT
        addq.w  #1,d1
        endif
        lsl.w   #8,d1
        ori.w   #$61,d1
        move.w  d1,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        move.w  #$0100,d2
        add.w   d0,d2
        move.w  d2,(a1)+
        addq.w  #1,d0
        ifd     RUN_NTSC
        cmpi.w  #7,d0
        else
        cmpi.w  #$38,d0
        endif
        bne.s   .nloop

        move.w  #$FFFF,(a1)+           ; END
        move.w  #$FFFE,(a1)+

        ; ---- step 4: display registers ---------------------------------
        ifd     RUN_LACE
        move.w  #$0204,BPLCON0(a0)
        else
        move.w  #$0200,BPLCON0(a0)
        endif

        ; ---- step 5: start the Copper (p.30's own sequence) ------------
        move.l  #LIST_A_BASE,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  #$8280,DMACON(a0)

        ; ---- step 6: trace loop -----------------------------------------
        lea     TRACE_BASE,a6

        ifd     RUN_HOLD
.pollh: move.w  VHPOSR(a0),d0
        andi.w  #$FF,d0
        cmpi.w  #$20,d0
        blt.s   .pollh
        move.l  VPOSR(a0),d1
        rept    SAMPLE_COUNT
        move.l  d1,(a6)+
        endr
        else
        ifnd    RUN_NOCOP
        ifnd    RUN_SLOW
        ; a flat run of the same instruction: SAMPLE_COUNT copies of one
        ; long-word read of VPOSR,VHPOSR stored to the trace buffer, with no
        ; loop control between samples
        rept    SAMPLE_COUNT
        move.l  VPOSR(a0),(a6)+
        endr
        endif
        endif

        ifd     RUN_NOCOP
        moveq   #0,d5                  ; previous V
        moveq   #0,d4                  ; boundary counter
        moveq   #0,d7                  ; DMACON-already-cleared flag
        move.l  #NOCOP_SAMPLE_COUNT,d6
.trloop: move.l VPOSR(a0),d1
        move.l  d1,(a6)+
        move.l  d1,d2
        swap    d2
        and.w   #1,d2
        lsl.w   #8,d2
        move.w  d1,d3
        lsr.w   #8,d3
        or.w    d3,d2
        cmp.w   d5,d2
        bge.s   .nowrap
        addq.w  #1,d4
.nowrap: move.w d2,d5
        cmpi.w  #3,d4
        bne.s   .nostop
        tst.w   d7
        bne.s   .nostop
        move.w  #1,d7
        move.w  #$0080,DMACON(a0)
.nostop: subq.l #1,d6
        bne.w   .trloop
        endif

        ifd     RUN_SLOW
        move.l  #SLOW_SAMPLE_COUNT,d6
.trloop2: move.l VPOSR(a0),(a6)+
        move.w  #100,d0
.slowdelay: dbra d0,.slowdelay
        subq.l  #1,d6
        bne.w   .trloop2
        endif
        endif

        ; ---- step 7: DONE -------------------------------------------------
        DONE
