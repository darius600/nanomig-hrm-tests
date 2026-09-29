; prog_p2.s -- P2 [p21,p36,p275]: the written data is the word read from RAM
; as the instruction's second word, at each execution. Run P1-A-set (see
; prog_p1a.s): identical list L_A at $0E0000 (p1a_list_set.i), identical
; frame and monitor loop, with one addition (SPECIFICATION.md P2.2): after
; the first wrap that follows the first complete execution of the list, the
; CPU writes $AAAA to $0E000E (b_0's own IR2 word) once, bracketed by
; markers.
;
; The monitor loop is split at a fixed iteration count into two segments,
; each byte-for-byte the loop prog_p1a.s and prog_p3.s already use (no
; added instructions in either segment, so the per-line sample density
; those files already established is unaffected); the write sits between
; them. The split point (SEG1_ITERS) is sized from this package's own,
; already-measured timing for this identical loop body: SPECIFICATION.md
; section 7 records "13 to 15 fields" over 30000 iterations for P1-A/P3's
; own captures on this harness, i.e. comfortably more than one field's
; worth of iterations by SEG1_ITERS. The C++ side's own P2-p1 precondition
; verifies the write's actual placement from the captured log, independent
; of this choice.
;
; Symbols this source answers to:
;   P2_MUT_EARLY -- P2-p1's own mutation: the write happens in step 3,
;                    before the Copper starts, instead of after the first
;                    wrap.
;   P2_C1        -- P2's own contrasting case: $AAA8 is written instead of
;                    $AAAA, at the same point.
;   P2_NOCOPEN   -- G1's own baseline for P2 (4.5: "a baseline run of the
;                    same program"): DMACON started $8200 (no COPEN)
;                    instead of $8280. The runtime write of WRITE_VAL still
;                    happens, as in every other variant of this source.

        include "nanomig.i"
        include "ch2move.i"

L_A             equ     $0E0000
MARK_M0         equ     $0E7000
MARK_FINAL      equ     $0E7002
DMACONR_SAVE    equ     $0E7100
MARK_WA         equ     $0E7108         ; marker before the CPU's own write (I3)
MARK_WB         equ     $0E710A         ; marker after it
WRITE_TARGET    equ     $0E000E         ; b_0's own IR2 word

; Log buffer sizing: see prog_p3.s's own comment (same $110000-$13FFFF
; area). SEG1_ITERS is comfortably more than one field's worth of
; iterations (see header); SEG2_ITERS leaves several more fields captured
; after the write, for P2's own "4 fields captured" (P2.2).
SEG1_ITERS      equ     3000
SEG2_ITERS      equ     9000
LOGBUF          equ     $110000

        ifd     P2_C1
WRITE_VAL       equ     $AAA8
        else
WRITE_VAL       equ     $AAAA
        endif

        org     PROGRAM_ORG

start:
        FRAME_INIT

        ; ---- step 3: write list L_A, one CPU write per word (identical to
        ; P1-A-set) ------------------------------------------------------
        lea     L_A,a1
        include "p1a_list_set.i"
        move.w  #$FFFF,(a1)+            ; park
        move.w  #$FFFE,(a1)+

        ifd     P2_MUT_EARLY
        ; P2-p1's own mutation: the write happens here, before the Copper
        ; starts, instead of after the first wrap.
        move.w  VHPOSR(a0),MARK_WA.l
        move.w  #WRITE_VAL,WRITE_TARGET.l
        move.w  VHPOSR(a0),MARK_WB.l
        endif

        ; ---- step 5: start the Copper (p.30's own sequence) -----------
        move.l  #L_A,COP1LCH(a0)
        move.w  #0,COPJMP1(a0)
        move.w  VHPOSR(a0),MARK_M0.l     ; marker M0
        ifd     P2_NOCOPEN
        move.w  #$8200,DMACON(a0)        ; SET/CLR, DMAEN only (no COPEN)
        else
        move.w  #$8280,DMACON(a0)        ; SET/CLR, DMAEN, COPEN
        endif
        move.w  DMACONR(a0),DMACONR_SAVE.l

        ; ---- step 6: monitor loop (I2), segment 1 -----------------------
        lea     LOGBUF,a1
        move.l  #SEG1_ITERS,d6
.mloop1: move.w  INTENAR(a0),(a1)+
        move.w  VPOSR(a0),(a1)+
        move.w  VHPOSR(a0),(a1)+
        subq.l  #1,d6
        bne.s   .mloop1

        ifnd    P2_MUT_EARLY
        move.w  VHPOSR(a0),MARK_WA.l
        move.w  #WRITE_VAL,WRITE_TARGET.l
        move.w  VHPOSR(a0),MARK_WB.l
        endif

        ; ---- step 6: monitor loop (I2), segment 2 -----------------------
        move.l  #SEG2_ITERS,d6
.mloop2: move.w  INTENAR(a0),(a1)+
        move.w  VPOSR(a0),(a1)+
        move.w  VHPOSR(a0),(a1)+
        subq.l  #1,d6
        bne.s   .mloop2

        ; ---- step 7: final marker, then DONE ---------------------------
        move.w  VHPOSR(a0),MARK_FINAL.l
        DONE
