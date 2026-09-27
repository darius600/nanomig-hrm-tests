; prog_audio.s -- P1-P4 (SPECIFICATION.md section 3): both bytes of every
; audio data word reach the output in order, upper byte first, each byte
; two's-complement, every one-LSB step equal. Runs tables T1, T2, T3 in
; the order the manual's own steps give (2.6): the test finds each
; table's own pass boundaries in ram_log() by the reads of that table's
; own AUD0LC address, so this single capture serves all four
; propositions and their shared preconditions A1-A4 (2.7).
;
; T1/T2/T3's word content is poked directly into chip RAM by the C++
; test (NanomigHarness::poke_word(), the same "direct memory preload, no
; bus cycles" mechanism load_program() itself uses) before boot(): no
; instruction in this program ever has to write the table data itself,
; only the pointer/length/period/volume/enable registers that tell the
; audio DMA engine where to read it. CLEAR_OVL (ch5regs.i) is still
; needed once, first, so that chip bank 0 (where the tables and T1_ADDR/
; T2_ADDR/T3_ADDR sit, all below $080000 per 2.1) is real memory and not
; the boot-time Kickstart-shadow window (measured to hold for both the
; CPU's own accesses and the audio DMA engine's; see SPECIFICATION.md
; section 5).
;
; P1-P4's own contrasting cases (T1's last word, every T1 word's bytes,
; every T2 byte's bit 7, T3's word for code +5) are pure data and need no
; separate assembled binary: the C++ test pokes the mutated table content
; into this same prog_audio.bin run under the matching NMH_MUTATE value
; (P1, P2, P3, P4). Only a mutation that changes what the CPU does needs
; its own assembly-time symbol (set by an including mutant source, the
; same "include this file with a symbol set" pattern as
; apparatus-vasm-program/prog_control.s):
;   MUT_A1  T1's own DMA is never enabled (DMACON $8200 instead of $8201: A1 for T1 goes red)
;   MUT_A2  ADKCON $8001 (USE0V1) is set before the ADKCONR read (A2 goes red)
;   MUT_A4  T1's own AUD0VOL is 0 instead of 64 (A4 for T1 goes red)

        include "nanomig.i"
        include "ch5regs.i"

        org     PROGRAM_ORG

T1_ADDR equ     $002000         ; 4 words, below $080000 (2.1)
T2_ADDR equ     $002100         ; 4 words
T3_ADDR equ     $002200         ; 256 words

; Waits for one VERTB pulse (INTREQR bit 5), clearing it afterwards, with
; a bounded fallback loop counter so the wait can never hang (2.3
; "Bounded waits"; how this program waits is not graded). One VERTB
; pulse (about 71000 color clocks) is well above every table's own
; "N_T + 3 passes" minimum (2.6): T1/T2 need about 11000, T3 about
; 330000 across the five calls used for it below. Clobbers d6, d7.
WAITVBL macro
                move.l  #200000,d7
.wv\@           btst    #5,INTREQR(a0)
                bne.s   .wvd\@
                dbra    d7,.wv\@
.wvd\@          move.w  #$0020,INTREQ(a0)
                endm

start:
        CLEAR_OVL
        lea     CUSTOM,a0

        ; ---- preamble (2.3) ----
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,ADKCON(a0)
        ifd     MUT_A2
        move.w  #$8001,ADKCON(a0)      ; MUT_A2: attach mode (USE0V1) set, after the
                                        ; preamble's own clear, so it survives to the read
        endc

        ; ---- step 2: read ADKCONR, store it (precondition A2) ----
        move.w  ADKCONR(a0),RESULTS

        ; ---- step 3: channels 1-3 stay off (p.266 "000000=no output") ----
        move.w  #0,AUD1VOL(a0)
        move.w  #0,AUD2VOL(a0)
        move.w  #0,AUD3VOL(a0)

        ; ---- table T1 (P1, P2; A1/A2/A4 demonstrated on this table) ----
        move.l  #T1_ADDR,AUD0LCH(a0)   ; one 32-bit write sets both halves (p.139)
        move.w  #4,AUD0LEN(a0)
        ifd     MUT_A4
        move.w  #0,AUD0VOL(a0)         ; MUT_A4: no output, L has at most one entry
        else
        move.w  #64,AUD0VOL(a0)        ; p.140: 64 = maximum volume
        endc
        move.w  #128,AUD0PER(a0)       ; p.140/p.266 minimum is 124; well above it
        ifd     MUT_A1
        move.w  #$8200,DMACON(a0)      ; MUT_A1: AUD0EN not set -- no audio reads
        else
        move.w  #$8201,DMACON(a0)      ; SET, DMAEN, AUD0EN (p.144)
        endc
        WAITVBL
        move.w  #$0001,DMACON(a0)      ; clears AUD0EN only (p.144)

        ; ---- table T2 (P3) ----
        move.l  #T2_ADDR,AUD0LCH(a0)
        move.w  #4,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #128,AUD0PER(a0)
        move.w  #$8201,DMACON(a0)
        WAITVBL
        move.w  #$0001,DMACON(a0)

        ; ---- table T3 (P4) ----
        move.l  #T3_ADDR,AUD0LCH(a0)
        move.w  #256,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #128,AUD0PER(a0)
        move.w  #$8201,DMACON(a0)
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        WAITVBL
        move.w  #$0001,DMACON(a0)

        DONE

; No table data is assembled into this binary: T1/T2/T3's words are
; poked directly into chip RAM by the C++ test's run_audio_program()
; (poke_word() at T1_ADDR/T2_ADDR/T3_ADDR, before boot()), which is the
; single source of truth for their content, MUT_P1-MUT_P4 included, so
; that no copy of the data can drift from what the test actually checks.
