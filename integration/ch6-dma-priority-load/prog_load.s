; prog_load.s -- ch6-dma-priority-load. The program of SPECIFICATION.md section 1.5.
; The data of section 1.4, the Copper lists of section 1.6 and the variant table
; VT (section 1.7) are part of the loaded image: the test places them before the
; reset.  Every run executes the same instructions up to S5.
;
; Symbols (a mutation or a contrasting case assembles this file with one set):
;   FIXTURE_MUTANT  the stored fixture constant is $DEAD instead of $BEEF
;   LITERAL        COP1LC = L1 is written at S4.7, before Copper DMA is on
;   C1NOP          one NOP before S3
;   HTOT           HTOTAL = $E4 and BEAMCON0 = $00A0 written at S1

        include "nanomig.i"

; Appendix B register offsets not in nanomig.i
DSKPTH          equ $020
DSKLEN          equ $024
COPCON          equ $02E
COP1LCH         equ $080
COP2LCH         equ $084
COPJMP2         equ $08A
HTOTAL          equ $1C0
BEAMCON0        equ $1DC
DIWSTRT         equ $08E
DIWSTOP         equ $090
DDFSTRT         equ $092
DDFSTOP         equ $094
ADKCON          equ $09E
AUD0LCH         equ $0A0
AUD0LEN         equ $0A4
AUD0PER         equ $0A6
AUD0VOL         equ $0A8
BPL1PTH         equ $0E0
BPLCON0         equ $100
BPLCON1         equ $102
BPLCON2         equ $104
BPL1MOD         equ $108
BPL2MOD         equ $10A
SPR0PTH         equ $120

; data placed by the test
VT              equ $0C1000     ; variant table
VT_CPU          equ VT+0        ; 0 STOP, 1 LOOP
VT_DMACON       equ VT+2
VT_BPLCON0      equ VT+4
VT_DDFSTRT      equ VT+6
VT_DDFSTOP      equ VT+8
VT_BPL1MOD      equ VT+10
VT_BPL2MOD      equ VT+12
VT_AUD0PER      equ VT+14
VT_VECADDR      equ VT+16       ; long: where the handler address is stored
VT_BPLPT        equ VT+20       ; four longs
VT_DELAY        equ VT+36       ; iterations of the S4.13 count-down loop
VT_XVAL1        equ VT+38       ; S4.13: value written to the register whose address is at VT_XREG1
VT_XREG1        equ VT+40       ; long
VT_XVAL2        equ VT+44
VT_XREG2        equ VT+46       ; long
L0              equ $10000
L1              equ $10100
S_BASE          equ $60000
AU_BASE         equ $62000
DK_BASE         equ $66000
BA_BASE         equ $20000
BD_BASE         equ $28000

R_VEC           equ RESULTS+0   ; long
R_DMA           equ RESULTS+4
R_FIX           equ RESULTS+6
R_MARK          equ RESULTS+8   ; loop-exit marker
A2WORD          equ RESULTS+$200
HANDLER         equ PROGRAM_ORG+$400

                org PROGRAM_ORG
start:
; ---- S1
                move.b  #3,$BFE201              ; CIA-A: OVL and LED are outputs
                move.b  #0,$BFE001              ; OVL low: Chip memory at $000000
                lea     CUSTOM,a6
                move.w  #$2700,sr
                ifd     HTOT
                move.w  #$00E4,HTOTAL(a6)
                move.w  #$00A0,BEAMCON0(a6)     ; VARBEAMEN, PAL
                endif
                move.w  #$7FFF,INTENA(a6)
                move.w  #$7FFF,INTREQ(a6)
                move.w  #$7FFF,DMACON(a6)
                ifd     FIXTURE_MUTANT
                move.w  #$DEAD,R_FIX
                else
                move.w  #$BEEF,R_FIX
                endif
; ---- S2 (the data are in the image)
                movea.l VT_VECADDR,a0
                move.l  #HANDLER,(a0)
                move.l  $6C,d0
                move.l  d0,R_VEC
                ifd     C1NOP
                nop
                endif
; ---- S3: wait for line $20
.s3a:           move.w  VHPOSR(a6),d0
                lsr.w   #8,d0
                cmp.b   #$20,d0
                bne.s   .s3a
                move.w  VPOSR(a6),d0
                btst    #0,d0
                bne.s   .s3a
; ---- S4.1 display
                move.w  #$2C81,DIWSTRT(a6)
                move.w  #$2CC1,DIWSTOP(a6)
                move.w  VT_DDFSTRT,DDFSTRT(a6)
                move.w  VT_DDFSTOP,DDFSTOP(a6)
                move.w  VT_BPLCON0,BPLCON0(a6)
                move.w  #0,BPLCON1(a6)
                move.w  #0,BPLCON2(a6)
                move.w  VT_BPL1MOD,BPL1MOD(a6)
                move.w  VT_BPL2MOD,BPL2MOD(a6)
                move.l  VT_BPLPT+0,BPL1PTH+0(a6)
                move.l  VT_BPLPT+4,BPL1PTH+4(a6)
                move.l  VT_BPLPT+8,BPL1PTH+8(a6)
                move.l  VT_BPLPT+12,BPL1PTH+12(a6)
