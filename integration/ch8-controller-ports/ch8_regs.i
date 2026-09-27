; ch8_regs.i -- register offsets and helpers this package's own programs use,
; beyond what nanomig.i already defines. Not part of the harness contract:
; this package's own file. Offsets come from Appendix B (custom chip
; registers, p.289-291) and from Appendix A / p.229 / p.241 (CIA-A), as
; SPECIFICATION.md's register table cites.

; ---- custom chip registers, offsets from CUSTOM (a0 holds CUSTOM) --------
JOY0DAT         equ     $00A
JOY1DAT         equ     $00C
POTGOR          equ     $016    ; read; printed page also calls it POTINP
BPLCON0         equ     $100
POTGO           equ     $034    ; write

; ---- CIA-A, absolute addresses (not custom-chip-relative) ----------------
CIAAPRA         equ     $BFE001 ; R -- bit6 port1 fire, bit7 port2 fire (p.232)
CIAADDRA        equ     $BFE201 ; R/W -- direction mask for CIAAPRA (p.241)

; ---- V (beam position), p.238: VPOSR bit0=V8, VHPOSR bits15-8=V7-V0. -----
; VPOSR/VHPOSR are adjacent registers ($DFF004/$DFF006), read as one 32-bit
; word at VPOSR (SPECIFICATION.md, "Beam position").
;
; READV: out d1.w = V (0-511). Clobbers d0. a0 must hold CUSTOM.
;
; Reads VPOSR/VHPOSR as one long word (p.238's own technique, one bus
; cycle for both), then extracts with SWAP (4 cycles) and word shifts
; instead of the longword shifts an earlier version used: those cost
; 8+2n cycles each on a real 68000 (24 cycles per 8-bit .l shift) and,
; with two V reads per R-step iteration (SPECIFICATION.md's R step),
; left too little of one scan line's ~1820 clk_sys cycles between Vr0
; and Vr1 -- measured directly: PC1's own R-step yield (a genuine
; measurement, not a value from the RTL) rose from 12/18 qualifying R
; steps to the version this macro now is once this was cut.
READV           macro
                move.l  VPOSR(a0),d0    ; d0 = {VPOSR:16, VHPOSR:16}
                move.w  d0,d1
                lsr.w   #8,d1           ; d1 = VHPOSR[15:8] = V7-V0
                swap    d0              ; d0 = {VHPOSR:16, VPOSR:16}
                and.w   #1,d0           ; d0.w = VPOSR[0] = V8
                lsl.w   #8,d0
                or.w    d0,d1           ; d1 = V
                endm

; WAITV target: blocks until V==\1. Clobbers d0,d1. a0 must hold CUSTOM.
WAITV           macro
.wv\@:          READV
                cmp.w   #\1,d1
                bne.s   .wv\@
                endm

; S1 (SPECIFICATION.md "Setup, at the start of every program"): clears every
; DMACON/INTENA enable. a0 must hold CUSTOM.
SETUP_S1        macro
                move.w  #$7FFF,INTENA(a0)
                move.w  #$7FFF,DMACON(a0)
                endm

; S2: BPLCON0<-0 so LPEN=0 and VPOSR is not latched (p.239). a0 = CUSTOM.
SETUP_S2        macro
                move.w  #$0000,BPLCON0(a0)
                endm
