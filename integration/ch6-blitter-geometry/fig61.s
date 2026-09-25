; fig61.s -- Figure 6-1 (p.171) as a blit: a bitplane's word addresses
; increase by one across a row and across the row boundary
; (6-dma-channels.c30, c31, c32, ch6-dma.f1, f2, f3). BLTAPT = word
; address 20 (byte $28), 7 words/row, 6 rows -- channel-A reads must
; cover exactly words 20..61. The overlay is cleared first: word 20 is
; inside $000000-$07FFFF, which the overlay would otherwise redirect for
; CPU reads (it does not redirect blitter DMA, but chip0's own bank is
; gated by "!ovl || cpu_hlt").
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        CLEAR_OVL
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)     ; ascending
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTAPTH(a0)
        ifnd    WRONGSTART
        move.w  #$0028,BLTAPTL(a0)     ; word address 20 = byte $000028
        else
        move.w  #$002A,BLTAPTL(a0)     ; mutation: word address 21
        endc
        move.w  #2,BLTDMOD(a0)         ; one-word gap marks each row end in D
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        move.w  #(6<<6)|7,BLTSIZE(a0)  ; height 6, width 7 words
        WAITBLT
        DONE
