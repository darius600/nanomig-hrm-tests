; ch2build_code.i -- the subroutine bodies ch2build_macros.i's own macros
; call. Included once, near the end of each program (after its own
; VBLANK_HANDLER invocation), so this code sits after DONE's own infinite
; branch and mainline execution never falls into it -- it is reached only
; by bsr, from S1.

; ---- wr_movel_copper: writes one Copper MOVE-pair (regH, hi(ptr), regL,
; lo(ptr)) at the cursor. d1.w = regH, d2.w = regL, d3.l = ptr (preserved).
; a1 = cursor (in/out).
wr_movel_copper:
                move.w  d1,(a1)+
                swap    d3
                move.w  d3,(a1)+
                swap    d3
                move.w  d2,(a1)+
                move.w  d3,(a1)+
                rts

; ---- build_Vspr8: writes V_spr (8 x MOVE pairs, SPRxPTH/L) at the cursor.
; a1 = cursor (in/out). a5 = table of 8 long pointers (consumed). Destroys
; d0-d3.
build_Vspr8:
                move.w  #8,d0
                move.w  #SPR0PTH,d4
.bv8:
                move.w  d4,d1
                move.w  d4,d2
                addq.w  #2,d2
                move.l  (a5)+,d3
                bsr     wr_movel_copper
                addq.w  #4,d4
                subq.w  #1,d0
                bne.s   .bv8
                rts

; ---- build_Vbpln: writes V_bpl(n) (n x MOVE pairs, BPLyPTH/L) at the
; cursor. a1 = cursor (in/out). a6 = table of n long pointers (consumed).
; d4.w = n (destroyed). Destroys d0-d3.
build_Vbpln:
                move.w  #BPL1PTH,d0
.bvn:
                move.w  d0,d1
                move.w  d0,d2
                addq.w  #2,d2
                move.l  (a6)+,d3
                bsr     wr_movel_copper
                addq.w  #4,d0
                subq.w  #1,d4
                bne.s   .bvn
                rts

; ---- build_L5_waitloop: writes the 113-entry WAIT/MOVE loop (i=0..112)
; and L5's own trailer (WAIT VP=$B4; SKIP VP=$B4,HP=$40; MOVE COLOR00<-0;
; COPJMP2 strobe), SPECIFICATION.md 2.8 L5. a1 = cursor (in/out, must
; already be past V_all). d5.w = override_i (i whose HP is replaced) or
; -1/$FFFF for none. d6.w = override_hp, used only when d0==d5. Destroys
; d0-d4.
build_L5_waitloop:
                moveq   #0,d0
.wl:
                move.w  d0,d2
                lsl.w   #8,d2
                add.w   #$4000,d2               ; d2 = ($40+i)<<8
                move.w  d0,d1
                add.w   d1,d1                    ; d1 = 2*i
                cmp.w   d5,d0
                bne.s   .noovr
                move.w  d6,d1                    ; d1 = override_hp
.noovr:
                ori.w   #1,d1                     ; d1 = hp|1
                or.w    d1,d2                     ; d2 = w1
                move.w  d2,(a1)+
                move.w  #$FFFE,(a1)+
                move.w  #$0180,(a1)+
                move.w  #$0000,(a1)+
                move.w  #$0180,(a1)+
                move.w  #$0000,(a1)+
                move.w  #$0180,(a1)+
                move.w  #$0000,(a1)+
                addq.w  #1,d0
                cmp.w   #113,d0
                blt.s   .wl

                move.w  #$B401,(a1)+
                move.w  #$FFFE,(a1)+
                move.w  #$B441,(a1)+
                move.w  #$FFFF,(a1)+
                move.w  #$0180,(a1)+
                move.w  #$0000,(a1)+
                move.w  #COPJMP2,(a1)+
                move.w  #$0000,(a1)+
                rts

; ---- build_L5: V_all (V_spr8; V_bpl(6); COP2LC <- a2) then
; build_L5_waitloop. a1 = destination (in/out). a5 = 8-long sprite table
; (consumed). a6 = 6-long bpl table (consumed). a2.l = L5b address. d5/d6
; = build_L5_waitloop's own override params (untouched by build_Vspr8/
; build_Vbpln, which only use d0-d4).
build_L5:
                bsr     build_Vspr8
                moveq   #6,d4
                bsr     build_Vbpln
                move.w  #COP2LCH,d1
                move.w  #COP2LCL,d2
                move.l  a2,d3
                bsr     wr_movel_copper
                bsr     build_L5_waitloop
                rts

; ---- build_structs_c3: writes all eight of C3's own sprite structures
; (SPECIFICATION.md 2.9, structure_C3), at CH_SPR0+$400*x for x=0..7: two
; control words (VSTART=$50/HSTART, VSTOP=$64/same), 20 data pairs
; ($AAAA,$5555), two control words (VSTART=$80, VSTOP=$94), 20 more data
; pairs, two end words. HSTART = $70+$18*x (p.70/128). Destroys d0-d6,a1.
build_structs_c3:
                moveq   #0,d0
.sx:
                move.l  #CH_SPR0,a1
                move.w  d0,d6
                mulu.w  #$400,d6
                add.l   d6,a1

                move.w  d0,d1
                mulu.w  #$18,d1
                add.w   #$70,d1                  ; d1 = HSTART

                move.w  d1,d2
                lsr.w   #1,d2
                move.w  #$5000,d3
                or.w    d2,d3
                move.w  d3,(a1)+                  ; ctrl1a = $50<<8 | HSTART>>1
                move.w  d1,d2
                and.w   #1,d2
                move.w  #$6400,d3
                or.w    d2,d3
                move.w  d3,(a1)+                  ; ctrl2a = $64<<8 | HSTART&1
                moveq   #19,d4
.sd1:
                move.w  #$AAAA,(a1)+
                move.w  #$5555,(a1)+
                dbra    d4,.sd1

                move.w  d1,d2
                lsr.w   #1,d2
                move.w  #$8000,d3
                or.w    d2,d3
                move.w  d3,(a1)+                  ; ctrl1b = $80<<8 | HSTART>>1
                move.w  d1,d2
                and.w   #1,d2
                move.w  #$9400,d3
                or.w    d2,d3
                move.w  d3,(a1)+                  ; ctrl2b = $94<<8 | HSTART&1
                moveq   #19,d4
.sd2:
                move.w  #$AAAA,(a1)+
                move.w  #$5555,(a1)+
                dbra    d4,.sd2

                move.w  #$0000,(a1)+
                move.w  #$0000,(a1)+

                addq.w  #1,d0
                cmp.w   #8,d0
                blt.s   .sx
                rts

; ---- cpy_words: copies a static table to a destination. a1 = destination
; (in/out). a2 = source (in/out). d0.w = word count - 1 (destroyed).
cpy_words:
.cw:
                move.w  (a2)+,(a1)+
                dbra    d0,.cw
                rts
