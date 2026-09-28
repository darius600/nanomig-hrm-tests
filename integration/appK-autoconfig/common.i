; common.i -- shared program environment for this package (SPECIFICATION.md
; section 2.1, "Conventions for every run"): interrupts and DMA cleared,
; the ROM overlay cleared, the exception vectors installed and checked,
; and the M0 marker. Not part of the harness; a package-local include.
;
; Every program includes "nanomig.i" first, then this file, then defines
; its own additional RESULTS offsets starting at MK_NEXT.
;
; ENV_SETUP leaves a1 = CUSTOM (offset base for DFF-space registers) for
; the caller to reuse. It clobbers a1, a3 and the condition codes.
; H_E_HANDLER defines the H_E label and its body; include it once, after
; the program's own tail.

CIAADDRA equ    $BFE201
CIAAPRA  equ    $BFE001
CIABDDRB equ    $BFD300
CIABPRB  equ    $BFD100
AC_BASE  equ    $00E80000

CE_COLOR equ    $0F0F
V0       equ    $AAAA
VE_MARK  equ    $DEAD
VECOK_YES equ   $FACE
VECOK_NO equ    $0000

MK0      equ    RESULTS
MK_E     equ    RESULTS+2
VECOK    equ    RESULTS+4
MK_NEXT  equ    RESULTS+6       ; first offset free for a program's own data

ENV_SETUP       macro
        move.w  #V0,MK0                 ; M0: first action
        lea     CUSTOM,a1
        move.w  #$7FFF,INTENA(a1)       ; clear every interrupt enable
        move.w  #$7FFF,DMACON(a1)       ; clear every DMA enable
        move.b  #$03,CIAADDRA           ; ddra: bits 0,1 output (p.339)
        move.b  #$02,CIAAPRA            ; OVL=0, /LED=1 (dim)
        move.l  #H_E,$000008
        move.l  #H_E,$00000C
        move.l  #H_E,$000010
        move.l  #H_E,$000020
        move.l  #H_E,$000028
        move.l  #H_E,$00002C
        lea     H_E,a3
        cmpa.l  $000020,a3              ; vector-table check: RAM answers at the vectors
        beq.s   .vecok\@
        move.w  #VECOK_NO,VECOK
        bra.s   .vecdone\@
.vecok\@:
        move.w  #VECOK_YES,VECOK
.vecdone\@:
        endm

H_E_HANDLER     macro
H_E:
        move.w  #CE_COLOR,CUSTOM+COLOR00
        move.w  #VE_MARK,MK_E
.hang\@ bra.s   .hang\@
        endm

; Polls INTREQR bit 5 (VERTB, "start of vertical blank", p.280) after
; clearing it: a page-documented way to wait for one frame boundary,
; independent of any assumed cycle count. Clobbers d2. Needs a1=CUSTOM.
WAITVERTB       macro
        move.w  #$0020,INTREQ(a1)
.poll\@ move.w  INTREQR(a1),d2
        btst    #5,d2
        beq.s   .poll\@
        endm
