; prog_p1.s -- Proposition 1 (POTGO bit 0 dumps the pot-line capacitors).
; See SPECIFICATION.md section "Proposition 1" for the stimulus, the
; preconditions and the expected values; this file only implements what
; SPECIFICATION.md already committed to. The harness holds all four pot
; lines (pins 5, 9 of both connectors -- mouse_buttons and joystick0/1 bit
; 5) at level 1 throughout this program's run (test_ch8_controller_ports.cpp),
; except the K5 variant, which is the same binary run under a different
; harness stimulus (no separate source needed).
;
; K1, K3, K4, K6, K7 and OUTLOW are compiled-in variants (this source with a
; symbol set, included by prog_p1_k1.s etc.), because they change a value
; the CPU instruction stream itself writes or a wait target baked into the
; code -- not something the harness can vary at its ports. K2 is already
; inside the sequence below (every Phase-D config's own output-driven
; line). K5 is a harness-side stimulus variant of this same
; binary (see the test).

        include "nanomig.i"
        include "ch8_regs.i"
        org     PROGRAM_ORG

P1_BASE         equ     $0C1000
MARK_M1         equ     $4D31   ; 'M1'
MARK_M2         equ     $4D32   ; 'M2'

; ---- Fc's R-step wait target: 10 normally; K3 moves it to 12 (PC1
; mutation); K6 moves it to 5 (PC1 mutation). Only one of K3/K6 is ever
; defined in a given build.
        ifd     K3
FCTARGET        equ     12
        else
        ifd     K6
FCTARGET        equ     5
        else
FCTARGET        equ     10
        endif
        endif

; ---- W step (line 4): wait V==4, store Vw0 (the read that confirmed it),
; write POTGO<-\1, store Vw1, store marker M1. 3 words via (a1)+.
WSTEP           macro
                WAITV   4
                move.w  d1,(a1)+        ; Vw0
                move.w  #\1,POTGO(a0)
                READV
                move.w  d1,(a1)+        ; Vw1
                ifd     K7
                WAITV   9               ; K7: the M1 marker is stored five lines late
                endif
                move.w  #MARK_M1,(a1)+  ; M1
                endm

; ---- W step with Rpre (Fc only): wait V==4, store Vw0, read POTGOR
; (Rpre) and store it, write POTGO<-\1 (K1: withhold bit 0, the START
; bit), store Vw1, store M1. 4 words via (a1)+.
WSTEP_RPRE      macro
                WAITV   4
                move.w  d1,(a1)+        ; Vw0
                move.w  POTGOR(a0),d0
                move.w  d0,(a1)+        ; Rpre
                ifd     K1
                move.w  #\1,d0
                and.w   #$FFFE,d0       ; K1: bit 0 (START) withheld
                move.w  d0,POTGO(a0)
                else
                move.w  #\1,POTGO(a0)
                endif
                READV
                move.w  d1,(a1)+        ; Vw1
                ifd     K7
                WAITV   9               ; K7: the M1 marker is stored five lines late
                endif
                move.w  #MARK_M1,(a1)+  ; M1
                endm

; ---- R step (line \1): wait V==\1, then four records of {Vr0,M2,POTGOR,
; Vr1}, unconditionally (the C++ side filters on Vr0==Vr1==the target).
; 16 words via (a1)+.
RSTEP           macro
                WAITV   \1
                moveq   #3,d4
.rloop\@:       READV
                move.w  d1,(a1)+        ; Vr0
                move.w  #MARK_M2,(a1)+  ; M2
                move.w  POTGOR(a0),d0
                move.w  d0,(a1)+        ; POTGOR
                READV
                move.w  d1,(a1)+        ; Vr1
                dbra    d4,.rloop\@
                endm

start:
        lea     CUSTOM,a0
        SETUP_S1
        SETUP_S2
        lea     P1_BASE,a1

; ---- Phase P: POTGOR follows the output-driven level (PC2). -------------
        ifd     K4
        WSTEP   $AA00           ; F1 (K4 mutant: wrong value, PC2 must fail)
        else
        WSTEP   $FF00           ; F1
        endif
        RSTEP   10
        WSTEP   $AA00           ; F2
        RSTEP   10
        WSTEP   $FF00           ; F3
        RSTEP   10

; ---- Phase D: the dump, for C0..C4 in order. -----------------------------
; C0 ($0000): all four lines inputs.
        WSTEP        $FF00      ; Fa
        RSTEP        10
        WSTEP        $0000      ; Fb
        RSTEP        10
        WSTEP_RPRE   $0001      ; Fc: C0 | START
        RSTEP        FCTARGET

; C1 ($0300): LX output.
        WSTEP        $FF00
        RSTEP        10
        WSTEP        $0300
        RSTEP        10
        ifd     OUTLOW
        WSTEP_RPRE   $0201   ; OUTLOW mutant: OUTLX=1, DATLX=0 (output driven low), + START
        else
        WSTEP_RPRE   $0301
        endif
        RSTEP        FCTARGET

; C2 ($0C00): LY output.
        WSTEP        $FF00
        RSTEP        10
        WSTEP        $0C00
        RSTEP        10
        ifd     OUTLOW
        WSTEP_RPRE   $0801   ; OUTLOW mutant: OUTLY=1, DATLY=0, + START
        else
        WSTEP_RPRE   $0C01
        endif
        RSTEP        FCTARGET

; C3 ($3000): RX output.
        WSTEP        $FF00
        RSTEP        10
        WSTEP        $3000
        RSTEP        10
        ifd     OUTLOW
        WSTEP_RPRE   $2001   ; OUTLOW mutant: OUTRX=1, DATRX=0, + START
        else
        WSTEP_RPRE   $3001
        endif
        RSTEP        FCTARGET

; C4 ($C000): RY output.
        WSTEP        $FF00
        RSTEP        10
        WSTEP        $C000
        RSTEP        10
        ifd     OUTLOW
        WSTEP_RPRE   $8001   ; OUTLOW mutant: OUTRY=1, DATRY=0, + START
        else
        WSTEP_RPRE   $C001
        endif
        RSTEP        FCTARGET

        DONE
