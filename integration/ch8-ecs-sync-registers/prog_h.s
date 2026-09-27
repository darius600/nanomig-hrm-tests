; prog_h.s -- Program H (SPECIFICATION.md sec.4, "Program H (used by P1 and
; P2)"). Never assembled directly: an includer sets N (HTOTAL value) and B
; (BEAMCON0 value) and includes this file. Optional symbols select a
; precondition or anchor mutation in place of the normal body:
;   HALT_BEFORE  -- X-HALT-H: BRA.S * inserted before the HTOTAL write.
;   SHORT_LOOP   -- X-SHORT: 16 blocks (1024 samples) instead of 256 (16384).
;   BURST_MODE   -- X-BURST: read VHPOSR without storing until H decreases
;                   (a wrap), then store 4 samples; repeat 400 times.
;   AUD0_ON      -- X-AUD0: audio channel 0 DMA started, so a DMA access
;                   other than refresh appears at the port.
; MARK1/MARK2 are two words the test finds as the CPU write of $A5A5/$5A5A
; at the Chip-memory port. ID holds the VPOSR value read before any
; BEAMCON0 write (PC-ID). SAMPLES holds the 16384 words the sampling loop
; stores (32 KB).

        include "nanomig.i"
        include "regs.i"

MARK1           equ     $0C0100
MARK2           equ     $0C0102
ID              equ     $0C0104
SAMPLES         equ     $0C4000
AUDBUF          equ     $0C0200

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$07FF,DMACON(a0)
        move.w  #$0000,BPLCON0(a0)     ; clears LPEN so VHPOSR reads the beam (p.216,p.271)

        move.w  VPOSR(a0),d0
        move.w  d0,ID

        ifd     AUD0_ON
        move.l  #AUDBUF,AUD0LCH(a0)
        move.w  #1,AUD0LEN(a0)
        move.w  #50,AUD0PER(a0)
        move.w  #32,AUD0VOL(a0)
        move.w  #$8201,DMACON(a0)      ; SET,DMAEN,AUD0EN
        endif

        ifd     HALT_BEFORE
        bra.s   *
        endif

        move.w  #N,HTOTAL(a0)
        move.w  #$0138,VTOTAL(a0)      ; 312: standard PAL field size (p.216), nothing vertical graded
        move.w  #B,BEAMCON0(a0)

        move.w  #$A5A5,MARK1

        lea     $DFF006,a1              ; VHPOSR, fixed pointer (never auto-increments)
        lea     SAMPLES,a0

        ifd     BURST_MODE
        move.w  #399,d1                  ; 400 bursts of 4 = 1600 stores
.burst: move.w  (a1),d2
        and.w   #$00FF,d2
.spin:  move.w  d2,d3
        move.w  (a1),d2
        and.w   #$00FF,d2
        cmp.w   d2,d3                   ; d3-d2: d3<=d2 (unsigned) means no wrap yet
        bls.s   .spin
        moveq   #3,d4
.store: move.w  (a1),(a0)+
        dbra    d4,.store
        dbra    d1,.burst
        else
        ifd     SHORT_LOOP
        move.w  #15,d0                   ; 16 blocks of 64 = 1024 samples
        else
        move.w  #255,d0                  ; 256 blocks of 64 = 16384 samples
        endif
.block: rept    64
        move.w  (a1),(a0)+
        endr
        dbra    d0,.block
        endif

        move.w  #$5A5A,MARK2
        bra.s   *
