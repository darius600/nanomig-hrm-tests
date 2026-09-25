; prog_copyloop_tiny.s -- ORDER mutation: prog_copyloop.s with CL_TINY
; set, shrinking the CPU copy loop to one word so it finishes before the
; nasty-mode blit's own 500-word copy -- the unchanged "blit finishes
; first" ordering check must go red.

CL_TINY equ     1
        include "prog_copyloop.s"
