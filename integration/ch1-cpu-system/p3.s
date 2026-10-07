; p3.s -- P3 (SPECIFICATION.md section 5): every 68000-family member reads
; instruction words ahead of the one it executes. Four blocks, each ending
; in a one-word stop instruction that (in this run) never falls through to
; the guard word right after it (W_k = X_k+2); a CPU read at W_k inside
; the run is only explainable by prefetch. Runs unmodified on the 68000,
; 68010 and 68020 (cpu_config selects the mode at run time; P6 reuses this
; same mechanism with a longer guard).
;
; Optional assembly-time symbols:
;   MUT_A    block 2's landing marker is written twice (P3.a's contrast:
;            one S_P and one S_L write per block)
;   MUT_ORD  block 2's progress marker is written inside block 1, so the
;            markers run P1,P2,L1,L2,... (P3.a's contrast: the complete
;            order P1<L1<P2<L2<...)
;   MUT_B    block 1's BRA.S targets an instruction behind its first guard
;            word; the guard's S_X store runs and the guard branches to
;            L_1 instead of the failure path (P3.b's contrast)
;   MUT_C    a TST.W W_3 (absolute-long data read of the guard word) is
;            inserted before block 3 (P3.c's contrast)
;   MUT_CPC  a MOVE.W W_3(PC),D2 (PC-relative data read of the guard word)
;            is inserted before block 3 (P3.c's contrast)
;   MUT_CREG a LEA W_3(PC),A1 and a TST.W (A1) (register-indirect data
;            read of the guard word) are inserted before block 3 (P3.c's
;            contrast)
;   MUT_D    block 1's stop instruction is assembled as BRA.W instead of
;            BRA.S (P3.d's contrast)
;   MUT_WIN  the S_END write is made right after the setup, so the run
;            window closes before the first block (the main relation's
;            contrast)
;   MUT_CAL  a TST.W of the far word Z is inserted before the ending (the
;            calibration's contrast)
;   MUT_C1/MUT_C2/MUT_C3/MUT_C4  common preconditions' contrasts (ch1.i)
;   MUT_NODONE/MUT_PEEK  fixture contrasts (ch1.i)

        include "nanomig.i"
        include "ch1.i"

        org     PROGRAM_ORG

start:
        SETUP
        ifd     MUT_WIN
        move.w  #$E0D0,S_END
        endc

; ---- block 1: BRA.S --------------------------------------------------
        move.w  #1,S_P
        ifd     MUT_ORD
        move.w  #2,S_P
        endc
        ifd     MUT_B
X1:     bra.s   W1X
W1:     nop
W1X:    move.w  #$DEAD,S_X
        bra.w   L1
        else
X1:     ifd     MUT_D
        bra.w   L1
        else
        bra.s   L1
        endc
W1:     move.w  #$DEAD,S_X
        bra.w   FAILRUN
        endc
L1:     move.w  #1,S_L
        lea     X1(pc),a6
        move.l  a6,XTAB3+0

; ---- block 2: JMP (A0) -------------------------------------------------
        ifnd    MUT_ORD
        move.w  #2,S_P
        endc
        lea     L2(pc),a0
X2:     jmp     (a0)
W2:     move.w  #$DEAD,S_X
        bra.w   FAILRUN
L2:     move.w  #2,S_L
        ifd     MUT_A
        move.w  #2,S_L                 ; P3.a contrast: a second landing marker
        endc
        lea     X2(pc),a6
        move.l  a6,XTAB3+4

; ---- block 3: RTS -------------------------------------------------------
        ifd     MUT_C
        tst.w   W3                     ; P3.c contrast: stray operand read of a guard word
        endc
        ifd     MUT_CPC
        move.w  W3(pc),d2              ; P3.c contrast: PC-relative operand read of a guard word
        endc
        ifd     MUT_CREG
        lea     W3(pc),a1
        tst.w   (a1)                   ; P3.c contrast: register-indirect operand read of a guard word
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

        ifd     MUT_CAL
        tst.w   (FARZ).l               ; calibration contrast: a CPU read of the far word Z
        endc
        ENDING
