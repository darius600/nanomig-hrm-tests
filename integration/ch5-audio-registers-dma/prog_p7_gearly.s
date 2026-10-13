; prog_p7_gearly.s -- P7-G(CPU-still-wanted-the-bus)'s mutation (stimulus
; red): the CPU's logging loop runs before the BLTSIZE write and the CPU then
; waits for the blit to end without writing G, so no CPU write into G follows
; T_last. The blit completes and the program still finishes.

GEARLY          equ     1
        include "prog_p7.s"
