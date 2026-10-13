; prog_s_sa0_a5ref.s -- X-A5REF mutation: SA0 (BCON=$00A0, s=$0010,
; e=$0030) with audio channel 0 DMA started on a one-word buffer at Chip
; address 0. Must fail A5-REF alone: the Chip-word-0 reads of a line no
; longer form a group of exactly four.
BCON            equ     $00A0
SVAL            equ     $0010
EVAL            equ     $0030
AUD0_AT0       equ     1
        include "prog_s.s"
