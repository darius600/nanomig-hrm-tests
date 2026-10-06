; ch5.i -- register and memory-map constants for this package, beyond what
; nanomig.i already defines. Appendix B (Hardware Reference Manual, 3rd
; edition) offsets from CUSTOM ($DFF000).
;
; G, R and MK (SPECIFICATION.md section 2.3: a CPU log area, a results
; area, a marker word) sit at $78000, $7C000 and $7D000, and every DMA
; block sits at $60000-$75FFF: all below $80000, inside the ROM overlay
; window. "An additional copy of the system ROM responds starting at
; memory location $00000000. The system RAM that would normally be
; located at this address is not available. ... The OVL bit in one of the
; 8520 Chips disables the overlay" (p.223). Every program in this package
; therefore clears the overlay first (CLEAR_OVL below), before its first
; write to G, R or MK.

                include "nanomig.i"

; ---- ROM overlay (p.223; Appendix F, p.339) ----------------------------
CIAADDRA        equ     $BFE201         ; CIA-A ddra (p.339)
CIAAPRA         equ     $BFE001         ; CIA-A pra; bit 0 is OVL (p.339)

; Clears the ROM overlay: ddra=$03 makes port A bit 0 an output (p.339,
; "Direction for port A (BFE001);1=output (set to 0x03)"), then pra=$00
; clears OVL, "the OVL bit in one of the 8520 Chips" that "disables the
; overlay" (p.223). Every program below calls this before its first write
; to G, R or MK, and before anything else that needs chip RAM below
; $80000.
CLEAR_OVL       macro
                move.b  #$03,CIAADDRA
                move.b  #$00,CIAAPRA
                endm

; ---- registers not in nanomig.i --------------------------------------
ADKCON          equ     $09E
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
DSKLEN          equ     $024
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
COPCON          equ     $02E
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
BPLCON0         equ     $100
BPLCON1         equ     $102
BPL1MOD         equ     $108
SPR0PTH         equ     $120

; ---- DMA-block memory map (SPECIFICATION.md section 2.3) --------------
A0              equ     $60000
A1              equ     $61000
A2              equ     $62000
A3              equ     $63000
A0L             equ     A0+$0100
A1L             equ     A1+$0100
A2L             equ     A2+$0100
A3L             equ     A3+$0100
LST             equ     $64000          ; L (Copper list); L is a vasm-reserved-ish name, avoided
LST2            equ     $64100          ; an end-of-list location the "list ran" mutations point COP1LC at
S0              equ     $65000
N1              equ     $65100
N2              equ     $65200
N3              equ     $65300
N4              equ     $65400
N5              equ     $65500
N6              equ     $65600
N7              equ     $65700
FW              equ     $66000          ; F
ZB              equ     $66100
PBM             equ     $68000          ; P (bitplane area)
A0H             equ     $70000
A1H             equ     $71000
A2H             equ     $72000
A3H             equ     $73000
B0              equ     $70800
B1              equ     $71800
B2              equ     $72800
B3              equ     $73800
QQ              equ     $74000          ; Q
QQ2             equ     $75000          ; Q2

; P7's blitter sources, three blocks of $2000 bytes (4096 words each,
; SPECIFICATION.md P7), above the overlay window and clear of the program,
; its stack and RESULTS.
SRCA            equ     $A0000
SRCB            equ     $A2000
SRCC            equ     $A4000

; ---- CPU bookkeeping (SPECIFICATION.md section 2.3) --------------------
GLOG            equ     $78000          ; G
RSTORE          equ     $7C000          ; R
MK              equ     $7D000          ; MK
