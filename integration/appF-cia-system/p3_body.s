; p3_body.s -- P3 (toggle mode): DDRB<-$00; CR<-$00; R-LATCH(LATCHVAL);
; R-SAMPLE(600; PRB, ICR); CR<-$00. Generic body shared by all four
; instances.
;
; Wrapper symbols: PRB,DDRB,CR,TLO,THI,ICR, LATCHVAL, CRVAL.
;   Baseline: LATCHVAL=60, CRVAL=$07 (START,PBON,toggle,continuous).
;   Pre3.1's own contrast: LATCHVAL=2.
;   Pre3.2's own contrast: CRVAL=$06 (not started).
;   Contrasting case: CRVAL=$03 (pulse) or $05 (PBON=0).
;
; SPECIFICATION.md P3.

P3_PRB          equ     RESULTS+8       ; 600 words
P3_ICR          equ     P3_PRB+600*2    ; 600 words

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,DDRB
        move.b  #$00,CR
        RLATCH  TLO,THI,LATCHVAL
        moveq   #0,d0
        move.b  ICR,d0                  ; discard
        move.b  #CRVAL,CR
        RSAMPLE2 600,P3_PRB,PRB,P3_ICR,ICR
        move.b  #$00,CR

        DONE
