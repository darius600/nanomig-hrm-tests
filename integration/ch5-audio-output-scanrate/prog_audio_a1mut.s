; prog_audio_a1mut.s -- prog_audio.s with MUT_A1 set: precondition A1's
; contrasting case, demonstrated on T1 (DMACON $8200 instead of $8201:
; AUD0EN never set, so no audio reads occur for T1's pass).
MUT_A1  equ     1
        include "prog_audio.s"
