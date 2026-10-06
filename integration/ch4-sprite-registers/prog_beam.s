; prog_beam.s -- run BEAM (SPECIFICATION.md P14).
;
; Wrapper sources set one symbol and include this file. The chip set (ECS or
; OCS) and the video standard (NTSC or PAL) are the test's configuration.
;
;   (no symbol)  -- the normal run: BPLCON0 = $0204 (COLOR, LACE), all DMA
;                   off, VPOSR into RESULT+0, 6 VERTB, M1, BEAMCON0 = $0020,
;                   8 VERTB, M2
;   MUT_B0       -- proposition contrast (ECS): BEAMCON0 = $0000
;   MUT_LACE1    -- proposition contrast (OCS): BPLCON0 = $0200 (LACE clear)
;                   is written at M1 as well as BEAMCON0
;   MUT_NOLACE   -- precondition 3 contrast: BPLCON0 = $0200 in step 1
;   MUT_SHORT    -- population contrast: 3 VERTB after M1 instead of 8
;   MUT_LOWREAD  -- anchor contrast: the VERTB wait loop reads Chip word $0100,
;                   a non-refresh access at a low address
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

        moveq   #5,d7
.pre:   bsr     wait_vertb
        dbra    d7,.pre

        move.w  #1,MARK+0                       ; M1

        ifd     MUT_B0
        move.w  #$0000,BEAMCON0(a0)             ; proposition contrast (ECS)
        else
        move.w  #$0020,BEAMCON0(a0)             ; step 5: bit 5, PAL
        endif
        ifd     MUT_LACE1
        move.w  #$0200,BPLCON0(a0)              ; proposition contrast (OCS)
        endif

        ifd     MUT_SHORT
        moveq   #2,d7
        else
        moveq   #7,d7
        endif
.post:  bsr     wait_vertb
        dbra    d7,.post

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
