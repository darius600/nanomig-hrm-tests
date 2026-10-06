; prog_s_lena.s -- S-LEN-A [P9]: ch0 (M_LA, 5 words, attach volume,
; ADKCON bit 0) modulates ch1 (SQ, 3 words) -- lengths coprime, modulator
; longer than modulated. See SPECIFICATION.md, P9.
;
; LEN4 set: P9's own contrasting case (AUD0LEN=4 instead of 5; p.147
; predicts a 4-word cycle 8,48,20,60 instead of the true 5-word
; 8,48,20,60,32). CH1LEN2 set: AUD1LEN=2 instead of 3 (P9 (b)'s own contrasting
; case: channel 1 then cycles two words, not the three the check expects).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mla_data:       dc.w    $0008,$0030,$0014,$003C,$0020
        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mla_data
        ifd     LEN4
        dc.w    4,4000,63
        else
        dc.w    5,4000,63
        endif
        dc.l    sq_data
        ifd     CH1LEN2
        dc.w    2,200,40
        else
        dc.w    3,200,40
        endif
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.w    $8001
        dc.w    $8203
        dc.w    $0003
        dc.l    150000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
