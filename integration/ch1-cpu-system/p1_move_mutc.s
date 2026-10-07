; p1_move_mutc.s -- p1.s with MUT_MOVE and MUT_C set: the MOVE contrast's P1.c contrast: case 11 is assembled as MOVE.L #0 while the table names MOVE.W.
MUT_MOVE        equ     1
MUT_C           equ     1
        include "p1.s"
