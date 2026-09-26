; prog_fastrun.s -- 6-blitter-done-flag.c8,ch6-done.c9 [p186]: "A processor
; running from Fast memory or cache can continue executing while the
; blitter is denied Chip-memory access" / "may be running completely
; uninhibited out of Fast memory or its internal cache and continue to
; have memory cycles."
;
; The nasty blit (BLTPRI set, A4: locks the CPU out of the chip bus for
; its whole active window) is started by the Copper's own list, never by
; a CPU chip-bus write -- the Copper is its own DMA channel, so starting
; the blit needs no CPU chip-bus access at all, and the CPU's own last
; chip-bus write before it jumps into Zorro II fast RAM (cpu_wrapper.v:
; 117 sel_z2ram, enabled by one register write at $E80048,
; cpu_wrapper.v:640) is unaffected by the blit that only begins slightly
; afterward. The loop that then runs in fast RAM touches only a fast-RAM
; counter and its own registers -- no chip-bus access at all -- until it
; is done, when it stores the counter to RESULTS.
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

        ; Arm DMAEN, BLTEN, BLTPRI and COPEN together, then strobe
        ; COPJMP1 -- the last chip-bus write this program makes before
        ; jumping into fast RAM. The Copper, not the CPU, writes BLTSIZE,
        ; so nothing here needs the CPU to touch the chip bus again.
        move.w  #$86C0,DMACON(a0)      ; SET DMAEN,BLTEN,BLTPRI,COPEN
        move.w  #0,COPJMP1(a0)

        move.w  #$FADE,PRE_JUMP.l      ; last chip-bus write before the jump (see PRE_JUMP above)

        lea     FR_CNT,a2
        move.w  #FR_ITERS-1,d0
        jmp     FR_CODE

; ---- relocated into FR_CODE (see above). Touches only a2/d0 and fast
; RAM -- no chip-bus access until the final JMP back.
loop_template:
.loop:  addq.w  #1,(a2)
        dbra    d0,.loop
        jmp     back_from_fastram
loop_template_end:

back_from_fastram:
        move.w  FR_CNT.l,d1
        move.w  d1,RESULTS
        DONE
