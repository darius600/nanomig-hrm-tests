; prog_blit.s -- run BLIT (SPECIFICATION.md P11).
;
; Wrapper sources set one mutation symbol and include this file:
;
;   (no symbol)  -- the normal run
;   MUT_P11PROP  -- proposition contrast: BLTCON0 = $08F0 (USED cleared)
;   MUT_P11PRE1  -- precondition 1/2 contrast: DMACON without BLTEN
;   MUT_P11PRE3  -- precondition 3 contrast: an extra CPU write to Chip
;                   memory inside the M1-M2 window, breaking the quiet window
;   MUT_FIXTURE  -- the fixture check's own contrast: $DEAD instead of $BEEF
;   MUT_NODONE   -- the program never writes DONE

        include "nanomig.i"
        include "sprreg.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL

        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)

        ; buffers (SPECIFICATION.md P11 step 1). The CPU fills them itself,
        ; below $80000 (3.2's anchor check).
        move.l  #BLIT_BASE,a1
        move.w  #$1234,(a1)+
        move.w  #$5678,(a1)+
        move.w  #$9ABC,(a1)+
        move.w  #$DEF0,(a1)+

        move.l  #BLIT_BASE+$200,a1
        moveq   #4,d0                  ; DST1 (4 words) + guard word
.dstfill:
        move.w  #$EEEE,(a1)+
        dbra    d0,.dstfill

        ifd     MUT_P11PRE1
        move.w  #$8200,DMACON(a0)      ; P11 precondition 1/2 contrast: no BLTEN
        else
        move.w  #$8240,DMACON(a0)      ; SET, DMAEN, BLTEN
        endif
        move.w  DMACONR(a0),RESULTS+2

        ifd     MUT_P11PROP
        move.w  #$08F0,BLTCON0(a0)     ; P11 proposition contrast: USED cleared
        else
        move.w  #$09F0,BLTCON0(a0)
        endif
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.l  #BLIT_BASE,BLTAPTH(a0)
        move.l  #BLIT_BASE+$200,BLTDPTH(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)

        move.w  #1,MARK+0                      ; M1

        ifd     MUT_P11PRE3
        ; P11 precondition 3 contrast: a write clearly outside DST1
        ; ($30200-$30206) and its guard word ($30208), so it cannot be
        ; mistaken for the blitter's own expected output.
        move.w  #$1111,BLIT_BASE+$300
        endif

        move.w  #$0044,BLTSIZE(a0)             ; starts the blit, written last

        ; wait 2 VERTB, inline (no bsr/rts: a subroutine call would write the
        ; return address to the stack, itself in Chip memory, breaking the
        ; quiet window this precondition checks -- SPECIFICATION.md 3.3
        ; permits reading and writing custom registers here, not Chip memory).
        move.w  #$0020,d1
.wv1:   move.w  INTREQR(a0),d0
        and.w   d1,d0
        beq.s   .wv1
        move.w  #$0020,INTREQ(a0)
.wv2:   move.w  INTREQR(a0),d0
        and.w   d1,d0
        beq.s   .wv2
        move.w  #$0020,INTREQ(a0)

        move.w  #2,MARK+2                      ; M2

        ifd     MUT_FIXTURE
        move.w  #$DEAD,RESULTS+0
        else
        move.w  #$BEEF,RESULTS+0
        endif
        FINISH

wait_vertb:
        move.w  INTREQR(a0),d0
        and.w   #$0020,d0
        beq.s   wait_vertb
        move.w  #$0020,INTREQ(a0)
        rts
