; prog_pot.s -- the pen-pressed-button program (P5). Sets POTGO from a
; memory word (so its declared value is visible on the Chip-memory port,
; C2-style). It first runs a short connector-identity loop (for
; precondition C0) while the harness flips a pin on one connector, marks
; the phase boundary in chip RAM once that loop ends, then loops reading
; VPOSR:VHPOSR (for the sample's own line) and POTGOR into a small ring
; buffer for as long as the harness drives its pin-5/pin-9 schedule. The
; harness synchronises on the phase marker rather than assuming how long
; the connector-identity loop takes.
;
; NO_DONE (define before including): the sampling loop's done mark is
;   skipped -- the program hangs instead of storing it. Used for
;   precondition C1's contrasting case.

        include "nanomig.i"
        include "appDE_regs.i"
        org     PROGRAM_ORG

S2_ITERS        equ 3000        ; connector-identity loop iterations
N_POT           equ 10000       ; sampling loop iterations

start:
        move.w  #$2700,sr               ; supervisor, all interrupt levels masked
        lea     CUSTOM,a6

        move.w  #$5A5A,MARK0            ; fixture: known constant, stored first

        move.w  #$7FFF,INTENA(a6)
        move.w  #$7FFF,DMACON(a6)
        move.w  #$0000,BPLCON0R(a6)     ; LPEN=0: VPOSR/VHPOSR read the moving beam

; ---- connector identity (C0): ring-sample JOY0DAT/JOY1DAT while the
; harness flips a pin on one connector. No density requirement applies to
; C0, so plain indexed addressing (a small ring, wrapped with an AND mask)
; is fine here.
        lea     RING_S2,a5
        moveq   #0,d6
        move.l  #S2_ITERS,d5
.s2loop:
        move.w  JOY0DAT(a6),0(a5,d6.w)
        move.w  JOY1DAT(a6),2(a5,d6.w)
        addq.w  #4,d6
        andi.w  #(RING_S2_BYTES-4),d6
        subq.l  #1,d5
        bne.s   .s2loop

        move.w  #$B5B5,MARK_PHASE       ; boundary: connector-identity loop done

; ---- POTGO <- cfg word, from memory (C2-style: the port shows the CPU
; reading the declared value) ---------------------------------------------
        move.w  CFG_POTGO,POTGO(a6)

; ---- sampling loop: VPOSR:VHPOSR (the sample's own line) and POTGOR into
; a small ring, for as long as the harness drives its schedule -----------
        lea     RING_POT,a4
        moveq   #0,d6
        move.l  #N_POT,d5
.potloop:
        move.l  VPOSR(a6),d0
        move.w  POTGOR(a6),d1
        move.l  d0,0(a4,d6.w)
        move.w  d1,4(a4,d6.w)
        addq.w  #RING_POT_STRIDE,d6
        cmpi.w  #(RING_POT_STRIDE*RING_POT_COUNT),d6
        blt.s   .potok
        moveq   #0,d6
.potok:
        subq.l  #1,d5
        bne.s   .potloop

        ifd     NO_DONE
.hang   bra.s   .hang
        else
        DONE
        endif
