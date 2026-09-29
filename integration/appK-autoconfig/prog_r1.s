; prog_r1.s -- run R1: SPECIFICATION.md section 5.1, P1 and P2. Applies the
; shared environment of common.i, then P1's own additions (bitplanes and
; sprites off, COLOR00 set to C1), calibrates the power LED and the
; drive-motor latch, waits two frames to anchor F_ref, then executes the
; RESET sequence of section 2.3.
;
; Four independent points vary, each selected by a symbol an including
; source sets before this file:
;   MUT_NORESET  -- R1a: RESET at A replaced by NOP (section 5.1.6)
;   MUT_NOMOVE   -- R1b: MOVE.W D0,(A0) at A+2 replaced by NOP (section 5.2.6 of P2)
;   MUT_C1PRIME  -- display-anchor contrast: COLOR00 preset to C1'=$0C3F
;   MUT_PRIV     -- "no exception" contrast: SR set to user mode ($0700)
;                    immediately before the RESET sequence, so RESET itself
;                    is a privilege violation
; prog_r1.s itself sets none of them.

        include "nanomig.i"
        include "common.i"

BPLCON0  equ $100
SPR0CTL  equ $142

        ifd     MUT_C1PRIME
C1       equ     $0C3F
        else
C1       equ     $0C30
        endif
C2       equ     $03AC

V1       equ     $BBBB
V2       equ     $CCCC

CALDIM   equ     $CA10           ; power-LED calibration phase markers
CALBRIGHT equ    $CA20           ; (section 5.1.5: L_dim and L_bright are
                                  ; derived from the phases these bound)

MK_CALDIM equ    MK_NEXT
MK_CALBRIGHT equ MK_NEXT+2
MK1      equ     MK_NEXT+4
MK2      equ     MK_NEXT+6

        org     PROGRAM_ORG

start:
        ENV_SETUP                      ; a1 = CUSTOM on return

        move.w  #$0000,BPLCON0(a1)     ; no bitplanes (p.48); ENBPLCN3=0 (p.307)
        move.w  #$0000,SPR0CTL+0*8(a1)
        move.w  #$0000,SPR0CTL+1*8(a1)
        move.w  #$0000,SPR0CTL+2*8(a1)
        move.w  #$0000,SPR0CTL+3*8(a1)
        move.w  #$0000,SPR0CTL+4*8(a1)
        move.w  #$0000,SPR0CTL+5*8(a1)
        move.w  #$0000,SPR0CTL+6*8(a1)
        move.w  #$0000,SPR0CTL+7*8(a1)
        move.w  #C1,COLOR00(a1)

        ; power-LED calibration (p.351: bit1=1 not bright, bit1=0 bright).
        ; Each phase is marked in chip RAM as it starts, so the test can
        ; read L_dim and L_bright from the port trace bounded by the
        ; markers instead of assuming a starting level.
        move.w  #CALDIM,MK_CALDIM      ; entering the dim phase (ddra/pra set by ENV_SETUP)
        WAITVERTB
        move.b  #$00,CIAAPRA           ; /LED=0 (bright); OVL stays 0
        move.w  #CALBRIGHT,MK_CALBRIGHT ; entering the bright phase
        WAITVERTB

        ; drive-motor calibration (p.340 prb bits; p.244 latch sequence)
        move.b  #$FF,CIABDDRB
        move.b  #$FF,CIABPRB
        move.b  #$F7,CIABPRB
        move.b  #$FF,CIABPRB           ; off-latch
        move.b  #$7F,CIABPRB
        move.b  #$77,CIABPRB
        move.b  #$7F,CIABPRB
        move.b  #$FF,CIABPRB           ; on-latch

        ; wait two complete frames; F_ref is the last of them
        WAITVERTB
        WAITVERTB

        ifd     MUT_PRIV
        move.w  #$0700,SR              ; user mode: RESET below is a privilege violation
        endif

        ; RESET sequence (section 2.3): A0=COLOR00, D0=C2, A2=MK1, D1=V1
        lea     CUSTOM+COLOR00,a0
        move.w  #C2,d0
        lea     MK1,a2
        move.w  #V1,d1

        cnop    2,4                    ; A-2 == 2 (mod 4), so A (RESET) is long-word aligned
seq_am2:
        move.w  d1,(a2)                ; A-2: marker V1 -> MK1 (t_M1)
seq_a:
        ifd     MUT_NORESET
        nop
        else
        reset
        endif
seq_ap2:
        ifd     MUT_NOMOVE
        nop
        else
        move.w  d0,(a0)                ; A+2: C2 -> COLOR00
        endif
tail:
        move.w  #V2,MK2                ; M2 (only if Chip memory answers here)
.hang:  bra.s   .hang

        H_E_HANDLER
