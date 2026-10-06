; prog_beam.s -- the beam programs of P9-P11, P13 and P14 (SPECIFICATION.md
; F3, F4, F6).
;
; The test stores the run's parameters in Chip memory before the program
; starts, and the program reads them (a word each) before it writes any
; register:
;   CFG+0   1: sample one frame in normal mode before the register list (F4 dry run)
;   CFG+2   number of frames (0, 1 or 2) sampled after the register list
;   CFG+4   1: store HHPOSR after every VPOSR/VHPOSR sample
;   CFG+6   number of frames the CPU is stopped for at the end
;   CFG+8   number of entries of the register list
;   CFG+$10 the register list: pairs of (register offset, value)
;
; Sequence: the initial state (SPECIFICATION.md 2.2) with BEAMCON0 = $0020,
; the ROM overlay cleared and the level-3 vector set, frame sync, the optional
; dry-run sample frame, the register list, a marker, one stopped frame, the
; optional sample frame, the stopped frames, DONE. A sample is the long word
; VPOSR:VHPOSR, then (with CFG+4) the word HHPOSR, stored at the buffer
; pointer; a sample frame ends at VERTB.

        include "nanomig.i"
        include "s2.i"

CFG     equ     $0B8000
SBUF0   equ     $1B0000                 ; the dry-run frame's samples
SBUF1   equ     $1D0000                 ; the first post-list frame's samples
SBUF2   equ     $1E0000                 ; the second one's
M_PRE_END       equ     $F002
M_REGS          equ     $F003
M_POST_END      equ     $F004

        ifnd    FIXVAL
FIXVAL  equ     $BEEF
        endc

; REV, defined before this file is included: each sample is stored below the
; previous one, the buffer being filled downward from its end.
SETBUF  macro
        ifd     REV
        lea     \1+$10000,a2
        else
        lea     \1,a2
        endc
        endm

        org     PROGRAM_ORG

start:
        move.w  #FIXVAL,RESULTS         ; the fixture: a known constant, stored first
        lea     CUSTOM,a0
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$0000,BPLCON0(a0)
        move.w  #$0000,COPCON(a0)
        move.w  #$0020,BEAMCON0(a0)
; the ROM overlay: CIA-A DDRA bit 0 an output, then PRA bit 0 (OVL) low
        move.b  #$03,$BFE201
        move.b  #$00,$BFE001
        lea     irq3(pc),a1
        move.l  a1,($6C).w
        move.w  CFG+4,d4                ; HHPOSR interleaved
        move.w  CFG+0,d0
        beq.s   .nopre
        bsr     fsync
        SETBUF  SBUF0
        bsr     sample
        move.w  #M_PRE_END,MARKER
.nopre: lea     CFG+$10,a1
        move.w  CFG+8,d7
        subq.w  #1,d7
.rl:    move.w  (a1)+,d0
        move.w  (a1)+,d1
        move.w  d1,0(a0,d0.w)
        dbra    d7,.rl
        move.w  #M_REGS,MARKER
        bsr     stopframe
        move.w  CFG+2,d0
        beq.s   .nopost
        SETBUF  SBUF1
        bsr     sample
        move.w  #M_POST_END,MARKER
        cmp.w   #2,d0
        bne.s   .nopost
        SETBUF  SBUF2
        bsr     sample
        move.w  #M_POST_END,MARKER
.nopost:
        move.w  CFG+6,d7
        subq.w  #1,d7
.sf:    bsr     stopframe
        dbra    d7,.sf
        DONE

; fsync: wait for VERTB, clear it, write the frame marker.
fsync:  move.w  #$0020,INTREQ(a0)
.p:     move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .p
        move.w  #$0020,INTREQ(a0)
        move.w  #M_FRAME,MARKER
        rts

; sample: clear VERTB, then store VPOSR:VHPOSR (and HHPOSR when d4 is non-zero)
; at (a2)+ until VERTB is seen again.
sample: move.w  #$0020,INTREQ(a0)
        tst.w   d4
        bne.s   .hh
        ifd     REV
.s1:    sub.l   #4,a2
        move.l  VPOSR(a0),(a2)
        else
.s1:    move.l  VPOSR(a0),(a2)+
        endc
        move.w  INTREQR(a0),d1
        btst    #5,d1
        beq.s   .s1
        rts
        ifd     REV
.hh:    sub.l   #6,a2
        move.l  VPOSR(a0),(a2)
        move.w  HHPOSR(a0),4(a2)
        else
.hh:    move.l  VPOSR(a0),(a2)+
        move.w  HHPOSR(a0),(a2)+
        endc
        move.w  INTREQR(a0),d1
        btst    #5,d1
        beq.s   .hh
        rts

; stopframe: stop the CPU until the next VERTB, then write the frame marker.
stopframe:
        move.w  #$0020,INTREQ(a0)
        move.w  #$C020,INTENA(a0)
        stop    #$2000
        move.w  #M_FRAME,MARKER
        move.w  #$7FFF,INTENA(a0)
        rts

irq3:   move.w  #$0020,$DFF09C
        rte
