; p4_body.s -- P4 (pulse mode): DDRB<-$00; CR<-$00; R-LATCH(LATCHVAL);
; R-SAMPLE(400; PRB, ICR); CR<-$00. Generic body shared by all four
; instances.
;
; Wrapper symbols: PRB,DDRB,CR,TLO,THI,ICR, LATCHVAL, CRVAL.
;   Baseline: CRVAL=$03 (START,PBON,pulse,continuous).
;   Contrasting case: CRVAL=$07 (toggle, same L) or $02 (not started).
;
; SPECIFICATION.md P4.

P4_PRB          equ     RESULTS+8       ; 400 words
P4_ICR          equ     P4_PRB+400*2    ; 400 words

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,DDRB
        move.b  #$00,CR
        RLATCH  TLO,THI,LATCHVAL
        moveq   #0,d0
        move.b  ICR,d0                  ; discard
        move.b  #CRVAL,CR
        RSAMPLE2 400,P4_PRB,PRB,P4_ICR,ICR
        move.b  #$00,CR

        DONE
