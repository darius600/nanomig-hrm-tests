; prog_p3_base.s -- P3: DMACON controls every DMA channel (SPECIFICATION.md
; P3). Included by the eleven case files and the three mutation files
; below, each setting one symbol first (default = Base, P = $03FF).
;
;   (no symbol)   -- Base: DMACON = $83FF (every channel + DMAEN)
;   P3_OFF_BPL    -- Case BPL:  $82FF (bit 8 clear)
;   P3_OFF_SPR    -- Case SPR:  $83DF (bit 5 clear)
;   P3_OFF_AUD0   -- Case AUD0: $83FE (bit 0 clear)
;   P3_OFF_AUD1   -- Case AUD1: $83FD (bit 1 clear)
;   P3_OFF_AUD2   -- Case AUD2: $83FB (bit 2 clear)
;   P3_OFF_AUD3   -- Case AUD3: $83F7 (bit 3 clear)
;   P3_OFF_DSK    -- Case DSK:  $83EF (bit 4 clear)
;   P3_OFF_BLT    -- Case BLT:  $83BF (bit 6 clear)
;   P3_OFF_COP    -- Case COP:  $837F (bit 7 clear)
;   P3_CASE_M     -- Case M:    $81FF (bit 9, DMAEN, clear)
;   P3_MUT_A      -- precondition (a) mutation: the second DSKLEN=$C064
;                    write left out (Base pattern otherwise)
;   P3_MUT_B      -- precondition (b) mutation: DMACON written without
;                    SET/CLR ($03FF, Base pattern otherwise)
;   P3_MUT_C      -- precondition (c) mutation: AUD0LC at AUD0's region
;                    start + 256, not at the region's own start (Base
;                    pattern otherwise)
;   P3_MUT_D      -- precondition (d) mutation: 2.4's own sparse-sampler
;                    contrast for B2/B3 (Base pattern otherwise)

        include "nanomig.i"
        include "copctl.i"

; This proposition's own nine data regions (SPECIFICATION.md P3 stimulus,
; step 2): 32 KB each, $10000 + k*$8000, k = 0..8, in order BPL, SPR, AUD0,
; AUD1, AUD2, AUD3, DSK, BLT, COP.
BPL_REGION      equ     $010000
SPR_REGION      equ     $018000
AUD0_REGION     equ     $020000
AUD1_REGION     equ     $028000
AUD2_REGION     equ     $030000
AUD3_REGION     equ     $038000
DSK_REGION      equ     $040000
BLT_REGION      equ     $048000
COP_REGION      equ     $050000

        org     PROGRAM_ORG

