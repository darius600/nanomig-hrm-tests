; rx_core.s -- P5 shared nine-bit receive stimulus (SPECIFICATION.md P5
; "Stimulus"). Included by prog_rx_*.s, each of which sets MUT_LONG (define
; for run A, bit 15/LONG set; leave undefined for run B) and an optional
; precondition-contrast symbol before including this file. The RxD driver
; itself (the schedule s_j, the bit levels) lives in the C++ test, which
; steps the harness one cycle at a time and drives uart_rx; this program
; only ever reads SERDATR/INTREQR.

        include "nanomig.i"
        include "ch8_defs.i"

BND_VERTB       equ 500000
; BND_RBF: tuned well above 3x the per-character step (16.125*T), so it
; still holds when a character's own RBF genuinely never comes (as in the
; 5c contrast's own cascade); large enough to satisfy G4, small enough that
; exhausting it on every one of the eight polls still fits the run's own
; cycle budget (see SPECIFICATION.md 4.1).
BND_RBF         equ 1500
DELAYCOUNT      equ 300

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

        ifd MUT_R1READ
; ---- R1 contrast: clear the ROM overlay, then one CPU read of Chip word
; $000100 (word address $80, below $1000) ----
        move.b  #$03,$BFE201
        move.b  #$00,$BFE001
        move.w  $000100,d5
        endif

; ---- step 2: R4 line-level anchor ----
        ifnd MUT_NOBRK
        move.w  #$8800,ADKCON(a0)      ; UARTBRK set
        endif
        move.w  #MARK_BRK,MARK
        move.w  #DELAYCOUNT-1,d0       ; >= 100 colour clocks (G4-style margin)
.dly:   dbra    d0,.dly
        move.w  #$0800,ADKCON(a0)      ; UARTBRK cleared
        move.w  #MARK_UNBRK,MARK

; ---- step 3: SERPER (N=371; LONG per run) ----
        ifd MUT_LONG
        ifd MUT_ALTPER
; ---- MUTATION (b) contrast: N=185 instead of 371, driver unchanged (p.255:
; N+1 color clocks between samples) ----
        move.w  #$80B9,SERPER(a0)
        else
        move.w  #$8173,SERPER(a0)
        endif
        else
        move.w  #$0173,SERPER(a0)
        endif

; ---- step 4: INTREQ <- $0800 (clear RBF, bit 11), MARK <- READY ----
        move.w  #$0800,INTREQ(a0)
        move.w  #MARK_READY,MARK

