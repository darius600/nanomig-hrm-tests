; prog_s_pair0.s -- S-PAIR-0 [P6, b=0]: ch0 (M6V, silent, ADKCON bit 0)
; modulates ch1's volume (SQ, target, left). ch2/ch3 are tripwires
; (W_V, silent). See SPECIFICATION.md, P6.
;
; Each define below changes the program of the session and nothing else:
; SWAP: the second and third modulator words are exchanged (P6-A's contrast:
;   every value is still a word of the modulator's data set, the order is not).
; TGTPER: ch1 (the target)'s own AUDxPER is rewritten from 200 to 300 halfway through
;   the session, with the attach write unchanged (P6-B's contrast: the target's
;   durations leave D_1(200)).
; ADKW: the ADKCON write of step 4 is this word instead of $8001 (P6-C's contrast:
;   Table 5-5 makes the added bit route another channel's data to a tripwire).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
m6v_data:
        ifd     SWAP
        dc.w    $0008,$0014,$0030,$003C
        else
        dc.w    $0008,$0030,$0014,$003C
        endif
        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C
        org     CH2_DATA
wv_data2:       dc.w    $0030,$0010
        org     CH3_DATA
wv_data3:       dc.w    $0030,$0010

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    m6v_data
        dc.w    4,4000,0
        dc.l    sq_data
        dc.w    4,200,40
        dc.l    wv_data2
        dc.w    2,1800,0
        dc.l    wv_data3
        dc.w    2,2000,0
        ifd     ADKW
        dc.w    ADKW
        else
        dc.w    $8001
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
        ifd     TGTPER
        move.w  #1,d5
        move.w  #300,d6
        bsr     run_session_mut_ch_per
        else
        bsr     run_session
        endif
        DONE
