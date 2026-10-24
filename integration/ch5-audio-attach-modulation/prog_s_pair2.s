; prog_s_pair2.s -- S-PAIR-2 [P6, b=2]: ch2 (M6V, silent, ADKCON bit 2)
; modulates ch3's volume (SQ, target, right). ch0/ch1 are tripwires
; (W_V, silent). See SPECIFICATION.md, P6.
;
; Each define below changes the program of the session and nothing else:
; SWAP: the second and third modulator words are exchanged (P6-A's contrast:
;   every value is still a word of the modulator's data set, the order is not).
; ADKW: the ADKCON write of step 4 is this word instead of $8004 (P6-C's contrast:
;   Table 5-5 makes the added bit route another channel's data to a tripwire).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
wv_data0:       dc.w    $0030,$0010
        org     CH1_DATA
wv_data1:       dc.w    $0030,$0010
        org     CH2_DATA
m6v_data:
        ifd     SWAP
        dc.w    $0008,$0014,$0030,$003C
        else
        dc.w    $0008,$0030,$0014,$003C
        endif
        org     CH3_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    wv_data0
        dc.w    2,1400,0
        dc.l    wv_data1
        dc.w    2,1600,0
        dc.l    m6v_data
        dc.w    4,4000,0
        dc.l    sq_data
        dc.w    4,200,40
        ifd     ADKW
        dc.w    ADKW
        else
        dc.w    $8004
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
