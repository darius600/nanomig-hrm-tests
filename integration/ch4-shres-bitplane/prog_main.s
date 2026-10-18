; prog_main.s -- ch4-shres-bitplane: SuperHires bitplane colour encoding.
; Implements SPECIFICATION.md section 4.5 verbatim: every register write
; besides the CPU's own DMACON/COP1LC/strobe happens through the Copper
; list tables below, exactly as section 4.3/4.4/4.6 specify them. The
; tables themselves (bp_data_*.i, bp_cop_*.i) are generated data,
; transcribed from SPECIFICATION.md sections 3 and 4; nothing here computes
; a register value.
;
; Assembled standalone (no MUT_xxx symbol defined) this is the baseline
; stimulus. Each mutant wrapper (prog_vNN.s / prog_cNN.s) equ's one symbol
; and includes this file unchanged otherwise; every frame's own ifd cascade
; below resolves which table that symbol selects, falling back to that
; frame's baseline. A frame with no mutation symbol always uses its
; baseline table.

	include "nanomig.i"
	include "ch4bp.i"
	org	PROGRAM_ORG

start:
	; ---- step 1 (SPECIFICATION.md 4.5.1): the fixture, a program-stored
	; constant read back before anything else is trusted (RESULTS is
	; ordinary chip RAM, unaffected by the overlay regardless of its state).
	move.w	#FIXTUREVAL,RESULTS

	lea	CUSTOM,a0

	; ---- step 2 (4.5.2)
	move.w	#$7FFF,DMACON(a0)

	; ---- step 3 (4.5.3): chip identification, stored for the test to grade
	move.w	VPOSR(a0),RESULTS+2
	move.w	DENISEID(a0),RESULTS+4

	; ---- step 4 (4.5.4): clear the ROM overlay -- every address below
	; $080000 this program writes is unreachable until this runs.
	CLEAR_OVL

	; ---- step 5 (4.5.5): build every buffer and Copper list.

	; plane-1 / plane-2 stream buffers (32768 words each: 256-word R section
	; then the xorshift pattern, SPECIFICATION.md 4.2)
	lea	PATTERN1TAB,a2
	move.l	#PLANE1BUF,a1
	move.w	#(PATTERN1TAB_END-PATTERN1TAB)/2-1,d0
.cpyp1:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyp1
	lea	PATTERN2TAB,a2
	move.l	#PLANE2BUF,a1
	move.w	#(PATTERN2TAB_END-PATTERN2TAB)/2-1,d0
.cpyp2:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyp2

	; C8B: plane 2's words over plane 1's for buffer words 1920-13439 (values 0 and 3 only there)
	ifd	MUT_C8B
	lea	PLANE2BUF+1920*2,a2
	lea	PLANE1BUF+1920*2,a1
	move.w	#(13440-1920)-1,d0
.c8b:
	move.w	(a2)+,(a1)+
	dbra	d0,.c8b
	endif

	; C3D: bit 0 of plane 1 buffer word 1500 flipped (line 18 of every frame)
	ifd	MUT_C3D
	eori.w	#1,PLANE1BUF+1500*2
	endif

	; C3E: bit 0 of plane 1 buffer word 4256 flipped (line 53 of F1-F4 and F11, line 106 of F5)
	ifd	MUT_C3E
	eori.w	#1,PLANE1BUF+4256*2
	endif

	; zero buffer (32768 words of $0000): a plain clear loop, no table needed
	lea	ZEROBUF,a1
	move.w	#32767,d0
.clrzero:
	move.w	#0,(a1)+
	dbra	d0,.clrzero

	; sprite 0 structure (124 words, SPECIFICATION.md 4.2)
	lea	SPR0TAB,a2
	move.l	#SPR0STRUCT,a1
	move.w	#(SPR0TAB_END-SPR0TAB)/2-1,d0
.cpyspr0:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyspr0

	; null sprite (2 words of $0000)
	lea	NULLSPR,a1
	move.w	#0,(a1)+
	move.w	#0,(a1)+
	; C9N: a non-zero control word that does not start a sprite
	ifd	MUT_C9N
	move.w	#$0001,NULLSPR+2
	endif
	; C9S: control words that start a one-line sprite (VSTART $2C, VSTOP $2D)
	ifd	MUT_C9S
	move.w	#$2C40,NULLSPR
	move.w	#$2D00,NULLSPR+2
	endif

	; Copper lists F0-F11 (SPECIFICATION.md 4.3, 4.4, 4.6), one table and one
	; copy loop per frame; each frame's own ifd cascade above its table
	; picks the mutation's data, or the baseline if none is selected.
	lea	F0TAB,a2
	move.l	#F0LIST,a1
	move.w	#(F0TAB_END-F0TAB)/2-1,d0
