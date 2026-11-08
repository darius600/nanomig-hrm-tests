; arbgrow.s -- A8a: arbitrary-region copy where the destination spans
; more words than the source. A 23-bit-wide pattern (word0 = 16 bits,
; word1 = its leading 7 bits) shifted right 12 bits spans 3 words
; (6-copying-arbitrary-regions.c1, ch6-copy.c1, ch6-copy.c2). The A
; channel is the constant mask (BLTADAT-equivalent, ASH=0, ch6-copy.c13);
; BLTAFWM/BLTALWM mask it as if masking the destination's own first/last
; word (ch6-copy.c14); D = A AND B (LF=$C0) so the mask clips B's shifted
; data, in particular the real neighbour word fetched past the true
; 2-word source (SRC+4, standing in for real adjacent bitmap data) that
; the trailing edge would otherwise leak into word 2. Blit size is the
; larger of the two spans, 3 (ch6-copy.c4, ch6-copy.c9).
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        ifnd    WRONGASHA
        move.w  #$0DC0,BLTCON0(a0)     ; ASH=0, USEA+USEB+USED, LF=$C0 (A AND B)
        else
        move.w  #$CDC0,BLTCON0(a0)     ; mutation: ASH=12, A shifted as B is
        endc
        move.w  #$C000,BLTCON1(a0)     ; BSH=12
        ifnd    WRONGMASKS
        move.w  #$000F,BLTAFWM(a0)
        move.w  #$E000,BLTALWM(a0)
        else
        move.w  #$FFFF,BLTAFWM(a0)     ; mutation: no destination-edge masking
        move.w  #$FFFF,BLTALWM(a0)
        endc
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        ifnd    WRONGSIZE
        move.w  #(1<<6)|3,BLTSIZE(a0)  ; height 1, width 3 (the larger span)
        else
        move.w  #(1<<6)|2,BLTSIZE(a0)  ; mutation: only 2, the naive span
        endc
        WAITBLT
        DONE

        org     MASKBUF
        dc.w    $FFFF,$FFFF,$FFFF
        org     SRC
        dc.w    $FFFF,$FE00,$F000      ; the 23-bit image, then a real neighbour word
