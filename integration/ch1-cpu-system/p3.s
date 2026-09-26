; p3.s -- P3 (SPECIFICATION.md section 5): every 68000-family member reads
; instruction words ahead of the one it executes. Four blocks, each ending
; in a one-word stop instruction that (in this run) never falls through to
; the guard word right after it (W_k = X_k+2); a CPU read at W_k inside
; the run is only explainable by prefetch. Runs unmodified on the 68000,
; 68010 and 68020 (cpu_config selects the mode at run time; P6 reuses this
; same mechanism with a longer guard).
;
; Optional assembly-time symbols:
;   MUT_NOP  block 1's BRA.S is replaced by NOP (P3.a's and P3.b's shared
;            contrast: falls through into the guard)
;   MUT_C    a TST.W W_3 (data read of the guard word) is inserted before
;            block 3 (P3.c's contrast)
;   MUT_D    block 1's stop instruction is assembled as BRA.W instead of
;            BRA.S (P3.d's contrast)
;   MUT_C1/MUT_C2/MUT_C3/MUT_C4  common preconditions' contrasts (ch1.i)

        include "nanomig.i"
        include "ch1.i"

        org     PROGRAM_ORG

start:
        SETUP

; ---- block 1: BRA.S --------------------------------------------------
        move.w  #1,S_P
X1:     ifd     MUT_NOP
        nop
        else
        ifd     MUT_D
        bra.w   L1
        else
        bra.s   L1
        endc
        endc
W1:     move.w  #$DEAD,S_X
        bra.w   FAILRUN
L1:     move.w  #1,S_L
        lea     X1(pc),a6
        move.l  a6,XTAB3+0

; ---- block 2: JMP (A0) -------------------------------------------------
        move.w  #2,S_P
        lea     L2(pc),a0
X2:     jmp     (a0)
W2:     move.w  #$DEAD,S_X
        bra.w   FAILRUN
L2:     move.w  #2,S_L
        lea     X2(pc),a6
        move.l  a6,XTAB3+4

; ---- block 3: RTS -------------------------------------------------------
        ifd     MUT_C
        tst.w   W3                     ; P3.c contrast: stray operand read of a guard word
        endc
        move.w  #3,S_P
        pea     L3(pc)
X3:     rts
W3:     move.w  #$DEAD,S_X
        bra.w   FAILRUN
L3:     move.w  #3,S_L
        lea     X3(pc),a6
        move.l  a6,XTAB3+8

; ---- block 4: BEQ.S (Z set by CMP.W D0,D0) ------------------------------
        move.w  #4,S_P
        cmp.w   d0,d0
X4:     beq.s   L4
W4:     move.w  #$DEAD,S_X
        bra.w   FAILRUN
L4:     move.w  #4,S_L
        lea     X4(pc),a6
        move.l  a6,XTAB3+12

        ENDING
