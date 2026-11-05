; p9_legf_ciab_ta_cputoggle.s -- see ctl_toggle_body.s. CIAB/timer A, L=40.
LATCHVAL        equ     40
INITLEVEL       equ     $FF
CTLM            equ     $40
CTLCR           equ     $BFDE00       ; CIAB_CRA
CTLLO           equ     $BFD400       ; CIAB_TALO
CTLHI           equ     $BFD500       ; CIAB_TAHI
CTLFLAG         equ     0
                include "ctl_toggle_body.s"
