; prog_h_h1_nodone.s -- X-NODONE mutation: H1 (N=$00B0, B=$08A0) that holds
; in place after MARK2 without storing the DONE value. PC-ID and PC-MARK
; hold; only the DONE line must fail.
N               equ     $00B0
B               equ     $08A0
NO_DONE         equ     1
        include "prog_h.s"
