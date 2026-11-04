; prog_s_sb0_nodone.s -- X-NODONE-ALL mutation: SB0 (BCON=$00A0, s=$0020, e=$0030)
; that holds in place after MARK1 without storing the DONE value. PC-ID and PC-MARK hold;
; only the DONE line must fail. The display DMA keeps running.
BCON            equ     $00A0
SVAL            equ     $0020
EVAL            equ     $0030
NO_DONE         equ     1
        include "prog_s.s"
