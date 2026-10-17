; prog_s1_q2.s -- contrast for Q2 (SPECIFICATION.md 2.5): S1 whose use 1 has the low data word of its first line at $7FFF, so its leftmost pixel is transparent (p.108) and the run of non-background output pixels on that line is 15 sprite pixels wide.

S1_Q2         equ     1
        include "prog_s1.s"
