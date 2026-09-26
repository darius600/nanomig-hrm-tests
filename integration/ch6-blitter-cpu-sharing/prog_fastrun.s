; prog_fastrun.s -- ch6-done.c9 [p186]: "may be running completely
; uninhibited out of Fast memory or its internal cache and continue to
; have memory cycles."
;
; The blit is armed (BLTCON*, BLTAPTH/DPTH, DMAEN/BLTEN/BLTPRI) and the
; Copper's own list is built and pointed at (COP1LC), all from chip RAM,
; but the CPU never itself writes BLTSIZE -- the Copper does, once it
; runs. p.30: "you must initialize one of the Copper location registers
; (COP1LC or COP2LC) and write to its strobe address before Copper DMA is
; turned on. This ensures a known start address and known state." COPJMP1
; is therefore strobed here, from chip RAM, with COPEN still clear (the
; strobe only loads the Copper's own program counter from COP1LC; nothing
; runs until DMA is on); COPEN, set from fast RAM below, is the one event
; that starts the Copper running the already-loaded list. Enabling COPEN
; from fast RAM is the CPU's own deliberate, occasional chip-bus write
; while "running from Fast memory or cache" and "continu[ing] to have
; memory cycles" (p.186) -- everything else in the loop touches only fast
; RAM.
;
; This file alone (no symbol set) runs FR_ITERS=50 passes. prog_fastrun_
; long.s (FR_LONG set) runs FR_ITERS=2000: a loop long enough to still be
; running well after the blit (500 words, ~8112 clk_sys, the same known
; span as A4/ORDER's own nasty capture) has ended, so its own counter
; writes are observable both strictly inside the blit's active window and
; strictly after it -- test_blitter_cpu_sharing.cpp takes the 2000-
; iteration capture's own modal (most frequent) counter-write period on
; each side and compares the two periods for equality, the way A6
; compares its own in-window and no-DMA periods. Both loops reaching
; their own FR_ITERS (RESULTS equal to it, observed at DONE) is itself
; part of what "continue to have memory cycles" means here.
;
; prog_fastrun_chipcode.s (FR_CHIPCODE set) is this program's own
; MUTATION: only FR_CODE moves to chip RAM ($0C8000); FR_CNT stays in fast
; RAM. A4 (BLTPRI set denies the CPU every chip-bus cycle, fetches
; included, for the whole of the blit's active window) then predicts zero
; counter writes in that window, since the CPU cannot even fetch its own
; next loop instruction while the window holds.

        include "nanomig.i"

        ifd     FR_LONG
FR_ITERS        equ     2000
        else
FR_ITERS        equ     50
        endif

BLIT_SRC        equ     $0C2000         ; unused content: only the blit's own timing matters here
BLIT_DST        equ     $0C4000

FR_REG          equ     $E80048         ; Zorro II autoconfig "Register 0x48", ZII RAM (cpu_wrapper.v:640);
                                        ; not a manual register -- only how this harness capability is enabled
        ifd     FR_CHIPCODE
FR_CODE         equ     $0C8000         ; chip RAM: this program's own MUTATION target (see header)
        else
FR_CODE         equ     $200000         ; Zorro II fast RAM, bank 1 (cpu_wrapper.v:117, addr[23:21]=1)
        endif
FR_CNT          equ     $200100         ; the loop's own counter word, always in fast RAM

COPCON          equ     $02E
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088

COP_LIST        equ     $0C1000         ; the Copper's own list: one MOVE to BLTSIZE, then WAIT-forever
PRE_JUMP        equ     $0C0006         ; the CPU's own last chip-RAM write before it jumps into fast
                                        ; RAM -- a plain marker of the program's own flow, not a value
                                        ; test_blitter_cpu_sharing.cpp reads (custom-register writes
                                        ; such as COPJMP1 do not appear in ram_log(), which only
                                        ; samples the physical RAM port, so this word gives the
                                        ; program itself a real, checkable last-write-before-the-jump,
                                        ; even though nothing in this package currently reads it back)

        org     PROGRAM_ORG

start:
        ; Enable Zorro II fast RAM. The write's data value is not decoded
        ; (cpu_wrapper.v:640): only the strobe matters.
        move.w  #0,FR_REG.l

        ; Relocate the loop (below) into fast RAM. Entirely self-contained
        ; (its only branch is PC-relative), so it runs unchanged from
        ; wherever it ends up.
        lea     loop_template,a0
        lea     FR_CODE,a1
        move.w  #(loop_template_end-loop_template)/2-1,d1
.copy:  move.w  (a0)+,(a1)+
        dbra    d1,.copy

        clr.w   FR_CNT.l

        ; The blit itself: BLTPRI set (nasty), straight A-to-D, no shift.
        ; Everything except BLTSIZE -- the write that actually starts a
        ; blit -- so nothing here has begun transferring yet.
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #BLIT_SRC,BLTAPTH(a0)
        move.l  #BLIT_DST,BLTDPTH(a0)

        ; CDANG (COPCON bit 1, p.26): "protected by a bit in the Copper
        ; control register ... When CDANG is 0, these registers cannot be
        ; written by the Copper. When CDANG is 1, these registers can be
        ; written by the Copper" (registers $40-$7E). Appendix B (p.289)
        ; marks BLTSIZE ($058) `~058`; the legend (p.287): "Address not
        ; writable by the Copper unless the 'copper danger bit', COPCON is
        ; set true."
        move.w  #$0002,COPCON(a0)

        ; The Copper list: MOVE #$7D01,BLTSIZE (height=500,width=1 --
        ; A4/ORDER's own known ~8112 clk_sys span), then WAIT-forever.
        move.w  #BLTSIZE,COP_LIST.l
        move.w  #$7D01,COP_LIST+2.l
        move.w  #$FFFF,COP_LIST+4.l
        move.w  #$FFFE,COP_LIST+6.l

        move.l  #COP_LIST,d0
        move.w  d0,COP1LCL(a0)
        swap    d0
        move.w  d0,COP1LCH(a0)

        ; Arm DMAEN, BLTEN and BLTPRI only -- COPEN stays clear here: the
        ; blitter is primed but nothing can make it run until BLTSIZE is
        ; written, which only the Copper does, which only runs once COPEN
        ; is set below, from fast RAM.
        move.w  #$8640,DMACON(a0)      ; SET DMAEN,BLTEN,BLTPRI

        ; p.30's own start sequence (see the header comment): the strobe,
        ; from chip RAM, while COPEN is still clear.
        move.w  #0,COPJMP1(a0)

        move.w  #$FADE,PRE_JUMP.l      ; last chip-bus write before the jump (see PRE_JUMP above)

        lea     FR_CNT,a2
        move.w  #FR_ITERS-1,d0
        lea     back_from_fastram,a3    ; return address, loaded before the
                                        ; jump: an indirect JMP through a3
                                        ; cannot be collapsed by the
                                        ; assembler the way a direct jump to
                                        ; the following instruction can.
        jmp     FR_CODE

; ---- relocated into FR_CODE (see above): fast RAM normally, chip RAM
; under FR_CHIPCODE (this program's own MUTATION, see header). Touches
; a2/a3/d0 and FR_CNT, plus the one deliberate chip-bus write below that
; starts the Copper (and, through it, the blit).
loop_template:
        move.w  #$8080,DMACON(a0)      ; SET COPEN (a0 still holds CUSTOM,
                                        ; unchanged since the blit setup
                                        ; above): the one start event (see
                                        ; the header comment) -- the Copper
                                        ; now runs the list already loaded
                                        ; from COP1LC by the strobe above
.loop:  addq.w  #1,(a2)
        dbra    d0,.loop
        jmp     (a3)
loop_template_end:

back_from_fastram:
        move.w  FR_CNT.l,d1
        move.w  d1,RESULTS
        DONE
