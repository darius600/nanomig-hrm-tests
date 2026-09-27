; prog_p1_k1.s -- prog_p1.s with K1 set: every Fc write withholds bit 0
; (START), so no dump is ever requested. See SPECIFICATION.md section 6,
; K1.

K1      equ     1
        include "prog_p1.s"
