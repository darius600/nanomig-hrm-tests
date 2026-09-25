; shiftc9.s -- ch6-copy.c9: shift values are the bit position of the
; leftmost edge within a word, not an absolute shift. Moving source 1111
; 2222 3333 four pixels left: the edge lands at bit 14 of the word
; before, which is a right shift of 12 (ASH=12) with BLTDPT one word
; lower -- ascending mode gives the moved (leftward) image, even though
; the register-level shift value and mode are those of a rightward move.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$C9F0,BLTCON0(a0)     ; ASH=12, USEA+USED, LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)     ; ascending
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,SRC
        ifnd    NOLOWPTR
        SETPTR  BLTDPTH,BLTDPTL,DST-2  ; one word lower than "naive"
        else
        SETPTR  BLTDPTH,BLTDPTL,DST    ; mutation: naive (same-address) pointer
        endc
        BLTEN_ON
        move.w  #(1<<6)|3,BLTSIZE(a0)
        WAITBLT
        DONE

        org     SRC
        dc.w    $1111,$2222,$3333
