; ciaregs.i -- shared definitions for appF-cia-system's own programs. Not
; part of the harness contract: this package's own file (like
; ch1-cpu-system/ch1.i, ch6-blitter-geometry/geom.i). nanomig.i already
; defines PROGRAM_ORG, RESULTS, DONE_ADDR, DONE_VALUE, DONE, CUSTOM,
; DMACON, INTENA; this file adds the 8520 CIA register addresses and
; control-register bit numbers from SPECIFICATION.md 1.1 (the printed
; Chip Register Map, p340/341/343), which nanomig.i does not define.

; ---- CIA register addresses (byte access only, SPECIFICATION.md 1.1) --
CIAA_PRB        equ     $BFE101
CIAA_DDRB       equ     $BFE301
CIAA_TALO       equ     $BFE401
CIAA_TAHI       equ     $BFE501
CIAA_TBLO       equ     $BFE601
CIAA_TBHI       equ     $BFE701
CIAA_TODLO      equ     $BFE801
CIAA_TODMID     equ     $BFE901
CIAA_TODHI      equ     $BFEA01
CIAA_SDR        equ     $BFEC01
CIAA_ICR        equ     $BFED01
CIAA_CRA        equ     $BFEE01
CIAA_CRB        equ     $BFEF01

CIAB_PRB        equ     $BFD100
CIAB_DDRB       equ     $BFD300
CIAB_TALO       equ     $BFD400
CIAB_TAHI       equ     $BFD500
CIAB_TBLO       equ     $BFD600
CIAB_TBHI       equ     $BFD700
CIAB_TODLO      equ     $BFD800
CIAB_TODMID     equ     $BFD900
CIAB_TODHI      equ     $BFDA00
CIAB_SDR        equ     $BFDC00
CIAB_ICR        equ     $BFDD00
CIAB_CRA        equ     $BFDE00
CIAB_CRB        equ     $BFDF00

; ---- control-register bit masks (SPECIFICATION.md 1.1 table) ----------
CR_START        equ     $01
CR_PBON         equ     $02
CR_OUTMODE      equ     $04
CR_RUNMODE      equ     $08
CR_LOAD         equ     $10
CRA_INMODE      equ     $20     ; CRA bit 5: 1=CNT, 0=phi2
CRA_SPMODE      equ     $40     ; CRA bit 6: 1=serial port output
CRB_INMODE_LO   equ     $20     ; CRB bit 5
CRB_INMODE_HI   equ     $40     ; CRB bit 6
CRB_ALARM       equ     $80     ; CRB bit 7

; ---- ICR bits (p347) ----------------------------------------------------
ICR_TA          equ     $01
ICR_TB          equ     $02
ICR_ALRM        equ     $04
ICR_SP          equ     $08
ICR_FLG         equ     $10
ICR_IR          equ     $80
ICR_CLEARMASK   equ     $7F     ; SETCLR=0: clears every mask bit named here

; ---- marker area ($0C1000-$0C1FFF, SPECIFICATION.md 1.2 E9) -----------
; A marker is a plain word store of a fixed sentinel to its own, otherwise
; unused address (E5): the test tells markers apart by address, not value.
; MK_START closes over environment setup (E1-E4, EP1's own reads) and
; opens the window EP2 grades; DONE's own store (nanomig.i) is always the
; run's last marker (E8).
MARKER_SENTINEL equ     $FACE
MK_START        equ     $0C1000

; ---- fixture constant (SPECIFICATION.md 1.2, FIXTURE) -------------------------
; A program-stored constant, delivered before anything else is graded: it
; proves the fixture can hand back a value the program itself wrote, so a
; check expecting $00 on a word nothing wrote cannot pass by coincidence.
FIXTURE_ADDR    equ     $0C1FFC
        ifnd    FIXTURE_VALUE
FIXTURE_VALUE   equ     $BEEF
        endc

; ---- FINISH: the program's last action. DONE (nanomig.i), or, when a
; wrapper defines MUT_EP3, a final marker with another value followed by a
; branch to itself, so the program completes its work without the final
; marker EP3 requires.
        ifd     MUT_EP3
FINISH          macro
                move.w  #$600E,DONE_ADDR
.fin\@:         bra.s   .fin\@
                endm
        else
FINISH          macro
                DONE
                endm
        endc

; ---- results area ($0C0000+, SPECIFICATION.md 1.2 E9) ------------------
; EP1's own four reads (CRA/CRB on both CIAs, made before any other write
; to them) are stored first, in every program that includes SETUP below,
; so every run's own EP1 precondition is checked from the same four words.
EP1_CRA_A       equ     RESULTS+0
EP1_CRB_A       equ     RESULTS+2
EP1_CRA_B       equ     RESULTS+4
EP1_CRB_B       equ     RESULTS+6

