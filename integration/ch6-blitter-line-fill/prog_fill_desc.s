; prog_fill_desc.s -- ch6-blitter-line-fill: FILL-DESC-CORRECT /
; FILL-ASC-INCORRECT (page 182, 184, 200). Builds a 5-word, 1-line boundary
; row in chip RAM with a single marker bit in word0, word2 and word4 (words
; 1 and 3 carry no marker), then blits it through the blitter's inclusive
; area-fill logic: channel C supplies the row (DMA fetch), channel A and B
; are unused, BLTCON0's minterm is programmed so D = C (channel A/B forced
; to 0, see SPECIFICATION.md), and channel D writes the filled row to a
; second buffer. Reads back the two marker-free words (word1, word3).
;
; ASCENDING, if defined, runs the same blit with the pointer started at
; word0 instead of word4 and DESC cleared instead of set (prog_fill_asc.s).
; MOVE_MARKER, if defined, moves the middle marker from word2 into word1
; (prog_fill_desc_mut.s). DROP_MARKER0, if defined, removes word0's marker,
; leaving only word2 and word4 (prog_fill_asc_mut.s, combined with
; ASCENDING: with word0 no longer contributing a toggle, the ascending scan
; reproduces the descending-mode word1/word3 pattern by coincidence).

        include "nanomig.i"
        org     PROGRAM_ORG

SRC_ROW equ     $082000
DST_ROW equ     $082100
MARK    equ     $0100

start:
        move.l  #SRC_ROW,a1
        ifd     MOVE_MARKER
        move.w  #MARK,(a1)             ; word0
        move.w  #MARK,2(a1)            ; word1 (marker moved here)
        move.w  #0,4(a1)                ; word2 (no longer a marker)
        move.w  #0,6(a1)                ; word3
        move.w  #MARK,8(a1)             ; word4
        else
        ifd     DROP_MARKER0
        move.w  #0,(a1)                ; word0 (marker removed)
        move.w  #0,2(a1)                 ; word1
        move.w  #MARK,4(a1)               ; word2
        move.w  #0,6(a1)                   ; word3
        move.w  #MARK,8(a1)                 ; word4
        else
        move.w  #MARK,(a1)             ; word0
        move.w  #0,2(a1)                ; word1
        move.w  #MARK,4(a1)             ; word2
        move.w  #0,6(a1)                 ; word3
        move.w  #MARK,8(a1)              ; word4
        endif
        endif

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

        ifd     ASCENDING
        move.w  #$0008,BLTCON1(a0)     ; DESC=0, IFE=1, FCI=0
        move.l  #SRC_ROW,d0
        move.l  #DST_ROW,d1
        else
        move.w  #$000A,BLTCON1(a0)     ; DESC=1, IFE=1, FCI=0
        move.l  #SRC_ROW+8,d0
        move.l  #DST_ROW+8,d1
        endif

        move.w  d0,BLTCPTL(a0)
        swap    d0
        move.w  d0,BLTCPTH(a0)

        move.w  d1,BLTDPTL(a0)
        swap    d1
        move.w  d1,BLTDPTH(a0)

        move.w  #$0045,BLTSIZE(a0)     ; height=1, width=5 -- starts the blit

.wait:  move.w  DMACONR(a0),d3
        btst.l  #14,d3                 ; BBUSY
        bne.s   .wait

        lea     RESULTS,a1
        move.l  #DST_ROW+2,a2
        move.w  (a2),(a1)+              ; word1
        move.l  #DST_ROW+6,a2
        move.w  (a2),(a1)+               ; word3

        DONE
