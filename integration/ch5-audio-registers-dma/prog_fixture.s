; prog_fixture.s -- FIXTURE and FIXTURE-R (no claim id): the program can
; deliver a known value before anything in this package reads one, by both
; paths the propositions use. It stores a known word at RESULTS, which the
; test peeks, and, after clearing the ROM overlay, a known word at R, which
; the test reads back from the RAM-port log the way every proposition reads
; its stored DMACONR values. prog_fixture_mut.s (FIXTURE_BREAK set) stores a
; different word at both, so the unchanged value checks must fail on it.

        include "ch5.i"

        org     PROGRAM_ORG

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     RESULTS,a0
        lea     RSTORE,a1
        ifd     FIXTURE_BREAK
        move.w  #$DEAD,(a0)
        move.w  #$DEAD,(a1)
        else
        move.w  #$BEEF,(a0)
        move.w  #$BEEF,(a1)
        endif
        DONE
