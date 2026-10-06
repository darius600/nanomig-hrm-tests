; prog_s_av.s -- S-AV [P1, P3, P5, P8, P10]: ch0 (M_V, attach volume,
; ADKCON bit 0) modulates ch1 (SQ). See SPECIFICATION.md.
;
; NOATTACH set: P1's own contrasting case (omit the ADKCON $8001 write;
; PC1 then expects mask $00). PER6000 set: P5's own contrasting case
; (AUD0PER=6000 instead of 4000, page-predicted spacing becomes wrong
; against the check's own D_mod from R-MOD). MV33 set: the last modulator word
; is 33 instead of 21, a volume with no reference level pair (P1-B's contrast).
; PER1WRITE set: ch1's own AUDxPER is rewritten from 200 to 300 halfway through
; the session (P1-C's contrast).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mv_data:
        ifd     MV33
        dc.w    $0020,$0001,$0002,$0004,$0008,$0010,$003F,$0021
        else
        dc.w    $0020,$0001,$0002,$0004,$0008,$0010,$003F,$0015
        endif
        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mv_data
        ifd     PER6000
        dc.w    8,6000,63
        else
        dc.w    8,4000,63
        endif
        dc.l    sq_data
        dc.w    4,200,40
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        ifd     NOATTACH
        dc.w    $8000
        else
        dc.w    $8001
        endif
        dc.w    $8203
        dc.w    $0003
        dc.l    200000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        ifd     PER1WRITE
        move.w  #1,d5
        move.w  #300,d6
        bsr     run_session_mut_ch_per
        else
        bsr     run_session
        endif
        DONE
