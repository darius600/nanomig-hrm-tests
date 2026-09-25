; prog_pointer.s -- A1 [p170]: pointer register adjacency, one 32-bit
; write. Writes a known source word, sets BLTAPTH/BLTAPTL with a single
; 32-bit MOVE.L, runs a minimal A-to-D copy blit, stores the destination
; result. prog_pointer_mutant.s includes this with PTR_MUTANT set: the
; same single 32-bit write then points at a different, equally known
; source word.

        include "nanomig.i"

PTR_SRC_A       equ     $0C2000
PTR_SRC_B       equ     $0C2100
PTR_DST         equ     $0C2200

        org     PROGRAM_ORG

start:
        move.w  #$CAFE,PTR_SRC_A
        move.w  #$5150,PTR_SRC_B

        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; SET DMAEN,BLTEN
        move.w  #$09F0,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #PTR_DST,BLTDPTH(a0)
        ifd     PTR_MUTANT
        move.l  #PTR_SRC_B,BLTAPTH(a0) ; single 32-bit write -- mutated pointer
        else
        move.l  #PTR_SRC_A,BLTAPTH(a0) ; single 32-bit write -- both words at once
        endif
        move.w  #$0041,BLTSIZE(a0)     ; height=1,width=1 -- starts the blit

.wait:  move.w  DMACONR(a0),d0
        btst    #14,d0
        bne.s   .wait

        move.w  PTR_DST,RESULTS

        DONE
