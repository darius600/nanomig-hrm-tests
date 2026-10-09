; copbeam.i -- shared constants and macros for ch2-copper-beam's own
; programs. Package-local; not part of the harness contract (nanomig.i is).
;
; Custom registers Appendix B gives but nanomig.i does not define, needed by
; this package's own CPU-side (a0-relative) accesses. Copper-list content is
; written as literal hex per SPECIFICATION.md 3.3 and never needs these
; symbols.
COPCON          equ     $02E
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
BPLCON0         equ     $100
BPLCON1         equ     $102
BPLCON2         equ     $104
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
BPL1MOD         equ     $108
COLOR01         equ     $182

CIAADDRA        equ     $BFE201
CIAAPRA         equ     $BFE001

; Common prologue (SPECIFICATION.md 3.4, steps 1-2). a0 must already hold
; CUSTOM.
FRAME_INIT      macro
                move.w  #$7FFF,DMACON(a0)
                move.w  #$7FFF,INTENA(a0)
                move.b  #$03,CIAADDRA
                move.b  #0,CIAAPRA
                ifnd    RUN_NOFIX
                move.w  #$A5C3,RESULTS+2        ; fixture constant
                endif
                endm

; Memory layout (SPECIFICATION.md 3.6).
LIST_A_BASE     equ     $010000
TRACE_BASE      equ     $100000