; ---- S4.2 sprites
                move.l  #S_BASE+$0000,SPR0PTH+0(a6)
                move.l  #S_BASE+$0400,SPR0PTH+4(a6)
                move.l  #S_BASE+$0800,SPR0PTH+8(a6)
                move.l  #S_BASE+$0C00,SPR0PTH+12(a6)
                move.l  #S_BASE+$1000,SPR0PTH+16(a6)
                move.l  #S_BASE+$1400,SPR0PTH+20(a6)
                move.l  #S_BASE+$1800,SPR0PTH+24(a6)
                move.l  #S_BASE+$1C00,SPR0PTH+28(a6)
; ---- S4.3 audio, channels 0..3
                move.l  #AU_BASE+$0000,AUD0LCH+$00(a6)
                move.w  #2048,AUD0LEN+$00(a6)
                move.w  #32,AUD0VOL+$00(a6)
                move.w  VT_AUD0PER,AUD0PER+$00(a6)
                move.l  #AU_BASE+$1000,AUD0LCH+$10(a6)
                move.w  #2048,AUD0LEN+$10(a6)
                move.w  #32,AUD0VOL+$10(a6)
                move.w  #124,AUD0PER+$10(a6)
                move.l  #AU_BASE+$2000,AUD0LCH+$20(a6)
                move.w  #2048,AUD0LEN+$20(a6)
                move.w  #32,AUD0VOL+$20(a6)
                move.w  #124,AUD0PER+$20(a6)
                move.l  #AU_BASE+$3000,AUD0LCH+$30(a6)
                move.w  #2048,AUD0LEN+$30(a6)
                move.w  #32,AUD0VOL+$30(a6)
                move.w  #124,AUD0PER+$30(a6)
; ---- S4.4 disk
                move.l  #DK_BASE,DSKPTH(a6)
                move.w  #$8100,ADKCON(a6)
; ---- S4.5 blitter, everything except BLTSIZE
                move.w  #$09F0,BLTCON0(a6)
                move.w  #$0000,BLTCON1(a6)
                move.w  #$FFFF,BLTAFWM(a6)
                move.w  #$FFFF,BLTALWM(a6)
                move.w  #$0000,BLTAMOD(a6)
                move.w  #$0000,BLTDMOD(a6)
                move.l  #BA_BASE,BLTAPTH(a6)
                move.l  #BD_BASE,BLTDPTH(a6)
; ---- S4.6 COPCON: CDANG
                move.w  #$0002,COPCON(a6)
; ---- S4.7 Copper location registers and the strobe
                ifd     LITERAL
                move.l  #L1,COP1LCH(a6)
                else
                move.l  #L0,COP1LCH(a6)
                endif
                move.l  #L0,COP2LCH(a6)
                move.w  d0,COPJMP2(a6)
; ---- S4.8 INTENA
                move.w  #$C010,INTENA(a6)
; ---- S4.9 DMACON
                move.w  VT_DMACON,DMACON(a6)
; ---- S4.10 DMACONR
                move.w  DMACONR(a6),R_DMA
; ---- S4.11 disk start
                move.w  #$4000,DSKLEN(a6)
                move.w  #$DF40,DSKLEN(a6)
                move.w  #$DF40,DSKLEN(a6)
; ---- S4.12
                ifnd    LITERAL
                move.l  #L1,COP1LCH(a6)         ; the next vertical blank starts L1
                endif
; ---- S4.13: two register writes and a delay whose operands are variant words; in every run
;      but the contrasts of C5 and C7 they write 0 to the no-op register and count down from 0
                movea.l VT_XREG1,a0
                move.w  VT_XVAL1,(a0)
                movea.l VT_XREG2,a0
                move.w  VT_XVAL2,(a0)
                move.w  VT_DELAY,d3
.dly:           dbra    d3,.dly
; ---- S5
                move.w  VT_CPU,d0
                bne.s   .loop
                stop    #$2000
                bra.s   .s6
.loop:          lea     A2WORD,a2
.again:
                rept    8
                move.w  d0,(a2)
                move.w  (a2),d1
                endr
                move.w  INTREQR(a6),d2
                btst    #4,d2
                beq.s   .again
                move.w  #$0010,INTREQ(a6)
                move.w  #1,R_MARK
; ---- S6
.s6:            move.w  #$7FFF,DMACON(a6)
                move.w  #$4000,DSKLEN(a6)
                move.w  #$7FFF,INTENA(a6)
                DONE

                org HANDLER
handler:        move.w  #$0010,CUSTOM+INTREQ
                rte
