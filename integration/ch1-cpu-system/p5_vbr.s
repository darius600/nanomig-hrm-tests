; p5_vbr.s -- p5.s with MUT_VBR set (SPECIFICATION.md section 7, step 3):
; MOVEC D0,VBR points the vectors at address 0, needed for the second
; core in 68010 and 68020 mode (same binary, both modes; only cpu_config
; differs between those two runs).
MUT_VBR         equ     1
        include "p5.s"
