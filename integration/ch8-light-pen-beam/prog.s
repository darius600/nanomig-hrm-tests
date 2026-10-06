; prog.s -- light pen / beam counter stimulus program (SPECIFICATION.md
; section 4). Runs from Chip memory in supervisor mode; every register
; write is a whole word (p.89). Phases A-D sample VPOSR:VHPOSR (or
; POTGOR) into buffers the harness reads back through the RAM-port log.
;
; NMH_K1: phase B's BPLCON0 write uses $0000 instead of $0008.
; NMH_K3: phase C's BPLCON0 write uses $0008 instead of $0000.
; NMH_K4: the initial BPLCON0 write (governing phase A) uses $0008.
; NMH_K13: BUFB is shortened so phase B ends within field B19.
; NMH_K14: after the wait150 that ends phase B, one extra write sets
;          VHPOSW ($02C) = $6400, before the phase-C BPLCON0 write.
; NMH_K5: the phase-B loop gets a delay of at least 300 colour clocks per
;         sample (CP2's own mutation), and BUFB is shortened to match.
; NMH_KFIX: the M0 marker write stores a different value (the FIXTURE
;           mutation), while M1-M4 and DONE are unaffected.
; NMH_KNODONE: the program hangs after M4 without storing DONE_VALUE (the
;           FIXTURE-DONE mutation).
        include "nanomig.i"

M0      equ     $A000
M1      equ     $A001
M2      equ     $A002
M3      equ     $A003
M4      equ     $A004

BUFA_COUNT      equ     22000
        ifd     NMH_K13
BUFB_COUNT      equ     60000
        else
        ifd     NMH_K5
BUFB_COUNT      equ     2000
        else
BUFB_COUNT      equ     90000
        endif
        endif
BUFC_COUNT      equ     13000
BUFD_COUNT      equ     3500

        org     PROGRAM_ORG

start:
        move.w  #$2700,sr               ; CPU ignores all interrupt levels
        lea     $dff000,a6
        move.w  #$7fff,$09a(a6)         ; INTENA: clear every enable bit (bit 15 = 0 clears, p.218)
        move.w  #$7fff,$09c(a6)         ; INTREQ: clear every request
        move.w  #$7fff,$096(a6)         ; DMACON: clear every DMA enable (p.278): no Copper, bitplane,
                                        ;   sprite, audio, disk or blitter DMA; only refresh remains
        ifd     NMH_K4
        move.w  #$0008,$100(a6)         ; BPLCON0: LPEN=1 (K4)
        else
        move.w  #$0000,$100(a6)         ; BPLCON0: BPU=0, HIRES=0, LACE=0 (non-interlaced), ERSY=0, LPEN=0
        endif
        ifd     NMH_KFIX
        move.w  #$BEEF,MARK             ; FIXTURE mutation: a different value where M0 belongs
        else
        move.w  #M0,MARK
        endif
        move.b  $bfe001,FIRE            ; CIAAPRA, read only: bit 6 port 1 and bit 7 port 2 pin 6,
                                        ;   1 = "switch open" (p.229, p.232); for CP3
; ---- phase A: beam counter, light pen off
        lea     BUFA,a0
        lea     BUFA_END,a1
.a      move.l  $dff004,(a0)+           ; VPOSR:VHPOSR as one long word (p.238)
        cmpa.l  a1,a0
        bne.s   .a
        bsr     wait150
        ifd     NMH_K1
        move.w  #$0000,$100(a6)         ; BPLCON0: LPEN stays 0 (K1)
        else
        move.w  #$0008,$100(a6)         ; BPLCON0: the same word with bit 3 = 1 (p.239)
        endif
        move.w  #M1,MARK
; ---- phase B: light pen on
        lea     BUFB,a0
        lea     BUFB_END,a1
.b      move.l  $dff004,(a0)+
        ifd     NMH_K5
        move.w  #250,d1                 ; K5: delay well over 300 colour clocks between samples
.b5dly  dbra    d1,.b5dly
        endif
        cmpa.l  a1,a0
        bne.s   .b
        bsr     wait150
        ifd     NMH_K14
        move.w  #$6400,$02c(a6)         ; VHPOSW (K14): line 100, H 0
        endif
        ifd     NMH_K3
        move.w  #$0008,$100(a6)         ; BPLCON0: LPEN stays 1 (K3)
        else
        move.w  #$0000,$100(a6)         ; BPLCON0: bit 3 = 0 again
        endif
        move.w  #M2,MARK
; ---- phase C: light pen off again
        lea     BUFC,a0
        lea     BUFC_END,a1
.c      move.l  $dff004,(a0)+
        cmpa.l  a1,a0
        bne.s   .c
        move.w  #$ff00,$034(a6)         ; POTGO: OUTRY DATRY OUTRX DATRX OUTLY DATLY OUTLX DATLX = 1,
                                        ;   bits 7-1 = 0, START = 0 (p.240, p.241, p.236)
        move.w  #M3,MARK
; ---- phase D: pot pins as buttons
        lea     BUFD,a0
        lea     BUFD_END,a1
.d      move.w  $dff016,(a0)+           ; POTGOR / POTINP (p.240)
        cmpa.l  a1,a0
        bne.s   .d
        move.w  #M4,MARK
        ifd     NMH_KNODONE
.nodone bra.s   .nodone                 ; FIXTURE-DONE mutation: no DONE store
        else
        DONE                            ; stores DONE_VALUE at DONE_ADDR, then hangs (nanomig.i)
        endif

wait150 move.l  $dff004,d0              ; wait until a read shows vertical position 150
        lsr.l   #8,d0
        andi.w  #$01ff,d0
        cmpi.w  #150,d0
        bne.s   wait150
        rts

        org     $100000
MARK:   ds.w    1
        org     $100004
FIRE:   ds.b    1
        org     $100010
BUFA:   ds.l    BUFA_COUNT
BUFA_END:
BUFB:   ds.l    BUFB_COUNT
BUFB_END:
BUFC:   ds.l    BUFC_COUNT
BUFC_END:
BUFD:   ds.w    BUFD_COUNT
BUFD_END:
