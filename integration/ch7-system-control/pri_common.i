; pri_common.i -- what the P1 and P2 programs share: register offsets, the
; memory map (everything the chipset fetches lies below $80000, so OCS
; pointers reach it), and the builders for bitplane images, sprite blocks,
; Copper lists and the palette. See SPECIFICATION.md, "Common conventions"
; and "Playfield geometry and row tags".

        include "nanomig.i"

CIAAPRA         equ     $BFE001
CIAADDRA        equ     $BFE201
COP1LCH         equ     $080
COPJMP1         equ     $088
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPLCON0         equ     $100
BPLCON1         equ     $102
BPLCON2         equ     $104
BPL1MOD         equ     $108
BPL2MOD         equ     $10A
BPL1PTH         equ     $0E0
SPR0PTH         equ     $120

; ---- memory map ----
IMG_ZERO        equ     $10000          ; all-zero image
IMG_A_P1        equ     $13000          ; one image per 10240 bytes (256 rows x 40 bytes)
IMG_B_P1        equ     $16000
IMG_B_P2        equ     $19000
IMG_E_P1        equ     $1C000
SPR_A1          equ     $20000          ; eight sprite blocks, $200 apart
SPR_A2          equ     $21000
SPR_B           equ     $22000
SPR_E           equ     $23000
SPR_P2          equ     $24000
P2_IMG          equ     $40000          ; P2: plane n image at P2_IMG + n*$6000 (n = 0..5), all-zero image at +$3000
LIST_BASE       equ     $30000          ; Copper list k at LIST_BASE + k*$100

; ---- what the CPU leaves for the test (above $80000, CPU stores only) ----
TICKS           equ     $0C1000         ; one word per VERTB: the field counter
MARKS           equ     $0C1100         ; marker k = $C000+k
ENDMARK         equ     $0C1120         ; $CFFF

; BAND rows,id,fill,tag: \1 rows, \2 tag id, \3 fill long for bytes 5..39 (0 or $FFFFFFFF), \4 1 = draw the tag
BAND            macro
                move.w  #\1-1,d0
                moveq   #\2,d1
                move.l  #\3,d2
                moveq   #\4,d7
                bsr     mkrows
                endm

; ---- common start: releases the boot overlay, clears interrupts and DMA.
COMMON_START    macro
                move.b  #$03,CIAADDRA
                move.b  #$00,CIAAPRA
                lea     CUSTOM,a0
                move.w  #$7FFF,INTENA(a0)
                move.w  #$7FFF,INTREQ(a0)
                move.w  #$7FFF,DMACON(a0)
                endm

; ---- window registers (SPECIFICATION.md "Playfield geometry and row tags").
SET_WINDOW      macro
                move.w  #$2C81,DIWSTRT(a0)
                ifd     NTSC_BUILD
                move.w  #$F4C1,DIWSTOP(a0)
                else
                move.w  #$2CC1,DIWSTOP(a0)
                endif
                move.w  #$0038,DDFSTRT(a0)
                move.w  #$00D0,DDFSTOP(a0)
                move.w  #0,BPL1MOD(a0)
                move.w  #0,BPL2MOD(a0)
                move.w  #0,BPLCON1(a0)
                endm

                ifd     MUT_SPRENCLR
DMACON_RUN      equ     $8380
                else
DMACON_RUN      equ     $83A0
                endif
