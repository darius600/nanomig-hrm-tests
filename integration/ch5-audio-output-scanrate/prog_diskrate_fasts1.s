; prog_diskrate_fasts1.s -- prog_diskrate.s with MUT_FASTS1 set: run S's
; ADKCON write becomes $8100 instead of $0100. Used for both G2's own
; contrasting case (run S actually runs at the fast bit-cell time) and
; G3's own contrasting case (both runs now at the same speed) -- the
; C++ test grades this same capture both ways.
MUT_FASTS1      equ     1
        include "prog_diskrate.s"
