; prog_descblit_offby2.s -- DESC-OFFBY1's own mutation: prog_descblit.s
; with OFFBY2 set. BLTAPTH/BLTDPTH point TWO words past the block's last
; word (neither the blitter's own convention nor the 680x0 -(An)
; convention) -- the unchanged "writes exactly $0C2102-$0C2108, with
; $0C210A unwritten" check must go red: this capture instead writes
; $0C2104-$0C210A, leaving both $0C2100 and $0C2102 unwritten.

OFFBY2  equ     1
        include "prog_descblit.s"
