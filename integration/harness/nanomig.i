; nanomig.i -- what a test program run by nanomig_harness.h may rely on.
;
; Assemble with   vasmm68k_mot -Fbin -I<this directory> -o prog.bin prog.s
; and start the source with   include "nanomig.i"   and   org PROGRAM_ORG.
; The harness loads prog.bin at PROGRAM_ORG, points the reset vectors at it
; (SSP = PROGRAM_STACK) and runs until the program executes DONE, or a cycle
; budget runs out.
;
; Program, stack and results all live in chip RAM that nanomig.v maps
; one-to-one ($080000-$1FFFFF with 2 MB chip RAM): the harness reads a result
; there as the word the program stored, and never places code in the overlay
; window, which any write to CIA-A switches away.

PROGRAM_ORG     equ $080400     ; first word of the program (nanomig_harness.h kProgramOrg)
PROGRAM_STACK   equ $0FFF00     ; initial supervisor stack pointer (kProgramStack)
RESULTS         equ $0C0000     ; where a program stores what it measured
DONE_ADDR       equ $0C0FFE     ; DONE writes DONE_VALUE here, last
DONE_VALUE      equ $600D

; Custom chip registers, from the Hardware Reference Manual's Appendix B,
; Register Summary Address Order. Offsets from CUSTOM.
CUSTOM          equ $DFF000
DMACONR         equ $002
VPOSR           equ $004
VHPOSR          equ $006
INTENAR         equ $01C
INTREQR         equ $01E
BLTCON0         equ $040
BLTCON1         equ $042
BLTAFWM         equ $044
BLTALWM         equ $046
BLTCPTH         equ $048
BLTCPTL         equ $04A
BLTBPTH         equ $04C
BLTBPTL         equ $04E
BLTAPTH         equ $050
BLTAPTL         equ $052
BLTDPTH         equ $054
BLTDPTL         equ $056
BLTSIZE         equ $058
BLTCMOD         equ $060
BLTBMOD         equ $062
BLTAMOD         equ $064
BLTDMOD         equ $066
BLTCDAT         equ $070
BLTBDAT         equ $072
BLTADAT         equ $074
DMACON          equ $096
INTENA          equ $09A
INTREQ          equ $09C
COLOR00         equ $180

; DONE -- the last thing a program does: store the mark the harness waits
; for, then stop in place. Everything the test inspects must be stored
; before it.
DONE            macro
                move.w  #DONE_VALUE,DONE_ADDR
.hang\@         bra.s   .hang\@
                endm
