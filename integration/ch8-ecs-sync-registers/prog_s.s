; prog_s.s -- Program S (SPECIFICATION.md sec.4, "Program S (used by P3 and
; P4)"). Never assembled directly: an includer sets BCON (BEAMCON0), SVAL
; (HSSTRT) and EVAL (HSSTOP) and includes this file. Optional symbols
; select a precondition or anchor mutation in place of the normal body:
;   MARK_OMIT    -- X-HALT-S: MARK1's write omitted. PC-MARK fails (the
;                   marker never appears), while the rest of the program,
;                   including DONE, still runs.
;   SHORT_PLANE  -- X-NOBPL*: PLANE holds only 32 lines of pattern instead
;                   of 256 (the fetch continues, unmodified, into the
;                   zero-filled memory beyond -- a live, reduced but
;                   nonempty white-carrying population).
;   A4_DOUBLE    -- X-A4DOUBLE: every line's data words become 6 black,
;                   6 white, 2 black, 6 white instead of 14 black and
;                   6 white, so the display carries two transitions into
;                   white per line instead of one, the first white span as
;                   wide as the normal one.
;   A4_WIDTH     -- X-A4WIDTH: every line's data words become 14 black,
;                   5 white, 1 black, so each line carries one transition
;                   into white and one out of white, with a white span one
;                   data word narrower than the normal one.
;   SHIFT_ROWS   -- X-SHIFT-*: in lines 0 to 127 of PLANE, the white span
;                   starts one data word earlier (13 black, 6 white,
;                   1 black), so the span keeps its width and moves
;                   relative to the sync pulse in half of the lines.
;   AUD0_AT0     -- X-A5REF: audio channel 0 DMA started on a one-word
;                   buffer at Chip address 0 (period 124, p.140/p.266).
; MARK1 is the word the test finds as the CPU write of $A5A5 at the
; Chip-memory port. ID holds the VPOSR value read before any BEAMCON0
; write (PC-ID). COPLIST and PLANE are filled by the CPU itself before the
; Copper is started, exactly as SPECIFICATION.md sec.4 describes: three
; Copper instructions (two MOVEs loading BPL1PTH/BPL1PTL from the CPU-
; filled COPLIST, then a WAIT that never completes) and 256 lines of 20
; words each. The program ends with DONE (nanomig.i); the display DMA
; keeps producing video after the CPU reaches it, since bitplane and
; Copper DMA do not depend on further CPU activity.
;
; HTOTAL and VTOTAL are written before BEAMCON0 because their own rows
; carry "(VARBEAMEN=1)" (p.280, p.298) and this program's BCON sets
; VARBEAMEN: the enabled comparators must not use an unprogrammed value
; (as Program H's own HTOTAL/VTOTAL writes do for the same reason).

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

        ; PLANE: 256 lines of 20 words each (32 under SHORT_PLANE; the
        ; buffer beyond the lines actually written is the harness's own
        ; zero-filled memory, so the fetch keeps running unmodified and the
        ; extra lines simply carry no white).
        lea     PLANE,a1
        ifd     SHORT_PLANE
        move.w  #31,d1                  ; 32 lines
        else
        move.w  #255,d1                 ; 256 lines
        endif
.line:
        ifd     A4_DOUBLE
        moveq   #5,d0
.d0:    move.w  #$0000,(a1)+
        dbra    d0,.d0
        moveq   #5,d0
.d1:    move.w  #$FFFF,(a1)+
        dbra    d0,.d1
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        moveq   #5,d0
.d2:    move.w  #$FFFF,(a1)+
        dbra    d0,.d2
        else
        ifd     A4_WIDTH
        moveq   #13,d0
.w0:    move.w  #$0000,(a1)+
        dbra    d0,.w0
        moveq   #4,d0
.w1:    move.w  #$FFFF,(a1)+
        dbra    d0,.w1
        move.w  #$0000,(a1)+
        else
        ifd     SHIFT_ROWS
        cmp.w   #128,d1
        bcs.s   .norm
        moveq   #12,d0
.s0:    move.w  #$0000,(a1)+
        dbra    d0,.s0
        moveq   #5,d0
.s1:    move.w  #$FFFF,(a1)+
        dbra    d0,.s1
        move.w  #$0000,(a1)+
        bra.s   .rowend
        endif
.norm:
        moveq   #13,d0
.blk0:  move.w  #$0000,(a1)+
        dbra    d0,.blk0
        moveq   #5,d0
.blk1:  move.w  #$FFFF,(a1)+
        dbra    d0,.blk1
        endif
        endif
.rowend:
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

        move.w  #$8380,DMACON(a0)      ; SET,DMAEN,BPLEN,COPEN (p.30,p.278)

        ifd     AUD0_AT0
        ; Audio channel 0 as p.145 prescribes (location, length in words,
        ; volume, period, DMA enable): a one-word buffer at Chip address 0,
        ; the smallest period p.266 gives (p.140: 124 NTSC, 123 PAL), the maximum volume
        ; (p.140: "from 64 to 0").
        move.l  #0,AUD0LCH(a0)
        move.w  #1,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #124,AUD0PER(a0)
        move.w  #$8201,DMACON(a0)      ; SET,DMAEN,AUD0EN
        endif

        move.w  #SVAL,HSSTRT(a0)
        move.w  #EVAL,HSSTOP(a0)
        move.w  #$00E2,HTOTAL(a0)       ; p.23: horizontal position's maximum value
        move.w  #$0138,VTOTAL(a0)       ; 312: standard PAL field size (p.216), matching Program H
        move.w  #BCON,BEAMCON0(a0)

        ifd     MARK_OMIT
        ; X-HALT-S: the marker write below is omitted; PC-MARK finds no
        ; MARK1 and fails, while everything after it (DONE) still runs.
        else
        move.w  #$A5A5,MARK1
        endif
        DONE
