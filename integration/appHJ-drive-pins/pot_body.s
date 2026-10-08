; pot_body.s -- P5-P8 (SPECIFICATION.md section 5): four pins (POT0X,
; POT0Y, POT1X, POT1Y), each run through the same eight-step sequence
; (SPECIFICATION.md 5.2 table). A wrapper sets PP2MUT, PP3MUT or WRONGREG
; before including this file: prog_pot_pp2.s and prog_pot_pp3.s give PP2's
; and PP3's own contrasts (SPECIFICATION.md 5.5), prog_pot_p5/p6/p8.s the
; register contrasts of P5, P6 and P8 (5.6); the base run (prog_pot.s) sets
; none.
;
; Per-pin POTGO values, from B=$FF00 (SPECIFICATION.md 5.2) with OUTxx/DATxx
; cleared per p.282's bit list (SPECIFICATION.md 5.1 "Supporting pages"):
;   P5 POT0X: OUTLX=9 DATLX=8   P6 POT0Y: OUTLY=11 DATLY=10
;   P7 POT1X: OUTRX=13 DATRX=12 P8 POT1Y: OUTRY=15 DATRY=14
        include "nanomig.i"
        include "hj_defs.i"

BASE            equ     $FF00

P5_C1           equ     BASE
        ifd     PP2MUT
P5_C1_STEP      equ     $FF01           ; PP2: bit 0 = 1 in C1 (SPECIFICATION.md 5.5)
        else
P5_C1_STEP      equ     P5_C1
        endif
P5_C4           equ     $FE00
P5_C6           equ     $FC00
P5_C8           equ     BASE

        ifd     PP3MUT
P5_C1_LINES     equ     5               ; PP3: step shortened to 6 lines (dbra count = N-1)
        else
P5_C1_LINES     equ     23              ; 24 lines (SPECIFICATION.md 5.2)
        endif

P6_C1           equ     BASE
P6_C4           equ     $FB00
P6_C6           equ     $F300
P6_C8           equ     BASE

P7_C1           equ     BASE
P7_C4           equ     $EF00
P7_C6           equ     $CF00
P7_C8           equ     BASE

P8_C1           equ     BASE
P8_C4           equ     $BF00
P8_C6           equ     $3F00
P8_C8           equ     BASE

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #0,BPLCON0(a0)                 ; light pen latch off (SPECIFICATION.md 3.2)
        move.w  #FIXTURE_VALUE,FIXTURE_ADDR    ; fixture, stored before anything is graded

        move.b  VHPOSR(a0),d7                  ; prime: current line value
        lea     BUFBASE,a1

; ---- pin 5: POT0X -----------------------------------------------------
        move.w  #P5_C1_STEP,POTGO(a0)
        move.w  #P5_C1_STEP,RESULTS
        move.w  #P5_C1_LINES,d5
        bsr     WAITN                           ; C1
        move.w  #23,d5
        bsr     WAITN                           ; C2 (button press, harness)
        move.w  #23,d5
        bsr     WAITN                           ; C3 (button release, harness)
        move.w  #P5_C4,POTGO(a0)
        move.w  #P5_C4,RESULTS
        move.w  #23,d5
        bsr     WAITN                           ; C4
        move.w  #23,d5
        bsr     WAITN                           ; C5 (button press, harness)
        move.w  #P5_C6,POTGO(a0)
        move.w  #P5_C6,RESULTS
        move.w  #23,d5
        bsr     WAITN                           ; C6
        move.w  #23,d5
        bsr     WAITN                           ; C7 (button release, harness)
        move.w  #P5_C8,POTGO(a0)
        move.w  #P5_C8,RESULTS
        move.w  #23,d5
        bsr     WAITN                           ; C8

; ---- pin 6: POT0Y -------------------------------------------------------
        move.w  #P6_C1,POTGO(a0)
        move.w  #P6_C1,RESULTS
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #P6_C4,POTGO(a0)
        move.w  #P6_C4,RESULTS
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #P6_C6,POTGO(a0)
        move.w  #P6_C6,RESULTS
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #P6_C8,POTGO(a0)
        move.w  #P6_C8,RESULTS
        move.w  #23,d5
        bsr     WAITN

; ---- pin 7: POT1X -------------------------------------------------------
        move.w  #P7_C1,POTGO(a0)
        move.w  #P7_C1,RESULTS
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #P7_C4,POTGO(a0)
        move.w  #P7_C4,RESULTS
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #P7_C6,POTGO(a0)
        move.w  #P7_C6,RESULTS
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #P7_C8,POTGO(a0)
        move.w  #P7_C8,RESULTS
        move.w  #23,d5
        bsr     WAITN

; ---- pin 8: POT1Y -------------------------------------------------------
        move.w  #P8_C1,POTGO(a0)
        move.w  #P8_C1,RESULTS
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #P8_C4,POTGO(a0)
        move.w  #P8_C4,RESULTS
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #P8_C6,POTGO(a0)
        move.w  #P8_C6,RESULTS
        move.w  #23,d5
        bsr     WAITN
        move.w  #23,d5
        bsr     WAITN
        move.w  #P8_C8,POTGO(a0)
        move.w  #P8_C8,RESULTS
        move.w  #23,d5
        bsr     WAITN

        DONE

; d5 = (line count - 1) on entry; stores one (VHPOSR,POTGOR) sample per
; line change (SPECIFICATION.md 3.3), a0/a1/d7 as in start.
WAITN:
.loop:  bsr     WAIT1
        dbra    d5,.loop
        rts

WAIT1:
.w:     move.b  VHPOSR(a0),d0
        cmp.b   d7,d0
        beq.s   .w
        move.b  d0,d7
        move.w  VHPOSR(a0),(a1)+
        ifd     WRONGREG
        cmpa.l  #BUFBASE+768*WRONGREG,a1       ; register contrast: the samples of pin
        blo.s   .potgor                        ; WRONGREG (192 samples of 4 bytes) store
        cmpa.l  #BUFBASE+768*(WRONGREG+1),a1   ; POTGOR with the pin's own DAT bit
        bhs.s   .potgor                        ; (WRONGBIT) replaced by the DAT bit of the
        move.w  POTGOR(a0),d0                  ; other pot pin of the same port (OTHERBIT)
        bclr    #WRONGBIT,d0
        btst    #OTHERBIT,d0
        beq.s   .st
        bset    #WRONGBIT,d0
.st:    move.w  d0,(a1)+
        rts
        endif
.potgor:
        move.w  POTGOR(a0),(a1)+
        rts
