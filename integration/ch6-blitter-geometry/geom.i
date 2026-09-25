; geom.i -- shared constants and macros for ch6-blitter-geometry's own
; programs. Not part of the harness contract: this package's own file.
;
; Scratch chip-RAM layout (all in the identity-mapped $080000-$1FFFFF
; range; the program itself is small and loads at PROGRAM_ORG=$080400).
SCRATCH         equ     $082000         ; general scratch base
SRC             equ     SCRATCH+$0000   ; primary source buffer
SRC2            equ     SCRATCH+$0800   ; secondary source buffer
MASKBUF         equ     SCRATCH+$1000   ; A-channel mask buffer
DST             equ     SCRATCH+$4000   ; primary destination buffer
DST2            equ     SCRATCH+$4800   ; secondary destination buffer
BITMAP          equ     SCRATCH+$6000   ; whole-bitmap scratch (A3)

; DMACON: bit15 SET, bit10 BLTPRI(unused), bit9 DMAEN, bit6 BLTEN.
DMACON_BLTEN    equ     $8240

; DMACONR bit 14 = BBUSY.
BBUSY_BIT       equ     14

; Poll DMACONR until the blitter is no longer busy. Clobbers d0.
WAITBLT         macro
.wb\@           move.w  DMACONR(a0),d0
                btst    #BBUSY_BIT,d0
                bne.s   .wb\@
                endm

; Enable blitter DMA (a0 must already hold CUSTOM). Clobbers nothing else.
BLTEN_ON        macro
                move.w  #DMACON_BLTEN,DMACON(a0)
                endm

; Set a channel pointer register pair PTH/PTL to a compile-time-constant
; byte address. Usage: SETPTR PTH_OFFS,PTL_OFFS,address
SETPTR          macro
                move.w  #(\3)>>16,\1(a0)
                move.w  #(\3)&$FFFF,\2(a0)
                endm
