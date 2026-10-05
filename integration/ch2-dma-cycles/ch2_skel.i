; ch2_skel.i -- the S0-S7 program skeleton of SPECIFICATION.md section 2.6,
; factored into macros so every configuration's own prog_*.s only supplies
; what differs: S1's own chip-memory build calls (ch2build.i), S2's
; register writes, and S4's DMACON bits. Every block S1 writes sits below
; logical $080000 (ch2dma.i) and is reachable to a CPU write only once the
; boot overlay is released, which S0 does.

; CIA-A: PRA bit 0 is OVL (p.223: "The OVL bit in one of the 8520 Chips
; disables the overlay"; Appendix F p.339: pra at $BFE001, ddra at $BFE201,
; "1-output (set to 0x03)"). Every block this package writes below $080000
; is unreachable to a CPU write while the overlay covers it.
CIAAPRA         equ     $BFE001
CIAADDRA        equ     $BFE201

CLEAR_OVL       macro
                move.b  #$03,CIAADDRA
                move.b  #0,CIAAPRA
                endm

; Writes the level-3 (VERTB) autovector, address $6C, by an ordinary CPU
; write -- only reachable once CLEAR_OVL has run. vblank_handler is a
; forward reference to the VBLANK_HANDLER macro's own label (every program
; invokes that macro once, after its own S7 finish).
INSTALL_VECT    macro
                move.l  #vblank_handler,$6C.w
                endm

; S0 -- quiesce, and release the boot overlay: every block S1 writes (and
; the level-3 vector S0 installs here) sits below $080000. Leaves a0 =
; CUSTOM for the caller's own S1/S2 code.
S0_QUIESCE      macro
                CLEAR_OVL
                lea     CUSTOM,a0
                move.w  #$7FFF,DMACON(a0)
                move.w  #$7FFF,INTENA(a0)
                move.w  #$7FFF,INTREQ(a0)
                move.w  #$7FFF,ADKCON(a0)
                clr.w   FRAME_CTR
                clr.b   DISK_FLAG
                INSTALL_VECT
                endm

; S3 -- install the Copper list. \1 = list byte address, written as one
; long word (p.25: "you write the 18-bit address by moving one long word
; to the register whose name ends in H" -- COP1LCH/COP1LCL are adjacent).
; Caller must reload a0 = CUSTOM first (S1/S2 code may have used a0 for
; something else).
S3_COPPER       macro
                move.l  #(\1),COP1LCH(a0)
                move.w  #0,COPJMP1(a0)
                endm

; S4 -- enable DMA. \1 = the run's extra DMACON bits (0 if none); the SET
; bit, DMAEN and COPEN are always included, a0 = CUSTOM.
S4_DMAEN        macro
                move.w  #$8000|DMAEN|COPEN|(\1),DMACON(a0)
                endm

; S5 -- arm the VERTB interrupt. a0 = CUSTOM. The vector itself is written
; by S0_QUIESCE (INSTALL_VECT), once the overlay is clear.
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

; S6 -- as S6_WAIT_DISK, but the CPU never stops: it branches to itself
; instead (T2's own contrast, SPECIFICATION.md P7.6). \1 = the run's
; DSKLEN value. Interrupts must be unmasked explicitly once, since STOP
; #$2000's own unmasking never runs here.
S6_LOOP_DISK    macro
                lea     CUSTOM,a0
                move.w  #$2000,sr
.w6\@           cmp.w   #2,FRAME_CTR
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

; S6 -- C1's own contrast (SPECIFICATION.md 2.6): the wait loop's exit
; condition is removed, so FRAME_CTR never gates an exit and DONE is never
; reached within the run's cycle budget -- the completion word is missing.
S6_WAIT_NOEXIT  macro
                lea     CUSTOM,a0
.w6\@           stop    #$2000
                bra.s   .w6\@
                endm

; S7 -- finish, no disk. DMACON first, then INTENA (2.6's own order). a0 =
; CUSTOM (S6 leaves it so).
S7_FINISH       macro
                move.w  #$7FFF,DMACON(a0)
                move.w  #$7FFF,INTENA(a0)
                DONE
                endm

; S7 -- finish, disk runs. DMACON first, then DSKLEN <- $4000 (p.247 step
; 5), then INTENA -- SPECIFICATION.md 2.6's own order.
S7_FINISH_DISK  macro
                move.w  #$7FFF,DMACON(a0)
                move.w  #$4000,DSKLEN(a0)
                move.w  #$7FFF,INTENA(a0)
                DONE
                endm

; The level-3 handler. Every program invokes this macro exactly once, after
; its own S7 finish (DONE never falls through, so the handler's own code
; never executes as mainline instructions -- see nanomig.i's DONE). It
; clobbers a0 unconditionally; every S6 loop above reloads a0 = CUSTOM
; right after every stop/branch, never assuming it survives an interrupt.
VBLANK_HANDLER  macro
vblank_handler:
                addq.w  #1,FRAME_CTR
                lea     CUSTOM,a0
                move.w  #INT_VERTB,INTREQ(a0)
                rte
                endm
