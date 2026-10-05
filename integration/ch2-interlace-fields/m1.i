; m1.i -- Measurement M1 (SPECIFICATION.md section 2.5): sample the beam
; position with the CPU, detect field wraps, and record each field.
;
; The includer defines, before `include "m1.i"`:
;   M1_K        -- number of fields to record
;   M1_RECBASE  -- address of the first of M1_K x 10-byte records:
;                  word0 firstV, word1 lastV, word2 gapFlag,
;                  word3 lofOR, word4 lofAND
;   M1_STARTW   -- address of the one-word start marker
;   M1_DONEW    -- address of the one-word completion marker
; and calls `bsr m1_run`. m1_run returns (rts) once M1_DONEW has been
; written; d0-d7/a0/a1/a6 are clobbered.
;
; M1_DELAY, if the includer defines it, adds a busy-wait inside the
; sampling loop -- section 3.6's precondition contrast (I-j / T-f): "M1
; waits about 100 microseconds between two samples", so consecutive
; samples land many lines apart and PI-1/PT-1's own validity check (every
; record's first V is 0, gap flag clear) must go red. The exact count is
; this build's own tuning; it is not a page value and is guarded by the
; very check its contrast is meant to break.
;
; Algorithm, verbatim from section 2.5:
;  1. Take a sample: read VPOSR:VHPOSR as a long twice; accept only if
;     bits 16-8 agree between the two reads.
;  2. V = bits 16-8 of the accepted long, LOF = bit 31.
;  3. A wrap is an accepted sample whose V is lower than the previous
;     accepted V.
;  4. The 2nd wrap after this routine starts opens recording (writes
;     M1_STARTW); every wrap after that closes the field just sampled and
;     opens the next, until M1_K records have been written, at which point
;     M1_DONEW is written and the routine returns.
;
; Performance note: a field is only ~312-313 (PAL) or ~262-263 (NTSC)
; lines of ~227-228 colour clocks (~65 microseconds) each, so the sampling
; loop must not take anywhere near that long per iteration or it misses
; lines (the gap flag would spuriously fire on real, otherwise-valid
; fields). Keeping the running state in chip-RAM variables, read and
; written every sample, measurably costs enough real time to do that: an
; absolute-long chip-RAM access is itself a slow-chip-bus instruction
; fetch (a 3-word instruction) plus a slow-chip-bus data access. This
; routine instead keeps all of that state in data registers across the
; sampling loop -- d4 = the previous accepted sample's V, in its raw bit
; position (bits 16-8), or -1 if there is no previous sample yet; d5 =
; the current field's first accepted V, same raw position and sentinel;
; d6/d7 = the OR/AND of the LOF bit (31) seen this field, also left in
; its raw bit position. Memory is touched only on a wrap (m1_onwrap, at
; most M1_K+2 times for the whole run).

; The run's own bookkeeping lives in address registers, so that a wrap
; touches chip memory only for the record words and the two markers:
;   a2 = pointer to the next record word
;   a3 = number of wraps seen
;   a4 = number of records written
;   a5 = gap flag of the field being sampled (0 or 1)

m1_run:
                moveq   #-1,d4          ; PREVP: no previous accepted sample yet
                moveq   #-1,d5          ; FIRSTP: no field open yet
                moveq   #0,d6           ; LOFORP
                moveq   #0,d7           ; LOFANDP
                suba.l  a3,a3
                suba.l  a4,a4
                suba.l  a5,a5
                lea     M1_RECBASE,a2
                lea     CUSTOM,a6

.sample:
                move.l  VPOSR(a6),d0
                move.l  VPOSR(a6),d1
                move.l  d0,d2
                eor.l   d1,d2
                and.l   #$0001FF00,d2
                bne.s   .sample         ; the two reads disagree on V: discard both

                ifd     M1_DELAY
                movem.l d0-d1,-(a7)
                move.l  #M1_DELAY,d3
.wait:          subq.l  #1,d3
                bne.s   .wait
                movem.l (a7)+,d0-d1
                endif

                ; d1 stays the accepted sample, untouched, in its raw bit
                ; position: V lives at bits 16-8, LOF at bit 31.
                cmpi.l  #-1,d4
                beq     .doaccum        ; no previous sample: accept unconditionally

                move.l  d1,d3
                and.l   #$0001FF00,d3   ; d3 = this sample's V, raw position
                cmp.l   d3,d4           ; d4 - d3 (prevV - V, both raw/same scale)
                bgt     .iswrap
                move.l  d4,d2
                add.l   #$100,d2        ; d2 = prevV + 1 (raw)
                cmp.l   d2,d3
                ble     .doaccum        ; V <= prevV+1: no gap
                lea     1.w,a5
                bra     .doaccum

.iswrap:
                bsr     m1_onwrap       ; clobbers d2/d3 and a2-a5; d0/d1 preserved
                tst.w   d2              ; d2 = 1 once the completion word is written
                beq.s   .newfield
                bra     m1_return
.newfield:
                moveq   #-1,d5          ; the new field has no accumulation yet

.doaccum:
                cmpi.l  #-1,d5
                bne.s   .notfirst
                move.l  d1,d5
                and.l   #$0001FF00,d5   ; FIRSTP = this sample's V, raw position
                move.l  d1,d2
                and.l   #$80000000,d2
                move.l  d2,d6           ; LOFORP  = this sample's LOF, raw position
                move.l  d2,d7           ; LOFANDP = same
                bra.s   .storeprev

.notfirst:
                move.l  d1,d2
                and.l   #$80000000,d2
                or.l    d2,d6
                and.l   d2,d7

.storeprev:
                move.l  d1,d4
                and.l   #$0001FF00,d4   ; PREVP = this sample's V, raw position
                bra     .sample

m1_return:
                rts

; ---- wrap handling: section 2.5 item 5, verbatim (see file header) ----
; On entry: d4 = PREVP of the field that just ended (its last accepted
; V), d5 = FIRSTP of that same field, d6/d7 = its LOF OR/AND, all in raw
; bit position; a5 is that field's gap flag. d0/d1 (the sample that
; triggered this wrap -- the new field's own first sample) must be left
; untouched; d2/d3 and a2-a5 are this routine's. d2 returns 1 once the
; completion word has been written, otherwise 0.

m1_onwrap:
                moveq   #0,d2
                lea     1(a3),a3
                cmpa.w  #2,a3
                beq     .startcap
                blt     .ignore
                ; more than 2 wraps: close the field.
                move.l  d5,d3
                lsr.l   #8,d3
                and.w   #$1FF,d3
                move.w  d3,(a2)+                ; firstV
                move.l  d4,d3
                lsr.l   #8,d3
                and.w   #$1FF,d3
                move.w  d3,(a2)+                ; lastV
                move.w  a5,(a2)+                ; gap flag
                tst.l   d6
                smi     d3
                and.w   #1,d3
                move.w  d3,(a2)+                ; lofOR
                tst.l   d7
                smi     d3
                and.w   #1,d3
                move.w  d3,(a2)+                ; lofAND
                suba.l  a5,a5
                lea     1(a4),a4
                cmpa.w  #M1_K,a4
                blt     .ret
                move.w  #$D1CE,M1_DONEW.l
                moveq   #1,d2
.ret:
                rts
.startcap:
                move.w  #$FACE,M1_STARTW.l
                suba.l  a5,a5
                rts
.ignore:
                rts
