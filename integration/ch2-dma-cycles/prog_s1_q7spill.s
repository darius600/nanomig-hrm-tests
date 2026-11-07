; prog_s1_q7spill.s -- contrast for Q7 (S1-q7spill, SPECIFICATION.md P12-EXC.6): S1c with list L_S1c whose WAIT is VP=$4F, HP=$E0 (IR1 $4FE1, mask $FFFE), the last position of the line before use 2, followed by COLOR17-19 <- $0FF0,$00FF,$0F0F.

S1_C         equ     1
S1_WAIT      equ     $4FE1
        include "prog_s1.s"
