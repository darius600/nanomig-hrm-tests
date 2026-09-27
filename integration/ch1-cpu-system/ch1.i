; ch1.i -- shared definitions for ch1-cpu-system's own programs. Not part
; of the harness contract: this package's own file (like ch6-blitter-
; geometry/geom.i, ch5-audio-output-scanrate/ch5regs.i). nanomig.i already
; defines PROGRAM_ORG, RESULTS, DONE_ADDR, DONE_VALUE, DONE, CUSTOM,
; DMACONR, INTENAR, DMACON, INTENA; this file adds only what ch1's own
; programs need.
;
; SPECIFICATION.md 2.2's addresses ($3FF0, $4000.., $8000+) are used
; literally, below, as absolute addresses -- but PROGRAM_ORG ($080400,
; nanomig_harness.h) is the harness's own fixed code-entry constant, not
; $8000 as 2.2 literally says; code starts at PROGRAM_ORG, comfortably
; above every data address below (2.2's own "the builder may shift it if
; the rules above still hold").
;
; Every program below clears the boot overlay (CLEAR_OVL) as its very
; first instruction, exactly as ch2-copper-control, ch4-shres-sprite-b,
; ch5-audio-output-scanrate and ch6-blitter-geometry already do: one CIA-A
; write is what makes $000000-$07FFFF ordinary, real, identity-mapped chip
; RAM at this boundary, needed for SPECIFICATION.md 2.2's literal
; addresses (including P1's own abs.W cases, 2.2: "P1's absolute-short
; cases need their operand below $8000", and P5's exception vectors,
; $000010 etc., 68000 instruction set).

; ---- common sentinels and results (2.2 table) ------------------------
S_A             equ     $4000           ; window-open sentinel (P1, P2, P5)
S_B             equ     $4002           ; window-close sentinel (P1, P2, P5)
S_START         equ     $4004           ; run start (all)
S_END           equ     $4006           ; run end (all)
S_FAIL          equ     $4008           ; failure path (all)
S_P             equ     $400A           ; progress sentinel (P3, P6)
S_L             equ     $400C           ; landing sentinel (P3, P6)
S_X             equ     $400E           ; guard (P3, P6; must never be written)
R_INT           equ     $4010           ; stored INTENAR
R_DMA           equ     $4012           ; stored DMACONR
SAVE_SP         equ     $4014           ; long: P2 saved stack pointer
PRE_SP          equ     $4018           ; long: P5 stack pointer before each exception
POST_SP1        equ     $4024           ; long: P5 stack pointer on handler entry, e=1
POST_SP2        equ     $4028
POST_SP3        equ     $402C
POST_SP4        equ     $4030
POST_SP5        equ     $4034

; ---- P1/P4: CLR operand table ($4100+16c) and readback ($4300+4c) ----
T_BASE          equ     $4100           ; T_c = T_BASE + 16*c
R_BASE          equ     $4300           ; R_c = R_BASE + 4*c  (c=1..21, 4 bytes each)
XTAB            equ     $4400           ; 21 longs: address of the CLR/MOVE instruction per case

; ---- P2: 32-bit register store table (2.2's $4400+4*index) -----------
M_BASE          equ     $4500           ; M_r = M_BASE + 4*index, D0=0 .. A7=15
M_USP           equ     $4540

; ---- P3: X_k table, 4 longs (block k's stop-instruction address) -----
XTAB3           equ     $4550
; ---- P6: X_k table, 8 longs (block k's stop-instruction address) -----
XTAB6           equ     $4570

; ---- stacks -----------------------------------------------------------
SSP_INIT        equ     $3FF0           ; explicit supervisor stack (2.2 rule 1)
USP_INIT        equ     $3E00           ; P5's user stack pointer (2.2)

; ---- far word Z (2.2): the highest word of 2 MB, never referenced -----
FARZ            equ     $1FFFFE

; ---- CIA-A: clears the boot overlay (see header) ----------------------
CIAAPRA         equ     $BFE001
CLEAR_OVL       macro
                move.b  #0,CIAAPRA
                endm

; ---- common setup sequence (2.3), steps 1-5 -------------------------
; MUT_C4 (contrast, C4): the S_START write is left out.
; MUT_C1 (contrast, C1): an extra INTENA write follows step 2.
; MUT_C2 (contrast, C2): an extra DMACON write follows step 4.
SETUP           macro
                CLEAR_OVL
                move.l  #SSP_INIT,a7
                ifnd    MUT_C4
                move.w  #$0001,S_START
                endc
                lea     CUSTOM,a0
                move.w  #$7FFF,INTENA(a0)
                ifd     MUT_C1
                move.w  #$C000,INTENA(a0)
                endc
                move.w  #$2700,sr
                move.w  #$07FF,DMACON(a0)
                ifd     MUT_C2
                move.w  #$8240,DMACON(a0)
                endc
                move.w  INTENAR(a0),R_INT
                move.w  DMACONR(a0),R_DMA
                endm

; ---- common ending: FAIL: writes S_FAIL then falls into S_END --------
; MUT_C3 (contrast, C3): a branch to FAIL is inserted just before the
; normal S_END write, in the program that uses this macro.
ENDING          macro
                ifd     MUT_C3
                bra.w   FAILRUN
                endc
                bra.w   REALEND
FAILRUN:
                move.w  #$FA11,S_FAIL
REALEND:
                move.w  #$E0D0,S_END
                DONE
                endm
