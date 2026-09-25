; truthorder.s -- p.174's own worked example: D = A+B, read from the
; bottom of the truth table up, is LF = $FC (6-function-generator.c24,
; ch6-funcgen.c6). A=FF00, B=F0F0; C is not used by this function.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        ifnd    TOPDOWN
        move.w  #$0DFC,BLTCON0(a0)     ; USEA+USEB+USED, LF=$FC (bottom-up: A+B)
        else
        move.w  #$0D3F,BLTCON0(a0)     ; mutation: LF=$3F (top-down reading)
        endc
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT
        DONE

        org     MASKBUF
        dc.w    $FF00
        org     SRC
        dc.w    $F0F0
