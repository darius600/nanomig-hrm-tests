; prog_s_pair4.s -- S-PAIR-4 [P6, b=4]: ch0 (M_P4, silent, ADKCON bit 4)
; modulates ch1's period (SQ, target, left). ch2/ch3 are tripwires
; (W_P, silent). See SPECIFICATION.md, P6.
;
; NOATTACH set: omit the attach write. EXTRA set: ADKCON $8050 instead of
; $8010 (bit 6 also set: ch2 -- a tripwire here -- now modulates ch3's
; period with W_P's own valid words 600 and 1000, so ch3's fetch spacing
; leaves 4000).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mp4_data:       dc.w    $012C,$02BC,$01F4,$0384
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
        ifd     NOATTACH
        dc.w    $8000
        else
        ifd     EXTRA
        dc.w    $8050
        else
        dc.w    $8010
        endif
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
