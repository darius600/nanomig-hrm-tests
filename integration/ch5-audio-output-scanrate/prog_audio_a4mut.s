; prog_audio_a4mut.s -- prog_audio.s with MUT_A4 set: precondition A4's
; contrasting case, demonstrated on T1 (AUD0VOL = 0 from seven lines after the
; DMA starts, p.266 "000000=no output": the output goes flat inside the
; window, so L has few entries while the runs before it keep their length).
MUT_A4  equ     1
        include "prog_audio.s"
