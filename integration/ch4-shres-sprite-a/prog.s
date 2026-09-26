; prog.s -- ch4-shres-sprite-a: the register writes of SPECIFICATION.md
; section 3.3 and the pointer writes of section 3.2. The sprite, bitplane
; and Copper-list *data* is poked directly into chip RAM by the C++ test
; (test_ch4_shres_sprite_a.cpp) at the fixed addresses ch4.i defines, the
; same technique the ch6 packages use for their own DMA data areas; this
; source only ever performs CPU register writes.
;
; Wrapper sources set one mutation symbol and include this file, each
; producing its own assembled program (SPECIFICATION.md 3.8, 4):
;
;   (no symbol) -- the normal run
;   MUT_C1      -- P1's precondition contrast: BPLCON0 = $9000 (HIRES,
;                  SHRES=0) instead of $1040
;   MUT_C2      -- PC2's contrast: SPR1PTH points at NULL_DATA instead of
;                  SPR1_DATA (the C++ side points the Copper list's own
;                  SPR1 MOVE the same way when NMH_MUTATE=C2)
;   MUT_C3      -- PC3's contrast: SPR2PTH points at SPR0_DATA instead of
;                  NULL_DATA (Copper list likewise, when NMH_MUTATE=C3)
;   MUT_C6      -- PC6's contrast: DONE immediately after step 7, no
;                  four-frame wait, so there is no graded frame

        include "nanomig.i"
        include "ch4.i"

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0

        ; step 1 (p.278): clear every interrupt and DMA enable
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)

        ; step 3: wait for vertical blank (VPOSR bit 0 = V8, VHPOSR
        ; bits 15-8 = V7-V0, p.285)
        bsr     wait_v_ge_100
        bsr     wait_v_lt_020

        ; step 4: BPLCON0 and the other display registers
        ifd     MUT_C1
        move.w  #$9000,BPLCON0(a0)     ; C1: HIRES, one bitplane, SHRES=0
        else
        move.w  #$1040,BPLCON0(a0)     ; BPU=001, SHRES=1, HIRES=0
        endif
        move.w  #$0000,BPLCON1(a0)
        move.w  #$0024,BPLCON2(a0)     ; PF2P=PF1P=100
        move.w  #$2C81,DIWSTRT(a0)     ; PAL values (p.59)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$003C,DDFSTRT(a0)     ; normal hi-res values (p.60)
        move.w  #$00D4,DDFSTOP(a0)
        move.w  #$0000,BPL1MOD(a0)

        ; step 5: pointers, each a long write (p.25, p.110)
        move.l  #BPL_BUF,BPL1PTH(a0)
        move.l  #SPR0_DATA,SPR0PTH(a0)
        ifd     MUT_C2
        move.l  #NULL_DATA,SPR1PTH(a0) ; PC2 contrast: sprite 1 -> null
        else
        move.l  #SPR1_DATA,SPR1PTH(a0)
        endif
        ifd     MUT_C3
        move.l  #SPR0_DATA,SPR2PTH(a0) ; PC3 contrast: sprite 2 -> sprite 0's data
        else
        move.l  #NULL_DATA,SPR2PTH(a0)
        endif
        move.l  #NULL_DATA,SPR3PTH(a0)
        move.l  #NULL_DATA,SPR4PTH(a0)
        move.l  #NULL_DATA,SPR5PTH(a0)
        move.l  #NULL_DATA,SPR6PTH(a0)
        move.l  #NULL_DATA,SPR7PTH(a0)

        ; step 6 (p.30): COP1LC then its strobe, before Copper DMA is on
        move.l  #COP_LIST,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        ; step 7 (p.278): DMAEN, BPLEN, COPEN, SPREN
        move.w  #$83A0,DMACON(a0)

        ifd     MUT_C6
        bra.s   finish                 ; PC6 contrast: no frame wait at all
        endif

        ; step 8: four frames, each a wait for V>=$100 then V<$020
        moveq   #3,d7
.frames:
        bsr     wait_v_ge_100
        bsr     wait_v_lt_020
        dbra    d7,.frames

finish:
        ; step 9: DENISEID, recorded and not graded
        lea     RESULTS,a1
        move.w  DENISEID(a0),(a1)+

        ; fixture: a known, program-stored constant, asserted before
        ; anything else is read (not an HRM claim; proves the fixture can
        ; deliver a value at all)
        move.w  #$BEEF,(a1)+

        ; step 10
        DONE

; ---- subroutines (a0 = CUSTOM throughout; d0/d1 scratch) ------------------

wait_v_ge_100:
        move.w  VPOSR(a0),d0
        and.w   #1,d0
        lsl.w   #8,d0
        move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        or.w    d1,d0
        cmp.w   #$100,d0
        blt.s   wait_v_ge_100
        rts

wait_v_lt_020:
        move.w  VPOSR(a0),d0
        and.w   #1,d0
        lsl.w   #8,d0
        move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        or.w    d1,d0
        cmp.w   #$020,d0
        bge.s   wait_v_lt_020
        rts
