; prog_minper.s -- SPECIFICATION-min-period.md section 3.3: one run plays one
; period value on audio channel 0 and waits a number of lines by beam
; position. The period and the number of lines are two parameter words that
; the test pokes before boot().
;
; Assembly-time symbols select the contrasting cases of the specification
; (each is set by a source of its own that includes this one):
;   MUT_MC2DMA   channel 3 set up on a second table; DMACON $8209 (3.7 MC2)
;   MUT_MC2ADK   ADKCON $8080 written after step 3 (3.7 MC2)
;   MUT_MC3VOL   AUD0VOL <- 0 (3.7 MC3)
;   MUT_MC4LEN   AUD0LEN <- 65, bytes 0 and 2 at T+128 (3.7 MC4)
;   MUT_MC4CPU   one CPU read of the word at T+64 in the middle of the wait (3.7 MC4)
;   MUT_MC5MARK  the done marker is not stored (3.7 MC5)
;   MUT_MC5STOP  DMACON <- $0001 after 100 lines of the wait (3.7 MC5)
;   MUT_MP5CH1   channel 1 set up on the same table, period 64; DMACON $8203 (4 MP5)
;   MUT_FIXTURE  the fixture constant is $DEAD instead of $BEEF

        include "nanomig.i"
        include "ch5regs.i"

CIAADDRA equ    $BFE201
AUD1LCH equ     $0B0
AUD1LEN equ     $0B4
AUD1PER equ     $0B6
AUD3LCH equ     $0D0
AUD3LEN equ     $0D4
AUD3PER equ     $0D6

        org     PROGRAM_ORG

T       equ     $002000         ; 64 words, below $080000 (3.1)
T3      equ     $002200         ; second 64-word table (MC2 contrast)
R       equ     RESULTS+$100    ; result block (3.3 steps 11-14)
KFIX    equ     RESULTS+$120    ; fixture constant
MP_P    equ     RESULTS+$200    ; parameter: AUD0PER value
MP_N    equ     RESULTS+$202    ; parameter: N_run

; Reads the vertical position into d1 (VPOSR, VHPOSR, VPOSR; repeated while
; the two V8 bits differ). Clobbers d2, d3.
RDV     macro
.r\@    move.w  VPOSR(a0),d1
        move.w  VHPOSR(a0),d2
        move.w  VPOSR(a0),d3
        eor.w   d1,d3
        btst    #0,d3
        bne.s   .r\@
        andi.w  #1,d1
        lsl.w   #8,d1
        lsr.w   #8,d2
        or.w    d2,d1
        endm

start:
        ; ---- step 0: the boot overlay (p.223, p.339): DDRA <- $03, PRA <- $02 ----
        move.b  #$03,CIAADDRA
        move.b  #$02,CIAAPRA
        lea     CUSTOM,a0

        ; ---- steps 1-4 ----
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        ifd     MUT_MC2ADK
        move.w  #$8080,ADKCON(a0)
        endc
        move.w  #0,AUD1VOL(a0)
        move.w  #0,AUD2VOL(a0)
        move.w  #0,AUD3VOL(a0)

        ; ---- fixture: a program-stored known value ----
        ifd     MUT_FIXTURE
        move.w  #$DEAD,KFIX
        else
        move.w  #$BEEF,KFIX
        endc

        ; ---- step 5: the table; byte k holds 2k-127 ----
        lea     T,a1
        moveq   #0,d0
        move.w  #63,d3
.tl     move.w  d0,d1
        add.w   d1,d1
        subi.w  #127,d1
        andi.w  #$FF,d1
        move.w  d1,d2
        addq.w  #2,d2
        andi.w  #$FF,d2
        lsl.w   #8,d1
        or.w    d2,d1
        move.w  d1,(a1)+
        addq.w  #2,d0
        dbra    d3,.tl
        ifd     MUT_MC4LEN
        move.w  #$0002,(a1)             ; bytes 0 and 2 at T+128
        endc

        ; ---- steps 6-9 ----
        move.l  #T,AUD0LCH(a0)
        ifd     MUT_MC4LEN
        move.w  #65,AUD0LEN(a0)
        else
        move.w  #64,AUD0LEN(a0)
        endc
        ifd     MUT_MC3VOL
        move.w  #0,AUD0VOL(a0)
        else
        move.w  #64,AUD0VOL(a0)
        endc
        move.w  MP_P,AUD0PER(a0)
        ifd     MUT_MC2DMA
        move.l  #T3,AUD3LCH(a0)
        move.w  #64,AUD3LEN(a0)
        move.w  #447,AUD3PER(a0)
        endc
        ifd     MUT_MP5CH1
        move.l  #T,AUD1LCH(a0)
        move.w  #64,AUD1LEN(a0)
        move.w  #64,AUD1PER(a0)
        endc

        ; ---- step 10 ----
        ifd     MUT_MC2DMA
        move.w  #$8209,DMACON(a0)
        else
        ifd     MUT_MP5CH1
        move.w  #$8203,DMACON(a0)
        else
        move.w  #$8201,DMACON(a0)
        endc
        endc

        ; ---- step 11 ----
        move.w  DMACONR(a0),R+0
        move.w  ADKCONR(a0),R+2

        ; ---- step 12: wait N_run lines by beam position ----
        move.w  MP_N,d7
        move.w  d7,d0
        lsr.w   #1,d0
        movea.w d0,a3                   ; N_run / 2
        moveq   #0,d5                   ; largest vertical position seen
        moveq   #0,d6                   ; lines counted
        RDV
        move.w  d1,d4                   ; the previous vertical position
.wl     RDV
        cmp.w   d4,d1
        beq.s   .wl                     ; no change of line yet
        move.w  d1,d4
        addq.w  #1,d6
        cmp.w   d5,d1
        bls.s   .nomax
        move.w  d1,d5
.nomax
        ifd     MUT_MC5STOP
        cmp.w   #100,d6
        bne.s   .nostop
        move.w  #$0001,DMACON(a0)
.nostop
        endc
        ifd     MUT_MC4CPU
        cmp.w   a3,d6
        bne.s   .nocpu
        move.w  T+64,d0
.nocpu
        endc
        cmp.w   d7,d6
        bne.s   .wl
        move.w  d5,R+4
        move.w  d6,R+6

        ; ---- step 13 ----
        move.w  #$0001,DMACON(a0)

        ; ---- step 14 ----
        ifnd    MUT_MC5MARK
        move.w  #$A5C3,R+8
        endc
        DONE
