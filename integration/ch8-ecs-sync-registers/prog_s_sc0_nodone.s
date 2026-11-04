; prog_s_sc0_nodone.s -- X-NODONE-ALL mutation: SC0 (BCON=$00A0, s=$0010, e=$0040)
; that holds in place after MARK1 without storing the DONE value. PC-ID and PC-MARK hold;
; only the DONE line must fail. The display DMA keeps running.
BCON            equ     $00A0
SVAL            equ     $0010
EVAL            equ     $0040
NO_DONE         equ     1
        include "prog_s.s"
