; prog_bars.s -- generic S1+S2+S3 bar/stripe stimulus (SPECIFICATION.md
; sections 3-4), driven entirely by a configuration block the harness
; preloads into chip RAM with poke_word/poke_long before boot(), the same
; preload technique ch3-playfield-system uses for its Copper list and
; bitplane data. What varies between propositions (BPLCON0, BPLCON1 and
; whether it is written at all, BPLCON4, FMODE, DDFSTRT/DDFSTOP, DMACON,
; and the two line-buffer contents) is a runtime value, not a recompiled
; program; what the CPU executes -- the S1/S2 register writes, the S3
; per-line Copper pointer reload it builds, enabling DMA -- is fixed and
; identical for every run. The CPU still performs a genuine bus write for
; every register in section 4: MOVE.W reads a config word from chip RAM
; and writes it to the custom register, exactly as MOVE.W #imm would.
;
; BPLCON0/BPLCON2/BPLCON3/BPLCON4/BPL1MOD/BPL2MOD/BPL1PTH/BPL1PTL/BPL2PTH/
; BPL2PTL/DIWSTRT/DIWSTOP/DDFSTRT/DDFSTOP/BEAMCON0/COP1LCH/COP1LCL/COPJMP1/
; FMODE are not in nanomig.i (only the subset used elsewhere in this suite
; is) and are defined locally here from the AA specification's own
; register list (AA p.6-p.10, Appendix-B style).

        include "nanomig.i"

BEAMCON0        equ     $1DC
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPLCON0         equ     $100
BPLCON1         equ     $102
BPLCON2         equ     $104
BPLCON3         equ     $106
BPL1MOD         equ     $108
BPL2MOD         equ     $10A
BPLCON4         equ     $10C
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
BPL2PTH         equ     $0E4
BPL2PTL         equ     $0E6
FMODE           equ     $1FC
COLOR01         equ     $182
COLOR02         equ     $184
COLOR03         equ     $186

; Configuration block: the harness's BarsConfig (test_aa_integ_s1.cpp)
; poke_word()s these fields before boot(). Addresses are chosen inside
; $080000-$1FFFFF, clear of PROGRAM_ORG, PROGRAM_STACK, RESULTS/DONE_ADDR
; and the buffers below (SPECIFICATION.md section 0, Addresses).
CFG             equ     $0B0000
CFG_BPLCON0     equ     CFG+0
CFG_BPLCON1     equ     CFG+2
CFG_BPLCON1_WR  equ     CFG+4   ; nonzero: CPU writes CFG_BPLCON1 to BPLCON1
CFG_BPLCON4     equ     CFG+6
CFG_FMODE       equ     CFG+8
CFG_DDFSTRT     equ     CFG+10
CFG_DDFSTOP     equ     CFG+12
CFG_DMACON      equ     CFG+14
CFG_BEAMCON0    equ     CFG+22  ; BEAMCON0 value (PAL $0020)
CFG_WAITLO      equ     CFG+24  ; low word of every Copper WAIT of the S3 reload ($0007: horizontal position $06)

BUF1            equ     $0B1000 ; plane-1 line buffer, 192 bytes, preloaded
BUF2            equ     $0B1100 ; plane-2 line buffer, 192 bytes, preloaded
COPLIST         equ     $0B2000 ; generated per-line Copper list

        org     PROGRAM_ORG

start:
        ; The fixture: a known constant, stored before anything else reads
        ; one (SPECIFICATION.md, "What a finished package looks like").
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS
        endc

        lea     CUSTOM,a0

        ; S1 step 1.
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  CFG_BEAMCON0,d0        ; PAL (S1 step 1)
        move.w  d0,BEAMCON0(a0)

        ; S2 baseline, with the proposition's overrides read from CFG.
        move.w  CFG_BPLCON0,d0
        move.w  d0,BPLCON0(a0)
        move.w  #$0024,BPLCON2(a0)
        move.w  #$0C00,BPLCON3(a0)
        move.w  CFG_BPLCON4,d0
        move.w  d0,BPLCON4(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$F0C1,DIWSTOP(a0)
        move.w  CFG_DDFSTRT,d0
        move.w  d0,DDFSTRT(a0)
        move.w  CFG_DDFSTOP,d0
        move.w  d0,DDFSTOP(a0)
        move.w  #$0000,BPL1MOD(a0)
        move.w  #$0000,BPL2MOD(a0)
        move.w  CFG_FMODE,d0
        move.w  d0,FMODE(a0)
        move.w  #$0222,COLOR00(a0)
        move.w  #$0F00,COLOR01(a0)
        move.w  #$00F0,COLOR02(a0)
        move.w  #$000F,COLOR03(a0)

        ; BPLCON1 is written by the CPU before DMA is enabled (S2), except
        ; where CFG_BPLCON1_WR is 0 (P4's reset-state runs).
        tst.w   CFG_BPLCON1_WR
        beq.s   .nobplcon1
        move.w  CFG_BPLCON1,d0
        move.w  d0,BPLCON1(a0)
.nobplcon1:

        ; S3 "pointer reload each line": build, for every V from $2C to
        ; $EF, WAIT(V,$06) then MOVE BPL1PTH/L, MOVE BPL2PTH/L to the start
        ; of each line buffer -- generated here rather than written out as
        ; 196 literal WAIT/MOVE pairs.
        lea     COPLIST,a1
        move.w  #$2C,d2
.genloop:
        move.w  d2,d0
        lsl.w   #8,d0
        or.w    CFG_WAITLO,d0
        move.w  d0,(a1)+                       ; WAIT IR1: line V, early in the line (R3's own proven low byte)
        move.w  #$FFFE,(a1)+                   ; WAIT IR2
        move.w  #BPL1PTH,(a1)+
        move.w  #(BUF1>>16)&$FFFF,(a1)+
        move.w  #BPL1PTL,(a1)+
        move.w  #BUF1&$FFFF,(a1)+
        move.w  #BPL2PTH,(a1)+
        move.w  #(BUF2>>16)&$FFFF,(a1)+
        move.w  #BPL2PTL,(a1)+
        move.w  #BUF2&$FFFF,(a1)+
        addq.w  #1,d2
        cmp.w   #$F0,d2
        bne.s   .genloop
        move.w  #$FFFF,(a1)+                   ; end-of-list wait
        move.w  #$FFFE,(a1)+

        ; Copper: load COP1LC and strobe it before Copper DMA is enabled
        ; (S1 step 3, AA p.21).
        move.w  #(COPLIST>>16)&$FFFF,COP1LCH(a0)
        move.w  #COPLIST&$FFFF,COP1LCL(a0)
        move.w  #0,COPJMP1(a0)

        move.w  CFG_DMACON,d0
        move.w  d0,DMACON(a0)

.idle:  bra.s   .idle
