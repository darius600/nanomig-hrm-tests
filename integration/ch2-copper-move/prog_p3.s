; prog_p3.s -- P3: the word a MOVE delivers is the content of RAM at its IR2
; fetch (SPECIFICATION.md P3). The list at $36000 is placed by the test; from
; the third field on, the CPU changes the IR2 word at $3600A once per field.
;
; Symbols this source answers to:
;   P3_DEST   address the four values are written to (default $3600A, the IR2 word)
;   P3_FAST   the four writes follow each other without a wait for the beam
;   P3_ATTR   a CPU read of the IR2 word before the Copper starts

        include "nanomig.i"
        include "ch2move.i"

        ifnd    P3_DEST
P3_DEST         equ     $3600A
        endc

; Writes the value \1 once the beam is on line $A0 of a field with V8 = 0,
; then waits for the beam to leave the line.
VALUEWRITE      macro
.w\@            btst    #0,CUSTOM+VPOSR+1
                bne.s   .w\@
                move.b  CUSTOM+VHPOSR,d0
                cmp.b   #$A0,d0
                bne.s   .w\@
                move.w  #\1,P3_DEST
.o\@            move.b  CUSTOM+VHPOSR,d0
                cmp.b   #$A0,d0
                beq.s   .o\@
                endm

        org     PROGRAM_ORG

start:
        BASELINE
        VIDEOREGS
        PARKPTRS
        ifd     P3_ATTR
        move.w  $3600A,d1
        endc
        COPSTART $36000
        IDLEFIELDS 3
        ifd     P3_FAST
        move.w  #$00FF,P3_DEST
        move.w  #$0FF0,P3_DEST
        move.w  #$0A5A,P3_DEST
        move.w  #$05A5,P3_DEST
        else
        VALUEWRITE $00FF
        VALUEWRITE $0FF0
        VALUEWRITE $0A5A
        VALUEWRITE $05A5
        endc
        IDLEFIELDS 4
        DONE
