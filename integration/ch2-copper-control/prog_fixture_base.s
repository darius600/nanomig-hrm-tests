; prog_fixture_base.s -- package-level fixture check: proves peek_logical
; reads back a value the program stored, before this package's own
; DMACONR/INTENAR peeks and record counts are graded through it.
;
;   (no symbol)   -- stores $BEEF
;   FIXTURE_MUT   -- stores $DEAD instead

        include "nanomig.i"

        org     PROGRAM_ORG

start:
        ifd     FIXTURE_MUT
        move.w  #$DEAD,RESULTS
        else
        move.w  #$BEEF,RESULTS
        endif

        DONE
