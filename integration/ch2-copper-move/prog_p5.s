; prog_p5.s -- P5 [p25,p273,p276,p287,p290,p309]: one long-word write to
; COPxLCH loads the whole list address; the lower-addressed register holds
; the high bits. No Copper list, no monitor loop: the CPU paces itself by
; polling VPOSR/VHPOSR (SPECIFICATION.md P5.2). At each of the 40 targets
; (S_L1, S_L0, S_H1, S_H0, P1-B's own sets and order) there is a park
; followed by six $0000 words (16 bytes), so a read of the word after a
; park (G4) is a word this specification prescribes. Phase 1 (COP1): for
; each target, MOVE.L #T,$DFF080, marker, word write to $DFF088 (COPJMP1).
; Phase 2 (COP2): first COP1LC = D (the decoy park), then for each target,
; MOVE.L #T,$DFF084, marker, word write to $DFF08A (COPJMP2). Phase 3:
; MOVE.W #$000E,$DFF080, MOVE.W #$9010,$DFF082, marker, word write to
; $DFF088. Every step starts once the line after the previous step's own
; line has begun; the run starts once V8=0 and V=$28.
;
; Symbols this source answers to:
;   P5_MUT_P5P1 -- P5-p1's own mutation: before the tenth phase-2 step the
;                   CPU waits for line 5 of the next field.
;   P5_MUT_P5P2 -- P5-p2's own mutation: the park at $0C0020 is replaced by
;                   a WAIT (line 0) followed by a park at $0C0024.
;   P5_MUT_P5P3 -- P5-p3's own mutation: the ROM-overlay-clearing writes
;                   are omitted.
;   P5_C1       -- contrasting case C1: phase 3's two words are exchanged
;                   (COP1LCL first, COP1LCH second), with a park at
;                   $10000E instead of $0E9010.
;   P5_C2       -- contrasting case C2: one extra step directly after the
;                   phase-1 step to $0C8000 -- MOVE.W #$4000,$DFF082 only,
;                   then a marker and COPJMP1.
;   P5_C3       -- contrasting case C3: the first phase-2 step strobes
;                   COPJMP1 instead of COPJMP2.
;   P5_NOCOPEN  -- G1's own baseline: DMACON started $8200 (no COPEN)
;                   instead of $8280.

        include "nanomig.i"
        include "ch2move.i"

MARK_M0         equ     $0E7000         ; the program frame's own marker (4.3, step 5)
DMACONR_SAVE    equ     $0E7100
MARK_LOG        equ     $0E7002         ; 81 (82 in C2) step markers, then the final marker (I3)
DECOY_D         equ     $0E8000
PARK_C3         equ     $10000E         ; C3's own park (unused address in this run)
PARK_C1         equ     $10000E         ; C1's own park (same address; the two cases are separate runs)

        org     PROGRAM_ORG

start:
        FRAME_INIT

        ; ---- step 3: clear the ROM overlay before any target below
        ; $080000 is written (SPECIFICATION.md 4.3; P5-p3) -----------------
        ifnd    P5_MUT_P5P3
        move.b  #$03,$BFE201.l          ; CIA-A DDRA: bit 0 (OVL) output
        move.b  #$00,$BFE001.l          ; CIA-A PRA: OVL = 0, overlay off
        endif

        ; ---- step 3: write the park at each of the 40 targets, plus C1's
        ; and C3's own decoy parks -------------------------------------
        lea     TARGETS,a2
        move.w  #39,d7
.padloop:
        move.l  (a2)+,d0
        movea.l d0,a1
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        dbra    d7,.padloop

        ifd     P5_MUT_P5P2
        ; P5-p2's own mutation: $0C0020 becomes a WAIT (line 0, satisfied
        ; at once, p.22) followed by a park at $0C0024, instead of a park.
        move.w  #$0001,$0C0020.l
        move.w  #$FF00,$0C0022.l
        move.w  #$FFFF,$0C0024.l
        move.w  #$FFFE,$0C0026.l
        endif

        move.w  #$FFFF,DECOY_D.l
        move.w  #$FFFE,DECOY_D+2.l

        ; Phase 3's own landing park (SPECIFICATION.md 4.2:
        ; "$0E9010-$0E9013 P5 word-write target park").
        move.w  #$FFFF,$0E9010.l
        move.w  #$FFFE,$0E9012.l

        ifd     P5_C1
        move.w  #$FFFF,PARK_C1.l
        move.w  #$FFFE,PARK_C1+2.l
        endif
        ifd     P5_C3
        move.w  #$FFFF,PARK_C3.l
        move.w  #$FFFE,PARK_C3+2.l
        endif

        ; ---- step 5 (4.3): the program frame starts the Copper with
        ; L = D (p.30's own sequence) ---------------------------------
        move.l  #DECOY_D,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  VHPOSR(a0),MARK_M0.l     ; marker M0
        ifd     P5_NOCOPEN
        move.w  #$8200,DMACON(a0)        ; SET/CLR, DMAEN only (no COPEN)
        else
        move.w  #$8280,DMACON(a0)        ; SET/CLR, DMAEN, COPEN
        endif
        move.w  DMACONR(a0),DMACONR_SAVE.l

        ; ---- wait for V8=0 and V=$28 (the run's own start line) --------
.waitstart:
        move.w  VPOSR(a0),d0
        and.w   #1,d0
        bne.s   .waitstart
        move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   #$28,d1
        bne.s   .waitstart

        move.w  #$28,d3                 ; d3 = the current (just-reached) line
        lea     MARK_LOG,a3
        lea     TARGETS,a2

        ; ---- Phase 1 (COP1): one step per target ------------------------
        ifd     P5_C2
        move.w  #10,d7                  ; first 11 targets (through $0C8000)
        bsr     phase1_steps
        ; C2's own extra step: L alone (COP1LCH keeps its own value)
        addq.w  #1,d3
.waitc2: move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   d3,d1
        bne.s   .waitc2
        move.w  #$4000,COP1LCL(a0)
        move.w  VHPOSR(a0),(a3)+
        move.w  #0,COPJMP1(a0)
        move.w  #28,d7                  ; remaining 29 targets (11+29=40)
        bsr     phase1_steps
        else
        move.w  #39,d7
        bsr     phase1_steps
        endif

        ; ---- Phase 2 (COP2): COP1LC = D first, then one step per target -
        move.l  #DECOY_D,COP1LCH(a0)
        lea     TARGETS,a2               ; phase 1 consumed the whole table; start over

        ifd     P5_C3
        ; C3's own mutation: the first phase-2 step strobes COPJMP1.
        addq.w  #1,d3
.waitc3: move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   d3,d1
        bne.s   .waitc3
        move.l  (a2)+,COP2LCH(a0)
        move.w  VHPOSR(a0),(a3)+
        move.w  #0,COPJMP1(a0)          ; C3: COPJMP1, not COPJMP2
        move.w  #38,d7                  ; remaining 39 targets
        bsr     phase2_steps
        else
        ifd     P5_MUT_P5P1
        move.w  #8,d7                   ; first 9 targets
        bsr     phase2_steps
        ; P5-p1's own mutation: wait for line 5 of the NEXT field (forces
        ; a full-field wrap, since by now the beam is well past line 5).
        move.w  #5,d3
.waitp1: move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   d3,d1
        bne.s   .waitp1
        move.l  (a2)+,COP2LCH(a0)
        move.w  VHPOSR(a0),(a3)+
        move.w  #0,COPJMP2(a0)
        move.w  #29,d7                  ; remaining 30 targets
        bsr     phase2_steps
        else
        move.w  #39,d7
        bsr     phase2_steps
        endif
        endif

        ; ---- Phase 3: separate word writes -------------------------------
        addq.w  #1,d3
.wait3: move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   d3,d1
        bne.s   .wait3
        ifd     P5_C1
        move.w  #$9010,COP1LCH(a0)      ; C1: the two words exchanged
        move.w  #$000E,COP1LCL(a0)
        else
        move.w  #$000E,COP1LCH(a0)
        move.w  #$9010,COP1LCL(a0)
        endif
        move.w  VHPOSR(a0),(a3)+
        move.w  #0,COPJMP1(a0)

        ; ---- once the line after phase 3's step has begun, final marker -
        addq.w  #1,d3
.waitfin: move.w VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   d3,d1
        bne.s   .waitfin
        move.w  VHPOSR(a0),(a3)+

        DONE

; Processes (d7+1) targets from (a2), pacing each on the line after the
; previous one (d3), via COP1LCH/COPJMP1. Shared by phase 1 and C2's own
; split. Advances a2, a3 and d3; d7 is consumed by dbra.
phase1_steps:
.loop:  addq.w  #1,d3
.wait:  move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   d3,d1
        bne.s   .wait
        move.l  (a2)+,COP1LCH(a0)
        move.w  VHPOSR(a0),(a3)+
        move.w  #0,COPJMP1(a0)
        dbra    d7,.loop
        rts

; As phase1_steps, via COP2LCH/COPJMP2.
phase2_steps:
.loop:  addq.w  #1,d3
.wait:  move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   d3,d1
        bne.s   .wait
        move.l  (a2)+,COP2LCH(a0)
        move.w  VHPOSR(a0),(a3)+
        move.w  #0,COPJMP2(a0)
        dbra    d7,.loop
        rts

; The 40 targets, in P1-B's own order: S_L1 (bits 5-15, then bits 1-4),
; S_L0 (bits 5-15, then bits 1-4), S_H1, S_H0 (SPECIFICATION.md P1.2).
TARGETS:
        dc.l    $0C0020,$0C0040,$0C0080,$0C0100,$0C0200,$0C0400,$0C0800,$0C1000,$0C2000,$0C4000,$0C8000
        dc.l    $0C5002,$0C6004,$0CA008,$0CC010
        dc.l    $0DFFDE,$0DFFBE,$0DFF7E,$0DFEFE,$0DFDFE,$0DFBFE,$0DF7FE,$0DEFFE,$0DDFFE,$0DBFFE,$0D7FFE
        dc.l    $0DAFFC,$0D9FFA,$0D5FF6,$0D3FEE
        dc.l    $01F000,$02F000,$04F000,$08F000,$10F000
        dc.l    $1EF000,$1DF000,$1BF000,$17F000,$0FF000
