; s7r.i -- what every program of aa-integ-s7-rest shares (SPECIFICATION.md
; sections 2 to 4): addresses, register offsets from the AA address list
; (pp. 6-10), the common prologue, the vertical-blank wait, the R1 marker loop
; and the common epilogue. A program is a wrapper that sets symbols and
; includes its family's source.

        include "nanomig.i"

; ---- memory layout (SPECIFICATION.md section 2) ----------------------------
Z               equ     $090000         ; zero block, 4096 bytes
L               equ     $091000         ; Copper list, 12 bytes
SDREC           equ     $0C0000         ; setup-done record (MK - 4)
MK              equ     $0C0004         ; markers
BUF             equ     $0C0400         ; sample buffer, 64 words
FIXTURE         equ     $0C0800         ; known constant, stored before anything reads one; FIXTURE + 2: end of prologue step 1
; DONE_ADDR ($0C0FFE) and PROGRAM_STACK come from nanomig.i

; ---- custom registers (AA pp. 6-10, offsets from CUSTOM) --------------------
REFPTR          equ     $028
COPCON          equ     $02E
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
BPL1PTH         equ     $0E0
SPR0PTH         equ     $120
BPLCON0         equ     $100
HTOTAL          equ     $1C0
BEAMCON0        equ     $1DC
SPRHSTRT        equ     $1D0
SPRHSTOP        equ     $1D2
BPLHSTRT        equ     $1D4
BPLHSTOP        equ     $1D6
HHPOSW          equ     $1D8
HHPOSR          equ     $1DA
BPLHMOD         equ     $1E6
SPRHPTH         equ     $1E8
SPRHPTL         equ     $1EA
BPLHPTH         equ     $1EC
BPLHPTL         equ     $1EE
VTOTAL          equ     $1C8

; ---- common prologue, step 1 (SPECIFICATION.md section 2) --------------------
; The fixture constant is stored first; then DMACON, INTENA and INTREQ are
; cleared (bit 15 = 0), BPLCON0 and COPCON are 0, and BEAMCON0 = $0020
; (PAL, every other bit 0). The word 1 stored at FIXTURE + 2 marks the end of step 1.
PROLOGUE        macro
                ifnd    NOFIX
                move.w  #$BEEF,FIXTURE
                endc
                lea     CUSTOM,a0
                move.w  #$7FFF,DMACON(a0)
                move.w  #$7FFF,INTENA(a0)
                move.w  #$7FFF,INTREQ(a0)
                move.w  #$0000,BPLCON0(a0)
                move.w  #$0000,COPCON(a0)
                move.w  #$0020,BEAMCON0(a0)
                move.w  #$0001,FIXTURE+2
                endm

; ---- wait for the start of the vertical blank (prologue step 3) --------------
; INTREQ <- $0020 clears VERTB; INTREQR bit 5 is then polled until it is 1.
VBWAIT          macro
                move.w  #$0020,INTREQ(a0)
.vb\@           move.w  INTREQR(a0),d0
                btst    #5,d0
                beq.s   .vb\@
                endm

; ---- common epilogue --------------------------------------------------------
; DMACON <- $7FFF, BPLCON0 <- 0, then DONE (stores DONE_VALUE at DONE_ADDR).
; NOEND leaves the DONE store out and hangs.
EPILOGUE        macro
                move.w  #$7FFF,DMACON(a0)
                move.w  #$0000,BPLCON0(a0)
                ifd     NOEND
.hang\@         bra.s   .hang\@
                else
                DONE
                endc
                endm

; ---- R1 marker loop (SPECIFICATION.md section 3) ------------------------------
; \1 = Vlo, \2 = Vhi, \3 = V step (1), \4 = gate low byte of H (0x60),
; \5 = gate width (0x30). For V = Vlo..Vhi: sample VHPOSR until its low byte
; lies in [gate, gate + width); read VPOSR; when the 11-bit line number equals V,
; store v, then h, at MK + 4*(V - Vlo). With U_RWV defined, the UHRES sprite pointer is
; written again (U_SPH, U_SPL) right after the marker of line U_RWV. With U_PAD defined, a delay of
; U_PAD passes of DBRA follows the gate test (the pass that finds a marker takes longer).
; With U_NOHEAD defined, a NOP follows the loop's first instruction. With U_WALK defined, the four UHRES
; pointer registers are written from the table at A4 (BPLHPTH, BPLHPTL, SPRHPTH, SPRHPTL of the next
; case) after the marker store of each line V with U_WLO <= V < U_WHI.
MARKLOOP        macro
                lea     $DFF006,a2
                lea     $DFF004,a3
                lea     MK,a1
                moveq   #(\4)-(((\4)&$80)<<1),d5
                moveq   #\5,d6
                move.w  #\1,d3
.rep\@          move.w  (a2),d0
                ifd     U_NOHEAD
                nop
                endc
                move.b  d0,d1
                sub.b   d5,d1
                cmp.b   d6,d1
                bhs.s   .rep\@
                ifd     U_PAD
                moveq   #U_PAD,d7
.pad\@          dbra    d7,.pad\@
                endc
                move.w  (a3),d2
                move.w  d2,d4
                and.w   #7,d4
                lsl.w   #8,d4
                move.w  d0,d1
                lsr.w   #8,d1
                or.w    d1,d4
                cmp.w   d3,d4
                bne.s   .rep\@
                move.w  d2,(a1)
                move.w  d0,2(a1)
                ifd     U_RWV
                cmp.w   #U_RWV,d3
                bne.s   .nrw\@
                move.w  #U_SPH,SPRHPTH(a0)
                move.w  #U_SPL,SPRHPTL(a0)
.nrw\@
                endc
                ifd     U_WALK
                cmp.w   #U_WLO,d3
                blo.s   .nw\@
                cmp.w   #U_WHI,d3
                bhs.s   .nw\@
                move.w  (a4)+,BPLHPTH(a0)
                move.w  (a4)+,BPLHPTL(a0)
                move.w  (a4)+,SPRHPTH(a0)
                move.w  (a4)+,SPRHPTL(a0)
.nw\@
                endc
                addq.l  #4*\3,a1
                addq.w  #\3,d3
                cmp.w   #\2,d3
                bls.s   .rep\@
                endm

; Setup-done record: VPOSR, then VHPOSR, at MK - 4, after the last setup write.
SETUPDONE       macro
                move.w  VPOSR(a0),SDREC
                move.w  VHPOSR(a0),SDREC+2
                endm
