; prog_s_dac3p.s -- S-DAC3-P [P8]: ch3 alone, attach period (ADKCON bit
; 7), plays W_P at VOL 63. Contrasted against R-DAC3-P (unattached).
; See SPECIFICATION.md, P8.
;
; NOATTACH set: gives R-DAC3-P's own ADKCON word.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH3_DATA
data3:  dc.w    $0258,$03E8

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.l    data3
        dc.w    2,2000,63
        ifd     NOATTACH
        dc.w    $8000
        else
        dc.w    $8080
        endif
        dc.w    $8208
        dc.w    $0008
        dc.l    30000

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)
        lea     desc,a2
        bsr     run_session
        DONE
