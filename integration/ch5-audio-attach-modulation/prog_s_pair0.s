; prog_s_pair0.s -- S-PAIR-0 [P6, b=0]: ch0 (M6V, silent, ADKCON bit 0)
; modulates ch1's volume (SQ, target, left). ch2/ch3 are tripwires
; (W_V, silent). See SPECIFICATION.md, P6.
;
; NOATTACH set: omit the attach write (target then keeps VOL 40 always).
; EXTRA set: ADKCON $8005 instead of $8001 (bit 2 also set: p.151 predicts
; ch2 -- a tripwire here -- now modulates ch3's volume with W_V's own
; valid words 48 and 16, so ch3 becomes audible and the "other side
; constant" check must fail).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
m6v_data:       dc.w    $0008,$0030,$0014,$003C
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
        dc.w    2,2000,0
        ifd     NOATTACH
        dc.w    $8000
        else
        ifd     EXTRA
        dc.w    $8005
        else
        dc.w    $8001
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
