; p5b_body.s -- P5 leg (b), per sample: CR<-$00; R-LATCH($FFFF); CR<-$01;
; R-SAMPLE(256; LO register); CR<-$00. Generic body shared by CRA (timer A)
; and CRB (timer B), on both CIAs.
;
; Wrapper symbols: CR,TLO,THI,ICR (LO register sampled is TLO).
;
; SPECIFICATION.md P5.

P5B_LO          equ     RESULTS+8       ; 256 words

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CR
        RLATCH  TLO,THI,$FFFF
        move.b  #$01,CR
        RSAMPLE1 256,P5B_LO,TLO
        move.b  #$00,CR

        DONE
