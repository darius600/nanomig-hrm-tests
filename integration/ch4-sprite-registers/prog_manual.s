; prog_manual.s -- run MANUAL (SPECIFICATION.md P4, P5).
;
; Wrapper sources set one mutation symbol and include this file, each
; producing its own assembled program (SPECIFICATION.md's "apparatus
; control" technique, integration/apparatus-vasm-program/prog_control.s):
;
;   (no symbol) -- the normal run
;   MUT_P4READ  -- P4's read-half contrast: read a Chip-memory mirror word,
;                  written just before the read, instead of the register
;   MUT_P4WRITE -- P4's write-half contrast: field B's SPR0POS write carries
;                  $0062 instead of $0061 (p.123: d = 72 instead of 68)
;   MUT_P4NOPOS -- not a contrast: field B without any SPR0POS write, run once
;                  for the MEASURED line (SPR0POS keeps $0060 from field A)
;   MUT_P4PRE1  -- P4 precondition 1 contrast: omit the DECOY reads
;   MUT_P4PRE2  -- P4 precondition 2 contrast: read at V7-0=$0080 (inside
;                  the display window) instead of $00F8
;   MUT_P5PROP  -- P5's proposition contrast: field B SPR0CTL = $0001
;   MUT_P5REF   -- P5's reference contrast: omit the SPR4DATA write
;   MUT_P5PRE1  -- P5 precondition 1 contrast: DMACON = $83A0 (SPREN set)
;   MUT_NULLPTR -- all eight sprite pointers at the null sprite before DMACON
;                  (with MUT_P5PRE1: SPREN set and the sprite channel reads
;                  no refresh address)
;   MUT_P5PRE2  -- P5 precondition 2 contrast: delay field B's writes
;                  until V7-0>=$0080
;   MUT_FIXTURE -- the fixture check's own contrast: $DEAD stored instead
;                  of $BEEF
;   MUT_LOWREAD -- anchor contrast: the VERTB wait loop reads Chip word $0100,
;                  a non-refresh access at a low address
;   MUT_NODONE  -- the program never writes DONE
;
; RESULTS+12/14/16/18 record the V (V8:V7-0) read at the moment of each
; marker (MA/RA/MB/RB): the test's own precondition checks (SPECIFICATION.md
; P4 precondition 2, P5 precondition 2) read these back instead of assuming
; timing, using the same VPOSR/VHPOSR technique SPECIFICATION.md 3.8
; prescribes throughout.

        include "nanomig.i"
        include "sprreg.i"

MEMTRACK        equ     RESULTS+$5000   ; MUT_P4READ only: +0 mirrors SPR0POS, +2 mirrors SPR0CTL
VMARK_MA        equ     RESULTS+12
VMARK_RA        equ     RESULTS+14
VMARK_MB        equ     RESULTS+16
VMARK_RB        equ     RESULTS+18

        org     PROGRAM_ORG

