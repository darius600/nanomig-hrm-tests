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
;   RUN_SLOW     -- A-slow: the flat run is cut into blocks of SLOW_BLOCK
;                    samples with a delay of SLOW_DELAY dbra iterations
;                    (shorter than one line, longer than 16 slots) between
;                    two blocks.
;   RUN_NOCOP    -- A-noCop: DMACON = $0080 (COPEN cleared) is written by
;                    the CPU between two parts of the flat run, after the
;                    third field boundary.
;   RUN_DUP      -- A-dup: the flat run, with one read of $DFF004 stored twice
;                    after every DUP_GAP samples.
;   RUN_ANCHOR   -- A-anchor: the main loop's WAIT target is v+1 instead of
;                    v (v running $30..$FE), while MARK(v) keeps v.
;   RUN_SHIFT    -- A-shift: WAIT($80,$42) in place of WAIT($80,$40).
;   RUN_P2WAITED -- an extra WAIT($7F,$40) inserted right after MARK($7F):
;                    it is already satisfied when fetched (P2's "waited"
;                    precondition mutation).
;   RUN_NODONE   -- the whole program, but it halts in place of storing the
;                    completion word.
;   RUN_SHORT    -- A-short: the trace buffer holds 64 samples.
;   RUN_FEW      -- A-few: the trace buffer holds 35000 samples.
;   RUN_EARLYCOP -- the Copper is started before the list is written; the
;                    CPU writes the list once V is in $20 .. $3F.
;   RUN_NOE0     -- MARK($A0E0) after WAIT($FF,$E0) is another padding move.
;   RUN_P7EARLY  -- WAIT($00,$60) and its marker precede WAIT($FF,$E0), so its
;                    second word is fetched on line 255; the other tail
;                    WAITs follow the padding as before.
;   RUN_NOH0     -- the sampler stores bursts of NOH0_BURST samples that
;                    start at H >= $10 and end before the end of the line,
;                    so H = 0 is never stored.
;   RUN_TAILSHIFT-- the tail loop's WAIT(n,$60) becomes WAIT(n+1,$60).
;   RUN_LONGPAD  -- 16 further padding moves: WAIT(0,$60) is fetched after count $60.
;   RUN_NOFIX    -- the fixture constant is not stored (copbeam.i).

        include "nanomig.i"
        include "copbeam.i"

        ifd     RUN_SHORT
SAMPLE_COUNT    equ     64
        else
        ifd     RUN_FEW
SAMPLE_COUNT    equ     35000
        else
SAMPLE_COUNT    equ     50000
        endif
        endif
NOCOP_FIRST     equ     28000           ; samples before DMACON is written
DUP_GAP         equ     5000            ; samples between two reads stored twice
FLATDUP         macro                   ; DUP_GAP samples, then one read stored twice
                rept    DUP_GAP
                move.l  VPOSR(a0),(a6)+
                endr
                move.l  VPOSR(a0),d1
                move.l  d1,(a6)+
                move.l  d1,(a6)+
                endm
SLOW_BLOCK      equ     1000            ; samples between two delays
SLOW_DELAY      equ     20              ; dbra iterations of a delay
NOH0_LINES      equ     2500            ; lines sampled
NOH0_BURST      equ     12              ; samples of a burst

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        FRAME_INIT

        ifd     RUN_EARLYCOP
        ; ---- steps 4 and 5 before step 3: the Copper reads the empty
        ; list area; the CPU writes the list once V is in $20 .. $3F ------
        move.w  #$0200,BPLCON0(a0)
        move.l  #LIST_A_BASE,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  #$8280,DMACON(a0)
.early: move.w  VPOSR(a0),d1
        btst    #0,d1                   ; V8 clear
        bne.s   .early
        move.b  VHPOSR(a0),d0           ; V7 ... V0
        cmpi.b  #$20,d0
        bcs.s   .early
        cmpi.b  #$40,d0
        bcc.s   .early
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

        ifd     RUN_P7EARLY
        move.w  #$0061,(a1)+           ; WAIT($00,$60) before WAIT($FF,$E0): fetched on line 255
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+           ; MARK($0100)
        move.w  #$0100,(a1)+
        endif
        move.w  #$FFE1,(a1)+           ; WAIT($FF,$E0)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        ifd     RUN_NOE0
        move.w  #$A0EF,(a1)+           ; a padding move in place of MARK($A0E0)
        else
        move.w  #$A0E0,(a1)+           ; MARK($A0E0)
        endif

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

        ; tail loop: n = 0 .. N, WAIT(n,$60) MARK($0100+n)
        ifd     RUN_P7EARLY
        moveq   #1,d0
        else
        moveq   #0,d0
        endif
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

        ifnd    RUN_EARLYCOP
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
        endif

        ; ---- step 6: trace loop -----------------------------------------
        lea     TRACE_BASE,a6

        ifd     RUN_NOH0
        ; one burst per line: wait for the start of a line (H below $30), then
        ; for H >= $10, and store NOH0_BURST samples, which end before the end
        ; of the line
        move.w  #NOH0_LINES-1,d7
.nline:
.nw1:   move.b  VHPOSR+1(a0),d0
        cmpi.b  #$30,d0
        bcc.s   .nw1
.nw2:   move.b  VHPOSR+1(a0),d0
        cmpi.b  #$10,d0
        bcs.s   .nw2
        rept    NOH0_BURST
        move.l  VPOSR(a0),(a6)+
        endr
        dbra    d7,.nline
        else
        ifd     RUN_SLOW
        move.w  #(SAMPLE_COUNT/SLOW_BLOCK)-1,d6
.block: rept    SLOW_BLOCK
        move.l  VPOSR(a0),(a6)+
        endr
        move.w  #SLOW_DELAY,d0
.slowdelay: dbra d0,.slowdelay
        dbra    d6,.block
        else
        ifd     RUN_NOCOP
        rept    NOCOP_FIRST
        move.l  VPOSR(a0),(a6)+
        endr
        move.w  #$0080,DMACON(a0)
        rept    SAMPLE_COUNT-NOCOP_FIRST
        move.l  VPOSR(a0),(a6)+
        endr
        else
        ifd     RUN_DUP
        FLATDUP
        FLATDUP
        FLATDUP
        FLATDUP
        FLATDUP
        FLATDUP
        FLATDUP
        FLATDUP
        FLATDUP
        rept    SAMPLE_COUNT-9*(DUP_GAP+2)
        move.l  VPOSR(a0),(a6)+
        endr
        else
        ; a flat run of the same instruction: SAMPLE_COUNT copies of one
        ; long-word read of VPOSR,VHPOSR stored to the trace buffer, with no
        ; loop control between samples
        rept    SAMPLE_COUNT
        move.l  VPOSR(a0),(a6)+
        endr
        endif
        endif
        endif
        endif

        ; ---- step 7: DONE -------------------------------------------------
        ifd     RUN_NODONE
.nodone: bra.s  .nodone
        else
        DONE
        endif
