; p_unused_body.s -- P7.7 (CRA bit 7, unused: unaffected by a write, forced
; to 0 on a read). Generic body, CRA only (CRB's bit 7 is ALARM, P8.7).
;
; Wrapper symbols: CR,TLO,THI, FIRSTVAL (the unbracketed first write),
; SECONDVAL (the bracketed second write).
;
; SPECIFICATION.md P7.7.

PUN_C1          equ     RESULTS+8
PUN_C2          equ     RESULTS+10
PUN_W           equ     RESULTS+12

MK_UN_A         equ     $0C1002         ; before the bracketed (second) write
MK_UN_B         equ     $0C1004         ; after the bracketed (second) write
MK_UN_C         equ     $0C1006         ; before the stop write
MK_UN_D         equ     $0C1008         ; after the stop write

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #FIRSTVAL,CR
        READ_STORE CR,PUN_C1
        RLATCH  TLO,THI,$8000
        move.w  #MARKER_SENTINEL,MK_UN_A
        move.b  #SECONDVAL,CR
        move.w  #MARKER_SENTINEL,MK_UN_B
        READ_STORE CR,PUN_C2
        move.w  #$03E8,d7              ; ~1000 E periods
.d1:    dbra    d7,.d1
        move.w  #MARKER_SENTINEL,MK_UN_C
        move.b  #$00,CR
        move.w  #MARKER_SENTINEL,MK_UN_D
        READ_TIMER THI,TLO,PUN_W

        DONE
