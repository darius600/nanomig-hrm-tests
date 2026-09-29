; prog_s_pair6.s -- S-PAIR-6 [P6, b=6]: ch2 (M_P4, silent, ADKCON bit 6)
; modulates ch3's period (SQ, target, right). ch0/ch1 are tripwires
; (W_P, silent). See SPECIFICATION.md, P6.
;
; NOATTACH set: omit the attach write. EXTRA set: ch1's own AUDxPER is
; rewritten from 1600 to 800 halfway through the session, with the
; ordinary attach write ($8040) unchanged: ch1's fetch spacing leaves
; 3200 -- clause (d)'s own honest contrast.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
wp_data0:       dc.w    $0258,$03E8
        org     CH1_DATA
wp_data1:       dc.w    $0258,$03E8
        org     CH2_DATA
mp4_data:       dc.w    $012C,$02BC,$01F4,$0384
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
        ifd     NOATTACH
        dc.w    $8000
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
        ifd     EXTRA
        move.w  #1,d5
        move.w  #800,d6
        bsr     run_session_mut_ch_per
        else
        bsr     run_session
        endif
        DONE
