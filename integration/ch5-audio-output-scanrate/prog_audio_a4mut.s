; prog_audio_a4mut.s -- prog_audio.s with MUT_A4 set: precondition A4's
; contrasting case, demonstrated on T1 (AUD0VOL = 0: p.266 "000000=no
; output", the output stays flat).
MUT_A4  equ     1
        include "prog_audio.s"
