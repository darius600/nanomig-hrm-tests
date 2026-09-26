; ch5regs.i -- register offsets and macros for ch5-audio-output-scanrate's
; own programs. Not part of the harness contract: this package's own file
; (like ch6-blitter-geometry/geom.i). Offsets from Appendix B (pp.289-293)
; and Appendix A (p.265); nanomig.i already defines DMACONR, VPOSR, VHPOSR,
; INTENAR, INTREQR, the blitter registers, DMACON, INTENA, INTREQ, COLOR00,
; so only the registers this package needs beyond that set are added here.

ADKCONR         equ     $010    ; R  Audio, disk control register read (p.289)
DSKBYTR         equ     $01A    ; R  Disk data byte and status read (p.289)
DSKPTH          equ     $020    ; W  Disk pointer, high 3 (5 if ECS) bits (p.289)
DSKPTL          equ     $022    ; W  Disk pointer, low 15 bits (p.289)
DSKLEN          equ     $024    ; W  Disk length (p.246, p.289)
ADKCON          equ     $09E    ; W  Audio, disk, UART control (p.265, p.290)
AUD0LCH         equ     $0A0    ; W  Audio channel 0 location, high bits (p.139, p.290)
AUD0LCL         equ     $0A2    ; W  Audio channel 0 location, low 15 bits (p.290)
AUD0LEN         equ     $0A4    ; W  Audio channel 0 length (p.266, p.290)
AUD0PER         equ     $0A6    ; W  Audio channel 0 period (p.143, p.266, p.290)
AUD0VOL         equ     $0A8    ; W  Audio channel 0 volume (p.140, p.266, p.290)
AUD1VOL         equ     $0B8    ; W  Audio channel 1 volume (p.266, p.290)
AUD2VOL         equ     $0C8    ; W  Audio channel 2 volume (p.266, p.290)
AUD3VOL         equ     $0D8    ; W  Audio channel 3 volume (p.266, p.290)
HTOTAL          equ     $1C0    ; W  ECS only (p.304)
VTOTAL          equ     $1C8    ; W  ECS only (p.304)
BEAMCON0        equ     $1DC    ; W  ECS only (p.304-305)

; CIA-A Port A data register. SPECIFICATION.md 2.8 explains why a single
; write here is needed: it is the only way this design makes chip RAM
; below $080000 (the range the audio/disk pointer registers must target,
; 2.1) real instead of the boot-time Kickstart-shadow window, and it is a
; CIA-A write only -- it does not touch CIA-B, so no floppy drive select
; line is ever driven (CIA-B is a separate chip-select, gary.v sel_cia_b).
CIAAPRA         equ     $BFE001

; Clear the overlay (any write to CIA-A; minimig.v: "ovl <= 0" on
; sel_cia_a, unconditional on the data written). Chip bank 0
; ($000000-$07FFFF) is otherwise gated "!ovl" (gary.v, cpuhlt is tied to
; 0 in this build, so the gate is exactly !ovl, for CPU and DMA access
; alike).
CLEAR_OVL       macro
                move.b  #0,CIAAPRA
                endm
