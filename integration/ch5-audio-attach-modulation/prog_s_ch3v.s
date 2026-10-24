; prog_s_ch3v.s -- S-CH3-V [P7]: ch3's attach-volume bit (ADKCON bit 3)
; modulates nothing; all four channels are silent tripwires playing W_V.
; See SPECIFICATION.md, P7.
;
; ADKW: the ADKCON write of step 4 is this word instead of $8008 (the contrast of
; P7-CONST: bit 2 is ch2's attach-volume bit, which Table 5-5 makes route ch2's
; words to ch3's volume).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
wv_data0:       dc.w    $0030,$0010
        org     CH1_DATA
wv_data1:       dc.w    $0030,$0010
        org     CH2_DATA
wv_data2:       dc.w    $0030,$0010
        org     CH3_DATA
wv_data3:       dc.w    $0030,$0010

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    wv_data0
        dc.w    2,1400,0
        dc.l    wv_data1
        dc.w    2,1600,0
        dc.l    wv_data2
        dc.w    2,1800,0
        dc.l    wv_data3
        dc.w    2,2000,0
        ifd     ADKW
        dc.w    ADKW
        else
        dc.w    $8008
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
