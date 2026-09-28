; prog_p7.s -- P7 [p139,p145,p147,p164,p288]: the audio pointer restarts
; from AUD0LC whenever the length count finishes. No Copper. Buffer A
; ($0E4000) holds $1101,$1102,$1103,$1104 then four $0000 guard words;
; buffer B ($0E4100) holds $2201,$2202,$2203,$2204 then four $0000 guard
; words (SPECIFICATION.md 4.2, P7.2). The enable sequence follows p.145's
; own order (SPECIFICATION.md P7.2, steps 1-7); DMACONR is stored at
; $0E7108, the address the specification itself names. The CPU then idles
; for a fixed cycle count (long enough for several audio blocks) before
; DONE.
;
; Symbols this source answers to:
;   P7_NOAUD0     -- G1's own baseline: DMACON started $8200 (no AUD0EN)
;                     instead of $8201.
;   P7_NOAUD0_MUT -- G1's own mutation, layered on P7_NOAUD0: the CPU also
;                     reads $0E4000 once (a read inside an audio region).
;   P7_MUT_P7P2   -- P7-p2's own mutation: DMACON is written $8203
;                     (AUD0EN and AUD1EN), with AUD1LC also pointing at A.
;   P7_MUT_P7P3   -- P7-p3's own mutation: AUD0LC is set to B instead of A.
;   P7_MUT_P7P4   -- P7-p4's own mutation: AUD0LEN is 64 instead of 4.

        include "nanomig.i"
        include "ch2move.i"

BUF_A           equ     $0E4000
BUF_B           equ     $0E4100
MARK_M0         equ     $0E7000
DMACONR_SAVE    equ     $0E7108         ; specification's own address (P7.2, step 7)

; A busy loop, not tied to any port read, that occupies a fixed number of
; clk_sys cycles so several audio blocks elapse before DONE (P7-p4: about
; 17 four-word blocks per PAL field at AUD0PER=500).
IDLE_ITERS      equ     70000

        org     PROGRAM_ORG

start:
        FRAME_INIT

        ; ---- step 3: write the two buffers, one CPU write per word -----
        lea     BUF_A,a1
        move.w  #$1101,(a1)+
        move.w  #$1102,(a1)+
        move.w  #$1103,(a1)+
        move.w  #$1104,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        lea     BUF_B,a1
        move.w  #$2201,(a1)+
        move.w  #$2202,(a1)+
        move.w  #$2203,(a1)+
        move.w  #$2204,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        ; ---- P7.2 steps 1-4: location, length, volume, period ----------
        ifd     P7_MUT_P7P3
        move.l  #BUF_B,AUD0LCH(a0)       ; P7-p3's own mutation: AUD0LC = B
        else
        move.l  #BUF_A,AUD0LCH(a0)
        endif

        ifd     P7_MUT_P7P4
        move.w  #64,AUD0LEN(a0)          ; P7-p4's own mutation
        else
        move.w  #4,AUD0LEN(a0)
        endif
        move.w  #0,AUD0VOL(a0)
        move.w  #500,AUD0PER(a0)

        ifd     P7_MUT_P7P2
        move.l  #BUF_A,AUD1LCH(a0)       ; P7-p2's own mutation: AUD1LC also -> A
        endif

        ; ---- step 5: marker ----------------------------------------------
        move.w  VHPOSR(a0),MARK_M0.l

        ; ---- step 6: DMACON, step 7: DMACONR readback -------------------
        ifd     P7_NOAUD0
        move.w  #$8200,DMACON(a0)        ; SET/CLR only (no AUD0EN)
        else
        ifd     P7_MUT_P7P2
        move.w  #$8203,DMACON(a0)        ; SET/CLR, AUD0EN, AUD1EN
        else
        move.w  #$8201,DMACON(a0)        ; SET/CLR, AUD0EN
        endif
        endif
        move.w  DMACONR(a0),DMACONR_SAVE.l

        ifd     P7_NOAUD0_MUT
        move.w  BUF_A,d0                  ; G1's own mutation: one CPU read in an audio region
        endif

        ; ---- idle, then DONE ---------------------------------------------
        move.l  #IDLE_ITERS,d7
.idle:  subq.l  #1,d7
        bne.s   .idle

        DONE
