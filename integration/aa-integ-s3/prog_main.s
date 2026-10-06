; prog_main.s -- the stimulus program of package aa-integ-s3.
;
; The program performs, with word writes from a data register, the register
; writes of a setup table in Chip memory, stores the setup marker, and then
; works through an event table. Both tables are placed in Chip memory by the
; test before the program starts.
;
; Setup table (SETUP): pairs (register offset, value), ended by the word $FFFF.
;
; Event table (EVENTS): per event
;     word  1 (0 ends the table)
;     word  nvb   number of vertical-blank flags (INTREQR bit 5) to wait for
;                 and clear by a write of $0020 to INTREQ, before anything else
;     word  line  V10-V0 to wait for; $FFFF waits for no beam position
;     word  hmin  lowest H (VHPOSR bits 7-0) of that line
;     word  npre  number of words of the preparation routine
;     npre words  the routine (run before the beam wait): MOVE.W #value,Dn
;                 for the values the action writes most often, ended by RTS
;     word  ncode number of words of the event's routine
;     ncode words the routine, run when the beam position is reached, ended by
;                 RTS: MOVE.W Dn,offset(A1) or MOVE.W #value,offset(A1) per
;                 register write, MOVE.W offset(A1),n(A2) per register read
;                 (the value is stored to the event's result record), and
;                 MOVE.W #value,address per Chip-memory word write (A1 holds
;                 the custom-register base, A2 the event's result record)
; The result record of event k lies at RESULTS + $10 + $10*k:
;     +0 marker $E000+k (stored last)   +2 VPOSR re-read   +4 VHPOSR re-read
;     +6.. the values read
;
; The program ends with DONE.

                include "nanomig.i"

SETUP           equ     $088000
EVENTS          equ     $08A000
RESULT0         equ     RESULTS+$10

; wait for the beam position in D4 (line) and D5 (H); $FFFF in D4: no wait
POLLWAIT        macro
                cmp.w   #$FFFF,d4
                beq.s   .go\@
                lsl.l   #8,d4
                or.l    d5,d4
                move.l  #$0007FFFF,d6
.p\@:           move.l  VPOSR(a1),d7
                and.l   d6,d7
                cmp.l   d4,d7
                blo.s   .p\@
.go\@:
                endm

                org     PROGRAM_ORG

start:          lea     CUSTOM,a1
                lea     SETUP,a0
setup:          move.w  (a0)+,d1
                bmi.s   setupdone
                move.w  (a0)+,d0
                move.w  d0,(a1,d1.w)
                bra.s   setup
setupdone:
                ifnd    NOMARKER
                move.w  #$5E7F,RESULTS
                endc

                lea     EVENTS,a0
                lea     RESULT0,a2
                movea.w #$E000,a6
event:          move.w  (a0)+,d4
                beq     finish

                move.w  (a0)+,d5
                subq.w  #1,d5
                bmi.s   novb
vb1:            move.w  INTREQR(a1),d7
                btst    #5,d7
                beq.s   vb1
                move.w  #$0020,INTREQ(a1)
                dbra    d5,vb1
novb:           moveq   #0,d4
                move.w  (a0)+,d4
                moveq   #0,d5
                move.w  (a0)+,d5
                move.w  (a0)+,d6
                movea.l a0,a4
                add.w   d6,d6
                lea     (a0,d6.w),a0
                tst.w   d6
                beq.s   nopre
                jsr     (a4)
nopre:          move.w  (a0)+,d6
                movea.l a0,a4
                add.w   d6,d6
                lea     (a0,d6.w),a0
                tst.w   d6
                bne.s   hascode
                lea     norts(pc),a4
hascode:        POLLWAIT
                jsr     (a4)
                move.l  VPOSR(a1),d6
                move.l  d6,2(a2)
                move.w  a6,(a2)
                addq.w  #1,a6
                lea     16(a2),a2
                bra     event

norts:          rts

finish:         DONE
