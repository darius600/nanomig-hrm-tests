; Precondition 4d contrast: FAST=1, but partway through the transfer DSKEN
; (DMACON bit 4) is cleared for a while and then reasserted -- some PROBE_D
; samples inside the window then show DMAON = 0 (DISKWRITE stays 1: DSKLEN is
; not written), while the same transfer resumes and still fills the whole
; t_64..t_192 window.
; MIDSTOP4D_DELAY and PAUSE4D are tuned against this build's own measured
; per-word cadence (not page-defined numbers; see SPECIFICATION.md).
MUT_FAST1       equ 1
MUT_MIDSTOP4D   equ 1
BND_POLL        equ 200000
MIDSTOP4D_DELAY equ 199819
PAUSE4D         equ 40
        include "fast_core.s"
