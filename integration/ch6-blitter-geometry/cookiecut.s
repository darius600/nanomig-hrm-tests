; cookiecut.s -- A4: a cookie-cut mask is one bitplane regardless of
; background depth, using function $CA (6-function-generator.c40,
; ch6-minterms.c9, ch6-copy.c6): D = (A AND B) OR (NOT A AND C). The same
; mask (A) and object (B) are blitted against two different backgrounds
; (C = D, run1 at DST, run2 at DST2).
;
; A5 (ch6-minterms.c12) reuses run1's RAM log: channel C reads the
; background at the same address D writes, so a read must precede the
; write at every destination word.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        ifnd    WRONGLF
        move.w  #$0FCA,BLTCON0(a0)     ; USEA+USEB+USEC+USED, LF=$CA
        else
        move.w  #$0FAC,BLTCON0(a0)     ; mutation: wrong function
        endc
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        BLTEN_ON

        ; run 1: background/destination = DST. A and B are reloaded before
        ; each run: ascending mode leaves every active pointer advanced
        ; past the end of its buffer once a blit completes.
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,DST
        SETPTR  BLTDPTH,BLTDPTL,DST
        move.w  #(1<<6)|4,BLTSIZE(a0)
        WAITBLT

        ; run 2: same mask and object, reloaded, against a different
        ; background = DST2
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,DST2
        SETPTR  BLTDPTH,BLTDPTL,DST2
        move.w  #(1<<6)|4,BLTSIZE(a0)
        WAITBLT
        DONE

        org     MASKBUF
        dc.w    $F0F0,$F0F0,$F0F0,$F0F0
        org     SRC
        dc.w    $AAAA,$AAAA,$AAAA,$AAAA
        org     DST
        dc.w    $5555,$5555,$5555,$5555
        org     DST2
        dc.w    $3333,$3333,$3333,$3333
