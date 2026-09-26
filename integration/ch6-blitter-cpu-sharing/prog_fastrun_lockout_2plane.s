; prog_fastrun_lockout_2plane.s -- prog_fastrun_lockout.s with FR_2PLANE
; set: two high-resolution bitplanes (BPLCON0=$A000) instead of four, same
; DDFSTRT/DDFSTOP. This program's own precondition MUTATION
; (NMH_MUTATE=FASTRUN-LOCKOUT-PRE, in place of prog_fastrun_lockout.bin);
; see prog_fastrun_lockout.s's header for the whole design and what this
; substitution predicts.

FR_2PLANE equ     1
        include "prog_fastrun_lockout.s"
