; prog_t_s.s -- Program T, variant T-s (SPECIFICATION.md 4.4): T-n with the lines 51 to 60 writing
; the value of the line before them, so every field holds two lines that repeat the length of the line
; before (the lines 51 and 61) and the parity of the lines is back after line 61. Stimulus record that
; separates P3 from P3(pattern) (selector P3(interior)).
T_BEAM          equ     1
T_CHAIN         equ     1
CH_SKIP         equ     1
                include "prog_t.s"
