; prog_audio_a1mut.s -- prog_audio.s with MUT_A1 set: precondition A1's
; contrasting case, demonstrated on T1: AUD0EN is set as in the normal run
; and cleared again after MUT_A1LINES lines, so the DMA reads T1 for a few
; passes and then stops, short of the N + 3 reads of its first word.
MUT_A1LINES equ 20
MUT_A1  equ     1
        include "prog_audio.s"
