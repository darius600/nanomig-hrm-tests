; mem512.s -- A20: p.170's 512 KB pointer claim, distinguishing which high
; bits are ignored. Channel A is disabled and its data preloaded directly
; (BLTADAT), so no source pointer is needed. Three blits write to DST
; ($086000), DST+$040000 ($0C6000) and DST+$100002 ($186002); this same
; program also runs unmodified under the 2 MB configuration, where the
; first pointer must be honoured, not folded.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$01F0,BLTCON0(a0)     ; ASH=0, USED only (A disabled), LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTDMOD(a0)
        BLTEN_ON

        move.w  #$AAAA,BLTADAT(a0)
        SETPTR  BLTDPTH,BLTDPTL,DST
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT

        move.w  #$BBBB,BLTADAT(a0)
        SETPTR  BLTDPTH,BLTDPTL,DST+$040000
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT

        move.w  #$CCCC,BLTADAT(a0)
        SETPTR  BLTDPTH,BLTDPTL,DST+$100002
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT
        DONE
