; prog_h_h1_aud0.s -- X-AUD0 mutation: H1 (N=$00B0, B=$08A0) with audio
; channel 0 DMA started. Must fail A2: an audio fetch is a DMA access at
; the port that is not memory refresh, so a graded period's group no
; longer has exactly 4 members.
N               equ     $00B0
B               equ     $08A0
AUD0_ON         equ     1
        include "prog_h.s"
