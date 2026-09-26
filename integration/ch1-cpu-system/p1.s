; p1.s -- P1 (SPECIFICATION.md section 3): on the 68000, CLR reads its
; operand then writes it; P4 (section 6) reuses this same program on the
; second core in 68020 mode. 21 cases (c=1..21), every size and every
; memory-alterable destination mode, in the order SPECIFICATION.md's
; table gives. Registers: A0-A4 hold An per mode (reused per size group),
; D1 is Xn (always zero), A6 is scratch for the X_c self-report, A5 is
; unused.
;
; Each case stores the address of its own CLR/MOVE instruction (X_c) into
; XTAB+4*(c-1) via a PC-relative LEA, so the C++ test knows exactly where
; that case's opcode word lives in the assembled image, with no hand
; counting of instruction lengths.
;
; Optional assembly-time symbols:
;   MUT_MOVE  every CLR.x is replaced by MOVE.x #0 (P1's own contrasting
;             case, P1-MOVE)
;   MUT_A     case 5's S_A store (step 3) is left out (P1.a's contrast)
;   MUT_B     case 8's effective address is T_8+8 instead of T_8 (P1.b's
;             contrast)
;   MUT_C     case 11 is assembled as CLR.L instead of CLR.W (P1.c's
;             contrast)
;   MUT_C1/MUT_C2/MUT_C3/MUT_C4  common preconditions' contrasts (ch1.i)

        include "nanomig.i"
        include "ch1.i"

        org     PROGRAM_ORG

start:
        SETUP
        moveq   #0,d1                  ; Xn for every d8(An,Xn) case, always zero

; ---- case 1: .B (An), A0, target T1+1 --------------------------------
        move.w  #$A5A5,T_BASE+16
        lea     T_BASE+17,a0
        move.w  #$A001,S_A
X1:     ifd     MUT_MOVE
        move.b  #0,(a0)
        else
        clr.b   (a0)
        endc
        move.w  #$B001,S_B
        move.w  T_BASE+16,R_BASE+4
        lea     X1(pc),a6
        move.l  a6,XTAB+0

; ---- case 2: .B (An)+, A1, target T2 ----------------------------------
        move.w  #$A5A5,T_BASE+32
        lea     T_BASE+32,a1
        move.w  #$A002,S_A
X2:     ifd     MUT_MOVE
        move.b  #0,(a1)+
        else
        clr.b   (a1)+
        endc
        move.w  #$B002,S_B
        move.w  T_BASE+32,R_BASE+8
        lea     X2(pc),a6
        move.l  a6,XTAB+4

; ---- case 3: .B -(An), A2, target T3+1 --------------------------------
        move.w  #$A5A5,T_BASE+48
        lea     T_BASE+50,a2
        move.w  #$A003,S_A
X3:     ifd     MUT_MOVE
        move.b  #0,-(a2)
        else
        clr.b   -(a2)
        endc
        move.w  #$B003,S_B
        move.w  T_BASE+48,R_BASE+12
        lea     X3(pc),a6
        move.l  a6,XTAB+8

; ---- case 4: .B d16(An), A3, target T4 --------------------------------
        move.w  #$A5A5,T_BASE+64
        lea     T_BASE+60,a3
        move.w  #$A004,S_A
X4:     ifd     MUT_MOVE
        move.b  #0,4(a3)
        else
        clr.b   4(a3)
        endc
        move.w  #$B004,S_B
        move.w  T_BASE+64,R_BASE+16
        lea     X4(pc),a6
        move.l  a6,XTAB+12

; ---- case 5: .B d8(An,Xn), A4, target T5+1 ----------------------------
        move.w  #$A5A5,T_BASE+80
        lea     T_BASE+77,a4
        ifnd    MUT_A
        move.w  #$A005,S_A
        endc
X5:     ifd     MUT_MOVE
        move.b  #0,4(a4,d1.w)
        else
        clr.b   4(a4,d1.w)
        endc
        move.w  #$B005,S_B
        move.w  T_BASE+80,R_BASE+20
        lea     X5(pc),a6
        move.l  a6,XTAB+16

; ---- case 6: .B abs.W, target T6 --------------------------------------
        move.w  #$A5A5,T_BASE+96
        move.w  #$A006,S_A
