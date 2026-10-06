; prog_joy.s -- P11: read JOY0DAT and JOY1DAT in a loop and store each value
; to its own buffer, 7200 times, then DONE. Each store's port time is the
; time of the read before it. NOFIX, defined before the include of this
; file, leaves out the fixture store.
        include "nanomig.i"

JOY0DAT equ     $00A
JOY1DAT equ     $00C
JA      equ     $0D0000
JB      equ     $0D8000

        org     PROGRAM_ORG

start:
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS
        endc
        lea     CUSTOM,a0
        lea     JA,a1
        lea     JB,a2
        move.w  #7199,d7
.l:     move.w  JOY0DAT(a0),(a1)+
        move.w  JOY1DAT(a0),(a2)+
        dbra    d7,.l
        DONE
