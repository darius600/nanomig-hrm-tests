; ch2move.i -- custom-chip register offsets this package uses beyond
; nanomig.i's own list (DMACONR, VPOSR, VHPOSR, INTENAR, DMACON, INTENA,
; INTREQ), taken from the Hardware Reference Manual's Appendix B, Register
; Summary Address Order. Offsets from CUSTOM ($DFF000).

COPCON          equ     $02E
ADKCON          equ     $09E

COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
COP2LCH         equ     $084
COP2LCL         equ     $086
COPJMP2         equ     $08A

AUD0LCH         equ     $0A0
AUD0LCL         equ     $0A2
AUD0LEN         equ     $0A4
AUD0PER         equ     $0A6
AUD0VOL         equ     $0A8
AUD1LCH         equ     $0B0
AUD1LCL         equ     $0B2

; Common frame steps (SPECIFICATION.md 4.3, steps 1-2), shared by every
; program in this package. A0 is CUSTOM ($DFF000) throughout.
FRAME_INIT      macro
                move.w  #$2700,SR
                lea     CUSTOM,a0
                move.w  #$7FFF,INTENA(a0)
                move.w  #$7FFF,INTREQ(a0)
                move.w  #$7FFF,DMACON(a0)
                move.w  #$7FFF,ADKCON(a0)
                move.w  #$0000,COPCON(a0)
                endm
