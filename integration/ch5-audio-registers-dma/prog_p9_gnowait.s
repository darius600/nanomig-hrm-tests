; prog_p9_gnowait.s -- P9's "after completion" precondition mutation (stimulus red): the wait
; moves behind Z_1, so Z_1 is read straight after the dummy read.

G_NOWAIT        equ     1
X_NOWAIT        equ     1
        include "prog_p9.s"
