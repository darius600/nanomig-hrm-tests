; p3_body.s -- P3 (toggle mode): DDRB bit n<-1, PRB bit n<-0 (SPECIFICATION.md's
; own change from p342, the same one P2 leg S makes); CR<-$00; R-LATCH(LATCHVAL);
; R-SAMPLE(600; PRB, ICR); one more PRB read; CR<-$00. Generic body shared by all four
; instances.
;
; Wrapper symbols: PRB,DDRB,CR,TLO,THI,ICR,M, LATCHVAL, CRVAL.
;   Baseline: LATCHVAL=60, CRVAL=$07 (START,PBON,toggle,continuous).
;   Pre3.1's own contrast: LATCHVAL=2.
;   Pre3.2's own contrast: CRVAL=$06 (not started).
;   Contrasting case: CRVAL=$03 (pulse) or $05 (PBON=0).
;
; SPECIFICATION.md P3.

P3_PRB          equ     RESULTS+8       ; 600 words
P3_ICR          equ     P3_PRB+600*2    ; 600 words
P3_PRB_LAST     equ     P3_ICR+600*2    ; one more PRB read, after the last ICR read

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #M,DDRB
        move.b  #$00,PRB
        move.b  #$00,CR
        RLATCH  TLO,THI,LATCHVAL
        moveq   #0,d0
        move.b  ICR,d0                  ; discard
        move.b  #CRVAL,CR
        RSAMPLE2 600,P3_PRB,PRB,P3_ICR,ICR
        moveq   #0,d0
        move.b  PRB,d0
        move.w  d0,P3_PRB_LAST
        move.b  #$00,CR

        FINISH
