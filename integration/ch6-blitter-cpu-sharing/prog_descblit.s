; prog_descblit.s -- ch6-desc.c12 [p182]: the blitter's own descending-mode
; pointer convention differs from the 680x0 -(An) predecrement convention.
; Descending mode is BLTCON1 bit 1 (p.182: "Descending mode is turned on
; by setting bit one of BLTCON1"); in that mode BLTAPTH/BLTDPTH must be
; preloaded with the address of the LAST word of the block (the blitter
; then decrements each step), not the address one word past it the way a
; 680x0 -(An) operand
; would leave a register pointing after a full pass. Copies a known
; 4-word pattern ($1111,$2222,$3333,$4444, ascending in DB_SRC) from
; DB_SRC to DB_DST in descending mode, straight A-to-D (no shift), with a
; fifth, distinct sentinel word ($5555) one word past the block on both
; sides so a one-word displacement is directly observable in ram_log().
; This file alone (no symbol set) is the blitter's own convention:
; BLTAPTH/BLTDPTH point at DB_SRC+6/DB_DST+6, the last word. Chip RAM is
; zero-initialized by the harness, so an untouched destination word reads
; back $0000.
;
; prog_descblit_offby1.s (OFFBY1 set, ch6-desc.c12's own contrast) points
; BLTAPTH/BLTDPTH at DB_SRC+8/DB_DST+8 instead -- one word past the last,
; the 680x0 -(An) convention -- so the same 4-word transfer is displaced
; by one word: the block's own first destination word goes unwritten, and
; the sentinel word past the block is written instead.
;
; prog_descblit_offby2.s (OFFBY2 set, DESC-OFFBY1's own mutation) points
; BLTAPTH/BLTDPTH at DB_SRC+10/DB_DST+10 -- two words past the last --
; displacing the same transfer by two words instead of one.

        include "nanomig.i"

DB_SRC  equ     $0C2000         ; 5 words: $1111,$2222,$3333,$4444,$5555
DB_DST  equ     $0C2100         ; 5 destination words, harness zero-initialized

        org     PROGRAM_ORG

start:
        move.w  #$1111,DB_SRC
        move.w  #$2222,DB_SRC+2
        move.w  #$3333,DB_SRC+4
        move.w  #$4444,DB_SRC+6
        move.w  #$5555,DB_SRC+8

        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; SET DMAEN,BLTEN
        move.w  #$09F0,BLTCON0(a0)     ; USEA,USED, LF=$F0 (D=A straight copy)
        move.w  #$0002,BLTCON1(a0)     ; DESC set (bit 1), no other mode bits
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        ifd     OFFBY2
        move.l  #DB_SRC+10,BLTAPTH(a0) ; two words past the last
        move.l  #DB_DST+10,BLTDPTH(a0)
        else
        ifd     OFFBY1
        move.l  #DB_SRC+8,BLTAPTH(a0)  ; one word past the last (680x0 -(An) convention)
        move.l  #DB_DST+8,BLTDPTH(a0)
        else
        move.l  #DB_SRC+6,BLTAPTH(a0)  ; the last word (the blitter's own convention)
        move.l  #DB_DST+6,BLTDPTH(a0)
        endif
        endif
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.w  #$0101,BLTSIZE(a0)     ; height=4,width=1 -- starts the blit

.wait:  move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .wait

        DONE
