; prog_fill_plain.s -- FIXTURE for the fill cluster: the same 5-word,
; 1-line, 3-marker row as prog_fill_desc.s, blitted with fill disabled
; (IFE=EFE=0, a plain channel C -> D copy through the minterm). Proves the
; marker data and the blit's pointer/pipeline setup reach RESULTS exactly
; as written before any fill-mode assertion is trusted.

        include "nanomig.i"
        org     PROGRAM_ORG

SRC_ROW equ     $082000
DST_ROW equ     $082200
MARK    equ     $0100

start:
        move.l  #SRC_ROW,a1
        move.w  #MARK,(a1)             ; word0
        move.w  #0,2(a1)                ; word1
        move.w  #MARK,4(a1)             ; word2
        move.w  #0,6(a1)                 ; word3
        move.w  #MARK,8(a1)              ; word4

        move.l  #DST_ROW,a1
        move.w  #0,(a1)
        move.w  #0,2(a1)
        move.w  #0,4(a1)
        move.w  #0,6(a1)
        move.w  #0,8(a1)

        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; set DMAEN | BLTEN
        move.w  #0,BLTADAT(a0)
        move.w  #0,BLTBDAT(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        move.w  #$0302,BLTCON0(a0)     ; USEC|USED, LF=$02 (D = C when A=B=0)
        move.w  #$0000,BLTCON1(a0)     ; DESC=0, IFE=0, EFE=0 -- fill disabled

        move.l  #SRC_ROW,d0
        move.w  d0,BLTCPTL(a0)
        swap    d0
        move.w  d0,BLTCPTH(a0)

        move.l  #DST_ROW,d1
        move.w  d1,BLTDPTL(a0)
        swap    d1
        move.w  d1,BLTDPTH(a0)

        move.w  #$0045,BLTSIZE(a0)     ; height=1, width=5 -- starts the blit

.wait:  move.w  DMACONR(a0),d3
        btst.l  #14,d3                 ; BBUSY
        bne.s   .wait

        lea     RESULTS,a1
        move.l  #DST_ROW,a2
        move.w  (a2)+,(a1)+             ; word0
        move.w  (a2)+,(a1)+              ; word1
        move.w  (a2)+,(a1)+               ; word2
        move.w  (a2)+,(a1)+                ; word3
        move.w  (a2)+,(a1)+                 ; word4

        DONE
