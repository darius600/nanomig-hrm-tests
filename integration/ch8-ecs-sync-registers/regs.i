; regs.i -- custom registers this package needs beyond nanomig.i, from the
; Hardware Reference Manual's Appendix B (Register Summary, Address Order,
; p.287-293) and Appendix C (p.298). Offsets from CUSTOM ($DFF000).

HTOTAL          equ     $1C0    ; p.280,p.298,p.304: highest count, horiz. line (VARBEAMEN=1)
HSSTOP          equ     $1C2    ; p.280,p.298,p.304: horizontal line position for HSYNC stop
HSSTRT          equ     $1DE    ; p.280,p.298,p.304: horizontal sync start (VARHSY)
VTOTAL          equ     $1C8    ; p.298,p.304: line number at which to reset the vertical position counter
BEAMCON0        equ     $1DC    ; p.280,p.293,p.298,p.305: beam counter control register
DIWSTRT         equ     $08E    ; p.59: display window start
DIWSTOP         equ     $090    ; p.59: display window stop
DDFSTRT         equ     $092    ; p.60: display data fetch start
DDFSTOP         equ     $094    ; p.60: display data fetch stop
BPLCON0         equ     $100    ; p.63,p.216: bitplane control register 0
BPLCON1         equ     $102    ; p.63: bitplane control register 1
BPLCON2         equ     $104    ; p.63: bitplane control register 2
BPL1MOD         equ     $108    ; p.63: bitplane modulo, odd planes
BPL1PTH         equ     $0E0    ; p.63: bitplane 1 pointer, high word
BPL1PTL         equ     $0E2    ; p.63: bitplane 1 pointer, low word
COP1LCH         equ     $080    ; p.30: Copper first location register, high word
COP1LCL         equ     $082    ; p.30: Copper first location register, low word
COPJMP1         equ     $088    ; p.30: Copper restart strobe ("causes copper to load PC from COP1LC")
COLOR01         equ     $182    ; p.63: colour table entry 1
