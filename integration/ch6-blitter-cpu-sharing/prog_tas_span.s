; prog_tas_span.s -- TAS-SPAN [p196]: the cycles at one address. Three
; ordinary accesses (a write, a read, a write) and then one TAS.B, with
; no DMA channel enabled. The test reads the cycles at TAS_ADDR from the RAM
; port: ordinary cycles first, the TAS read and write last.
; prog_tas_span_mut.s (TAS_MUT set) replaces the TAS.B with NOP.

        include "nanomig.i"

TAS_ADDR        equ     $0C2000

        org     PROGRAM_ORG

start:
        lea     TAS_ADDR,a1
        move.b  #$00,TAS_ADDR          ; ordinary write
        nop
        nop
        move.b  (a1),d1                ; ordinary read
        nop
        nop
        move.b  d1,(a1)                ; ordinary write
        nop
        nop
        ifd     TAS_MUT
        nop
        else
        tas.b   (a1)                   ; the indivisible read-modify-write cycle
        endif
        nop
        nop

        DONE
