; prog_c6_shift.s -- run C6s (SELFTEST-P7-SHIFT, SPECIFICATION.md P7.6): as C6, but list L5s -- every WAIT of the loop is one line lower (WAIT i at VP=$41+i). Compared with C5 on the unmodified L5, every Copper access from WAIT 0 on lies one line lower at the same in-line cycle (p.22), so a comparison that subtracts each segment's own first-access line finds nothing and the comparator of P7, which counts lines from the frame's restart line, finds every segment different; K6, T1, T2 and T3 still hold.

L5_VPSHIFT    equ     1
        include "prog_c6.s"
