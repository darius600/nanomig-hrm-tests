; prog_beam.s -- run BEAM (SPECIFICATION.md P14).
;
; Wrapper sources set symbols and include this file. The chip set (ECS or
; OCS) and the video standard (NTSC or PAL) are the test's configuration.
;
;   (no symbol)  -- the ECS run: BPLCON0 = $0204 (COLOR, LACE), all DMA
;                   off, VPOSR into RESULT+0, 6 VERTB, M1, BEAMCON0 = $0020,
;                   8 VERTB, M2
;   SWEEP        -- the OCS run: the same up to M1; after M1 sixteen writes to
;                   BEAMCON0, each followed by one VERTB: each of bits 0-14
;                   alone ($0001, $0002, ... $4000, the bits p.305 defines),
;                   then $7FFF; M2
;   MUT_B0       -- proposition contrast (ECS): BEAMCON0 = $0000
;   MUT_LACE1    -- proposition contrast (OCS): BPLCON0 = $0200 (LACE clear)
;                   is written at M1 as well
;   MUT_NOLACE   -- precondition 3 contrast: BPLCON0 = $0200 in step 1
;   MUT_SHORT    -- population contrast: 3 VERTB (3 writes with SWEEP) after M1
;                   instead of 8 (16)
;   MUT_LOWREAD  -- anchor contrast: the VERTB wait loop reads Chip word $0100,
;                   a non-refresh access at a low address
;   MUT_COLOR    -- stationarity contrast: COLOR00 = $0F00 is written after the
;                   third of the six VERTB before M1
;   MUT_FIXTURE  -- the fixture check's own contrast: $DEAD instead of $BEEF
;   MUT_NODONE   -- the program never writes DONE

        include "nanomig.i"
        include "sprreg.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL

        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,DMACON(a0)               ; step 2: all DMA off
        move.w  #$7FFF,INTREQ(a0)

        ifd     MUT_NOLACE
        move.w  #$0200,BPLCON0(a0)              ; precondition 3 contrast: LACE clear
        else
        move.w  #$0204,BPLCON0(a0)              ; step 1: COLOR, LACE
        endif

        move.w  VPOSR(a0),RESULTS+0             ; step 3

        moveq   #2,d7
.pre:   bsr     wait_vertb
        dbra    d7,.pre
        ifd     MUT_COLOR
        move.w  #$0F00,COLOR00(a0)              ; stationarity contrast: the background changes
        endif
        moveq   #2,d7
.pre2:  bsr     wait_vertb
        dbra    d7,.pre2

        move.w  #1,MARK+0                       ; M1

        ifd     MUT_LACE1
        move.w  #$0200,BPLCON0(a0)              ; proposition contrast (OCS)
        endif

        ifd     SWEEP
        lea     sweeptbl(pc),a3
        ifd     MUT_SHORT
        moveq   #2,d7
        else
        moveq   #15,d7
        endif
.sweep: move.w  (a3)+,BEAMCON0(a0)
        bsr     wait_vertb
        dbra    d7,.sweep
        else
        ifd     MUT_B0
        move.w  #$0000,BEAMCON0(a0)             ; proposition contrast (ECS)
        else
        move.w  #$0020,BEAMCON0(a0)             ; step 5: bit 5, PAL
        endif
        ifd     MUT_SHORT
        moveq   #2,d7
        else
        moveq   #7,d7
        endif
.post:  bsr     wait_vertb
        dbra    d7,.post
        endif

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

sweeptbl:
        dc.w    $0001,$0002,$0004,$0008,$0010,$0020,$0040,$0080
        dc.w    $0100,$0200,$0400,$0800,$1000,$2000,$4000,$7FFF
