; prog_s_pair2.s -- S-PAIR-2 [P6, b=2]: ch2 (M6V, silent, ADKCON bit 2)
; modulates ch3's volume (SQ, target, right). ch0/ch1 are tripwires
; (W_V, silent). See SPECIFICATION.md, P6.
;
; NOATTACH set: omit the attach write. EXTRA set: ADKCON $8005 instead of
; $8004 (bit 0 also set: ch0 -- a tripwire here -- now modulates ch1's
; volume, and the left side becomes audible).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
wv_data0:       dc.w    $0030,$0010
        org     CH1_DATA
wv_data1:       dc.w    $0030,$0010
        org     CH2_DATA
m6v_data:       dc.w    $0008,$0030,$0014,$003C
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
        ifd     NOATTACH
        dc.w    $8000
        else
        ifd     EXTRA
        dc.w    $8005
        else
        dc.w    $8004
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
