; ep2_contrast.s -- EP2's own contrast (SPECIFICATION.md 1.2, EP2): Copper
; DMA started in the order p.30 prescribes (load COP1LC, strobe COPJMP1,
; then DMACON<-$8280 SET,DMAEN,COPEN) before the environment's own marker
; window closes. COP1LC points at a one-instruction list at $0C3000, a
; WAIT that never completes ($FFFF,$FFFE, "ends Copper list", p.23), so
; the Copper fetches it once and then stops requesting -- enough for the
; fetch itself to appear at the Chip-memory port between MK_START and DONE.
        include "nanomig.i"
        include "ciaregs.i"

COPLIST         equ     $0C3000

        org     PROGRAM_ORG
start:
        SETUP
        move.l  #$FFFFFFFE,COPLIST     ; WAIT $FFFF,$FFFE
        lea     CUSTOM,a0
        move.w  #(COPLIST)>>16,COP1LCH(a0)
        move.w  #(COPLIST)&$FFFF,COP1LCL(a0)
        move.w  #0,COPJMP1(a0)          ; strobe: reloads the Copper PC from COP1LC
        move.w  #$8280,DMACON(a0)       ; SET,DMAEN,COPEN
        move.w  #$0200,d7
.d1:    dbra    d7,.d1
        DONE
