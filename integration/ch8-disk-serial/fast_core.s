; fast_core.s -- P4 shared disk-write-DMA stimulus (SPECIFICATION.md P4
; "Stimulus"). Included by prog_fast*.s, each of which sets MUT_FAST1 (define
; for the FAST=1 run, leave undefined for FAST=0), BND_POLL (a poll bound,
; G4) and an optional mutation symbol before including this file. NTSC vs
; PAL is a harness Config choice, not a program difference.

        include "nanomig.i"
        include "ch8_defs.i"

BND_VERTB       equ 500000
DBUF_LEN        equ 256

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0

; ---- G1: interrupts and DMA off ----
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)

; ---- G2: lead-in ----
        move.w  #$0020,INTREQ(a0)
        POLLBIT INTREQR,5,BND_VERTB
        move.w  #$0020,INTREQ(a0)
        POLLBIT INTREQR,5,BND_VERTB
        move.w  #$0020,INTREQ(a0)

; ---- step 2 (repaired): clear the ROM overlay before the CPU fills DBUF.
; The overlay is the reset default (p.223: "the system RAM that would
; normally be located at this address is not available... Software must
; assume that memory is not available"); the OVL bit that disables it lives
; in CIA-A (p.223, "See Appendix F for the bit location"). Appendix F
; (p.339): CIA-A's ddra ($BFE201) is "set to 0x03", and pra ($BFE001) bit 0
; is OVL. DBUF ($020000) is below the overlay window, so without this the
; fill below never reaches Chip RAM. ----
        move.b  #$03,$BFE201
        move.b  #$00,$BFE001

; ---- step 2: fill L=256 words at B=DBUF with $AAAA (valid raw MFM, p.250) ----
        lea     DBUF,a1
        move.w  #DBUF_LEN-1,d1
.fill:  move.w  #$AAAA,(a1)+
        dbra    d1,.fill

; ---- step 3: ADKCON <- $7F00 (clear bits 14-8); FAST=1 also sets bit 8;
; read ADKCONR and store it in RESULT ----
        move.w  #$7F00,ADKCON(a0)
        ifd MUT_FAST1
        ifd MUT_4C
; ---- precondition 4c contrast: bit 15 clear means data bits of 1 CLEAR
; the corresponding ADKCON bit (p.249), so this does not set FAST ----
        move.w  #$0100,ADKCON(a0)
        else
        move.w  #$8100,ADKCON(a0)
        endif
        endif
        move.w  ADKCONR(a0),d0
        move.w  d0,RESULT

; ---- step 4: DSKPTH/DSKPTL <- B, a single long-word write ----
        move.l  #DBUF,DSKPTH(a0)

; ---- step 5: INTREQ <- $0002 (clear DSKBLK, bit 1) ----
        move.w  #$0002,INTREQ(a0)

; ---- step 6: start the DMA (p.247) ----
        move.w  #$8210,DMACON(a0)      ; DMAEN + DSKEN
        ifd MUT_LEN128
        move.w  #$4000,DSKLEN(a0)
        move.w  #$C080,DSKLEN(a0)      ; DMAEN, WRITE, LENGTH=128
        ifnd MUT_NODSKLEN2
        move.w  #$C080,DSKLEN(a0)      ; actually starts the DMA
        endif
        else
        move.w  #$4000,DSKLEN(a0)
        move.w  #$C100,DSKLEN(a0)      ; DMAEN, WRITE, LENGTH=256
        ifnd MUT_NODSKLEN2
        move.w  #$C100,DSKLEN(a0)      ; actually starts the DMA
        endif
        endif
        move.w  #MARK_START,MARK

; ---- step 7: poll, storing DSKBYTR to PROBE_D and INTREQR to PROBE_I
; every iteration, until DSKBLK (INTREQR bit 1) is 1 ----
        move.l  #BND_POLL-1,d2
.poll:  move.w  DSKBYTR(a0),d0
        move.w  d0,PROBE_D
        ifd MUT_MIDSTOP
        cmp.l   #MIDSTOP_DELAY,d2
        bne.s   .nomid
        move.w  #$0100,ADKCON(a0)      ; clear FAST mid-transfer (4f contrast)
.nomid:
        endif
        move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        btst    #1,d0
        bne.s   .polldone
        subq.l  #1,d2
        bpl.s   .poll
.polldone:

; ---- step 8: DSKLEN <- $4000 (stop), MARK <- DONE ----
        move.w  #$4000,DSKLEN(a0)
        move.w  #MARK_DONE,MARK

        DONE
