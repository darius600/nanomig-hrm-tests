; prog_u.s -- the UHRES runs (SPECIFICATION.md section 4) and the dual-mode runs of P9
; (section 5). A wrapper defines the run's values (the table of section 4):
;   U_BPH U_BPL U_BMOD U_BSTRT U_BSTOP  (BPLHPTH, BPLHPTL, BPLHMOD, BPLHSTRT, BPLHSTOP)
;   U_SPH U_SPL U_SSTRT U_SSTOP         (SPRHPTH, SPRHPTL, SPRHSTRT, SPRHSTOP)
;   U_CON0 U_DMA                        (BPLCON0, DMACON)
;   U_VLO U_VHI                         (marker window)
; Optional: U_HTOT, U_BEAM (P9: HTOTAL and BEAMCON0 written between setup steps 3 and 4),
; U_VTOT (VTOTAL written before BEAMCON0), U_LATE (wait for line $60 before the setup),
; U_STEP (marker line step, default 1), U_GLO and U_GW (marker gate, default $60 and $30),
; NOEND, NOFIX.

        include "s7r.i"

        ifnd    U_STEP
U_STEP          equ     1
        endc
        ifnd    U_GLO
U_GLO           equ     $60
        endc
        ifnd    U_GW
U_GW            equ     $30
        endc

        org     PROGRAM_ORG

start:
        PROLOGUE
        VBWAIT
        ifd     U_LATE
.late:  move.w  VHPOSR(a0),d0
        lsr.w   #8,d0
        cmp.b   #$60,d0
        blo.s   .late
        endc
        ; step 1: every ordinary sprite and bit-plane pointer points at Z
SPRN    set     SPR0PTH
        rept    8
        move.w  #(Z>>16),SPRN(a0)
        move.w  #(Z&$FFFF),SPRN+2(a0)
SPRN    set     SPRN+4
        endr
BPLN    set     BPL1PTH
        rept    8
        move.w  #(Z>>16),BPLN(a0)
        move.w  #(Z&$FFFF),BPLN+2(a0)
BPLN    set     BPLN+4
        endr
        ; step 2: the UHRES sprite pointer, start and stop
        move.w  #U_SPH,SPRHPTH(a0)
        move.w  #U_SPL,SPRHPTL(a0)
        move.w  #U_SSTRT,SPRHSTRT(a0)
        move.w  #U_SSTOP,SPRHSTOP(a0)
        ; step 3: the UHRES bit-plane pointer, modulo, start and stop
        move.w  #U_BPH,BPLHPTH(a0)
        move.w  #U_BPL,BPLHPTL(a0)
        move.w  #U_BMOD,BPLHMOD(a0)
        move.w  #U_BSTRT,BPLHSTRT(a0)
        move.w  #U_BSTOP,BPLHSTOP(a0)
        ifd     U_HTOT
        move.w  #U_HTOT,HTOTAL(a0)
        endc
        ifd     U_VTOT
        move.w  #U_VTOT,VTOTAL(a0)
        endc
        ifd     U_BEAM
        move.w  #U_BEAM,BEAMCON0(a0)
        endc
        ; steps 4 and 5
        move.w  #U_CON0,BPLCON0(a0)
        move.w  #U_DMA,DMACON(a0)
        ; step 6: the setup-done record, the marker loop, the epilogue
        SETUPDONE
        MARKLOOP U_VLO,U_VHI,U_STEP,U_GLO,U_GW
        EPILOGUE
