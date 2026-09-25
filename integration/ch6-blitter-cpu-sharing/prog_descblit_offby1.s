; prog_descblit_offby1.s -- ch6-desc.c12's own contrast: prog_descblit.s
; with OFFBY1 set. BLTAPTH/BLTDPTH point one word past the block's last
; word (the 680x0 -(An) convention) instead of at the last word itself.

OFFBY1  equ     1
        include "prog_descblit.s"
