; prog_p11.s -- P11: with the danger bit set, the Copper writes the
; blitter registers at $054, $056 and $058 (SPECIFICATION.md P11). The
; Copper list at LST is preloaded by the C++ harness (per-run: the real
; list, or one of the two contrasts) -- the same mechanism load_program()
; itself uses; no instruction here writes it. OCS/ECS is a harness Config
; choice, not a symbol here.
;
; Precondition mutations (each its own source, including this one):
;   prog_p11_g1.s      (G1_BREAK)  -- branch-to-self before END
;   prog_p11_g2.s      (G2_BREAK)  -- DMAEN (bit 9) left out of the enabling write
;   prog_p11_g3.s      (G3_BREAK)  -- one extra CPU write into L inside the window
;   prog_p11_g4.s      (DELAY_N=0) -- the delay collapses to nothing
;   prog_p11_g5.s      (G5_BREAK)  -- SPREN also set in the enabling write
;   prog_p11_g_nolist.s (G_NOLIST) -- COPEN omitted from the enabling write

        include "ch5.i"

        ifnd    DELAY_N
DELAY_N equ     50000
        endif

        org     PROGRAM_ORG

OM      equ     $0080
CM      equ     $0081
ENDM    equ     $00FF

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     MK,a2
        lea     RSTORE,a3

        move.w  #$07FF,DMACON(a0)
        move.w  #$0002,COPCON(a0)

        move.w  #$01FF,BLTCON0(a0)
        move.w  #0,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTDMOD(a0)
        move.l  #QQ,BLTDPTH(a0)          ; the CPU's own destination

        move.l  #LST,COP1LCH(a0)
        moveq   #0,d0
        move.w  d0,COPJMP1(a0)           ; strobe (p.30)

        move.w  #OM,(a2)
        ifd     G_NOLIST
        move.w  #$8240,DMACON(a0)        ; COPEN omitted (precondition mutation)
        else
        ifd     G2_BREAK
        move.w  #$80C0,DMACON(a0)        ; DMAEN (bit 9) left out (precondition mutation)
        else
        ifd     G5_BREAK
        move.w  #$82E0,DMACON(a0)        ; SPREN also set (precondition mutation)
        else
        move.w  #$82C0,DMACON(a0)        ; SET,DMAEN,COPEN,BLTEN
        endif
        endif
        endif
        move.w  DMACONR(a0),(a3)+
        ifd     G3_BREAK
        move.w  #$DEAD,($64030).l        ; G3's mutation: a CPU write into L, inside the window
        endif

        move.l  #DELAY_N,d0
        beq.s   .sk
.d:     subq.l  #1,d0
        bne.s   .d
.sk:    move.w  #CM,(a2)
        move.w  #$07FF,DMACON(a0)

        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        move.w  #ENDM,(a2)
        DONE
