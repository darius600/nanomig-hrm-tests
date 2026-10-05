; prog_r1_c1prime.s -- display-anchor contrast: prog_r1.s with COLOR00
; preset to C1'=$0C3F in step a instead of C1 (section 5.1.6), and RESET at
; A replaced by NOP so that the run completes. F_ref ends before t_M1, so
; the NOP does not reach the anchor comparator.

MUT_C1PRIME equ 1
MUT_NORESET equ 1
        include "prog_r1.s"
