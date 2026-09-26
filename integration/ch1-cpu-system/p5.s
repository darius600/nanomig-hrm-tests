; p5.s -- P5 (SPECIFICATION.md section 7): the exception stack frame
; differs between 68000-family members. Installs one handler per
; exception (TRAP #0, illegal instruction, line 1010, TRAPV, privilege
; violation), runs the same five cases from the same starting state, and
; has each handler jump straight to the next case without returning
; through the frame. cpu_config selects 68000/68010/68020 at run time; the
; MUT_VBR image (p5_vbr.s) additionally points the vectors at VBR=0 via
; MOVEC, needed on the 68010/68020 runs (68000 instruction set has no
; MOVEC/VBR, so the plain image, run in 68000 mode, is P5's own 68000
; case and also the contrasting case against the first core).
;
; Optional assembly-time symbols:
;   MUT_VBR  MOVEQ #0,D0 / MOVEC D0,VBR replaces two NOPs (needed for the
;            68010/68020 runs; SPECIFICATION.md section 7 step 3)
;   MUT_A    case 1's TRAP #0 is replaced by NOP (P5.a's contrast)
;   MUT_B    SUBQ.L #4,A7 is inserted before case 1 (P5.b's contrast; used
;            together with MUT_VBR, "in the 68010 run")
;   MUT_C    case 2 gets an extra MOVE.W D0,S_P before its instruction
;            (P5.c's contrast)
;   MUT_C1/MUT_C2/MUT_C3/MUT_C4  common preconditions' contrasts (ch1.i)

        include "nanomig.i"
        include "ch1.i"

        org     PROGRAM_ORG

start:
        SETUP

; ---- step 2: install the five handlers ---------------------------------
        lea     H1,a0
        move.l  a0,$80                  ; vector 32: TRAP #0
        lea     H2,a0
        move.l  a0,$10                  ; vector 4: illegal instruction
        lea     H3,a0
        move.l  a0,$28                  ; vector 10: line 1010
        lea     H4,a0
        move.l  a0,$1C                  ; vector 7: TRAPV
        lea     H5,a0
        move.l  a0,$20                  ; vector 8: privilege violation

; ---- step 3: VBR (68010/68020 runs only) --------------------------------
        ifd     MUT_VBR
        moveq   #0,d0
        movec   d0,vbr
        else
        nop
        nop
        endc

; ---- step 4: USP --------------------------------------------------------
        movea.l #USP_INIT,a0
        move.l  a0,usp

        ifd     MUT_B
        subq.l  #4,a7                   ; P5.b contrast: PRE_SP differs from the other runs
        endc

; ---- case 1 (e=1): TRAP #0 ----------------------------------------------
        move.l  a7,PRE_SP
        move.w  #$C001,S_A
        move.w  #$0000,ccr
Y1:     ifd     MUT_A
        nop                             ; P5.a contrast: no exception is taken
        else
        trap    #0
        endc
        bra.w   FAILRUN
H1:     move.w  #$D001,S_B
        move.l  a7,POST_SP1
        movea.l PRE_SP,a7
        jmp     CASE2

; ---- case 2 (e=2): illegal instruction ----------------------------------
CASE2:
        move.l  a7,PRE_SP
        move.w  #$C002,S_A
        move.w  #$0000,ccr
        ifd     MUT_C
        move.w  d0,S_P                  ; P5.c contrast: a write outside the stack, inside W_2
        endc
Y2:     dc.w    $4AFC                   ; ILLEGAL
        bra.w   FAILRUN
H2:     move.w  #$D002,S_B
        move.l  a7,POST_SP2
        movea.l PRE_SP,a7
        jmp     CASE3

; ---- case 3 (e=3): line 1010 --------------------------------------------
CASE3:
        move.l  a7,PRE_SP
        move.w  #$C003,S_A
        move.w  #$0000,ccr
Y3:     dc.w    $A000
        bra.w   FAILRUN
H3:     move.w  #$D003,S_B
        move.l  a7,POST_SP3
        movea.l PRE_SP,a7
        jmp     CASE4

; ---- case 4 (e=4): TRAPV, V set -----------------------------------------
CASE4:
        move.l  a7,PRE_SP
        move.w  #$C004,S_A
        move.w  #$0002,ccr
Y4:     trapv
        bra.w   FAILRUN
H4:     move.w  #$D004,S_B
        move.l  a7,POST_SP4
        movea.l PRE_SP,a7
        jmp     CASE5

; ---- case 5 (e=5): privilege violation ----------------------------------
CASE5:
        move.l  a7,PRE_SP
        move.w  #$C005,S_A
        move.w  #$0000,ccr
        andi.w  #$DFFF,sr                ; enter user mode; condition codes kept
Y5:     move.w  #$2700,sr                ; privileged in user mode
        bra.w   FAILRUN
H5:     move.w  #$D005,S_B
        move.l  a7,POST_SP5
        movea.l PRE_SP,a7

        ENDING
