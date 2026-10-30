; prog_s1_q2p.s -- contrast for Q2 (S1-q2p, SPECIFICATION.md 2.5): S1 whose use 1 has the low data word of its first line at $FF00, so its right-hand eight pixels are transparent (p.108) and the run of non-background output pixels on that line is eight sprite pixels wide: 16 output pixels, divisible by 16, although only half of the sixteen sprite pixels are set.

S1_Q2P         equ     1
        include "prog_s1.s"
