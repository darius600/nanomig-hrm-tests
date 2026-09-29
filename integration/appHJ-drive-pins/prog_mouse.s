; prog_mouse.s -- P1-P4 (SPECIFICATION.md section 4): a single continuous
; sampling loop over four back-to-back phases (M0H, M0V, M1H, M1V). The
; harness supplies the pin schedule of SPECIFICATION.md 4.2 by changing
; joystick0/joystick1 while this loop runs; the program itself is
; schedule-agnostic, it only samples one triple per line change
; (SPECIFICATION.md 3.3) for TOTAL_SAMPLES lines, covering all four
; 512-line phases (16+192+288+16 each), then stops.
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
        move.b  d0,d7
        move.w  VHPOSR(a0),(a1)+
        move.w  JOY0DAT(a0),(a1)+
        move.w  JOY1DAT(a0),(a1)+
        dbra    d6,.loop

        DONE
