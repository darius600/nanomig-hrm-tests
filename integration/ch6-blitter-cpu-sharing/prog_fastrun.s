; prog_fastrun.s -- 6-blitter-done-flag.c8,ch6-done.c9 [p186]: "A processor
; running from Fast memory or cache can continue executing while the
; blitter is denied Chip-memory access" / "may be running completely
; uninhibited out of Fast memory or its internal cache and continue to
; have memory cycles."
;
; The blit is armed (BLTCON*, BLTAPTH/DPTH, DMAEN/BLTEN/BLTPRI) and the
; Copper's own list is built and pointed at (COP1LC), all from chip RAM,
; but the CPU never itself writes BLTSIZE -- the Copper does, once it
; runs. COPEN and the COPJMP1 strobe that starts the Copper running are
; both deferred to fast RAM, not issued before the jump: with COP1LC
; already pointed at the list, enabling COPEN from chip RAM risks the
; Copper reloading its own instruction pointer from COP1LC on its own, at
; the next vertical blank, independently of any CPU strobe -- and a nasty
; blit (BLTPRI set, A4: locks the CPU out of the chip bus for its whole
; active window, instruction fetches included) started that way, while
; the CPU is still in chip RAM reaching for the jump, would stall the CPU
; there until the blit's own end, defeating the point. Enabling COPEN and
; strobing COPJMP1 from fast RAM are the CPU's own deliberate, occasional
; chip-bus writes while "running from Fast memory or cache" and
; "continu[ing] to have memory cycles" (p.186) -- everything after them,
; the counting loop itself, touches only fast RAM.
;
; This file alone (no symbol set) runs FR_ITERS=50 passes.
; prog_fastrun_long.s (FR_LONG set) runs FR_ITERS=2000: a loop long
; enough to still be running well after the blit (500 words, ~8112
; clk_sys, the same known span as A4/ORDER's own nasty capture) has
; ended, so the two captures' own elapsed spans (the CPU's last chip-bus
; write before the jump, to the RESULTS store after the loop) must differ
; -- both loops reaching their own FR_ITERS is what "continue to have
; memory cycles" means here; identical spans regardless of FR_ITERS would
; instead mean the loop never ran until the blit itself released the bus.

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
FR_CODE         equ     $200000         ; Zorro II fast RAM, bank 1 (cpu_wrapper.v:117, addr[23:21]=1)
FR_CNT          equ     $200100         ; the loop's own counter word, in fast RAM

COPCON          equ     $02E
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088

COP_LIST        equ     $0C1000         ; the Copper's own list: one MOVE to BLTSIZE, then WAIT-forever
PRE_JUMP        equ     $0C0006         ; a real chip-RAM write: the test's own reference point for
                                        ; "the CPU's last chip-bus access before it jumps into fast RAM"
                                        ; (custom-register writes such as COPJMP1 do not appear in
                                        ; ram_log(), which only samples the physical RAM port)

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

        ; CDANG (COPCON bit 1): without it, the Copper may only reach
        ; $080-$1FE (agnus_copper.v's own "illegal register access" check);
        ; BLTSIZE ($058) is below that, so the Copper's own MOVE to it
        ; would otherwise halt the Copper instead of starting the blit.
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

        ; Arm DMAEN, BLTEN and BLTPRI only -- COPEN stays clear here, so
        ; the Copper cannot reload from COP1LC on its own yet (no vertical
        ; blank can trigger it): the blitter is primed but nothing can
        ; make it run until BLTSIZE is written, which only the Copper
        ; does, which only runs once COPEN is set below, from fast RAM.
        move.w  #$8640,DMACON(a0)      ; SET DMAEN,BLTEN,BLTPRI

        move.w  #$FADE,PRE_JUMP.l      ; last chip-bus write before the jump (see PRE_JUMP above)

        lea     FR_CNT,a2
        move.w  #FR_ITERS-1,d0
        lea     back_from_fastram,a3    ; return address, loaded before the
                                        ; jump: an indirect JMP through a3
                                        ; cannot be collapsed by the
                                        ; assembler the way a direct jump to
                                        ; the following instruction can.
        jmp     FR_CODE

; ---- relocated into FR_CODE (see above). Touches a2/a3/d0 and fast RAM,
; plus the two deliberate chip-bus writes below that start the blit.
loop_template:
        move.w  #$8080,DMACON(a0)      ; SET COPEN, from fast RAM (a0 still
                                        ; holds CUSTOM, unchanged since the
                                        ; blit setup above): only now can
                                        ; the Copper reload from COP1LC
        move.w  #0,COPJMP1(a0)         ; strobe now: starts the already-
                                        ; armed blit without costing the
                                        ; CPU an instruction fetch in chip RAM
.loop:  addq.w  #1,(a2)
        dbra    d0,.loop
        jmp     (a3)
loop_template_end:

back_from_fastram:
        move.w  FR_CNT.l,d1
        move.w  d1,RESULTS
        DONE
