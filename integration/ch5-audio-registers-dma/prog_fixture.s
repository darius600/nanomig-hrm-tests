; prog_fixture.s -- FIXTURE and FIXTURE-R (no claim id): the program can
; deliver a known value before anything in this package reads one, by both
; paths the propositions use. It stores a known word at RESULTS, which the
; test peeks, and, after clearing the ROM overlay, a known word at R, which
; the test reads back from the RAM-port log the way every proposition reads
; its stored DMACONR values. Each check has its own mutation:
; prog_fixture_mut.s (FIXTURE_BREAK set) stores a different word at RESULTS
; only, prog_fixture_mutr.s (FIXTURE_BREAK_R set) stores a different word at
; R only, so the unchanged value check of the one line must fail on it and
; the other line must not.

        include "ch5.i"

        org     PROGRAM_ORG

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     RESULTS,a0
        lea     RSTORE,a1
        ifd     FIXTURE_BREAK
        move.w  #$DEAD,(a0)
        else
        move.w  #$BEEF,(a0)
        endif
        ifd     FIXTURE_BREAK_R
        move.w  #$DEAD,(a1)
        else
        move.w  #$BEEF,(a1)
        endif
        DONE
