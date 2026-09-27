; prog_joy.s -- the joystick program shared by P2, P3 and P4
; (SPECIFICATION.md, "The joystick program (shared by P2, P3 and P4)").
;
; This single binary serves the baseline run AND every one of P2/P3/P4's
; contrasting cases (Q1-Q4, P2's variant and anchor contrast, P3's
; variant, P4's variant): every one of those changes only which harness
; port level is applied for a given step, never the program's own code, so
; test_ch8_controller_ports.cpp selects the mutation by choosing a
; different pin-level schedule for this same binary, not by building a
; different program.
;
; Protocol (one iteration per step k = 0..NSTEPS-1):
;   1. store k to JOY_MARK (S_k: "the harness may now apply step k's
;      levels").
;   2. wait until V has changed twice (at least one full line passes).
;   3. store k to JOY_MARK again (T_k).
;   4. read JOY0DAT, JOY1DAT, CIAAPRA (byte, zero-extended) and POTGOR;
;      store all four, in that order (record k's first store is U_k).

        include "nanomig.i"
        include "ch8_regs.i"
        org     PROGRAM_ORG

JOY_MARK        equ     $0C1C00
JOY_RESULTS     equ     $0C2000
NSTEPS          equ     54

start:
        lea     CUSTOM,a0
        SETUP_S1
        SETUP_S2
        move.w  #$FF00,POTGO(a0)       ; p.241's recipe: OUT=1,DAT=1 on all four (no START)
        move.b  CIAADDRA,d0
        and.b   #$3F,d0                ; clear DDRA bits 6,7: both fire bits inputs
        move.b  d0,CIAADDRA
        lea     JOY_MARK,a2
        lea     JOY_RESULTS,a1
        moveq   #0,d5

.step:
        move.w  d5,(a2)                 ; S_k
        READV
        move.w  d1,d6                   ; V0
.wait1: READV
        cmp.w   d6,d1
        beq.s   .wait1
        move.w  d1,d6                   ; V1 (changed once)
.wait2: READV
        cmp.w   d6,d1
        beq.s   .wait2                  ; changed twice: >= one full line passed
        move.w  d5,(a2)                 ; T_k
        move.w  JOY0DAT(a0),(a1)+
        move.w  JOY1DAT(a0),(a1)+
        moveq   #0,d0
        move.b  CIAAPRA,d0
        move.w  d0,(a1)+
        move.w  POTGOR(a0),(a1)+
        addq.w  #1,d5
        cmp.w   #NSTEPS,d5
        bne     .step

        DONE
