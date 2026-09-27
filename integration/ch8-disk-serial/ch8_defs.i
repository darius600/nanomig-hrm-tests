; ch8_defs.i -- custom chip registers this package needs beyond nanomig.i,
; from the Hardware Reference Manual's Appendix B, Register Summary Address
; Order (p.289-290). Offsets from CUSTOM (nanomig.i defines CUSTOM itself).
; MARK/probe addresses used by the package's own programs (SPECIFICATION.md
; G3), distinct from RESULTS/DONE_ADDR (nanomig.i). MARK, RESULT, PROBE_D,
; PROBE_I and POLL_TIMEOUT sit above $80000, not below it: p.223 says that
; while the ROM overlay is on -- the reset default -- "the system RAM that
; would normally be located at this address is not available... Software
; must assume that memory is not available", and every program but P4's own
; disk-buffer fill runs with the overlay on throughout (see SPECIFICATION.md
; section 4).

ADKCONR         equ $010
SERDATR         equ $018
DSKPTH          equ $020
DSKPTL          equ $022
DSKLEN          equ $024
DSKBYTR         equ $01A
SERDAT          equ $030
SERPER          equ $032
ADKCON          equ $09E

; G3 markers: a fixed Chip word, each run's marks distinct from each other
; and from any loop-counter value (k in 2..4 for P1/P2, j+1 in 1..8 for P5).
MARK            equ $0C1000
MARK_BRK        equ $00F1
MARK_UNBRK      equ $00F2
MARK_READY      equ $00F3
MARK_START      equ $00F4
MARK_DONE       equ $00F5

; P4's own addresses (disk write DMA), per SPECIFICATION.md's own stimulus.
DBUF            equ $020000     ; L=256-word raw-MFM buffer, even address (p.246)
RESULT          equ $0C0000     ; ADKCONR readback (RESULTS, nanomig.i); P5's
                                 ; eight-word RESULT array also starts here
                                 ; (RESULT..RESULT+$0E) -- P4 and P5 never run
                                 ; concurrently.
PROBE_D         equ $0C0010     ; DSKBYTR probe stream (kept clear of P5's RESULT array)
PROBE_I         equ $0C0012     ; INTREQR probe stream (kept clear of P5's RESULT array)
POLL_TIMEOUT    equ $0C0020     ; G4: ORed with 1 by POLLBIT whenever a poll's own bound is reached

; G4: every poll loop is bounded (>= 3x its expected wait), and falls
; through on timeout rather than hanging -- the program always reaches
; DONE. A poll that reaches its bound ORs 1 into POLL_TIMEOUT before
; falling through, so a run can be checked for "no poll ended at its
; bound" directly. \1 = register offset from a0, \2 = bit number, \3 =
; bound (iteration count, a 32-bit immediate).
POLLBIT         macro
                move.l  #\3-1,d2
.poll\@         move.w  \1(a0),d0
                btst    #\2,d0
                bne.s   .done\@
                subq.l  #1,d2
                bpl.s   .poll\@
                ori.w   #1,POLL_TIMEOUT
.done\@
                endm
