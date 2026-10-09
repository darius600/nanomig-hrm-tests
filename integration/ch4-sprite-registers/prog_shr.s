; prog_shr.s -- run SHR-SWEEP / SHR-LORES / BITS-OCS (SPECIFICATION.md P1, P2, P3).
;
; Wrapper sources set symbols and include this file. The chip set (ECS or OCS)
; is the test's configuration.
;
;   (no symbol)  -- SHR-SWEEP: BPLCON0 = $1240 (SHRES), ten SWP usages
;   LORES        -- BPLCON0 = $1200 (SHR-LORES, and BITS-OCS with OCS5)
;   OCS5         -- the five SWP usages of BITS-OCS (P3) instead of ten
;   MUT_NOSH1    -- ten usages: SHSH1 cleared in every odd usage (P1 contrast)
;   MUT_SREF79   -- SREF with 79 scan lines (CTL $8F00, 79 pairs; precondition 2 contrast)
;   HIRES        -- OCS5 with BPLCON0 = $9200 (Hires), bit 4 clear in every usage
;                   (P3 precondition 2 contrast)
;   MUT_G0H5     -- usage 0 with VSTOP = VSTART + 5 (precondition 3 contrast)
;   MUT_U2CL     -- ten usages: usage 2 with CTL $5400 (P2 precondition 2)
;   MUT_U1SH0    -- OCS5: usage 1 with CTL $4C01 (P3 proposition contrast)
;   MUT_U4CL     -- OCS5: usage 4 with CTL $6400 (P3 precondition 3 contrast)
;   MUT_SWITCH   -- the Copper writes BPLCON0 = $1200 at the head of its list and
;                   $1240 after the ruler lines (P2 proposition contrast)
;   MUT_LOWREAD  -- anchor contrast: the VERTB wait loop reads Chip word $0100,
;                   a non-refresh access at a low address
;   MUT_FIXTURE  -- the fixture check's own contrast: $DEAD instead of $BEEF
;   MUT_NODONE   -- the program never writes DONE
;
; The program writes no custom register after it starts DMA other than
; INTREQ (SPECIFICATION.md P1 step 7).

        include "nanomig.i"
        include "sprreg.i"

CM      macro
        dc.w    \1,\2
        endm

; one SWP usage: POS, CTL, four pairs (low $FFFF, high $0000)
SU      macro
        dc.w    \1,\2
        dc.w    $FFFF,$0000,$FFFF,$0000,$FFFF,$0000,$FFFF,$0000
        endm

; usage 0: with MUT_G0H5 its VSTOP is VSTART + 5, so it carries a fifth pair
; and the usages after it stay aligned
SU0     macro
        dc.w    \1,\2
        dc.w    $FFFF,$0000,$FFFF,$0000,$FFFF,$0000,$FFFF,$0000
        ifd     MUT_G0H5
        dc.w    $FFFF,$0000
        endif
        endm

CPYT    macro
        lea     \1(pc),a2
        move.l  #\3,a1
        move.w  #((\2-\1)/2)-1,d0
.c\@:   move.w  (a2)+,(a1)+
        dbra    d0,.c\@
        endm

        org     PROGRAM_ORG

start:
        CLEAR_OVL

        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTREQ(a0)

        move.w  VPOSR(a0),RESULTS+0

        ; colours: the SuperHires-safe set (3.6)
        lea     coltbl(pc),a2
        lea     COLOR00(a0),a1
        moveq   #31,d0
.col:   move.w  (a2)+,(a1)+
        dbra    d0,.col

        ; display (3.5), except BPLCON0
        ifd     HIRES
        move.w  #$9200,BPLCON0(a0)
        else
        ifd     LORES
        move.w  #$1200,BPLCON0(a0)
        else
        move.w  #$1240,BPLCON0(a0)
        endif
        endif
        move.w  #$0000,BPLCON1(a0)
        move.w  #$0024,BPLCON2(a0)
        move.w  #$0000,BPL1MOD(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$F4C1,DIWSTOP(a0)

        ; RULER: words $FF00. Only the first lines are ever fetched from it
        ; (the Copper switches to ZERO at line $30), so the first 8 KB are filled.
        move.l  #RULER_BUF,a1
        move.w  #2047,d0
