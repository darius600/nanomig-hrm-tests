; highbit.s -- 6-dma-channels.c21, ch6-dma.c19: some of a pointer's most
; significant bits are ignored. BLTDPTH only stores the low 5 bits of the
; word written to it (bits 20:16 of the pointer), so bits above 20 of a
; 32-bit-style address are structurally unreachable regardless of chip
; size -- setting one (bit 24) has no effect on where the blit writes.
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
        SETPTR  BLTDPTH,BLTDPTL,DST|$01000000   ; bit 24 set, above the register's range
        BLTEN_ON
        move.w  #(1<<6)|4,BLTSIZE(a0)
        WAITBLT
        DONE

        org     SRC
        dc.w    $1234,$5678,$9ABC,$DEF0
