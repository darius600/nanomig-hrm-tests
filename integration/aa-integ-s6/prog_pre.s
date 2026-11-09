; prog_pre.s -- the prologue of RP-1 (SPECIFICATION.md 3.9, 6): code that runs before the test program.
; The test points the reset PC here. It sets COP1LC to the one-instruction list at $088100, strobes
; COPJMP1, enables Copper DMA and jumps to PROGRAM_ORG, where prog_main.s starts as in every other run.
        include "nanomig.i"

COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088

        org     $080200
        lea     CUSTOM,a0
        move.w  #$0008,COP1LCH(a0)
        move.w  #$8100,COP1LCL(a0)
        move.w  d0,COPJMP1(a0)
        move.w  #$8280,DMACON(a0)
        jmp     PROGRAM_ORG
