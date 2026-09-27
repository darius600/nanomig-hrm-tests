; tx_core.s -- P1/P2/P3 shared transmit stimulus (SPECIFICATION.md P1
; "Stimulus"). Included by prog_tx_*.s, each of which sets NVAL, WCOUNT
; (and WMULTI when WCOUNT>1), BND_TSRE (a bound for the TSRE polls, at
; least 3x that run's own expected wait, G4) and a mutation symbol before
; including this file. NTSC vs PAL is not a program difference: the
; harness selects it via Config before boot.
;
; BND_VERTB does not depend on N or W: VERTB's expected wait is at most one
; field regardless of SERPER. BND_TBE only bounds the WMULTI loop's
; per-word poll (N=371, the only wrapper that sets WMULTI): TBE resets once
; the shift register empties, so the wait is about one word time,
; 10*(N+1)*C, and BND_TBE is tuned well above 3x that for N=371 on the
; faster-executing core (see SPECIFICATION.md 4.1).

        include "nanomig.i"
        include "ch8_defs.i"

BND_VERTB       equ 500000
BND_TBE         equ 1000

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

        ifd MUT_BRK
; ---- precondition 1c contrast: force TxD low then high before MARK=0 ----
        move.w  #$8800,ADKCON(a0)      ; UARTBRK set (p.257)
        move.w  #$0800,ADKCON(a0)      ; UARTBRK cleared
        endif

; ---- step 2: SERPER <- N, bit15 (LONG/receive-mode select) = 0 ----
        ifd MUT_NPLUS1
        move.w  #NVAL+1,SERPER(a0)
        else
        move.w  #NVAL,SERPER(a0)
        endif

; ---- step 3: INTREQ <- $0001 (clear TBE, bit 0) ----
        move.w  #$0001,INTREQ(a0)

; ---- step 4: MARK <- 0, SERDAT <- $0155 (word 1) ----
        move.w  #0,MARK
        move.w  #$0155,SERDAT(a0)

        ifd WMULTI
; ---- step 5: for k = 2 to WCOUNT ----
        moveq   #2,d1
.loop:
        ifd MUT_BACKTOBACK
; ---- precondition 1d contrast: poll TSRE (bit 12) instead of TBE, so the
; next word is written only once the current one has fully cleared out ----
        POLLBIT SERDATR,12,BND_TSRE
        move.w  #$0155,SERDAT(a0)
        move.w  #$0001,INTREQ(a0)
        move.w  d1,MARK
        else
        POLLBIT INTREQR,0,BND_TBE
        ifd MUT_MISSING2
; ---- precondition 1e contrast: leave out the SERDAT write for word 2,
; still poll and mark as normal ----
        cmp.w   #2,d1
        beq.s   .skipwr
        endif
        move.w  #$0155,SERDAT(a0)
.skipwr:
        move.w  #$0001,INTREQ(a0)
        move.w  d1,MARK
        endif
        addq.w  #1,d1
        cmp.w   #WCOUNT,d1
        ble.s   .loop
        endif

; ---- step 6: poll SERDATR bit 12 (TSRE) until 1, then MARK <- DONE ----
        POLLBIT SERDATR,12,BND_TSRE
        move.w  #MARK_DONE,MARK

        DONE