X6:     ifd     MUT_MOVE
        move.b  #0,(T_BASE+96).w
        else
        clr.b   (T_BASE+96).w
        endc
        move.w  #$B006,S_B
        move.w  T_BASE+96,R_BASE+24
        lea     X6(pc),a6
        move.l  a6,XTAB+20

; ---- case 7: .B abs.L, target T7+1 ------------------------------------
        move.w  #$A5A5,T_BASE+112
        move.w  #$A007,S_A
X7:     ifd     MUT_MOVE
        move.b  #0,(T_BASE+113).l
        else
        clr.b   (T_BASE+113).l
        endc
        move.w  #$B007,S_B
        move.w  T_BASE+112,R_BASE+28
        lea     X7(pc),a6
        move.l  a6,XTAB+24

; ---- case 8: .W (An), A0, target T8 -----------------------------------
        move.w  #$A5A5,T_BASE+128
        ifd     MUT_B
        lea     T_BASE+128+8,a0        ; P1.b contrast: effective address T_8+8
        else
        lea     T_BASE+128,a0
        endc
        move.w  #$A008,S_A
X8:     ifd     MUT_MOVE
        move.w  #0,(a0)
        else
        clr.w   (a0)
        endc
        move.w  #$B008,S_B
        move.w  T_BASE+128,R_BASE+32
        lea     X8(pc),a6
        move.l  a6,XTAB+28

; ---- case 9: .W (An)+, A1, target T9 ----------------------------------
        move.w  #$A5A5,T_BASE+144
        lea     T_BASE+144,a1
        move.w  #$A009,S_A
X9:     ifd     MUT_MOVE
        move.w  #0,(a1)+
        else
        clr.w   (a1)+
        endc
        move.w  #$B009,S_B
        move.w  T_BASE+144,R_BASE+36
        lea     X9(pc),a6
        move.l  a6,XTAB+32

; ---- case 10: .W -(An), A2, target T10 --------------------------------
        move.w  #$A5A5,T_BASE+160
        lea     T_BASE+162,a2
        move.w  #$A00A,S_A
X10:    ifd     MUT_MOVE
        move.w  #0,-(a2)
        else
        clr.w   -(a2)
        endc
        move.w  #$B00A,S_B
        move.w  T_BASE+160,R_BASE+40
        lea     X10(pc),a6
        move.l  a6,XTAB+36

; ---- case 11: .W d16(An), A3, target T11 ------------------------------
        move.w  #$A5A5,T_BASE+176
        lea     T_BASE+172,a3
        move.w  #$A00B,S_A
X11:    ifd     MUT_MOVE
        move.w  #0,4(a3)
        else
        ifd     MUT_C
        clr.l   4(a3)                  ; P1.c contrast: assembled as CLR.L, table still names CLR.W
        else
        clr.w   4(a3)
        endc
        endc
        move.w  #$B00B,S_B
        move.w  T_BASE+176,R_BASE+44
        lea     X11(pc),a6
        move.l  a6,XTAB+40

; ---- case 12: .W d8(An,Xn), A4, target T12 ----------------------------
        move.w  #$A5A5,T_BASE+192
        lea     T_BASE+188,a4
        move.w  #$A00C,S_A
X12:    ifd     MUT_MOVE
        move.w  #0,4(a4,d1.w)
        else
        clr.w   4(a4,d1.w)
        endc
        move.w  #$B00C,S_B
        move.w  T_BASE+192,R_BASE+48
        lea     X12(pc),a6
        move.l  a6,XTAB+44

; ---- case 13: .W abs.W, target T13 ------------------------------------
        move.w  #$A5A5,T_BASE+208
        move.w  #$A00D,S_A
X13:    ifd     MUT_MOVE
        move.w  #0,(T_BASE+208).w
        else
        clr.w   (T_BASE+208).w
        endc
        move.w  #$B00D,S_B
        move.w  T_BASE+208,R_BASE+52
        lea     X13(pc),a6
        move.l  a6,XTAB+48

; ---- case 14: .W abs.L, target T14 ------------------------------------
        move.w  #$A5A5,T_BASE+224
        move.w  #$A00E,S_A
