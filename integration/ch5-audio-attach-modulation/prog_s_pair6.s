; prog_s_pair6.s -- S-PAIR-6 [P6, b=6]: ch2 (M_P4, silent, ADKCON bit 6)
; modulates ch3's period (SQ, target, right). ch0/ch1 are tripwires
; (W_P, silent). See SPECIFICATION.md, P6.
;
; Each define below changes the program of the session and nothing else:
; SWAP: the second and third modulator words are exchanged (P6-A's contrast:
;   every value is still a word of the modulator's data set, the order is not).
; ADKW: the ADKCON write of step 4 is this word instead of $8040 (P6-C's and P6-D's
;   contrasts: Table 5-5 makes the added bit route another channel's data to a tripwire).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
wp_data0:       dc.w    $0258,$03E8
        org     CH1_DATA
wp_data1:       dc.w    $0258,$03E8
        org     CH2_DATA
mp4_data:
        ifd     SWAP
        dc.w    $012C,$01F4,$02BC,$0384
        else
        dc.w    $012C,$02BC,$01F4,$0384
        endif
        org     CH3_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    wp_data0
        dc.w    2,1400,0
        dc.l    wp_data1
        dc.w    2,1600,0
        dc.l    mp4_data
        dc.w    4,4000,0
        dc.l    sq_data
        dc.w    4,200,40
        ifd     ADKW
        dc.w    ADKW
        else
        dc.w    $8040
        endif
        dc.w    $820F
        dc.w    $000F
        dc.l    110000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
