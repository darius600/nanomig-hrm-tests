; prog_p1a.s -- P1-A [p19-21,26,36,274-275]: a Copper MOVE writes the 16
; bits of its second instruction word, unchanged and bit n to bit n, into
; the register its first word addresses. Instrument A: INTENA, read back
; through INTENAR by the CPU. List L_A at $0E0000: 64 half-steps (a WAIT
; then a MOVE to INTENA, SPECIFICATION.md P1.2), then a park. The CPU
; writes every half-step word at runtime (a1)+ so they appear on the RAM
; port -- G4's own reference. The Copper auto-restarts from COP1LC (=L_A)
; every field (P6), so the list replays every field without an explicit
; loop; the monitor loop (I2) runs generously long and the C++ side counts
; every field of the capture (P1.3: "Every field of the capture counts").
;
; Symbols this source answers to:
;   P1A_CLEAR    -- run P1-A-clear instead of P1-A-set (a_i clears all,
;                    b_i clears P_i's bits; SPECIFICATION.md P1.2).
;   P1A_MUT_P1P1 -- P1-p1's own mutation: half-step a_5's WAIT replaced by
;                    a park (set run only).
;   P1A_MUT_P1P2 -- P1-p2's own mutation: the $D555 write is omitted (the
;                    instrument self-test).
;   P1A_C1       -- contrasting case C1: b_0's IR1 is $009C (INTREQ)
;                    instead of INTENA (set run only).
;   P1A_C2       -- contrasting case C2: b_0's IR2 is $8002 instead of
;                    $8001 (set run only).

        include "nanomig.i"
        include "ch2move.i"

L_A             equ     $0E0000
MARK_M0         equ     $0E7000
MARK_FINAL      equ     $0E7002
DMACONR_SAVE    equ     $0E7100
P1P2_V1         equ     $0E7102
P1P2_V2         equ     $0E7104
P1P2_V3         equ     $0E7106

; Log buffer sizing: see prog_p3.s's own comment (same $110000-$13FFFF
; area, same 32768-sample cap).
MON_ITERS       equ     30000
LOGBUF          equ     $110000

        org     PROGRAM_ORG

start:
        FRAME_INIT

        ; ---- P1-p2: the instrument works (before step 5) --------------
        move.w  #$7FFF,INTENA(a0)
        move.w  INTENAR(a0),d1
        move.w  d1,P1P2_V1.l
        ifnd    P1A_MUT_P1P2
        move.w  #$D555,INTENA(a0)
        endif
        move.w  INTENAR(a0),d1
        move.w  d1,P1P2_V2.l
        move.w  #$7FFF,INTENA(a0)
        move.w  INTENAR(a0),d1
        move.w  d1,P1P2_V3.l

        ; ---- step 3: write list L_A, one CPU write per word ------------
        lea     L_A,a1
        ifd     P1A_C1
        include "p1a_list_set_c1.i"
        else
        ifd     P1A_C2
        include "p1a_list_set_c2.i"
        else
        ifd     P1A_MUT_P1P1
        include "p1a_list_set_p1p1.i"
        else
        ifd     P1A_CLEAR
        include "p1a_list_clear.i"
        else
        include "p1a_list_set.i"
        endif
        endif
        endif
        endif

        move.w  #$FFFF,(a1)+            ; park
        move.w  #$FFFE,(a1)+

        ; ---- step 5: start the Copper (p.30's own sequence) -----------
        move.l  #L_A,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  VHPOSR(a0),MARK_M0.l     ; marker M0

        move.w  #$8280,DMACON(a0)       ; SET/CLR, DMAEN, COPEN
        move.w  DMACONR(a0),DMACONR_SAVE.l

        ; ---- step 6: monitor loop (I2) ---------------------------------
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
