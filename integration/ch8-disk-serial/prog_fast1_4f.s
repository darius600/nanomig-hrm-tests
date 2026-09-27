; Precondition 4f contrast: FAST=1, but partway through the transfer the
; program writes ADKCON <- $0100 (clearing FAST), predicting 4us cells for
; the second half. MIDSTOP_DELAY is a poll-iteration count tuned against
; this build's own measured per-word cadence (not a page-defined number; see
; SPECIFICATION.md) to land at approximately word 128 of 256.
MUT_FAST1       equ 1
MUT_MIDSTOP     equ 1
BND_POLL        equ 200000
MIDSTOP_DELAY   equ 100000
        include "fast_core.s"
