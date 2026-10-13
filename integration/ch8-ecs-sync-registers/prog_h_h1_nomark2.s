; prog_h_h1_nomark2.s -- X-NOMARK2 mutation: H1 (N=$00B0, B=$08A0) with
; MARK2's write omitted. MARK1 appears and nothing follows it: PC-MARK [H1]
; must fail through its "MARK2 later" clause alone, while the rest of the
; program, including DONE, still runs.
N               equ     $00B0
B               equ     $08A0
MARK2_OMIT      equ     1
        include "prog_h.s"
