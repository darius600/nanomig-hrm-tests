; prog_i_p.s -- Program I, variant I-p (SPECIFICATION.md 3.5): BPL1MOD =
; BPL2MOD = 38. Both lists still write PIC and PIC + 40, so a B-field starts
; 40 bytes past an A-field, but the next A-group no longer starts 40 bytes
; past the B-group before it. Contrasting case for P4d.
I_MOD38         equ     1
                include "prog_i.s"
