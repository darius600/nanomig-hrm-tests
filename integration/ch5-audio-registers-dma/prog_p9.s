; prog_p9.s -- P9: after a blit, BZERO (bit 13) reads 1 exactly when the
; blit's logic output was zero throughout (SPECIFICATION.md P9). Five
; blits in turn, each ending with a dummy read, a poll until BBUSY (bit 14)
; clears, the store into G of the read that showed bit 14 = 0, and one more
; read stored as Z_k in R.
;
; This file alone is the MAIN build. prog_p9_c1.s (C1 set) exchanges
; BLTCON0 between blits 1/2 and the source F/Zb between blits 4/5.
; prog_p9_g1.s (G1_BREAK), prog_p9_g2.s (G2_BREAK, SPREN also set in the
; enabling write), prog_p9_gnowait.s (G_NOWAIT, drops the wait before Z_1)
; and prog_p9_gdwrite.s (D4_ON, blit 4 also writes D) are the precondition
; mutations.

        include "ch5.i"

        ifnd    BC0_1
BC0_1   equ     $0100
        endif
        ifnd    BC0_2
BC0_2   equ     $01FF
        endif
        ifnd    ASRC4
ASRC4   equ     FW
        endif
        ifnd    ASRC5
ASRC5   equ     ZB
        endif

        org     PROGRAM_ORG

ENDM    equ     $00FF

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     MK,a2
        lea     RSTORE,a3
        lea     GLOG,a4

        move.w  #$07FF,DMACON(a0)
        ifd     G2_BREAK
        move.w  #$8260,DMACON(a0)       ; SPREN also set
        else
        move.w  #$8240,DMACON(a0)
        endif

        move.w  #0,BLTCON1(a0)
        move.w  #0,BLTDMOD(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)

        ; ---- blit 1 ----
        move.w  #BC0_1,BLTCON0(a0)
        move.l  #QQ,BLTDPTH(a0)
        move.w  #$0104,BLTSIZE(a0)
        ifd     G_NOWAIT
        move.w  DMACONR(a0),d0          ; no wait: one read straight after BLTSIZE
        else
        move.w  DMACONR(a0),d0
.w1:    move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .w1
        endif
        move.w  d0,(a4)+               ; the last read of the wait
        move.w  DMACONR(a0),(a3)+      ; Z1

        ; ---- blit 2 ----
        move.w  #BC0_2,BLTCON0(a0)
        move.l  #QQ+$100,BLTDPTH(a0)
        move.w  #$0104,BLTSIZE(a0)
        move.w  DMACONR(a0),d0
.w2:    move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .w2
        move.w  d0,(a4)+
        move.w  DMACONR(a0),(a3)+      ; Z2

        ; ---- blit 3 ----
        move.w  #$0100,BLTCON0(a0)
        move.l  #QQ+$200,BLTDPTH(a0)
        move.w  #$0104,BLTSIZE(a0)
        move.w  DMACONR(a0),d0
.w3:    move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .w3
        move.w  d0,(a4)+
        move.w  DMACONR(a0),(a3)+      ; Z3

        ; ---- blit 4 (USEA only, D=A, source ASRC4, D not written) ----
        ifd     D4_ON
        move.w  #$09F0,BLTCON0(a0)      ; precondition mutation: USEA and USED
        move.l  #QQ+$300,BLTDPTH(a0)
        else
        move.w  #$08F0,BLTCON0(a0)
        endif
        move.l  #ASRC4,BLTAPTH(a0)
        move.w  #$0104,BLTSIZE(a0)
        move.w  DMACONR(a0),d0
.w4:    move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .w4
        move.w  d0,(a4)+
        move.w  DMACONR(a0),(a3)+      ; Z4

        ; ---- blit 5 (source ASRC5) ----
        move.w  #$08F0,BLTCON0(a0)
        move.l  #ASRC5,BLTAPTH(a0)
        move.w  #$0104,BLTSIZE(a0)
        move.w  DMACONR(a0),d0
.w5:    move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .w5
        move.w  d0,(a4)+
        move.w  DMACONR(a0),(a3)+      ; Z5

        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        move.w  #ENDM,(a2)
        DONE
