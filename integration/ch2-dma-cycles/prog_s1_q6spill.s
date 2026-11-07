; prog_s1_q6spill.s -- contrast for Q6 (S1-q6spill, SPECIFICATION.md P12.6): S1 with list L_S1c whose WAIT is VP=$3F, HP=$E0 (IR1 $3FE1, mask $FFFE), the last position of the line before use 1, followed by COLOR17-19 <- $0FF0,$00FF,$0F0F.

S1_C         equ     1
S1_WAIT      equ     $3FE1
        include "prog_s1.s"