.cpyf0:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf0

	lea	F1TAB,a2
	move.l	#F1LIST,a1
	move.w	#(F1TAB_END-F1TAB)/2-1,d0
.cpyf1:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf1

	lea	F2TAB,a2
	move.l	#F2LIST,a1
	move.w	#(F2TAB_END-F2TAB)/2-1,d0
.cpyf2:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf2

	lea	F3TAB,a2
	move.l	#F3LIST,a1
	move.w	#(F3TAB_END-F3TAB)/2-1,d0
.cpyf3:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf3

	lea	F4TAB,a2
	move.l	#F4LIST,a1
	move.w	#(F4TAB_END-F4TAB)/2-1,d0
.cpyf4:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf4

	lea	F5TAB,a2
	move.l	#F5LIST,a1
	move.w	#(F5TAB_END-F5TAB)/2-1,d0
.cpyf5:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf5

	lea	F6TAB,a2
	move.l	#F6LIST,a1
	move.w	#(F6TAB_END-F6TAB)/2-1,d0
.cpyf6:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf6

	lea	F7TAB,a2
	move.l	#F7LIST,a1
	move.w	#(F7TAB_END-F7TAB)/2-1,d0
.cpyf7:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf7

	lea	F8TAB,a2
	move.l	#F8LIST,a1
	move.w	#(F8TAB_END-F8TAB)/2-1,d0
.cpyf8:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf8

	lea	F9TAB,a2
	move.l	#F9LIST,a1
	move.w	#(F9TAB_END-F9TAB)/2-1,d0
.cpyf9:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf9

	lea	F10TAB,a2
	move.l	#F10LIST,a1
	move.w	#(F10TAB_END-F10TAB)/2-1,d0
.cpyf10:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf10

	lea	F11TAB,a2
	move.l	#F11LIST,a1
	move.w	#(F11TAB_END-F11TAB)/2-1,d0
.cpyf11:
	move.w	(a2)+,(a1)+
	dbra	d0,.cpyf11

	; ---- step 6 (4.5.6): start the Copper exactly as p.30 prescribes --
	; initialise COP1LC and strobe COPJMP1 before Copper DMA is turned on.
	move.l	#F0LIST,COP1LCH(a0)
	move.w	COPJMP1(a0),d1
	move.w	#$83A0,DMACON(a0)

	; ---- step 7 (4.5.7): wait for at least 13 vertical blanks, writing no
	; custom register from here on. F0 runs twice when it was started in the
	; last lines of a frame, and F11 must be shown whole either way. A frame
	; start is V dropping below 16 after having been at or above 256
	; (VPOSR/VHPOSR, p.299/Appendix B).
	moveq	#0,d5
	move.w	#13,d7
.poll:
	move.w	VHPOSR(a0),d1
	move.w	VPOSR(a0),d0
	and.w	#1,d0
	lsl.w	#8,d0
	and.w	#$FF00,d1
	or.w	d1,d0
	cmp.w	#256,d0
	bcs.s	.checklow
	moveq	#1,d5
	bra.s	.poll
.checklow:
	cmp.w	#16,d0
	bcc.s	.poll
	tst.w	d5
	beq.s	.poll
	moveq	#0,d5
	subq.w	#1,d7
	bne.s	.poll

	; ---- step 8 (4.5.8)
	DONE

	; ---- data tables (below), never reached by falling off the end of the
	; code above -- DONE hangs in place, and every ifd/else branch above
	; also ends in a hang, so execution never runs on into what follows.
PATTERN1TAB:
	ifd	MUT_C8
	include	"bp_data_pattern1_vc8.i"
	else
	ifd	MUT_R0
	include	"bp_data_pattern1_vr0.i"
	else
	include	"bp_data_pattern1.i"
	endif
	endif
PATTERN1TAB_END:
PATTERN2TAB:
	ifd	MUT_C8
	include	"bp_data_pattern2_vc8.i"
	else
	ifd	MUT_R0
	include	"bp_data_pattern2_vr0.i"
	else
	include	"bp_data_pattern2.i"
	endif
	endif
PATTERN2TAB_END:
SPR0TAB:
	ifd	MUT_C11
	include	"bp_data_sprite0_vc11.i"
	else
	include	"bp_data_sprite0.i"
	endif
SPR0TAB_END:
F0TAB:
				include	"bp_cop_f0_base.i"
