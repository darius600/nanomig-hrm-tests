; prog_fastrun_lockout.s -- 6-blitter-done-flag.c8 [p186]: "If a blit has
; just been started but has been locked out of memory access because of,
; for instance, display fetches ... The processor, on the other hand, may
; be running completely uninhibited out of Fast memory or its internal
; cache, so it will continue to have memory cycles."
;
; Unlike prog_fastrun.s (a nasty blit with no display, where the blitter
; itself holds the bus and the CPU is the one locked out), this stimulus
; reaches the page's own case: a four-high-resolution-bitplane display
; (prog_bitplane_hires.s's own setup) whose fetch windows claim every one
; of their 160 slots (A10PRE: p.197's "40 * 4 = 160 time slots", none left
; over) -- p.197 itself: "This effectively locks out the 68000 (as well as
; the blitter or Copper)". A blit is started by the Copper, from a single
; MOVE to BLTSIZE timed by a preceding WAIT to just before a fetch window,
; so the blit cannot get its first DMA cycle until the window ends. The
; counting loop runs in fast RAM throughout, started before either the
; display or the blit, so it is already running when the window locks the
; blitter out; its own writes (not on the physical RAM port, see
; prog_fastrun.s's own header) are unaffected by any of it.
;
; The CPU first spin-reads VHPOSR from chip RAM (a plain register read,
; not a DMA channel) until the beam's own line (VHPOSR's high byte) is
; well inside the display's own active vertical area (DIWSTRT/DIWSTOP
; below put it at line $2C-ish to the low $C0s; $32-$C8 is comfortably
; inside that) -- establishing a known line, LINES_AHEAD lines short of
; where the Copper's own WAIT (built right after, from that same known
; line) is targeted, so the WAIT's own condition cannot already be true
; the moment the Copper first checks it. p.23: "the largest number that
; will ever appear in the comparison is $FFE2", and "as soon as the line
; number becomes higher than the entered number, the comparison will
; evaluate to true" -- {VP,HP} is compared as one concatenated value, so a
; VP target strictly ahead of any VP already reached holds the WAIT until
; that exact future line, and only then does the HP field's own
; comparison take over -- releasing precisely at that line's own HP=$28,
; not at some arbitrary position already passed by the time the Copper
; was started. (VE=$00 would mask the vertical field, leaving a same-line
; "already true" case unresolved whenever the Copper happens to start
; past the HP target on whatever line it is already on; a fully compared
; VP field removes it.)
;
; The WAIT's own HP target is written in the page's own notation, not an
; RTL-internal one. p.22: IR1 "Bits 7 - 1 Horizontal beam position (called
; HP)". p.23: "The horizontal beam position has a value of $0 to $E2 ...
; The least significant bit is not used in the comparison". p.60: "the
; hardware resolution of display window start and stop is twice the
; hardware resolution of data fetch" -- DDFSTRT and the WAIT's own HP
; field are the same color-clock unit (Figure 6-9 draws data-fetch start
; on that same axis). HP=$28 (40) is therefore 20 color clocks before
; DDFSTRT=$3C (60): measured, the Copper's first fetch after the WAIT
; lands exactly 20 slots before the fetch window that follows -- wider
; than the blitter's own natural first-access latency (measured ~6 color
; clocks after the Copper's own data-word fetch, on an otherwise identical
; but unobstructed capture), so a blit armed right there would still get
; its first access before the window opens. Four MOVEs to COLOR00 ($180,
; a harmless, otherwise-unused write) between the WAIT and the real MOVE
; to BLTSIZE close most of that margin: Figure 6-9's own note gives each
; Copper Data Move instruction 4 slots, so four of them consume 16 color
; clocks, leaving 2 -- inside the blitter's own natural latency, so its
; first attempt lands after the window has already claimed the bus
; (measured: the blitter's first successful access follows the window's
; own end, not before its start). The number of fill MOVEs (four) was
; chosen on this RTL to land BLTSIZE there; FASTRUN-LOCKOUT-PRE, below, is
; what checks the placement, not this comment.
;
; The Copper's own write to BLTSIZE (a custom register) is not itself on
; the physical RAM port, the same as COPJMP1 (see PRE_JUMP's own comment
; in prog_fastrun.s); the Copper's own fetch of the MOVE's data word (a
; plain chip-RAM read, at COP_LIST+22 below, test_blitter_cpu_sharing.cpp's
; own kFrCopDataWord) is, and bounds the BLTSIZE write from below by at
; most the Copper's own single-slot MOVE-execution latency. The blitter's
; own first DMA access (its first A-read, in BLIT_SRC's range, or its
; first D-write if that happens to be logged first) is a physical
; RAM-port access throughout, so the interval between that access and the
; Copper's own data-word fetch is what test_blitter_cpu_sharing.cpp
; actually measures.
;
; FR_ITERS is large: the Copper's own WAIT does not release until
; LINES_AHEAD lines after the one the spin loop confirms (chosen large
; enough -- see LINES_AHEAD below -- that the WAIT is reliably still
; comparing against a steady-state line, not one still warming up right
; after the display DMA enable a few instructions earlier), which is tens
; of thousands of clk_sys away; the counting loop must still be running
; when the Copper's own MOVE and the blit's own lockout interval actually
; happen, not have already finished and returned.
;
; This file alone (no symbol set) runs the loop in fast RAM ($200000,
; FR_CODE below), with four high-resolution bitplanes (BPLCON0=$C000).
; prog_fastrun_lockout_chipcode.s (FR_CHIPCODE set) moves only FR_CODE to
; chip RAM ($0C0800; FR_CNT stays in fast RAM) -- this program's own
; MUTATION: the window is entirely DMA (A10PRE: p.197's own "40 * 4 = 160
; time slots", none left over), so no CPU access is possible anywhere
; strictly inside it, and the counter-write count graded there (see
; test_blitter_cpu_sharing.cpp) predicts zero.
;
; prog_fastrun_lockout_2plane.s (FR_2PLANE set) instead keeps the loop in
; fast RAM but reduces the display to two hi-res bitplanes (BPLCON0=$A000,
; same DDFSTRT/DDFSTOP): the window then holds 80 bitplane fetches instead
; of 160 (Figure 6-9's own double dagger, "These operations only take
; slots if the associated operation is being performed"; p.197 gives the
; full lockout only for four hi-res planes, "40 * 4 = 160 time slots").
; Freeing half the window's slots lets the blitter's own first access fall
; inside it instead of after it -- this program's own precondition
; MUTATION (FASTRUN-LOCKOUT-PRE, below): the precondition goes red.

        include "nanomig.i"

FR_ITERS        equ     8000            ; large enough to still be running well after the Copper's own
                                        ; WAIT (LINES_AHEAD lines out) releases and the blit's own
                                        ; lockout interval passes -- see the header comment

BLIT_SRC        equ     $0C2000         ; unused content: only the blit's own timing matters here
BLIT_DST        equ     $0C4000         ; same range test_blitter_cpu_sharing.cpp's kShareDstLo/Hi use

FR_REG          equ     $E80048         ; Zorro II autoconfig "Register 0x48", ZII RAM (cpu_wrapper.v:640)
        ifd     FR_CHIPCODE
FR_CODE         equ     $0C0800         ; chip RAM: this program's own MUTATION target (see header).
                                        ; Not $0C8000 (prog_fastrun.s's own choice): that address falls
                                        ; inside BP_PTR's own $0C6000-$0D6000 heuristic range, which
                                        ; test_blitter_cpu_sharing.cpp's fetch_windows() reads from --
                                        ; the CPU's own instruction fetches there would then be
                                        ; miscounted as bitplane-DMA reads and corrupt this capture's
                                        ; own window detection. $0C0800 sits between RESULTS ($0C0000)
                                        ; and COP_LIST ($0C1000), clear of every other range this
                                        ; program or the test uses.
        else
FR_CODE         equ     $200000         ; Zorro II fast RAM, bank 1 (cpu_wrapper.v:117, addr[23:21]=1)
        endif
FR_CNT          equ     $200100         ; the loop's own counter word, always in fast RAM

COPCON          equ     $02E
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088

BPLCON0         equ     $100
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPL1MOD         equ     $108
BPL2MOD         equ     $10A
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
BP_PTR          equ     $0C6000         ; prog_bitplane.s's own convention: BPL2-4 pointers left unset

COP_LIST        equ     $0C1000         ; WAIT (VP,HP=$28 both compared), MOVE BLTSIZE, WAIT-forever
PRE_JUMP        equ     $0C0006         ; last chip-RAM write before the jump (see prog_fastrun.s)

WAIT_HP         equ     $28             ; page's own notation: the same color-clock unit as DDFSTRT
                                        ; (p.60); see the header comment
WAIT_HP_FIELD   equ     (WAIT_HP|1)     ; HP with bit0 forced to 1 (the WAIT marker, IR1 bit 0),
                                        ; precomputed to keep the operand below a plain immediate
LINES_AHEAD     equ     20              ; the WAIT's own VP target: this many lines past the one the
                                        ; spin loop confirms, so the WAIT cannot already be satisfied

        org     PROGRAM_ORG

start:
        ; Enable Zorro II fast RAM (harmless under FR_CHIPCODE too: FR_CNT
        ; still needs it).
        move.w  #0,FR_REG.l

        ; Relocate the loop (below) into FR_CODE -- fast RAM normally,
        ; chip RAM under FR_CHIPCODE.
        lea     loop_template,a0
        lea     FR_CODE,a1
        move.w  #(loop_template_end-loop_template)/2-1,d1
.copy:  move.w  (a0)+,(a1)+
        dbra    d1,.copy

        clr.w   FR_CNT.l

        lea     CUSTOM,a0

        ; High-resolution bitplanes (prog_bitplane_hires.s's own values,
        ; Chapter 3's standard display-window/fetch-window constants): four
        ; of them (BPLCON0=$C000, below) make every one of the resulting
        ; 160-slot-per-line fetch windows full DMA, none left over
        ; (A10PRE); FR_2PLANE's own two (BPLCON0=$A000) leave half of each
        ; window's 80 slots free (see the header comment). BPL2-4's own
        ; pointers are left unset, as in prog_bitplane.s.
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0000,BPL1MOD(a0)
        move.w  #$0000,BPL2MOD(a0)
        move.l  #BP_PTR,BPL1PTH(a0)
        move.w  #$003C,DDFSTRT(a0)
        move.w  #$00D4,DDFSTOP(a0)
        ifd     FR_2PLANE
        move.w  #$A000,BPLCON0(a0)     ; HIRES, 2 bitplanes -- this program's own
                                        ; precondition MUTATION (see header)
        else
        move.w  #$C000,BPLCON0(a0)     ; HIRES, 4 bitplanes
        endif
        move.w  #$8300,DMACON(a0)      ; SET DMAEN,BPLEN -- display starts fetching now

        ; The blit itself: BLTPRI set (nasty), the same 500-word A-to-D
        ; span A4/ORDER establish. Everything except BLTSIZE -- the
        ; Copper's own MOVE below writes that.
        move.w  #$09F0,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #BLIT_SRC,BLTAPTH(a0)
        move.l  #BLIT_DST,BLTDPTH(a0)
        move.w  #$8640,DMACON(a0)      ; SET DMAEN,BLTEN,BLTPRI -- primed, but
                                        ; BLTSIZE is unwritten, so nothing
                                        ; transfers until the Copper's own
                                        ; MOVE below runs.

        ; CDANG (COPCON bit 1, p.26): "protected by a bit in the Copper
        ; control register ... When CDANG is 0, these registers cannot be
        ; written by the Copper. When CDANG is 1, these registers can be
        ; written by the Copper" (registers $40-$7E). Appendix B (p.289)
        ; marks BLTSIZE ($058) `~058`; the legend (p.287): "Address not
        ; writable by the Copper unless the 'copper danger bit', COPCON is
        ; set true."
        move.w  #$0002,COPCON(a0)

        ; Spin until the beam's own line is well inside the display's own
        ; active vertical area (see the header comment) -- VHPOSR(a0), a
        ; plain register read, costs the CPU nothing the display DMA
        ; would otherwise use. d1 keeps the confirmed line afterwards, so
        ; the Copper list built right below can target a line strictly
        ; ahead of it.
.spin:  move.w  VHPOSR(a0),d1
        lsr.w   #8,d1
        cmp.w   #$32,d1
        blt.s   .spin
        cmp.w   #$C8,d1
        bgt.s   .spin

        ; The Copper list (see the header comment): a WAIT whose own VP
        ; target is LINES_AHEAD past the line the spin above just
        ; confirmed (so the {VP,HP} comparison, taken as one concatenated
        ; value (p.23), cannot already be true the moment the Copper first
        ; checks it -- this also delays the HP half until that specific
        ; future line), HP=WAIT_HP, both fields fully compared (IR2=$FFFE); then
        ; MOVE #$7D01,BLTSIZE (height=500,width=1); then WAIT-forever.
        add.w   #LINES_AHEAD,d1
        lsl.w   #8,d1
        or.w    #WAIT_HP_FIELD,d1
        move.w  d1,COP_LIST.l           ; WAIT IR1: VP=target,HP=WAIT_HP,bit0=1
        move.w  #$FFFE,COP_LIST+2.l     ; WAIT IR2: BFD=1,VE=$7F,HE=$7F (both fields fully compared)
        ; Four MOVEs to COLOR00 close most of the WAIT's own measured
        ; margin before DDFSTRT (see the header comment).
        move.w  #COLOR00,COP_LIST+4.l
        move.w  #0,COP_LIST+6.l
        move.w  #COLOR00,COP_LIST+8.l
        move.w  #0,COP_LIST+10.l
        move.w  #COLOR00,COP_LIST+12.l
        move.w  #0,COP_LIST+14.l
        move.w  #COLOR00,COP_LIST+16.l
        move.w  #0,COP_LIST+18.l
        move.w  #BLTSIZE,COP_LIST+20.l
        move.w  #$7D01,COP_LIST+22.l     ; the MOVE's data word: kFrCopDataWord = COP_LIST+22
        move.w  #$FFFF,COP_LIST+24.l
        move.w  #$FFFE,COP_LIST+26.l

        move.l  #COP_LIST,d0
        move.w  d0,COP1LCL(a0)
        swap    d0
        move.w  d0,COP1LCH(a0)

        ; p.30's own start sequence: strobe COPJMP1 (loads the Copper's PC
        ; from COP1LC) while COPEN is still clear; COPEN itself, set from
        ; fast RAM below, is then the one event that starts it running --
        ; deferred there (not issued here in chip RAM) so the fast-RAM
        ; loop is already running, and the WAIT's own HP target already
        ; reached on some line, before the Copper is allowed to move at
        ; all: otherwise the whole WAIT-then-MOVE-then-lockout sequence
        ; could run to completion while the CPU is still here in chip RAM
        ; finishing its own setup, before the loop below ever starts.
        move.w  #0,COPJMP1(a0)

        move.w  #$FADE,PRE_JUMP.l      ; last chip-bus write before the jump

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
; starts the Copper (and, through it, the blit) -- only after the loop
; below is already running (see the COPJMP1 comment above).
loop_template:
        move.w  #$8080,DMACON(a0)      ; SET COPEN (a0 still holds CUSTOM,
                                        ; unchanged since the display/blit
                                        ; setup above): the one start event
                                        ; -- the Copper now runs the list
                                        ; already loaded from COP1LC by the
                                        ; strobe above
.loop:  addq.w  #1,(a2)
        dbra    d0,.loop
        jmp     (a3)
loop_template_end:

back_from_fastram:
        move.w  FR_CNT.l,d1
        move.w  d1,RESULTS
        DONE
