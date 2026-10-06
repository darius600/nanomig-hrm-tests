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

; ---- step 2: clear the ROM overlay before the CPU fills DBUF.
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
        ifd MUT_SETFAST0
; ---- precondition 4c contrast, FAST=0 run: bit 15 set, so this sets FAST ----
        move.w  #$8100,ADKCON(a0)
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
        move.w  #$C080,DSKLEN(a0)      ; actually starts the DMA
        else
        move.w  #$4000,DSKLEN(a0)
        move.w  #$C100,DSKLEN(a0)      ; DMAEN, WRITE, LENGTH=256
        move.w  #$C100,DSKLEN(a0)      ; actually starts the DMA
        endif
        move.w  #MARK_START,MARK

; ---- step 7: poll, storing DSKBYTR to PROBE_D and INTREQR to PROBE_I
; every iteration, until DSKBLK (INTREQR bit 1) is 1. G4: a poll that
; reaches its own bound ORs 1 into POLL_TIMEOUT before falling through,
; matching POLLBIT's own behaviour (this loop is not POLLBIT itself, since
; it also stores PROBE_D/PROBE_I every iteration). ----
        move.l  #BND_POLL-1,d2
        ifd MUT_MIDSTOP4D
        moveq   #0,d4                  ; one-shot latch for the pause below
        endif
.poll:  move.w  DSKBYTR(a0),d0
        move.w  d0,PROBE_D
        ifd MUT_MIDSTOP4D
; ---- precondition 4d contrast: once the poll count first drops to or below
; MIDSTOP4D_DELAY, clear DSKEN (DMACON bit 4) and keep sampling DSKBYTR into
; PROBE_D for PAUSE4D iterations (so the paused DMAON/DISKWRITE=0 state is
; itself captured, not only the state right before and after the pause),
; then set DMAEN+DSKEN again. The internal DMA-armed state and the pointer
; are untouched by a DMACON-only write, so the same transfer resumes. d4
; latches so this fires once. MIDSTOP4D_DELAY and PAUSE4D are tuned against
; this build's own measured per-word cadence (not page-defined numbers; see
; SPECIFICATION.md) to land the pause inside the t_64..t_192 window. ----
        cmp.l   #MIDSTOP4D_DELAY,d2
        bhi.s   .no4dstop
        tst.l   d4
        bne.s   .no4dstop
        moveq   #1,d4
        move.w  #$0010,DMACON(a0)      ; clear DSKEN
        move.w  #PAUSE4D-1,d3
.p4dwait: move.w DSKBYTR(a0),d0
        move.w  d0,PROBE_D
        dbra    d3,.p4dwait
        move.w  #$8210,DMACON(a0)      ; DMAEN+DSKEN reasserted, resumes
.no4dstop:
        endif
        move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        btst    #1,d0
        bne.s   .polldone
        subq.l  #1,d2
        bpl.s   .poll
        ori.w   #1,POLL_TIMEOUT
.polldone:
        ifd MUT_ONCE
; ---- precondition 4e contrast: the CPU reads word 10 of the buffer, which
; the disk DMA fetched long before, so that word is read twice ----
        move.w  DBUF+20,d5
        endif

; ---- step 8: DSKLEN <- $4000 (stop), MARK <- DONE ----
        move.w  #$4000,DSKLEN(a0)
        ifd MUT_FORCETO
; ---- completion contrast: a poll that ended at its bound is declared ----
        ori.w   #1,POLL_TIMEOUT
        endif
        move.w  #MARK_DONE,MARK

        DONE
