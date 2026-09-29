; prog_drivelight.s -- section 6 record (no claim, not graded): the motor/
; select sequence of SPECIFICATION.md section 6 (S0-S8), each write followed
; by a marker (the byte written, zero-extended) held for 64 line changes.
; fdd_led itself is read by the C++ driver, cycle by cycle, between markers
; (a top-level port, not reachable from the program).
        include "nanomig.i"
        include "hj_defs.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #0,BPLCON0(a0)                 ; light pen latch off (SPECIFICATION.md 3.2)
        move.w  #FIXTURE_VALUE,FIXTURE_ADDR    ; fixture, stored before anything is graded

        move.b  #$FF,CIAB_PRB                  ; p.354: PRB=0 first, all lines high
        move.b  #$FF,CIAB_DDRB                 ; then DDRB=$FF (p.244: motor set up before any select)

        move.b  VHPOSR(a0),d7                  ; prime: current line value

        move.b  #$FF,CIAB_PRB                  ; S0: motor high, no drive selected
        move.w  #$00FF,RESULTS
        bsr     WAIT64
        move.b  #$7F,CIAB_PRB                  ; S1: motor low, no drive selected
        move.w  #$007F,RESULTS
        bsr     WAIT64
        move.b  #$77,CIAB_PRB                  ; S2: select drive 0 (motor low)
        move.w  #$0077,RESULTS
        bsr     WAIT64
        move.b  #$7F,CIAB_PRB                  ; S3: deselect
        move.w  #$007F,RESULTS
        bsr     WAIT64
        move.b  #$FF,CIAB_PRB                  ; S4: motor high, no drive selected
        move.w  #$00FF,RESULTS
        bsr     WAIT64
        move.b  #$F7,CIAB_PRB                  ; S5: select drive 0 (motor high)
        move.w  #$00F7,RESULTS
        bsr     WAIT64
        move.b  #$FF,CIAB_PRB                  ; S6: deselect
        move.w  #$00FF,RESULTS
        bsr     WAIT64
        move.b  #$7F,CIAB_PRB                  ; S7: motor low, no drive selected
        move.w  #$007F,RESULTS
        bsr     WAIT64
        move.b  #$FF,CIAB_PRB                  ; S8: motor high
        move.w  #$00FF,RESULTS
        bsr     WAIT64

        DONE

; waits 64 line changes (SPECIFICATION.md section 6, section 3.3); a0=CUSTOM,
; d7 = last-seen VHPOSR hi byte, persistent across calls.
WAIT64:
        move.w  #63,d5
.loop:
.w:     move.b  VHPOSR(a0),d0
        cmp.b   d7,d0
        beq.s   .w
        move.b  d0,d7
        dbra    d5,.loop
        rts
