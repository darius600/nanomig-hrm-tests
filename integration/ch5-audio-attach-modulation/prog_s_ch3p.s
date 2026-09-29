; prog_s_ch3p.s -- S-CH3-P [P7]: ch3's attach-period bit (ADKCON bit 7)
; modulates nothing; all four channels are silent tripwires playing W_P.
; See SPECIFICATION.md, P7.
;
; EXTRA set: ch0's own AUDxPER is rewritten from 1400 to 700 halfway
; through the session, with the ordinary attach write ($8080) unchanged:
; ch0's fetch spacing leaves 2800 -- the honest contrast for P7P.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
wp_data0:       dc.w    $0258,$03E8
        org     CH1_DATA
wp_data1:       dc.w    $0258,$03E8
        org     CH2_DATA
wp_data2:       dc.w    $0258,$03E8
        org     CH3_DATA
wp_data3:       dc.w    $0258,$03E8

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    wp_data0
        dc.w    2,1400,0
        dc.l    wp_data1
        dc.w    2,1600,0
        dc.l    wp_data2
        dc.w    2,1800,0
        dc.l    wp_data3
        dc.w    2,2000,0
        dc.w    $8080
        dc.w    $820F
        dc.w    $000F
        dc.l    60000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        ifd     EXTRA
        move.w  #0,d5
        move.w  #700,d6
        bsr     run_session_mut_ch_per
        else
        bsr     run_session
        endif
        DONE
