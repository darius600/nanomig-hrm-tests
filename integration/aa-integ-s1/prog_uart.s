; prog_uart.s -- P12: SERPER = $001D (one bit every 30 CCK), ADKCON = $0800
; (clears UARTBRK), wait 1 ms, marker M1, ADKCON = $8800 (sets UARTBRK),
; marker M2, wait 0.2 ms, SERDAT = $0155, wait for the transmit shift register
; to go busy and then empty (SERDATR bit 12), wait 0.2 ms, marker M3,
; ADKCON = $0800, marker M4, wait 1 ms, DONE. The waits are dbra loops of 10
; CPU clocks (5 CCK) per iteration: 714 iterations are about 1 ms, 143 about
; 0.2 ms. Symbols defined before the include of this file select the builds:
;   KRUN   step 5 writes ADKCON = $0800 instead of $8800 (contrast K)
;   EARLY  ADKCON = $8800 (UARTBRK set) in place of $0800 at step 2, so TxD is 0 from the start
;   NOFIX  no fixture store
        include "nanomig.i"

SERDAT  equ     $030
SERPER  equ     $032
SERDATR equ     $018
ADKCON  equ     $09E
MARKS   equ     $0B9200                 ; M1..M4 as one word each, 2 bytes apart

        org     PROGRAM_ORG

start:
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS
        endc
        lea     CUSTOM,a0
        move.w  #$001D,SERPER(a0)
        ifd     EARLY
        move.w  #$8800,ADKCON(a0)
        else
        move.w  #$0800,ADKCON(a0)
        endc
        move.w  #713,d2
.w1:    dbra    d2,.w1
        move.w  #1,MARKS                ; M1
        ifd     KRUN
        move.w  #$0800,ADKCON(a0)
        else
        move.w  #$8800,ADKCON(a0)
        endc
        move.w  #2,MARKS+2              ; M2
        move.w  #142,d2
.w2:    dbra    d2,.w2
        move.w  #$0155,SERDAT(a0)
.busy:  move.w  SERDATR(a0),d0
        btst    #12,d0
        bne.s   .busy                   ; wait for TSRE to read 0 (shifting)
.empty: move.w  SERDATR(a0),d0
        btst    #12,d0
        beq.s   .empty                  ; then for TSRE to read 1 (done)
        move.w  #142,d2
.w3:    dbra    d2,.w3
        move.w  #3,MARKS+4              ; M3
        move.w  #$0800,ADKCON(a0)
        move.w  #4,MARKS+6              ; M4
        move.w  #713,d2
.w4:    dbra    d2,.w4
        DONE
