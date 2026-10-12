; prog_p8.s -- P8: BBUSY (bit 14) reads 1 while a blit is in progress and 0
; when no blit is in progress (SPECIFICATION.md P8). Every DMACONR read is
; a register access (not on the RAM port); the test reads each value from
; the data field of the log write that stores it into GLOG, in order:
; 4 idle reads, then the poll (index 1 not graded, index >= 2 checked
; against T_last), then 8 more unconditional reads.
;
; This file alone is the MAIN build. prog_p8_c1.s (C1 set) is the
; contrasting case: the BLTSIZE write is replaced by the same MOVE.W to
; COLOR00, so no blit starts; it is also the mutation of P8-G3 ("the blit
; ran").
; prog_p8_g1.s (G1_BREAK), prog_p8_gbltpri.s (G_BLTPRI, sets BLTPRI in
; step 2) and prog_p8_g4.s (G4_LATE, a delay between BLTSIZE and the first
; poll read) are the precondition mutations.

        include "ch5.i"

        org     PROGRAM_ORG

OM      equ     $0060
CM      equ     $0061

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     RSTORE,a3
        lea     GLOG,a4
        lea     MK,a5

        move.w  #$07FF,DMACON(a0)
        move.w  #$01FF,BLTCON0(a0)
        move.w  #0,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTDMOD(a0)
        move.l  #QQ,BLTDPTH(a0)

        ifd     G_BLTPRI
        move.w  #$8640,DMACON(a0)       ; BLTPRI set (precondition mutation)
        else
        move.w  #$8240,DMACON(a0)       ; BLTPRI clear (share mode, p.198)
        endif
        move.w  DMACONR(a0),(a3)+

        move.w  DMACONR(a0),d0
        move.w  d0,(a4)+
        move.w  DMACONR(a0),d0
        move.w  d0,(a4)+
        move.w  DMACONR(a0),d0
        move.w  d0,(a4)+
        move.w  DMACONR(a0),d0
        move.w  d0,(a4)+

        move.w  #OM,(a5)
        ifd     C1
        move.w  #$2020,COLOR00(a0)      ; contrast: no blit starts
        else
        move.w  #$2020,BLTSIZE(a0)      ; height=128,width=32 -- starts the blit
        endif
        ifd     G4_LATE
        move.l  #6000,d0                ; precondition mutation: the first poll read is made after the blit
.late:  subq.l  #1,d0
        bne.s   .late
        endif

        moveq   #0,d2                    ; running read index
        move.l  #8192,d3
.poll:
        move.w  DMACONR(a0),d0
        move.w  d0,(a4)+
        addq.l  #1,d2
        cmp.l   #1,d2
        beq.s   .cont                    ; index 1 is never graded
        btst    #14,d0
        beq.s   .stopped
.cont:  cmp.l   d3,d2
        blt.s   .poll
.stopped:
        moveq   #7,d1
.extra: move.w  DMACONR(a0),d0
        move.w  d0,(a4)+
        dbra    d1,.extra

        move.w  #CM,(a5)

        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        DONE
