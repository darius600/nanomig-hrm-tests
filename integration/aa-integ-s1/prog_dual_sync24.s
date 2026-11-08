; prog_dual_sync24.s -- prog_dual_sync.s with the window at $24-$33: the live input of P14-U1 (a counter that restarts at HTOTAL = $0040).
SYNCLO  equ     $24
        include "prog_dual_sync.s"
