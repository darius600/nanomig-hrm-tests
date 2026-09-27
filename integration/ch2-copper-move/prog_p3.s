; prog_p3.s -- P3 [p19]: a MOVE after a WAIT is performed once the WAIT is
; satisfied, and not before. List L_3 at $0E2000 (SPECIFICATION.md 5, P3.2):
;
;   $0E2000  MOVE INTENA,$7FFF   clear all
;   $0E2004  WAIT line $64      horizontal masked
;   $0E2008  MOVE INTENA,$8004  set bit 2
;   $0E200C  park
;
; The Copper auto-restarts from COP1LC (=L_3) every field (P6), so this
; list replays every field without any explicit loop. Frame steps 1-5
; (SPECIFICATION.md 4.3), then the monitor loop (I2) runs generously long
; (many fields' worth); the C++ side windows however many fields each
; check needs, so the same capture serves the main run and every
; capture-length precondition/mutation.
;
; Symbols this source answers to:
;   P3_NOCOPEN      -- G1's own baseline: DMACON started $8200 (no COPEN)
;                       instead of $8280.
;   P3_NOCOPEN_MUT  -- G1's own mutation, layered on P3_NOCOPEN: the CPU
;                       also executes one MOVE.W $0E2000,D0 (a read inside
;                       the Copper region), predicted to turn G1 red.
;   P3_MUT_WAIT0    -- P3-p1's own mutation: the WAIT is line 0 (satisfied
;                       at once).
;   P3_MUT_NOCLEAR  -- P3-p2's own mutation: the clearing MOVE is removed
;                       (the list starts with the WAIT).
;   P3_C1           -- contrasting case C1: the WAIT is line $A0 (160).
;   P3_C2           -- contrasting case C2: the WAIT is replaced by a park.
;   P3_MUT_G3       -- G3's own mutation: COP1LCH is loaded with the decoy
;                       park D ($0E8000) instead of L_3.
;   P3_MUT_G4       -- G4's own mutation: the set-MOVE's data word is
;                       written as $8006 instead of $8004 (an extra bit,
;                       INTENA bit 1, that no P3 check reads).

        include "nanomig.i"
        include "ch2move.i"

L_3             equ     $0E2000
MARK_M0         equ     $0E7000         ; strobe marker
MARK_FINAL      equ     $0E7002         ; final marker
DMACONR_SAVE    equ     $0E7100

; The log buffer ($110000-$13FFFF, 0x30000 bytes) holds at most 32768
; samples of 3 words (6 bytes) each; MON_ITERS stays comfortably under
; that so the pointer never runs past the reserved area.
MON_ITERS       equ     30000
LOGBUF          equ     $110000

        org     PROGRAM_ORG

start:
        FRAME_INIT

        ; ---- step 3: write list L_3 -----------------------------------
        lea     L_3,a1
        ifd     P3_MUT_NOCLEAR
        ; P3-p2's mutation: no clearing MOVE, list starts with the WAIT.
        else
        move.w  #INTENA,(a1)+
        move.w  #$7FFF,(a1)+            ; clear all
        endif

        ifd     P3_C2
        move.w  #$FFFF,(a1)+            ; park instead of WAIT
        move.w  #$FFFE,(a1)+
        else
        ifd     P3_MUT_WAIT0
        move.w  #$0001,(a1)+            ; WAIT line 0
        move.w  #$FF00,(a1)+
        else
        ifd     P3_C1
        move.w  #$A001,(a1)+            ; WAIT line $A0 (160)
        move.w  #$FF00,(a1)+
        else
        move.w  #$6401,(a1)+            ; WAIT line $64 (100), horizontal masked
        move.w  #$FF00,(a1)+
        endif
        endif
        endif

        move.w  #INTENA,(a1)+
        ifd     P3_MUT_G4
        move.w  #$8006,(a1)+            ; G4's mutation: diverges from $8004
        else
        move.w  #$8004,(a1)+            ; set bit 2
        endif

        move.w  #$FFFF,(a1)+            ; park
        move.w  #$FFFE,(a1)+

        ; ---- step 5: start the Copper (p.30's own sequence) -----------
        ifd     P3_MUT_G3
        move.l  #$0E8000,COP1LCH(a0)    ; G3's mutation: decoy park D instead of L_3
        else
        move.l  #L_3,COP1LCH(a0)
        endif
        move.w  #0,COPJMP1(a0)
        move.w  VHPOSR(a0),MARK_M0.l     ; marker M0

        ifd     P3_NOCOPEN
        move.w  #$8200,DMACON(a0)       ; SET/CLR, DMAEN only (no COPEN)
        else
        move.w  #$8280,DMACON(a0)       ; SET/CLR, DMAEN, COPEN
        endif
        move.w  DMACONR(a0),DMACONR_SAVE.l

        ifd     P3_NOCOPEN_MUT
        move.w  L_3,d0                   ; G1's own mutation: one CPU read in the Copper region
        endif

        ; ---- step 6: monitor loop (I2) ---------------------------------
        ; A plain subq.l/bne loop (not dbra: MON_ITERS exceeds dbra's
        ; 16-bit range).
        lea     LOGBUF,a1
        move.l  #MON_ITERS,d6
.mloop: move.w  INTENAR(a0),(a1)+
        move.w  VPOSR(a0),(a1)+
        move.w  VHPOSR(a0),(a1)+
        subq.l  #1,d6
        bne.s   .mloop

        ; ---- step 7: final marker, then DONE ---------------------------
        move.w  VHPOSR(a0),MARK_FINAL.l
        DONE
