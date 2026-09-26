; prog_s_dac3v.s -- S-DAC3-V [P8]: ch3 alone, attach volume (ADKCON bit
; 3), plays W_V at VOL 63. Contrasted against R-DAC3-V (unattached).
; See SPECIFICATION.md, P8.
;
; NOATTACH set: gives R-DAC3-V's own ADKCON word (omits the attach bit),
; which the page predicts shows ch3's bytes on the right.

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH3_DATA
data3:  dc.w    $0030,$0010

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
        dc.w    $8008
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
