; prog_s1_colv.s -- contrast for P12 (S1-colv, SPECIFICATION.md P12.6): S1 with list L_S1v, whose vertical-blanking part writes COLOR17-19 <- $0FF0,$00FF,$0F0F; nothing is written between the uses.

S1_COLV         equ     1
        include "prog_s1.s"
