; prog_fill_desc_nowrite.s -- MUTATION support for PRECONDITION-FILL-DESC:
; prog_fill_desc.s with NOWRITE defined, clearing BLTCON0's USED bit so
; channel D never writes. The blit still runs and BBUSY still clears, but
; the destination keeps the $A5A5 sentinel -- the precondition (word1/word3
; no longer read back as $A5A5) must go red.

NOWRITE equ 1
        include "prog_fill_desc.s"
