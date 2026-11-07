; prog_c6_lateservice.s -- the contrast of P7's restart-line clause (SPECIFICATION.md
; P7.6): C6 whose VERTB service is late. The VERTB handler starts CIA-B timer A
; (one-shot, p.340-p.342), which underflows after the last line of L5 and L5b and
; before the frame ends; the CIA interrupt is INT6 (p.340, EXTER p.280, level 6).
; Its handler waits for the VERTB request of the next frame (INTREQR bit 5) and then
; for a beam line from 2 to $3F before it returns; while it runs the CPU's mask is 6, so
; the pending VERTB is served on line 2 or later, and the Copper list is untouched.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

INT_EXTER       equ     $2000
CIABTALO        equ     $BFD400
CIABTAHI        equ     $BFD500
CIABICR         equ     $BFDD00
CIABCRA         equ     $BFDE00
LATE_TICKS      equ     10200           ; about 224 lines at 45.4 ticks (E clock, p.340) per line

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        move.l  #late_handler,$78.w
        BUILD_L5  COP_MAIN,c6_spr_tab,c6_bpl_tab,COP_SECOND,-1,0
        BUILD_L5B COP_SECOND
        bsr     build_structs_c3
        ; ---- C1a: audio, period 124 ----
        move.w  #(CH_AUD0)>>16,AUD0LCH(a0)
        move.w  #(CH_AUD0)&$FFFF,AUD0LCL(a0)
        move.w  #16,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #124,AUD0PER(a0)
        move.w  #(CH_AUD1)>>16,AUD1LCH(a0)
        move.w  #(CH_AUD1)&$FFFF,AUD1LCL(a0)
        move.w  #16,AUD1LEN(a0)
        move.w  #64,AUD1VOL(a0)
        move.w  #124,AUD1PER(a0)
        move.w  #(CH_AUD2)>>16,AUD2LCH(a0)
        move.w  #(CH_AUD2)&$FFFF,AUD2LCL(a0)
        move.w  #16,AUD2LEN(a0)
        move.w  #64,AUD2VOL(a0)
        move.w  #124,AUD2PER(a0)
        move.w  #(CH_AUD3)>>16,AUD3LCH(a0)
        move.w  #(CH_AUD3)&$FFFF,AUD3LCL(a0)
        move.w  #16,AUD3LEN(a0)
        move.w  #64,AUD3VOL(a0)
        move.w  #124,AUD3PER(a0)
        ; ---- C2a: disk, write direction ----
        move.w  #$9100,ADKCON(a0)
        move.w  #(CH_DISK)>>16,DSKPTH(a0)
        move.w  #(CH_DISK)&$FFFF,DSKPTL(a0)
        ; ---- C3: eight sprites ----
        move.w  #(CH_SPR0)>>16,SPR0PTH(a0)
        move.w  #(CH_SPR0)&$FFFF,SPR0PTL(a0)
        move.w  #(CH_SPR0+$0400)>>16,SPR1PTH(a0)
        move.w  #(CH_SPR0+$0400)&$FFFF,SPR1PTL(a0)
        move.w  #(CH_SPR0+$0800)>>16,SPR2PTH(a0)
        move.w  #(CH_SPR0+$0800)&$FFFF,SPR2PTL(a0)
        move.w  #(CH_SPR0+$0C00)>>16,SPR3PTH(a0)
        move.w  #(CH_SPR0+$0C00)&$FFFF,SPR3PTL(a0)
        move.w  #(CH_SPR0+$1000)>>16,SPR4PTH(a0)
        move.w  #(CH_SPR0+$1000)&$FFFF,SPR4PTL(a0)
        move.w  #(CH_SPR0+$1400)>>16,SPR5PTH(a0)
        move.w  #(CH_SPR0+$1400)&$FFFF,SPR5PTL(a0)
        move.w  #(CH_SPR0+$1800)>>16,SPR6PTH(a0)
        move.w  #(CH_SPR0+$1800)&$FFFF,SPR6PTL(a0)
        move.w  #(CH_SPR0+$1C00)>>16,SPR7PTH(a0)
        move.w  #(CH_SPR0+$1C00)&$FFFF,SPR7PTL(a0)
        ; ---- C4-4: four low-res bitplanes ----
        move.w  #$4200,BPLCON0(a0)
        move.w  #0,BPLCON1(a0)
        move.w  #0,BPL1MOD(a0)
        move.w  #0,BPL2MOD(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #(CH_BPL1)>>16,BPL1PTH(a0)
        move.w  #(CH_BPL1)&$FFFF,BPL1PTL(a0)
        move.w  #(CH_BPL2)>>16,BPL2PTH(a0)
        move.w  #(CH_BPL2)&$FFFF,BPL2PTL(a0)
        move.w  #(CH_BPL3)>>16,BPL3PTH(a0)
        move.w  #(CH_BPL3)&$FFFF,BPL3PTL(a0)
        move.w  #(CH_BPL4)>>16,BPL4PTH(a0)
        move.w  #(CH_BPL4)&$FFFF,BPL4PTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  AUD0123EN|DSKEN|SPREN|BPLEN
        move.b  CIABICR,d0                      ; read clears the CIA's data register (p.346)
        move.b  #$81,CIABICR                    ; set the mask bit of timer A (p.347)
        move.w  #INT_SET|INT_INTEN|INT_VERTB|INT_EXTER,INTENA(a0)
        S6_WAIT_DISK $DF40
        S7_FINISH_DISK
vblank_handler:
                addq.w  #1,FRAME_CTR
                lea     CUSTOM,a0
                move.w  #INT_VERTB,INTREQ(a0)
                move.b  #$08,CIABCRA            ; one-shot, stopped (p.348)
                move.b  #LATE_TICKS&$FF,CIABTALO
                move.b  #LATE_TICKS>>8,CIABTAHI ; in one-shot mode the write starts the timer (p.342)
                rte

late_handler:
                movem.l d0/a0,-(sp)
                lea     CUSTOM,a0
                move.b  CIABICR,d0              ; acknowledges the CIA
                move.w  #INT_EXTER,INTREQ(a0)
.wvert          move.w  INTREQR(a0),d0
                btst    #5,d0                   ; VERTB requested: line 0 of the next frame
                beq.s   .wvert
.wline          move.w  VHPOSR(a0),d0
                lsr.w   #8,d0
                cmp.b   #2,d0
                bcs.s   .wline
                cmp.b   #$40,d0                 ; before the first segment line
                bcc.s   .wline
                movem.l (sp)+,d0/a0
                rte

c6_spr_tab:     dc.l    CH_SPR0,CH_SPR0+$0400,CH_SPR0+$0800,CH_SPR0+$0C00
                dc.l    CH_SPR0+$1000,CH_SPR0+$1400,CH_SPR0+$1800,CH_SPR0+$1C00
c6_bpl_tab:     dc.l    CH_BPL1,CH_BPL2,CH_BPL3,CH_BPL4,0,0

        include "ch2build_code.i"
