; ep3_contrast.s -- EP3's own contrast (SPECIFICATION.md 1.2, EP3): the
; program never reaches DONE, so the run has no final marker and no
; graded result.
        include "nanomig.i"
        include "ciaregs.i"

        org     PROGRAM_ORG
start:
        SETUP
never:  bra.s   never
