; prog_samp.s -- run HV and the P8 runs (SPECIFICATION.md sections 2 and 5):
; after the prologue, 64 reads of SAMP_REG, each word stored to BUF + 2i.
; SAMP_DUAL set: HTOTAL <- SAMP_N, BEAMCON0 <- $2860 (LPENDIS, LOLDIS, DUAL, PAL),
; HHPOSW <- SAMP_X come first (P8 steps 2 to 4).
; Optional: SAMP_PRENOP (one NOP before the loop), SAMP_DELAY (a delay inside the loop).

        include "s7r.i"

        ifnd    SAMP_REG
SAMP_REG        equ     VHPOSR
        endc

        org     PROGRAM_ORG

start:
        PROLOGUE
        ifd     SAMP_DUAL
        move.w  #SAMP_N,HTOTAL(a0)
        move.w  #$2860,BEAMCON0(a0)
        move.w  #SAMP_X,HHPOSW(a0)
        endc
        ifd     SAMP_PRENOP
        nop
        endc
        lea     BUF,a1
        moveq   #63,d7
.l:     move.w  SAMP_REG(a0),(a1)+
        ifd     SAMP_DELAY
        moveq   #SAMP_DELAY,d6
.d:     dbra    d6,.d
        endc
        dbra    d7,.l
        EPILOGUE
