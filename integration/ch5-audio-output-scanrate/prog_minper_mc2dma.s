; prog_minper_mc2dma.s -- prog_minper.s with MUT_MC2DMA set: precondition MC2's contrasting case: channel 3 is set up on a second table and DMACON is $8209 while the run declares DMAEN + AUD0EN.
MUT_MC2DMA  equ     1
        include "prog_minper.s"
