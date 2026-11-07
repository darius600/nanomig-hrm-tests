; MUTATION support: P1-a contrast, configuration C -- two VERTB detections after
; the Copper start, then one write to VHPOSW that moves H sixteen counts on with
; V unchanged (PAL, 16 colours, high resolution).
PALETTE16       equ     1
HIRES_MODE      equ     1
MUT_SHIFT       equ     1
        include "prog_playfield_core.s"
