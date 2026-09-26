; prog_s_pair5.s -- S-PAIR-5 [P6, b=5]: ch1 (M_P4, silent, ADKCON bit 5)
; modulates ch2's period (SQ, target, left). ch0/ch3 are tripwires
; (W_P, silent). See SPECIFICATION.md, P6.
;
; NOATTACH set: omit the attach write. EXTRA set: ADKCON $8030 instead of
; $8020 (bit 4 also set: ch0 -- a tripwire here -- now modulates the
; period of ch1, the modulator itself, so ch1's own fetch spacing leaves
; 8000).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
wp_data0:       dc.w    $0258,$03E8
        org     CH1_DATA
mp4_data:       dc.w    $012C,$02BC,$01F4,$0384
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
        ifd     NOATTACH
        dc.w    $8000
        else
        ifd     EXTRA
        dc.w    $8030
        else
        dc.w    $8020
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
