; fig63.s -- Figure 6-3 (p.178) as a blit: the A circle encloses minterms
; 7,6,5,4 = LF $F0, giving D = A (ch6-venn.f-labels). Table 6-1 (p.177)
; gives LF $CC = B and LF $AA = C, confirming the diagram's B and C
; circles too. A=FF00, B=F0F0, C=CCCC cover all eight combinations.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        BLTEN_ON

        ; LF = $F0: D = A (the A circle)
        ifnd    WRONGLF
        move.w  #$0FF0,BLTCON0(a0)
        else
        move.w  #$0F0F,BLTCON0(a0)     ; mutation: NOT A instead of A
        endc
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,SRC2
        SETPTR  BLTDPTH,BLTDPTL,DST
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT

        ; LF = $CC: D = B (the B circle)
        move.w  #$0FCC,BLTCON0(a0)
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,SRC2
        SETPTR  BLTDPTH,BLTDPTL,DST2
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT

        ; LF = $AA: D = C (the C circle)
        move.w  #$0FAA,BLTCON0(a0)
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,SRC2
        SETPTR  BLTDPTH,BLTDPTL,DST3
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT
        DONE

        org     MASKBUF
        dc.w    $FF00
        org     SRC
        dc.w    $F0F0
        org     SRC2
        dc.w    $CCCC
