; prog_bitplane.s -- A9/A10/A11 [p196,197]: bitplane-DMA bandwidth
; pressure on the CPU. Enables bitplane DMA at a chosen plane count and
; resolution, and runs forever the same counting loop prog_sharing.s
; uses; the test tells bitplane-DMA activity apart from the CPU's own
; accesses by address (BP_PTR vs BP_SCRATCH). This file alone (no symbol
; set) is the four low-resolution-bitplane build (A9's baseline).
; prog_bitplane_hires.s (BP_HIRES4 set): four high-resolution bitplanes
; (A10). prog_bitplane_planes6.s (BP_PLANES6 set): six low-resolution
; bitplanes (A11).
;
; DIWSTRT/DIWSTOP/DDFSTRT/DDFSTOP values are the manual's own standard
; display-window/fetch-window constants (Chapter 3), not RTL-derived.
; BPLCON0/DIWSTRT/DIWSTOP/DDFSTRT/DDFSTOP/BPL1MOD/BPL2MOD/BPL1PTH/BPL1PTL
; are not in nanomig.i (only the subset used elsewhere in this suite is)
; and are defined locally here from the manual's Appendix B.

        include "nanomig.i"

BP_SCRATCH      equ     $0C0100
BP_PTR          equ     $0C6000

BPLCON0         equ     $100
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPL1MOD         equ     $108
BPL2MOD         equ     $10A
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        move.w  #$8300,DMACON(a0)      ; SET DMAEN,BPLEN
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0000,BPL1MOD(a0)
        move.w  #$0000,BPL2MOD(a0)
        move.l  #BP_PTR,BPL1PTH(a0)

        ifd     BP_HIRES4
        move.w  #$003C,DDFSTRT(a0)
        move.w  #$00D4,DDFSTOP(a0)
        move.w  #$C000,BPLCON0(a0)     ; HIRES, 4 bitplanes
        else
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        ifd     BP_PLANES6
        move.w  #$6000,BPLCON0(a0)     ; 6 low-resolution bitplanes
        else
        move.w  #$4000,BPLCON0(a0)     ; 4 low-resolution bitplanes
        endif
        endif

        moveq   #0,d0
        lea     BP_SCRATCH,a1
.loop:  addq.w  #1,d0
        move.w  d0,(a1)
        bra.s   .loop
