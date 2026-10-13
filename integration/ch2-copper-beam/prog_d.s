; prog_d.s -- Runs D and E: pixel calibration, Copper edge sweep, standard
; screen (SPECIFICATION.md 5, Runs D and E).
;
; Symbols this source answers to (a variant wrapper sets them before
; including this file). With none set the program is D-lo, PAL.
;   RUN_NTSC       -- DIWSTOP = $F4C1 (NTSC) instead of $2CC1 (PAL).
;   RUN_DHI        -- D-hi: HIRES, one plane, 40 words per line.
;   RUN_SWEEP      -- D-sweep: no bitplanes, colour changes by the Copper.
;   RUN_E          -- Run E: standard screen, planes $FFFF, COLOR01 = $FF0.
;   RUN_EWIDEFETCH -- E-widefetch: DDFSTRT $30, DDFSTOP $D8, 22 words per line.
;   RUN_EWIDEWIN   -- E-widewindow: E-widefetch with DIWSTRT $2C71 and
;                     DIWSTOP $..D1.
;   RUN_EBRDBLNK   -- E-border-blank: Run E with BPLCON0 bit 0 and BPLCON3 bit 5 set
;                     (p.307: ENBPLCN3, BRDRBLNK "Border blank").
;   RUN_EBLACKGAP  -- COLOR00 = $000 with the 10th plane word of every line $0000.
;   RUN_ENOBPL     -- Run E with bitplane DMA left off (DMACON = $8280).
;   RUN_DFLAT      -- D-flat: D-lo / D-hi with every plane word $FFFF.
;   RUN_DSHIFT     -- D-shift: line $50, second WAIT at $36+2*24+2.
;   RUN_DDROP      -- D-drop: line $50, second MOVE($180,B) left out.
;   RUN_DLATE      -- D-late: line $50, second WAIT at $3A.
;
; The list is written by the CPU one word per store (so every word appears on
; the Chip-memory port). The program then stores the first VPOSR word at
; RESULTS and polls the beam counter until FIELDS_TO_WAIT field boundaries
; have passed.

        include "nanomig.i"
        include "copbeam.i"

FIELDS_TO_WAIT  equ     6

PLANE_BASE      equ     $020000
PLANE_WORDS     equ     10480

        ifd     RUN_DHI
WPL             equ     40
        else
        ifd     RUN_EWIDEFETCH
WPL             equ     22
        else
        ifd     RUN_EWIDEWIN
WPL             equ     22
        else
WPL             equ     20
        endif
        endif
        endif

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        FRAME_INIT

        ; ---- step 3: plane data and Copper list --------------------------
        ifnd    RUN_SWEEP
        lea     PLANE_BASE,a2
        move.w  #$AAAA,d1
        ifd     RUN_E
        move.w  #$FFFF,d1
        endif
        ifd     RUN_DFLAT
        move.w  #$FFFF,d1
        endif
        move.w  #261,d5                 ; 262 lines
.fline: move.w  #WPL-1,d6
        moveq   #0,d7
.fword: move.w  d1,d2
        ifd     RUN_EBLACKGAP
        cmpi.w  #9,d7
        bne.s   .nogap
        moveq   #0,d2
.nogap:
        endif
        move.w  d2,(a2)+
        addq.w  #1,d7
        dbra    d6,.fword
        dbra    d5,.fline
        endif

        lea     LIST_A_BASE,a1
        ifnd    RUN_SWEEP
        move.w  #$00E0,(a1)+            ; MOVE BPL1PTH
        move.w  #(PLANE_BASE>>16),(a1)+
        move.w  #$00E2,(a1)+            ; MOVE BPL1PTL
        move.w  #(PLANE_BASE&$FFFF),(a1)+
        move.w  #$FFFF,(a1)+            ; END
        move.w  #$FFFE,(a1)+
        else
        move.w  #$0001,(a1)+            ; WAIT($00,$00)
        move.w  #$FFFE,(a1)+
        move.w  #$01FE,(a1)+            ; MARK($D000)
        move.w  #$D000,(a1)+
        move.w  #$0180,(a1)+            ; MOVE($180,B)
        move.w  #$00F0,(a1)+

        ; sub-sweep 1: n = 8 .. 77, line u = $40 + (n - 8)
        moveq   #8,d0                   ; n
.sw1:   move.w  d0,d1
        addi.w  #$38,d1                 ; u = $40 + n - 8
        move.w  d1,d2
        lsl.w   #8,d2
        ori.w   #$37,d2                 ; WAIT(u,$36)
        move.w  d2,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$0180,(a1)+            ; MOVE($180,A_u)
        move.w  d1,d3
        subi.w  #$40,d3
        addi.w  #$0F00,d3
        move.w  d3,(a1)+
        move.w  d1,d2
        lsl.w   #8,d2
        move.w  d0,d4
        add.w   d4,d4
        addi.w  #$36,d4                 ; $36 + 2n
        ifd     RUN_DSHIFT
        cmpi.w  #$50,d1
        bne.s   .nosh
        addq.w  #2,d4
.nosh:
        endif
        ifd     RUN_DLATE
        cmpi.w  #$50,d1
        bne.s   .nolate
        moveq   #$3A,d4
.nolate:
        endif
        or.w    d4,d2
        ori.w   #1,d2
        move.w  d2,(a1)+                ; WAIT(u,$36+2n)
        move.w  #$FFFE,(a1)+
        ifd     RUN_DDROP
        cmpi.w  #$50,d1
        beq.s   .skipB
        endif
        move.w  #$0180,(a1)+            ; MOVE($180,B)
        move.w  #$00F0,(a1)+
.skipB:
        addq.w  #1,d0
        cmpi.w  #78,d0
        bne.s   .sw1

        ; sub-sweep 2: m = 0 .. 69, line u = $86 + m, n = 77 - m
        moveq   #0,d0                   ; m
.sw2:   move.w  d0,d1
        addi.w  #$86,d1                 ; u
        move.w  d1,d2
        lsl.w   #8,d2
        move.w  d0,d4
        add.w   d4,d4
        addi.w  #$36,d4                 ; $36 + 2m
        or.w    d4,d2
        ori.w   #1,d2
        move.w  d2,(a1)+                ; WAIT(u,$36+2m)
        move.w  #$FFFE,(a1)+
        move.w  #$0180,(a1)+            ; MOVE($180,A_u)
        move.w  d1,d3
        subi.w  #$40,d3
        addi.w  #$0F00,d3
        move.w  d3,(a1)+
        move.w  d1,d2
        lsl.w   #8,d2
        ori.w   #$D1,d2                 ; WAIT(u,$D0)
        move.w  d2,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$0180,(a1)+            ; MOVE($180,B)
        move.w  #$00F0,(a1)+
        addq.w  #1,d0
        cmpi.w  #70,d0
        bne.s   .sw2

        move.w  #$FFFF,(a1)+            ; END
        move.w  #$FFFE,(a1)+
        endif

        ; ---- step 4: display registers -----------------------------------
        ifd     RUN_EWIDEWIN
        move.w  #$2C71,DIWSTRT(a0)
        else
        move.w  #$2C81,DIWSTRT(a0)
        endif
        ifd     RUN_NTSC
        ifd     RUN_EWIDEWIN
        move.w  #$F4D1,DIWSTOP(a0)
        else
        move.w  #$F4C1,DIWSTOP(a0)
        endif
        else
        ifd     RUN_EWIDEWIN
        move.w  #$2CD1,DIWSTOP(a0)
        else
        move.w  #$2CC1,DIWSTOP(a0)
        endif
        endif
        move.w  #0,BPLCON1(a0)
        move.w  #0,BPLCON2(a0)
        move.w  #0,BPL1MOD(a0)
        ifd     RUN_SWEEP
        move.w  #$0200,BPLCON0(a0)
        move.w  #$00F0,COLOR00(a0)
        else
        ifd     RUN_DHI
        move.w  #$9200,BPLCON0(a0)
        move.w  #$003C,DDFSTRT(a0)
        move.w  #$00D4,DDFSTOP(a0)
        else
        ifd     RUN_EBRDBLNK
        move.w  #$1201,BPLCON0(a0)
        move.w  #$0020,BPLCON3(a0)
        else
        move.w  #$1200,BPLCON0(a0)
        endif
        ifd     RUN_EWIDEFETCH
        move.w  #$0030,DDFSTRT(a0)
        move.w  #$00D8,DDFSTOP(a0)
        else
        ifd     RUN_EWIDEWIN
        move.w  #$0030,DDFSTRT(a0)
        move.w  #$00D8,DDFSTOP(a0)
        else
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        endif
        endif
        endif
        ifd     RUN_EBLACKGAP
        move.w  #$0000,COLOR00(a0)
        else
        move.w  #$000F,COLOR00(a0)
        endif
        ifd     RUN_E
        move.w  #$0FF0,COLOR01(a0)
        else
        ifd     RUN_EWIDEFETCH
        move.w  #$0FF0,COLOR01(a0)
        else
        ifd     RUN_EWIDEWIN
        move.w  #$0FF0,COLOR01(a0)
        else
        ifd     RUN_EBLACKGAP
        move.w  #$0FF0,COLOR01(a0)
        else
        move.w  #$0FFF,COLOR01(a0)
        endif
        endif
        endif
        endif
        endif

        ; ---- step 5: start the Copper (p.30's own sequence) ---------------
        move.l  #LIST_A_BASE,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        ifd     RUN_SWEEP
        move.w  #$8280,DMACON(a0)
        else
        ifd     RUN_ENOBPL
        move.w  #$8280,DMACON(a0)
        else
        move.w  #$8380,DMACON(a0)
        endif
        endif

        ; ---- step 6: chip identification, then wait for fields -----------
        move.w  VPOSR(a0),RESULTS
        moveq   #0,d5                   ; previous V
        moveq   #0,d4                   ; field boundaries seen
.poll:  move.l  VPOSR(a0),d1
        cmpi.b  #16,d1                  ; H >= 16: V8 and V7-V0 agree
        bcs.s   .poll
        move.w  d1,d3
        lsr.w   #8,d3                   ; V7-V0
        swap    d1
        andi.w  #1,d1
        lsl.w   #8,d1
        or.w    d3,d1                   ; V
        cmp.w   d5,d1
        bge.s   .nofall
        addq.w  #1,d4
.nofall:
        move.w  d1,d5
        cmpi.w  #FIELDS_TO_WAIT,d4
        bcs.s   .poll

        ; ---- step 7: DONE --------------------------------------------------
        DONE
