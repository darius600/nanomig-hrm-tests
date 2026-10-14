; p5b_body.s -- P5 leg (b), per sample: CR<-$00; R-LATCH($FFFF); CR<-$01;
; R-SAMPLE(256; LO register); CR<-$00. Generic body shared by CRA (timer A)
; and CRB (timer B), on both CIAs.
;
; Wrapper symbols: CR,TLO,THI,ICR (LO register sampled is TLO).
; Optional: MUT_NOSTART (CR<-$00 instead of $01: the timer never runs)
;           MUT_CHIP    (each sample reads a Chip-memory byte, not TLO: no CIA access in the loop)
;           MUT_SHORT   (255 iterations instead of 256)
;
; SPECIFICATION.md P5.

P5B_LO          equ     RESULTS+8       ; 256 words

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CR
        RLATCH  TLO,THI,$FFFF
        ifd     MUT_NOSTART
        move.b  #$00,CR
        else
        move.b  #$01,CR
        endc
        ifd     MUT_CHIP
        RSAMPLE1_CHIP 256,P5B_LO
        else
        ifd     MUT_SHORT
        RSAMPLE1 255,P5B_LO,TLO
        else
        RSAMPLE1 256,P5B_LO,TLO
        endc
        endc
        move.b  #$00,CR

        FINISH
