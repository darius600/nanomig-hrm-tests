; prog_p3_off_aud0.s -- P3 Case AUD0: DMACON = $83FE (bit 0, AUD0EN, clear).
; See prog_p3_base.s.
P3_OFF_AUD0     equ     1
        include "prog_p3_base.s"
