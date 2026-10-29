; p7.s -- P7 (SPECIFICATION.md section 9): MOVE SR,<dest> acts differently
; on the 68010 and the 68020 than on the 68000. One image for every mode.
; A witness window W_w enters user mode and executes a privileged MOVE to
; SR, which raises the privilege-violation exception from user mode on every
; member; the moved-SR window W_m then enters user mode again and executes
; MOVE SR,DEST followed by TRAP #0. A member on which MOVE SR is
; unprivileged stores SR and reaches the TRAP handler; a member on which it
; raises the privilege-violation exception reaches the privilege handler.
; No handler returns through its frame.
;
; Optional assembly-time symbols:
;   MUT_A    the write of $E702 to S_A is left out (P7.a's contrast)
;   MUT_B    the instruction at X_m is MOVE.W SR,D2 (P7.b's contrast)
;   MUT_C    the witness instruction is TRAP #1 with HW also installed at
;            vector 33 (P7.c's contrast)
;   MUT_CTL  the instruction at X_m is MOVE.W D1,DEST (the same-mode
;            control's contrast)
;   MUT_C1/MUT_C2/MUT_C3/MUT_C4  common preconditions' contrasts (ch1.i)
;   MUT_C4POP  every S_A and S_B write is left out (C4's empty population)
;   MUT_NODONE/MUT_PEEK  fixture contrasts (ch1.i)

        include "nanomig.i"
        include "ch1.i"

        org     PROGRAM_ORG

start:
        SETUP

; ---- step 1: handlers, user stack pointer, destination prefill ----------
        lea     HW(pc),a0
        move.l  a0,$20                  ; vector 8: privilege violation
        lea     HT(pc),a0
        move.l  a0,$80                  ; vector 32: TRAP #0
        ifd     MUT_C
        lea     HW(pc),a0
        move.l  a0,$84                  ; vector 33: TRAP #1
        endc
        movea.l #USP_INIT,a0
        move.l  a0,usp
        move.w  #$5555,DEST

; ---- step 2: witness window W_w ------------------------------------------
        move.l  a7,PRE_SP
        ifnd    MUT_C4POP
        move.w  #$E701,S_A
        endc
        andi.w  #$DFFF,sr               ; user mode, condition codes kept
        ifd     MUT_C
XW:     trap    #1
        else
XW:     move.w  #$2700,sr               ; privileged in user mode
        endc
        bra.w   FAILRUN                 ; reached only if no exception was taken
HW:     ifnd    MUT_C4POP
        move.w  #$F701,S_B
        endc
        movea.l PRE_SP,a7
        bra.w   MPHASE

; ---- step 3: moved-SR window W_m -------------------------------------------
MPHASE: lea     HM(pc),a0
        move.l  a0,$20                  ; vector 8 now reaches HM
        move.l  a7,PRE_SP
        ifnd    MUT_A
        ifnd    MUT_C4POP
        move.w  #$E702,S_A
        endc
        endc
        andi.w  #$DFFF,sr
        ifd     MUT_B
XM:     move.w  sr,d2
        else
        ifd     MUT_CTL
XM:     move.w  d1,(DEST).w
        else
XM:     move.w  sr,(DEST).w
        endc
        endc
        trap    #0                      ; reached only if MOVE SR raised no exception
        bra.w   FAILRUN
HM:     move.w  #$7001,MRK_PRIV
        ifnd    MUT_C4POP
        move.w  #$F702,S_B
        endc
        movea.l PRE_SP,a7
        bra.w   CEND
HT:     move.w  #$7002,MRK_TRAP
        ifnd    MUT_C4POP
        move.w  #$F702,S_B
        endc
        movea.l PRE_SP,a7
        bra.w   CEND

; ---- step 4: X_w, X_m and the ending ---------------------------------------
CEND:   lea     XW(pc),a6
        move.l  a6,XTAB7
        lea     XM(pc),a6
        move.l  a6,XTAB7+4
        ENDING
