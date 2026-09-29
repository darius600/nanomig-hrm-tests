; ch2_skel.i -- the S0-S7 program skeleton of SPECIFICATION.md section 2.6,
; factored into macros so every configuration's own prog_*.s only supplies
; what differs: S1/S2's register writes and S4's DMACON bits. The Copper
; list and every audio/disk/sprite/bitplane block's CONTENT is preloaded
; by the C++ test through poke_word/poke_long before boot() -- gary.v's
; DMA-side address decode (the `dbr` branch) carries no ovl term, so Agnus
; reaches chip block 0 ($000000-$07FFFF) regardless of the overlay, but a
; CPU write to the same range is blocked while ovl==1 (gary.v:159), and
; this package never clears the overlay (CIA-A is not touched anywhere in
; this package). A program only ever writes a block's ADDRESS into a
; pointer register, which is an ordinary custom-register write, unaffected
; by ovl either way.

; PROG_HEADER must be the very first thing after `org PROGRAM_ORG`: a
; fixed-size `bra.s` ahead of the level-3 (VERTB) autovector handler, so
; the handler always starts at PROGRAM_ORG+2 -- the address
; test_ch2_dma_cycles.cpp's kHandlerAddr hardcodes and pokes into the
; level-3 autovector slot ($6C) in the kickstart shadow
; (nanomig_harness.h's kKickShadowWordBase) before boot(), exactly where
; the harness itself writes the two reset vectors. The handler clears
; VERTB and counts the field; it does not touch a0 permanently across STOP
; (see S6_WAIT/S6_WAIT_DISK: the wait loop's own a0 is reloaded with
; CUSTOM right after every stop, not assumed to survive it).
; A local (dot-prefixed) label is scoped to the nearest PRECEDING global
; label in vasm; since this macro's own reference to its skip-target
; comes before any global label at all, and its definition comes after
; the global label `vblank_handler:`, a local label would name two
; different symbols and fail to resolve -- so the skip-target is a plain
; global label instead (ph_header_main\@, unique per invocation though
; this macro is only ever invoked once per program).
PROG_HEADER     macro
                bra.s   ph_header_main\@
vblank_handler:
                addq.w  #1,FRAME_CTR
                lea     CUSTOM,a0
                move.w  #INT_VERTB,INTREQ(a0)
                rte
ph_header_main\@:
                endm

; S0 -- quiesce. Leaves a0 = CUSTOM for the caller's own S1/S2 code.
S0_QUIESCE      macro
                lea     CUSTOM,a0
                move.w  #$7FFF,DMACON(a0)
                move.w  #$7FFF,INTENA(a0)
                move.w  #$7FFF,INTREQ(a0)
                move.w  #$7FFF,ADKCON(a0)
                clr.w   FRAME_CTR
                clr.b   DISK_FLAG
                endm

; S3 -- install the Copper list. \1 = list byte address. Caller must
; reload a0 = CUSTOM first (S1/S2 code may have used a0 for something
; else; every macro here re-states that requirement rather than assuming
; it).
S3_COPPER       macro
                move.w  #(\1)>>16,COP1LCH(a0)
                move.w  #(\1)&$FFFF,COP1LCL(a0)
                move.w  #0,COPJMP1(a0)
                endm

; S4 -- enable DMA. \1 = the run's extra DMACON bits (0 if none); the SET
; bit, DMAEN and COPEN are always included, a0 = CUSTOM.
S4_DMAEN        macro
                move.w  #$8000|DMAEN|COPEN|(\1),DMACON(a0)
                endm

; S5 -- arm the VERTB interrupt. a0 = CUSTOM. The vector itself is written
; by the C++ test before boot(), not by this program (see PROG_HEADER).
S5_ARM_INT      macro
                move.w  #INT_SET|INT_INTEN|INT_VERTB,INTENA(a0)
                endm

; S6 -- wait loop, no disk steps.
S6_WAIT         macro
                lea     CUSTOM,a0
.w6\@           stop    #$2000
                cmp.w   #7,FRAME_CTR
                bne.s   .w6\@
                endm

; S6 -- wait loop with the disk start sequence (p.247 steps 2-4) run once,
; at FRAME_CTR==2. \1 = the run's DSKLEN value for steps 3-4.
S6_WAIT_DISK    macro
                lea     CUSTOM,a0
.w6\@           stop    #$2000
                cmp.w   #2,FRAME_CTR
                bne.s   .w6skip\@
                tst.b   DISK_FLAG
                bne.s   .w6skip\@
                move.w  #$4000,DSKLEN(a0)
                move.w  #\1,DSKLEN(a0)
                move.w  #\1,DSKLEN(a0)
                move.w  DSKBYTR(a0),d0
                move.w  d0,RES_DSKBYTR
                move.b  #1,DISK_FLAG
.w6skip\@       cmp.w   #7,FRAME_CTR
                bne.s   .w6\@
                endm

; S7 -- finish, no disk. a0 = CUSTOM (S6 leaves it so).
S7_FINISH       macro
                move.w  #$7FFF,DMACON(a0)
                move.w  #$7FFF,INTENA(a0)
                DONE
                endm

; S7 -- finish, disk runs (p.247 step 5: DSKLEN<-$4000 first).
S7_FINISH_DISK  macro
                move.w  #$4000,DSKLEN(a0)
                move.w  #$7FFF,DMACON(a0)
                move.w  #$7FFF,INTENA(a0)
                DONE
                endm
