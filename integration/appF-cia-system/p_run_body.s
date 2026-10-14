; p_run_body.s -- P7.3/P8.3 (bit 3, RUNMODE): the start value (leg O, one-
; shot, vs. leg C, continuous) is read from the parameter block (E7), so
; the same assembled program serves both legs; the C++ side pokes PARAM_A's
; low byte before boot. Generic body shared by CRA (timer A) and CRB
; (timer B), on both CIAs.
;
; Wrapper symbols: CR,TLO,THI,ICR,ICRMASK. Optional: MUT_PAD (Pre7.3's own
; contrast: a loop body padded beyond 20 E periods), MUT_SHORT (399 samples
; instead of 400).
;
; SPECIFICATION.md P7.3/P8.3.

PRUN_ICR        equ     RESULTS+8            ; 400 words
PRUN_C          equ     PRUN_ICR+400*2
PRUN_U          equ     PRUN_C+2

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CR
        RLATCH  TLO,THI,40
        moveq   #0,d0
        move.b  ICR,d0                  ; discard (E3 already cleared it; this is the leg's own read)
        move.b  PARAM_A+1,CR            ; leg O: $09  leg C: $01 (E7)
        ifd     MUT_PAD
        RSAMPLE1_PAD 400,PRUN_ICR,ICR,150
        else
        ifd     MUT_SHORT
        RSAMPLE1 399,PRUN_ICR,ICR
        else
        RSAMPLE1 400,PRUN_ICR,ICR
        endc
        endc
        READ_STORE CR,PRUN_C
        READ_TIMER THI,TLO,PRUN_U

        FINISH