start:
        ; step 1: common prologue, overlay cleared (2.1).
        CLEAR_OVL_DDR
        lea     CUSTOM,a0
        CLEAR_DMA_INT

        ; step 2: bitplane registers (p.59, p.60, p.63, p.64).
        move.w  #$1200,BPLCON0(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #0,BPL1MOD(a0)

        ; step 3: audio x = 0-3 (p.139, p.140, p.143).
        ifd     P3_MUT_C
        move.l  #AUD0_REGION+256,AUD0LCH(a0)
        else
        move.l  #AUD0_REGION,AUD0LCH(a0)
        endif
        move.w  #16,AUD0LEN(a0)
        move.w  #447,AUD0PER(a0)
        move.w  #48,AUD0VOL(a0)
        move.l  #AUD1_REGION,AUD1LCH(a0)
        move.w  #16,AUD1LEN(a0)
        move.w  #447,AUD1PER(a0)
        move.w  #48,AUD1VOL(a0)
        move.l  #AUD2_REGION,AUD2LCH(a0)
        move.w  #16,AUD2LEN(a0)
        move.w  #447,AUD2PER(a0)
        move.w  #48,AUD2VOL(a0)
        move.l  #AUD3_REGION,AUD3LCH(a0)
        move.w  #16,AUD3LEN(a0)
        move.w  #447,AUD3PER(a0)
        move.w  #48,AUD3VOL(a0)

        ; step 4: Copper -- list `$FFFF,$FFFE` at COP_REGION; COP1LC, COPJMP1 (p.30).
        move.l  #$FFFFFFFE,COP_REGION
        move.l  #COP_REGION,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        ; step 5: sprite data structures (p.110): sprite 0 -- control words,
        ; data words, end-of-data words; sprites 1-7 -- end-of-data words
        ; only, all sharing one small structure.
        lea     SPR_REGION,a1
        move.w  #$5060,(a1)+
        move.w  #$5200,(a1)+
        move.w  #$0FF0,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0FF0,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.l  #$00000000,SPR_REGION+$20

        ; step 6: wait for vertical blank (p.219).
        move.w  #$0020,INTREQ(a0)
        WAIT_INTREQ_SET 5

        ; step 7: in that vertical blank, write BPL1PT and SPR0PT-SPR7PT
        ; (p.110).
        move.l  #BPL_REGION,BPL1PTH(a0)
        move.l  #SPR_REGION,SPR0PTH(a0)
        move.l  #SPR_REGION+$20,SPR1PTH(a0)
        move.l  #SPR_REGION+$20,SPR2PTH(a0)
        move.l  #SPR_REGION+$20,SPR3PTH(a0)
        move.l  #SPR_REGION+$20,SPR4PTH(a0)
        move.l  #SPR_REGION+$20,SPR5PTH(a0)
        move.l  #SPR_REGION+$20,SPR6PTH(a0)
        move.l  #SPR_REGION+$20,SPR7PTH(a0)

        lea     RECBUF,a6
        moveq   #0,d7                   ; running record count

        ; step 8: M1; DMACON = the case's pattern; M2; COPJMP1 again, MJ
        ; (2.8 step 3) -- the runs with bits 9 and 7 set then do not depend
        ; on the step-4 strobe.
        move.w  #1,MARKBUF              ; M1
        ifd     P3_OFF_BPL
        move.w  #$82FF,DMACON(a0)
        else
        ifd     P3_OFF_SPR
        move.w  #$83DF,DMACON(a0)
        else
        ifd     P3_OFF_AUD0
        move.w  #$83FE,DMACON(a0)
        else
        ifd     P3_OFF_AUD1
        move.w  #$83FD,DMACON(a0)
        else
        ifd     P3_OFF_AUD2
        move.w  #$83FB,DMACON(a0)
        else
        ifd     P3_OFF_AUD3
        move.w  #$83F7,DMACON(a0)
        else
        ifd     P3_OFF_DSK
        move.w  #$83EF,DMACON(a0)
        else
        ifd     P3_OFF_BLT
        move.w  #$83BF,DMACON(a0)
        else
        ifd     P3_OFF_COP
        move.w  #$837F,DMACON(a0)
        else
        ifd     P3_CASE_M
        move.w  #$81FF,DMACON(a0)
        else
        ifd     P3_MUT_B
        move.w  #$03FF,DMACON(a0)       ; precondition (b) mutation: no SET/CLR
        else
        move.w  #$83FF,DMACON(a0)       ; Base
        endif
        endif
        endif
        endif
        endif
        endif
        endif
        endif
        endif
        endif
        endif
        move.w  #2,MARKBUF+2            ; M2
        move.w  #0,COPJMP1(a0)          ; strobe again (2.8 step 3)
        move.w  #3,MARKBUF+4            ; MJ

        ; step 9: disk, in p.247's order.
        move.l  #DSK_REGION,DSKPTH(a0)
        move.w  #$4000,DSKLEN(a0)
        move.w  #$C064,DSKLEN(a0)
        ifnd    P3_MUT_A
        move.w  #$C064,DSKLEN(a0)       ; "This actually starts the DMA"
        endif
        move.w  DSKBYTR(a0),RESULTS+4

        ; step 10: blitter (p.200, p.267).
        move.w  #$01FF,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #BLT_REGION,BLTDPTH(a0)
        move.w  #$1020,BLTSIZE(a0)      ; last

        ; step 11: DMACONR, then the sampler until the end of the first
        ; complete frame after M2. The same unrolled FAST_SAMPLE loop
        ; (3000*8) that measured >= two complete frames for P1's own
        ; window; one complete frame is a smaller ask.
        move.w  DMACONR(a0),RESULTS+6

        ifd     P3_MUT_D
        ; precondition (d) mutation: 2.4's own sparse-sampler contrast for
        ; B2/B3 (same shape as P1c/P2c's own SLOWDELAY mutation).
        move.w  #4000-1,d3
.pd:    FAST_SAMPLE
        addq.l  #1,d7
        SLOWDELAY
        dbra    d3,.pd
        else
        move.w  #3000-1,d3
.pd:    FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        FAST_SAMPLE
        addq.l  #8,d7
        dbra    d3,.pd
        endif

        move.l  d7,RESULTS+8

        DONE
