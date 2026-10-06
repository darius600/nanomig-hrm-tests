; prog_run.s -- the stimulus program of aa-integ-s5: it executes the script
; that the test places in chip RAM before the run (one op after another, each
; custom register written by one MOVE.W or one MOVE.L on a pointer pair,
; never by CLR or a read-modify-write instruction) and then ends with DONE.
; Assembled for the 68000, vasm's default CPU.
;
; Script (words, big endian) at SCRIPT:
;   0 END                          end of script
;   1 W16  reg val                 marker, MOVE.W val to reg, marker
;   2 W32  reg hi lo               marker, MOVE.L to reg and reg+2, marker
;   3 DLY  n                       n+1 iterations of a DBRA loop
;   4 RD   reg                     read reg, store the word in the log
;   5 WAITB reg mask tmo           read reg and store it in the log until
;                                  (value & mask) != 0, at most tmo+1 reads
;   6 POLL reg n                   n+1 reads of reg, each stored in the log
;   7 PACT reg mask n creg cval    n+1 reads of reg, each stored in the log;
;                                  after a read with (value & mask) != 0 the
;                                  loop writes cval to creg
;   8 MARK                         one marker
;   9 W8   addr_hi addr_lo val     marker, MOVE.B val to the absolute address,
;                                  marker
;  10 RD3  rega regb               read rega, regb, rega in turn; store all
;                                  three in the log
;  11 WM   addr_hi addr_lo val     marker, MOVE.W val to the chip-RAM word,
;                                  marker
;  13 FEED2 reg mask nhits tmo cval rhi rlo w1 .. wn
;                                  as FEED, but the loop also writes cval to
;                                  INTREQ after every read without a hit
;  12 FEED reg mask nhits tmo cval rhi rlo w1 .. wn
;                                  read reg and store it in the log until
;                                  nhits reads have had (value & mask) != 0
;                                  (n = nhits); after each such read the loop
;                                  writes cval to INTREQ and the next word wi
;                                  to SERDAT; a wait of tmo+1 reads without a
;                                  hit adds 1 to the word at RESULTS+2 and
;                                  ends the op; the script goes on at rhi:rlo
; A marker is the running count 1, 2, 3, ... stored at MARKS + 2*(count-1).
; A wait that ends by its timeout adds 1 to the word at RESULTS+2.
; Symbol NOFIX leaves out the fixture store.
        include "nanomig.i"

SCRIPT  equ     $0B0000
SAVE    equ     $0B7FF0
MARKS   equ     $0B8000
LOG     equ     $100000
INTREQ_ equ     $09C
SERDAT_ equ     $030

        org     PROGRAM_ORG

start:
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS
        endc
        lea     CUSTOM,a0
        lea     SCRIPT,a1
        lea     LOG,a3
        lea     MARKS,a4
        moveq   #0,d6
next:   move.w  (a1)+,d0
        lsl.w   #2,d0
        jmp     tbl(pc,d0.w)
tbl:    bra.w   op_end
        bra.w   op_w16
        bra.w   op_w32
        bra.w   op_dly
        bra.w   op_rd
        bra.w   op_waitb
        bra.w   op_poll
        bra.w   op_pact
        bra.w   op_mark
        bra.w   op_w8
        bra.w   op_rd3
        bra.w   op_wm
        bra.w   op_feed
        bra.w   op_feed2

op_end:
        DONE

op_w16: move.w  (a1)+,d1
        move.w  (a1)+,d2
        addq.w  #1,d6
        move.w  d6,(a4)+
        move.w  d2,0(a0,d1.w)
        addq.w  #1,d6
        move.w  d6,(a4)+
        bra     next

op_w32: move.w  (a1)+,d1
        move.w  (a1)+,d2
        swap    d2
        move.w  (a1)+,d2
        addq.w  #1,d6
        move.w  d6,(a4)+
        move.l  d2,0(a0,d1.w)
        addq.w  #1,d6
        move.w  d6,(a4)+
        bra     next

