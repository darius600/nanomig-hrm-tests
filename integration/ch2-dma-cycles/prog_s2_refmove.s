; prog_s2_refmove.s -- contrast for Q-REF (SPECIFICATION.md P8.6): S2 whose list L_B points sprite 2 at a copy of R with HSTART $98 instead of $90, so the reference is at another left edge in L_B frames.

S2_REFMOVE         equ     1
        include "prog_s2.s"
