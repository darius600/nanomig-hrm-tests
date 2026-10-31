; prog_joy.s -- P11: read JOY0DAT and JOY1DAT in a loop and store each value
; to its own buffer until the fourth VERTB (AA p.25, R5: INTREQ bit 5 is cleared
; first, then polled with INTREQR) has been seen, then DONE. The fourth VERTB comes
; at least three frame periods after the program starts, whatever the CPU's speed.
; The loop also ends when a buffer is full (JBUF words). Each store's port time
; is the time of the read before it. NOFIX, defined before the include of this
; file, leaves out the fixture store.
        include "nanomig.i"

JOY0DAT equ     $00A
JOY1DAT equ     $00C
JA      equ     $0D0000
JB      equ     $0D8000
JBUF    equ     16384

        org     PROGRAM_ORG

start:
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS
        endc
        lea     CUSTOM,a0
        lea     JA,a1
        lea     JB,a2
        move.w  #$0020,INTREQ(a0)       ; clear VERTB
        moveq   #3,d6                   ; VERTB edges still to see after this one
        move.w  #JBUF-1,d7
.l:     move.w  JOY0DAT(a0),(a1)+
        move.w  JOY1DAT(a0),(a2)+
        move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .nov
        move.w  #$0020,INTREQ(a0)
        dbra    d6,.nov
        bra.s   .out
.nov:   dbra    d7,.l
.out:   DONE