; ---- step 5: for j = 0 to 7, unrolled (each poll iteration stores
; INTREQR to PROBE_I, per the spec's own "storing every value" wording) ----

        move.l  #BND_RBF-1,d2
.poll0: move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        ifd MUT_SLOWPOLL
        move.w  #SLOWCOUNT-1,d3
.slow0: dbra    d3,.slow0
        endif
        btst    #11,d0
        bne.s   .rbf0
        subq.l  #1,d2
        bpl.s   .poll0
        ori.w   #1,POLL_TIMEOUT
.rbf0:  move.w  SERDATR(a0),d1
        move.w  d1,RESULT+0
        ifnd MUT_NOCLEAR
        move.w  #$0800,INTREQ(a0)
        endif
        move.w  #1,MARK

        move.l  #BND_RBF-1,d2
.poll1: move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        ifd MUT_SLOWPOLL
        move.w  #SLOWCOUNT-1,d3
.slow1: dbra    d3,.slow1
        endif
        btst    #11,d0
        bne.s   .rbf1
        subq.l  #1,d2
        bpl.s   .poll1
        ori.w   #1,POLL_TIMEOUT
.rbf1:  move.w  SERDATR(a0),d1
        move.w  d1,RESULT+2
        ifnd MUT_NOCLEAR
        move.w  #$0800,INTREQ(a0)
        endif
        move.w  #2,MARK

        move.l  #BND_RBF-1,d2
.poll2: move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        ifd MUT_SLOWPOLL
        move.w  #SLOWCOUNT-1,d3
.slow2: dbra    d3,.slow2
        endif
        btst    #11,d0
        bne.s   .rbf2
        subq.l  #1,d2
        bpl.s   .poll2
        ori.w   #1,POLL_TIMEOUT
.rbf2:  move.w  SERDATR(a0),d1
        move.w  d1,RESULT+4
        ifnd MUT_NOCLEAR
        move.w  #$0800,INTREQ(a0)
        endif
        move.w  #3,MARK

        move.l  #BND_RBF-1,d2
.poll3: move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        ifd MUT_SLOWPOLL
        move.w  #SLOWCOUNT-1,d3
.slow3: dbra    d3,.slow3
        endif
        btst    #11,d0
        bne.s   .rbf3
        subq.l  #1,d2
        bpl.s   .poll3
        ori.w   #1,POLL_TIMEOUT
.rbf3:  move.w  SERDATR(a0),d1
        move.w  d1,RESULT+6
        ifnd MUT_NOCLEAR
        move.w  #$0800,INTREQ(a0)
        endif
        move.w  #4,MARK

        move.l  #BND_RBF-1,d2
.poll4: move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        ifd MUT_SLOWPOLL
        move.w  #SLOWCOUNT-1,d3
.slow4: dbra    d3,.slow4
        endif
        btst    #11,d0
        bne.s   .rbf4
        subq.l  #1,d2
        bpl.s   .poll4
        ori.w   #1,POLL_TIMEOUT
.rbf4:  move.w  SERDATR(a0),d1
        move.w  d1,RESULT+8
        ifnd MUT_NOCLEAR
        move.w  #$0800,INTREQ(a0)
        endif
        move.w  #5,MARK

        move.l  #BND_RBF-1,d2
.poll5: move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        ifd MUT_SLOWPOLL
        move.w  #SLOWCOUNT-1,d3
.slow5: dbra    d3,.slow5
        endif
        btst    #11,d0
        bne.s   .rbf5
        subq.l  #1,d2
        bpl.s   .poll5
        ori.w   #1,POLL_TIMEOUT
.rbf5:  move.w  SERDATR(a0),d1
        move.w  d1,RESULT+10
        ifnd MUT_NOCLEAR
        move.w  #$0800,INTREQ(a0)
        endif
        move.w  #6,MARK

        move.l  #BND_RBF-1,d2
.poll6: move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        ifd MUT_SLOWPOLL
        move.w  #SLOWCOUNT-1,d3
.slow6: dbra    d3,.slow6
        endif
        btst    #11,d0
        bne.s   .rbf6
        subq.l  #1,d2
        bpl.s   .poll6
        ori.w   #1,POLL_TIMEOUT
.rbf6:  move.w  SERDATR(a0),d1
        move.w  d1,RESULT+12
        ifnd MUT_NOCLEAR
        move.w  #$0800,INTREQ(a0)
        endif
        move.w  #7,MARK

        move.l  #BND_RBF-1,d2
.poll7: move.w  INTREQR(a0),d0
        move.w  d0,PROBE_I
        ifd MUT_SLOWPOLL
        move.w  #SLOWCOUNT-1,d3
.slow7: dbra    d3,.slow7
        endif
        btst    #11,d0
        bne.s   .rbf7
        subq.l  #1,d2
        bpl.s   .poll7
        ori.w   #1,POLL_TIMEOUT
.rbf7:
        ifd MUT_5C
; ---- precondition 5c contrast: the read of character 7 is delayed until
; after the driver's ninth character has been received ----
        move.w  #C5DELAY-1,d3
.c5dly: dbra    d3,.c5dly
        endif
        move.w  SERDATR(a0),d1
        move.w  d1,RESULT+14
        ifnd MUT_NOCLEAR
        move.w  #$0800,INTREQ(a0)
        endif
        move.w  #8,MARK

        ifd MUT_FORCETO
; ---- completion contrast: a poll that ended at its bound is declared ----
        ori.w   #1,POLL_TIMEOUT
        endif
        move.w  #MARK_DONE,MARK
        DONE