; ---- common environment (SPECIFICATION.md 1.2, E1-E4 and EP1) --------
; E1 is the harness's own doing (reset vectors point at PROGRAM_ORG).
; E2: SR<-$2700, DMACON<-$7FFF, INTENA<-$7FFF (bit15=0 clears, p278/p280).
; E3: for each CIA, ICR<-$7F (clears every mask bit) then one ICR read,
; discarded (clears the flags, p346/p347).
; E4: CIA-A PRA/DDRA and CIA-B PRA/DDRA are never written here.
; EP1: CRA/CRB on both CIAs are read here, before this or any later code
; writes either -- every proposition's own stimulus starts with its own
; "CR<-$00", strictly after this macro.
SETUP           macro
                move.w  #FIXTURE_VALUE,FIXTURE_ADDR
                move.w  #$2700,sr
                lea     CUSTOM,a0
                move.w  #$7FFF,DMACON(a0)
                move.w  #$7FFF,INTENA(a0)
                move.b  #ICR_CLEARMASK,CIAA_ICR
                move.b  CIAA_ICR,d0             ; discarded (clears flags)
                move.b  #ICR_CLEARMASK,CIAB_ICR
                move.b  CIAB_ICR,d0             ; discarded (clears flags)
                ifd     MUT_EP1
                move.b  #$0A,CIAA_CRA           ; a write before the first read of CRA
                endc
                move.b  CIAA_CRA,d0
                move.w  d0,EP1_CRA_A
                move.b  CIAA_CRB,d0
                move.w  d0,EP1_CRB_A
                move.b  CIAB_CRA,d0
                move.w  d0,EP1_CRA_B
                move.b  CIAB_CRB,d0
                move.w  d0,EP1_CRB_B
                move.w  #MARKER_SENTINEL,MK_START
                ifd     MUT_F1
                move.w  #$00C8,$1C0(a0)         ; HTOTAL: shorter lines
                move.w  #$0080,$1DC(a0)         ; BEAMCON0: VARBEAMEN
                endc
                ifd     MUT_EP2LOW
                move.w  #0,COP1LCH(a0)          ; Copper list in low Chip memory
                move.w  #$0100,COP1LCL(a0)
                move.w  #0,COPJMP1(a0)
                move.w  #$8280,DMACON(a0)       ; SET,DMAEN,COPEN
                endc
                endm

; ---- R-LATCH(T,L) (SPECIFICATION.md 1.5): low byte then high byte -----
; Caller ensures the timer's CR has START=0 and RUNMODE=0 first (p352,
; p342). \1=LO register address, \2=HI register address, \3=16-bit value.
RLATCH          macro
                move.b  #(\3)&$FF,\1
                move.b  #((\3)>>8)&$FF,\2
                endm

; ---- Copper registers (Appendix B, Register Summary Address Order), for
; EP2's own contrast only: nanomig.i does not define these. Offsets from
; CUSTOM.
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088

; ---- zero-extend a byte read into d0 into a word result store ---------
; \1=CIA register byte address, \2=RESULTS word address to store into.
READ_STORE      macro
                moveq   #0,d0
                move.b  \1,d0
                move.w  d0,\2
                endm

; ---- 16-bit timer read (SPECIFICATION.md "Read TAHI, TALO -> u"): HI then
; LO, combined into one word and stored. \1=HI reg addr \2=LO reg addr
; \3=RESULTS word address to store into.
READ_TIMER      macro
                moveq   #0,d0
                move.b  \1,d0
                lsl.w   #8,d0
                move.b  \2,d0
                move.w  d0,\3
                endm

; ---- R-SAMPLE(n; X) (SPECIFICATION.md 1.5): n iterations, each a plain
; byte read of X stored (zero-extended) to the next word of an array with
; its own distinct address (SPECIFICATION.md 1.2 E9's own per-run word
; budget), so every iteration's store time is separately visible at the
; Chip-memory port. \1=iteration count \2=array base address \3=X reg addr.
RSAMPLE1        macro
                lea     \2,a1
                move.w  #(\1-1),d2
.rs\@:          moveq   #0,d0
                move.b  \3,d0
                move.w  d0,(a1)+
                dbra    d2,.rs\@
                endm

; ---- R-SAMPLE(n; X, Y): as above, with a second read/store each
; iteration. \1=count \2=array base X \3=X reg addr \4=array base Y
; \5=Y reg addr.
RSAMPLE2        macro
                lea     \2,a1
                lea     \4,a2
                move.w  #(\1-1),d2
.rs\@:          moveq   #0,d0
                move.b  \3,d0
                move.w  d0,(a1)+
                moveq   #0,d0
                move.b  \5,d0
                move.w  d0,(a2)+
                dbra    d2,.rs\@
                endm

; ---- R-SAMPLE(n; X) with the register read replaced by a read of K3's Chip-memory
; byte (the loop then holds no CIA access). \1=count \2=array base address.
RSAMPLE1_CHIP   macro
                lea     \2,a1
                move.w  #(\1-1),d2
