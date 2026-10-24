; prog_s_p2.s -- S-P2 [P2, P3]: ch0 (M_P, 17 words, attach period,
; ADKCON bit 4) modulates ch1's period; ch1 plays RAMP, sample for sample,
; at its own volume. See SPECIFICATION.md.
;
; Each define below changes the program of the session and nothing else:
; SWAP: the eighth and ninth modulator words (464 and 272) are exchanged (the
;   contrast of P2-A and P2-B: every value is still a word of M_P, the order is not).
; RAMPSWAP: the second and third words of ch1's own data (RAMP) are exchanged (the
;   contrast of P2-C: the periods are those of M_P, the waveform order is not RAMP's).
; W129: the modulator word 144 ($0090) is 129 ($0081), a whole-word period equal to the
;   period a byte-wise reading of $8190 would give (the contrast of P3-SP2).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mp_data:
        ifd     SWAP
        dc.w    $0190,$0191,$0192,$0194,$0198,$0180,$01B0,$0110
        dc.w    $01D0,$4190,$0090,$8190,$0390,$2190,$0590,$1190
        else
        ifd     W129
        dc.w    $0190,$0191,$0192,$0194,$0198,$0180,$01B0,$01D0
        dc.w    $0110,$4190,$0081,$8190,$0390,$2190,$0590,$1190
        else
        dc.w    $0190,$0191,$0192,$0194,$0198,$0180,$01B0,$01D0
        dc.w    $0110,$4190,$0090,$8190,$0390,$2190,$0590,$1190
        endif
        endif
        dc.w    $0990
        org     CH1_DATA
ramp_data:
        ifd     RAMPSWAP
        dc.w    $0818,$4858,$2838,$6878,$F8E8,$D8C8,$B8A8,$9888
        else
        dc.w    $0818,$2838,$4858,$6878,$F8E8,$D8C8,$B8A8,$9888
        endif

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mp_data
        dc.w    17,35000,63
        dc.l    ramp_data
        dc.w    8,700,48
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.w    $8010
        dc.w    $8203
        dc.w    $0003
        dc.l    2600000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
