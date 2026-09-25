; fourplanes.s -- 6-dma-channels.c36, ch6-dma.c34: moving a sixteen-colour
; (four-bitplane) image needs four separate copy operations. Four
; separate plane bitmaps (40 bytes x 12 rows each, far apart in memory),
; a 7-word x 6-row image embedded at row 2, column 0 in each. One blit's
; row modulo (40 - 2*7 = 26 bytes) only ever steps to the next row of the
; SAME bitmap -- it cannot reach a different plane's separate buffer, so
; moving all four images takes four separate blits, one pointer pair
; each. BLTAPT is reloaded before each blit: ascending mode leaves it
; advanced past the 42-word image once a blit completes.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)         ; source is a plain flat 7*6-word buffer
        move.w  #26,BLTDMOD(a0)        ; 40 - 2*7: stays within one plane's bitmap
        BLTEN_ON

        SETPTR  BLTAPTH,BLTAPTL,PLANEIMG
        SETPTR  BLTDPTH,BLTDPTL,PLANEBMP0+80   ; row 2, column 0
        move.w  #(6<<6)|7,BLTSIZE(a0)
        WAITBLT

        ifnd    ONLYPLANE1
        SETPTR  BLTAPTH,BLTAPTL,PLANEIMG
        SETPTR  BLTDPTH,BLTDPTL,PLANEBMP1+80
        move.w  #(6<<6)|7,BLTSIZE(a0)
        WAITBLT

        SETPTR  BLTAPTH,BLTAPTL,PLANEIMG
        SETPTR  BLTDPTH,BLTDPTL,PLANEBMP2+80
        move.w  #(6<<6)|7,BLTSIZE(a0)
        WAITBLT

        SETPTR  BLTAPTH,BLTAPTL,PLANEIMG
        SETPTR  BLTDPTH,BLTDPTL,PLANEBMP3+80
        move.w  #(6<<6)|7,BLTSIZE(a0)
        WAITBLT
        endc
        DONE

        org     PLANEIMG
        dc.w    $1111,$2222,$3333,$4444,$5555,$6666,$7777
        dc.w    $1111,$2222,$3333,$4444,$5555,$6666,$7777
        dc.w    $1111,$2222,$3333,$4444,$5555,$6666,$7777
        dc.w    $1111,$2222,$3333,$4444,$5555,$6666,$7777
        dc.w    $1111,$2222,$3333,$4444,$5555,$6666,$7777
        dc.w    $1111,$2222,$3333,$4444,$5555,$6666,$7777
