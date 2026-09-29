; prog_auto.s -- run AUTO (SPECIFICATION.md P6, P7); also AUTO-ODD (P8) via
; MUT_ODD.
;
; Wrapper sources set one mutation symbol and include this file:
;
;   (no symbol)  -- the normal run (AUTO)
;   MUT_P6PROP   -- P6 proposition contrast: S1 usage 2's CTL = $5E01
;   MUT_P6REF    -- P6 reference contrast: S4's CTL = $6700 (VSTOP $67),
;                   31 pairs instead of 32
;   MUT_P6PRE1   -- P6 precondition 1 contrast (also stands for P7's and
;                   P8's own precondition 1, which restate it unchanged):
;                   DMACON = $8380 (SPREN clear)
;   MUT_P6PRE2   -- P6 precondition 2 contrast: a WAIT for line $60
;                   inserted in the Copper list before the SPR0PT MOVEs
;   MUT_P7PROP   -- P7 proposition contrast: the Copper's SPR0PT MOVEs
;                   target S2_DATA instead of S1_DATA
;   MUT_FIXTURE  -- the fixture check's own contrast: $DEAD instead of $BEEF
;   MUT_ODD      -- AUTO-ODD (P8): the Copper's SPR0PTL MOVE carries $5001
;                   instead of $5000
;   MUT_ODDC     -- P8 proposition contrast (needs MUT_ODD's baseline
;                   otherwise unchanged): the MOVE carries $5002 instead

        include "nanomig.i"
        include "sprreg.i"

        ifd     MUT_P7PROP
SPR0PT_HI       equ     (S2_DATA>>16)
SPR0PT_LO       equ     (S2_DATA&$FFFF)
        else
        ifd     MUT_ODDC
SPR0PT_HI       equ     (S1_DATA>>16)
SPR0PT_LO       equ     $5002
        else
        ifd     MUT_ODD
SPR0PT_HI       equ     (S1_DATA>>16)
SPR0PT_LO       equ     $5001
        else
SPR0PT_HI       equ     (S1_DATA>>16)
SPR0PT_LO       equ     (S1_DATA&$FFFF)
        endif
        endif
        endif

        org     PROGRAM_ORG

start:
        CLEAR_OVL

        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)

        ; plain colour set (SPECIFICATION.md 3.6)
        lea     COLOR00(a0),a1
        moveq   #31,d1
.clrcol:
        move.w  #0,(a1)+
        dbra    d1,.clrcol
        move.w  #$F00,COLOR17(a0)
        move.w  #$0F0,COLOR18(a0)
        move.w  #$00F,COLOR19(a0)
        move.w  #$FFF,COLOR25(a0)
        move.w  #$FFF,COLOR26(a0)
        move.w  #$FFF,COLOR27(a0)

        ; standard display (SPECIFICATION.md 3.5)
        move.w  #$1200,BPLCON0(a0)
        move.w  #$0000,BPLCON1(a0)
        move.w  #$0024,BPLCON2(a0)
        move.w  #$0000,BPL1MOD(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$F4C1,DIWSTOP(a0)

        ; NULL_SPRITE: $0000,$0000 (the safe null sprite, p.111, p.114)
        move.l  #NULL_SPRITE,a1
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        ; S1 (SPECIFICATION.md P6 step 2)
        move.l  #S1_DATA,a1
        move.w  #$5060,(a1)+           ; usage 1 POS
        move.w  #$5401,(a1)+           ; usage 1 CTL
        moveq   #3,d0                  ; 4 pairs
.u1fill:
        move.w  #$FF00,(a1)+
        move.w  #$F0F0,(a1)+
        dbra    d0,.u1fill
        move.w  #$5861,(a1)+           ; usage 2 POS
        ifd     MUT_P6PROP
        move.w  #$5E01,(a1)+           ; P6 proposition contrast
        else
        move.w  #$5E00,(a1)+           ; usage 2 CTL
        endif
        moveq   #5,d0                  ; 6 pairs
.u2fill:
        move.w  #$F0F0,(a1)+
        move.w  #$FF00,(a1)+
        dbra    d0,.u2fill
        move.w  #$0000,(a1)+           ; end
        move.w  #$0000,(a1)+

        ; S2 (P7's own contrasting case: SPR0PT retargeted at S2_DATA; same
        ; structure as S1, at a different address, so MUT_P7PROP's "reads at
        ; $27000-$27032" has a real 26-word structure to read, not
        ; uninitialized memory). Filled unconditionally; nothing reads it
        ; unless the Copper list points a sprite at it.
        move.l  #S2_DATA,a1
        move.w  #$5060,(a1)+
        move.w  #$5401,(a1)+
        moveq   #3,d0