start:
        CLEAR_OVL                       ; SPECIFICATION.md 3.2, before any write below $80000

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

        ; Copper list L1 (SPECIFICATION.md P5 step 1): BPL1PT=ZERO, end.
        ; The CPU fills it itself, below $80000 (3.2's anchor check).
        move.l  #COPLIST_L1,a1
        move.w  #BPL1PTH,(a1)+
        move.w  #(ZERO_BUF>>16),(a1)+
        move.w  #BPL1PTL,(a1)+
        move.w  #(ZERO_BUF&$FFFF),(a1)+
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+

        move.l  #COPLIST_L1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        ifd     MUT_NULLPTR
        lea     SPR0PTH(a0),a1          ; every sprite pointer at the null sprite
        moveq   #7,d1
.np:    move.l  #NULL_SPRITE,(a1)+
        dbra    d1,.np
        endif

        ifd     MUT_P5PRE1
        move.w  #$83A0,DMACON(a0)       ; P5 precondition 1 contrast: SPREN set
        else
        move.w  #$8380,DMACON(a0)       ; SET, DMAEN, BPLEN, COPEN (p.278)
        endif

        move.w  DMACONR(a0),RESULTS+2

        bsr     wait_vertb

        ; reference, sprite 4 (P5 step 4)
        move.w  #$0050,SPR4POS(a0)
        move.w  #$0000,SPR4CTL(a0)
        move.w  #$0000,SPR4DATB(a0)
        ifd     MUT_P5REF
        ; P5 reference contrast: omit the SPR4DATA write -- sprite 4 stays disarmed
        else
        move.w  #$FFFF,SPR4DATA(a0)
        endif

        ; field A, sprite 0
        move.w  #$0060,SPR0POS(a0)
        ifd     MUT_P4READ
        move.w  #$0060,MEMTRACK+0
        endif
        move.w  #$0001,SPR0CTL(a0)
        ifd     MUT_P4READ
        move.w  #$0001,MEMTRACK+2
        endif
        move.w  #$F0F0,SPR0DATB(a0)
        move.w  #$FF00,SPR0DATA(a0)
        bsr     store_vmark
        move.w  d0,VMARK_MA
        move.w  #1,MARK+0                      ; MA

        ifd     MUT_P4PRE2
        move.w  #$0080,d1                      ; P4 precondition 2 contrast: inside the display window
        else
        move.w  #$00F8,d1
        endif
        bsr     wait_v_ge

        ifd     MUT_P4PRE1
        ; P4 precondition 1 contrast: omit the DECOY read
        else
        move.w  DECOY,d0
        endif
        ifd     MUT_P4READ
        move.w  MEMTRACK+0,d0
        else
        move.w  SPR0POS(a0),d0
        endif
        move.w  d0,RESULTS+4

        ifd     MUT_P4PRE1
        else
        move.w  DECOY,d0
        endif
        ifd     MUT_P4READ
        move.w  MEMTRACK+2,d0
        else
        move.w  SPR0CTL(a0),d0
        endif
        move.w  d0,RESULTS+6

        bsr     store_vmark
        move.w  d0,VMARK_RA
        move.w  #2,MARK+2                      ; RA

        bsr     wait_vertb

        ; field B, sprite 0
        ifd     MUT_P5PRE2
        move.w  #$0080,d1                      ; P5 precondition 2 contrast: delay past VERTB
        bsr     wait_v_ge
        endif

        ifd     MUT_P4NOPOS
        ; not a contrast: omit field B's SPR0POS write
        else
        ifd     MUT_P4WRITE
        move.w  #$0062,SPR0POS(a0)             ; P4 write-half contrast
        else
        move.w  #$0061,SPR0POS(a0)
        endif
        ifd     MUT_P4READ
        move.w  #$0061,MEMTRACK+0
        endif
        endif

        ifd     MUT_P5PROP
        move.w  #$0001,SPR0CTL(a0)             ; P5 proposition contrast
        ifd     MUT_P4READ
        move.w  #$0001,MEMTRACK+2
        endif
        else
        move.w  #$0000,SPR0CTL(a0)
        ifd     MUT_P4READ
        move.w  #$0000,MEMTRACK+2
        endif
        endif
        move.w  #$FF00,SPR0DATB(a0)
        move.w  #$F0F0,SPR0DATA(a0)

        bsr     store_vmark
        move.w  d0,VMARK_MB
        move.w  #3,MARK+4                      ; MB

        ifd     MUT_P4PRE2
        move.w  #$0080,d1
        else
        move.w  #$00F8,d1
        endif
        bsr     wait_v_ge

        ifd     MUT_P4PRE1
        else
        move.w  DECOY,d0
        endif
        ifd     MUT_P4READ
        move.w  MEMTRACK+0,d0
        else
        move.w  SPR0POS(a0),d0
        endif
        move.w  d0,RESULTS+8

        ifd     MUT_P4PRE1
        else
        move.w  DECOY,d0
        endif
        ifd     MUT_P4READ
        move.w  MEMTRACK+2,d0
        else
        move.w  SPR0CTL(a0),d0
        endif
        move.w  d0,RESULTS+10

        bsr     store_vmark
        move.w  d0,VMARK_RB
        move.w  #4,MARK+6                      ; RB

        ifd     MUT_FIXTURE
        move.w  #$DEAD,RESULTS+0
        else
        move.w  #$BEEF,RESULTS+0
        endif
        FINISH

; ---- subroutines (a0 = CUSTOM throughout) ---------------------------------

wait_vertb:
        ifd     MUT_LOWREAD
        move.w  $0100,d3
        endif
        move.w  INTREQR(a0),d0
        and.w   #$0020,d0
        beq.s   wait_vertb
        move.w  #$0020,INTREQ(a0)
        rts

; wait until (V8<<8 | V7-0) >= d1; d0/d2 scratch, d1 preserved.
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

; returns the current (V8<<8 | V7-0) in d0; d2 scratch.
store_vmark:
        move.w  VPOSR(a0),d0
        and.w   #1,d0
        lsl.w   #8,d0
        move.w  VHPOSR(a0),d2
        lsr.w   #8,d2
        or.w    d2,d0
        rts
