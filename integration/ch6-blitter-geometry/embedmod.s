; embedmod.s -- A3: twice the blit width in words plus the byte modulo
; equals the containing row's byte width (6-dma-channels.c65). Figure
; 6-2's own numbers and position (p.173): a 40-byte row, a 5-word
; (10-byte) rectangle at row 13, byte column 12, 12 bytes skip-left + 18
; bytes skip-right = 30-byte modulo, height 5 (rows 13-17)
; (6-dma-channels.c73/c74/c75/c76, ch6-dma.f5/f7/f8).
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)     ; ascending
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)         ; source is a plain flat 5*5-word buffer
        ifnd    WRONGMOD
        move.w  #30,BLTDMOD(a0)
        else
        move.w  #20,BLTDMOD(a0)        ; mutation: wrong modulo
        endc
        SETPTR  BLTAPTH,BLTAPTL,SRC
        SETPTR  BLTDPTH,BLTDPTL,BITMAP+(40*13)+12      ; row 13, byte column 12
        BLTEN_ON
        move.w  #(5<<6)|5,BLTSIZE(a0)  ; height 5 (rows 13-17), width 5 words (10 bytes)
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
