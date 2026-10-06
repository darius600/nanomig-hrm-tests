; prog_lpen.s -- P8: samples of VPOSR:VHPOSR and HHPOSR across the end of line
; 255, taken while the test drives the light-pen input (SPECIFICATION.md P8).
;
; The test stores the run's parameters in Chip memory before the program
; starts:
;   LCFG+0  the BEAMCON0 value ($0020 LPENDIS = 0, $2020 LPENDIS = 1)
;   LCFG+2  the number of samples minus one
;   LCFG+4  the BPLCON0 value of step 4 ($0008 LPEN, $0000 without)
;
; Steps: DMACON, INTENA, INTREQ = $7FFF, BPLCON0 = 0, BEAMCON0, frame sync,
; poll VHPOSR until V = 254 and H >= $80, BPLCON0, marker LP_GO, then the
; samples, each the long word VPOSR:VHPOSR and the word HHPOSR, marker LP_END,
; BPLCON0 = 0.

        include "nanomig.i"
        include "s2.i"

LCFG    equ     $0B8000
SBUF    equ     $1B0000
M_LP_GO         equ     $F010
M_LP_END        equ     $F011

        ifnd    FIXVAL
FIXVAL  equ     $BEEF
        endc

        org     PROGRAM_ORG

start:
        move.w  #FIXVAL,RESULTS         ; the fixture: a known constant, stored first
        lea     CUSTOM,a0
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$0000,BPLCON0(a0)
        move.w  LCFG+0,d0
        move.w  d0,BEAMCON0(a0)
; frame sync
        move.w  #$0020,INTREQ(a0)
.p:     move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .p
        move.w  #$0020,INTREQ(a0)
        move.w  #M_FRAME,MARKER
; wait for V = 254, H >= $80
.w:     move.w  VHPOSR(a0),d0
        move.w  d0,d1
        lsr.w   #8,d1
        cmp.w   #254,d1
        bne.s   .w
        and.w   #$00FF,d0
        cmp.w   #$0080,d0
        bcs.s   .w
        move.w  LCFG+4,d0
        move.w  d0,BPLCON0(a0)
        move.w  #M_LP_GO,MARKER
        move.w  LCFG+2,d7
        ifd     REV
        lea     SBUF+$10000,a2          ; REV: each sample is stored below the previous one
.s:     sub.l   #6,a2
        move.l  VPOSR(a0),(a2)
        move.w  HHPOSR(a0),4(a2)
        else
        lea     SBUF,a2
.s:     move.l  VPOSR(a0),(a2)+
        move.w  HHPOSR(a0),(a2)+
        endc
        dbra    d7,.s
        move.w  #M_LP_END,MARKER
        move.w  #$0000,BPLCON0(a0)
        DONE
