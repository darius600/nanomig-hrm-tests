; prog_dual_sync70.s -- prog_dual_sync.s with the window at $50-$5F: the live input of P14-U2 (a counter that restarts at HTOTAL = $0070).
SYNCLO  equ     $50
        include "prog_dual_sync.s"
