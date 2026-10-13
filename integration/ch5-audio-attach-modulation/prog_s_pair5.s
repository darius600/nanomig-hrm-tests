; prog_s_pair5.s -- S-PAIR-5 [P6, b=5]: ch1 (M_P4, silent, ADKCON bit 5)
; modulates ch2's period (SQ, target, left). ch0/ch3 are tripwires
; (W_P, silent). See SPECIFICATION.md, P6.
;
; SWAP set: the second and third modulator words are exchanged (P6-A's contrast:
; every value is still a word of the modulator's data set, the order is not).
; EXTRA set: ch3's own AUDxPER is
; rewritten from 2000 to 1000 halfway through the session, with the
; ordinary attach write ($8020) unchanged: ch3's fetch spacing leaves
; 4000 -- clause (d)'s own honest contrast.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
wp_data0:       dc.w    $0258,$03E8
        org     CH1_DATA
mp4_data:
        ifd     SWAP
        dc.w    $012C,$01F4,$02BC,$0384
        else
        dc.w    $012C,$02BC,$01F4,$0384
        endif
        org     CH2_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C
        org     CH3_DATA
wp_data3:       dc.w    $0258,$03E8

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    wp_data0
        dc.w    2,1400,0
        dc.l    mp4_data
        dc.w    4,4000,0
        dc.l    sq_data
        dc.w    4,200,40
        dc.l    wp_data3
        dc.w    2,2000,0
        dc.w    $8020
        dc.w    $820F
        dc.w    $000F
        dc.l    110000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        ifd     EXTRA
        move.w  #3,d5
        move.w  #1000,d6
        bsr     run_session_mut_ch_per
        else
        bsr     run_session
        endif
        DONE
