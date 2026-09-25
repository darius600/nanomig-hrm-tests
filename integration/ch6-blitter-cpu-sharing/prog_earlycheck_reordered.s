; prog_earlycheck_reordered.s -- A3PRE mutation: prog_earlycheck.s with
; EARLY_REORDER set. The dummy chip-memory read happens before the blit is
; even started, so its own destination write cannot precede it in
; ram_log() -- the unchanged "destination write precedes the dummy read"
; check must go red.

EARLY_REORDER   equ     1
        include "prog_earlycheck.s"