op_dly: move.w  (a1)+,d1
.l:     dbra    d1,.l
        bra     next

op_rd:  move.w  (a1)+,d1
        move.w  0(a0,d1.w),(a3)+
        bra     next

op_waitb:
        move.w  (a1)+,d1
        move.w  (a1)+,d2
        move.w  (a1)+,d3
.l:     move.w  0(a0,d1.w),d4
        move.w  d4,(a3)+
        and.w   d2,d4
        bne     next
        dbra    d3,.l
        addq.w  #1,RESULTS+2
        bra     next

op_poll:
        move.w  (a1)+,d1
        move.w  (a1)+,d3
        lea     0(a0,d1.w),a2
.l:     move.w  (a2),(a3)+
        dbra    d3,.l
        bra     next

op_pact:
        move.w  (a1)+,d1
        move.w  (a1)+,d2
        move.w  (a1)+,d3
        move.w  (a1)+,d7
        move.w  (a1)+,d5
        lea     0(a0,d1.w),a2
        lea     0(a0,d7.w),a6
.l:     move.w  (a2),d4
        move.w  d4,(a3)+
        and.w   d2,d4
        beq.s   .c
        move.w  d5,(a6)
.c:     dbra    d3,.l
        bra     next

op_mark:
        addq.w  #1,d6
        move.w  d6,(a4)+
        bra     next

op_w8:  move.w  (a1)+,d1
        swap    d1
        move.w  (a1)+,d1
        move.l  d1,a2
        move.w  (a1)+,d2
        addq.w  #1,d6
        move.w  d6,(a4)+
        move.b  d2,(a2)
        addq.w  #1,d6
        move.w  d6,(a4)+
        bra     next

op_rd3: move.w  (a1)+,d1
        move.w  (a1)+,d2
        move.w  0(a0,d1.w),(a3)+
        move.w  0(a0,d2.w),(a3)+
        move.w  0(a0,d1.w),(a3)+
        bra     next

op_wm:  move.w  (a1)+,d1
        swap    d1
        move.w  (a1)+,d1
        move.l  d1,a2
        move.w  (a1)+,d2
        addq.w  #1,d6
        move.w  d6,(a4)+
        move.w  d2,(a2)
        addq.w  #1,d6
        move.w  d6,(a4)+
        bra     next

op_feed:
        move.w  (a1)+,d1
        move.w  (a1)+,d2
        move.w  (a1)+,d7
        move.w  (a1)+,d3
        move.w  (a1)+,d5
        move.l  (a1)+,SAVE
        lea     0(a0,d1.w),a2
        lea     INTREQ_(a0),a6
        lea     SERDAT_(a0),a5
        subq.w  #1,d7
.l:     move.w  (a2),d4
        move.w  d4,(a3)+
        and.w   d2,d4
        bne.s   .hit
        dbra    d3,.l
        addq.w  #1,RESULTS+2
        bra.s   .done
.hit:   move.w  d5,(a6)
        move.w  (a1)+,(a5)
        dbra    d7,.l
.done:  move.l  SAVE,a1
        bra     next

op_feed2:
        move.w  (a1)+,d1
        move.w  (a1)+,d2
        move.w  (a1)+,d7
        move.w  (a1)+,d3
        move.w  (a1)+,d5
        move.l  (a1)+,SAVE
        lea     0(a0,d1.w),a2
        lea     INTREQ_(a0),a6
        lea     SERDAT_(a0),a5
        subq.w  #1,d7
.l:     move.w  (a2),d4
        move.w  d4,(a3)+
        and.w   d2,d4
        bne.s   .hit
        move.w  d5,(a6)
        dbra    d3,.l
        addq.w  #1,RESULTS+2
        bra.s   .done
.hit:   move.w  d5,(a6)
        move.w  (a1)+,(a5)
        dbra    d7,.l
.done:  move.l  SAVE,a1
        bra     next
