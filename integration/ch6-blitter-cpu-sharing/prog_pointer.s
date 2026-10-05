; prog_pointer.s -- A1 [p170]: each blitter pointer register is two adjacent
; words that one 32-bit processor write sets. Four one-word blits, one per
; channel, each with its own source and destination words and its own
; decoy; in each blit the pointer of the channel under test is set by a
; single MOVE.L to BLTxPTH, every other pointer by two MOVE.W writes, so
; that a failed 32-bit write shows in that channel alone:
;   A: BLTAPTH by MOVE.L  (D = A, minterm $F0)   result at PTR_DST_A
;   B: BLTBPTH by MOVE.L  (D = B, minterm $CC)   result at PTR_DST_B
;   C: BLTCPTH by MOVE.L  (D = C, minterm $AA)   result at PTR_DST_C
;   D: BLTDPTH by MOVE.L  (D = A, minterm $F0)   result at PTR_DST_D
; The four result words are stored to RESULTS, RESULTS+2, +4 and +6.
; Every source word holds a distinct pattern and every decoy another, so a
; pointer that missed its target reads a different, wrong value. The
; mutants set one channel's 32-bit write (PTR_MUT_A .. PTR_MUT_D), or all
; four (prog_pointer_mutant.s), to the channel's decoy address instead.

        include "nanomig.i"

PTR_SRC_A       equ     $0C2000
PTR_ALT_A       equ     $0C2100
PTR_DST_A       equ     $0C2200
PTR_SRC_B       equ     $0C2300
PTR_ALT_B       equ     $0C2400
PTR_DST_B       equ     $0C2500
PTR_SRC_C       equ     $0C2600
PTR_ALT_C       equ     $0C2700
PTR_DST_C       equ     $0C2800
PTR_SRC_D       equ     $0C2900
PTR_DST_D       equ     $0C2A00
PTR_ALT_D       equ     $0C2B00

        org     PROGRAM_ORG

start:
        move.w  #$CAFE,PTR_SRC_A
        move.w  #$5150,PTR_ALT_A
        move.w  #$B00B,PTR_SRC_B
        move.w  #$0BAD,PTR_ALT_B
        move.w  #$C0DE,PTR_SRC_C
        move.w  #$FACE,PTR_ALT_C
        move.w  #$D00D,PTR_SRC_D

        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; SET DMAEN,BLTEN
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTBMOD(a0)
        move.w  #$0000,BLTCMOD(a0)
        move.w  #$0000,BLTDMOD(a0)

        ; ---- A: BLTAPTH/BLTAPTL by one MOVE.L
        move.w  #$09F0,BLTCON0(a0)
        move.w  #(PTR_DST_A>>16),BLTDPTH(a0)
        move.w  #(PTR_DST_A&$FFFF),BLTDPTL(a0)
        ifd     PTR_MUT_A
        move.l  #PTR_ALT_A,BLTAPTH(a0)
        else
        move.l  #PTR_SRC_A,BLTAPTH(a0)
        endif
        move.w  #$0041,BLTSIZE(a0)
.wa:    move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .wa

        ; ---- B: BLTBPTH/BLTBPTL by one MOVE.L
        move.w  #$05CC,BLTCON0(a0)
        move.w  #(PTR_DST_B>>16),BLTDPTH(a0)
        move.w  #(PTR_DST_B&$FFFF),BLTDPTL(a0)
        ifd     PTR_MUT_B
        move.l  #PTR_ALT_B,BLTBPTH(a0)
        else
        move.l  #PTR_SRC_B,BLTBPTH(a0)
        endif
        move.w  #$0041,BLTSIZE(a0)
.wb:    move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .wb

        ; ---- C: BLTCPTH/BLTCPTL by one MOVE.L
        move.w  #$03AA,BLTCON0(a0)
        move.w  #(PTR_DST_C>>16),BLTDPTH(a0)
        move.w  #(PTR_DST_C&$FFFF),BLTDPTL(a0)
        ifd     PTR_MUT_C
        move.l  #PTR_ALT_C,BLTCPTH(a0)
        else
        move.l  #PTR_SRC_C,BLTCPTH(a0)
        endif
        move.w  #$0041,BLTSIZE(a0)
.wc:    move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .wc

        ; ---- D: BLTDPTH/BLTDPTL by one MOVE.L
        move.w  #$09F0,BLTCON0(a0)
        move.w  #(PTR_SRC_D>>16),BLTAPTH(a0)
        move.w  #(PTR_SRC_D&$FFFF),BLTAPTL(a0)
        ifd     PTR_MUT_D
        move.l  #PTR_ALT_D,BLTDPTH(a0)
        else
        move.l  #PTR_DST_D,BLTDPTH(a0)
        endif
        move.w  #$0041,BLTSIZE(a0)
.wd:    move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .wd

        move.w  PTR_DST_A,RESULTS
        move.w  PTR_DST_B,RESULTS+2
        move.w  PTR_DST_C,RESULTS+4
        move.w  PTR_DST_D,RESULTS+6

        DONE
