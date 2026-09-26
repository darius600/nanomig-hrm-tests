; prog_s_ch3p.s -- S-CH3-P [P7]: ch3's attach-period bit (ADKCON bit 7)
; modulates nothing; all four channels are silent tripwires playing W_P.
; See SPECIFICATION.md, P7.
;
; EXTRA set: ADKCON $8040 instead of $8080 (bit 6: ATPER2 makes ch2
; modulate ch3's period with W_P's own valid words 600 and 1000, so
; ch3's fetch spacing leaves 4000).

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
        ifd     EXTRA
        dc.w    $8040
        else
        dc.w    $8080
        endif
        dc.w    $820F
        dc.w    $000F
        dc.l    60000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
