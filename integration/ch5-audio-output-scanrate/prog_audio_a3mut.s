; prog_audio_a3mut.s -- prog_audio.s with MUT_A3 set: precondition A3's
; contrasting case, demonstrated on T1 (AUD0PER = 129 instead of 128: the
; held levels last 1032 samples, not the programmed 1024).
MUT_A3  equ     1
        include "prog_audio.s"
