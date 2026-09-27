; prog_diskrate.s -- P6 (SPECIFICATION.md section 3), and F1/F3 (2.4):
; disk write DMA takes a word per 16 bit cells of about 2 us with FAST=1
; and about 4 us with FAST=0. Two runs in sequence, F (FAST=1, buffer A)
; then S (FAST=0, buffer B), each following the manual's own DSKLEN
; sequence (p.247 steps 1-5). No drive is selected and the program makes
; no CIA-B write, so no floppy select/motor line is ever driven (2.9);
; the one CIA-A write (CLEAR_OVL) is needed only so that chip bank 0,
; where the two buffers sit (both below $080000, 2.1), is real memory
; instead of the boot-time Kickstart-shadow window (see prog_audio.s's
; header for the same mechanism, and SPECIFICATION.md section 5).
;
; Buffers A and B are poked with $AAAA in every word by the C++ test
; before boot() (p.249/p.250: a run of data zeros encodes as $AAAA),
; the same direct-preload mechanism prog_audio.s's tables use.
;
; Optional assembly-time symbols (set by an including mutant source):
;   MUT_FASTF0  run F's ADKCON write becomes $0100 instead of $8100
;               (B1 for run F's own contrasting case)
;   MUT_FASTS1  run S's ADKCON write becomes $8100 instead of $0100
;               (B1 for run S's own contrasting case)
;   MUT_B2      run F's DMACON write becomes $8010 instead of $8210
;               (DMAEN, bit 9, clear -- B2's own contrasting case; p.247,
;               p.248 and p.278 agree DMAON then reads 0)
;   MUT_B3      run F's steps 5 and 6 use LENGTH=100 (DSKLEN=$C064) instead of 544
;   MUT_F1      before run F, write VTOTAL/HTOTAL/BEAMCON0 (ECS only):
;               F1's and B4's own contrasting case. MUT_F1_NTSC selects
;               the NTSC BEAMCON0 value ($0080) in place of the PAL one
;               ($00A0), per SPECIFICATION.md 2.4.

        include "nanomig.i"
        include "ch5regs.i"

        org     PROGRAM_ORG

DISKBUF_A       equ     $003000         ; 544 words, below $080000 (2.1)
DISKBUF_B       equ     $004000         ; 544 words

R_B1_F          equ     RESULTS+0
R_B2_F          equ     RESULTS+2
R_SEEN_F        equ     RESULTS+4       ; not graded
R_B1_S          equ     RESULTS+6
R_B2_S          equ     RESULTS+8
R_SEEN_S        equ     RESULTS+10      ; not graded

; Poll DSKBLK (INTREQR bit 1), bounded by up to 8 VERTB pulses (2.3, P6
; step 8). Leaves D4 = 1 if seen, 0 otherwise; not graded (2.9). Each
; VERTB wait itself has its own bounded fallback loop counter, so the
; whole thing can never hang. Clobbers d4, d5, d7.
POLL_DSKBLK     macro
                moveq   #8-1,d5
.pdbloop\@      btst    #1,INTREQR(a0)
                bne.s   .pdbseen\@
                move.l  #200000,d7
.pdbvbl\@       btst    #5,INTREQR(a0)
                bne.s   .pdbvbld\@
                dbra    d7,.pdbvbl\@
.pdbvbld\@      move.w  #$0020,INTREQ(a0)
                dbra    d5,.pdbloop\@
                moveq   #0,d4
                bra.s   .pdbdone\@
.pdbseen\@      moveq   #1,d4
.pdbdone\@
                endm

start:
        CLEAR_OVL
        lea     CUSTOM,a0

        ; ---- preamble (2.3) ----
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,ADKCON(a0)

        ifd     MUT_F1
        ; F1/B4's own contrasting case (2.4, ECS only): before the measured
        ; runs, write VTOTAL, then HTOTAL, then BEAMCON0, exactly as the page
        ; states.
        move.w  #524,VTOTAL(a0)
        move.w  #113,HTOTAL(a0)
        ifd     MUT_F1_NTSC
        move.w  #$0080,BEAMCON0(a0)    ; VARBEAMEN, NTSC (p.304, p.305)
        else
        move.w  #$00A0,BEAMCON0(a0)    ; VARBEAMEN + PAL (p.304, p.305)
        endc
        endc

        ; ==== run F: FAST = 1, buffer A ====
        ifd     MUT_FASTF0
        move.w  #$0100,ADKCON(a0)      ; FASTF0 mutation: clears bit 8 instead of setting it
        else
        move.w  #$8100,ADKCON(a0)
        endc
        move.w  ADKCONR(a0),R_B1_F
        ifd     MUT_B2
        move.w  #$8010,DMACON(a0)              ; B2 mutation: DMAEN (bit 9) clear
        else
        move.w  #$8210,DMACON(a0)              ; SET, DMAEN, DSKEN (p.247 step 1, p.278)
        endc
        move.l  #DISKBUF_A,DSKPTH(a0)           ; one 32-bit write (p.246)
        move.w  #$4000,DSKLEN(a0)               ; p.247 step 2
        ifd     MUT_B3
        move.w  #$C064,DSKLEN(a0)               ; B3 mutation: LENGTH=100
        move.w  #$C064,DSKLEN(a0)
        else
        move.w  #$C220,DSKLEN(a0)               ; DMAEN, WRITE, LENGTH=544 (p.247 step 3)
        move.w  #$C220,DSKLEN(a0)               ; p.247 step 4: starts the DMA
        endc
        move.w  DSKBYTR(a0),R_B2_F
        POLL_DSKBLK
        move.w  d4,R_SEEN_F
        move.w  #$4000,DSKLEN(a0)               ; p.247 step 5
        move.w  #$0002,INTREQ(a0)

        ; ==== run S: FAST = 0, buffer B ====
        ifd     MUT_FASTS1
        move.w  #$8100,ADKCON(a0)      ; FASTS1 mutation: sets bit 8 instead of clearing it
        else
        move.w  #$0100,ADKCON(a0)
        endc
        move.w  ADKCONR(a0),R_B1_S
        move.w  #$8210,DMACON(a0)
        move.l  #DISKBUF_B,DSKPTH(a0)
        move.w  #$4000,DSKLEN(a0)
        move.w  #$C220,DSKLEN(a0)
        move.w  #$C220,DSKLEN(a0)
        move.w  DSKBYTR(a0),R_B2_S
        POLL_DSKBLK
        move.w  d4,R_SEEN_S
        move.w  #$4000,DSKLEN(a0)
        move.w  #$0002,INTREQ(a0)

        DONE
