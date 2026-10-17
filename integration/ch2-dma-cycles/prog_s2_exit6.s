; prog_s2_exit6.s -- contrast for FRAMES (SPECIFICATION.md 2.5): S2 whose wait loop exits when FRAME_CTR reaches 6 instead of 7; fewer output frames fall between S4 and S7.

S2_EXIT6         equ     1
        include "prog_s2.s"
