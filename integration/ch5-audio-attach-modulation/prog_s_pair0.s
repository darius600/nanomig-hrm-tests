; prog_s_pair0.s -- S-PAIR-0 [P6, b=0]: ch0 (M6V, silent, ADKCON bit 0)
; modulates ch1's volume (SQ, target, left). ch2/ch3 are tripwires
; (W_V, silent). See SPECIFICATION.md, P6.
;
; SWAP set: the second and third modulator words are exchanged (P6-A's contrast:
; every value is still a word of the modulator's data set, the order is not).
; EXTRA set: ch3 (the tripwire on ch0's own side) is given a non-zero
; starting AUDxVOL (48) instead of 0, with the ordinary attach write
; ($8001) unchanged: ch3 becomes audible on its own, so the "other side
; constant" check must fail -- clause (c)'s own honest contrast.
; TGTPER set: ch1 (the target)'s own AUDxPER is rewritten from 200 to 300
; halfway through the session, with the attach write unchanged -- clause (b)'s
; own contrast (the target's durations leave D_1(200)).

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
        ifd     EXTRA
        dc.w    2,2000,48
        else
        dc.w    2,2000,0
        endif
        dc.w    $8001
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