.s2u1fill:
        move.w  #$FF00,(a1)+
        move.w  #$F0F0,(a1)+
        dbra    d0,.s2u1fill
        move.w  #$5861,(a1)+
        move.w  #$5E00,(a1)+
        moveq   #5,d0
.s2u2fill:
        move.w  #$F0F0,(a1)+
        move.w  #$FF00,(a1)+
        dbra    d0,.s2u2fill
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        ; S4 (SPECIFICATION.md P6 step 3)
        move.l  #S4_DATA,a1
        move.w  #$4850,(a1)+           ; POS
        ifd     MUT_P6REF
        move.w  #$6700,(a1)+           ; P6 reference contrast: VSTOP $67
        moveq   #29,d0                 ; 30, plus the one below: 31 pairs
        else
        move.w  #$6800,(a1)+           ; VSTOP $68
        moveq   #30,d0                 ; 31, plus the one below: 32 pairs
        endif
.s4fill:
        move.w  #$FFFF,(a1)+
        move.w  #$0000,(a1)+
        dbra    d0,.s4fill
        move.w  #$FFFF,(a1)+           ; the last pair, common to both counts
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+           ; end
        move.w  #$0000,(a1)+

        ; Copper list L1 (SPECIFICATION.md P6 step 4). The CPU fills it
        ; itself, below $80000 (3.2's anchor check).
        move.l  #COPLIST_L1,a1
        move.w  #BPL1PTH,(a1)+
        move.w  #(ZERO_BUF>>16),(a1)+
        move.w  #BPL1PTL,(a1)+
        move.w  #(ZERO_BUF&$FFFF),(a1)+
        ifd     MUT_P6PRE2
        move.w  #$6001,(a1)+           ; WAIT line $60 (P6 precondition 2 contrast)
        move.w  #$FFFE,(a1)+
        endif
        move.w  #SPR0PTH,(a1)+
        move.w  #SPR0PT_HI,(a1)+
        move.w  #SPR0PTL,(a1)+
        move.w  #SPR0PT_LO,(a1)+
        move.w  #SPR1PTH,(a1)+
        move.w  #(NULL_SPRITE>>16),(a1)+
        move.w  #SPR1PTL,(a1)+
        move.w  #(NULL_SPRITE&$FFFF),(a1)+
        move.w  #SPR2PTH,(a1)+
        move.w  #(NULL_SPRITE>>16),(a1)+
        move.w  #SPR2PTL,(a1)+
        move.w  #(NULL_SPRITE&$FFFF),(a1)+
        move.w  #SPR3PTH,(a1)+
        move.w  #(NULL_SPRITE>>16),(a1)+
        move.w  #SPR3PTL,(a1)+
        move.w  #(NULL_SPRITE&$FFFF),(a1)+
        move.w  #SPR4PTH,(a1)+
        move.w  #(S4_DATA>>16),(a1)+
        move.w  #SPR4PTL,(a1)+
        move.w  #(S4_DATA&$FFFF),(a1)+
        move.w  #SPR5PTH,(a1)+
        move.w  #(NULL_SPRITE>>16),(a1)+
        move.w  #SPR5PTL,(a1)+
        move.w  #(NULL_SPRITE&$FFFF),(a1)+
        move.w  #SPR6PTH,(a1)+
        move.w  #(NULL_SPRITE>>16),(a1)+
        move.w  #SPR6PTL,(a1)+
        move.w  #(NULL_SPRITE&$FFFF),(a1)+
        move.w  #SPR7PTH,(a1)+
        move.w  #(NULL_SPRITE>>16),(a1)+
        move.w  #SPR7PTL,(a1)+
        move.w  #(NULL_SPRITE&$FFFF),(a1)+
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+

        move.l  #COPLIST_L1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        ifd     MUT_P6PRE1
        move.w  #$8380,DMACON(a0)      ; P6/P7/P8 precondition 1 contrast: SPREN clear
        else
        move.w  #$83A0,DMACON(a0)
        endif
        move.w  DMACONR(a0),RESULTS+2

        bsr     wait_vertb
        bsr     wait_vertb
        move.w  #1,MARK+0                      ; M1
        bsr     wait_vertb
        bsr     wait_vertb
        bsr     wait_vertb
        move.w  #2,MARK+2                      ; M2

        ifd     MUT_FIXTURE
        move.w  #$DEAD,RESULTS+0
        else
        move.w  #$BEEF,RESULTS+0
        endif
        DONE

wait_vertb:
        move.w  INTREQR(a0),d0
        and.w   #$0020,d0
        beq.s   wait_vertb
        move.w  #$0020,INTREQ(a0)
        rts