.ruler: move.l  #$FF00FF00,(a1)+
        dbra    d0,.ruler

        CPYT    nulltbl,nullend,NULL_SPRITE
        CPYT    sreftbl,srefend,SREF_DATA
        CPYT    swptbl,swpend,SWP_DATA
        CPYT    l1tbl,l1end,COPLIST_L1

        move.l  #COPLIST_L1,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)

        move.w  #$83A0,DMACON(a0)               ; SET, DMAEN, BPLEN, COPEN, SPREN

        bsr     wait_vertb
        bsr     wait_vertb
        bsr     wait_vertb
        move.w  #1,MARK+0                       ; M1
        bsr     wait_vertb
        bsr     wait_vertb
        bsr     wait_vertb
        move.w  #2,MARK+2                       ; M2

        ifd     MUT_FIXTURE
        move.w  #$DEAD,RESULTS+2
        else
        move.w  #$BEEF,RESULTS+2
        endif
        FINISH

wait_vertb:
        ifd     MUT_LOWREAD
        move.w  $0100,d3
        endif
        move.w  INTREQR(a0),d0
        and.w   #$0020,d0
        beq.s   wait_vertb
        move.w  #$0020,INTREQ(a0)
        rts

coltbl:
        dc.w    $000,$C00,$C00,$C00,$300,$F00,$F00,$F00
        dc.w    $300,$F00,$F00,$F00,$300,$F00,$F00,$F00
        dc.w    $000,$0C0,$0C0,$0C0,$030,$0F0,$0F0,$0F0
        dc.w    $030,$0F0,$0F0,$0F0,$030,$0F0,$0F0,$0F0

nulltbl: dc.w   $0000,$0000
nullend:

; SREF (sprite 0, reference): POS $4050, CTL $9000, 80 pairs, end
sreftbl:
        ifd     MUT_SREF79
        dc.w    $4050,$8F00
        rept    79
        dc.w    $FFFF,$0000
        endr
        else
        dc.w    $4050,$9000
        rept    80
        dc.w    $FFFF,$0000
        endr
        endif
        dc.w    $0000,$0000
srefend:

; SWP (sprite 2)
        ifd     MUT_G0H5
CTL0    equ     $4500
        else
CTL0    equ     $4400
        endif
        ifd     MUT_U1SH0
CTL1    equ     $4C01
        else
        ifd     MUT_NOSH1
CTL1    equ     $4C00
        else
        ifd     HIRES
CTL1    equ     $4C00
        else
CTL1    equ     $4C10
        endif
        endif
        endif
        ifd     HIRES
CTL3O   equ     $5C08
        else
CTL3O   equ     $5C18
        endif
        ifd     MUT_U2CL
CTL2    equ     $5400
        else
CTL2    equ     $5401
        endif

swptbl:
        ifd     OCS5
        SU0     $4064,CTL0
        SU      $4864,CTL1
        SU      $5064,$5408
        SU      $5864,CTL3O
        ifd     MUT_U4CL
        SU      $6064,$6400
        else
        SU      $6064,$6401
        endif
        else
        ifd     MUT_NOSH1
CTL3    equ     $5C01
CTL5    equ     $6C00
CTL7    equ     $7C01
CTL9    equ     $8C00
        else
CTL3    equ     $5C11
CTL5    equ     $6C10
CTL7    equ     $7C11
CTL9    equ     $8C10
        endif
        SU0     $4064,CTL0
        SU      $4864,CTL1
        SU      $5064,CTL2
        SU      $5864,CTL3
        SU      $6065,$6400
        SU      $6865,CTL5
        SU      $7065,$7401
        SU      $7865,CTL7
        SU      $8066,$8400
        SU      $8866,CTL9
        endif
        dc.w    $0000,$0000
swpend:

; Copper list L1 (step 5)
l1tbl:
        ifd     MUT_SWITCH
        CM      BPLCON0,$1200
        endif
        CM      BPL1PTH,$0004
        CM      BPL1PTL,$0000
        CM      SPR0PTH,$0002
        CM      SPR0PTL,$A000
        CM      SPR1PTH,$0002
        CM      SPR1PTL,$1000
        CM      SPR2PTH,$0002
        CM      SPR2PTL,$9000
        CM      SPR3PTH,$0002
        CM      SPR3PTL,$1000
        CM      SPR4PTH,$0002
        CM      SPR4PTL,$1000
        CM      SPR5PTH,$0002
        CM      SPR5PTL,$1000
        CM      SPR6PTH,$0002
        CM      SPR6PTL,$1000
        CM      SPR7PTH,$0002
        CM      SPR7PTL,$1000
        dc.w    $3001,$FFFE                     ; WAIT line $30
        CM      BPL1PTH,$0005
        CM      BPL1PTL,$0000
        ifd     MUT_SWITCH
        CM      BPLCON0,$1240
        endif
        dc.w    $FFFF,$FFFE
l1end:
