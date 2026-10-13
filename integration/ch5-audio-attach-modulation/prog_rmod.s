; prog_rmod.s -- R-MOD: channel 0 alone, unattached, plays M_V (p.145
; steps 3-7; ADKCON attach bits clear). Serves as: the unit anchor for
; D_mod and PC6/PC7 (SPECIFICATION.md Conventions/Units), P3's control
; case (an unattached channel plays its data set as two bytes per word,
; p.138), and P10's normal-operation fetch-phase anchor.
; PC6MUT sets AUD0PER=5000 halfway through (PC6's own contrast); PER3000MUT
; sets it to 3000 (PC7's own contrast). FRAMEMUT writes M_on late, after a
; delay following the DMACON enable (PC7 frame's own contrast). None of the
; three is combined with another. NODMAMUT omits DMAEN from the DMACON enable
; (PC2's own contrast), LEN7MUT sets AUD0LEN = 7 (PC3's own contrast), PER6000MUT
; sets AUD0PER = 6000 from the start (PC7-CAL's own contrast) and VOL0MUT sets
; AUD0VOL = 0 (P3-PRE's and P8-PRE's contrast).

        include "nanomig.i"
        include "ch5_regs.i"

        org     CH0_DATA
mv_data:        dc.w    $0020,$0001,$0002,$0004,$0008,$0010,$003F,$0015

        org     PROGRAM_ORG
        bra     start
        include "ch5_subs.i"

desc:
        dc.l    mv_data
        ifd     VOL0MUT
        dc.w    8,4000,0
        else
        ifd     LEN7MUT
        dc.w    7,4000,63
        else
        ifd     PER6000MUT
        dc.w    8,6000,63
        else
        dc.w    8,4000,63
        endif
        endif
        endif
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.l    0
        dc.w    0,0,0
        dc.w    $8000                   ; ADKCON: no attach bits (mask $00)
        ifd     NODMAMUT
        dc.w    $8001                   ; DMACON on: AUD0EN only, DMAEN omitted
        else
        dc.w    $8201                   ; DMACON on: DMAEN + AUD0EN
        endif
        dc.w    $0001                   ; DMACON off
        dc.l    200000                  ; duration, ticks

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$00FF,ADKCON(a0)

        ifd     PC6MUT
        lea     desc,a2
        bsr     run_session_half_then_mutate_per
        bra.s   .done
        endif
        ifd     PER3000MUT
        lea     desc,a2
        bsr     run_session_half_then_mutate_per3000
        bra.s   .done
        endif
        ifd     FRAMEMUT
        lea     desc,a2
        bsr     run_session_mon_late
        bra.s   .done
        endif

        lea     desc,a2
        bsr     run_session
.done:
        DONE

        ifd     PC6MUT
; PC6's own contrast: AUD0PER=5000 written halfway through R-MOD's
; duration (PC6: "CPU writes AUD0PER = 5000 halfway through R-MOD: the
; spread fails"). Runs the descriptor's steps 3-6 as run_session would,
; but splits the step-7 wait in half around the extra write, and skips
; straight to step 8 afterwards (duplicated here rather than parametrising
; run_session, which every other program relies on staying exactly the
; sequence SPECIFICATION.md gives).
run_session_half_then_mutate_per:
        lea     CUSTOM,a0
        move.l  (a2),a1
        move.w  4(a2),d1
        move.w  6(a2),d2
        move.w  8(a2),d3
        moveq   #0,d0
        bsr     setup_channel
        lea     CUSTOM,a0
        move.w  40(a2),ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  42(a2),DMACON(a0)
        move.w  #1,MARKER
        move.w  DMACONR(a0),DMACR_STORE

        move.l  46(a2),d0
        lsr.l   #1,d0
        bsr     wait_ticks_via_lines
        move.w  #5000,AUD0PER(a0)
        move.w  #3,MARKER
        move.l  46(a2),d0
        lsr.l   #1,d0
        bsr     wait_ticks_via_lines

        move.w  44(a2),DMACON(a0)
        move.w  #2,MARKER
        move.w  #$00FF,ADKCON(a0)
        rts
        endif

        ifd     FRAMEMUT
; PC7 frame's own contrast: DMACON is enabled, then the CPU waits briefly
; before M_on is written (p.164: "DMA requests are sent to Agnus as soon
; as DMA is turned on"), so a fetch can occur before the marker.
run_session_mon_late:
        lea     CUSTOM,a0
        move.l  (a2),a1
        move.w  4(a2),d1
        move.w  6(a2),d2
        move.w  8(a2),d3
        moveq   #0,d0
        bsr     setup_channel
        lea     CUSTOM,a0
        move.w  40(a2),ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  42(a2),DMACON(a0)      ; DMA enabled here
        move.l  #500,d0
        bsr     wait_ticks_via_lines   ; delay before the marker
        move.w  #1,MARKER              ; M_on, written late
        move.w  DMACONR(a0),DMACR_STORE

        move.l  46(a2),d0
        bsr     wait_ticks_via_lines

        move.w  44(a2),DMACON(a0)
        move.w  #2,MARKER
        move.w  #$00FF,ADKCON(a0)
        rts
        endif

        ifd     PER3000MUT
; PC7's own contrast: AUD0PER=3000 written halfway through (PC7: "CPU
; writes AUD0PER = 3000 halfway through R-MOD: the offsets leave the
; window").
run_session_half_then_mutate_per3000:
        lea     CUSTOM,a0
        move.l  (a2),a1
        move.w  4(a2),d1
        move.w  6(a2),d2
        move.w  8(a2),d3
        moveq   #0,d0
        bsr     setup_channel
        lea     CUSTOM,a0
        move.w  40(a2),ADKCON(a0)
        move.w  ADKCONR(a0),ADKR_STORE
        move.w  42(a2),DMACON(a0)
        move.w  #1,MARKER
        move.w  DMACONR(a0),DMACR_STORE

        move.l  46(a2),d0
        lsr.l   #1,d0
        bsr     wait_ticks_via_lines
        move.w  #3000,AUD0PER(a0)
        move.w  #3,MARKER
        move.l  46(a2),d0
        lsr.l   #1,d0
        bsr     wait_ticks_via_lines

        move.w  44(a2),DMACON(a0)
        move.w  #2,MARKER
        move.w  #$00FF,ADKCON(a0)
        rts
        endif
