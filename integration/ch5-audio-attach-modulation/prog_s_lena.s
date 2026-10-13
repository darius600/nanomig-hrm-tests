; prog_s_lena.s -- S-LEN-A [P9]: ch0 (M_LA, 5 words, attach volume,
; ADKCON bit 0) modulates ch1 (SQ, 3 words) -- lengths coprime, modulator
; longer than modulated. See SPECIFICATION.md, P9.
;
; Each define below changes the program of the session and nothing else:
; LEN10: AUD0LEN=10 over a table that holds M_LA twice, and the session lasts 260000
;   ticks (P9-A's contrast: the volumes still cycle through M_LA, the offsets do not
;   wrap at 5).
; SWAP: the third and fourth modulator words are exchanged (P9-C's contrast: every
;   value is still a word of M_LA, the order is not).
; CH1LEN2: AUD1LEN=2 instead of 3 (P9-B's contrast: channel 1 then cycles two
;   words, not the three the check expects).
; PER1WRITE: ch1's own AUDxPER is rewritten from 200 to 300 halfway through the
;   session (P9-D's contrast).
; CH1LEN1000: AUD1LEN=1000 (P9-WINDOW's contrast: channel 1 does not restart in
;   the session).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mla_data:
        ifd     SWAP
        dc.w    $0008,$0030,$003C,$0014,$0020
        else
        dc.w    $0008,$0030,$0014,$003C,$0020
        endif
        ifd     LEN10
        dc.w    $0008,$0030,$0014,$003C,$0020
        endif
        org     CH1_DATA
sq_data:        dc.w    $649C,$649C,$649C

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mla_data
        ifd     LEN10
        dc.w    10,4000,63
        else
        dc.w    5,4000,63
        endif
        dc.l    sq_data
        ifd     CH1LEN2
        dc.w    2,200,40
        else
        ifd     CH1LEN1000
        dc.w    1000,200,40
        else
        dc.w    3,200,40
        endif
        endif
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.w    $8001
        dc.w    $8203
        dc.w    $0003
        ifd     LEN10
        dc.l    260000
        else
        dc.l    150000
        endif

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
