; prog_p4_f1nolpen.s -- P4-PRE-LATCHF1 contrast: BPLCON0 LPEN is clear during F1
; only (cleared at E60 of F1, set again after Y), so the nominal F1 pulse
; latches nothing and Y reads the live beam; F3 is as in the normal build.
F1NOLPEN        equ     1
        include "prog_p4.s"
