; prog_s_pair4.s -- S-PAIR-4 [P6, b=4]: ch0 (M_P4, silent, ADKCON bit 4)
; modulates ch1's period (SQ, target, left). ch2/ch3 are tripwires
; (W_P, silent). See SPECIFICATION.md, P6.
;
; Each define below changes the program of the session and nothing else:
; SWAP: the second and third modulator words are exchanged (P6-A's contrast:
;   every value is still a word of the modulator's data set, the order is not).
; TGTVOL: ch1 (the target)'s own AUDxVOL is rewritten from 40 to 20 halfway through the
;   session, with the attach write unchanged (P6-B's contrast: the target's level pair
;   leaves the reference for VOL 40).
; ADKW: the ADKCON write of step 4 is this word instead of $8010 (P6-C's and P6-D's
;   contrasts: Table 5-5 makes the added bit route another channel's data to a tripwire).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mp4_data:
        ifd     SWAP
        dc.w    $012C,$01F4,$02BC,$0384
        else
        dc.w    $012C,$02BC,$01F4,$0384
        endif
        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C
        org     CH2_DATA
wp_data2:       dc.w    $0258,$03E8
        org     CH3_DATA
wp_data3:       dc.w    $0258,$03E8

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mp4_data
        dc.w    4,4000,0
        dc.l    sq_data
        dc.w    4,200,40
        dc.l    wp_data2
        dc.w    2,1800,0
        dc.l    wp_data3
        dc.w    2,2000,0
        ifd     ADKW
        dc.w    ADKW
        else
        dc.w    $8010
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
        ifd     TGTVOL
        move.w  #1,d5
        move.w  #20,d6
        bsr     run_session_mut_ch_vol
        else
        bsr     run_session
        endif
        DONE
