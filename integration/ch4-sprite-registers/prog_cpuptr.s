; prog_cpuptr.s -- run CPU-PTR (SPECIFICATION.md P9).
;
; Wrapper sources set one mutation symbol and include this file:
;
;   (no symbol)  -- the normal run
;   MUT_P9PROP   -- proposition contrast: each field writes SPR0PTH alone
;                   (MOVE.W #$0002,SPR0PTH) instead of the long-word write,
;                   leaving SPR0PTL at its earlier decoy value $7000 (S2's
;                   own low word)
;   MUT_P9PRE1   -- precondition 1 contrast: after the VERTB, wait until
;                   V7-0>=$0060 before each field's pointer write
;   MUT_P9PRE2   -- precondition 2 contrast (as P6's precondition 1):
;                   DMACON = $8380 (SPREN clear)
;   MUT_P9PRE3   -- precondition 3 contrast: SPR4PT=NULL in the Copper list
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

        ; NULL_SPRITE
        move.l  #NULL_SPRITE,a1
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        ; S1 (P9 step 1, as AUTO)
        move.l  #S1_DATA,a1
        move.w  #$5060,(a1)+
        move.w  #$5401,(a1)+
        moveq   #3,d0
.u1fill:
        move.w  #$FF00,(a1)+
        move.w  #$F0F0,(a1)+
        dbra    d0,.u1fill
        move.w  #$5861,(a1)+
        move.w  #$5E00,(a1)+
        moveq   #5,d0
.u2fill:
        move.w  #$F0F0,(a1)+
        move.w  #$FF00,(a1)+
        dbra    d0,.u2fill
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        ; S2 (SPECIFICATION.md P9 step 1): S1 with usage 1's POS=$5070
        move.l  #S2_DATA,a1
        move.w  #$5070,(a1)+
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

        ; S4 (as AUTO's baseline, 32 pairs)
        move.l  #S4_DATA,a1
        move.w  #$4850,(a1)+
        move.w  #$6800,(a1)+
        moveq   #30,d0
.s4fill:
        move.w  #$FFFF,(a1)+
        move.w  #$0000,(a1)+
        dbra    d0,.s4fill
        move.w  #$FFFF,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        ; Copper list L1 (P9 step 1: no SPR0PT MOVEs)
        move.l  #COPLIST_L1,a1
        move.w  #BPL1PTH,(a1)+
        move.w  #(ZERO_BUF>>16),(a1)+
        move.w  #BPL1PTL,(a1)+
        move.w  #(ZERO_BUF&$FFFF),(a1)+
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
        ifd     MUT_P9PRE3
        move.w  #(NULL_SPRITE>>16),(a1)+       ; P9 precondition 3 contrast
        move.w  #SPR4PTL,(a1)+
        move.w  #(NULL_SPRITE&$FFFF),(a1)+
        else
        move.w  #(S4_DATA>>16),(a1)+
        move.w  #SPR4PTL,(a1)+
        move.w  #(S4_DATA&$FFFF),(a1)+
        endif
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

        ; step 1: SPR0PT = NULL once, before DMA starts (p.114)
        move.l  #NULL_SPRITE,SPR0PTH(a0)

        move.l  #COPLIST_L1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        ifd     MUT_P9PRE2
        move.w  #$8380,DMACON(a0)      ; P9 precondition 2 contrast (as P6's precondition 1)
        else
        move.w  #$83A0,DMACON(a0)
        endif
        move.w  DMACONR(a0),RESULTS+2

        lea     MARK,a2
        moveq   #3,d7                          ; 4 fields
.fields:
        bsr     wait_vertb
        ifd     MUT_P9PRE1
        move.w  #$0060,d1                      ; P9 precondition 1 contrast: write at line $60 of the field
        bsr     wait_v_ge
        endif

        move.w  #$7000,SPR0PTL(a0)
        ifd     MUT_P9PROP
        move.w  #$0002,SPR0PTH(a0)             ; P9 proposition contrast
        else
        move.l  #$00025000,SPR0PTH(a0)
        endif
        move.w  #1,(a2)+                       ; P_f
        dbra    d7,.fields

        bsr     wait_vertb
        move.w  #1,(a2)+                       ; P_5

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

wait_v_ge:
        move.w  VPOSR(a0),d0
        and.w   #1,d0
        lsl.w   #8,d0
        move.w  VHPOSR(a0),d2
        lsr.w   #8,d2
        or.w    d2,d0
        cmp.w   d1,d0
        blt.s   wait_v_ge
        rts
