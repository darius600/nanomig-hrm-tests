; prog_fixture.s -- FIXTURE (no claim id): the program can deliver a known
; value before anything in this package reads one. Stores a known word at
; RESULTS and ends. prog_fixture_mut.s (FIXTURE_BREAK set) stores a
; different word, so the unchanged value check must fail on it.

        include "ch5.i"

        org     PROGRAM_ORG

start:
        move.w  #$2700,sr
        lea     RESULTS,a0
        ifd     FIXTURE_BREAK
        move.w  #$DEAD,(a0)
        else
        move.w  #$BEEF,(a0)
        endif
        DONE
