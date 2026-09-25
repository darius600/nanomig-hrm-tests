; prog_tas.s -- TAS [p196]: 6-system-dma.c56 (TAS uses an indivisible
; read-modify-write cycle). Initializes a chip-RAM scratch byte, then
; executes one TAS.B on it and DONE. The test reads the RAM-port log's
; read-then-write cycle gap for this single instruction and compares it
; against the ordinary back-to-back bus-cycle spacing measured elsewhere
; (prog_sharing.bin's own uncontested loop, the SLOTSPAN assertion).

        include "nanomig.i"

TAS_ADDR        equ     $0C2000

        org     PROGRAM_ORG

start:
        move.b  #$00,TAS_ADDR
        tas.b   TAS_ADDR

        DONE
