; prog_minper_mc3f4.s -- prog_minper.s with MUT_MC3F4 set: precondition MC3's contrasting case (anchor F4): after MUT_LINE lines of the wait channel 0 is stopped and channel 1, set up on the same table, is started, so the table fetches of the port window come in the audio slot of channel 0 and then in that of channel 1.
MUT_LINE    equ     1140
MUT_MC3F4  equ     1
        include "prog_minper.s"
