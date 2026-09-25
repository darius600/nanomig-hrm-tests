; arbmask.s -- ch6-copy.c11/c12: for an arbitrary-region copy, the A
; channel is a disabled, all-ones row mask (p.183 rule 1) whose shift
; matches the source (B channel)'s own shift (rule 4: same shift value
; for A as for source B, ch6-copy.c11), and whose first/last masks are
; set as if masking the B source data's edges (ch6-copy.c12). A=disabled,
; BLTADAT=$FFFF, ASH=BSH=2, BLTAFWM=$0003, BLTALWM=$C000; B = $FFFF x3
; (uniform, standing for "past the true image" data on both edges); C=D
; = background $5555 x3; LF=$CA (cookie-cut: D = A?B:C).
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$27CA,BLTCON0(a0)     ; ASH=2, USEB+USEC+USED (A disabled), LF=$CA
        move.w  #$2000,BLTCON1(a0)     ; BSH=2
        move.w  #$FFFF,BLTADAT(a0)     ; A disabled, preloaded all-ones
        ifnd    NOMASK
        move.w  #$0003,BLTAFWM(a0)
        move.w  #$C000,BLTALWM(a0)
        else
        move.w  #$FFFF,BLTAFWM(a0)     ; mutation: no masking
        move.w  #$FFFF,BLTALWM(a0)
        endc
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,DST
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        move.w  #(1<<6)|3,BLTSIZE(a0)
        WAITBLT
        DONE

        org     SRC
        dc.w    $FFFF,$FFFF,$FFFF
        org     DST
        dc.w    $5555,$5555,$5555
