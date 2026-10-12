; prog_c17.s -- R5's cross-frame contrast (SPECIFICATION.md 3.8, C17): the
; program stores a mark word after the second of its frame waits; the C++
; side then changes the sprite data words. NMH_MUTATE=C17.

MUT_C17 equ     1
        include "prog.s"
