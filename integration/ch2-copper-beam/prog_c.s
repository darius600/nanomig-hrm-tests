; prog_c.s -- Run C: horizontal position sweep (SPECIFICATION.md 5, Run C).
; List C is written by the CPU, one word per store. The CPU then repeats
; reading the long word at $DFF004 into the trace buffer.
;
; Symbols this source answers to (a variant wrapper sets one before
; including this file):
;   RUN_CSHIFT    -- C-shift: k = 50, WAIT(u_50,$66) instead of $64.
;   RUN_CSHIFTEND -- C-shift-end: k = 106, WAIT(u_106,$D6) instead of $D4.
;   RUN_CLATE     -- C-late: k = 90, the reference is WAIT(u_90,$C4).
;   RUN_CNEXT     -- C-next: j = 5, WAIT(w_5+1,$50) instead of WAIT(w_5,g_5).
;   RUN_CR1LINE   -- j = 5, WAIT(w_5-1,$C0) instead of WAIT(w_5,$C0).
;   RUN_NTSC      -- NTSC (the program is the same).
;   RUN_SLOW, RUN_HOLD, RUN_NOCOP, RUN_NODONE, RUN_SHORT, RUN_COPOFF -- unused:
;                    no wrapper sets them; prog_a.s no longer has these forms.

        include "nanomig.i"
        include "copbeam.i"

        ifd     RUN_SHORT
SAMPLE_COUNT    equ     64
        else
SAMPLE_COUNT    equ     30000
        endif
NOCOP_SAMPLE_COUNT equ  20000
SLOW_SAMPLE_COUNT equ   300

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

        ; ---- step 3: list C -----------------------------------------------
        lea     LIST_A_BASE,a1
        move.w  #$0001,(a1)+            ; WAIT($00,$00)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($C000)
        move.w  #$C000,(a1)+

        moveq   #1,d0                   ; k
.kloop: move.w  d0,d1
        addi.w  #$30,d1                 ; u_k
        lsl.w   #8,d1                   ; u_k << 8
        move.w  d0,d2
        add.w   d2,d2                   ; h_k = 2k
        cmpi.w  #$80,d2
        bhi     .second

        ; h_k <= $80: WAIT(u,h) MARK($C100+k), then WAIT(u,$C0) MARK($C200+k)
        move.w  d2,d3
        ifd     RUN_CSHIFT
        cmpi.w  #50,d0
        bne.s   .nosh1
        addq.w  #2,d3
.nosh1:
        endif
        or.w    d1,d3
        ori.w   #1,d3
        move.w  d3,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        move.w  #$C100,d4
        add.w   d0,d4
        move.w  d4,(a1)+
        move.w  d1,d3
        ori.w   #$C1,d3
        move.w  d3,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        move.w  #$C200,d4
        add.w   d0,d4
        move.w  d4,(a1)+
        bra     .knext

.second:
        ; h_k > $80: WAIT(u,$10) MARK($C200+k), then WAIT(u,h) MARK($C100+k)
        move.w  d1,d3
        ori.w   #$11,d3
        ifd     RUN_CLATE
        cmpi.w  #90,d0
        bne.s   .nolate
        move.w  d1,d3
        ori.w   #$C5,d3
.nolate:
        endif
        move.w  d3,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        move.w  #$C200,d4
        add.w   d0,d4
        move.w  d4,(a1)+
        move.w  d2,d3
        ifd     RUN_CSHIFTEND
        cmpi.w  #106,d0
        bne.s   .nosh2
        addq.w  #2,d3
.nosh2:
        endif
        or.w    d1,d3
        ori.w   #1,d3
        move.w  d3,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        move.w  #$C100,d4
        add.w   d0,d4
        move.w  d4,(a1)+

.knext: addq.w  #1,d0
        cmpi.w  #114,d0
        bne     .kloop

        moveq   #0,d0                   ; j
.jloop: move.w  d0,d1
        add.w   d1,d1
        addi.w  #$A4,d1                 ; w_j
        move.w  d1,d2
        lsl.w   #8,d2                   ; w_j << 8
        move.w  d0,d3
        add.w   d3,d3
        addi.w  #$E4,d3                 ; g_j

        ; r1_j: WAIT(w_j,$C0) MARK($C300+j)
        move.w  d2,d4
        ifd     RUN_CR1LINE
        cmpi.w  #5,d0
        bne.s   .nor1
        subi.w  #$100,d4
.nor1:
        endif
        ori.w   #$C1,d4
        move.w  d4,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        move.w  #$C300,d4
        add.w   d0,d4
        move.w  d4,(a1)+

        ; q_j: WAIT(w_j,g_j) MARK($C400+j)
        move.w  d2,d4
        or.w    d3,d4
        ori.w   #1,d4
        ifd     RUN_CNEXT
        cmpi.w  #5,d0
        bne.s   .nonext
        move.w  d2,d4
        addi.w  #$100,d4
        ori.w   #$51,d4
.nonext:
        endif
        move.w  d4,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        move.w  #$C400,d4
        add.w   d0,d4
        move.w  d4,(a1)+

        ; r2_j: WAIT(w_j+1,$40) MARK($C500+j)
        move.w  d2,d4
        addi.w  #$100,d4
        ori.w   #$41,d4
        move.w  d4,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+
        move.w  #$C500,d4
        add.w   d0,d4
        move.w  d4,(a1)+

        addq.w  #1,d0
        cmpi.w  #14,d0
        bne     .jloop

        move.w  #$FFFF,(a1)+            ; END
        move.w  #$FFFE,(a1)+

        ; ---- step 4: display registers -----------------------------------
        move.w  #$0200,BPLCON0(a0)

        ; ---- step 5: start the Copper (p.30's own sequence) ---------------
        move.l  #LIST_A_BASE,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        ifd     RUN_COPOFF
        move.w  #$8200,DMACON(a0)
        else
        move.w  #$8280,DMACON(a0)
        endif

        ; ---- step 6: trace loop -------------------------------------------
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
        move.w  #4000,d0
.slowdelay: dbra d0,.slowdelay
        subq.l  #1,d6
        bne.w   .trloop2
        endif
        endif

        ; ---- step 7: DONE -------------------------------------------------
        DONE
