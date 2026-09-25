; prog_line.s -- ch6-blitter-line-fill: LINE-RAW-DELTA, LINE-NORMALIZE and
; the LINE-NORMALIZE-NOOP control (page 190-191). Pure CPU arithmetic: the
; manual's line-mode setup recipe (dx = abs(x2-x1), dy = abs(y2-y1), then
; dx,dy swapped if necessary so dx is the greater) computed for two endpoint
; pairs and stored where the harness reads it back. MUTATE_EP1, if defined,
; changes the first pair's endpoints (prog_line_mut.s).

        include "nanomig.i"
        org     PROGRAM_ORG

start:
        lea     RESULTS,a1
        move.w  #$BEEF,(a1)+           ; fixture: known constant, checked first

        ; ---- EP1: (10,5)-(3,40) ----
        ifd     MUTATE_EP1
        move.w  #0,d0                  ; x1
        move.w  #0,d1                  ; y1
        move.w  #6,d2                  ; x2
        move.w  #2,d3                  ; y2
        else
        move.w  #10,d0                 ; x1
        move.w  #5,d1                  ; y1
        move.w  #3,d2                  ; x2
        move.w  #40,d3                 ; y2
        endif

        move.w  d2,d4
        sub.w   d0,d4                  ; d4 = x2-x1
        bpl.s   .xok1
        neg.w   d4
.xok1:
        move.w  d4,(a1)+               ; dx0

        move.w  d3,d5
        sub.w   d1,d5                  ; d5 = y2-y1
        bpl.s   .yok1
        neg.w   d5
.yok1:
        move.w  d5,(a1)+               ; dy0

        move.w  d4,d6
        move.w  d5,d7
        cmp.w   d7,d6                  ; d6-d7
        bge.s   .noswap1
        exg     d6,d7
.noswap1:
        move.w  d6,(a1)+               ; dx_n
        move.w  d7,(a1)+               ; dy_n

        ; ---- EP2 (control, no swap needed): (0,0)-(50,10) ----
        moveq   #0,d0
        moveq   #0,d1
        move.w  #50,d2
        move.w  #10,d3

        move.w  d2,d4
        sub.w   d0,d4
        bpl.s   .xok2
        neg.w   d4
.xok2:
        move.w  d4,(a1)+               ; dx0 EP2

        move.w  d3,d5
        sub.w   d1,d5
        bpl.s   .yok2
        neg.w   d5
.yok2:
        move.w  d5,(a1)+               ; dy0 EP2

        move.w  d4,d6
        move.w  d5,d7
        cmp.w   d7,d6
        bge.s   .noswap2
        exg     d6,d7
.noswap2:
        move.w  d6,(a1)+               ; dx_n EP2
        move.w  d7,(a1)+               ; dy_n EP2

        DONE
