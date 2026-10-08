; hj_defs.i -- custom chip registers this package needs beyond nanomig.i,
; from the Hardware Reference Manual's Appendix B, Register Summary Address
; Order (p.289-290). Offsets from CUSTOM (nanomig.i defines CUSTOM itself).
; Not part of the harness contract: this package's own file (like
; ch1-cpu-system/ch1.i, appF-cia-system/ciaregs.i).

JOY0DAT         equ     $00A
JOY1DAT         equ     $00C
POTGOR          equ     $016            ; POTINP
BPLCON0         equ     $100
POTGO           equ     $034
JOYTEST         equ     $036

; ---- CIA-B register addresses (byte access only, SPECIFICATION.md 3.1) ---
CIAB_PRB        equ     $BFD100
CIAB_DDRB       equ     $BFD300

; ---- fixture and buffer layout (package's own; SPECIFICATION.md 3.2 fixes
; the shared slots -- RESULTS as the step-marker word, the buffer at
; $0C1000 upward, PROGRAM_STACK below $0FFF00; the fixture slot is this
; package's own addition, clear of both) -----------------------------------
FIXTURE_VALUE   equ     $BEEF
FIXTURE_ADDR    equ     $0C0FFC         ; two words before DONE_ADDR

BUFBASE         equ     $0C1000
