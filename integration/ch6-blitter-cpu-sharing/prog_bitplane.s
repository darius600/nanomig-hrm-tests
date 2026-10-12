; prog_bitplane.s -- A9/A10/A11 [p196,197]: bitplane-DMA bandwidth
; pressure on the CPU. Enables bitplane DMA at a chosen plane count and
; resolution, and runs forever the same counting loop prog_sharing.s
; uses; the test tells bitplane-DMA activity apart from the CPU's own
; accesses by address (BP_PTR vs BP_SCRATCH). This file alone (no symbol
; set) is the four low-resolution-bitplane build (A9's baseline).
; prog_bitplane_hires.s (BP_HIRES4 set): four high-resolution bitplanes
; (A10). prog_bitplane_planes6.s (BP_PLANES6 set): six low-resolution
; bitplanes (A11). prog_bitplane_aligned.s (ALIGNED set, A6) and
; prog_bitplane_planes6_aligned.s (BP_PLANES6 and ALIGNED both set, A6's
; mutation) replace the loop's BRA.S with JMP (An), the same whole-4-clock
; -aligned loop prog_sharing_aligned.s uses -- see that file.
; prog_bitplane_planes5.s (BP_NPLANES=5) and prog_bitplane_aligned_p1/p2/p3.s
; (BP_NPLANES=1/2/3 with ALIGNED): other low-resolution plane counts.
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

LOOP_ORG        equ     PROGRAM_ORG+$100        ; every setup above is shorter than this

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        move.w  #$8300,DMACON(a0)      ; SET DMAEN,BPLEN
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0000,BPL1MOD(a0)
        move.w  #$0000,BPL2MOD(a0)
        move.l  #BP_PTR,BPL1PTH(a0)
        ifd     BP_ALLPTR
        move.l  #BP_PTR+$10000,BPL1PTH+4(a0)    ; planes 2-6 point at their own Chip memory
        move.l  #BP_PTR+$14000,BPL1PTH+8(a0)    ; (BPL2PTH.. = $0E4, $0E8, ...), none at the
        move.l  #BP_PTR+$18000,BPL1PTH+12(a0)   ; low addresses an unset pointer would read
        move.l  #BP_PTR+$1C000,BPL1PTH+16(a0)
        move.l  #BP_PTR+$20000,BPL1PTH+20(a0)
        endif

        ifd     BP_HIRES4
        move.w  #$003C,DDFSTRT(a0)
        move.w  #$00D4,DDFSTOP(a0)
        move.w  #$C000,BPLCON0(a0)     ; HIRES, 4 bitplanes
        else
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        ifd     BP_NPLANES
        move.w  #BP_NPLANES*$1000,BPLCON0(a0) ; BP_NPLANES low-resolution bitplanes (BPLCON0 bits 14-12)
        else
        ifd     BP_PLANES6
        move.w  #$6000,BPLCON0(a0)     ; 6 low-resolution bitplanes
        else
        move.w  #$4000,BPLCON0(a0)     ; 4 low-resolution bitplanes
        endif
        endif
        endif

        moveq   #0,d0
        lea     BP_SCRATCH,a1
        ifd     ALIGNED
        lea     LOOP,a2
        endif
        jmp     LOOP.l                  ; the counting loop sits at the same address in every build of
                                        ; prog_sharing.s and prog_bitplane.s, so the same instruction
                                        ; boundary has the same pair of addresses in every capture

        org     LOOP_ORG
        ifd     ALIGNED
LOOP:   addq.w  #1,d0                   ; 4 clocks (2 slots)
        move.w  d0,(a1)                 ; 8 clocks (4 slots)
        jmp     (a2)                    ; 8 clocks (4 slots) -- aligned, unlike BRA.S's 10
        else
LOOP:   addq.w  #1,d0
        move.w  d0,(a1)
        bra.s   LOOP
        endif
