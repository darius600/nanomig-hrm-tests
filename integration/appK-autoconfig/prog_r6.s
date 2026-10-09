; prog_r6.s -- SPECIFICATION.md section 5.2, P6 (runs R6 and R6m, and their
; contrasts). Reuses P1 steps a-d (bitplanes/sprites off, COLOR00=C1, the
; power-LED and drive-motor calibrations, the two-frame wait) before its
; own step b (read IMG0, configure at B1, memory-board content check) and
; the RESET sequence of section 2.3, whose tail is R6/R6m's own last
; action instead of a hang loop.
;
; Three independent points vary, each selected by a symbol an including
; source sets before this file; prog_r6.s itself sets none of them:
;   MUT_R6M       -- selects the R6m ending (P0'=$6996 re-check at B1)
;                     instead of R6's own ending (IMG3 read)
;   MUT_R6_NORESET -- R6a/R6am: RESET at A replaced by NOP
;   MUT_R6_HANG    -- the instruction stream stops at the tail (no M2, no
;                     completion marker)

        include "nanomig.i"
        include "common.i"

BPLCON0  equ     $100
SPR0CTL  equ     $142
C1       equ     $0C30
C2       equ     $03AC

B1       equ     $00200000
B1_HI    equ     $00
B1_LO    equ     $20

P0       equ     $A55A
P0PRIME  equ     $6996

V1       equ     $BBBB
V2       equ     $CCCC

MK1      equ     MK_NEXT
MK2      equ     MK_NEXT+2
IMG0     equ     MK_NEXT+4       ; 32 words: +4 .. +66
R0V      equ     MK_NEXT+68
SCRATCH  equ     MK_NEXT+70
IMG3     equ     MK_NEXT+72      ; 32 words (R6 ending only)
R3V      equ     MK_NEXT+72      ; same offset as IMG3 (R6m ending only; mutually exclusive)

        org     PROGRAM_ORG

start:
        ENV_SETUP

        ; P1 steps a-d
        move.w  #$0000,BPLCON0(a1)
        move.w  #$0000,SPR0CTL+0*8(a1)
        move.w  #$0000,SPR0CTL+1*8(a1)
        move.w  #$0000,SPR0CTL+2*8(a1)
        move.w  #$0000,SPR0CTL+3*8(a1)
        move.w  #$0000,SPR0CTL+4*8(a1)
        move.w  #$0000,SPR0CTL+5*8(a1)
        move.w  #$0000,SPR0CTL+6*8(a1)
        move.w  #$0000,SPR0CTL+7*8(a1)
        move.w  #C1,COLOR00(a1)

        WAITVERTB
        move.b  #$00,CIAAPRA
        WAITVERTB

        move.b  #$FF,CIABDDRB
        move.b  #$FF,CIABPRB
        move.b  #$F7,CIABPRB
        move.b  #$FF,CIABPRB
        move.b  #$7F,CIABPRB
        move.b  #$77,CIABPRB
        move.b  #$7F,CIABPRB
        move.b  #$FF,CIABPRB

        WAITVERTB
        WAITVERTB

        ; step b: read IMG0, configure at B1, memory-board content check
        lea     AC_BASE,a0
        lea     IMG0,a2
        moveq   #15,d3
.imgl0: move.w  (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl0

        move.b  #B1_HI,AC_BASE+$4A
        move.b  #B1_LO,AC_BASE+$48

        move.w  #P0,B1
        move.w  #$0000,SCRATCH
        move.w  B1,R0V

        ; RESET sequence (section 2.3)
        lea     CUSTOM+COLOR00,a0
        move.w  #C2,d0
        lea     MK1,a2
        move.w  #V1,d1

        cnop    2,4
seq_am2:
        move.w  d1,(a2)
seq_a:
        ifd     MUT_R6_NORESET
        nop
        else
        reset
        endif
seq_ap2:
        move.w  d0,(a0)
tail:
        ifd     MUT_R6_HANG
.stop:  bra.s   .stop                   ; the stream stops here: no M2, no completion marker
        endif
        move.w  #V2,MK2

        ifd     MUT_R6M
        move.w  #P0PRIME,B1
        move.w  #$0000,SCRATCH
        move.w  B1,R3V
        DONE
        else
        lea     AC_BASE,a0
        lea     IMG3,a2
        moveq   #15,d3
.imgl3: move.w  (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl3
        DONE
        endif

        H_E_HANDLER