.rs\@:          moveq   #0,d0
                move.b  CHIPBYTE_K3,d0
                move.w  d0,(a1)+
                dbra    d2,.rs\@
                endm

; ---- R-SAMPLE(n; X, Y) with both register reads replaced by reads of K3's
; Chip-memory byte. \1=count \2=array base X \4=array base Y.
RSAMPLE2_CHIP   macro
                lea     \2,a1
                lea     \4,a2
                move.w  #(\1-1),d2
.rs\@:          moveq   #0,d0
                move.b  CHIPBYTE_K3,d0
                move.w  d0,(a1)+
                moveq   #0,d0
                move.b  CHIPBYTE_K3,d0
                move.w  d0,(a2)+
                dbra    d2,.rs\@
                endm

; ---- R-SAMPLE(n; X, Y), X read from a CIA register and stored at once, Y read
; from K3's Chip-memory byte after one NOP in every second iteration: the Y
; stores take four phases of the E cycle while the X stores keep the CIA read's
; two. \1=count (even) \2=array base X \3=X reg addr \4=array base Y.
RSAMPLE2_YSPREAD macro
                lea     \2,a1
                lea     \4,a2
                move.w  #(\1/2-1),d2
.rs\@:          moveq   #0,d0
                move.b  \3,d0
                move.w  d0,(a1)+
                moveq   #0,d0
                move.b  CHIPBYTE_K3,d0
                move.w  d0,(a2)+
                moveq   #0,d0
                move.b  \3,d0
                move.w  d0,(a1)+
                nop
                moveq   #0,d0
                move.b  CHIPBYTE_K3,d0
                move.w  d0,(a2)+
                dbra    d2,.rs\@
                endm

; ---- R-SAMPLE(n; X, Y), X read from K3's Chip-memory byte after one NOP in every
; second iteration and stored, Y read from a CIA register and stored at once: the
; X stores take four phases of the E cycle while the Y stores keep the CIA read's
; two. \1=count (even) \2=array base X \4=array base Y \5=Y reg addr.
RSAMPLE2_XSPREAD macro
                lea     \2,a1
                lea     \4,a2
                move.w  #(\1/2-1),d2
.rs\@:          moveq   #0,d0
                move.b  CHIPBYTE_K3,d0
                move.w  d0,(a1)+
                moveq   #0,d0
                move.b  \5,d0
                move.w  d0,(a2)+
                nop
                moveq   #0,d0
                move.b  CHIPBYTE_K3,d0
                move.w  d0,(a1)+
                moveq   #0,d0
                move.b  \5,d0
                move.w  d0,(a2)+
                dbra    d2,.rs\@
                endm

; ---- R-SAMPLE(n; X), padded loop body (P7.3/P8.3's own precondition
; contrast: "a loop body padded beyond 20 E periods fails it"). \4 is an
; inner delay-loop count added inside every iteration.
RSAMPLE1_PAD    macro
                lea     \2,a1
                move.w  #(\1-1),d2
.rs\@:          moveq   #0,d0
                move.b  \3,d0
                move.w  d0,(a1)+
                move.w  #\4,d3
.pad\@:         dbra    d3,.pad\@
                dbra    d2,.rs\@
                endm

; ---- parameter block (SPECIFICATION.md 1.2 E7, from $0C2000): a byte the
; C++ side pokes before boot (nmh::NanomigHarness::poke_word, on the
; identity-mapped chip RAM), read by the *same* assembled program in two
; otherwise-identical runs, so the compared runs execute identical code
; (E7). Used only where the specification names "(E7)" explicitly:
; P7.3/P8.3's start value, and P8.65 mode 11's CRB value.
PARAM_BASE      equ     $0C2000
PARAM_A         equ     PARAM_BASE       ; word; low byte read at PARAM_A+1
PARAM_B         equ     PARAM_BASE+2

; ---- K3's own Chip-memory byte (SPECIFICATION.md 1.2 E9, P6's contrast) -
CHIPBYTE_K3     equ     $0C4000

; ---- 24-bit TOD write, HI/MID/LO order (SPECIFICATION.md P8.7). \1=HI reg
; \2=MID reg \3=LO reg \4=24-bit value.
WRTOD           macro
                move.b  #((\4)>>16)&$FF,\1
                move.b  #((\4)>>8)&$FF,\2
                move.b  #(\4)&$FF,\3
                endm

; ---- 24-bit TOD read, HI/MID/LO order, into d0 (clobbers d1). \1=HI reg
; \2=MID reg \3=LO reg.
RDTOD24         macro
                moveq   #0,d0
                move.b  \1,d0
                lsl.l   #8,d0
                moveq   #0,d1
                move.b  \2,d1
                or.l    d1,d0
                lsl.l   #8,d0
                moveq   #0,d1
                move.b  \3,d1
                or.l    d1,d0
                endm
