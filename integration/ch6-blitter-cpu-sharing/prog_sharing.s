; prog_sharing.s -- A2/A4/A6/A8 [p169,186,194,196]: CPU/blitter DMA
; sharing. This file alone (no symbol set) is the BASELINE build: no
; blitter DMA, just the counting loop. prog_sharing_share.s (SHARE set)
; also runs a large, continuously-demanding A-to-D blit with BLTPRI clear
; (share mode). prog_sharing_nasty.s (NASTY set) runs the same blit with
; BLTPRI set (nasty mode, never yields). prog_sharing_tiny.s (SHARE and
; TINY both set) runs the share-mode blit shrunk to a near-instant
; one-word transfer (A2's mutation); prog_sharing_nasty_tiny.s (NASTY and
; TINY both set) does the same in nasty mode (A4PRE's mutation). prog_sharing_aligned.s (ALIGNED set,
; SYSDMA-C29's mutation and A6's own no-DMA reference) replaces the
; counting loop's BRA.S (10 clocks, not a whole multiple of 4) with
; JMP (An) (8 clocks): every instruction's own bus-cycle timing (MC68000
; tables) is then a whole multiple of 4 clocks, so the loop's own period
; never straddles a slot boundary the way BRA.S's does (see SYSDMA-C29 in
; SPECIFICATION.md). prog_sharing_slow.s (SLOW set, A4-SHARE's mutation) runs
; the BASELINE build's loop with two register-only MULU.W instructions
; between its counter writes, from a copy at SLOW_RAM in Zorro II fast RAM, so
; that its instruction fetches do not use the Chip-memory bus and the accesses
; the test classes as the 68000's (a program fetch in the program's range, a
; write to the counter) come far less often than the BASELINE loop's. All run forever; the test runs each for a fixed cycle
; budget and reads the RAM-port log, telling the CPU's own progress
; (writes to SHARE_SCRATCH) apart from the blitter's (writes to the
; SHARE_DST range) by address alone.

        include "nanomig.i"

SHARE_SCRATCH   equ     $0C0100
SHARE_SRC       equ     $0C2000
SHARE_DST       equ     $0C4000

LOOP_ORG        equ     PROGRAM_ORG+$100        ; every setup above is shorter than this
SLOW_RAM        equ     $200000                 ; SLOW only: Zorro II fast RAM, bank 1 (as FR_CODE of prog_fastrun.s)
FR_REG          equ     $E80048                 ; SLOW only: autoconfig "Register 0x48", the write that enables ZII RAM

        org     PROGRAM_ORG

start:
        ifd     SHARE
        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; SET DMAEN,BLTEN (BLTPRI stays clear: share)
        move.w  #$09F0,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #SHARE_SRC,BLTAPTH(a0)
        move.l  #SHARE_DST,BLTDPTH(a0)
        ifd     TINY
        move.w  #$0041,BLTSIZE(a0)     ; height=1,width=1 -- near-instant blit
        else
        move.w  #$7D01,BLTSIZE(a0)     ; height=500,width=1 -- starts the blit
        endif
        endif

        ifd     NASTY
        lea     CUSTOM,a0
        move.w  #$8640,DMACON(a0)      ; SET DMAEN,BLTEN,BLTPRI (nasty: never yields)
        move.w  #$09F0,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #SHARE_SRC,BLTAPTH(a0)
        move.l  #SHARE_DST,BLTDPTH(a0)
        ifd     TINY
        move.w  #$0041,BLTSIZE(a0)     ; height=1,width=1 -- near-instant blit
        else
        move.w  #$7D01,BLTSIZE(a0)     ; height=500,width=1 -- starts the blit
        endif
        endif

        moveq   #0,d0
        lea     SHARE_SCRATCH,a1
        ifd     ALIGNED
        lea     LOOP,a2
        endif
        ifd     SLOW
        move.w  #0,FR_REG.l             ; the write that enables Zorro II fast RAM (not a manual register)
        move.w  #$FFFF,d2               ; MULU.W by $FFFF takes 70 clocks, no bus cycle of its own
        lea     LOOP,a3
        lea     SLOW_RAM,a4
        moveq   #4,d1
.copy:  move.w  (a3)+,(a4)+             ; the five words of the loop below, which is position independent
        dbra    d1,.copy
        jmp     SLOW_RAM.l
        else
        jmp     LOOP.l                  ; the counting loop sits at the same address in every build of
                                        ; prog_sharing.s and prog_bitplane.s, so the same instruction
                                        ; boundary has the same pair of addresses in every capture
        endif

        org     LOOP_ORG
        ifd     ALIGNED
LOOP:   addq.w  #1,d0                   ; 4 clocks (2 slots)
        move.w  d0,(a1)                 ; 8 clocks (4 slots)
        jmp     (a2)                    ; 8 clocks (4 slots) -- aligned, unlike BRA.S's 10
        else
        ifd     SLOW
LOOP:   addq.w  #1,d0
        move.w  d0,(a1)
        mulu.w  d2,d4                   ; register-only work between the Chip-memory accesses
        mulu.w  d2,d4
        bra.s   LOOP
        else
LOOP:   addq.w  #1,d0
        move.w  d0,(a1)
        bra.s   LOOP
        endif
        endif
