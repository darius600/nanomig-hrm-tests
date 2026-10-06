; prog_t2s_fixturemut.s -- prog_t2s.s with MUT_FIXTURE set: the FIXTURE-CONST line's
; own contrasting case (KFIX stores $DEAD instead of $BEEF).
MUT_FIXTURE     equ     1
        include "prog_t2s.s"
