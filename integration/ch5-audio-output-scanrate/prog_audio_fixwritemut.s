; prog_audio_fixwritemut.s -- prog_audio.s with MUT_FIXWRITE set: the
; FIXTURE-WRITE line's own contrasting case (the store of the ADKCONR read
; into its result slot is left out, so the port shows no CPU write there).
MUT_FIXWRITE    equ     1
        include "prog_audio.s"
