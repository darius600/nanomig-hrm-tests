; prog_p7_gearly.s -- P7-G(CPU-still-wanted-the-bus)'s mutation (stimulus
; red): the CPU's logging loop runs before the BLTSIZE write, so no CPU write
; into G follows T_last. The program still finishes.

GEARLY          equ     1
        include "prog_p7.s"
