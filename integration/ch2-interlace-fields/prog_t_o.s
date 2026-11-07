; prog_t_o.s -- Program T, variant T-o (SPECIFICATION.md 4.4): the WAIT of line 256 uses
; H=$40, so its mark lies early in line 256 and the lengths of lines 255 and 256 change. Contrasting
; case for precondition PT-9 (the offset of the Copper marks of the lines 255 and above).
T_LINE256_40    equ     1
                include "prog_t.s"
