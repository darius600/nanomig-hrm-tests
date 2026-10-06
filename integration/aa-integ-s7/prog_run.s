; prog_run.s -- the stimulus program of aa-integ-s7: it executes the script that
; the test places in chip RAM before the run, one op after another, and then
; ends with DONE. Every custom register is written by one MOVE.W (or by one
; MOVE.L on a pointer pair) of an immediate or script word, never by CLR or a
; read-modify-write instruction. Assembled for the 68000, vasm's default CPU.
;
; The script lies inside the program image: SCRIPT .. SCRIPT_END is part of
; [PROGRAM_ORG, 0x08CC36), the span the specification calls the program.
;
; Script words (big endian) at SCRIPT:
;   0 END                          end of script
;   1 W16   reg val                MOVE.W val to custom reg
;   2 W32   reg hi lo              MOVE.L hi:lo to reg and reg+2
;   3 DLY   n                      n+1 iterations of a DBRA loop
;   4 RD    reg                    read reg, store the word in the log
;   5 MARK  n                      MOVE.W n to MK (the marker word)
;   6 VBLSYNC                      INTREQ = $0020; read INTREQR until bit 5
;                                  is set; INTREQ = $0020
;   7 WAITLINE v                   read VHPOSR until bits 15-8 >= v; store
;                                  the last read in the log
;   8 LINES n                      read VHPOSR until bits 15-8 changed n times
;   9 BLITDONE                     LINES 1, then read DMACONR until bit 14 is 0
;  10 POLLB reg mask tmo           read reg until (value & mask) != 0, at most
;                                  tmo+1 reads; store the last read in the log
;  11 WM    addr_hi addr_lo val    MOVE.W val to a chip-RAM word
;  12 W16R  reg1 val1 reg2 val2    two MOVE.W writes in a row
;  13 WAITB reg mask tmo           read reg until (value & mask) != 0, at most
;                                  tmo+1 reads; a wait that ends by its timeout
;                                  adds 1 to the word at RESULTS+2
; A wait of the ops 6-9 that ends by its timeout adds 1 to the word at RESULTS+2.
; Symbol NOFIX leaves out the fixture store.
        include "nanomig.i"

SCRIPT  equ     $081000
MK      equ     $0B8000
LOG     equ     $0C0010
TMO     equ     RESULTS+2
VPOSR_  equ     $004
VHPOSR_ equ     $006
VBTMO   equ     15000
LNTMO   equ     40000
BLTMO   equ     30000
WLTMO   equ     30000

        org     PROGRAM_ORG

start:
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS
        endc
        lea     CUSTOM,a0
        lea     SCRIPT,a1
        lea     LOG,a3
next:   move.w  (a1)+,d0
        lsl.w   #2,d0
        jmp     tbl(pc,d0.w)
tbl:    bra.w   op_end
        bra.w   op_w16
        bra.w   op_w32
        bra.w   op_dly
        bra.w   op_rd
        bra.w   op_mark
        bra.w   op_vblsync
        bra.w   op_waitline
        bra.w   op_lines
        bra.w   op_blitdone
        bra.w   op_pollb
        bra.w   op_wm
        bra.w   op_w16r
        bra.w   op_waitb

op_end:
        DONE

op_w16: move.w  (a1)+,d1
        move.w  (a1)+,d2
        move.w  d2,0(a0,d1.w)
        bra     next

op_w16r:
        move.w  (a1)+,d1
        move.w  (a1)+,d2
        move.w  (a1)+,d4
        move.w  (a1)+,d5
        move.w  d2,0(a0,d1.w)
        move.w  d5,0(a0,d4.w)
        bra     next

op_w32: move.w  (a1)+,d1
        move.w  (a1)+,d2
        swap    d2
        move.w  (a1)+,d2
        move.l  d2,0(a0,d1.w)
        bra     next

op_dly: move.w  (a1)+,d1
.l:     dbra    d1,.l
        bra     next

op_rd:  move.w  (a1)+,d1
        move.w  0(a0,d1.w),(a3)+
        bra     next

op_mark:
        move.w  (a1)+,MK
        bra     next

op_vblsync:
        move.w  (a1)+,d3
        move.w  #$0020,INTREQ(a0)
.l:     move.w  INTREQR(a0),d4
        and.w   #$0020,d4
        bne.s   .got
        dbra    d3,.l
        addq.w  #1,TMO
.got:   move.w  #$0020,INTREQ(a0)
        bra     next

op_waitline:
        move.w  (a1)+,d2
        move.w  #WLTMO,d3
.l:     move.w  VHPOSR_(a0),d4
        move.w  d4,d5
        lsr.w   #8,d5
        cmp.b   d2,d5
        bcc.s   .ok
        dbra    d3,.l
        addq.w  #1,TMO
.ok:    move.w  d4,(a3)+
        bra     next

op_lines:
        move.w  (a1)+,d2
        subq.w  #1,d2
        move.w  VHPOSR_(a0),d5
        lsr.w   #8,d5
        move.w  #LNTMO,d3
.l:     move.w  VHPOSR_(a0),d4
        lsr.w   #8,d4
        cmp.b   d4,d5
        bne.s   .chg
.t:     dbra    d3,.l
        addq.w  #1,TMO
        bra     next
.chg:   move.b  d4,d5
        dbra    d2,.t
        bra     next

op_blitdone:
        moveq   #0,d2
        move.w  VHPOSR_(a0),d5
        lsr.w   #8,d5
        move.w  #LNTMO,d3
.l:     move.w  VHPOSR_(a0),d4
        lsr.w   #8,d4
        cmp.b   d4,d5
        bne.s   .one
        dbra    d3,.l
        addq.w  #1,TMO
.one:   move.w  #BLTMO,d3
.p:     move.w  DMACONR(a0),d4
        and.w   #$4000,d4
        beq     next
        dbra    d3,.p
        addq.w  #1,TMO
        bra     next

op_pollb:
        move.w  (a1)+,d1
        move.w  (a1)+,d2
        move.w  (a1)+,d3
        lea     0(a0,d1.w),a2
.l:     move.w  (a2),d4
        move.w  d4,d5
        and.w   d2,d5
        bne.s   .got
        dbra    d3,.l
.got:   move.w  d4,(a3)+
        bra     next

op_wm:  move.w  (a1)+,d1
        swap    d1
        move.w  (a1)+,d1
        move.l  d1,a2
        move.w  (a1)+,d2
        move.w  d2,(a2)
        bra     next

op_waitb:
        move.w  (a1)+,d1
        move.w  (a1)+,d2
        move.w  (a1)+,d3
        lea     0(a0,d1.w),a2
.l:     move.w  (a2),d4
        and.w   d2,d4
        bne     next
        dbra    d3,.l
        addq.w  #1,TMO
        bra     next