X14:    ifd     MUT_MOVE
        move.w  #0,(T_BASE+224).l
        else
        clr.w   (T_BASE+224).l
        endc
        move.w  #$B00E,S_B
        move.w  T_BASE+224,R_BASE+56
        lea     X14(pc),a6
        move.l  a6,XTAB+52

; ---- case 15: .L (An), A0, target T15 ---------------------------------
        move.w  #$A5A5,T_BASE+240
        move.w  #$5A5A,T_BASE+242
        lea     T_BASE+240,a0
        move.w  #$A00F,S_A
X15:    ifd     MUT_MOVE
        move.l  #0,(a0)
        else
        clr.l   (a0)
        endc
        move.w  #$B00F,S_B
        move.w  T_BASE+240,R_BASE+60
        move.w  T_BASE+242,R_BASE+62
        lea     X15(pc),a6
        move.l  a6,XTAB+56

; ---- case 16: .L (An)+, A1, target T16 --------------------------------
        move.w  #$A5A5,T_BASE+256
        move.w  #$5A5A,T_BASE+258
        lea     T_BASE+256,a1
        move.w  #$A010,S_A
X16:    ifd     MUT_MOVE
        move.l  #0,(a1)+
        else
        clr.l   (a1)+
        endc
        move.w  #$B010,S_B
        move.w  T_BASE+256,R_BASE+64
        move.w  T_BASE+258,R_BASE+66
        lea     X16(pc),a6
        move.l  a6,XTAB+60

; ---- case 17: .L -(An), A2, target T17 --------------------------------
        move.w  #$A5A5,T_BASE+272
        move.w  #$5A5A,T_BASE+274
        lea     T_BASE+276,a2
        move.w  #$A011,S_A
X17:    ifd     MUT_MOVE
        move.l  #0,-(a2)
        else
        clr.l   -(a2)
        endc
        move.w  #$B011,S_B
        move.w  T_BASE+272,R_BASE+68
        move.w  T_BASE+274,R_BASE+70
        lea     X17(pc),a6
        move.l  a6,XTAB+64

; ---- case 18: .L d16(An), A3, target T18 ------------------------------
        move.w  #$A5A5,T_BASE+288
        move.w  #$5A5A,T_BASE+290
        lea     T_BASE+284,a3
        move.w  #$A012,S_A
X18:    ifd     MUT_MOVE
        move.l  #0,4(a3)
        else
        clr.l   4(a3)
        endc
        move.w  #$B012,S_B
        move.w  T_BASE+288,R_BASE+72
        move.w  T_BASE+290,R_BASE+74
        lea     X18(pc),a6
        move.l  a6,XTAB+68

; ---- case 19: .L d8(An,Xn), A4, target T19 -----------------------------
        move.w  #$A5A5,T_BASE+304
        move.w  #$5A5A,T_BASE+306
        lea     T_BASE+300,a4
        move.w  #$A013,S_A
X19:    ifd     MUT_MOVE
        move.l  #0,4(a4,d1.w)
        else
        clr.l   4(a4,d1.w)
        endc
        move.w  #$B013,S_B
        move.w  T_BASE+304,R_BASE+76
        move.w  T_BASE+306,R_BASE+78
        lea     X19(pc),a6
        move.l  a6,XTAB+72

; ---- case 20: .L abs.W, target T20 -------------------------------------
        move.w  #$A5A5,T_BASE+320
        move.w  #$5A5A,T_BASE+322
        move.w  #$A014,S_A
X20:    ifd     MUT_MOVE
        move.l  #0,(T_BASE+320).w
        else
        clr.l   (T_BASE+320).w
        endc
        move.w  #$B014,S_B
        move.w  T_BASE+320,R_BASE+80
        move.w  T_BASE+322,R_BASE+82
        lea     X20(pc),a6
        move.l  a6,XTAB+76

; ---- case 21: .L abs.L, target T21 -------------------------------------
        move.w  #$A5A5,T_BASE+336
        move.w  #$5A5A,T_BASE+338
        move.w  #$A015,S_A
X21:    ifd     MUT_MOVE
        move.l  #0,(T_BASE+336).l
        else
        clr.l   (T_BASE+336).l
        endc
        move.w  #$B015,S_B
        move.w  T_BASE+336,R_BASE+84
        move.w  T_BASE+338,R_BASE+86
        lea     X21(pc),a6
        move.l  a6,XTAB+80

        ENDING
