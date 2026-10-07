; p6_mutd.s -- p6.s with MUT_D set: the reused P3.d's contrast (SPECIFICATION.md section 8): block 1 is assembled with BRA.W while the table names BRA.S; the pair's pad NOP is dropped so the pair keeps opposite parity.
MUT_D           equ     1
        include "p6.s"
