; p2.s -- P2 (SPECIFICATION.md section 4): the 68000's data and address
; registers, and the user stack pointer, hold 32 bits. Loads all 17
; registers with a two-different-halves pattern, stores each as a 32-bit
; write to its own table slot, and reads back USP too.
;
; Optional assembly-time symbols:
;   MUT_P2LOW  every register is loaded with its pattern's lower half,
;              zero-extended (P2's own contrasting case, P2-LOW)
;   MUT_A      step 8's S_B write is made twice; the window stays usable
;              (P2.a's contrast)
;   MUT_B      the store of D3 (M_D3) is left out (P2.b's contrast)
;   MUT_C      a trap-0 handler (RTE only) is installed and TRAP #0 is
;              executed between steps 6 and 7 (P2.c's contrast)
;   MUT_RD     a TST.W of a word outside the program image and outside
;              SAVE_SP is made between steps 6 and 7: a read inside the
;              window that is neither a program-image read nor the read of
;              SAVE_SP (P2.c's read clause contrast)
;   MUT_VEC    a TST.W of vector word $0010 (not word 0) is made between
;              steps 6 and 7: a read inside the window in the vector area
;              (P2.c's vector-area contrast)
;   MUT_C4POP  the S_A and S_B writes are left out (C4's empty-population
;              contrast)
;   MUT_C1/MUT_C2/MUT_C3/MUT_C4  common preconditions' contrasts (ch1.i)
;   MUT_NODONE/MUT_PEEK  fixture contrasts (ch1.i)

        include "nanomig.i"
        include "ch1.i"

        org     PROGRAM_ORG

start:
        SETUP

        ifd     MUT_C
        lea     TRAP0H(pc),a0
        move.l  a0,$80                  ; vector 32 (TRAP #0), 68000 instruction set
        endc

; ---- step 1 ------------------------------------------------------------
        move.l  a7,SAVE_SP

; ---- step 2: USP ---------------------------------------------------
        ifd     MUT_P2LOW
        movea.l #$0000E0F0,a0
        else
        movea.l #$C0D0E0F0,a0
        endc
        move.l  a0,usp

; ---- step 3: load every register, A7 last ---------------------------
        ifd     MUT_P2LOW
        move.l  #$00003040,d0
        move.l  #$00003141,d1
        move.l  #$00003242,d2
        move.l  #$00003343,d3
        move.l  #$00003444,d4
        move.l  #$00003545,d5
        move.l  #$00003646,d6
        move.l  #$00003747,d7
        movea.l #$0000A0B0,a0
        movea.l #$0000A1B0,a1
        movea.l #$0000A2B0,a2
        movea.l #$0000A3B0,a3
        movea.l #$0000A4B0,a4
        movea.l #$0000A5B0,a5
        movea.l #$0000A6B0,a6
        movea.l #$0000A7B0,a7
        else
        move.l  #$10203040,d0
        move.l  #$11213141,d1
        move.l  #$12223242,d2
        move.l  #$13233343,d3
        move.l  #$14243444,d4
        move.l  #$15253545,d5
        move.l  #$16263646,d6
        move.l  #$17273747,d7
        movea.l #$8090A0B0,a0
        movea.l #$8191A1B0,a1
        movea.l #$8292A2B0,a2
        movea.l #$8393A3B0,a3
        movea.l #$8494A4B0,a4
        movea.l #$8595A5B0,a5
        movea.l #$8696A6B0,a6
        movea.l #$8797A7B0,a7
        endc

; ---- step 4 -----------------------------------------------------------
        ifnd    MUT_C4POP
        move.w  #$A200,S_A
        endc

; ---- step 5: store each register --------------------------------------
        move.l  d0,M_BASE+0
        move.l  d1,M_BASE+4
        move.l  d2,M_BASE+8
        ifnd    MUT_B
        move.l  d3,M_BASE+12
        endc
        move.l  d4,M_BASE+16
        move.l  d5,M_BASE+20
        move.l  d6,M_BASE+24
        move.l  d7,M_BASE+28
        move.l  a0,M_BASE+32
        move.l  a1,M_BASE+36
        move.l  a2,M_BASE+40
        move.l  a3,M_BASE+44
        move.l  a4,M_BASE+48
        move.l  a5,M_BASE+52
        move.l  a6,M_BASE+56
        move.l  a7,M_BASE+60

; ---- step 6: restore A7 -------------------------------------------------
        movea.l SAVE_SP,a7

        ifd     MUT_C
        trap    #0
        endc
        ifd     MUT_RD
        tst.w   (T_BASE+16).l           ; P2.c contrast: a data read inside the window
        endc
        ifd     MUT_VEC
        tst.w   ($0010).w               ; P2.c contrast: a read of vector word $0010 inside the window
        endc

; ---- step 7: USP --------------------------------------------------------
        move.l  usp,a0
        move.l  a0,M_USP

; ---- step 8 --------------------------------------------------------------
        ifnd    MUT_C4POP
        move.w  #$B200,S_B
        endc
        ifd     MUT_A
        move.w  #$B200,S_B              ; P2.a contrast: a second window-close marker
        endc

        ENDING

        ifd     MUT_C
TRAP0H: rte
        endc
