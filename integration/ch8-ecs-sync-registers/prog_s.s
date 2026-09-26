; prog_s.s -- Program S (SPECIFICATION.md sec.4, "Program S (used by P3 and
; P4)"). Never assembled directly: an includer sets BCON (BEAMCON0), SVAL
; (HSSTRT) and EVAL (HSSTOP) and includes this file. Two optional symbols
; select a precondition mutation in place of the normal body:
;   HALT_BEFORE  -- X-HALT-S: BRA.S * inserted before the HSSTRT write.
;   NOBPL        -- X-NOBPL: the DMACON SET write omits BPLEN (bitplane
;                   DMA never starts).
; MARK1 is the word the test finds as the CPU write of $A5A5 at the
; Chip-memory port. ID holds the VPOSR value read before any BEAMCON0
; write (PC-ID). COPLIST and PLANE are filled by the CPU itself before the
; Copper is started, exactly as SPECIFICATION.md sec.4 describes: three
; Copper instructions (two MOVEs loading BPL1PTH/BPL1PTL from the CPU-
; filled COPLIST, then a WAIT that never completes) and 256 lines of 20
; words each (14 words $0000, 6 words $FFFF).

        include "nanomig.i"
        include "regs.i"

MARK1           equ     $0C0100
ID              equ     $0C0102
COPLIST         equ     $0C1000
PLANE           equ     $0C2000

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        move.w  #$7FFF,INTENA(a0)
        move.w  #$07FF,DMACON(a0)

        move.w  VPOSR(a0),d0
        move.w  d0,ID

        move.w  #$0000,COLOR00(a0)
        move.w  #$0FFF,COLOR01(a0)
        move.w  #$1000,BPLCON0(a0)     ; one bitplane, low resolution, no interlace
        move.w  #$0000,BPLCON1(a0)
        move.w  #$0000,BPLCON2(a0)
        move.w  #$0000,BPL1MOD(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)     ; DIWHIGH not written: the old DIWSTRT/DIWSTOP scheme holds (p.306)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)

        ; PLANE: 256 lines of 20 words each -- 14 words $0000, then 6 words $FFFF.
        lea     PLANE,a1
        move.w  #255,d1
.line:  moveq   #13,d0
.blk0:  move.w  #$0000,(a1)+
        dbra    d0,.blk0
        moveq   #5,d0
.blk1:  move.w  #$FFFF,(a1)+
        dbra    d0,.blk1
        dbra    d1,.line

        ; COPLIST: MOVE BPL1PTH,hi(PLANE); MOVE BPL1PTL,lo(PLANE); WAIT forever (p.21,p.30).
        lea     COPLIST,a1
        move.w  #BPL1PTH,(a1)+
        move.w  #(PLANE>>16)&$FFFF,(a1)+
        move.w  #BPL1PTL,(a1)+
        move.w  #PLANE&$FFFF,(a1)+
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+

        move.l  #COPLIST,COP1LCH(a0)   ; one MOVE.L (p.263)
        move.w  COPJMP1(a0),d0         ; the strobe: causes Copper to load PC from COP1LC (p.30)

        ifd     NOBPL
        move.w  #$8280,DMACON(a0)      ; SET,DMAEN,COPEN (BPLEN withheld)
        else
        move.w  #$8380,DMACON(a0)      ; SET,DMAEN,BPLEN,COPEN (p.30,p.278)
        endif

        ifd     HALT_BEFORE
        bra.s   *
        endif

        move.w  #SVAL,HSSTRT(a0)
        move.w  #EVAL,HSSTOP(a0)
        move.w  #BCON,BEAMCON0(a0)

        move.w  #$A5A5,MARK1
        bra.s   *
