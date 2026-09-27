; prog_audiorate.s -- a proven-live comparator demonstration for G1/G2/G3
; (SPECIFICATION.md section 3, P6). Audio DMA's own rate does track
; AUD0PER on this RTL (unlike disk write DMA's rate, which G1/G2/G3
; measure as not tracking FAST); this program plays two buffers back to
; back at two legal periods, P and 2P, so the C++ test can run G1/G2/G3's
; own comparator formula on a genuinely two-times-related pair of rates.
;
; Buffers BUF_P and BUF_2P are poked by the C++ test before boot() (the
; same direct-preload mechanism prog_audio.s's and prog_diskrate.s's
; buffers use); their content is not graded, only the timing of the
; reads that fetch them.

        include "nanomig.i"
        include "ch5regs.i"

        org     PROGRAM_ORG

BUF_P   equ     $005000         ; 256 words, below $080000 (2.1)
BUF_2P  equ     $005200         ; 256 words

WAITVBL macro
                move.l  #200000,d7
.wv\@           btst    #5,INTREQR(a0)
                bne.s   .wvd\@
                dbra    d7,.wv\@
.wvd\@          move.w  #$0020,INTREQ(a0)
                endm

start:
        CLEAR_OVL
        lea     CUSTOM,a0

        ; ---- preamble (2.3) ----
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,ADKCON(a0)
        move.w  #0,AUD1VOL(a0)
        move.w  #0,AUD2VOL(a0)
        move.w  #0,AUD3VOL(a0)

        ; ==== run P: AUD0PER=124 (p.140/p.266's own legal minimum) ====
        move.l  #BUF_P,AUD0LCH(a0)
        move.w  #256,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #124,AUD0PER(a0)
        move.w  #$8201,DMACON(a0)      ; SET, DMAEN, AUD0EN (p.144)
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        move.w  #$0001,DMACON(a0)

        ; ==== run 2P: AUD0PER=248 ====
        move.l  #BUF_2P,AUD0LCH(a0)
        move.w  #256,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #248,AUD0PER(a0)
        move.w  #$8201,DMACON(a0)
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        move.w  #$0001,DMACON(a0)

        DONE
