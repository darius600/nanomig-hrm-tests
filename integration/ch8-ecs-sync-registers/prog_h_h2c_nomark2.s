; prog_h_h2c_nomark2.s -- X-NOMARK2-ALL mutation: H2c (N=$00C0, B=$0820) with
; MARK2's write omitted. MARK1 appears and nothing follows it: PC-MARK must fail through its
; "MARK2 later" clause alone, while the rest of the program, including DONE, still runs.
N               equ     $00C0
B               equ     $0820
MARK2_OMIT      equ     1
        include "prog_h.s"
