; prog_cb.s -- run COPBLT-v (SPECIFICATION.md P12, P13).
;
; Wrapper sources set the symbols below and include this file. The chip set
; (OCS or ECS) is the test's configuration; IS_ECS only adds the registers
; that exist on ECS (BLTSIZV before the run, blit 3 in the Copper list).
;
;   IS_ECS        -- the ECS variant (decoy BLTSIZV, Copper blit 3)
;   CV            -- the COPCON value (default $0002)
;   MUT_S81       -- first BLTSIZE MOVE carries $0081 (P12/P13 proposition contrast)
;   MUT_SIZH3     -- ECS: the BLTSIZH MOVE carries $0003 (P12 ECS-register contrast)
;   MUT_NOCOP     -- DMACON without COPEN (precondition 1 contrast)
;   MUT_NOBLT     -- DMACON without BLTEN (precondition 2 contrast)
;   MUT_CPUEARLY  -- the CPU writes BLTSIZE = $0082 before M1 (P12 precondition 3)
;   MUT_CPUSIZH   -- ECS: the CPU writes BLTSIZH = $0001 before M1 (P12 and P13
;                    structural contrast: a CPU store to BLTSIZH)
;   MUT_CPUBLT80  -- the CPU writes BLTSIZE = $0082 at V7-0 >= $80 of the first
;                    field inside M1-M2 (P13 pair-2 contrast, precondition 2)
;   MUT_CPUWR     -- the CPU writes $1111 to $30200 inside M1-M2 (P13 precondition 3)
;   MUT_FIXTURE   -- the fixture check's own contrast: $DEAD instead of $BEEF
;   MUT_NODONE    -- the program never writes DONE
;
; The code between M1 and M2 contains no subroutine call and no instruction
; that writes Chip memory other than the contrast writes named above: it
; reads and writes custom registers and polls INTREQR, so the window is
; quiet (SPECIFICATION.md 3.3).

        include "nanomig.i"
        include "sprreg.i"

        ifnd    CV
CV      equ     $0002
        endif

CM      macro
        dc.w    \1,\2
        endm

; WVB: wait for VERTB inline (d1 = $0020), then clear it.
WVB     macro
.w\@:   move.w  INTREQR(a0),d0
        and.w   d1,d0
        beq.s   .w\@
        move.w  #$0020,INTREQ(a0)
        endm

; FILLW dest,count-1,value
FILLW   macro
        move.l  #\1,a1
        move.w  #\2,d0
.f\@:   move.w  #\3,(a1)+
        dbra    d0,.f\@
        endm

; CPYT table,end,dest
CPYT    macro
        lea     \1(pc),a2
        move.l  #\3,a1
        move.w  #((\2-\1)/2)-1,d0
.c\@:   move.w  (a2)+,(a1)+
        dbra    d0,.c\@
        endm

        org     PROGRAM_ORG

