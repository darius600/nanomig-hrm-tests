; ch8_defs.i -- custom chip registers this package needs beyond nanomig.i,
; from the Hardware Reference Manual's Appendix B, Register Summary Address
; Order (p.289-290). Offsets from CUSTOM (nanomig.i defines CUSTOM itself).
; MARK/probe addresses used by the package's own programs (SPECIFICATION.md
; G3): all below $80000, distinct from RESULTS/DONE_ADDR (nanomig.i).

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
RESULT          equ $0C0000     ; ADKCONR readback (RESULTS, nanomig.i)
PROBE_D         equ $0C0002     ; DSKBYTR probe stream
PROBE_I         equ $0C0004     ; INTREQR probe stream (unused space reserved)

; G4: every poll loop is bounded (>= 3x its expected wait), and falls
; through on timeout rather than hanging -- the program always reaches
; DONE. \1 = register offset from a0, \2 = bit number, \3 = bound
; (iteration count, a 32-bit immediate).
POLLBIT         macro
                move.l  #\3-1,d2
.poll\@         move.w  \1(a0),d0
                btst    #\2,d0
                bne.s   .done\@
                subq.l  #1,d2
                bpl.s   .poll\@
.done\@
                endm
