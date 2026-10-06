; prog_p7_donly.s -- P7's measured, ungraded case: a blit whose Table 6-2
; sequence leaves a cycle without a blitter access (USE code 1, D only:
; "D0 - D1 - D2", p.188), BLTPRI set. BLTCON0 $01FF is USED with LF $FF,
; BLTSIZE $2020 is 4096 words written to Q. The CPU loop and the markers are
; those of prog_p7.s. prog_p7_donly_c1.s (C1 set) is the same blit with
; BLTPRI clear.

        include "ch5.i"

        org     PROGRAM_ORG

OM      equ     $0090
CM      equ     $0091
ENDM    equ     $00FF

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     MK,a2
        lea     RSTORE,a3
        lea     GLOG,a4

        move.w  #$07FF,DMACON(a0)
        move.w  #$01FF,BLTCON0(a0)      ; USED only, LF $FF
        move.w  #0,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTDMOD(a0)
        move.l  #QQ,BLTDPTH(a0)

        ifd     C1
        move.w  #$8240,DMACON(a0)       ; BLTPRI clear (share)
        else
        move.w  #$8640,DMACON(a0)       ; SET,BLTPRI,DMAEN,BLTEN
        endif
        move.w  DMACONR(a0),(a3)+

        move.w  #OM,(a2)
        move.w  #$2020,BLTSIZE(a0)      ; height=128,width=32 -- starts the blit

        moveq   #0,d1
        move.w  #2047,d2
.L:     move.w  d1,(a4)+
        addq.w  #1,d1
        dbra    d2,.L

        move.w  #CM,(a2)
        move.w  #ENDM,(a2)
        DONE
