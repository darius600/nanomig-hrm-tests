; prog_p5_core.s -- P5: VBSTRT ($1CC, ECS) and the line at which vertical
; blanking starts. The bitplane data (196 lines of $FFFF) and the Copper
; list (COLOR01 changes at WAIT(N-1,$30) and WAIT(N+1,$30)) are preloaded
; by the test at BPL_PLANE1_ADDR / COP_LIST_ADDR -- the overlay blocks a
; CPU write below $80000 (nanomig_harness.h's address-space notes).
;
; Flags: NTSC_MODE (BEAMCON0 carries the NTSC value); N_V2 (VBSTRT/VBSTOP
; use N=$B0 instead of $A0, and the poll threshold below follows).
;
; Four times: poll INTREQR bit 5 (VERTB), clear it, store the iteration
; index as a marker, poll VHPOSR's V7-V0 field until it reaches N-3 (both N
; values stay under 256, so V8 is 0 throughout and can be ignored), then
; read VPOSR/VHPOSR 200 times in a row, storing every value.

        include "nanomig.i"
        include "playfield.i"

        ifd     N_V2
N_VAL           equ     $B0
        else
N_VAL           equ     $A0
        endc

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)

        move.w  #$0359,COLOR00(a0)
        move.w  #$0EA2,COLOR01(a0)
        move.w  #$0000,BPLCON1(a0)
        move.w  #$0000,BPLCON2(a0)
        move.w  #$1000,BPLCON0(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$F0C1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #0,BPL1MOD(a0)

        move.w  #N_VAL,VBSTRT(a0)
        move.w  #N_VAL+8,VBSTOP(a0)

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

        move.l  #COP_LIST_ADDR,d0
        move.w  d0,COP1LCL(a0)
        swap    d0
        move.w  d0,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)         ; strobe: load Copper PC from COP1LC
        ifd     MUT_NOBPLEN
        move.w  #$8280,DMACON(a0)      ; P5-a contrast: BPLEN not set
        else
        move.w  #$8380,DMACON(a0)      ; SET/CLR, DMAEN, BPLEN, COPEN
        endc

        lea     MEASURE_BASE,a1
        moveq   #0,d6                  ; iteration index 0..3
.iter:
.poll:  move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .poll
        move.w  #$0020,INTREQ(a0)      ; clear VERTB

.pollh: move.w  VHPOSR(a0),d1
        lsr.w   #8,d1                  ; d1 = V7-V0 (VHPOSR bits 15-8)
        cmp.w   #N_VAL-3,d1
        blt.s   .pollh

        move.w  d6,(a1)+               ; marker: the iteration index
        move.w  #200-1,d5
.burst: move.l  VPOSR(a0),(a1)+        ; long read of VPOSR,VHPOSR (p.216)
        dbra    d5,.burst

        addq.w  #1,d6
        cmp.w   #4,d6
        bne.s   .iter

        DONE
