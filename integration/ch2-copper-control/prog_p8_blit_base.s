; prog_p8_blit_base.s -- P8 BLIT case: the 680x0 can poll the
; blitter-finished flag (SPECIFICATION.md P8). Included by the case files
; below, each setting one symbol first (default = main).
;
;   (no symbol)    -- main
;   P8_BLIT_NOSIZE -- device contrast: omit the BLTSIZE write. Bit 6 stays 0
;   P8_BLIT_MUT_A  -- precondition (a) mutation: INTENA = $8040 (bit 6 set,
;                     master enable left clear)
;   P8_BLIT_MUT_B  -- precondition (b) mutation: a whole blit runs and
;                     finishes before MC
;   P8_BLIT_MUT_III -- check (iii) contrast: once a poll record reads bit 6
;                     = 1, INTREQ = $0040 is written again (once)
;   P8_BLIT_MUT_D  -- population mutation: the program waits for bit 6 = 1
;                     before the first poll record, so no record is stored
;                     before the blit has finished

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL_DDR
        lea     CUSTOM,a0
        CLEAR_DMA_INT
        ifd     P8_BLIT_MUT_A
        move.w  #$8040,INTENA(a0)        ; precondition (a) mutation: bit 6
                                          ; set, master enable (bit 14) left
                                          ; clear: INTENAR is non-zero
        endif
        move.w  INTENAR(a0),RESULTS+4    ; precondition (a): INTENAR bits 14-0 = 0

        ; step 1: DMACON = $8240 (SET/CLR, DMAEN, BLTEN); INTREQ = $0040; MC.
        move.w  #$8240,DMACON(a0)
        move.w  #$0040,INTREQ(a0)
        ifd     P8_BLIT_MUT_B
        ; precondition (b) mutation: a whole blit runs and finishes before
        ; MC, so bit 6 already reads 1 at the first record stored after MC.
        move.w  #$01FF,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #DATA1,BLTDPTH(a0)
        move.w  #$1020,BLTSIZE(a0)
        WAIT_INTREQ_SET 6
        endif
        move.w  #7,MARKBUF+6              ; MC

        ; step 2: D-only blit into DATA1 (64 lines x 32 words).
        move.w  #$01FF,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #DATA1,BLTDPTH(a0)
        ifnd    P8_BLIT_NOSIZE
        move.w  #$1020,BLTSIZE(a0)        ; last
        endif

        ifd     P8_BLIT_MUT_D
        WAIT_INTREQ_SET 6                 ; population mutation: no record yet
        endif

        ; step 3: poll for one complete frame.
        lea     RECBUF,a6
        moveq   #0,d7
        ifd     P8_BLIT_MUT_III
        moveq   #0,d6                    ; 0 until the re-clear below has fired
        move.w  #3000-1,d3
.pd:    SAMPLE
        addq.l  #1,d7
        move.w  -8(a6),d0                ; the record's own INTREQR word
        btst    #6,d0
        beq.s   .pd_next
        tst.w   d6
        bne.s   .pd_next
        move.w  #$0040,INTREQ(a0)        ; check (iii) contrast: clear again
        moveq   #1,d6
.pd_next:
        dbra    d3,.pd
        else
        move.w  #3000-1,d3
.pd:    SAMPLE
        addq.l  #1,d7
        dbra    d3,.pd
        endif

        move.l  d7,RESULTS+8
        move.w  INTREQR(a0),RESULTS+12    ; precondition (c): bit 6 set after the frame

        DONE
