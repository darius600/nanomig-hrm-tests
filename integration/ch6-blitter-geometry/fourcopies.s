; fourcopies.s -- moving a sixteen-colour (four-bitplane) image requires
; four separate copy operations (6-dma-channels.c36, ch6-dma.c34): one
; blit per plane, each with its own pointer pair. Four one-word "planes"
; stand in for four bitplanes.
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
        BLTEN_ON

        SETPTR  BLTAPTH,BLTAPTL,PLANES+0
        SETPTR  BLTDPTH,BLTDPTL,PLANESDST+0
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT

        SETPTR  BLTAPTH,BLTAPTL,PLANES+2
        SETPTR  BLTDPTH,BLTDPTL,PLANESDST+2
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT

        SETPTR  BLTAPTH,BLTAPTL,PLANES+4
        SETPTR  BLTDPTH,BLTDPTL,PLANESDST+4
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT

        ifnd    ONLYTHREE
        SETPTR  BLTAPTH,BLTAPTL,PLANES+6
        SETPTR  BLTDPTH,BLTDPTL,PLANESDST+6
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT
        endc
        DONE

        org     PLANES
        dc.w    $AAAA,$BBBB,$CCCC,$DDDD
