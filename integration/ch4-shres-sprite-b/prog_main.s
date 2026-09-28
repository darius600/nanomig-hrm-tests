; prog_main.s -- ch4-shres-sprite-b: SuperHires sprite colour encoding.
; See SPECIFICATION.md section 3 for the stimulus this implements verbatim.
;
; Assembled standalone (no mutation symbol defined) this is the baseline
; stimulus. A mutant wrapper (prog_crN.s) equ's one MUT_CRx symbol to 1
; and includes this file unchanged otherwise; sprb.i resolves every
; mutation-selected constant and Copper-list group from that one symbol.

        include "nanomig.i"
        include "sprb.i"
        org     PROGRAM_ORG

start:
        ; ---- fixture: a program-stored constant, read back before anything
        ; else is trusted (RESULTS is ordinary chip RAM, unaffected by the
        ; overlay regardless of its state).
        move.w  #FIXTUREVAL,RESULTS

        ; ---- clear the overlay: COPLIST/SPR0DAT/SPR1DAT/BPLBUF all sit
        ; below logical $080000 (sprb.i), unreachable for CPU writes while
        ; the overlay is active.
        CLEAR_OVL

        lea     CUSTOM,a0

        ; ---- PC0: chip identification, read directly from the design and
        ; stored for the test to grade (Appendix B p.290 VPOSR/DENISEID;
        ; p.299 for the expected values).
        move.w  VPOSR(a0),RESULTS+2
        move.w  DENISEID(a0),RESULTS+4

        ; ---- step 1 (SPECIFICATION.md 3.4.1)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)

        ; ---- step 2: fill chip memory (3.2) and write the Copper list (3.5)

        ; BPLBUF: 32768 words of $CCCC
        move.l  #BPLBUF,a1
        move.w  #32767,d0
.fillbpl:
        move.w  #$CCCC,(a1)+
        dbra    d0,.fillbpl

        ; SPR0DAT / SPR1DAT headers (POS, CTL)
        move.l  #SPR0DAT,a1
        move.w  #$5060,(a1)+            ; SPR0POS
        move.w  #$8200,(a1)+            ; SPR0CTL
        move.l  #SPR1DAT,a1
        move.w  #$506C,(a1)+            ; SPR1POS
        move.w  #$8201,(a1)+            ; SPR1CTL

        ; SPR0DAT / SPR1DAT colour descriptors + terminator (SPRDESC, sprb.i)
        lea     SPRDESC,a2
        move.l  #SPR0DAT+4,a1
        move.w  #(SPRDESC_END-SPRDESC)/2-1,d0
.cpy0:  move.w  (a2)+,(a1)+
        dbra    d0,.cpy0

        lea     SPRDESC,a2
        move.l  #SPR1DAT+4,a1
        move.w  #(SPRDESC_END-SPRDESC)/2-1,d0
.cpy1:  move.w  (a2)+,(a1)+
        dbra    d0,.cpy1

        ; NULLSPR: $0000,$0000
        move.l  #NULLSPR,a1
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        ; COPLIST (COPTAB, sprb.i)
        lea     COPTAB,a2
        move.l  #COPLIST,a1
        move.w  #(COPTAB_END-COPTAB)/2-1,d0
.cpycop:
        move.w  (a2)+,(a1)+
        dbra    d0,.cpycop

        ; ---- step 3 (3.4.3)
        move.w  #BPLCON0VAL,BPLCON0(a0)

        ; ---- step 4 (3.4.4)
        move.w  #$0000,BPLCON1(a0)
        move.w  #$0024,BPLCON2(a0)

        ; ---- step 5 (3.4.5)
        move.w  #$0000,BPL1MOD(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$003C,DDFSTRT(a0)
        move.w  #$00D4,DDFSTOP(a0)

        ; ---- step 6 (3.4.6): COP1LC as one longword write (COP1LCH,COP1LCL
        ; are adjacent word registers), then a strobe read of COPJMP1.
        move.l  #COPLIST,COP1LCH(a0)
        move.w  COPJMP1(a0),d1

        ; ---- step 7 (3.4.7)
        move.w  #DMACONVAL,DMACON(a0)

        ; ---- step 8 (3.4.8): count FRAME_COUNT frame starts by polling
        ; VPOSR/VHPOSR, writing no custom register from here on. A frame
        ; start is V dropping below 16 after having been at or above 256.
        moveq   #0,d5                   ; "was at or above 256" flag
        move.w  #FRAME_COUNT,d7
.poll:
        move.w  VHPOSR(a0),d1
        move.w  VPOSR(a0),d0
        and.w   #1,d0
        lsl.w   #8,d0
        and.w   #$FF00,d1
        or.w    d1,d0                    ; d0 = V (V8:V7-V0), 0-511
        cmp.w   #256,d0
        bcs.s   .checklow
        moveq   #1,d5
        bra.s   .poll
.checklow:
        cmp.w   #16,d0
        bcc.s   .poll
        tst.w   d5
        beq.s   .poll
        moveq   #0,d5
        subq.w  #1,d7
        bne.s   .poll

        ; RUN contrast CR10: the program never writes MARKER. It still
        ; reaches nanomig.i's own DONE (an independent completion witness),
        ; so the run itself is demonstrably finished; only the MARKER-based
        ; RUN check is falsified.
        ifd     MUT_CR10
        DONE
        else
        move.w  #$A5A5,MARKER
.hang:  bra.s   .hang
        endif

        ; Data tables, placed here (after all code) so they land right
        ; after the program in the flat binary -- see sprb_data.i.
        include "sprb_data.i"