F0TAB_END:
F1TAB:
				ifd	MUT_V1C0R
				include	"bp_cop_f1_v1c0r.i"
				else
				ifd	MUT_V1C0G
				include	"bp_cop_f1_v1c0g.i"
				else
				ifd	MUT_V1C0B
				include	"bp_cop_f1_v1c0b.i"
				else
				ifd	MUT_V1C1R
				include	"bp_cop_f1_v1c1r.i"
				else
				ifd	MUT_V1C1G
				include	"bp_cop_f1_v1c1g.i"
				else
				ifd	MUT_V1C1B
				include	"bp_cop_f1_v1c1b.i"
				else
				ifd	MUT_V1C2R
				include	"bp_cop_f1_v1c2r.i"
				else
				ifd	MUT_V1C2G
				include	"bp_cop_f1_v1c2g.i"
				else
				ifd	MUT_V1C2B
				include	"bp_cop_f1_v1c2b.i"
				else
				ifd	MUT_V1C3R
				include	"bp_cop_f1_v1c3r.i"
				else
				ifd	MUT_V1C3G
				include	"bp_cop_f1_v1c3g.i"
				else
				ifd	MUT_V1C3B
				include	"bp_cop_f1_v1c3b.i"
				else
				ifd	MUT_V2
				include	"bp_cop_f1_v2.i"
				else
				ifd	MUT_C5
				include	"bp_cop_f1_v2.i"
				else
				ifd	MUT_V3A
				include	"bp_cop_f1_v3a.i"
				else
				ifd	MUT_C3
				include	"bp_cop_f1_vc3.i"
				else
				ifd	MUT_C6
				include	"bp_cop_f1_vc6.i"
				else
				ifd	MUT_C7
				include	"bp_cop_f1_vc7.i"
				else
				ifd	MUT_C9
				include	"bp_cop_f1_vc9.i"
				else
				ifd	MUT_R2B
				include	"bp_cop_f1_vr2b.i"
				else
				ifd	MUT_C7C
				include	"bp_cop_f1_vc7c.i"
				else
				ifd	MUT_C3P
				include	"bp_cop_f1_vc3p.i"
				else
				ifd	MUT_P2G
				include	"bp_cop_f1_vp2g.i"
				else
				include	"bp_cop_f1_base.i"
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
F1TAB_END:
F2TAB:
				ifd	MUT_C4S
				include	"bp_cop_f2_vc4s.i"
				else
				ifd	MUT_VP1JG
				include	"bp_cop_f2_vp1jg.i"
				else
				ifd	MUT_VLIVE
				include	"bp_cop_f2_vlive.i"
				else
				ifd	MUT_V4DN0B2
				include	"bp_cop_f2_v4dn0b2.i"
				else
				ifd	MUT_V4DN0B3
				include	"bp_cop_f2_v4dn0b3.i"
				else
				ifd	MUT_V4DN0B6
				include	"bp_cop_f2_v4dn0b6.i"
				else
				ifd	MUT_V4DN0B7
				include	"bp_cop_f2_v4dn0b7.i"
				else
				ifd	MUT_V4DN0B10
				include	"bp_cop_f2_v4dn0b10.i"
				else
				ifd	MUT_V4DN0B11
				include	"bp_cop_f2_v4dn0b11.i"
				else
				ifd	MUT_V4DN1B2
				include	"bp_cop_f2_v4dn1b2.i"
				else
				ifd	MUT_V4DN1B3
				include	"bp_cop_f2_v4dn1b3.i"
				else
				ifd	MUT_V4DN1B6
				include	"bp_cop_f2_v4dn1b6.i"
				else
				ifd	MUT_V4DN1B7
				include	"bp_cop_f2_v4dn1b7.i"
				else
				ifd	MUT_V4DN1B10
				include	"bp_cop_f2_v4dn1b10.i"
				else
				ifd	MUT_V4DN1B11
				include	"bp_cop_f2_v4dn1b11.i"
				else
				ifd	MUT_V4DN2B2
				include	"bp_cop_f2_v4dn2b2.i"
				else
				ifd	MUT_V4DN2B3
				include	"bp_cop_f2_v4dn2b3.i"
				else
				ifd	MUT_V4DN2B6
				include	"bp_cop_f2_v4dn2b6.i"
				else
				ifd	MUT_V4DN2B7
				include	"bp_cop_f2_v4dn2b7.i"
				else
				ifd	MUT_V4DN2B10
				include	"bp_cop_f2_v4dn2b10.i"
				else
				ifd	MUT_V4DN2B11
				include	"bp_cop_f2_v4dn2b11.i"
				else
				ifd	MUT_V4DN3B2
				include	"bp_cop_f2_v4dn3b2.i"
				else
				ifd	MUT_V4DN3B3
				include	"bp_cop_f2_v4dn3b3.i"
				else
				ifd	MUT_V4DN3B6
				include	"bp_cop_f2_v4dn3b6.i"
				else
				ifd	MUT_V4DN3B7
				include	"bp_cop_f2_v4dn3b7.i"
				else
				ifd	MUT_V4DN3B10
				include	"bp_cop_f2_v4dn3b10.i"
				else
				ifd	MUT_V4DN3B11
				include	"bp_cop_f2_v4dn3b11.i"
				else
				ifd	MUT_V4A
				include	"bp_cop_f2_v4a.i"
				else
				ifd	MUT_V4B
				include	"bp_cop_f2_v4b.i"
				else
				ifd	MUT_C6B
				include	"bp_cop_f2_vc6b.i"
				else
				ifd	MUT_V4C2
				include	"bp_cop_f2_vc4n.i"
				else
				ifd	MUT_VKEYAR
				include	"bp_cop_f2_vkeyar.i"
				else
				ifd	MUT_VKEYAG
				include	"bp_cop_f2_vkeyag.i"
				else
				ifd	MUT_VKEYAB
				include	"bp_cop_f2_vkeyab.i"
				else
				include	"bp_cop_f2_base.i"
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
F2TAB_END:
F3TAB:
				ifd	MUT_V4B
				include	"bp_cop_f3_v4b.i"
				else
				ifd	MUT_C6B
				include	"bp_cop_f3_vc6b.i"
				else
				ifd	MUT_V4C3
				include	"bp_cop_f3_vc4n.i"
				else
				include	"bp_cop_f3_base.i"
				endif
				endif
				endif
