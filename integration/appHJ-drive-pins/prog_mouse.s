; prog_mouse.s -- P1-P4 (SPECIFICATION.md section 4): a single continuous
; sampling loop over four back-to-back phases (M0H, M0V, M1H, M1V). The
; harness supplies the pin schedule of SPECIFICATION.md 4.2 by changing
; joystick0/joystick1 while this loop runs; the program itself is
; schedule-agnostic, it only samples four words (VHPOSR, JOY0DAT, JOY1DAT,
; VPOSR) per line change (SPECIFICATION.md 3.3) for TOTAL_SAMPLES lines,
; covering all four 512-line phases (16+192+288+16 each), then stops.
        include "nanomig.i"
        include "hj_defs.i"

TOTAL_SAMPLES   equ     2048            ; 4 phases x 512 lines (SPECIFICATION.md 4.2)

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #0,BPLCON0(a0)                 ; light pen latch off (SPECIFICATION.md 3.2)
        move.w  #FIXTURE_VALUE,FIXTURE_ADDR    ; fixture, stored before anything is graded

        move.b  VHPOSR(a0),d7                  ; prime: current line value
        lea     BUFBASE,a1
        move.w  #TOTAL_SAMPLES-1,d6
.loop:
.wait:  move.b  VHPOSR(a0),d0
        cmp.b   d7,d0
        beq.s   .wait
        ifd     PJ2SKIP
        cmp.b   #$FF,d0                        ; PJ2 contrast: line 255 is not sampled,
        bne.s   .noskip                        ; so the next stored sample ends at low byte $00
.skip:  move.b  VHPOSR(a0),d0                  ; without a frame start
        cmp.b   #$FF,d0
        beq.s   .skip
.noskip:
        endif
        move.b  d0,d7
        move.w  VHPOSR(a0),(a1)+
        ifd     SWAPPHASE
        move.w  d6,d1                          ; register-order contrast: in phase SWAPPHASE
        lsr.w   #8,d1                          ; (512 samples; d6 counts down from
        lsr.w   #1,d1                          ; TOTAL_SAMPLES-1) the two joystick
        cmp.w   #3-SWAPPHASE,d1                ; registers are stored in the other order
        bne.s   .plain
        move.w  JOY1DAT(a0),(a1)+
        move.w  JOY0DAT(a0),(a1)+
        bra.s   .vpos
        endif
.plain: move.w  JOY0DAT(a0),(a1)+
        move.w  JOY1DAT(a0),(a1)+
.vpos:
        move.w  VPOSR(a0),(a1)+                ; V8 is bit 0 (SPECIFICATION.md 4.5, PJ2)
        ifd     PJ3TEST
        cmp.w   #TOTAL_SAMPLES-1-8,d6          ; PJ3 contrast: after sample 8 (inside phase 0's
        bne.s   .nojt                          ; baseline window) the program writes all four
        move.w  #$0404,JOYTEST(a0)             ; joystick-mouse counters at once
.nojt:
        endif
        dbra    d6,.loop

        DONE
