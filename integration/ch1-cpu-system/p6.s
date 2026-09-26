; p6.s -- P6 (SPECIFICATION.md section 8): the 68020 prefetches more words
; than the 68000. Same mechanism as P3 (a one-word stop instruction never
; falls through into the guard right after it), but the four stop kinds
; each appear twice -- once at an address divisible by 4, once at
; remainder 2 -- and the guard is six words long (W_k to W_k+10), so
; g_k = the number of distinct guard words read inside R gives a lower
; bound on how far the processor loads ahead. D = the largest g_k over
; the eight blocks; SPECIFICATION.md compares D between cpu_config=68020
; and cpu_config=68000 on the same image (second core), and against the
; first core as the contrasting case.
;
; Each block stores its own stop instruction's address (X_k) into
; XTAB6+4*(k-1) via a self-reporting LEA+MOVE.L, exactly as p3.s does;
; the C++ test reads X_k back and classifies alignment (X_k mod 4) itself
; rather than this file guaranteeing a specific parity by hand -- a single
; one-word MOVEQ #0,D2 pad between each pair's two copies is enough to
; guarantee they differ in parity from each other, whichever way around.
;
; Optional assembly-time symbols:
;   MUT_E   every guard is shortened to a single word, DC.W $4AFC
;           (ILLEGAL), still never executed (P6.e's contrast)
;   MUT_C1/MUT_C2/MUT_C3/MUT_C4  common preconditions' contrasts (ch1.i)
;
; P3.a-P3.d's own contrasts (MUT_NOP, MUT_C [static-check], MUT_D) are not
; re-demonstrated here: SPECIFICATION.md section 8 says "the contrasts are
; the same as in P3", so p3.s's own mutants already cover them for this
; package (ch1-cpu-system's own scope decision, not a claim of P6's).

        include "nanomig.i"
        include "ch1.i"

; ---- one guard: six words, never executed, W_k = X_k+2 -----------------
GUARD           macro
                ifd     MUT_E
                dc.w    $4AFC
                else
                move.w  #$DEAD,S_X
                bra.w   FAILRUN
                endc
                endm

        org     PROGRAM_ORG

start:
        SETUP
        moveq   #3,d1                   ; DIVU's divisor, loaded once

; ---- blocks 1-2: BRA.S, aligned / remainder 2 --------------------------
        move.w  #1,S_P
        move.l  #$1000,d0
        divu.w  d1,d0
X1:     bra.s   L1
W1:     GUARD
L1:     move.w  #1,S_L
        lea     X1,a6
        move.l  a6,XTAB6+0

        moveq   #0,d2                   ; parity pad

        move.w  #2,S_P
        move.l  #$1000,d0
        divu.w  d1,d0
X2:     bra.s   L2
W2:     GUARD
L2:     move.w  #2,S_L
        lea     X2,a6
        move.l  a6,XTAB6+4

; ---- blocks 3-4: JMP (A0), aligned / remainder 2 -----------------------
        move.w  #3,S_P
        lea     L3,a0
        move.l  #$1000,d0
        divu.w  d1,d0
X3:     jmp     (a0)
W3:     GUARD
L3:     move.w  #3,S_L
        lea     X3,a6
        move.l  a6,XTAB6+8

        moveq   #0,d2

        move.w  #4,S_P
        lea     L4,a0
        move.l  #$1000,d0
        divu.w  d1,d0
X4:     jmp     (a0)
W4:     GUARD
L4:     move.w  #4,S_L
        lea     X4,a6
        move.l  a6,XTAB6+12

; ---- blocks 5-6: RTS, aligned / remainder 2 -----------------------------
        move.w  #5,S_P
        pea     L5
        move.l  #$1000,d0
        divu.w  d1,d0
X5:     rts
W5:     GUARD
L5:     move.w  #5,S_L
        lea     X5,a6
        move.l  a6,XTAB6+16

        moveq   #0,d2

        move.w  #6,S_P
        pea     L6
        move.l  #$1000,d0
        divu.w  d1,d0
X6:     rts
W6:     GUARD
L6:     move.w  #6,S_L
        lea     X6,a6
        move.l  a6,XTAB6+20

; ---- blocks 7-8: BEQ.S (Z set by CMP.W D0,D0), aligned / remainder 2 ----
        move.w  #7,S_P
        move.l  #$1000,d0
        divu.w  d1,d0
        cmp.w   d0,d0
X7:     beq.s   L7
W7:     GUARD
L7:     move.w  #7,S_L
        lea     X7,a6
        move.l  a6,XTAB6+24

        moveq   #0,d2

        move.w  #8,S_P
        move.l  #$1000,d0
        divu.w  d1,d0
        cmp.w   d0,d0
X8:     beq.s   L8
W8:     GUARD
L8:     move.w  #8,S_L
        lea     X8,a6
        move.l  a6,XTAB6+28

        ENDING
