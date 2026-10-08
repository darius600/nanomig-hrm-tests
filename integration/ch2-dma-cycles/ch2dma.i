; ch2dma.i -- constants shared by every ch2-dma-cycles program.
;
; Extends nanomig.i (which this file does not duplicate) with the custom
; registers nanomig.i does not define, from the Hardware Reference Manual's
; Appendix B, Register Summary Address Order, and with the fixed chip-
; address layout every program's own S1 step builds (ch2build_macros.i/
; ch2build_code.i) and every program's S2/S3 register writes point at. The
; addresses below are below $080000, reachable to a CPU write only once
; the boot overlay is released (ch2_skel.i's CLEAR_OVL, run by S0). They
; must match the constants of the same name in test_ch2_dma_cycles.cpp.

; ---- custom registers not already in nanomig.i -----------------------

DSKPTH          equ     $020
DSKPTL          equ     $022
DSKLEN          equ     $024
DSKBYTR         equ     $01A
ADKCON          equ     $09E

HTOTAL          equ     $1C0
BEAMCON0        equ     $1DC

COP1LCH         equ     $080
COP1LCL         equ     $082
COP2LCH         equ     $084
COP2LCL         equ     $086
COPJMP1         equ     $088
COPJMP2         equ     $08A

DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094

AUD0LCH         equ     $0A0
AUD0LCL         equ     $0A2
AUD0LEN         equ     $0A4
AUD0PER         equ     $0A6
AUD0VOL         equ     $0A8
AUD1LCH         equ     $0B0
AUD1LCL         equ     $0B2
AUD1LEN         equ     $0B4
AUD1PER         equ     $0B6
AUD1VOL         equ     $0B8
AUD2LCH         equ     $0C0
AUD2LCL         equ     $0C2
AUD2LEN         equ     $0C4
AUD2PER         equ     $0C6
AUD2VOL         equ     $0C8
AUD3LCH         equ     $0D0
AUD3LCL         equ     $0D2
AUD3LEN         equ     $0D4
AUD3PER         equ     $0D6
AUD3VOL         equ     $0D8

BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
BPL2PTH         equ     $0E4
BPL2PTL         equ     $0E6
BPL3PTH         equ     $0E8
BPL3PTL         equ     $0EA
BPL4PTH         equ     $0EC
BPL4PTL         equ     $0EE
BPL5PTH         equ     $0F0
BPL5PTL         equ     $0F2
BPL6PTH         equ     $0F4
BPL6PTL         equ     $0F6

SPR0PTH         equ     $120
SPR0PTL         equ     $122
SPR1PTH         equ     $124
SPR1PTL         equ     $126
SPR2PTH         equ     $128
SPR2PTL         equ     $12A
SPR3PTH         equ     $12C
SPR3PTL         equ     $12E
SPR4PTH         equ     $130
SPR4PTL         equ     $132
SPR5PTH         equ     $134
SPR5PTL         equ     $136
SPR6PTH         equ     $138
SPR6PTL         equ     $13A
SPR7PTH         equ     $13C
SPR7PTL         equ     $13E

BPLCON0         equ     $100
BPLCON1         equ     $102
BPL1MOD         equ     $108
BPL2MOD         equ     $10A

COLOR17         equ     $1A2
COLOR18         equ     $1A4
COLOR19         equ     $1A6
COLOR21         equ     $1AA
COLOR22         equ     $1AC
COLOR23         equ     $1AE

; ---- DMACON/INTENA/INTREQ bits (Appendix B) ---------------------------

DMAEN           equ     $0200
BPLEN           equ     $0100
COPEN           equ     $0080
BLTEN           equ     $0040
SPREN           equ     $0020
DSKEN           equ     $0010
AUD0EN          equ     $0001
AUD1EN          equ     $0002
AUD2EN          equ     $0004
AUD3EN          equ     $0008
AUD0123EN       equ     $000F

INT_SET         equ     $8000
INT_INTEN       equ     $4000
INT_VERTB       equ     $0020

; ---- results area (RESULTS=$0C0000, nanomig.i) -------------------------
; FRAME_CTR is both the live vblank counter the handler increments and the
; final stored result (the same word); RES_DSKBYTR and DISK_FLAG are
; scratch/result cells at fixed offsets, so the C++ side reads them the
; same way regardless of which config ran.

FRAME_CTR       equ     RESULTS+0       ; word: vblank counter, live and final
RES_DSKBYTR     equ     RESULTS+2       ; word: stored DSKBYTR (disk runs only, else 0)
DISK_FLAG       equ     RESULTS+4       ; byte: "disk start sequence already run"

; ---- chip-address layout ($000000-$07FFFF, DMA-only, see above) -------
; Every block below is disjoint and padded (>=64 bytes zero on both sides,
; see SPECIFICATION.md S2.7) by construction of the gaps between the
; addresses. The C++ side pokes nothing (every runner passes nopoke): each
; program builds its own contents by CPU writes. S0 clears the CIA-A boot
; overlay, quiesces DMA, writes the level-3 vector and clears the results
; area; S1 writes the run's Copper list(s) and sprite structures; only then
; do S3/S4 point COP1LC at the list and enable Copper and the channel DMA.
; S2/S3 write the run's register settings and pointer values; no block contents.

COP_MAIN        equ     $001000         ; the run's main Copper list (L0/L3/L4/L5); reserve 4KB
COP_SECOND      equ     $002000         ; a second list some runs need (L5b); reserve 2KB
CH_AUD0         equ     $003000
CH_AUD1         equ     $003200
CH_AUD2         equ     $003400
CH_AUD3         equ     $003600
CH_DISK         equ     $004000         ; 8000 words (16000 bytes); reserve 16KB before CH_SPR0
CH_SPR0         equ     $008000
CH_SPR1         equ     $008400
CH_SPR2         equ     $008800
CH_SPR3         equ     $008C00
CH_SPR4         equ     $009000
CH_SPR5         equ     $009400
CH_SPR6         equ     $009800
CH_SPR7         equ     $009C00
CH_SPR_NULL     equ     $00A000
CH_U_SUBLIST    equ     $00B000         ; outside every declared block (D7/§2.7)
CH_BPL1         equ     $010000
CH_BPL2         equ     $018000
CH_BPL3         equ     $020000
CH_BPL4         equ     $028000
CH_BPL5         equ     $030000
CH_BPL6         equ     $038000
