; highbit.s -- 6-dma-channels.c21, ch6-dma.c19: some of a pointer's most
; significant bits are ignored. p.309: "BLTxPTH ... (high 5 bits, was 3
; bits)" -- the register keeps pointer bits 20:16, so bit 21 is the
; lowest structurally-ignored bit, for any chip size. Bit 20 is within
; that kept range and does change the target.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,SRC
        ifnd    WRONGBIT
        SETPTR  BLTDPTH,BLTDPTL,DST|$00200000   ; bit 21, above the kept range (p.309)
        else
        SETPTR  BLTDPTH,BLTDPTL,DST|$00100000   ; mutation: bit 20, within the kept range
        endc
        BLTEN_ON
        move.w  #(1<<6)|4,BLTSIZE(a0)
        WAITBLT
        DONE

        org     SRC
        dc.w    $1234,$5678,$9ABC,$DEF0
