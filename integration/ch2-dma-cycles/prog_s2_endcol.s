; prog_s2_endcol.s -- contrast for Q5 (SPECIFICATION.md P8.6): S2 with END placed in L_A after the colour MOVEs and before its COP1LC pair; the pair is not fetched, so the restarts do not alternate.

S2_ENDCOL         equ     1
        include "prog_s2.s"
