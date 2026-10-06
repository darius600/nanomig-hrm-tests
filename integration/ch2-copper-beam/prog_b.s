; prog_b.s -- Run B: WAIT on line $FF beyond $E2 (SPECIFICATION.md 5, Run B).
; Each list points COP1LC at its successor, so each list runs one field.
; The lists are written by the CPU, one word per store. The CPU then repeats
; reading the long word at $DFF004 into the trace buffer in blocks.
;
; Symbols this source answers to (a variant wrapper sets one):
;   RUN_NTSC  -- NTSC (the program is the same; the harness selects the standard).
;   (L_FE, the B-FE field, is part of every run.)
;   RUN_BLATE -- B-late: in L_1 a WAIT($FF,$E0) and 16 padding moves precede
;                WAIT($FF,h_1).
;   RUN_B254  -- B-254: in L_2 the first WAIT is for line $FE, and a
;                WAIT($FF,$30) follows its marker.
;   RUN_BCHAIN -- the COP1LC moves of L_3 name L_5.
;   RUN_BFEOFF -- L_FE waits for ($FF,$FE) instead of ($FE,$FE).
;   RUN_BE4   -- L_0 waits for ($FF,$E4) instead of ($FF,$E2).
;   RUN_BEND  -- L_14 waits for ($FE,$FE) instead of ($FF,$FE).
;   RUN_BWAITED -- in L_4 a WAIT($FF,$80) precedes WAIT($FF,$40), so the
;                latter is fetched after its target has passed.
;   RUN_BSHORT -- the trace buffer holds one block only.
;   RUN_SLOW, RUN_NOOVL, RUN_NODONE are not used by this run.

        include "nanomig.i"
        include "copbeam.i"

LISTB_BASE      equ     $010400         ; L_j at LISTB_BASE + $100*j
LISTFE_BASE     equ     $010200
BLOCK_SAMPLES   equ     1000
        ifd     RUN_BSHORT
BLOCK_COUNT     equ     1
        else
BLOCK_COUNT     equ     130
        endif

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        FRAME_INIT

        ; ---- step 3: the lists ------------------------------------------
        lea     LIST_A_BASE,a1          ; L_init
        move.w  #$0080,(a1)+            ; MOVE COP1LCH
        move.w  #$0001,(a1)+
        move.w  #$0082,(a1)+            ; MOVE COP1LCL
        move.w  #(LISTFE_BASE&$FFFF),(a1)+
        move.w  #$FFFF,(a1)+            ; END
        move.w  #$FFFE,(a1)+

        lea     LISTFE_BASE,a1          ; L_FE
        move.w  #$0001,(a1)+            ; WAIT($00,$00)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($B3F0)
        move.w  #$B3F0,(a1)+
        move.w  #$0080,(a1)+            ; COP1LC -> L_0
        move.w  #$0001,(a1)+
        move.w  #$0082,(a1)+
        move.w  #(LISTB_BASE&$FFFF),(a1)+
        ifd     RUN_BFEOFF
        move.w  #$FFFF,(a1)+            ; WAIT($FF,$FE) in place of WAIT($FE,$FE)
        else
        move.w  #$FEFF,(a1)+            ; WAIT($FE,$FE)
        endif
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($B300)
        move.w  #$B300,(a1)+
        move.w  #$FF41,(a1)+            ; WAIT($FF,$40)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($B301)
        move.w  #$B301,(a1)+
        move.w  #$FFFF,(a1)+            ; END
        move.w  #$FFFE,(a1)+

        moveq   #0,d0                   ; j
.lj:    move.w  d0,d1
        lsl.w   #8,d1                   ; $100 * j
        lea     LISTB_BASE,a1
        adda.w  d1,a1                   ; base of L_j
        move.w  d1,d3
        addi.w  #(LISTB_BASE&$FFFF),d3  ; own base, low word
        cmpi.w  #15,d0
        beq.s   .selfnext
        addi.w  #$100,d3                ; successor's base
        ifd     RUN_BCHAIN
        cmpi.w  #3,d0
        bne.s   .nochain
        addi.w  #$100,d3                ; L_3 names L_5
