; prog_audio_a2mut.s -- prog_audio.s with MUT_A2 set: precondition A2's
; contrasting case (ADKCON $8001, USE0V1, set before the ADKCONR read;
; bit 0 then reads 1).
MUT_A2  equ     1
        include "prog_audio.s"
