; prog_adkcon.s -- P5 (SPECIFICATION.md section 3): ADKCON bits 8 (FAST)
; and 9 (MSBSYNC) are separate bits that ADKCONR reads back. Five steps,
; each an ADKCON write followed at once by an ADKCONR read stored to its
; own result slot R0-R4 (2.9 table). No table/pointer data is used here,
; so no CLEAR_OVL is needed (RESULTS is in identity-mapped chip RAM
; regardless of the overlay, nanomig.i).
;
; Optional assembly-time symbols:
;   MUT_P5  step 2 writes $8200 instead of $8100 (P5's own contrasting case)
;   MUT_C1  the store of R2 (step 3's readback) is left out (C1's contrasting case)

        include "nanomig.i"
        include "ch5regs.i"

        org     PROGRAM_ORG

R0      equ     RESULTS+0
R1      equ     RESULTS+2
R2      equ     RESULTS+4
R3      equ     RESULTS+6
R4      equ     RESULTS+8

start:
        lea     CUSTOM,a0

        ; ---- preamble (2.3) ----
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,ADKCON(a0)

        ; step 1: clear bits 9 and 8
        move.w  #$0300,ADKCON(a0)
        move.w  ADKCONR(a0),R0

        ; step 2: set bit 8
        ifd     MUT_P5
        move.w  #$8200,ADKCON(a0)      ; P5 mutation: sets bit 9 instead of bit 8
        else
        move.w  #$8100,ADKCON(a0)
        endc
        move.w  ADKCONR(a0),R1

        ; step 3: set bit 9
        move.w  #$8200,ADKCON(a0)
        ifnd    MUT_C1
        move.w  ADKCONR(a0),R2         ; C1 mutation: this store is left out
        endc

        ; step 4: clear bit 8
        move.w  #$0100,ADKCON(a0)
        move.w  ADKCONR(a0),R3

        ; step 5: clear bit 9
        move.w  #$0200,ADKCON(a0)
        move.w  ADKCONR(a0),R4

        DONE