.nochain:
        endif
.selfnext:
        move.w  d0,d2                   ; index of h and of the markers: j, but 14 for j = 15
        cmpi.w  #15,d2
        bne.s   .hok
        moveq   #14,d2
.hok:   move.w  d2,d7
        add.w   d2,d2
        addi.w  #$E2,d2                 ; h_j
        ori.w   #$FF01,d2               ; first word of WAIT($FF,h_j)
        ifd     RUN_BE4
        cmpi.w  #0,d0
        bne.s   .nobe4
        move.w  #$FFE5,d2               ; L_0 waits for $E4
.nobe4:
        endif
        ifd     RUN_BEND
        cmpi.w  #14,d0
        bne.s   .nobend
        move.w  #$FEFF,d2               ; L_14 waits for ($FE,$FE)
.nobend:
        endif

        move.w  #$0001,(a1)+            ; WAIT($00,$00)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($B000+j)
        move.w  #$B000,d4
        add.w   d7,d4
        move.w  d4,(a1)+
        move.w  #$0080,(a1)+            ; MOVE COP1LCH
        move.w  #$0001,(a1)+
        move.w  #$0082,(a1)+            ; MOVE COP1LCL
        move.w  d3,(a1)+

        ifd     RUN_BWAITED
        cmpi.w  #4,d0
        bne.s   .nobw
        move.w  #$FF81,(a1)+            ; WAIT($FF,$80)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($B520)
        move.w  #$B520,(a1)+
.nobw:
        endif
        ifd     RUN_B254
        cmpi.w  #2,d0
        bne.s   .n254a
        move.w  #$FE41,(a1)+            ; WAIT($FE,$40)
        bra.s   .w254a
.n254a:
        endif
        move.w  #$FF41,(a1)+            ; WAIT($FF,$40)
.w254a:
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($B100+j)
        move.w  #$B100,d4
        add.w   d7,d4
        move.w  d4,(a1)+

        ifd     RUN_B254
        cmpi.w  #2,d0
        bne.s   .n254b
        move.w  #$FF31,(a1)+            ; WAIT($FF,$30)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($B502)
        move.w  #$B502,(a1)+
.n254b:
        endif

        ifd     RUN_BLATE
        cmpi.w  #1,d0
        bne.s   .nlate
        move.w  #$FFE1,(a1)+            ; WAIT($FF,$E0)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($B400)
        move.w  #$B400,(a1)+
        move.w  #$B410,d5
.latepad:
        move.w  #$01FE,(a1)+
        move.w  d5,(a1)+
        addq.w  #1,d5
        cmpi.w  #$B420,d5
        bne.s   .latepad
.nlate:
        endif

        move.w  d2,(a1)+                ; WAIT($FF,h_j)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($B200+j)
        move.w  #$B200,d4
        add.w   d7,d4
        move.w  d4,(a1)+
        move.w  #$FFFF,(a1)+            ; END
        move.w  #$FFFE,(a1)+

        addq.w  #1,d0
        cmpi.w  #16,d0
        bne     .lj

        ; ---- step 4: display registers -----------------------------------
        move.w  #$0200,BPLCON0(a0)

        ; ---- step 5: start the Copper (p.30's own sequence) ---------------
        move.l  #LIST_A_BASE,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  #$8280,DMACON(a0)

        ; ---- step 6: trace loop, blocks of BLOCK_SAMPLES samples ----------
        lea     TRACE_BASE,a6
        move.w  #BLOCK_COUNT-1,d6
.block:
        rept    BLOCK_SAMPLES
        move.l  VPOSR(a0),(a6)+
        endr
        dbra    d6,.block

        ; ---- step 7: DONE -------------------------------------------------
        DONE
