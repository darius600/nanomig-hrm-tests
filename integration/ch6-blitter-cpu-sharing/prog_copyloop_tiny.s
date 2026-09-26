; prog_copyloop_tiny.s -- ORDER mutation: prog_copyloop.s with CL_SHORT
; set, shrinking the CPU copy loop to 50 words (a nonzero span shorter
; than the nasty-mode blit's own 500-word copy) so it finishes before the
; blit -- the unchanged "blit finishes first" ordering check must go red.

CL_SHORT equ    1
        include "prog_copyloop.s"