F3TAB_END:
F4TAB:
				ifd	MUT_V4B
				include	"bp_cop_f4_v4b.i"
				else
				ifd	MUT_C6B
				include	"bp_cop_f4_vc6b.i"
				else
				ifd	MUT_V4C4
				include	"bp_cop_f4_vc4n.i"
				else
				include	"bp_cop_f4_base.i"
				endif
				endif
				endif
F4TAB_END:
F5TAB:
				ifd	MUT_V3B
				include	"bp_cop_f5_v3b.i"
				else
				ifd	MUT_V5B
				include	"bp_cop_f5_v5b.i"
				else
				ifd	MUT_C4
				include	"bp_cop_f5_vc4.i"
				else
				ifd	MUT_R2M5
				include	"bp_cop_f5_vr2.i"
				else
				ifd	MUT_C6B
				include	"bp_cop_f5_vc6b.i"
				else
				ifd	MUT_V5D
				include	"bp_cop_f5_v5d.i"
				else
				include	"bp_cop_f5_base.i"
				endif
				endif
				endif
				endif
				endif
				endif
F5TAB_END:
F6TAB:
				ifd	MUT_V3C
				include	"bp_cop_f6_v3c.i"
				else
				ifd	MUT_R2M6
				include	"bp_cop_f6_vr2.i"
				else
				ifd	MUT_C6B
				include	"bp_cop_f6_vc6b.i"
				else
				include	"bp_cop_f6_base.i"
				endif
				endif
				endif
F6TAB_END:
F7TAB:
				ifd	MUT_V5C
				include	"bp_cop_f7_v5c.i"
				else
				ifd	MUT_V6A
				include	"bp_cop_f7_v6a.i"
				else
				ifd	MUT_V6C
				include	"bp_cop_f7_v6c.i"
				else
				ifd	MUT_V6S
				include	"bp_cop_f7_v6s.i"
				else
				ifd	MUT_C10S
				include	"bp_cop_f7_vc10s.i"
				else
				ifd	MUT_C10
				include	"bp_cop_f7_vc10.i"
				else
				ifd	MUT_C7B
				include	"bp_cop_f7_vc7b.i"
				else
				ifd	MUT_RUNENDS
				include	"bp_cop_f7_vruns.i"
				else
				ifd	MUT_C4C
				include	"bp_cop_f7_vc4c.i"
				else
				include	"bp_cop_f7_base.i"
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
				endif
F7TAB_END:
F8TAB:
				ifd	MUT_V6E
				include	"bp_cop_f8_v6e.i"
				else
				ifd	MUT_C4B
				include	"bp_cop_f8_vc4b.i"
				else
				ifd	MUT_V5E
				include	"bp_cop_f8_v5e.i"
				else
				include	"bp_cop_f8_base.i"
				endif
				endif
				endif
F8TAB_END:
F9TAB:
				ifd	MUT_V6B
				include	"bp_cop_f9_v6b.i"
				else
				ifd	MUT_V6Q
				include	"bp_cop_f9_v6q.i"
				else
				include	"bp_cop_f9_base.i"
				endif
				endif
F9TAB_END:
F10TAB:
				ifd	MUT_V6D
				include	"bp_cop_f10_v6d.i"
				else
				ifd	MUT_V6L
				include	"bp_cop_f10_v6l.i"
				else
				ifd	MUT_C4D
				include	"bp_cop_f10_vc4d.i"
				else
				include	"bp_cop_f10_base.i"
				endif
				endif
				endif
F10TAB_END:
F11TAB:
				ifd	MUT_V3G
				include	"bp_cop_f11_v3g.i"
				else
				ifd	MUT_C6B
				include	"bp_cop_f11_vc6b.i"
				else
				include	"bp_cop_f11_base.i"
				endif
				endif
F11TAB_END:
