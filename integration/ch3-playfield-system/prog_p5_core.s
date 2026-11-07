; prog_p5_core.s -- P5: VBSTRT ($1CC, ECS) and the line at which vertical
; blanking starts. The bitplane data (196 lines of $FFFF) and the Copper
; list (COLOR01 changes at WAIT(N-1,$30) and WAIT(N+1,$30)) are preloaded
; by the test at BPL_PLANE1_ADDR / COP_LIST_ADDR -- the overlay blocks a
; CPU write below $80000 (nanomig_harness.h's address-space notes).
;
; Flags: MUT_BAD_START (fixture contrast: another start word), MUT_BAD_MARKER (every marker
; stored with bit 8 set); NTSC_MODE (BEAMCON0 carries the NTSC value); N_V2 (VBSTRT/VBSTOP
; use N=$B0 instead of $A0, and the poll threshold below follows).
; MUT_NO_VARVBEN: BEAMCON0 without VARVBEN (P5-MAIN contrast). MUT_WRONGSTD: BEAMCON0 bit 5
; opposite the configured standard (P5-c contrast). MUT_VALUE: VBSTRT/VBSTOP name the lines
; $B0/$B8 while the Copper markers stay at N=$A0 (P5-MAIN value contrast). MUT_LATEDIW:
; DIWSTRT opens the window at line N-2 (P5-a contrast). MUT_ONESKIP: the second marker
; follows a VERTB detection that skips one field (P5-c marker-clause contrast). MUT_SHIFT:
; after the last burst, one write to VHPOSW moves H sixteen counts on with V unchanged
; (CAL-VIDEO contrast). No page gives
; the bit layout of VBSTRT and VBSTOP, so the line is written in the low bits and
; in the high byte alike: VBSTRT = N*$101, VBSTOP = (N+8)*$101.
;
; Four times: poll INTREQR bit 5 (VERTB), clear it, poll VHPOSR's V7-V0
; field until it reaches N-3 (both N values stay under 256, so V8 is 0
; throughout and can be ignored), store the iteration index as a marker,
; then read VPOSR/VHPOSR 200 times in a row, storing every value.

        include "nanomig.i"
        include "playfield.i"

        ifd     N_V2
N_VAL           equ     $B0
VB_START        equ     $B0B0
VB_STOP         equ     $B8B8
        else
N_VAL           equ     $A0
        ifd     MUT_VALUE
VB_START        equ     $B0B0
VB_STOP         equ     $B8B8
        else
VB_START        equ     $A0A0
VB_STOP         equ     $A8A8
        endc
        endc

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        ifd     MUT_BAD_START
        move.w  #$5AFF,START_ADDR      ; start word (contrast: another value)
        else
        move.w  #$5A05,START_ADDR      ; start word
        endc

        move.w  #$0359,COLOR00(a0)
        move.w  #$0EA2,COLOR01(a0)
        move.w  #$0000,BPLCON1(a0)
        move.w  #$0000,BPLCON2(a0)
        move.w  #$1000,BPLCON0(a0)
        ifd     MUT_LATEDIW
        move.w  #$9E81,DIWSTRT(a0)     ; window opens at line N-2 (N=$A0)
        else
        move.w  #$2C81,DIWSTRT(a0)
        endc
        move.w  #$F0C1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #0,BPL1MOD(a0)

        move.w  #VB_START,VBSTRT(a0)
        move.w  #VB_STOP,VBSTOP(a0)

        ifd     MUT_NO_VARVBEN
        ifd     NTSC_MODE
        move.w  #$0000,BEAMCON0(a0)     ; contrast: VARVBEN=0, bit5=0 (NTSC)
        else
        move.w  #$0020,BEAMCON0(a0)     ; contrast: VARVBEN=0, bit5=1 (PAL)
        endc
        else
        ifd     MUT_WRONGSTD
        ifd     NTSC_MODE
        move.w  #$1020,BEAMCON0(a0)     ; contrast: bit5=1 while configured NTSC
        else
        move.w  #$1000,BEAMCON0(a0)     ; contrast: bit5=0 while configured PAL
        endc
        else
        ifd     NTSC_MODE
        move.w  #$1000,BEAMCON0(a0)     ; VARVBEN=1, bit5=0 (NTSC), HARDDIS=0
        else
        move.w  #$1020,BEAMCON0(a0)     ; VARVBEN=1, bit5=1 (PAL)
        endc
        endc
        endc

        move.l  #COP_LIST_ADDR,COP1LCH(a0)  ; COP1LC, one long-word write to COP1LCH
        move.w  #0,COPJMP1(a0)         ; strobe: load Copper PC from COP1LC
        move.w  #$8380,DMACON(a0)      ; SET/CLR, DMAEN, BPLEN, COPEN

        lea     MEASURE_BASE,a1
        moveq   #0,d6                  ; iteration index 0..3
.iter:
.poll:  move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .poll
        move.w  #$0020,INTREQ(a0)      ; clear VERTB
        ifd     MUT_ONESKIP
        cmp.w   #1,d6
        bne.s   .noskip
.poll2: move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .poll2
        move.w  #$0020,INTREQ(a0)      ; the second marker skips one field
.noskip:
        endc

.pollh: move.w  VHPOSR(a0),d1
        lsr.w   #8,d1                  ; d1 = V7-V0 (VHPOSR bits 15-8)
        cmp.w   #N_VAL-3,d1
        blt.s   .pollh

        ifd     MUT_BAD_MARKER
        move.w  d6,d1
        or.w    #$0100,d1
        move.w  d1,(a1)+               ; marker with bit 8 set (contrast)
        else
        move.w  d6,(a1)+               ; marker: the iteration index
        endc
        move.w  #200-1,d5
.burst: move.l  VPOSR(a0),(a1)+        ; long read of VPOSR,VHPOSR (p.216)
        dbra    d5,.burst

        addq.w  #1,d6
        cmp.w   #4,d6
        bne.s   .iter
        ifd     MUT_SHIFT
.h:     move.w  VHPOSR(a0),d1          ; after the last burst, early in a line
        move.w  d1,d2
        and.w   #$00FF,d2
        cmp.w   #$40,d2
        bcs.s   .h
        cmp.w   #$60,d2
        bcc.s   .h
        add.w   #$0010,d1              ; one write to VHPOSW (p.216): H sixteen counts on, V unchanged
        move.w  d1,$2C(a0)
        endc

        DONE
