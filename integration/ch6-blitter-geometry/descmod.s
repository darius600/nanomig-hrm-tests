; descmod.s -- ch6-desc.c11: for a descending-mode copy, the modulo
; values, blit size and all other parameters are set the same as for the
; ascending copy. A3/embedmod's embedded-rectangle geometry (5-word
; rectangle, row 13, BLTDMOD=30) run in descending mode: pointers at the
; rectangle's last word (row 17), same BLTDMOD=30 and BLTSIZE as the
; ascending run. p.182: in descending mode "the modulo values will be
; subtracted rather than added" -- a zero modulo never exercises that,
; so this uses the same positive modulo as A3.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0002,BLTCON1(a0)     ; DESC=1
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        ifnd    ZEROMOD
        move.w  #30,BLTDMOD(a0)
        else
        move.w  #0,BLTDMOD(a0)         ; mutation: no modulo
        endc
        SETPTR  BLTAPTH,BLTAPTL,SRC+48                 ; source's last word
        SETPTR  BLTDPTH,BLTDPTL,BITMAP+(40*17)+12+8    ; rectangle's last word, row 17
        BLTEN_ON
        move.w  #(5<<6)|5,BLTSIZE(a0)  ; height 5, width 5 words -- same as A3
        WAITBLT
        DONE

        org     SRC
        dc.w    $1111,$2222,$3333,$4444,$5555
        dc.w    $6666,$7777,$8888,$9999,$AAAA
        dc.w    $BBBB,$CCCC,$DDDD,$EEEE,$FFFF
        dc.w    $1234,$2345,$3456,$4567,$5678
        dc.w    $6789,$789A,$89AB,$9ABC,$ABCD

        org     BITMAP
        dcb.w   360,$DEAD               ; 18 rows x 40 bytes (20 words), sentinel
