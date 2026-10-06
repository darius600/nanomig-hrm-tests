; s2.i -- constants shared by the programs of this package.
;
; MARKER is the marker area: a program announces an event by storing the
; event code, one word, to it. The port shows that write with its time.

MARKER          equ     $0C1000

; event codes
M_FRAME         equ     $F001           ; VERTB seen, cleared (frame sync)
M_BLIT_GO       equ     $A100           ; + blit index
M_BLIT_DONE     equ     $A200           ; + blit index

; custom registers the shared nanomig.i does not define
BPLCON0         equ     $100
COPCON          equ     $02E
BEAMCON0        equ     $1DC
HHPOSR          equ     $1DA
HTOTAL          equ     $1C0
HSSTOP          equ     $1C2
HBSTRT          equ     $1C4
HBSTOP          equ     $1C6
VTOTAL          equ     $1C8
VSSTOP          equ     $1CA
VBSTRT          equ     $1CC
VBSTOP          equ     $1CE
SPRHSTRT        equ     $1D0
SPRHSTOP        equ     $1D2
BPLHSTRT        equ     $1D4
BPLHSTOP        equ     $1D6
HSSTRT          equ     $1DE
VSSTRT          equ     $1E0
HCENTER         equ     $1E2
BPLHMOD         equ     $1E6
SPRHPTH         equ     $1E8
SPRHPTL         equ     $1EA
BPLHPTH         equ     $1EC
BPLHPTL         equ     $1EE
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088

; blit table entry (see blit.i)
BLIT_HDR        macro                   ; flags, BLTCON0, BLTCON1, BLTSIZE
                dc.w    \1,\2,\3,\4
                endm
BLIT_PTR        macro                   ; C, B, A, D pointers (byte addresses)
                dc.w    (\1)>>16,(\1)&$FFFF
                dc.w    (\2)>>16,(\2)&$FFFF
                dc.w    (\3)>>16,(\3)&$FFFF
                dc.w    (\4)>>16,(\4)&$FFFF
                endm
BLIT_PTRW       macro                   ; C, B, A, D as PTH, PTL word pairs
                dc.w    \1,\2,\3,\4
                dc.w    \5,\6,\7,\8
                endm
BLIT_MOD        macro                   ; C, B, A, D modulos
                dc.w    \1,\2,\3,\4
                endm
