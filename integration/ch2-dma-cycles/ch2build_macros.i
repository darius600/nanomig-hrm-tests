; ch2build_macros.i -- macros a program's own S1 step (SPECIFICATION.md
; 2.6/2.8/2.9) uses to build its Copper list(s) by CPU write, once the
; overlay is clear (ch2_skel.i's CLEAR_OVL). Each macro only sets up the
; registers a shared subroutine in ch2build_code.i needs and calls it;
; ch2build_code.i is included once, near the end of the program (after
; DONE), so its own code never falls into mainline execution.
;
; Register convention, held across every macro below: a1 is always the
; destination cursor (a Copper-list or structure byte address, advanced by
; the subroutine's own post-increment writes); a5/a6 are consumed pointer
; tables (8 and up-to-6 long words); a2/d1-d7 are the parameters a specific
; subroutine needs, documented at its own definition in ch2build_code.i.
; a0 (CUSTOM) is never touched, so S2's own register writes need not
; reload it after S1.

; L0 -- END only. \1 = destination byte address.
BUILD_END       macro
                move.l  #(\1),a1
                move.w  #$FFFF,(a1)+
                move.w  #$FFFE,(a1)+
                endm

; L3 -- V_spr; END. \1 = destination, \2 = 8-long sprite pointer table.
BUILD_L3        macro
                move.l  #(\1),a1
                lea     \2,a5
                bsr     build_Vspr8
                move.w  #$FFFF,(a1)+
                move.w  #$FFFE,(a1)+
                endm

; L4(n) -- V_bpl(n); END. \1 = destination, \2 = n-long bpl pointer table,
; \3 = n.
BUILD_L4        macro
                move.l  #(\1),a1
                lea     \2,a6
                move.w  #(\3),d4
                bsr     build_Vbpln
                move.w  #$FFFF,(a1)+
                move.w  #$FFFE,(a1)+
                endm

; L5 -- V_all (V_spr; V_bpl(6); COP2LC <- \4); the 113-WAIT/MOVE loop;
; trailer. \1 = destination (COP_MAIN), \2 = 8-long sprite pointer table,
; \3 = 6-long bpl pointer table, \4 = L5b destination address, \5 =
; override_i (-1 if none: P7's own L5' contrast), \6 = override_hp, \7 =
; extra_after_wait (-1 if none: T1's own contrast).
BUILD_L5        macro
                move.l  #(\1),a1
                lea     \2,a5
                lea     \3,a6
                move.l  #(\4),a2
                move.w  #(\5),d5
                move.w  #(\6),d6
                move.w  #(\7),d7
                bsr     build_L5
                endm

; L5, truncated to V_spr; V_bpl(6); END (K6's own contrast, SPECIFICATION.md
; P6.6): the WAIT/MOVE loop and trailer are never written, so K6 (every
; word of L5/L5b read) must fail. \1 = destination, \2/\3 as BUILD_L5.
BUILD_L5_TRUNC  macro
                move.l  #(\1),a1
                lea     \2,a5
                bsr     build_Vspr8
                lea     \3,a6
                moveq   #6,d4
                bsr     build_Vbpln
                move.w  #$FFFF,(a1)+
                move.w  #$FFFE,(a1)+
                endm

; L5b -- WAIT VP=$B8,HP=$20; four x MOVE COLOR00<-0; END. \1 = destination.
BUILD_L5B       macro
                move.l  #(\1),a1
                move.w  #$B821,(a1)+
                move.w  #$FFFE,(a1)+
                move.w  #$0180,(a1)+
                move.w  #$0000,(a1)+
                move.w  #$0180,(a1)+
                move.w  #$0000,(a1)+
                move.w  #$0180,(a1)+
                move.w  #$0000,(a1)+
                move.w  #$0180,(a1)+
                move.w  #$0000,(a1)+
                move.w  #$FFFF,(a1)+
                move.w  #$FFFE,(a1)+
                endm

; Copies a static dc.w table (SPECIFICATION.md-literal content with no
; pointer arithmetic: the S1 and S2 lists and structures) to a destination.
; \1 = destination, \2 = source table label, \3 = source table's own _end
; label (word count = (\3-\2)/2).
CPY_TABLE       macro
                lea     \2,a2
                move.l  #(\1),a1
                move.w  #((\3-\2)/2-1),d0
                bsr     cpy_words
                endm
