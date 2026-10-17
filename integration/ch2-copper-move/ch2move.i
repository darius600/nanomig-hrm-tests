; ch2move.i -- definitions and macros shared by the programs of ch2-copper-move.
; Register offsets are those of the Hardware Reference Manual's Appendix B,
; Register Summary Address Order, from CUSTOM ($DFF000). Every access to a
; custom register or to a Chip-memory word of the specification uses an
; absolute address, so that the assembler listing alone resolves every data
; access of the program (SPECIFICATION.md Section 6).

COPCON          equ     $02E
HTOTAL          equ     $1C0
BEAMCON0        equ     $1DC
DSKPTH          equ     $020
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
AUD2LCH         equ     $0C0
AUD3LCH         equ     $0D0
BPL1PTH         equ     $0E0
BPLCON0         equ     $100
SPR0PTH         equ     $120
SPR0CTL         equ     $142

CIAAPRA         equ     $BFE001

; Marker word (SPECIFICATION.md Section 3): a CPU word write of a step number.
MARKER          equ     $04F000

; Regions that no list, data block or marker uses, where the pointers of the
; DMA channels that stay off are parked (SPECIFICATION.md Section 6).
BPL_PARK        equ     $04C000
DSK_PARK        equ     $04D000
SPR_PARK        equ     $04E000
AUD_PARK        equ     $044200

MARK            macro
                move.w  #\1,MARKER
                endm

; Baseline (Section 3). The first instruction clears the boot overlay: while it
; is active, a CPU write below $080000 reaches no memory and a CPU read there
; is served from the Kickstart window.
BASELINE        macro
                move.b  #0,CIAAPRA
                move.w  #$2700,sr
                move.w  #$7FFF,CUSTOM+DMACON
                move.w  #$7FFF,CUSTOM+INTENA
                move.w  #$7FFF,CUSTOM+INTREQ
                MARK    0
                endm

; Video registers (Section 3): BPLCON0 = $0200, SPRxCTL = 0 for x = 0..7.
VIDEOREGS       macro
                move.w  #$0200,CUSTOM+BPLCON0
                move.w  #$0000,CUSTOM+SPR0CTL
                move.w  #$0000,CUSTOM+SPR0CTL+8
                move.w  #$0000,CUSTOM+SPR0CTL+16
                move.w  #$0000,CUSTOM+SPR0CTL+24
                move.w  #$0000,CUSTOM+SPR0CTL+32
                move.w  #$0000,CUSTOM+SPR0CTL+40
                move.w  #$0000,CUSTOM+SPR0CTL+48
                move.w  #$0000,CUSTOM+SPR0CTL+56
                endm

; Pointers of the channels that stay off: bitplanes, disk, sprites and
; audio channels 1-3 point into regions of their own.
PARKPTRS        macro
                move.l  #BPL_PARK,CUSTOM+BPL1PTH
                move.l  #BPL_PARK,CUSTOM+BPL1PTH+4
                move.l  #BPL_PARK,CUSTOM+BPL1PTH+8
                move.l  #BPL_PARK,CUSTOM+BPL1PTH+12
                move.l  #BPL_PARK,CUSTOM+BPL1PTH+16
                move.l  #BPL_PARK,CUSTOM+BPL1PTH+20
                move.l  #DSK_PARK,CUSTOM+DSKPTH
                move.l  #SPR_PARK,CUSTOM+SPR0PTH
                move.l  #SPR_PARK,CUSTOM+SPR0PTH+4
                move.l  #SPR_PARK,CUSTOM+SPR0PTH+8
                move.l  #SPR_PARK,CUSTOM+SPR0PTH+12
                move.l  #SPR_PARK,CUSTOM+SPR0PTH+16
                move.l  #SPR_PARK,CUSTOM+SPR0PTH+20
                move.l  #SPR_PARK,CUSTOM+SPR0PTH+24
                move.l  #SPR_PARK,CUSTOM+SPR0PTH+28
                move.l  #AUD_PARK,CUSTOM+AUD1LCH
                move.l  #AUD_PARK,CUSTOM+AUD2LCH
                move.l  #AUD_PARK,CUSTOM+AUD3LCH
                endm

; S(T), Section 3, up to and including the enable marker (marker 3).
COPSTART        macro
                move.l  #\1,CUSTOM+COP1LCH
                MARK    1
                move.w  #0,CUSTOM+COPJMP1
                MARK    2
                move.w  #$8280,CUSTOM+DMACON
                MARK    3
                endm

; Waits for the beam to enter and then leave line \1 of the field (a line
; number above $38, so the low byte of VHPOSR names the line without V8).
; The byte read at the even address $DFF006 is VHPOSR bits 15-8, V7-V0.
WAITLINE        macro
.in\@           move.b  CUSTOM+VHPOSR,d0
                cmp.b   #\1,d0
                bne.s   .in\@
.out\@          move.b  CUSTOM+VHPOSR,d0
                cmp.b   #\1,d0
                beq.s   .out\@
                endm

; Idles for \1 passages of the beam through line $A0.
IDLEFIELDS      macro
                rept    \1
                WAITLINE $A0
                endr
                endm