start:
        CLEAR_OVL

        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTREQ(a0)

        ; buffers (SPECIFICATION.md P12 step 1), filled by the CPU itself
        CPYT    srca,srcb,BLIT_BASE
        CPYT    srcb,srcc,BLIT_BASE+$10
        CPYT    srcc,srcend,BLIT_BASE+$20
        FILLW   BLIT_BASE+$200,3,$EEEE          ; DST1
        FILLW   BLIT_BASE+$210,3,$EEEE          ; DST2
        FILLW   BLIT_BASE+$220,15,$EEEE         ; DST3
        FILLW   BLIT_BASE+$100,127,$3333        ; decoy sources
        FILLW   BLIT_BASE+$300,127,$EEEE        ; decoy destinations

        ; decoys, written by the CPU (step 2)
        move.w  #$0100,BLTCON0(a0)
        move.w  #$0002,BLTCON1(a0)
        move.w  #$0000,BLTAFWM(a0)
        move.w  #$0000,BLTALWM(a0)
        move.l  #BLIT_BASE+$100,BLTAPTH(a0)
        move.l  #BLIT_BASE+$140,BLTBPTH(a0)
        move.l  #BLIT_BASE+$180,BLTCPTH(a0)
        move.l  #BLIT_BASE+$380,BLTDPTH(a0)
        move.w  #$0002,BLTAMOD(a0)
        move.w  #$0002,BLTBMOD(a0)
        move.w  #$0002,BLTCMOD(a0)
        move.w  #$0002,BLTDMOD(a0)
        move.w  #$0000,BLTADAT(a0)
        move.w  #$0000,BLTBDAT(a0)
        move.w  #$0000,BLTCDAT(a0)
        ifd     IS_ECS
        move.w  #$0003,BLTSIZV(a0)
        endif

        move.w  #CV,COPCON(a0)

        CPYT    l1tbl,l1end,COPLIST_L1

        move.l  #COPLIST_L1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        ifd     MUT_NOCOP
        move.w  #$8240,DMACON(a0)      ; precondition 1 contrast: no COPEN
        else
        ifd     MUT_NOBLT
        move.w  #$8280,DMACON(a0)      ; precondition 2 contrast: no BLTEN
        else
        move.w  #$82C0,DMACON(a0)      ; SET, DMAEN, COPEN, BLTEN
        endif
        endif
        move.w  DMACONR(a0),RESULTS+2

        ifd     MUT_CPUEARLY
        move.w  #$0082,BLTSIZE(a0)     ; P12 precondition 3 contrast
        endif
        ifd     MUT_CPUSIZH
        move.w  #$0001,BLTSIZH(a0)     ; P12/P13 structural contrast: CPU store to BLTSIZH
        endif

        move.w  #$0020,d1
        move.w  #1,MARK+0                      ; M1

        WVB
        ifd     MUT_CPUBLT80
.v80:   move.w  VHPOSR(a0),d0
        lsr.w   #8,d0
        cmp.w   #$0080,d0
        blo.s   .v80
        move.w  #$0082,BLTSIZE(a0)     ; P13 pair-2 contrast
        endif
        ifd     MUT_CPUWR
        move.w  #$1111,BLIT_BASE+$200  ; P13 precondition 3 contrast
        endif
        WVB
        WVB

        move.w  #2,MARK+2                      ; M2

        ifd     MUT_FIXTURE
        move.w  #$DEAD,RESULTS+0
        else
        move.w  #$BEEF,RESULTS+0
        endif
        FINISH

srca:   dc.w    $1234,$5678,$9ABC,$DEF0
srcb:   dc.w    $F0F0,$0FF0,$FF00,$00FF
srcc:   dc.w    $FF0F,$F0FF,$0FFF,$FFF0
srcend:

l1tbl:
        CM      BLTCON0,$0980
        CM      BLTCON1,$0000
        CM      BLTAFWM,$FFFF
        CM      BLTALWM,$FFFF
        CM      BLTAPTH,$0003
        CM      BLTAPTL,$0000
        CM      BLTDPTH,$0003
        CM      BLTDPTL,$0200
        CM      BLTAMOD,$0000
        CM      BLTDMOD,$0000
        CM      BLTBDAT,$FFFF
        CM      BLTCDAT,$FFFF
        ifd     MUT_S81
        CM      BLTSIZE,$0081
        else
        CM      BLTSIZE,$0082
        endif
        dc.w    $5001,$FFFE                    ; WAIT line $50
        CM      BLTCON0,$0780
        CM      BLTADAT,$FFFF
        CM      BLTBPTH,$0003
        CM      BLTBPTL,$0010
        CM      BLTCPTH,$0003
        CM      BLTCPTL,$0020
        CM      BLTBMOD,$0000
        CM      BLTCMOD,$0000
        CM      BLTDPTH,$0003
        CM      BLTDPTL,$0210
        CM      BLTSIZE,$0082
        ifd     IS_ECS
        dc.w    $6001,$FFFE                    ; WAIT line $60
        CM      BLTCON0L,$00CC
        CM      BLTBPTH,$0003
        CM      BLTBPTL,$0010
        CM      BLTCPTH,$0003
        CM      BLTCPTL,$0020
        CM      BLTDPTH,$0003
        CM      BLTDPTL,$0220
        CM      BLTSIZV,$0001
        ifd     MUT_SIZH3
        CM      BLTSIZH,$0003
        else
        CM      BLTSIZH,$0004
        endif
        endif
        dc.w    $FFFF,$FFFE
l1end:
