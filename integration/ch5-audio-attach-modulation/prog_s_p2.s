; prog_s_p2.s -- S-P2 [P2, P3]: ch0 (M_P, 17 words, attach period,
; ADKCON bit 4) modulates ch1's period; ch1 plays RAMP, sample for sample,
; at its own volume. See SPECIFICATION.md.
;
; NOATTACH set: P2's own contrasting case (omit the ADKCON $8010 write;
; ch1 then plays at 700 throughout).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mp_data:        dc.w    $0190,$0191,$0192,$0194,$0198,$0180,$01B0,$01D0
                dc.w    $0110,$4190,$0090,$8190,$0390,$2190,$0590,$1190
                dc.w    $0990
        org     CH1_DATA
ramp_data:      dc.w    $0818,$2838,$4858,$6878,$F8E8,$D8C8,$B8A8,$9888

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
        ifd     NOATTACH
        dc.w    $8000
        else
        dc.w    $8010
        endif
        dc.w    $8203
        dc.w    $0003
        dc.l    1400000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
