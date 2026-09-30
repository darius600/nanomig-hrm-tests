; prog_playfield_core.s -- P1/P2: the CPU-written half of the basic
; playfield set-up (pp.63-64). The Copper list and the bitplane data are
; NOT written by this program -- the overlay blocks a CPU write below
; $80000 (see nanomig_harness.h's address-space notes) -- the test preloads
; them at COP_LIST_ADDR / BPL_PLANE*_ADDR with poke_word/poke_long before
; boot(). This program only points COP1LC at the preloaded list and starts
; it, exactly as p.62 describes ("make certain the bitplane pointers are
; set and bitplane DMA is turned on").
;
; Flags:
;   NTSC_MODE    DIWSTOP carries the NTSC value; otherwise PAL
;   HIRES_MODE   DDFSTRT/DDFSTOP carry the high-resolution values (config C);
;                otherwise low-resolution (configs A, B, E)
;   PALETTE16    COLOR00..COLOR15 = the 16-colour palette (config C);
;                otherwise only COLOR00=C0, COLOR01=C1 (configs A, B, E)
;   MUT_MOD2     P2-b contrast: BPL1MOD = $0002 instead of $0000
;
; This program never varies BPLCON0, the bitplane pointers or run "0"/"1":
; those live entirely in the preloaded Copper list, which is what P1/P2
; grade.

        include "nanomig.i"
        include "playfield.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)

        ifd     PALETTE16
        ifd     MUT_COLOR00
        move.w  #$0358,COLOR00(a0)
        else
        move.w  #$0359,COLOR00(a0)
        endc
        ifd     MUT_COLOR01
        move.w  #$0EA3,COLOR01(a0)
        else
        move.w  #$0EA2,COLOR01(a0)
        endc
        move.w  #$0F3C,COLOR02(a0)
        move.w  #$04E7,COLOR03(a0)
        move.w  #$0A0F,COLOR04(a0)
        move.w  #$00F5,COLOR05(a0)
        move.w  #$0FF0,COLOR06(a0)
        move.w  #$0888,COLOR07(a0)
        move.w  #$0C63,COLOR08(a0)
        move.w  #$036C,COLOR09(a0)
        move.w  #$09E1,COLOR10(a0)
        move.w  #$0E19,COLOR11(a0)
        move.w  #$05A5,COLOR12(a0)
        move.w  #$0D7B,COLOR13(a0)
        move.w  #$01BD,COLOR14(a0)
        move.w  #$0B42,COLOR15(a0)
        else
        ifd     MUT_COLOR00
        move.w  #$0358,COLOR00(a0)
        else
        move.w  #$0359,COLOR00(a0)
        endc
        ifd     MUT_COLOR01
        move.w  #$0EA3,COLOR01(a0)
        else
        move.w  #$0EA2,COLOR01(a0)
        endc
        endc

        move.w  #$0000,BPLCON1(a0)
        move.w  #$0000,BPLCON2(a0)
        move.w  #$2C81,DIWSTRT(a0)
        ifd     NTSC_MODE
        move.w  #$F4C1,DIWSTOP(a0)
        else
        move.w  #$2CC1,DIWSTOP(a0)
        endc

        ifd     HIRES_MODE
        move.w  #$003C,DDFSTRT(a0)
        move.w  #$00D4,DDFSTOP(a0)
        else
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        endc

        ifd     MUT_MOD2
        move.w  #2,BPL1MOD(a0)
        else
        move.w  #0,BPL1MOD(a0)
        endc
        move.w  #0,BPL2MOD(a0)

        move.l  #COP_LIST_ADDR,d0
        move.w  d0,COP1LCL(a0)
        swap    d0
        move.w  d0,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)         ; strobe: load Copper PC from COP1LC
        ifd     MUT_NOBPLEN
        move.w  #$8280,DMACON(a0)      ; P1-c contrast: BPLEN not set
        else
        move.w  #$8380,DMACON(a0)      ; SET/CLR, DMAEN, BPLEN, COPEN (p.278)
        endc

.idle:  bra.s   .idle
