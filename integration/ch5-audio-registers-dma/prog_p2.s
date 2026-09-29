; prog_p2.s -- P2: DMACONR at $DFF002 reads back the DMA control bits held
; by DMACON (SPECIFICATION.md P2). Sets up every DMA-capable subsystem so
; that a transient enable during the bit-by-bit sweep is harmless (the
; Copper list is an immediate end-of-list WAIT; BPLCON0 configures zero
; bitplanes; every sprite pointer is a null structure; DSKLEN is written
; once, which p.247 says is not enough to start disk DMA; BLTSIZE is never
; written), then writes DMACON in the sequence of the proposition's table,
; reading DMACONR back into R after each write.
;
; This file alone (no symbol set) is the MAIN build. prog_p2_omit.s
; (OMIT_BIT0 set) is the contrasting case: the SET write for bit 0 in the
; sweep is left out. prog_p2_g1.s (G1_BREAK set) is P2-G1's mutation
; (branch-to-self before DONE).

        include "ch5.i"

        org     PROGRAM_ORG

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     RSTORE,a3

        move.w  #$07FF,DMACON(a0)
        move.w  DMACONR(a0),(a3)+              ; row 1

        move.l  #LST,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)                  ; strobe (p.30)

        move.w  #$0200,BPLCON0(a0)

        move.l  #S0,SPR0PTH(a0)
        move.l  #N1,SPR0PTH+4(a0)
        move.l  #N2,SPR0PTH+8(a0)
        move.l  #N3,SPR0PTH+12(a0)
        move.l  #N4,SPR0PTH+16(a0)
        move.l  #N5,SPR0PTH+20(a0)
        move.l  #N6,SPR0PTH+24(a0)
        move.l  #N7,SPR0PTH+28(a0)

        move.l  #A0,AUD0LCH(a0)
        move.w  #8,AUD0LEN(a0)
        move.w  #0,AUD0VOL(a0)
        move.w  #200,AUD0PER(a0)
        move.l  #A1,AUD1LCH(a0)
        move.w  #8,AUD1LEN(a0)
        move.w  #0,AUD1VOL(a0)
        move.w  #200,AUD1PER(a0)
        move.l  #A2,AUD2LCH(a0)
        move.w  #8,AUD2LEN(a0)
        move.w  #0,AUD2VOL(a0)
        move.w  #200,AUD2PER(a0)
        move.l  #A3,AUD3LCH(a0)
        move.w  #8,AUD3LEN(a0)
        move.w  #0,AUD3VOL(a0)
        move.w  #200,AUD3PER(a0)

        move.w  #$4000,DSKLEN(a0)               ; p.247: this alone does not start disk DMA

        move.w  #$8200,DMACON(a0)
        move.w  DMACONR(a0),(a3)+              ; row 2

        ; row 3: SET/CLR pair for each b in 0,1,2,3,4,5,6,7,8,10
        ifd     OMIT_BIT0
        else
        move.w  #$8001,DMACON(a0)
        endif
        move.w  DMACONR(a0),(a3)+
        move.w  #$0001,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$8002,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.w  #$0002,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$8004,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.w  #$0004,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$8008,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.w  #$0008,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$8010,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.w  #$0010,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$8020,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.w  #$0020,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$8040,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.w  #$0040,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$8080,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.w  #$0080,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$8100,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.w  #$0100,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$8400,DMACON(a0)
        move.w  DMACONR(a0),(a3)+
        move.w  #$0400,DMACON(a0)
        move.w  DMACONR(a0),(a3)+

        move.w  #$85FF,DMACON(a0)
        move.w  DMACONR(a0),(a3)+              ; row 4
        move.w  #$0200,DMACON(a0)
        move.w  DMACONR(a0),(a3)+              ; row 5
        move.w  #$8200,DMACON(a0)
        move.w  DMACONR(a0),(a3)+              ; row 6
        move.w  #$07FF,DMACON(a0)
        move.w  DMACONR(a0),(a3)+              ; row 7

        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        DONE
