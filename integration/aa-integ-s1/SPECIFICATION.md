# Specification: integration package aa-s1

This document specifies an integration test package for an FPGA re-implementation of the Amiga chip set. The oracle is Commodore's preliminary *Functional Specification for the Advanced Amiga Chip Set (AA)*. The author wrote it from the package's claims (`claims-spec-stage.jsonl`) and the specification's pages alone, and never saw the implementation or any test. The Amiga Hardware Reference Manual (3rd edition) was read only where the AA specification builds on the original chip set. A builder implements the package from this document and may not change what it specifies.

The package covers these subjects: BPLCON1 horizontal scroll, the BYPASS bit, the FMODE bit-plane fetch increment, bit-plane modulos with and without scan-doubling, the reset state of LPEN, the joystick and mouse registers, the UART break, the disk DMA pointer and the DUAL-mode secondary beam counter. It has 15 propositions (P1 to P15).

---

## 0. Conventions

- **"AA p.N"** means page N of the AA specification corpus (`corpus\pNN.txt`). The document prints no page numbers; the corpus numbers PDF pages. **"HRM p.N"** means printed page N of the Hardware Reference Manual. The HRM is cited only for behaviour of the original chip set that the AA pages take for granted. Every expected value comes from an AA page unless it is marked HRM.
- Every quotation used for an expected value was checked against the scan images of the printed page (`bands\`).
- Register names and offsets follow AA p.6 to p.10. The address of a register is $DFF000 + offset. Writes are 16-bit.
- **Harness state for every run:**
  - AA selected (`Config.chipset_config = 0x10`).
  - PAL, selected by the program: S1 step 1 writes BEAMCON0 = $0020 (AA p.12, bit 5: "PAL= Set appropriate decodes (in normal mode) for PAL"). The harness `Config` (nanomig_harness.h) has no PAL field. A proposition that writes BEAMCON0 itself (P10, P14, P15) sets PAL in its own value.
  - 2 MB Chip memory.
  - The program is 68000 code.
  - Which build is graded is set once, in section 4 ("Builds").
  - Every run starts from the harness's power-on reset.
- **Units:**
  - A **SHRES pixel** is 35 ns (AA p.17: "PFyH0=LSB=35nS=1 SHRES pixel").
  - A **HIRES pixel** is 70 ns and a **LORES pixel** is 140 ns (AA p.17 SPRES table: "LORES(140nS)", "HIRES(70nS)", "SHRES(35nS)").
  - A **CCK** (colour clock) is one memory cycle of 280 ns (AA p.21: "memory cycles are 280 ns").
  - An **op** is 70 ns of display time, measured at the video port. The harness takes one video sample per clk_sys cycle (`VideoSample`, nanomig_harness.h) and has no pixel clock, so R1 fixes the op in samples from a page-defined event: half the length of one LORES pixel (140 ns).
- **Frames:** "Fk" is the k-th complete frame after the write that enables DMA, delimited by the leading edges of vsync on the video output. A configuration that changes during a run is written by the CPU after the VERTB flag (R5) and before line $2C. The frame that starts with that VERTB is the observed one.
- **Addresses:** buffer addresses given below are examples, except in P13, where the address is the stimulus. The builder may choose others with the stated alignment, provided no two buffers overlap and none overlaps the program.
  - Every buffer, Copper list, sprite structure and data table lies in $080000-$1FFFFF. That is the Chip memory the harness maps one to one (nanomig_harness.h, `kChipIdentityLo`/`kChipIdentityHi`), so the word address the port shows is the address the program wrote. Below $080000 the ROM overlay covers Chip memory after reset (HRM p.223: "An additional copy of the system ROM responds starting at memory location $00000000. The system RAM that would normally be located at this address is not available"), and the harness does not claim a one-to-one mapping there.
  - Nothing may overlap the program (from `PROGRAM_ORG` = $080400), the stack (just below `PROGRAM_STACK` = $0FFF00) or the results area (nanomig.i).
  - No store other than DONE's own may reach `DONE_ADDR` = $0C0FFE (nanomig.i). `RESULTS` = $0C0000 holds 2047 words below it. P11 and P14 store more than that, so their stores go to a buffer of their own.

---

## 1. Claims that give no proposition

| Claim(s) | Page | Why no proposition |
|---|---|---|
| `sol:AA-mouse.c1` | 5 | "A two pin serial mouse interface replaces the 4 pin CCK multiplex scheme" describes the chip's pins. The test sees no chip pin. Mouse movement reaches the system as counts, not as signals on the mouse pins. P11 checks the programmer-visible consequence. |
| `sol:AA-mouse.c3`, `sol:AA-mouse.c4` | 5 | The eight extra bits come from "8 miscellaneous control bits" loaded into the external parallel-to-serial converter (AA p.25). They appear in LISAID bits 15-8 (AA p.26: "The upper 8 bits of this register are loaded from the serial mouse bus, and are reserved for future hardware implentation."). No page states their value, and the harness drives no such inputs, so a read of LISAID bits 15-8 has no expected value. |
| `sol:AA-mouse.c5` | 5 | "are intended for hardware configuration information" is an intent and gives no value. |
| `opus:AA-mouse.c3`, `sol:AA-mouse.c6`, `sol:AA-mouse.c7` | 5 | "The mouse counters are updated only 1/16 as often as before (223 KHz)" limits how fast quadrature edges on the pins can be counted. The harness delivers movement as counts, not as pin pulses, and no page states a latency a program could see. No output shows the update rate. |
| `sol:AA-mouse.c8` | 5 | "but this should be transparent to the user" is an expectation ("should") and gives no value. |
| `opus:AA-adkcon.c12` | 10 | WORDSYNC "Enables disk read synchronizing on a word equal to DISK SYNC CODE". Its effects need data read from a disk surface: the sync itself, the DSKSYNC interrupt (AA p.25 bit 12) and WORDEQUAL (AA p.23: "true only while DSKSYNC register equals the data from disk"). The harness serves no disk. |
| `opus:AA-adkcon.c13` | 10 | MSBSYNC fails for the same reason. Its description also continues on AA p.11 ("(most signif bit). Appl type GCR"). |
| `sol:AA-hhpos.c3`, `sol:AA-hhpos.c4` | 24 | The pages name the results of the HBSTRT/HBSTOP comparisons only on outputs the test does not see: the BLANK output (AA p.4, p.18 EXTBLKEN) and the CSY* pin (AA p.13 VARCSYEN). No page says what the RGB output shows during programmed horizontal blanking (AA p.4: BLANK "allows the analog level for blanking to be different than black"). |
| `sol:AA-hhpos.c6`, `sol:AA-hhpos.c7` | 24 | The pages give the results of the HSSTRT/HSSTOP comparisons only at the HSY* pin (AA p.12: "VARHSYEN= comparator HSY -> HSY* pin. Set on HSSTRT value, reset on HSSTOP value.") and at CSY* (AA p.13). The test sees horizontal sync only after the line doubler. The harness states that the design's scan doubler is always on and that `hs` toggles at about twice the chipset's own line rate (nanomig_harness.h, `find_lines`), so `hs` is regenerated after the doubler. Nothing says the doubler passes HSY* timing through unchanged, least of all a DUAL-mode HSY* with period HTOTAL+1 CCK. If the harness ever exposes HSY* before the doubler, the check would be: with DUAL=1 and VARHSYEN=1, HSY* is set at secondary-counter value HSSTRT and reset at HSSTOP, once per HTOTAL+1 CCK. |
| `opus:AA-dskpth.c10`, `sol:AA-disk-pointer.c2` | 23 | "These address registers must be initalized by the processor or coprocessor before disk DMA is enabled" is a rule for the program. The page states no outcome for breaking it. P13 follows it in its stimulus. |
| `opus:AA-bplcon1.h2` | 16 | "BPLCON1 is write-only." The page's "W" gives no value for a read, so the "only" has no expected value. The write part is graded in P1 through `opus:AA-bplcon1.h1` and `sol:AA-bplcon1.c7`. |
| `sol:AA-position-resolution.c1` | 2 | The claim is 35 ns scroll resolution "in every display resolution". The output keeps one 70 ns sample (R1). A 35 ns shift of a 140 ns (LORES) or 70 ns (HIRES) pixel changes the output only if the shifted pixel edge crosses one of those samples. The phase of the edge relative to the samples is fixed by the implementation, and only the bit under test moves it by 35 ns, so no LORES or HIRES stimulus can make a 35 ns check go red. The claim's LORES and HIRES part cannot be graded, so the claim is not carried. P2 grades each BPLCON1 bit's weight, the 35 ns bits in SHRES. |
| `opus:AA-bplcon1.c5`, `sol:AA-horizontal-comparators.c1` | 5 | The sentence covers every horizontal comparator field AA p.5 lists: "DIWHIGH,HBSTRT,HBSTOP,SPRCTL,BPLCON1". HBSTRT and HBSTOP have no visible output (see `sol:AA-hhpos.c3`), so the claim cannot be graded whole. P2 grades the BPLCON1 bit weights. |
| `opus:AA-bypass.c1`, `opus:AA-bypass.c2` | 4, 17 | Both claims put 8 bits of the colour address on R(7:0). The port list has a 4-bit red output (`r[3:0]`), so one nibble of the address is never visible (R4). P6 grades the visible nibble and the table bypass under `sol:AA-bplcon0.c19`, whose content (scrolling and priority unchanged) it grades whole. |
| `opus:AA-fmode.c19`-`c26`, `sol:AA-fmode-bitplane-fetch.c2`, `c3`, `c5`, `c6`, `c8`, `c9`, `c11`, `c12` | 23 | "Memory Cycle" (normal/double CAS) and "Bus Width" (16/32) describe the DRAM interface. The port shows neither a CAS strobe nor a bus width. On every read the harness drives `chip48` with the three words that follow the read address, wrapping inside the aligned 16-byte block (nanomig_harness.h), so the port does not even show how many bytes a fetch uses. The product of the two columns equals the increment column, which `sol:AA-fmode-bitplane-fetch.c1`, `c4`, `c7`, `c10` carry in P7. That product grades no CAS or width value: rows 01 (32 bits, normal CAS) and 10 (16 bits, double CAS) give the same increment. |
| `opus:AA-fmode.c35`-`c42`, `sol:AA-fmode-sprite-fetch.c2`, `c3`, `c5`, `c6`, `c8`, `c9`, `c11`, `c12` | 24 | The same holds for the sprite table. |
| `opus:AA-fmode.c10`, `sol:AA-fmode.c12` (bit 03 SPAGEM); `opus:AA-fmode.c12`, `sol:AA-fmode.c14` (bit 01 BPAGEM) | 23 | Each claim names the bit as the page-mode (double CAS) bit. CAS is not visible. Bit 3 alone and bit 2 alone (bit 1 alone and bit 0 alone) select rows with the same 4-byte increment, so no port observation tells the page-mode bit from the 32-bit bit. |
| `opus:AA-bpl1mod.c6`, `sol:AA-bplmod.c3`, `sol:AA-bplmod.c4` | 16 | "may have sizes that are different" is a permission that follows from the separate modulos and has no expected value of its own. It is the reason P8 gives the two modulos different values. |
| `opus:AA-hhposw.c6` | 24 | The claim bundles the secondary counter's HBSTRT, HBSTOP, HSSTRT and HSSTOP comparisons, which have no visible output (see `sol:AA-hhpos.c3`, `sol:AA-hhpos.c6`), with its HTOTAL comparison and the UHRES trigger. It cannot be graded whole. P14 grades the HTOTAL part under `sol:AA-hhpos.c5`, and P15 the UHRES part under `sol:AA-hhpos.c2`. |

## 2. How some claims are graded

| Claim(s) | Page | How it is graded |
|---|---|---|
| `sol:AA-bplcon1.c7`, `sol:AA-fmode.c5` | 16, 23 | "W": the header assigns write access, and a write that takes effect grades it (P1, P7). |
| `sol:AA-bplcon1.c9`, `sol:AA-bplcon1.c10` | 3, 17 | Recorded, not graded. Their restatement calls PF2Hx and PF1Hx a "three-bit" code, where AA p.3 ("BPLCON1 now contains an 8 bit scroll value for each of the playfields") and the quote ("x=0-7") give eight bits. The restatement is not graded as written. P2's rows grade the eight bits of each playfield, and the run prints one RECORDED line naming both claims. |
| `sol:AA-bplcon1.c12`, `opus:AA-bplcon1.c6`, `sol:AA-bplcon1.c5` | 5, 17 | A bit's name is not observable. P2 checks the weight the name carries: bit 00 = PF1H2 and bit 04 = PF2H2 = 140 ns, the weight each had before. `sol:AA-bplcon1.c12` ("old PFyH0 now PFyH2") names both playfields and is graded by P2-H2, the conjunction of the PF1H2 and PF2H2 rows. |
| `sol:AA-bplcon0.c19` | 17 | "bitplanes are scrolled and prioritized normally": P6-I compares, at every pixel, the visible nibble of the address the BYPASS output carries with the one the colour table shows without BYPASS, under a 6 op scroll and with a sprite in front. The green and blue outputs under BYPASS are not stated and not checked. |
| `sol:AA-mouse.c2` | 5 | "transparent to the programmer" is checked through the register values AA p.25-26 give (P11). Those values agree with HRM p.229-232, which describes the earlier chips. |

---

## 3. Reference frames

**R1 - output column and scale.**
- **Column and line.** Column c of an output line is the number of video samples after the leading edge of that line's hsync, divided by the op length from "Scale" below. The output line index is the number of hsync leading edges after the leading edge of vsync.
- **Doubled lines.** If a source line appears on more than one output line, every copy in the observed range is checked, and all copies must agree.
- **Comparing runs.** Positions are compared only between runs or frames with identical beam timing.
  - *Anchor check:* in each compared pair, the number of output lines per frame is equal and, line by line, the hsync period and the hsync width of every output line are equal: the two vectors of periods and the two vectors of widths compare element by element, and no average is compared. The pages give no tolerance, and none is applied.
  - *Contrast:* a run with NTSC selected (BEAMCON0 = $0000) gives a different line count, and the anchor check fails.
- **Exact comparison.** A displacement is the difference of two sample positions divided by r_L/2. It is compared with the expected number of op exactly, or against the exact set of values the expectation names. The pages give no tolerance, and none is applied.
- **Scale.** In LORES with plane 1 all $AAAA (S2, BPLCON0=$1000), the output alternates COLOR01 and COLOR00. On every window line of F3, the runs between the first and the last COLOR01 sample are complete, and all have one length, r_L video samples. Each run is one LORES pixel, 140 ns (AA p.17: "LORES(140nS)"), so **one op is r_L/2 samples**. Every column, width and displacement in this document is a sample count divided by r_L/2.
  - *Anchor checks* (graded as R1-FIXTURE, R1-F1, R1-RL, R1-EVEN and R1-HIRES; every proposition's lines rest on all five): the calibration runs loaded, booted and stored the fixture; F1 is located in both; all those runs have one length (R1-RL); r_L is even (R1-EVEN); and in HIRES (BPLCON0=$9000) the runs are r_L/2 samples, one 70 ns HIRES pixel (AA p.17: "HIRES(70nS)") (R1-HIRES).
  - *Contrast:* the LORES capture evaluated against the HIRES expectation fails: the HIRES run run as LORES fails R1-HIRES. Bytes of $AA and $FF alternating in plane 1 give runs of two lengths and fail R1-RL, and a HIRES capture taken as the LORES one gives an odd r_L and fails R1-EVEN.
  - *Measured (build), TG68K and fx68k:* r_L = 2 video samples per LORES pixel on F3, so one op is 1 sample, and the HIRES runs are 1 sample. R1-FIXTURE, R1-F1, R1-RL, R1-EVEN and R1-HIRES PASS on both cores. Each has its contrast (selectors `R1-FIXTURE`, `R1-F1`, `R1-RL`, `R1-EVEN`, `R1-HIRES`), which prints FAIL on its own line on both cores and NOT GRADED on the lines that rest on it.
  - The SHRES measures (stripes, S3) rest on each op showing one of every two 35 ns pixels. S3b's stripe precondition shows whether it does.

**R2 - Chip-memory port, CCK, line boundaries, slots.**
- **Time.** Port times are clk_sys cycles (`RamAccess.cycle`, nanomig_harness.h). The harness defines no clock constant, so K, the number of clk_sys cycles per CCK, is measured. Figure 6-9 (HRM p.195) draws the four refresh slots on alternate 280 ns cycles, so consecutive refresh accesses of one group are 2 CCK apart, and K is half their spacing.
- **Line boundaries.** Each line has four refresh accesses (AA p.29: "There are 4 refresh time slots"). They lie at the start of the line, before the disk, audio and sprite slots (HRM p.195, Figure 6-9). A line boundary is the first refresh access of a group of four.
  - A refresh access is recognised by the port list's `refresh` output ("ram refresh cycle"), read through `ports()`. In the RAM-port log it is one of a group of four equally spaced reads at addresses that no buffer, list or program uses. The anchor check below applies either way.
  - *Anchor check:* in runs with VARBEAMEN=0, refresh accesses come in groups of exactly four, and consecutive groups are exactly 227 or exactly 228 CCK apart. The pages give a line of 227 CCK (HRM p.23: "In PAL, they are all 227 long") and a long line of 228 CCK (HRM p.23; AA p.29: "identification of long horz. line(228 CC)"); no other length is accepted.
  - *Measured, not graded:* the number of line spacings of 227 CCK and of 228 CCK, whether the two alternate, and the average over all spacings. HRM p.194 gives 227.5 colour clocks per line on average and AA p.29 a 228 CC line "of every other line"; whether this system alternates is not asked here, and no line of this package depends on it.
- **Slots.** The harness reports no slot. The slot of an access is its distance in CCK from its line boundary.
- **Fetches.** The harness logs one entry per access: clk_sys cycle, one word address, the 16-bit word at it, direction and byte lanes (`RamAccess`). On every read it also drives `chip48` with the three words that follow the read address, wrapping inside the aligned 16-byte block (nanomig_harness.h). So the port shows where a fetch reads, not how many bytes it uses.
  - A fetch of a bit-plane or sprite channel is that channel's entries in one slot, and its address is the lowest word address among them.
  - A fetch's width is graded only through its address step: A_{j+1} - A_j between consecutive fetches of one channel.
- **Attribution.** An access is attributed to a plane or sprite by its address, because every channel's buffer lies in its own region.

**R3 - absolute line number (Copper marker).**
- A marker is a Copper WAIT for line V (IR1 = V<<8 | $07, IR2 = $FFFE) followed immediately by MOVE $1FE,$0000. Address $1FE is the NULL address (AA p.10), so the MOVE has no effect.
- The WAIT "keeps Coprocessor off of bus until beam position has been reached" (AA p.20). The MOVE's first instruction word is therefore fetched in beam line V. Line V is the R2 line that contains that fetch, and other lines are numbered by counting R2 boundaries from it.
- Only lines below $100 are marked, because the WAIT compares VP7-VP0 only.
- *Anchor check:* two markers at V_a and V_b fall V_b - V_a boundaries apart.
- *Contrast:* moving one marker by one line breaks the relation.

**R4 - which colour bits the 4-bit output shows** (needed by P6).
1. Write COLOR01 = $0A00 with LOCT=0. This gives red $AA: AA p.19, "When LOCT=0 the 4 MSB of RED,GREEN, and BLUE video data are selected ... The low order set of registers is also selected simultaneously, so that the 4 bit values are automatically extended to 8 bits".
2. Set BPLCON3 = $0E00 (LOCT=1) and write COLOR01 = $0500. The LOCT=1 row of AA p.19 writes R3-R0, so red becomes $A5.
3. Restore BPLCON3 = $0C00. The order follows AA p.5: "load LSB after MSB".

A COLOR01 pixel then shows red A if the output carries R7-R4, or 5 if it carries R3-R0. This document assumes R7-R4, "the high nibble". If R4 shows R3-R0, P6 reads "bits 3-0" wherever it says "bits 7-4".

**R5 - start of vertical blank.** INTREQR bit 5 is VERTB, "Start of Vertical blank" (AA p.25). The program clears it by writing INTREQ = $0020 (bit 15 = 0 clears, AA p.25) and polls INTREQR until bit 5 reads 1.

---

## 4. Common setup

**Builds.** `make test` builds the system under test twice (nanomig.mk), once with each CPU core, fx68k and TG68K, each with the defines of the system's own top for that core. The AA chip set is selected by `chipset_config` = $10 (section 0), an input of the system boundary that does not depend on the CPU core or on a build define, and `cpu_config` = 0 (68000 behaviour). Every proposition, precondition and anchor is graded on both cores, and every verdict is reported per core; where the cores differ, that is a result and is not reconciled.

**Programs (build).** P1-P5 share two 68000 programs: `prog_bars.s` (S1, S2, S3 and DMA enable, then DONE) and `prog_bars_midrun.s` (the same, plus an R5 VERTB-edge counter that performs one deferred register write, for P4's reset-then-F5-reference shape). What differs between runs -- BPLCON0, BPLCON1 and whether it is written at all, BPLCON4, FMODE, DDFSTRT/DDFSTOP, DMACON, and the two line buffers -- is a configuration block the harness preloads into chip RAM before `boot()`, the same technique `ch3-playfield-system` uses for its Copper list and bitplane data (nanomig_harness.h's `poke_word`, never a register: registers are not memory and are always written by a genuine CPU MOVE the program executes). The per-line Copper pointer reload (S3) is generated by a loop in the program itself rather than written out as 196 literal instruction pairs. R1's "Scale" (the video samples per LORES pixel) is measured once, from a dedicated LORES all-$AAAA calibration run, before any proposition's Δ is computed from it. The configuration block also holds the BEAMCON0 value (PAL, $0020) and the low word of every Copper WAIT of the S3 reload ($0007: horizontal position $06), so that the contrasts of R1 (NTSC) and of S3a (position $40) change the stimulus alone. `prog_bars_nofix.s` and `prog_bars_midrun_nofix.s` are the same programs without the fixture store.

**Programs (P7-P9).** `prog_regs.s` takes a list of (register offset, value) word pairs that the harness preloads into chip RAM at $0B8000, ended by $FFFF, and writes each value to its register with one MOVE.W, in list order. The list carries S1's order: the clears first, then the proposition's registers, the Copper location register and its strobe, DMACON last. After the list it reads LISAID once and stores it at RESULTS+4. Copper lists (at $0B2000) and bit-plane data are preloaded as well. `prog_regs_nofix.s` is the same program without the $BEEF store.

**Build conventions.**
- **Frames.** For the register-list runs the frame holding the first fetch of the Copper list is the frame the DMA-enabling write falls in; Fk is its k-th successor, delimited by vsync leading edges as in section 0. P1-P5 locate F1 from the video content: the first frame whose window population equals the next frame's.
- **P6 modulus.** P6 writes BPL1MOD = BPL2MOD = $FFD8 (-40 bytes, one line's 20 words) instead of S2's $0000, so that every window line shows the same 20 words (AA p.16: a modulo "is automatically added to the address at the end of each line").
- **Connectors.** JOY0DAT (the left controller pair, AA p.25) is driven through the input `joystick1` and JOY1DAT through `joystick0`; the port list wires the two inputs to the two connectors that way and no page names the mapping. P11's "port 0" is the connector of JOY0DAT.
- **Mouse input.** A count is a signed 8-bit datum on `kbd_mouse_data`, taken when `kbd_mouse_level` toggles; `kbd_mouse_type` = 0 carries an X count and `kbd_mouse_type` = 1 a Y count. +1 is right and down, -1 is left and up (HRM p.229: the counters increment for right and "down", decrement for left and "up"). The input takes a Y count only after an X datum, so the build sends an X count of 0 before each Y count.
- **Read times (P11).** Each loop iteration reads JOY0DAT and then JOY1DAT; a read precedes its store, so an iteration lies before a change when the store of its JOY1DAT value does.
- **Completion.** Every program ends with DONE (nanomig.i) after a bounded number of steps, and every run's FIXTURE line asserts that the run loaded, booted, stored $BEEF and reached DONE. The register-list and bar programs store DONE when their last register is written; P13's program polls INTREQR for at most 7000 reads (about 50 ms) before it writes DSKLEN and stores DONE; P15's programs store DONE after the fifth VERTB (the pointer programs) or after a fixed number of reads (the live-input programs), and P15-DONE asserts it.
- **Mutations.** `NMH_MUTATE=<selector>` runs one contrast and only the proposition the selector names, with the one changed stimulus the `// MUTATION` record in the test source describes, through the proposition's own comparators. A mutation never changes an expected value. The programs named `*_nofix`, `*_k`, `*_c`, `*_same`, `*_late`, `*_var`, `*_early`, `*_pad*`, `*_sync` and `prog_cpuuh_*` are the contrast builds.

**Programs (P10-P12).**
- `prog_lpen.s` (P10) first stores CIAA PRA bits 7-6 at RESULTS+6 (P10's precondition (iv)) and then performs P10's steps 1-4 with a wait of 13 dbra iterations (130 CPU clocks, 65 CCK) between the two reads; `prog_lpen_c.s` adds the BPLCON0 = $0008 write of contrast C, `prog_lpen_late.s` starts the reads about 110 lines after the marker, `prog_lpen_var.s` selects VARBEAMEN with HTOTAL = $00E8 and VTOTAL = $0138, `prog_lpen_same.s` stores one read as both w0 and w1, and `prog_lpen_nofix.s` has no fixture store.
- `prog_joy.s` (P11) reads JOY0DAT and JOY1DAT 7200 times and stores each value to a buffer of its own (the store's port time is the read's time). `prog_joy_nofix.s` has no fixture store.
- `prog_uart.s` (P12) follows P12's steps; `prog_uart_k.s` writes ADKCON = $0800 at step 5 (contrast K), `prog_uart_early.s` writes ADKCON = $8800 in place of $0800 at step 2, `prog_uart_nofix.s` has no fixture store.

**Unit run (build).** A run with no DMA enabled, made before P10-P14 use time. It fixes K, the number of clk_sys cycles per CCK, from the `refresh` output: the four refresh accesses of a line are 2 CCK apart (R2), so K is half their spacing. P11 and P12 convert their durations through K (1 ms is 1,000,000 ns / 280 ns CCK = 3571.4 CCK; 280 ns is AA p.21's memory cycle) and P14 converts sample times through it. Measured, TG68K and fx68k: K = 8 clk_sys cycles per CCK, and of 1401 line spacings 1401 are 227 CCK and none is 228 CCK, so the lines do not alternate and the average is 227 CCK. It also reads LISAID: its low byte is $F8 for the AA chip set (AA p.26, "LISA returns hex (F8)"; the ECS Denise returns $FC), which shows that `chipset_config` = $10 selected AA.

**Programs (P13-P15).** `prog_disk.s` (P13) follows the register list with a poll of INTREQR bit 1 and then writes DSKLEN = $4000; the disk DMA accesses are the reads that are neither the program's own fetches, the register list, the reset vectors nor refresh accesses. `prog_dual.s` (P14) follows the list with the 1024 samples; `prog_dual_pad121.s` lengthens the loop body to 121 CCK, and `prog_dual_sync.s` takes one sample per line, after the main counter's H8-H1 (VHPOSR bits 7-0) lies in $20-$2F. `prog_uhres.s` (P15) follows it with the rewrite of BPLHPTH/BPLHPTL after each VERTB; `prog_cpuuh_line.s` and `prog_cpuuh_57.s` read the region U with the CPU once per 227 and once per 57 CCK instead. `prog_disk_nofix.s`, `prog_dual_nofix.s` and `prog_uhres_nofix.s` have no fixture store.

**S1 - program skeleton.**
1. Clear all DMA enables, interrupt enables and interrupt requests: DMACON = $7FFF, INTENA = $7FFF, INTREQ = $7FFF (AA p.22 and p.25: bit 15 = 0 clears the bits written with 1). Write BEAMCON0 = $0020 (PAL, section 0), unless the proposition gives its own BEAMCON0 value.
2. Write the proposition's registers.
3. If the Copper is used, the order is: write COP1LCH/COP1LCL, then write COPJMP1, and only then set COPEN in DMACON. This follows AA p.21: "It is important that one of the jump registers be initalized and it's jump strobe address hit, after power up but before Coprocessor DMA is initalized."
4. The Copper list starts with the pointer MOVEs, because "COP1LC is automatically used at the beginning of each vertical blank time" (AA p.21). It ends with WAIT $FFFF,$FFFE.
5. Results the program reads are stored to Chip memory, where the port shows the stores.

**S2 - display baseline** (used unless a proposition says otherwise):

| Register | Value | Meaning |
|---|---|---|
| BPLCON2 | $0024 | PF2P=PF1P=100: the playfields lie behind all sprites (HRM p.210 Table 7-2; "PF2P2 - PF2P0, bits 5-3, are the priority bits for normal (non-dual) playfields"). |
| BPLCON3 | $0C00 | The RST_ values of AA p.16: PF2OF=011, all else 0. |
| BPLCON4 | $0011 | The RST_ values of AA p.16: ESPRM=OSPRM=0001, BPLAM=0. |
| DIWSTRT | $2C81 | Window starts at line $2C. |
| DIWSTOP | $F0C1 | Window ends before line $F0. V8 of the stop is implied as the complement of V7 (AA p.22: "V8=/=V7"), so the window covers lines $2C-$EF. DIWHIGH is not written. |
| DDFSTRT, DDFSTOP | LORES: $0038 / $00D0. HIRES and SHRES: $003C / $00D4 | HRM p.80 Table 3-14. The AA pages give no values for SHRES; the preconditions show whether the data appears. |
| BPL1MOD, BPL2MOD | $0000 | |
| FMODE | $0000 | Unless stated. |
| Colours | COLOR00 = $0222, COLOR01 = $0F00, COLOR02 = $00F0, COLOR03 = $000F, COLOR17 = $0FF0 | Written with LOCT=0 in bank 0. COLOR17 is sprite 0's colour 1, because ESPRM=0001 puts even-sprite colours at $10-$1F (AA p.18). |

- Bit-plane and sprite buffers start at 8-byte-aligned addresses. The AA pages say nothing about alignment in the wider fetch modes, so every buffer is aligned for the widest.
- Sprites not used by a proposition point, when SPREN is set, to a 16-byte zero block. All eight sprite pointers are reloaded by the Copper in vertical blank (AA p.28: "must be initialized by the processor or coprocessor every vertical blank time").
- BPLCON1 is written by the CPU before DMA is enabled, except in P4.

**S3 - bar display and edge measurement.**

*Line buffers.* Each plane has one line buffer of 192 bytes, all zero except:

| Variant | Plane 1 | Plane 2 |
|---|---|---|
| Bars (LORES) | bytes 8-11 = $FF | bytes 24-27 = $FF |
| Stripes (SHRES) | bytes 8-9 = $FF (solid part), bytes 10-25 = $AA (alternating part) | bytes 56-57 = $FF, bytes 58-73 = $AA |

*Pointer reload each line.* The Copper list contains, for every V from $2C to $EF:
- WAIT(V, $06);
- MOVE BPL1PTH, MOVE BPL1PTL, MOVE BPL2PTH, MOVE BPL2PTL, each to the start of its line buffer.

Every window line therefore shows the same data.

*Preconditions.*
- **S3a.** On every window line of the observed frame, the first plane-1 and plane-2 fetches (R2) read the first bytes of their line buffers. This shows the reload preceded the line's first fetch. The window has 196 lines ($2C-$EF, S2), and each plane is fetched on every one of them.
  - *Contrast:* WAIT(V, $40), which is later than DDFSTRT, makes S3a fail.
- **S3b.** In the reference configuration (BPLCON1 = $0000):
  - Edges E1 and E2 exist on every window line and are equal on all window lines.
  - The bar width is 64 op. For stripes, the solid part is 8 op wide.
  - In the stripe variant, the alternating part shows one colour over its whole width, excluding 4 op at each end. That colour, C1 for plane 1 and C2 for plane 2, is COLOR01 or COLOR00 for plane 1, and COLOR02 or COLOR00 for plane 2. It is taken at the offsets 12, 18, 24, 30 and 36 op from the frame's own solid edge (E1 for plane 1, E2 for plane 2), on every window line of that frame, and the colours at the five offsets agree.
  - *Contrast:* zeroing plane 1's bytes 8-11 removes E1. Replacing $AA by $CC makes the stripe colour non-uniform.

*Measures.*
- **E1** is the first column of the line, scanning from the left, that shows COLOR01's value. **E2** is the same for COLOR02.
- **Displacement:** Δ1(cfg) = E1(cfg) - E1(reference), and likewise Δ2, in op.
- **C1(cfg) and C2(cfg)** are the stripe colours defined in S3b.

---

## 5. Propositions

### P1 - BPLCON1 is the register at 102, written by a write

1. **Proposition.** A CPU write to $DFF102 sets the playfield delays: the register at address 102 is BPLCON1, and it is written by a write access.
   - Claims: `opus:AA-bplcon1.h1`, `sol:AA-bplcon1.c6`, `sol:AA-bplcon1.c7`. (`opus:AA-bplcon1.h2`, "write-only", is declined in section 1.)
   - Page: AA p.16.
2. **Stimulus.**
   - Setup: S1, S2 and S3 bars; LORES with two planes (BPLCON0 = $2000); FMODE = $0000; DMACON = $8380 (SET, DMAEN, BPLEN, COPEN).
   - Reference run: the CPU writes $0000 to $DFF102 before enabling DMA.
   - Test run: the CPU writes $0033 to $DFF102. Bits 0 and 1 are PF1H2 and PF1H3; bits 4 and 5 are PF2H2 and PF2H3.
3. **Observation.** E1 and E2 on every window line of F3.
4. **Expected value.**
   - Δ1 = Δ2 = +6 op, which is 12 SHRES pixels (4 + 8).
   - Source for the address and direction: AA p.16, "BPLCON1 p 102 W D Bit plane control reg.(horiz. scroll control)".
   - Source for the bit names: the AA p.16 table.
   - Source for the unit: AA p.17, "PFyH0=LSB=35nS=1 SHRES pixel".
   - Source for the direction of the shift: HRM p.82, "For each pixel of delay, the on-screen data shifts one pixel to the right".
   - A read of $DFF102 is not graded, because no page gives its value.
5. **Preconditions.** Fixture and F1 in both runs (P1-FIXTURE, P1-F1); R1 anchor between the two runs (P1-R1); S3a in both runs (P1-S3A); S3b in the reference run (P1-S3B-REF) and in the test run (P1-S3B-TEST). The two displacement lines (P1-D1, P1-D2) rest on all of them and on the five R1 anchors.
6. **Contrasting case.** Write $0000 to $DFF102 and $0033 to $DFF10C instead. $10C is BPLCON4 (AA p.8); its bits 7-0 are sprite colour banks and no sprite is on. Expected Δ1 = Δ2 = 0, so a check expecting +6 goes red. The preconditions have their S3 contrasts.
7. **Reference frame.** R1 and R2.
8. **Measured (build), TG68K and fx68k.** The R1 lines PASS (r_L = 2, one op = 1 sample). P1-FIXTURE, P1-F1, P1-R1 (reference and test run: 625 lines, hsync period 908, width 842 samples), P1-S3A (196 window lines) and P1-S3B-REF and P1-S3B-TEST PASS. Δ1 = Δ2 = +6 op on both cores, and P1-D1 and P1-D2 PASS. Contrast (`NMH_MUTATE=P1`, $0033 written to $DFF10C instead of $DFF102): Δ1 = Δ2 = 0 op, and the "+6 op" lines FAIL on both cores. Each precondition line has its contrast (selectors `P1-FIXTURE`, `P1-F1`, `P1-R1`, `P1-S3A`, `P1-S3B-REF`, `P1-S3B-TEST`): it prints FAIL on its own line on both cores, and P1-D1 and P1-D2 print NOT GRADED.

### P2 - weight and playfield of every BPLCON1 bit

1. **Proposition.** Each BPLCON1 bit, set alone, delays one playfield by 2^k SHRES pixels, where the bit's name in the AA p.16 table is PF1Hk or PF2Hk. PF1 is the odd planes and PF2 the even planes. Bit 00, now named PF1H2, keeps its old 140 ns weight.
   - Claims: `opus:AA-bplcon1.b00` to `opus:AA-bplcon1.b15`; `sol:AA-bplcon1.c23` to `sol:AA-bplcon1.c30`; `opus:AA-pf1h.c1`, `opus:AA-pf2h.c1`; `sol:AA-bplcon1.c12`, `opus:AA-bplcon1.c6`, `sol:AA-bplcon1.c5`. (`sol:AA-bplcon1.c9` and `c10` are recorded, not graded, section 2. `sol:AA-bplcon1.c15` to `c22` name bits 15-8 and also give their reset value; P5 carries them.)
   - Pages: AA p.16, p.17, p.5, p.3.
2. **Stimulus.** S1, S2, S3 with two planes; DMACON = $8380. BPLCON1 takes the value in the row's "value" column; the reference run writes BPLCON1 = $0000.
   - **(a1)** LORES (BPLCON0 = $2000), FMODE = $0000, bars. Used for rows 0-7, 9 and 13, whose delays lie within AA p.31's "1X Modes: LORES ... 0-63" SHRES pixels.
   - **(a2)** LORES, FMODE = $0003, bars. Used for rows 0-7, 9, 10, 11 and 13-15; AA p.31, "4X Modes: LORES 0-63 [LORES pixels] 0-255 [SHRES pixels]", puts every delay in range.
   - **(b)** SHRES (BPLCON0 = $2040), FMODE = $0000, stripes. Used for rows 8 and 12; AA p.31, "1X Modes: ... SHRES ... 0-15" SHRES pixels.
3. **Observation.** For (a1) and (a2), Δ1 and Δ2 on every window line of the observed frame. For (b), C1 and C2 and the displacement of the solid part's edge.
4. **Expected value.**

| bit | value | name | playfield | delay (35 ns units) | expected |
|---|---|---|---|---|---|
| 0 | $0001 | PF1H2 | PF1 | 4 | Δ1 = 2, Δ2 = 0 |
| 1 | $0002 | PF1H3 | PF1 | 8 | Δ1 = 4, Δ2 = 0 |
| 2 | $0004 | PF1H4 | PF1 | 16 | Δ1 = 8, Δ2 = 0 |
| 3 | $0008 | PF1H5 | PF1 | 32 | Δ1 = 16, Δ2 = 0 |
| 4 | $0010 | PF2H2 | PF2 | 4 | Δ1 = 0, Δ2 = 2 |
| 5 | $0020 | PF2H3 | PF2 | 8 | Δ1 = 0, Δ2 = 4 |
| 6 | $0040 | PF2H4 | PF2 | 16 | Δ1 = 0, Δ2 = 8 |
| 7 | $0080 | PF2H5 | PF2 | 32 | Δ1 = 0, Δ2 = 16 |
| 8 | $0100 | PF1H0 | PF1 | 1 | (b): C1 ≠ C1(ref) (COLOR01 and COLOR00 exchanged); C2 = C2(ref); Δ of the solid edge is 0 or 1 |
| 9 | $0200 | PF1H1 | PF1 | 2 | Δ1 = 1, Δ2 = 0 |
| 10 | $0400 | PF1H6 | PF1 | 64 | Δ1 = 32, Δ2 = 0 |
| 11 | $0800 | PF1H7 | PF1 | 128 | Δ1 = 64, Δ2 = 0 |
| 12 | $1000 | PF2H0 | PF2 | 1 | (b): C2 ≠ C2(ref); C1 = C1(ref) |
| 13 | $2000 | PF2H1 | PF2 | 2 | Δ1 = 0, Δ2 = 1 |
| 14 | $4000 | PF2H6 | PF2 | 64 | Δ1 = 0, Δ2 = 32 |
| 15 | $8000 | PF2H7 | PF2 | 128 | Δ1 = 0, Δ2 = 64 |

   - Rows run in both (a1) and (a2) must give the same Δ.
   - **Sources:**
     - AA p.16 table, for example "08 GAUD PF1H0=0", "00 ECSENA=0 PF1H2".
     - AA p.17: "PF2Hx= Playfield 2 horizontal scroll code,x=0-7 / PF1Hx= Playfield 1 horizontal scroll code,x=0-7 / where PFyH0=LSB=35nS=1 SHRES pixel(bits have been renamed,old PFyH0 now PFyH2,etc.)".
     - AA p.3: "BPLCON1 now contains an 8 bit scroll value for each of the playfields".
     - AA p.5: "Note that horizontal bit position representing 140nSec resolution has been changed to 3rd least significant bit,where before it used to be a field's LSB. For example, bit 00 in BPLCON1 used to be named PF1H0 and now it's called PF1H2."
     - AA p.16: "DPF=Double playfield (PF1=odd PF2=even bit planes)".
     - HRM p.85: "bits 3-0 for playfield 1 (bitplanes 1, 3, and 5) and bits 7-4 for playfield 2 (bitplanes 2, 4, and 6)".
     - Why the SHRES stripe works: a 35 ns shift moves the alternating pattern onto the other set of retained samples.
   - **Quantifiers:** every one of the 16 bits, every window line, and the "0" entries for the other playfield.
   - **Claims.** The claims that name the whole x = 0-7 code of a playfield (`opus:AA-pf1h.c1` for playfield 1, `opus:AA-pf2h.c1` for playfield 2) sit on one aggregate line per playfield (P2-PF1-CODE, P2-PF2-CODE), which holds when every row of that playfield holds. `sol:AA-bplcon1.c12` sits on P2-H2, which holds when the PF1H2 row and the PF2H2 row (every (a1) and (a2) line of both) hold. The row lines carry only the claims of their own bit, and the lines that compare (a1) with (a2) carry none. The run prints `sol:AA-bplcon1.c9` and `sol:AA-bplcon1.c10` on one RECORDED line: their restatement calls the code three bits, where AA p.3 and AA p.17 give eight, so they are not graded as written.
5. **Preconditions.** Each is one line over all the runs of its mode, and a row line rests on the lines of its own mode.
   - P2-FIXTURE and P2-F1: every reference and test run loaded, booted, stored the fixture, and located F1.
   - P2-R1: in every pair of a test run and its reference run, the R1 anchor holds.
   - P2-S3A: S3a in each reference run. P2-S3B: S3b, the edges, in every reference and test run.
   - P2-STRIPE: in every (b) run the alternating part shows one colour per playfield (S3b, stripes).
   - P2-STEP8: in every (a2) run, consecutive plane-1 fetches within each window line are 8 bytes apart (P7's measure, R2), which shows FMODE = $0003 took effect. *Contrast:* FMODE = $0000 gives 2 bytes and the precondition fails.
6. **Contrasting case.**
   - Writing a row's value to its neighbouring bit gives a different predicted Δ. For example $0002 in place of $0001 gives Δ1 = 4 instead of 2.
   - Writing $0000 gives Δ = 0 and flips no stripe. Either way the row's check goes red. Rows 8-15 use the $0000 write.
   - Preconditions: one contrast per precondition line (selectors `P2-FIXTURE`, `P2-F1`, `P2-R1`, `P2-S3A`, `P2-S3B`, `P2-STEP8`, `P2-STRIPE`).
7. **Reference frame.** R1, R2 and R3 (the window lines).
8. **Measured (build), TG68K and fx68k.** The preconditions P2-FIXTURE, P2-F1, P2-R1, P2-S3A, P2-S3B, P2-STEP8 and P2-STRIPE PASS on both cores. Every row line, every line that compares (a1) with (a2), P2-H2 and the aggregates P2-PF1-CODE and P2-PF2-CODE PASS on both cores, so each measured Δ equals the table's expected value exactly. Rows 8 and 12: the stripe colour of the row's playfield is exchanged (1 to 0), the other playfield's colour is unchanged, and the solid edge moves by 1 op. The lines are (a1) for rows 0-7, 9 and 13, (a2) for rows 0-7, 9-11 and 13-15, (b) for rows 8 and 12, and the ten lines that compare (a1) with (a2).

| Row (bit) | Value | Name | Expected | Lines graded (both cores) |
|---|---|---|---|---|
| 0 | $0001 | PF1H2 | Δ1=2,Δ2=0 | PASS (a1), PASS (a2) |
| 1 | $0002 | PF1H3 | Δ1=4,Δ2=0 | PASS (a1), PASS (a2) |
| 2 | $0004 | PF1H4 | Δ1=8,Δ2=0 | PASS (a1), PASS (a2) |
| 3 | $0008 | PF1H5 | Δ1=16,Δ2=0 | PASS (a1), PASS (a2) |
| 4 | $0010 | PF2H2 | Δ1=0,Δ2=2 | PASS (a1), PASS (a2) |
| 5 | $0020 | PF2H3 | Δ1=0,Δ2=4 | PASS (a1), PASS (a2) |
| 6 | $0040 | PF2H4 | Δ1=0,Δ2=8 | PASS (a1), PASS (a2) |
| 7 | $0080 | PF2H5 | Δ1=0,Δ2=16 | PASS (a1), PASS (a2) |
| 8 | $0100 | PF1H0 | stripe flips | PASS (b) |
| 9 | $0200 | PF1H1 | Δ1=1,Δ2=0 | PASS (a1), PASS (a2) |
| 10 | $0400 | PF1H6 | Δ1=32,Δ2=0 | PASS (a2) |
| 11 | $0800 | PF1H7 | Δ1=64,Δ2=0 | PASS (a2) |
| 12 | $1000 | PF2H0 | stripe flips | PASS (b) |
| 13 | $2000 | PF2H1 | Δ1=0,Δ2=1 | PASS (a1), PASS (a2) |
| 14 | $4000 | PF2H6 | Δ1=0,Δ2=32 | PASS (a2) |
| 15 | $8000 | PF2H7 | Δ1=0,Δ2=64 | PASS (a2) |

Contrasts (selectors `P2A1-<name>`, `P2A2-<name>`, `P2B-<name>`, `P2S-<name>` in the test source): for rows 0-7 the neighbouring bit is written in place of the row's own; for rows 8-15 BPLCON1 = $0000 is written. Each contrast prints FAIL on its row's line on both cores. `P2-CODE` runs every row, with the neighbouring bit written in the (a1) runs of rows PF1H2 and PF2H2, and prints FAIL on both aggregates and on P2-H2; `P2-H2` runs the rows PF1H2 and PF2H2 only, writes the neighbouring bit $0008 in place of $0010 in both runs of PF2H2, and prints FAIL on P2-H2 and on the two PF2H2 lines. Each precondition line has its contrast (selectors `P2-FIXTURE`, `P2-F1`, `P2-R1`, `P2-S3A`, `P2-S3B`, `P2-STEP8`, `P2-STRIPE`), which prints FAIL on its own line on both cores and NOT GRADED on the row lines of its mode.

### P3 - the scroll range is quadrupled

1. **Proposition.** In LORES with 64-bit bit-plane fetch, each playfield's delay reaches 254 and 255 SHRES pixels, that is 63.5 and 63.75 LORES pixels. The earlier maximum was 15 LORES pixels.
   - Claims: `opus:AA-pf1h.c3`, `sol:AA-bplcon1.c13`.
   - Pages: AA p.17, p.31, p.3.
2. **Stimulus.** As P2(a2). BPLCON1 takes these values; the reference is $0000.

| BPLCON1 | Bits set | Playfield value |
|---|---|---|
| $0E0F | bits 0-3, 9, 10, 11 | PF1 = $FE |
| $0F0F | as above plus bit 8 | PF1 = $FF |
| $E0F0 | bits 4-7, 13, 14, 15 | PF2 = $FE |
| $F0F0 | as above plus bit 12 | PF2 = $FF |

3. **Observation.** Δ1 and Δ2 on every window line.
4. **Expected value.**

| BPLCON1 | Δ1 (op) | Δ2 (op) |
|---|---|---|
| $0E0F | 127 | 0 |
| $0F0F | 127 or 128 | 0 |
| $E0F0 | 0 | 127 |
| $F0F0 | 0 | 127 or 128 |

   - For $0F0F and $F0F0 the 35 ns bit is not resolved in LORES; see `sol:AA-position-resolution.c1` in section 1.
   - **Sources:**
     - AA p.17: "Note that the scroll range has been quadrupled to allow for wider (32 or 64 bit) bitplanes."
     - AA p.31: "4X Modes: ... LORES 0-63 0-255".
     - AA p.3: "scroll can delay playfield thru 32 bus cycles" (32 × 280 ns = 256 × 35 ns).
     - AA p.31: "Pre AA scrolling was in 140ns increments", which with the 4-bit field of HRM p.85 ("0 to 15 bits of delay") gives the old maximum.
5. **Preconditions.** As P2(a2).
6. **Contrasting case.** BPLCON1 = $060F, with PF1H7 cleared. The page predicts Δ1 = 63, so a check expecting 127 goes red.
7. **Reference frame.** R1.
8. **Measured (build), TG68K and fx68k.** The preconditions P3-FIXTURE, P3-F1, P3-R1, P3-S3A, P3-S3B and P3-STEP8 PASS on both cores. TG68K: Δ1 = 127 op (BPLCON1 = $0E0F), Δ1 = 128 ($0F0F), Δ2 = 127 ($E0F0), Δ2 = 128 ($F0F0). fx68k: 127 in all four. All four lines and P3-AGG PASS on both cores (the expected value for $0F0F and $F0F0 is 127 or 128). Contrast (`NMH_MUTATE=P3`, PF1H7 cleared in the two PF1 test runs and PF2H7 in the two PF2 test runs, so that the delays are 63 op): all five lines FAIL on both cores. Each precondition line has its contrast (selectors `P3-FIXTURE`, `P3-F1`, `P3-R1`, `P3-S3A`, `P3-S3B`, `P3-STEP8`), which prints FAIL on its own line on both cores and NOT GRADED on the four row lines and P3-AGG.

### P4 - RST_ values of BPLCON1 bits 15-8

1. **Proposition.** After reset, and before any write to BPLCON1, bits 15-8 of BPLCON1 are 0: PF2H7, PF2H6, PF2H1, PF2H0, PF1H7, PF1H6, PF1H1 and PF1H0. Bits 7-0 carry no "=" mark in the AA p.16 table and have no stated reset value.
   - Claims: `opus:AA-bplcon1.b08.rst` to `opus:AA-bplcon1.b15.rst`. (`sol:AA-bplcon1.c15` to `c22` also name the bits; P5 carries them.)
   - Page: AA p.16.
2. **Stimulus.** Two runs. In each, the program writes everything that S1, S2 and S3 require except BPLCON1. The Copper list contains no MOVE to $102.
   - **Run (a).** LORES, FMODE = $0003, bars.
     - F3 is observed with BPLCON1 in its reset state.
     - After the VERTB that starts F5, the CPU writes BPLCON1 = $0000.
     - F5 is observed as the reference.
   - **Run (b).** SHRES, FMODE = $0003, stripes.
     - The same schedule as run (a).
     - The reset delay can reach 62 SHRES pixels, which is within AA p.31's "4X Modes: ... SHRES ... 0-63".
3. **Observation.**
   - Run (a): Δ1 and Δ2 of F3 relative to F5 on every window line.
   - Run (b): C1 and C2 in F3 and in F5.
4. **Expected value.**
   - **Run (a):** Δ1 and Δ2 are each one of 0, 2, 4, …, 30 op, and the same on every line. A playfield's line is graded only where run (b) shows its PFnH0 = 0 (P4b-C1 for playfield 1, P4b-C2 for playfield 2): a delay of an odd number of 35 ns steps is rounded to a whole number of op by the sampling, and which way it rounds is not set by the pages. Otherwise the line prints NOT GRADED.
     - The undetermined bits (3-0 for PF1, 7-4 for PF2) contribute a multiple of 4 SHRES pixels (2 op), up to 60 SHRES pixels (30 op).
     - PFnH1 = 1 would make Δ odd.
     - PFnH6 = 1 or PFnH7 = 1 would add at least 32 op.
   - **Run (b):** C1(F3) = C1(F5) and C2(F3) = C2(F5), each frame's colour taken relative to its own solid edge (S3b).
     - Delays that are multiples of 2 SHRES pixels do not change which pixels the output drops.
     - PFnH0 = 1 would change them.
   - **Source:** AA p.16, "=0/=1 bit values initialized by RST_ pin going low.", together with the BPLCON1 column "PF2H7=0 … PF1H0=0".
5. **Preconditions.**
   - The run starts at the harness's power-on reset, which is taken to drive LISA's RST_ input (AA p.2: "A RST_ input pin has been added").
   - Nothing writes BPLCON1 before F5. This is shown by the program text and the Copper list.
   - Bars or stripes are visible in F3, and S3a and S3b hold in F5.
   - *Contrast for the visibility precondition:* with BPLEN cleared in F3, no bar is visible and the precondition fails.
6. **Contrasting case.** The CPU writes, before F3:
   - $0200: the page predicts Δ1 = 1, which is odd, so red.
   - $0400: Δ1 = 32, so red.
   - $0100 in run (b): C1 changes, so red.
7. **Reference frame.** R1.
8. **Measured (build), TG68K and fx68k.** The preconditions P4-FIXTURE, P4-F1, P4-R1, P4-S3A, P4-S3B-A, P4-S3B-B and P4-SIG PASS on both cores. Run (b): the stripe colour of both playfields is 0 in F3 (reset state) and 1 in F5 (BPLCON1 = $0000), so P4b-C1 and P4b-C2 FAIL on both cores: after reset neither playfield's delay is a multiple of 2 SHRES pixels, while AA p.16 marks PF1H0 (bit 08) and PF2H0 (bit 12) "=0". Run (a): P4a-D1 and P4a-D2 print NOT GRADED on both cores, because run (b) does not show PF1H0 = 0 and PF2H0 = 0. The F3-against-F5 delays print as measured: Δ1 = Δ2 = 2 op on TG68K and 1 op on fx68k. The 35 ns bits are not resolved in LORES (see `sol:AA-position-resolution.c1` in section 1), so the two cores round the same delay to different whole ops. Contrasts: P4a-D1 (`NMH_MUTATE=P4a-D1`, BPLCON1 = $0200 written before F3 in run (a), and $0000 in run (b), so that P4b-C1 holds) and P4a-D2 ($2000) FAIL on both cores. The red lines have live inputs through the same comparators: with BPLCON1 = $0000 written before F3 in run (b) (`NMH_MUTATE=P4b-C1`, `P4b-C2`) both run (b) lines print PASS on both cores. Each precondition line has its contrast (selectors `P4-FIXTURE`, `P4-F1`, `P4-R1`, `P4-S3A`, `P4-S3B-A`, `P4-S3B-B`, `P4-SIG`). Implementation record: built against a private copy of the design, outside the repository, in which BPLCON1 resets to $0000, the P4 preconditions, P4a-D1, P4a-D2, P4b-C1, P4b-C2 and P5 all PASS on both cores (`NMH_MUTATE=P5-NONE`).

### P5 - BPLCON1 bits 15-8: the tabled name and the reset value together

1. **Proposition.** Each of BPLCON1 bits 15-8 is the field the AA p.16 table names for it, with the weight P2 grades, and RST_ initialises it to 0. Bits 12 (PF2H0) and 8 (PF1H0) are graded together on the line P5, through P2's rows 12 and 8 and P4's run (b). Each of the other six bits (15, 14, 13, 11, 10, 9) has a line of its own, through its P2 row and the P4 run (a) result of its playfield.
   - Claims: `sol:AA-bplcon1.c18` (bit 12) and `sol:AA-bplcon1.c22` (bit 8) on the line P5; `sol:AA-bplcon1.c15` (bit 15, PF2H7), `c16` (bit 14, PF2H6), `c17` (bit 13, PF2H1), `c19` (bit 11, PF1H7), `c20` (bit 10, PF1H6) and `c21` (bit 9, PF1H1) on the lines P5-PF2H7, P5-PF2H6, P5-PF2H1, P5-PF1H7, P5-PF1H6 and P5-PF1H1. Each claim states both the bit's name and its reset value, so its line requires both.
   - Page: AA p.16.
2. **Stimulus.** None of its own: the runs of P2's rows 8-15 and the runs of P4.
3. **Observation.** The verdicts of P2's rows 8-15 and of P4, from the same runs.
4. **Expected value.**
   - P5: P2's rows 12 and 8 hold, and P4b-C2 and P4b-C1 hold.
   - P5-PF2H7, P5-PF2H6, P5-PF2H1: P2's row of the bit holds and P4a-D2 holds. P5-PF1H7, P5-PF1H6, P5-PF1H1: P2's row of the bit holds and P4a-D1 holds. P4a-D1 and P4a-D2 each cover the three bits of their playfield in one delay result, so the three lines of a playfield rest on the same reset result.
   - **Source:** AA p.16, the BPLCON1 column from "15 … PF2H7=0" to "08 … PF1H0=0", and "=0/=1 bit values initialized by RST_ pin going low."
5. **Preconditions.** Those of P2 for the bit's row and those of P4. A line of the six is graded only where P4a-D1 (playfield 1) or P4a-D2 (playfield 2) is graded, which needs P4b-C1 or P4b-C2 to show PFnH0 = 0; otherwise it prints NOT GRADED. P5 needs P4's run (b) graded and P2's rows 8 and 12.
6. **Contrasting case.** Each contrast of P2's rows and of P4 turns the lines that rest on it red through the same clause. Examples: $0200 written before F3 in P4 (Δ1 = 1, odd) turns P4a-D1 and the three PF1 lines red.
7. **Reference frame.** R1, R2 and R3, as in P2 and P4.
8. **Measured (build), TG68K and fx68k.** P5: FAIL on both cores, through run (b): P2's rows 12 and 8 PASS, and the stripe colours of run (b) differ between F3 and F5 (P4b-C1 and P4b-C2 FAIL). The six lines of the other bits print NOT GRADED on both cores, because P4a-D1 and P4a-D2 do. Live input (`NMH_MUTATE=P5`, the P4 runs write BPLCON1 = $0000 before F3): P5 and all six lines print PASS on both cores, with P4b-C1, P4b-C2, P4a-D1 and P4a-D2. The contrasts `NMH_MUTATE=P4a-D1` (BPLCON1 = $0200 written before F3 in run (a), $0000 in run (b)) and `P4a-D2` ($2000) turn P5-PF1H7, P5-PF1H6 and P5-PF1H1 (respectively P5-PF2H7, P5-PF2H6 and P5-PF2H1) red together with the reset result they rest on, and leave P5 and the other playfield's three lines PASS.

### P6 - BYPASS sends the priority-selected colour address out on R(7:0)

1. **Proposition.** With BYPASS set in BPLCON0, the colour address chosen by the priority logic leaves the chip on R(7:0) instead of the colour table's red value, while scrolling and priority stay as without BYPASS. The visible part is that the red output shows address bits 7-4.
   - Claim: `sol:AA-bplcon0.c19`, on the line P6-I (scrolling and priority unchanged). (`opus:AA-bypass.c1` and `opus:AA-bypass.c2` put all 8 address bits on R(7:0), and the 4-bit red output shows only one nibble; they are declined in section 1.)
   - Pages: AA p.4, p.17.
2. **Stimulus.** S1, S2 with these values:
   - LORES, eight planes, FMODE = $0000.
   - BPLCON0 = $0010 (B0 runs: BPU = 8, AA p.16 "BPUx =Bit plane use code 0000-1000") or $0030 (B1 runs: BPU = 8 plus BYPASS, bit 5).
   - BPLCON1 = $0033.
   - Sprite 0 by DMA, placed in front of the playfields (BPLCON2 = $0024, S2): SPR0POS = $6060 (lines $60-$6F, SH10-SH3 = $60), SPR0CTL = $7000, 16 lines of DATA = $FFFF and DATB = $0000, then 0, 0 (AA p.28). Its colour address is $11: ESPRM = 0001 puts even-sprite colours at $10-$1F (AA p.18).
   - DMACON = $83A0.
   - **Plane data.** Every line holds 20 groups g = 0…19 of 16 pixels. The pixel value is v(g) = ((g mod 16) << 4) | (15 - (g mod 16)), which is never 0. Plane k holds bit k-1 of v.
   - **Colour tables.** 256 entries are loaded 32 at a time. For bank b = 0…7: BPLCON3 = $0C00 | b<<13, then COLOR00…COLOR31 = entries 32b…32b+31. The bank bits are BANK2-0 of AA p.5. Afterwards BPLCON3 = $0C00. Entries are written with LOCT=0, which sets both nibbles (AA p.19).
     - T_id: entry i has red nibble i>>4, green nibble i & $F and blue nibble $F - (i>>4).
     - T_x: entry i has red nibble $F - (i>>4), green nibble i & $F and blue nibble i>>4.
   - **Runs:** B0/T_id, B1/T_id, B0/T_x and B1/T_x, plus B0/T_id with BPLCON1 = $0000 for the scroll precondition.
3. **Observation.** The red channel of every column on window lines $2C-$EF of F3. The window columns are those whose colour differs from COLOR00's in B0/T_id; v is never 0, so the window interior never uses COLOR00.
4. **Expected value.**
   - (ii) red(B1/T_x) = red(B1/T_id) at every observed pixel: the table is not used.
   - (i) red(B1/T_id) = red(B0/T_id) at every observed pixel. T_id maps address i to red i>>4, so the B0 image shows the high nibble of the address that scrolling and priority selected. (i) is graded only where (ii) holds: with a BYPASS that leaves the red output to the colour table, B1/T_id and B0/T_id are the same run and (i) holds without showing anything about scrolling or priority. Where (ii) does not hold, P6-I prints NOT GRADED.
   - P6-AGG holds when (i) and (ii) hold; it carries no claim id. P6-I carries `sol:AA-bplcon0.c19`, whose content (scrolling and priority as without BYPASS) it grades; it is graded only where P6-II holds.
   - **Sources:**
     - AA p.17: "BYPASS= bitplanes are scrolled and prioritized normally, but bypass color table and 8 bit wide data appear on R(7:0)."
     - AA p.4: "BYPASS bit in BPLCON0 allows prioritized color address to bypass the color table and exit LISA on R(7:0) bus."
   - **Not graded:** green and blue in B1 runs (not stated) and address bits 3-0 (not visible).
5. **Preconditions.**
   - R1: the five R1 anchor lines hold, because the sprite width (32 op) and the scroll (6 op) are counted in the samples per op that R1 measures.
   - R4 shows R7-R4.
   - red(B0/T_x) ≠ red(B0/T_id) at every observed pixel, which shows the table matters without BYPASS. This holds because $F - n ≠ n for every nibble n.
   - B0/T_id shows all 16 red values in the window.
   - On lines $60-$6F, sprite 0's 32 columns show red 1 in B0/T_id, including over groups whose high nibble is not 1. This shows the sprite's address takes part. The sprite is 16 pixels wide, and "In lowres mode, 1 sprite pixel is 1 bitplane pixel wide" (AA p.28-29), so it covers 16 × 140 ns = 32 op.
   - The group boundaries in B0/T_id lie 6 op to the right of those in the B0/T_id run with BPLCON1 = $0000, which shows the scroll is in effect.
   - *Contrasts:*
     - T_x in the B0/T_id run breaks the table precondition.
     - SPREN cleared removes the sprite columns.
     - BPLCON1 = $0000 gives a 0 op shift.
6. **Contrasting case.**
   - Evaluate B0/T_x against expectation (ii): it differs at every pixel, so the check goes red.
   - A B1 run with BPLCON1 = $0000, compared with B0/T_id at $0033, differs at the group boundaries, so (i) goes red through the scroll clause. The B1/T_x run is run as B0/T_id with the same BPLCON1, so that (ii) holds and (i) is graded.
   - A B1/T_id run with BPLCON2 = $0000, compared with B0/T_id at $0024, differs on the sprite's columns wherever the group's high nibble is not 1, so (i) goes red through the priority clause. The B1/T_x run is run as B0/T_id with the same BPLCON2, so that (ii) holds and (i) is graded. PF2P = 000 puts the playfield in front of all sprites (HRM p.210, Table 7-2), and v is never 0, so the playfield covers the sprite.
7. **Reference frame.** R1 and R4.
8. **Measured (build), TG68K and fx68k.** Preconditions: the five R1 lines, P6-R4, P6-FIXTURE, P6-F3, P6-R1, P6-POP, P6-TBL, P6-RED16, P6-SPR and P6-SCROLL PASS. R4: a COLOR01 pixel shows red $A. Population: 392 lines of F3 with 634 window columns a line on TG68K and 633 on fx68k. Table: 248528 of 248528 observed pixels differ between B0/T_x and B0/T_id (fx68k 248136 of 248136). Sprite: 32 output lines carry sprite columns. Scroll: 19 group boundaries, each 6 samples (6 op) right of its unscrolled position. (ii): 248528 of 248528 pixels differ between B1/T_x and B1/T_id on TG68K (fx68k 248136 of 248136), so P6-II and the aggregate P6-AGG are FAIL on both cores: with BYPASS set the red output follows the colour table and not the address. (i) prints NOT GRADED on both cores, because (ii) does not hold, so `sol:AA-bplcon0.c19` (on P6-I) is not asserted on either core; the comparison would find 0 of 248528 pixels differing between B1/T_id and B0/T_id on TG68K (fx68k 0 of 248136), the two runs being alike. Contrasts: `P6-I` (BPLCON1 = $0000 in the B1/T_id run) and `P6-I-PRIO` (BPLCON2 = $0000), each with the B1/T_x run run as B0/T_id with the same register so that (ii) holds, print FAIL on P6-I on both cores. The red lines have a live input through the same comparators (`P6-II`, `P6-AGG`: the B1/T_x run is run as B0/T_id): P6-II, P6-I and P6-AGG print PASS on both cores.

### P7 - bit-plane fetch increment from FMODE bits 1-0

1. **Proposition.** The bit-plane fetch increment, the step of the bit-plane pointer from one fetch to the next, is:
   - 2 bytes with FMODE bits 1-0 = 00;
   - 4 bytes with 01;
   - 4 bytes with 10;
   - 8 bytes with 11.

   FMODE bits 3-2 do not change it. FMODE is written at $1FC.
   - Claims: `sol:AA-fmode-bitplane-fetch.c1`, `c4`, `c7`, `c10`; `sol:AA-fmode.c5`. (The CAS, bus-width and bit-name claims of the same table are declined in section 1.)
   - Pages: AA p.23, p.3.
2. **Stimulus.**
   - S1, S2; LORES, two planes (BPLCON0 = $2000); BPL1MOD = BPL2MOD = 0.
   - Plane buffers at P_1 = $120000 and P_2 = $130000 (section 0, Addresses).
   - The word at byte offset o holds $8000 | (o/2 & $7FFF), so the data identifies its offset.
   - The pointers are reloaded once per frame by the Copper in vertical blank. AA p.15: "must be reinitalized by the processor or coprocessor to point to the beginning of Bit Plane data every vertical blank time".
   - FMODE is written by the CPU to $DFF1FC before DMA is enabled, with one of $0000, $0001, $0002, $0003 or $000C.
   - DMACON = $8380.
3. **Observation.** All bit-plane accesses in [P_1, P_1+$8000) and [P_2, P_2+$8000) during F3, grouped into fetches (R2).
4. **Expected value.** For each plane, with n = 2, 4, 4, 8 and 2 bytes for the five FMODE values:
   - The first fetch of the frame is at P_k.
   - A_{j+1} = A_j + n for consecutive fetches, both within a line and from the last fetch of a line to the first of the next. The modulo is 0, and AA p.16 says "A Modulo is a number that is automatically added to the address at the end of each line".
   - **Sources:**
     - AA p.23 table, column "Bitplane Fetch Increment": "0 0 by 2 bytes (as before)", "0 1 by 4 bytes", "1 0 by 4 bytes", "1 1 by 8 bytes".
     - AA p.23: "03 SPAGEM", "02 SPR32", "01 BPAGEM", "00 BPL32".
     - AA p.3: "Bits BPAGEM and BPL32 in new register FMODE control size of bitplane data in BPL1DAT thru BPL8DAT".
   - **Not graded:** how many bytes one fetch moves. The port shows one word address per access, and the harness serves the three words after the read address on `chip48` on every read (R2). Nor which of bits 1 and 0 is the page-mode bit (section 1).
5. **Preconditions.**
   - In every run, both planes are fetched on at least 10 window lines, with at least 2 fetches per line.
   - The R2 line boundaries and slots are found (the harness reports no slot; R2 derives them).
   - *Contrast:* BPLEN cleared gives no fetches, and the precondition fails.
6. **Contrasting case.** The runs are each other's contrasts. The $0000 run fails the n = 8 expectation of the $0003 run, and the $0003 run fails n = 2.
7. **Reference frame.** R2.
8. **Measured (build), TG68K and fx68k (identical).** R2: K = 8 clk_sys cycles per CCK, consecutive lines 227 CCK apart. FMODE = $0000 and $000C: the first fetch of each plane is at its buffer address and every step is 2 bytes (3919 pairs per plane). FMODE = $0001 and $0002: every step is 4 bytes (2155 pairs per plane). FMODE = $0003: every step is 8 bytes (1175 pairs per plane). All five row lines and the FMODE-write line PASS. Each row's contrast (its FMODE written with a value the page gives another increment, selectors `P7-F0000` to `P7-F000C`) prints FAIL on that row's line on both cores; `P7-FMODE-W` writes $0000 in the $0001 run and prints FAIL on the write line and on the $0001 row.

### P8 - modulos in normal mode

1. **Proposition.** With bit-plane scan-doubling off, at the end of each line every odd plane's pointer is advanced by BPL1MOD, and every even plane's pointer by BPL2MOD, beyond the word after the last one fetched.
   - Claims: `opus:AA-bpl1mod.c5`, `sol:AA-bplmod.c1`, `sol:AA-bplmod.c2`. (`opus:AA-bpl1mod.c6`, `sol:AA-bplmod.c3`, `sol:AA-bplmod.c4` state a permission with no expected value and are declined in section 1. They are the reason the two modulos differ here.)
   - Page: AA p.16.
2. **Stimulus.**
   - S1, S2; LORES, eight planes (BPLCON0 = $0010); FMODE = $0000.
   - Plane k (k = 1…8) at P_k = $100000 + (k-1)·$4000 (section 0, Addresses), with the data encoded as in P7.
   - The pointers are reloaded once per frame by the Copper.
   - Run M: BPL1MOD = $0008, BPL2MOD = $0018. Run M': BPL1MOD = $0018, BPL2MOD = $0008.
   - DMACON = $8380.
3. **Observation.** Bit-plane fetches of every plane on window lines $2C-$EF of F3, and the line boundaries (R2).
4. **Expected value.** For every plane k and every pair of consecutive window lines n, n+1, the step first(k, n+1) - last(k, n) is:

| Run | Odd planes (1, 3, 5, 7) | Even planes (2, 4, 6, 8) |
|---|---|---|
| M | 2 + 8 = 10 | 2 + 24 = 26 |
| M' | 26 | 10 |

   - Within a line, consecutive fetches are 2 bytes apart.
   - **Sources:**
     - AA p.16: "These registers contain the Modulos for the odd and even bit planes. A Modulo is a number that is automatically added to the address at the end of each line, in order that the address then points to the start of the next line."
     - The "+2" is the pointer's own advance past the last fetched word: AA p.23 row 00, "by 2 bytes (as before)"; HRM p.84, "At this point, the bitplane pointers contain the value START+42. Adding the modulo of 38 gives the correct starting point for the next line."
   - **Quantifiers:** every odd plane, every even plane, every line pair.
5. **Preconditions.**
   - All eight planes are fetched on every window line, with the same number of fetches on each line, at least 2.
   - *Contrast:* BPU = 4 (BPLCON0 = $4000) removes planes 5-8, and the precondition fails.
6. **Contrasting case.** M' against M's expectation: every step differs, so the check goes red.
7. **Reference frame.** R2 and R3 (a marker at line $30 confirms the window's first line).
8. **Measured (build), TG68K and fx68k (identical).** R3: the markers at $30 and $80 are 80 line boundaries apart. In F3 all eight planes are fetched on the same 196 lines, 20 fetches per line per plane. Run M: 780 plane-line pairs step 10 bytes and 780 step 26 bytes, odd planes 10 and even planes 26; run M': the same counts with the planes exchanged. Both lines PASS. Contrasts: `P8-M` writes BPL1MOD = $0018 and BPL2MOD = $0008 in run M, and `P8-Mp` writes $0008 and $0018 in run M'; each prints FAIL on its line on both cores.

### P9 - one modulus per line with bit-plane scan-doubling

1. **Proposition.** With BSCAN2 set in FMODE, every plane, odd and even, takes the same modulus on a given line. The modulus is BPL1MOD on a line whose vertical-counter bit V0 equals DIWSTRT's V0, and BPL2MOD on the other lines.
   - Claims: `opus:AA-bpl1mod.c7`, `opus:AA-bpl1mod.c8`, `sol:AA-bplmod.c5`, `c6`, `c7`, `c8` on P9-AGG; `opus:AA-bitplanes.c6` and `sol:AA-dual-playfield.c3` (both playfields share the modulus under scan-doubling) on P9-DP.
   - Pages: AA p.16, p.3, p.23.
2. **Stimulus.** As P8 run M, with FMODE = $4000. Bit 14 is BSCAN2, which "enables use of 2nd P/F modulus on an alternate line basis to support bitplane scan-doubling" (AA p.23). Copper markers (R3) sit at lines $30 and $80. Runs:

| Run | DIWSTRT | FMODE | Copper change |
|---|---|---|---|
| D0 | $2C81 | $4000 | none |
| D1 | $2D81 | $4000 | none |
| D2 | $2C81 | $4000 | The list starts with MOVE DIWSTRT,$2C81 (vertical blank) and contains WAIT($60,$06) followed by MOVE DIWSTRT,$2D81. |
| DP | $2C81 | $4000 | none; BPLCON0 = $0410: DPF (bit 10) set, so the odd planes are playfield 1 and the even planes playfield 2 (AA p.16: "DPF=Double playfield (PF1=odd PF2=even bit planes)") |
| N | $2C81 | $0000 | none (contrast) |

3. **Observation.** step(k, n) = first(k, n+1) - last(k, n) for every plane k and every pair of consecutive window lines, with absolute line numbers from R3.
4. **Expected value.** The modulus added at the end of line n is the one line n's own designation selects. AA p.16 says a modulo "is automatically added to the address at the end of each line", and AA p.3 says that when the V0 bits match, "BPL1MOD contains the modulus for the display line". Together they put line n's designation on the addition made at the end of line n.

| Run | Step 10 (BPL1MOD) on | Step 26 (BPL2MOD) on | Not graded |
|---|---|---|---|
| D0 | even n | odd n | - |
| D1 (lines $2D-$EE) | odd n | even n | - |
| D2 | even n for n ≤ $5F; odd n for n ≥ $61 | odd n for n ≤ $5F; even n for n ≥ $61 | n = $60 |
| DP | even n | odd n | - |

   These hold for all k.
   - **Also graded:**
     - On every line, all eight planes have the same step; in run DP, the odd planes (playfield 1) and the even planes (playfield 2) alike. P9-DP carries `opus:AA-bitplanes.c6` and `sol:AA-dual-playfield.c3`: with DPF set and BSCAN2 set, both playfields take the same modulus on every line.
     - Consecutive steps alternate between 10 and 26, except that in D2 two consecutive lines around $60 share a step.
   - **Sources:**
     - AA p.16: "If bitplane scan-doubling is enabled, BPL1MOD serves as the primary bitplane modulus and BPL2MOD serves as the alternate. Lines whose LSBs of beam counter and DIWSTRT match are designated primary, whereas lines whose LSBs don't match are designated alternate."
     - AA p.3: "When V0 bit of DIWSTRT matches V0 of vertical beam counter, BPL1MOD contains the modulus for the display line, else BPL2MOD is used. When scan-doubled both odd and even bitplanes use the same modulus on a given line, whereas in normal mode odd bitplanes used BPL1MOD and even bitplanes used BPL2MOD."
     - AA p.3: "scan-doubling is enabled, in which case both playfields share the same bitplane modulus register."
   - **Not graded:** "As a result Dual Playfield screens will probably not display correctly when scan-doubled" (AA p.3) says "probably" and gives no value. Whether DPF takes effect on the display is not observed in this proposition: bit-plane DMA does not show it, so P9-DP grades the modulus the planes of both playfields take with the bit written.
5. **Preconditions.**
   - R3 anchor.
   - All eight planes are fetched on every window line in every run, including across the DIWSTRT write in D2.
   - The first fetch line is $2C in D0, D2 and DP and $2D in D1. AA p.21: "The vertical Bit Plane DMA timing is identical to the Display windows".
   - *Contrast:* DIWSTRT = $2E81 moves the first fetch line to $2E, and the "first line" precondition of D0 fails.
6. **Contrasting case.** Evaluate run N against D0's expectation. There, odd planes step 10 and even planes step 26 on every line, so the check goes red.
7. **Reference frame.** R2 and R3. The anchor is the beam counter's V0, obtained through R3 line numbers.
8. **Measured (build), TG68K and fx68k (identical).** D0: 196 window lines, 1560 graded plane-line pairs, step 10 bytes in 784 and 26 bytes in 776. D1: 195 lines, 1552 pairs, 776 and 776. D2: 196 lines, 1552 pairs (n = $60 excluded), 776 and 776. DP (BPLCON0 = $0410): 196 lines, 1560 graded plane-line pairs, step 10 bytes in 784 and 26 bytes in 776, as D0. The three run lines, P9-DP and the aggregate PASS. Contrasts: `P9-D0`, `P9-D1`, `P9-D2` and `P9-DP` write FMODE = $0000 (BSCAN2 clear) in the run and print FAIL on its line; `P9-AGG` exchanges D1's two modulos and prints FAIL on the aggregate.

### P10 - LPEN is off after power-up

1. **Proposition.** After power-on, before BPLCON0 is written, the light-pen latch is off: VPOSR and VHPOSR keep counting through the start of vertical blank.
   - Claim: `opus:AA-lpen.c1`.
   - Page: AA p.17.
2. **Stimulus.**
   1. S1 step 1.
   2. BEAMCON0 = $0020: PAL, and LPENDIS = 0. AA p.12: "When this bit is a low and LPE (BPLCON0,BIT 3) is enabled, the light-pen latched value (beam hit position) will be read by VHPOSR, VPOSR and HHPOSR".
   3. No write to BPLCON0.
   4. Three times:
      1. INTREQ = $0020.
      2. Poll INTREQR bit 5 (R5).
      3. Store a marker word. Its port time t_V marks the start of vertical blank.
      4. Read VPOSR as a long word, giving w0 = VPOSR:VHPOSR.
      5. Wait in a fixed loop of at least 40 CCK.
      6. Read again, giving w1.
      7. Store w0 and w1.

   The test leaves the fire bit (pin 6) of both `joystick0` and `joystick1` at the harness's reset level, 0, so that no light-pen trigger should occur. Pin 6 is the light pen's "Beam trigger" (HRM p.228, Table 8-1). Which input level is an open switch is a property of the system boundary that the pages do not give for the port (HRM p.232's "shorted to ground" is the connector's level), so precondition (iv) measures it: each run begins by reading CIAA PRA ($BFE001) and storing bits 7-6, /FIR1 and /FIR0 (HRM p.339, CIAA address map, "pra /FIR1 /FIR0"), which read 1 when no fire button is closed.
3. **Observation.** The three stored pairs.
4. **Expected value.**
   - w1 ≠ w0 in every pair: the counter advanced.
   - w0's vertical position is below 20. The vertical position is VPOSR bit 0 (V8) and VHPOSR bits 15-8 (V7-V0), AA p.29.
   - **Source:** AA p.17: "LPEN =Light pen enable(reset on power up)".
   - **Why equal reads would mean LPEN = 1:** HRM p.239: "Once the light pen input is enabled and the light pen issues a trigger signal, the value in VPOSR is frozen. If no trigger is seen, the counters latch at the end of the display field. ... This freeze is released at the end of internal vertical blanking (vertical position 20)."
5. **Preconditions.**
   - (i) The reads fall within vertical blank before line 20: fewer than 19 R2 line boundaries lie between t_V and the store of w1. VERTB is "Start of Vertical blank" (AA p.25), and the internal vertical blanking period is "0 < V < 20" (HRM p.239: "the internal vertical blanking period of 0 < V20"). This is measured from port times, not from w0, because a latched counter (the clause failing) would make w0 show the latched position.
   - (ii) In this harness, with the same BEAMCON0, the latch does freeze the counter; contrast C below shows it. If C does not freeze, the proposition cannot be graded: either the latch is absent or LPENDIS does not act as written, and the pages do not say which.
   - (iii) Nothing writes BPLCON0 before the reads, as the program text shows.
   - (iv) Boundary precondition (P10-FIRE): CIAA PRA bits 7 and 6 read 1 at the start of both runs, so neither fire button is closed. *Contrast:* both fire inputs (bit 4 of `joystick0` and `joystick1`) driven to 1 from the start make bits 7 and 6 read 0 and the precondition fails.
   - *Contrast for (i):* in run C, place the reads about 110 R2 line boundaries after t_V. Precondition (i) fails there. The freeze has been released at line 20, so w1 ≠ w0, which shows the window below 20 matters.
6. **Contrasting case C.** The same program with BPLCON0 = $0008 (LPEN = 1, no planes) written before step 4. The page predicts w1 = w0 in every pair, so the main check goes red.
7. **Reference frame.** R5, R2 (line boundaries after t_V) and the vertical-position field of AA p.29.
8. **Measured (build), TG68K and fx68k.** P10-FIXTURE PASS, P10-R2 PASS, P10-FIRE PASS (CIAA PRA bits 7-6 read $C0 in both runs), P10-WIN PASS (0 line boundaries between the marker and the store of w1 in every pair). P10-LATCH is FAIL: with BPLCON0 = $0008 written before the reads, w1 differs from w0 in every pair on both cores (TG68K: w0 = $A3000018, w1 = $A300006E; fx68k: w0 = $A300001B, w1 = $A3000068), so precondition (ii) does not hold and P10-MAIN prints NOT GRADED: the light-pen latch does not freeze the counters in this system. The main run's reads (TG68K: w0 = $A300001E, w1 = $A3000074; fx68k: w0 = $A3000023, w1 = $A3000070) show a vertical position of 0 in w0. Contrasts: `P10-MAIN` (both runs load `prog_lpen_same.bin`, which reads once and stores the value as w0 and w1) prints FAIL on P10-MAIN and PASS on P10-LATCH; `P10-FIXTURE`, `P10-R2`, `P10-FIRE` (both fire inputs driven to 1 in the main run, which also prints NOT GRADED on P10-MAIN) and `P10-WIN` print FAIL on their own lines. The red precondition has a live input through its comparator (`P10-LATCH`: the contrast run loads `prog_lpen_same.bin`): P10-LATCH prints PASS on both cores and P10-MAIN, now graded, PASS.

### P11 - the joystick and mouse registers read as before

1. **Proposition.** JOY0DAT and JOY1DAT read as they did before the serial interface. Joystick switch states decode from bits 9, 8, 1 and 0 as AA p.25-26 tabulates. Mouse movement changes the 8-bit horizontal count (bits 7-0) for right and left and the 8-bit vertical count (bits 15-8) for down and up: up for right and down, down for left and up. An equal movement back returns the count to its start.
   - Claim: `sol:AA-mouse.c2`.
   - Pages: AA p.5, p.25, p.26.
2. **Stimulus.** The program reads JOY0DAT and JOY1DAT in a loop and stores each value. Each store's port time gives the read's time.
   - **(a) Joystick.** For each port in turn, the test holds each of nine states for 2 ms on the port list's `joystick0`/`joystick1` inputs: none, up, down, left, right, up+left, up+right, down+left, down+right.
     - A closed switch is the input bit at 1: the port list gives one bit per switch (bit 3 up, bit 2 down, bit 1 left, bit 0 right), and no page gives the level or the bit order of that input. HRM p.232 ("When the switches are pressed, the input line is shorted to ground") gives the connector pin's level, which is not a port of this system. The convention is a boundary precondition (P11-POL): with every input bit at 0, the first hold of each connector, F, L, B and R of both registers read 0; when they do not, the nine-state decode is not graded.
     - All other switches are open, and there is no mouse movement.
   - **(b) Mouse.** Through the mouse inputs of the port list (`kbd_mouse_type`, `kbd_mouse_data`, `kbd_mouse_level`), with the direction switches open, the test makes four movements in turn, 2 ms apart: 40 counts right, then 40 left, then 40 down, then 40 up. Within a movement the counts go in one at a time, 50 µs apart, so that consecutive reads see the change of a single count. The register graded is the one those inputs feed.
3. **Observation.**
   - (a) For each held state, the reads made at least 0.2 ms after the change and before the next change.
   - (b) Every read of the mouse register from before the first movement to 0.2 ms after the last.
4. **Expected value.**
   - **(a)** Define F = bit 9 xor bit 8, L = bit 9, B = bit 1 xor bit 0 and R = bit 1.
     - Each equals 1 exactly when its switch is closed. For example, up+left gives F = 1, L = 1, B = 0, R = 0.
     - Port 0 appears in JOY0DAT and port 1 in JOY1DAT (AA p.25: "0=left controller pair, 1=right controller pair").
     - In the other register all four values are 0.
     - **Sources:** AA p.25-26: "Forward 1 Y1 xor Y0 (BIT#09 xor BIT#08)", "Left 3 Y1", "Back 2 X1 xor X0 (BIT#01 xor BIT#00)", "Right 4 X1". The polarity is HRM p.232: "The true logic state of the switch data in these registers is '1 = switch closed.'"
   - **(b)** Direction, axis and return to the start are graded. No size of change per port count is assumed. The pages fix one count per quadrature step at the connector (HRM p.229, Figure 8-2), and no page gives the unit of a count on the port's mouse inputs. Each change is taken as an 8-bit signed difference between consecutive reads, which gives the sign of a single count's change whenever that change is under 128 counter steps.

| Movement | Moving axis | Sign of every change of the moving axis | Other axis |
|---|---|---|---|
| 40 right | bits 7-0 | positive | bits 15-8 unchanged |
| 40 left | bits 7-0 | negative | bits 15-8 unchanged |
| 40 down | bits 15-8 | positive | bits 7-0 unchanged |
| 40 up | bits 15-8 | negative | bits 7-0 unchanged |

     - The moving axis changes at least once during each movement.
     - After 40 right then 40 left, bits 7-0 read the value they had before the 40 right. After 40 down then 40 up, bits 15-8 read the value they had before the 40 down.
     - **Sources:** AA p.25, "JOY0DAT Y7 … Y0 X7 … X0". HRM p.229: "The counters increment when the mouse is moved to the right or 'down' (toward you). The counters decrement when the mouse is moved to the left or 'up'".
   - **Quantifiers:** all nine states on both ports, and both axes in both directions.
5. **Preconditions.**
   - While nothing changes, consecutive reads are equal.
   - *Contrast:* injecting movement during a hold breaks it.
   - The "none" state is graded as one of the nine states in (a). P11-POL reads the same two holds as the boundary convention: a decode at rest that differs from all-open prints FAIL on P11-POL, and the nine-state decode and the aggregate then print NOT GRADED.
   - *Contrast for P11-POL:* the first hold of each connector ("none") drives the up switch (bit 3).
6. **Contrasting case.** Each state is the others' contrast. For example, the "right" data evaluated against the "left" expectation goes red. For the mouse, the "left" data evaluated against the "right" expectation has the opposite sign. With 39 counts left after 40 right, bits 7-0 end one count away from the start, and the return check goes red.
7. **Reference frame.** R2, for the read times.
8. **Measured (build), TG68K and fx68k.** Preconditions: P11-UNIT PASS, P11-FIXTURE PASS, P11-POP PASS, P11-STILL PASS, P11-POL PASS (every switch of both registers reads open with every input bit at 0). The register values in each of the nine states on connector 0 (JOY0DAT) and on connector 1 (JOY1DAT) are the same on both cores: none $0000, up $0100, down $0001, left $0300, right $04FF, up+left $06FC, up+right $05FF, down+left $07FD, down+right $08FE on connector 0; none $0000, up $0100, down $0001, left $FF00, right $FC03, up+left $FE00, up+right $FD03, down+left $FF05, down+right $FC06 on connector 1. Each decodes to the expected F, L, B and R (PASS). Mouse: 40 counts right moved bits 7-0 from $00 to $28 and 40 left back to $00; 40 down moved bits 15-8 from $00 to $28 and 40 up back to $00 (PASS). Aggregate PASS. Contrasts: `P11-JOY` (the state "right" drives the left switch), `P11-MOUSE` (the first movement is -1 counts), `P11-MOUSE-RET` (39 counts in the second movement), `P11-AGG` (the third movement is -1 counts), `P11-STILL` (one X count during a hold), `P11-POP` (holds of 0.15 ms), `P11-POL` (the first hold of each connector drives the up switch; P11-JOY and P11-AGG print NOT GRADED), `P11-FIXTURE` and `P11-UNIT` each print FAIL on their own line on both cores.

### P12 - UARTBRK holds TxD at 0

1. **Proposition.** While ADKCON bit 11 (UARTBRK) is set, TxD is 0, including while a word is being shifted out.
   - Claim: `opus:AA-adkcon.c11`.
   - Page: AA p.10.
2. **Stimulus.**
   1. SERPER = $001D, one bit every 30 CCK (AA p.27).
   2. ADKCON = $0800, which clears the bit.
   3. Wait 1 ms.
   4. Marker M1, a store to Chip memory.
   5. ADKCON = $8800, SET plus bit 11 (AA p.10: "15 SET/CLR Set/Clear control bit. Determines if bits written with a 1 get set or cleared").
   6. Marker M2.
   7. Wait 0.2 ms.
   8. SERDAT = $0155: stop bit at bit 8 and data $55 (HRM p.258, Figure 8-13).
   9. Poll SERDATR bit 12 (TSRE) until it reads 1.
   10. Wait 0.2 ms.
   11. Marker M3.
   12. ADKCON = $0800.
   13. Marker M4.
   14. Wait 1 ms.
3. **Observation.** The TxD level, the port list's `uart_tx` output, from t(M2) to t(M3), taken from the port times of the markers.
4. **Expected value.**
   - TxD = 0 at every sample in the interval, including while $0155, whose data and stop bits would otherwise be 1, is being transmitted.
   - **Sources:** AA p.10: "11 UARTBRK Forces a UART break (clears TXD) if true". HRM p.257: "UARTBRK Force the transmit pin to zero."
   - **Quantifier:** "forces" is graded as every sample.
5. **Preconditions.**
   - TxD = 1 before t(M1), and again from one bit time after t(M4). This shows the 0 in the interval is the bit's doing.
   - In contrast run K below, a word is transmitted inside the interval.
   - *Contrast:* writing ADKCON = $8800 at the start of the program makes TxD 0 before M1, and the first precondition fails.
6. **Contrasting case K.** Write ADKCON = $0800 instead of $8800 at step 5. TxD in (M2, M3) then shows, per HRM p.258: a start bit 0; the data bits LSB first, 1 0 1 0 1 0 1 0; and a stop bit 1, each 30 CCK long. The 1 bits make the main check go red.
7. **Reference frame.** R2.
8. **Measured (build), TG68K and fx68k.** Preconditions: P12-FIXTURE PASS, P12-UNIT PASS, P12-PRE1 PASS, P12-KPRE PASS. Contrast run K, bits at the bit centres: 0101010101 on both cores (start 0, data 1 0 1 0 1 0 1 0, stop 1). Main run: TxD is 0 at all of 16521 samples from M2 to M3 on TG68K (14329 on fx68k), 0 of them 1. P12-MAIN PASS. Contrasts: `P12-MAIN` (contrast run K as the main run), `P12-PRE1` (ADKCON = $8800 in place of $0800 at step 2), `P12-KPRE` (the contrast run keeps the break set), `P12-FIXTURE` and `P12-UNIT` each print FAIL on their own line on both cores.

### P13 - disk DMA takes its address from a 20-bit pointer

1. **Proposition.** Disk DMA transfers words at the address held in DSKPTH/DSKPTL. The pair is a 20-bit word address (A20-A1), covering all of 2 MB.
   - Claims: `opus:AA-dskpth.c9`, `sol:AA-disk-pointer.c1`. The stimulus also follows `opus:AA-dskpth.c10` and `sol:AA-disk-pointer.c2`.
   - Pages: AA p.23, p.6.
2. **Stimulus.** The write direction, RAM to disk, needs no disk data (AA p.23: "14 WRITE Disk Write(RAM to Disk ) if 1"). Four runs; X and Y give each of A20-A16 both values, and Z and Zc give each of A20-A1 both values:
   - Run X: A = $0A5432, so A20-A16 = 01010.
   - Run Y: A = $154320, so A20-A16 = 10101.
   - Run Z: A = $0B4B50, the word address $5A5A8.
   - Run Zc: A = $14B4AE, the word address $A5A57, the complement of run Z's in all twenty bits A20-A1. The eight reads of a run step A3-A1 through all their values as well.
   - Both addresses lie in $080000-$1FFFFF, where the program can write the data and the port's word address is the program's address (section 0, Addresses). In that range A20 and A19 are never both 0, so X takes 01 and Y takes 10 there.

   The eight words at A hold $D0D0 + i. The program then:
   1. Writes DSKPTH = A>>16 and DSKPTL = A & $FFFF (AA p.6: "+=Address register pair. Low word uses DB1-DB15, High word DB0-DB4"). This happens before disk DMA is enabled, as AA p.23 requires: "These address registers must be initalized by the processor or coprocessor before disk DMA is enabled".
   2. INTREQ = $0002, clearing DSKBLK.
   3. DMACON = $8210 (SET, DMAEN, DSKEN).
   4. DSKLEN = $4000.
   5. DSKLEN = $C008.
   6. DSKLEN = $C008 again. HRM p.247: "the DMAEN bit in the DSKLEN register must be turned on twice in order to actually enable the disk DMA hardware", steps 1-4.
   7. Poll INTREQR bit 1 (DSKBLK, AA p.25) for up to 50 ms.
   8. DSKLEN = $4000.
3. **Observation.** Disk DMA accesses at the port, in the disk slots (R2), from the second $C008 write until DSKBLK is set.
4. **Expected value.**
   - The first disk DMA access is a read at exactly A.
   - All disk DMA accesses are reads at A, A+2, …, A+14, in increasing order, in each of the four runs.
   - **Sources:** AA p.23: "This pair of registers contains the 20 bit address of Disk DMA data." AA p.6: "PTL,PTH=20 bit Pointer that addresses DMA data." HRM p.247: "After each transfer occurs, the value of the pointer is incremented."
5. **Preconditions.**
   - In all four runs, at least one disk DMA access occurs and DSKBLK is set.
   - The harness description leaves open whether disk write DMA runs without a disk image. If no disk DMA access occurs, this proposition cannot be checked in this harness.
   - *Contrast:* DMACON without DSKEN gives no access (AA p.23: "DMAON DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)").
6. **Contrasting case.** Run Y is run X's contrast: each of A20-A16 is 1 in one of the two runs, and run Zc is run Z's, each of A20-A1 being 1 in one of them. A pointer that holds or decodes only some of the bits A20-A1 (a stuck or dropped bit) puts one of the four first accesses at another address. The contrast `P13-MAIN-LOW` writes run Zc's DSKPTL with bit 5 flipped. If DSKPTH held only 3 bits (AA p.23: "(old-3 bits)"), run X's first access would appear at $025432 instead of $0A5432 and run Y's at $054320 instead of $154320. If A20 alone were dropped, run Y's would appear at $054320. Either way the check goes red.
7. **Reference frame.** R2.
8. **Measured (build), TG68K and fx68k.** Disk DMA runs without a disk: each run shows 8 disk DMA reads and DSKBLK set. The first reads are at $0A5432, $0A5434, $0A5436 (run X), $154320, $154322, $154324 (run Y), $0B4B50, $0B4B52, $0B4B54 (run Z) and $14B4AE, $14B4B0, $14B4B2 (run Zc), and every read is at the run's address plus 2 per access, so P13-RUN and P13-MAIN PASS on both cores. Contrasts: `P13-MAIN` (run Y writes DSKPTH = $0005, three of the five high bits), `P13-MAIN-LOW` (run Zc writes DSKPTL with bit 5 flipped), `P13-RUN` (run X without DSKEN) and `P13-FIXTURE` print FAIL on their own line on both cores.

### P14 - DUAL: a second horizontal counter that wraps at HTOTAL

1. **Proposition.** With DUAL set in BEAMCON0:
   - HHPOSR reads a second horizontal counter.
   - That counter counts colour clocks from 0 to HTOTAL and then restarts.
   - The main counter, read through VHPOSR, keeps the line length it has without DUAL.
   - Claims: `sol:AA-hhpos.c1`, `sol:AA-hhpos.c5`. (`opus:AA-hhposw.c6` bundles comparisons with no visible output and is declined in section 1.)
   - Pages: AA p.24, p.12.
2. **Stimulus.** No DMA is enabled apart from refresh. BEAMCON0 always has LOLDIS set, so there is no long/short line toggle under either reading of AA p.24's "If the PAL bit & LOLDIS are not high". Runs:

| Run | BEAMCON0 | Meaning | HTOTAL | Other |
|---|---|---|---|---|
| U1 | $0860 | LOLDIS, DUAL, PAL | $0040 | |
| U2 | $0860 | LOLDIS, DUAL, PAL | $0070 | |
| U0 | $0820 | LOLDIS, PAL | $0040 | |
| U3 (contrast) | $08A0 | LOLDIS, VARBEAMEN, PAL | $0040 | VTOTAL = $0138 |

   In each run the program takes 1024 samples, each "read HHPOSR ($DFF1DA), store; read VHPOSR, store". The 2048 stores go to a buffer of their own (section 0, Addresses). The loop body's length is chosen so that the samples cover the counter's phases (see the preconditions).
3. **Observation.**
   - h_i is HHPOSR bits 7-0 (AA p.24: "(See HTOTAL for bits)", which puts h8-h1 in bits 7-0).
   - m_i is VHPOSR bits 7-0 (H8-H1, AA p.29).
   - t_i is the port time of the store of h_i.
   - A pair of consecutive samples i, i+1 **qualifies** when no refresh access (R2) lies between the last store of sample i-1 and the store of h_{i+1}. Both loop bodies then run with no DMA cycle taking the bus, so each HHPOSR read lies the same time before its store, and the two stores are exactly as far apart as the two reads.
4. **Expected value.**
   - The pages do not fix the value the counter holds when HTOTAL is written, so the samples taken before the counter's first restart after that write are not graded. The first restart of a series is its first sample that is smaller than its predecessor. A series that never restarts shows no counter that restarts at HTOTAL, and its lines are red.
   - U1: the counter restarts and, from its first restart, every h_i ≤ $40 and max h = $40.
   - U2: the counter restarts and, from its first restart, every h_i ≤ $70, max h = $70, and values in ($40, $70] occur.
   - In U1 and U2, for every qualifying pair from the first restart on: (h_{i+1} - h_i) mod (HTOTAL+1) = ((t_{i+1} - t_i)/K) mod (HTOTAL+1), exactly. The page gives no tolerance, and none is applied. Pairs that do not qualify are not graded by this relation.
   - max m is equal in U0, U1 and U2: the main counter's line is unchanged.
   - **Sources:**
     - AA p.24: "HHPOSR H 1DA R A DUAL mode hires H beam counter read … This is the secondary beam counter for the faster mode, triggering the UHRES pointers & doing the comparisons for HBSTRT,STOP,HTOTAL,HSSTRT,HSSTOP (See HTOTAL for bits)".
     - AA p.24: "HTOTAL … Highest color clock count in horiz line … Horiz line has this many + 1 280ns increments. … Active if VARBEAMEN=1 or DUAL=1."
     - AA p.12: "DUAL= Run the horizontal comparators with the alternate horizontal beam counter"; "VARBEAMEN= Enables the variable beam counter comparators to operate (allowing different beam counter total values) on the main horiz counter."
   - **Not graded:** HHPOSR when DUAL = 0, and HHPOSR bits 15-8; no page states them.
5. **Preconditions.** All are properties of the sample times and the refresh accesses alone. None uses h_i or m_i, the values under test. Each graded line has its own preconditions and no others.
   - **PHASE** (U1, U2). The t_i in CCK (R2) taken modulo HTOTAL+1 take every value 0…HTOTAL. A counter with the page's period then shows every one of its values, $40 or $70 included.
   - **STEP** (U1, U2). Every distance between two consecutive samples is shorter than HTOTAL+1 CCK. A counter that restarts at HTOTAL then restarts between some two consecutive samples, and the first restart is observed.
   - **POS** (MAIN). U0, U1 and U2 sample the same set of R2-relative positions: the sample time modulo the R2 line, in CCK. The main counter's maximum is then sampled alike in the three runs. A 68000 reads only in the even-numbered memory access cycles (HRM p.196: "The 68000 uses only the even-numbered memory access cycles"), so no sampling loop of a CPU reaches every one of the 227 positions of the line; the number of positions each run reaches and the positions U1 does not reach are printed as measured.
   - **PAIRS** (REL). In U1 and U2 qualifying pairs exist, and for each of them t_{i+1} - t_i is a whole number of CCK (a multiple of K).
   - **R2** (U1, U2, REL, MAIN), **U3** (MAIN): see below and the contrasting cases.
   - Which lines rest on which: U1 and U2 on R2, PHASE and STEP; REL on R2, PAIRS and STEP; MAIN on R2, POS and U3; AGG on those of the four lines before it. A line whose precondition does not hold prints NOT GRADED.
   - *Contrasts:* a loop body of 121 CCK in U1 (the loop body padded) fails STEP and PAIRS; a loop that samples once per line, at one position of the line, fails POS; VARBEAMEN added in U1 (BEAMCON0 = $08E0) shortens the lines to the secondary period and fails R2 and PHASE.
6. **Contrasting case.**
   - Evaluate U2 against U1's "max h = $40": it goes red.
   - U3, where HTOTAL acts on the main counter, gives max m = $40, which differs from max m in U0. This shows the main-counter comparison can go red. MAIN's contrast replaces U2's VHPOSR samples by U3's.
   - Live input for U1, U2, REL and AGG: the comparators run on U0's VHPOSR samples, a live counter that restarts at the line length R2 measures (the line spacing in CCK, less 1) in place of HTOTAL. The comparators, their quantifiers and their preconditions are those of the lines.
7. **Reference frame.** R2, for the CCK conversion.
8. **Measured (build), TG68K and fx68k.** Preconditions: P14-FIXTURE, P14-UNIT, P14-R2, P14-PHASE, P14-STEP, P14-POS, P14-PAIRS and P14-U3 PASS on both cores. The CPU loop stores one HHPOSR sample every 22 CCK (23 for 100 of 1023 intervals) on TG68K and every 21 CCK (22 and 23 for 21 intervals each) on fx68k. In U1, U2 and U0 it samples the same 113 (TG68K) and 109 (fx68k) of the 227 line positions, so P14-POS passes; the coverage stays printed as measured. HHPOSR bits 7-0 are $00 in every sample of U1 and U2 and the whole HHPOSR word is $0000 at its largest (both cores): the counter never restarts, so P14-U1, P14-U2, P14-REL and P14-AGG are FAIL on both cores. There are 831 (TG68K) and 844 (fx68k) qualifying pairs in each of U1 and U2, none of them after a restart. P14-MAIN is PASS: VHPOSR bits 7-0 reach $E2 in U0, U1 and U2 on both cores, and $FC (TG68K) or $2F (fx68k) in U3. Contrasts: `P14-PAIRS` and `P14-STEP` (U1 loads a loop body of 121 CCK), `P14-POS` (U1 loads a loop that samples once per line), `P14-PHASE` and `P14-R2` (U1 writes BEAMCON0 = $08E0), `P14-U3` (U3 writes BEAMCON0 = $0820 and no VTOTAL), `P14-MAIN` (U2's VHPOSR samples replaced by U3's), `P14-FIXTURE` and `P14-UNIT` print FAIL on their own line on both cores. The red lines have live inputs through their comparators: U0's VHPOSR samples, a counter that restarts every 227 CCK, make P14-U1, P14-U2, P14-REL and P14-AGG print PASS on both cores (`NMH_MUTATE=P14-U1`, `P14-U2`, `P14-REL`, `P14-AGG`).

### P15 - DUAL: the UHRES pointer chain restarts with the secondary counter

1. **Proposition.** With DUAL set, the UHRES bit-plane pointer comes out once per secondary-counter period of HTOTAL+1 CCK instead of once per line, and so more than once in a line.
   - Claim: `sol:AA-hhpos.c2`. (`opus:AA-hhposw.c6` is declined in section 1.)
   - Pages: AA p.12, p.15, p.24.
2. **Stimulus.**
   - BPLCON0 = $0080: UHRES on, BPU = 0. AA p.16: "UHRES= ultrahi res enables the UHRES pointers (for 1k*1k) (also needs bits in DMACON)".
   - BPLHMOD = $0006, BPLHSTRT = $0060, BPLHSTOP = $00A0.
   - SPRHPTH/SPRHPTL = $168000, and SPRHSTRT = SPRHSTOP = $0000.
   - The CPU rewrites BPLHPTH/BPLHPTL = U = $160000 after every VERTB (AA p.6: pointers "Must be reloaded by a processor before use"). Both addresses follow section 0, Addresses.
   - DMACON = $8380 (SET, DMAEN, BPLEN, COPEN), with a Copper list holding only R3 markers at $68 and $98.
     - DMAEN is the DMACON bit the pages tie to UHRES: "09 DMAEN Enable all DMA below (also UHRES dma)" (AA p.22).
     - BPLEN is also set, because p.16 says "bits" and BPLHPT is a bit-plane pointer. With BPU = 0, BPLEN fetches nothing else.
   - Runs: V0 with BEAMCON0 = $0820; V1 with BEAMCON0 = $0860 (DUAL) and HTOTAL = $0038, a 57-CCK period.
3. **Observation.** Accesses at the port in [U, U+$8000) on lines $68-$98 of F3 (R2, R3).
4. **Expected value.**
   - **V0:** exactly one access per line, and consecutive accesses 8 bytes apart. AA p.15: BPLHMOD "is added to the UHRES bit plane pointer (BPLHPTL,H) every line, and then another 2 is added, just like the other modulos".
   - **V1:**
     - Consecutive accesses are 57 ± 2 CCK apart. The bound follows from the pages:
       - Disk, audio and sprite DMA are off and BPU = 0, so their slots are open ("If a device does not request one of its allocated time slots, the slot is open for other uses", HRM p.194).
       - The Copper and the CPU rank below the pointer: "'Free' means priority above the copper and below the fixed stuff" (AA p.15). The UHRES sprite pointer takes "the first and third free cycles" (AA p.15), which leaves the 2nd free cycle where it is.
       - So the only cycles that are not free are the four refresh cycles, and Figure 6-9 (HRM p.195) draws them on alternate 280 ns cycles. At most two of them fall before the 2nd free cycle after a counter reset, so each access comes 0, 1 or 2 CCK later than it would without refresh.
       - Two consecutive accesses therefore lie 57 CCK apart, give or take at most 2 CCK.
     - Each line has at least 3 accesses.
     - Consecutive addresses are 8 bytes apart.
   - **Sources:**
     - AA p.12: "DUAL= … starts the UHRES pointer chain with the reset of this counter rather than the normal one. This allows the UHRES pointers to come out more than once in a horizontal line, assuming there is some memory bandwidth left".
     - AA p.15: "When UHRES is enabled, this pointer comes out on the 2nd 'free' cycle after the start of each horiz. line. Its modulo is added every time it comes out."
5. **Preconditions.**
   - V0 shows the one-per-line pattern. V0 is printed as a measured line: it is the condition under which P15 can be checked, not a graded line.
   - If V0 shows no access in [U, U+$8000), the proposition cannot be checked, and V1 is NOT GRADED. The access may be missing because the UHRES cycle does not reach the Chip-memory port (AA p.15 describes it as an address for "external detectors ... to do the special cycle for the VRAMs"), or because it needs DMACON bits the pages do not name. Whether UHRES works is graded in other packages.
   - *Contrast:* BPLCON0 = $0000 gives no accesses.
   - P15-DONE: both runs reach DONE (the pointer program after its fifth VERTB, the live-input programs after a fixed number of reads); P15-V1 rests on it.
6. **Contrasting case.** Evaluate V0 against V1's "at least 3 per line": it goes red.
7. **Reference frame.** R2 and R3.
8. **Measured (build), TG68K and fx68k.** P15-FIXTURE, P15-DONE, P15-R2, P15-F3 and P15-R3 PASS. P15-V0 is a measured line: run V0 (DUAL clear) shows 0 accesses in [U, U+$8000) on 0 lines between the markers, so the one-per-line pattern is not shown on either core, and P15-V1 prints NOT GRADED. Contrasts: `P15-FIXTURE`, `P15-R2` and `P15-R3` print FAIL on their own line, and `P15-F3` (run V0 ends 3,300,000 cycles after boot, before the fifth VERTB) prints FAIL together with `P15-DONE`, on both cores. Live inputs through the comparators (`P15-V1`: run V0 loads `prog_cpuuh_line.bin`, one CPU read of the region per line in place of the UHRES pointer, and run V1 loads `prog_cpuuh_57.bin`, one per 57 CCK): P15-V0 shows the pattern and P15-V1 prints PASS on both cores.

---

## 6. Disagreements and open points on the pages

- **HSSTRT description (AA p.24).** The header reads "HSSTRT H 1DE W A Horiz line position for HSYNC stop", as printed. Its next line reads "Sets # of color clocks for sync start", and AA p.10 lists it as "Horizontal Sync start (VARHSY)". Nothing here depends on it.
- **Long/short line toggle (AA p.24).** "If the PAL bit & LOLDIS are not high, long line/short line toggle will occur" can mean "unless both are high" or "unless either is high". P14 and P15 set both PAL and LOLDIS, which satisfies either reading.
- **Scan-doubling wording (AA p.8 against AA p.16 and p.3).** AA p.8 describes BPL1MOD as "odd planes, or active-fetch lines if bitplane scan-doubling enabled" and BPL2MOD as "even planes or inactive-fetch lines". AA p.16 and p.3 speak of "primary" and "alternate" lines. The assignment agrees (BPL1MOD with primary/active), but the words differ. P9 uses p.16 and p.3.
- **RST_ scope (AA p.2 and p.4 against AA p.16).** AA p.2 lists the registers RST_ resets as "BPLCON3,BPLCON4,CLXCON2,DIWHIGH,FMODE", and AA p.4 says "RST_ pin resets all bits in all registers new to AA". AA p.16 also marks bits of BPLCON0, BPLCON1 and BPLCON2 as initialised by RST_. P4 rests on p.16 alone.
- **LPEN reset (AA p.17 against AA p.16).** AA p.17 says LPEN is "reset on power up", but the AA p.16 table carries no "=0" mark on LPEN. P10 therefore uses the harness's power-on reset and not a later reset.
- **HIRES scroll range (HRM p.82 against AA p.31).** HRM p.82 allows "up to 15 pixels of delay", in 2-pixel steps in hires. AA p.31 gives a HIRES 1X range of "0-7" LORES pixels. Nothing here scrolls in HIRES.
- **ECS sprite positions (HRM p.303 against AA p.2).** For ECS, HRM p.303 calls SH0 "unimplemented" and restricts 70 ns sprite positions to SuperHires. AA p.2 gives 35 ns sprite positions in all resolutions. Nothing here depends on it: the one sprite used (P6) has SH2-SH0 = 0.

---

## 7. Claim index

| Claim | Proposition |
|---|---|
| `opus:AA-bplcon1.h1`, `sol:AA-bplcon1.c6`, `sol:AA-bplcon1.c7` | P1 |
| `opus:AA-bplcon1.h2` | none (section 1) |
| `opus:AA-bplcon1.b00`, `opus:AA-bplcon1.b01`, `opus:AA-bplcon1.b02`, `opus:AA-bplcon1.b03`, `opus:AA-bplcon1.b04`, `opus:AA-bplcon1.b05`, `opus:AA-bplcon1.b06`, `opus:AA-bplcon1.b07`, `opus:AA-bplcon1.b08`, `opus:AA-bplcon1.b09`, `opus:AA-bplcon1.b10`, `opus:AA-bplcon1.b11`, `opus:AA-bplcon1.b12`, `opus:AA-bplcon1.b13`, `opus:AA-bplcon1.b14`, `opus:AA-bplcon1.b15` | P2 |
| `sol:AA-bplcon1.c23`, `sol:AA-bplcon1.c24`, `sol:AA-bplcon1.c25`, `sol:AA-bplcon1.c26`, `sol:AA-bplcon1.c27`, `sol:AA-bplcon1.c28`, `sol:AA-bplcon1.c29`, `sol:AA-bplcon1.c30` | P2 |
| `sol:AA-bplcon1.c18`, `sol:AA-bplcon1.c22` | P5 (line P5) |
| `sol:AA-bplcon1.c15`, `sol:AA-bplcon1.c16`, `sol:AA-bplcon1.c17`, `sol:AA-bplcon1.c19`, `sol:AA-bplcon1.c20`, `sol:AA-bplcon1.c21` | P5 (lines P5-PF2H7, P5-PF2H6, P5-PF2H1, P5-PF1H7, P5-PF1H6, P5-PF1H1) |
| `opus:AA-bplcon1.b08.rst`, `opus:AA-bplcon1.b09.rst`, `opus:AA-bplcon1.b10.rst`, `opus:AA-bplcon1.b11.rst`, `opus:AA-bplcon1.b12.rst`, `opus:AA-bplcon1.b13.rst`, `opus:AA-bplcon1.b14.rst`, `opus:AA-bplcon1.b15.rst` | P4 |
| `opus:AA-pf1h.c1`, `opus:AA-pf2h.c1`, `sol:AA-bplcon1.c12`, `opus:AA-bplcon1.c6`, `sol:AA-bplcon1.c5` | P2 |
| `sol:AA-bplcon1.c9`, `sol:AA-bplcon1.c10` | recorded in P2, not graded (section 2) |
| `opus:AA-bplcon1.c5`, `sol:AA-horizontal-comparators.c1` | none (section 1) |
| `opus:AA-pf1h.c3`, `sol:AA-bplcon1.c13` | P3 |
| `sol:AA-position-resolution.c1` | none (section 1) |
| `sol:AA-bplcon0.c19` | P6 (line P6-I) |
| `opus:AA-bypass.c1`, `opus:AA-bypass.c2` | none (section 1) |
| `sol:AA-fmode-bitplane-fetch.c1`, `sol:AA-fmode-bitplane-fetch.c4`, `sol:AA-fmode-bitplane-fetch.c7`, `sol:AA-fmode-bitplane-fetch.c10` | P7 |
| `sol:AA-fmode-bitplane-fetch.c2`, `sol:AA-fmode-bitplane-fetch.c3`, `sol:AA-fmode-bitplane-fetch.c5`, `sol:AA-fmode-bitplane-fetch.c6`, `sol:AA-fmode-bitplane-fetch.c8`, `sol:AA-fmode-bitplane-fetch.c9`, `sol:AA-fmode-bitplane-fetch.c11`, `sol:AA-fmode-bitplane-fetch.c12` | none (section 1) |
| `opus:AA-fmode.c12`, `opus:AA-fmode.c19`, `opus:AA-fmode.c20`, `opus:AA-fmode.c21`, `opus:AA-fmode.c22`, `opus:AA-fmode.c23`, `opus:AA-fmode.c24`, `opus:AA-fmode.c25`, `opus:AA-fmode.c26`, `sol:AA-fmode.c14` | none (section 1) |
| `sol:AA-fmode.c5` | P7 |
| `opus:AA-fmode.c10`, `opus:AA-fmode.c35`, `opus:AA-fmode.c36`, `opus:AA-fmode.c37`, `opus:AA-fmode.c38`, `opus:AA-fmode.c39`, `opus:AA-fmode.c40`, `opus:AA-fmode.c41`, `opus:AA-fmode.c42`, `sol:AA-fmode.c12` | none (section 1) |
| `sol:AA-fmode-sprite-fetch.c2`, `sol:AA-fmode-sprite-fetch.c3`, `sol:AA-fmode-sprite-fetch.c5`, `sol:AA-fmode-sprite-fetch.c6`, `sol:AA-fmode-sprite-fetch.c8`, `sol:AA-fmode-sprite-fetch.c9`, `sol:AA-fmode-sprite-fetch.c11`, `sol:AA-fmode-sprite-fetch.c12` | none (section 1) |
| `opus:AA-bpl1mod.c5`, `sol:AA-bplmod.c1`, `sol:AA-bplmod.c2` | P8 |
| `opus:AA-bpl1mod.c6`, `sol:AA-bplmod.c3`, `sol:AA-bplmod.c4` | none (section 1) |
| `opus:AA-bpl1mod.c7`, `opus:AA-bpl1mod.c8`, `sol:AA-bplmod.c5`, `sol:AA-bplmod.c6`, `sol:AA-bplmod.c7`, `sol:AA-bplmod.c8` | P9 (line P9-AGG) |
| `opus:AA-bitplanes.c6`, `sol:AA-dual-playfield.c3` | P9 (line P9-DP) |
| `opus:AA-lpen.c1` | P10 |
| `sol:AA-mouse.c2` | P11 |
| `opus:AA-adkcon.c11` | P12 |
| `opus:AA-dskpth.c9`, `sol:AA-disk-pointer.c1` | P13 |
| `opus:AA-dskpth.c10`, `sol:AA-disk-pointer.c2` | none (rule followed in P13's stimulus) |
| `sol:AA-hhpos.c1`, `sol:AA-hhpos.c5` | P14 |
| `opus:AA-hhposw.c6` | none (section 1) |
| `sol:AA-hhpos.c2` | P15 |
| `sol:AA-hhpos.c3`, `sol:AA-hhpos.c4`, `sol:AA-hhpos.c6`, `sol:AA-hhpos.c7` | none |
| `sol:AA-mouse.c1`, `sol:AA-mouse.c3`, `sol:AA-mouse.c4`, `sol:AA-mouse.c5`, `sol:AA-mouse.c6`, `sol:AA-mouse.c7`, `sol:AA-mouse.c8`, `opus:AA-mouse.c3` | none |
| `opus:AA-adkcon.c12`, `opus:AA-adkcon.c13` | none |
