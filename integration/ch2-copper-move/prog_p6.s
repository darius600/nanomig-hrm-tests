; prog_p6.s -- P6 [p25,p30,p288]: the Copper restarts from COP1LC once per
; field, during vertical blanking, without a strobe. Parks L6a ($0E3000),
; L6b ($0E3100), L6c ($0E3200), each a park followed by six $0000 words (16
; bytes, SPECIFICATION.md 4.2, P6.2), so that a read of the word after the
; park (G4) is a word this specification prescribes. Before step 5 the CPU
; loads COP2LC = L6c; the frame then starts the Copper at L6a. The program
; never writes a strobe after step 5: every later instruction fetch at L6a
; is the automatic restart this proposition tests. Run once with the
; harness's own ECS/PAL Config (default) for the PAL run, and once with
; ECS/NTSC (chipset_config=$0A, the same value P3-G5 already established)
; for the NTSC run -- the C++ side selects the Config; this source does not
; change with the video standard.
;
; Symbols this source answers to:
;   P6_MUT_P6P1     -- P6-p1's own mutation: L6a is `$6401,$FF00` (WAIT
;                       line $64) followed by a park at $0E3004, instead of
;                       a park at $0E3000.
;   P6_MUT_SHORTCAP -- P6-p2's own mutation: the capture is 2 fields long.
;   P6_NOCOPEN      -- G1's own baseline: DMACON started $8200 (no COPEN)
;                       instead of $8280.
;   P6_C1           -- P6's own contrasting case: in field 3 (the fourth
;                       field of the capture, 0-indexed), once the beam
;                       line lies in $64-$C8, the CPU executes
;                       MOVE.L #L6B,$DFF080 (COP1LC = L6b) once, bracketed
;                       by markers, with no strobe.

        include "nanomig.i"
        include "ch2move.i"

L6A             equ     $0E3000
L6B             equ     $0E3100
L6C             equ     $0E3200
MARK_M0         equ     $0E7000
MARK_FINAL      equ     $0E7002
MARK_WA         equ     $0E7004         ; P6_C1's own markers (I3)
MARK_WB         equ     $0E7006
DMACONR_SAVE    equ     $0E7100

LOGBUF          equ     $110000

        ifd     P6_MUT_SHORTCAP
; P6-p2's own mutation: short enough that fewer than four wraps fall in
; the capture (P6-p2's own clause), but long enough to still hold one
; genuinely complete middle field on both cores (measured: fx68k and
; TG68K start the monitor loop a few hundred clk_sys cycles apart, which
; at this small a capture is enough to shift the field count by one) so
; that P6-p3's own, unrelated video-standard check still has something
; to grade instead of finding no complete field at all.
MON_ITERS       equ     6642            ; about 3 fields (4500 + one field's own ~2142)
        else
        ifd     P6_C1
; P6_C1's own split: SEG_A_ITERS is sized from this package's own,
; already-measured timing for this identical loop body (SPECIFICATION.md
; section 7: "13 to 15 fields" over 30000 iterations, i.e. about 2142
; iterations/field and 6.87/line on this harness) to land the write
; inside the $64-$C8 window the specification names, once the beam-sync
; wait above removes the skew CPU-only instructions before it would
; otherwise carry in. The C++ side's own P6-C1 guard checks the write's
; own marker against the actually captured line, independent of this
; choice, and does not require any particular field index. SEG_B_ITERS
; keeps SEG_A+SEG_B's own total, and so the capture's overall field
; count, unchanged.
SEG_A_ITERS     equ     8006            ; 3*2142 + ~$98*6.87
SEG_B_ITERS     equ     7994            ; keeps SEG_A+SEG_B's own total at 16000
        else
MON_ITERS       equ     11000           ; about 5 fields (P6.2)
        endif
        endif

        org     PROGRAM_ORG

start:
        FRAME_INIT

        ; ---- step 3: write the three parks, one CPU write per word -----
        ifd     P6_MUT_P6P1
        lea     L6A,a1
        move.w  #$6401,(a1)+            ; P6-p1's own mutation: WAIT line $64
        move.w  #$FF00,(a1)+
        move.w  #$FFFF,(a1)+            ; park at $0E3004
        move.w  #$FFFE,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        else
        lea     L6A,a1
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        endif

        lea     L6B,a1
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        lea     L6C,a1
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+
        move.w  #$0000,(a1)+

        ; ---- step 4: COP2LC = L6c ---------------------------------------
        move.l  #L6C,COP2LCH(a0)

        ; ---- step 5: start the Copper at L6a (p.30's own sequence) -----
        move.l  #L6A,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  VHPOSR(a0),MARK_M0.l     ; marker M0

        ifd     P6_NOCOPEN
        move.w  #$8200,DMACON(a0)       ; SET/CLR, DMAEN only (no COPEN)
        else
        move.w  #$8280,DMACON(a0)       ; SET/CLR, DMAEN, COPEN
        endif
        move.w  DMACONR(a0),DMACONR_SAVE.l

        ; ---- step 6: monitor loop (I2). No strobe is written after this
        ; point in any variant. Every segment below is byte-for-byte the
        ; loop prog_p1a.s and prog_p3.s already use (no added
        ; instructions), so the per-line sample density those files
        ; already established is unaffected. P6_C1 splits the loop at a
        ; calibrated iteration count (see SEG_A_ITERS above) and writes
        ; COP1LC = L6b, bracketed by markers, between the two segments.
        lea     LOGBUF,a1
        ifd     P6_C1
; Waiting here for the beam's own V to read 0 (the start of a field, the
; same anchor on both cores since it comes from the beam counter, not CPU
; cycles) removes the skew the CPU-only instructions before this point
; would otherwise carry into SEG_A_ITERS below. VPOSR and VHPOSR are two
; separate register reads, so a V8 change between them can produce a torn
; exit (V8=1 already, but VHPOSR's own line byte still reads $00, the
; wrap instant); accepted, not guarded against, and stated per core (the
; C++ side's own P6-C1 notes) rather than fixed here, since a re-check
; against this same asymmetric timing moved fx68k's own calibrated write
; outside the $64-$C8 window on this harness.
.syncv: move.w  VPOSR(a0),d0
        andi.w  #1,d0
        bne.s   .syncv
        move.w  VHPOSR(a0),d1
        andi.w  #$FF00,d1
        bne.s   .syncv

        move.l  #SEG_A_ITERS,d6
.mloopA: move.w INTENAR(a0),(a1)+
        move.w  VPOSR(a0),(a1)+
        move.w  VHPOSR(a0),(a1)+
        subq.l  #1,d6
        bne.s   .mloopA

        move.w  VHPOSR(a0),MARK_WA.l
        move.l  #L6B,COP1LCH(a0)         ; COP1LC = L6b, no strobe
        move.w  VHPOSR(a0),MARK_WB.l

        move.l  #SEG_B_ITERS,d6
.mloopB: move.w INTENAR(a0),(a1)+
        move.w  VPOSR(a0),(a1)+
        move.w  VHPOSR(a0),(a1)+
        subq.l  #1,d6
        bne.s   .mloopB
        else
        move.l  #MON_ITERS,d6
.mloop: move.w  INTENAR(a0),(a1)+
        move.w  VPOSR(a0),(a1)+
        move.w  VHPOSR(a0),(a1)+
        subq.l  #1,d6
        bne.s   .mloop
        endif

        ; ---- step 7: final marker, then DONE ---------------------------
        move.w  VHPOSR(a0),MARK_FINAL.l
        DONE
