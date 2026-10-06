; ch4bp.i -- shared constants for ch4-shres-bitplane's own program
; (prog_main.s and its mutation wrappers). Package-local; not part of the
; harness contract (nanomig.i is). All register writes this program makes
; besides DMACON go through the Copper list tables (bp_cop_*.i); this file
; only names what the CPU itself touches directly.

DENISEID        equ     $07C    ; Appendix B; PC0 reads this back (p.299)
COP1LCH         equ     $080
COPJMP1         equ     $088

; CIA-A: on reset "an additional copy of the system ROM responds ... The
; system RAM that would normally be located at this address is not
; available. The OVL bit in one of the 8520 Chips disables the overlay"
; (p.223; Appendix F p.339 PRA at $BFE001, p.351 bit 0 "OVL").
CIAADDRA        equ     $BFE201
CIAAPRA         equ     $BFE001

CLEAR_OVL       macro
                move.b  #$03,CIAADDRA
                move.b  #$00,CIAAPRA
                endm

; SPECIFICATION.md 4.2 -- chip-memory layout. Addresses are the
; specification's own; this file only names them.
PLANE1BUF       equ     $010000
PLANE2BUF       equ     $020000
ZEROBUF         equ     $030000
SPR0STRUCT      equ     $040000
NULLSPR         equ     $040100
COPBASE         equ     $041000
COPSTEP         equ     $001000
F0LIST          equ     COPBASE+0*COPSTEP
F1LIST          equ     COPBASE+1*COPSTEP
F2LIST          equ     COPBASE+2*COPSTEP
F3LIST          equ     COPBASE+3*COPSTEP
F4LIST          equ     COPBASE+4*COPSTEP
F5LIST          equ     COPBASE+5*COPSTEP
F6LIST          equ     COPBASE+6*COPSTEP
F7LIST          equ     COPBASE+7*COPSTEP
F8LIST          equ     COPBASE+8*COPSTEP
F9LIST          equ     COPBASE+9*COPSTEP
F10LIST         equ     COPBASE+10*COPSTEP

; ---- mutation-selected constants ------------------------------------------
;
; Exactly one of the MUT_xxx symbols is equ'd to 1 by a mutant wrapper
; before it includes prog_main.s; prog_main.s assembled standalone (no
; symbol defined) gives the baseline/normal stimulus. Every Copper-list
; table is chosen the same way, directly at each frame's `include` in
; prog_main.s; this section only resolves the few things prog_main.s's own
; instruction stream branches on.

; fixture contrast: the fixture proves it can deliver a known value
; before anything reads one
                ifd     MUT_FIX
FIXTUREVAL      equ     $DEAD
                else
FIXTUREVAL      equ     $BEEF
                endif

; C1 (OCS instead of ECS) is a harness Config change, made by the C++ test
; when NMH_MUTATE=C1 -- no program symbol needed; the SAME prog.bin runs.
; C2 (never write DONE) needs its own tiny wrapper (prog_vc2.s) that
; branches around the DONE macro directly; no data table involved.
