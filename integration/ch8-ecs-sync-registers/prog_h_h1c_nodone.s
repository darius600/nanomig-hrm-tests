; prog_h_h1c_nodone.s -- X-NODONE-ALL mutation: H1c (N=$00B0, B=$0820) that holds in
; place after MARK2 without storing the DONE value. PC-ID and PC-MARK hold; only the DONE
; line must fail.
N               equ     $00B0
B               equ     $0820
NO_DONE         equ     1
        include "prog_h.s"
