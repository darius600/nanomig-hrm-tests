# Specification: integration package `aa-s7-rest` (UHRES registers, DUAL-mode beam counter, REFPTR, horizontal-sync strobes)

This specification was written from the package's claims (`claims-spec-stage.jsonl`) and the pages of Commodore's *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, by an author who never saw the implementation or any test. Page numbers "p.N" are the corpus pages of that specification (`corpus\pNN.txt`), and quotations are given as printed. Pages of the *Amiga Hardware Reference Manual*, 3rd edition, are cited as "HRM p.N" only where the AA specification builds on the original chip set; no expected value comes from them.

The package holds 107 claims about 15 registers. Nine propositions (P1-P9) and one joint observation over P1 and P2 are specified; together they grade 73 claims. Thirty-four claims cannot be turned into a proposition: the thirty of the three horizontal-sync strobes, and the `&` claims and the `~` claims of REFPTR. They are listed first.

---

## 1. Claims not turned into a proposition

### 1.1 STREQU (038), STRVBL (03A), STRHOR (03C): all claims, 30 in total

| register | claim ids |
|---|---|
| STREQU | opus:AA-strequ.c1, opus:AA-strequ.c2, opus:AA-strequ.c4, opus:AA-strequ.c5, opus:AA-strequ.c6, sol:AA-strequ.c1, sol:AA-strequ.c2, sol:AA-strequ.c4, sol:AA-strequ.c5, sol:AA-strequ.c6 |
| STRVBL | opus:AA-strvbl.c1, opus:AA-strvbl.c2, opus:AA-strvbl.c4, opus:AA-strvbl.c5, opus:AA-strvbl.c6, sol:AA-strvbl.c1, sol:AA-strvbl.c2, sol:AA-strvbl.c4, sol:AA-strvbl.c5, sol:AA-strvbl.c6 |
| STRHOR | opus:AA-strhor.c1, opus:AA-strhor.c2, opus:AA-strhor.c4, opus:AA-strhor.c5, opus:AA-strhor.c6, sol:AA-strhor.c1, sol:AA-strhor.c2, sol:AA-strhor.c4, sol:AA-strhor.c5, sol:AA-strhor.c6 |

Reason, common to all 30. A strobe is "write address with no register bits" (p.6, legend `S`). The only page that says how these three addresses are used is p.29: "One of the first 3 strobe addresses above, it is placed on the RGA bus during the first refresh time slot." The register address bus (RGA) runs between the chips; a test sees the Chip-memory port, the video and audio outputs and what a program reads back, not the RGA bus. The refresh slot itself shows at the Chip-memory port, but not which strobe address went with it. No page says what output shows that a line carried STREQU, STRVBL or STRHOR: the function column names the purpose ("Strobe for horiz sync with VB and EQU", "... with VB (Vert. Blank)", "Strobe for horiz sync"), but no sentence ties it to a pixel, a sync level, an audio sample or a readable register value. Per claim kind:

- c1 (address) and c6 (function): the address and what it signals exist only on the RGA bus.
- c2 (`S`, strobe): the fact that a write has no data bits could only be seen through an effect of the write. No page gives one.
- c4 (`&`, "Register used by DMA channel only", p.6): this says who uses the address. It predicts nothing a test can observe, and no page says what a processor write to it does.
- c5 (`~`, "Address not writable by the Coprocessor unless COPCON bit 1 is set true", p.6): an effective write can be told from an ineffective one only by its effect, and no page gives an effect for these three addresses. For REFPTR the write has an observable effect; the ids of REFPTR are treated in 1.2.

No claim in this package needs a disk image.

### 1.2 REFPTR (028), the `&` mark and the `~` mark: opus:AA-refptr.c4, sol:AA-refptr.c4, opus:AA-refptr.c5, sol:AA-refptr.c5

Reason for c4. Both claims are the legend entry "&=Register used by DMA channel only." (p.6) applied to REFPTR. Their content is the quantifier: the refresh DMA channel uses the register, and nothing else does. P1 shows the refresh accesses taking their addresses from REFPTR, but no stimulus makes "only", that no processor uses the register, observable, and a claim is graded whole or not at all. As for the three strobes (c4 in 1.1), the entry says who uses the address and predicts nothing further a test can observe; p.27 adds that the register "should never be written by the microprocesor", which states no effect either.

Reason for c5. Both claims are the legend entry "~=Address not writable by the Coprocessor unless COPCON bit 1 is set true." (p.6) applied to REFPTR. The entry is universal: whenever COPCON bit 1 is clear, no Copper write reaches the register. The package makes one Copper MOVE to 028, at one beam position, with two data words, and compares the refresh addresses that follow. Two complementary data words write each data bit both as 0 and as 1, but they do not establish that every otherwise legal Copper write is blocked while COPCON bit 1 is clear, and the package does not vary the beam position or the Copper list. The ids are therefore withdrawn whole. The observation itself stays in section 5 as the joint observation P1+P2, a graded line that carries no claim id.

### 1.3 Parts of used claims that are not graded

- The chip-set sentence in every page-10 claim (the `h` mark, which the p.6 and p.10 legends define differently). The claims themselves decline to name a chip set, and no proposition depends on it: the tests run with AA selected, and both legends place these registers in the "hires" (ECS) chips that AA extends (section 6, item 1).
- The access letters `W` (c2 of every write register) are exercised by the writes each proposition makes. What a read of a write-only address returns is not stated on any page, so no check reads one.

---

## 2. System, units and conventions

- **Configuration.** AA chip set selected (`Config.chipset_config = 0x10`), 2 MB Chip memory (the harness default `memory_config = 0x03`). The harness has no PAL/NTSC setting; every program selects PAL itself in prologue step 1, through BEAMCON0 bit 5: "PAL= Set appropriate decodes (in normal mode) for PAL" (p.12). Every run is made once on each CPU core in its default 68000 mode (`cpu_config = 0x00`). Any comparison between two runs uses the same core and the same configuration.
- **Graded cores.** The AA chip set is selected by `Config.chipset_config` = $10, an input of nanomig.v that does not depend on the CPU core or on a build define. Every proposition, precondition and anchor below is graded on both builds, fx68k and TG68K, and every verdict is reported per core; where the cores differ, that is a result and is not reconciled.
- **Register names** are the offsets from 0xDFF000 given in the address list (pp.6-10). The program writes them as 16-bit words.
- **Addresses** below are byte addresses. The Chip-memory port reports word addresses (byte address / 2).
- **Time unit.** One memory cycle (mc) is 280 ns: "memory cycles are 280 ns" (p.21). The same unit counts HTOTAL and the DUAL-mode counter: "Horiz line has this many + 1 280ns increments" (p.24), "Highest color clock count in horiz line" (p.24). The harness states no ratio between `clk_sys` and mc, so the conversion is measured on the one 280 ns count a program can read that no proposition here grades: the H field of VHPOSR, "H8 H7 H6 H5 H4 H3 H2 H1 ... RESOLUTION=1/160 OF SCREEN WIDTH (280 NS)" (p.29). **Run HV:** the prologue (BEAMCON0 = 0x0020, so DUAL = 0), then 64 reads of VHPOSR in the loop of P8 step 5, each word stored to BUF + 2i, then the epilogue. For every pair of consecutive samples with the same V field (bits 15-8), the H field (bits 7-0) advances by ΔH mc while the stores advance by ΔT `clk_sys`. The conversion factor is ΣΔT / ΣΔH over these pairs. It is a measurement, recorded per core, and every conversion to mc below uses it. The pairs form **S_HV segments**, the maximal runs of consecutive pairs; **J_HV** is the largest minus the smallest interval between two sample stores of run HV, in mc. S_HV and J_HV are printed. **Anchor check:** there are at least 16 such pairs, each has ΔH ≥ 1, and no V value has more than one same-V pair whose H field decreased (that pair straddles a line start; it is left out of the conversion, counted and printed, 7.2 item 4). *Contrast:* sample VPOSR instead of VHPOSR; its bits 7-0 do not count, and the check fails.
- **Unused bits** are written as 0: "x= don't care; but drive to 0 for upward compatibility !" (p.16); "(x's should be driven to 0 for upward compatibility)" (p.24); the x bits of SPRHSTRT and SPRHSTOP (p.28); bits 14-11 "unused" of BPLHSTOP (p.15). Bit 15 of BPLHSTOP (BPLHWRM) and of SPRHSTOP (SPRHWRM) is always written 0. Each of these bits "Swaps the polarity of ARW*" (pp.15, 28); no claim in this package covers them, and no check grades whether a UHRES access is a read or a write.
- **Memory layout.** Every address the programs use lies in the Chip memory the harness maps one to one, 0x080000-0x1FFFFF (`kChipIdentityLo`, `kChipIdentityHi` in `nanomig_harness.h`), where the harness loads the program at `PROGRAM_ORG` = 0x080400 (`nanomig.i`). Below 0x080000 the ROM overlay covers Chip memory after reset: "An additional copy of the system ROM responds starting at memory location $00000000. ... Software must assume that memory is not available." (HRM p.223). No program uses memory there. The fixed addresses:

  | use | byte addresses | from |
  |---|---|---|
  | program: code and its immediate data | from 0x080400, inside 0x080000-0x08FFFF | `PROGRAM_ORG` (`nanomig.i`) |
  | zero block Z, 4096 bytes of 0x0000 | 0x090000-0x090FFF | |
  | Copper list L, 12 bytes | 0x091000-0x09100B | |
  | setup-done record (MK - 4), then the markers from MK = 0x0C0004 | 0x0C0000-0x0C03FF | `RESULTS` (`nanomig.i`) |
  | sample buffer BUF (P8 and run HV), 64 words | 0x0C0400-0x0C047F | `RESULTS` + 0x400 |
  | end marker END | 0x0C0FFE | `DONE_ADDR` (`nanomig.i`) |
  | stack | 0x0FF000-0x0FFEFF | below `PROGRAM_STACK` = 0x0FFF00 (`nanomig.i`) |
  | UHRES pointer targets (section 4), with the BPLH and SPRH windows of R2 | 0x0A5294-0x0A5654, 0x0CDB96-0x0CEB95, 0x132468-0x133467, 0x15A9AA-0x15ADEA | |
  | UHRES pointer targets of the walking cases (run UW, section 4): one bit set, or one bit clear among the 20 address bits A20-A1 | 0x000002-0x100000 (a single bit), 0x0FFFFE-0x1FFFFC (all bits but one) | |

  No two uses overlap, except that the walking cases of run UW read addresses anywhere in the 2 MB Chip memory; they are told from every other access by the exact address of the case (section 4), and none of them lies in the program image 0x080400-0x08FFFF, in Z, in L, in the results block or in the stack. Nothing but the UHRES pointers uses the UHRES targets. The longest marker window (run UR, 0x040..0x120, 225 markers) ends at 0x0C0387.
- **Common prologue**, the same in every run:
  1. DMACON ← 0x7FFF, INTENA ← 0x7FFF, INTREQ ← 0x7FFF (clear all enables and requests; bit 15 SET/CLR = 0, p.22, p.25). BPLCON0 ← 0x0000. COPCON ← 0x0000. BEAMCON0 ← 0x0020: PAL (bit 5) set, every other bit 0, DUAL and VARBEAMEN among them (p.12). No page gives BEAMCON0's value after reset, so the programs write it; every program starts with this same step, at the same time.
  2. Prepare the run's data (zero block, Copper list, buffers).
  3. Wait for the start of the vertical blank: INTREQ ← 0x0020, then poll INTREQR until bit 5 is 1 ("05 VERTB 3 Start of Vertical blank", p.25). The run's setup writes follow immediately. The frame that begins here is **the observed frame**. Pointer registers are therefore loaded in the vertical blank: "PTL,PTH=20 bit Pointer that addresses DMA data. Must be reloaded by a processor before use (Vertical blank for Bit Plane and Sprite pointers ...)" (p.6).
- **Common epilogue:** DMACON ← 0x7FFF, BPLCON0 ← 0x0000, then `DONE` (`nanomig.i`): the CPU writes `DONE_VALUE` (0x600D) to END = `DONE_ADDR` (the **end marker**) and stops.

---

## 3. Reference frames used by several propositions

### R1. Line markers: which line an access belongs to

A test sees port accesses by time, but the pages state the UHRES behaviour per beam line: "each horiz. line" (p.15), "the line when the data fetch starts" (p.15), "V10-V0" (pp.15, 28). The line number of the vertical beam counter is read from VPOSR (bits 2-0 = v10, v9, V8) and VHPOSR (bits 15-8 = V7-V0, bits 7-0 = H8-H1) (p.29). After its setup, the program runs this loop:

```
for V = Vlo .. Vhi:
    repeat:
        h <- VHPOSR
        if (h & 0x00FF) in [0x60, 0x90):
            v <- VPOSR
            if (((v & 7) << 8) | (h >> 8)) == V: store v, then h, at MK + 4*(V - Vlo); leave repeat
```

- t(V), the **marker time** of line V, is the port time of the store of h.
- An access at time t **belongs to line V** if t(V-1) < t < t(V).
- The **setup-done record** has the same two-word format. The program stores it after its last setup write and before the loop, at MK - 4.
- **Anchor checks.** A marker exists for every V in Vlo..Vhi. The line number stored in each marker equals its V. The H field stored in each marker lies in [0x60, 0x90). The times t(V) strictly increase.

Why the rule is sound: each marker is taken in the middle of its line, at H ≥ 0x60 of about 0xE3. The UHRES pointers come out "on the 2nd 'free' cycle after the start of each horiz. line" (p.15) and "in the 1st & 3rd 'free' cycles (see BPLHPTH,L) after horiz line start" (p.27). An access early in line V therefore lies between the markers of V-1 and V. Precondition PR-U5 (section 4) checks the line of every access against the H counter itself (the next paragraph).

**The H counter as the line-start reference.** Each marker carries the H field it read: h(V), the position of the beam in its line V in units of 280 ns, "RESOLUTION=1/160 OF SCREEN WIDTH (280 NS)" (p.29), at the instant of the read of VHPOSR. The read belongs to the loop's first instruction (`move.w (a2),d0`), whose fetch the port shows. Let t_s(V) be the port time of the last fetch of that instruction before the store of marker V, and K the conversion of run HV (section 2). For an instant t before t_s(V), in the same line as the sample or earlier, the position of the beam is p(t) = h(V) - (t_s(V) - t)/K memory cycles, and p = 0 is the start of line V, where the H field wraps. The read follows the fetch by a fixed number of clock cycles, fewer than one memory cycle, and p is not corrected for it; h is the integer part of the position. An access at t that R1 assigns to line V lies on line V of the beam, after its start and before the sample, exactly when t < t_s(V) and p(t) ≥ 0. PR-U5 grades this for every UHRES access. Applied to run UR, whose window is 0x040..0x121, the same reference gives the start T0(V) = t_s(V) - h(V) K of every line V in that window, which PR-1c uses.

**The line start is open.** The pages fix neither at which clock the V field steps after the H field wraps (run HV measures the lag), nor where in the line the free cycles fall after the reset of the counter ("starts the UHRES pointer chain with the reset of this counter", p.12), nor whether the vertical comparison of the chain, made at that reset, already sees the new line's V. The chain's first accesses of a line may therefore belong, by this rule, to the line the page names or to the next one. The propositions leave the page's first line open and grade the lines after it: for the BPLH accesses the first access of a run lies on line start or start+1 and exactly one access lies on each line from start+1 to stop-1 (P3, P4, P5); for the SPRH accesses P7 grades only the lines from start+1 to stop-1.

### R2. Access classification

The harness names no source for an access: `ram_log()` gives each RAM-port access its time, word address, data, direction and byte lanes, and nothing else (`RamAccess` in `nanomig_harness.h`). Refresh is therefore identified by a port, and every other access by the fixed layout of section 2.

- **Refresh access:** a logged access in whose `clk_sys` step the `refresh` output of `nanomig.v` is high (port list: `output refresh, // ram refresh cycle`), whatever its address. The harness does not log this port, so the test steps with `run_cycles(1)` and reads `ports()->refresh` in the step that logs each access, as the fast-RAM capture of `ch6-blitter-cpu-sharing` reads its ports. A mark on a port, not an address, keeps the refresh accesses of P1 recognisable after REFPTR is written.
- **Cross-check against the reference run UR (fixture).** UR is made on the same core and in the same configuration as the run it serves; it is U1 with BPLCON0 ← 0x0000, DMACON ← 0x8000 and the marker window 0x040..0x120 (section 4). With DMAEN clear, "Enable all DMA below (also UHRES dma)" (p.22) is off for every DMA channel and for UHRES, and refresh is not among DMACON's channels, so every access of UR outside the CPU regions of the next item (a "non-CPU access" of UR) is a refresh access. Refresh slots do not depend on the program ("There are 4 refresh time slots" in every line, p.29), and every program starts with the same step 1, so a refresh slot comes at the same port time in every run. In UR, and in every run that uses this classification, from the end of prologue step 1 to that run's end marker, the accesses the `refresh` port marks are exactly the accesses at the port times of UR's non-CPU accesses. A disagreement is a fixture failure, not a result about the chip set: the classification is then unusable for that run, and nothing that rests on R2 is graded on it. *Contrast:* UR with DMACON ← 0x8320 (DMAEN and SPREN set); its sprite-DMA reads inside Z are non-CPU accesses the `refresh` port does not mark, and the cross-check fails.
- **CPU access:** any other access inside the program image 0x080400-0x08FFFF (`PROGRAM_ORG` to the end of the program block; instruction and operand fetches), the results block 0x0C0000-0x0C0FFF (setup-done record, markers, samples, end marker) or the stack 0x0FF000-0x0FFEFF; and any other write inside Z or L (the data preparation of prologue step 2).
- **DMA accesses** are all remaining accesses, sorted by source:
  - Copper: a read inside the Copper list L.
  - Sprite or bitplane DMA: a read inside Z.
  - UHRES: every remaining access.
- **Anchor check (UR):** between every two consecutive markers of UR there is at least one refresh access, and UR has no read inside Z. *Contrast:* UR with DMACON ← 0x8320 (DMAEN and SPREN set) reads Z, and the check fails. The pages give no stimulus that removes refresh, so the first part is a presence check only.
- A UHRES access is a **BPLH access** if its address lies between the smallest and largest expected BPLH address of the run, widened by 0x100 bytes on each side. It is an **SPRH access** if its address lies in [S, S + 0x1000), where S is the run's expected UHRES sprite address. Any other UHRES access is a **stray**.

### R3. Run pairs

Some checks compare two runs that differ only in the named words. Both runs use the same core and configuration, and the same program layout: the differing words have the same instruction length and the same addresses. The simulation is deterministic, so any difference between the two port traces comes from the named words.

---

## 4. The UHRES runs (shared by P3-P7)

The UHRES pointers need the UHRES bit: "UHRES= ultrahi res enables the UHRES pointers (for 1k*1k) (also needs bits in DMACON) (hires chips only)" (p.16, BPLCON0 bit 7). DMACON bit 9 covers them: "09 DMAEN Enable all DMA below (also UHRES dma)" (p.22). The page does not say which other DMACON bits are needed (section 6, item 7). Every UHRES run therefore also sets BPLEN and SPREN, and keeps ordinary bitplane and sprite DMA harmless: BPU = 0, and every ordinary pointer points at the zero block Z.

**Setup, in this order, right after the vertical-blank wait:**
1. SPR0PTH/SPR0PTL ... SPR7PTH/SPR7PTL ← Z. BPL1PTH/BPL1PTL ... BPL8PTH/BPL8PTL ← Z.
2. SPRHPTH, SPRHPTL, SPRHSTRT, SPRHSTOP ← the run's values (table).
3. BPLHPTH, BPLHPTL, BPLHMOD, BPLHSTRT, BPLHSTOP ← the run's values.
4. BPLCON0 ← the run's value (0x0080 = UHRES only: HIRES = 0, BPU = 0, LPEN = 0).
5. DMACON ← the run's value (0x8320 = SET, DMAEN, BPLEN, SPREN; Copper, blitter, disk and audio DMA stay off).
6. Setup-done record, then the R1 marker loop for Vlo..Vhi, then the epilogue.

| run | BPLHPTH | BPLHPTL | BPLHMOD | BPLHSTRT | BPLHSTOP | SPRHPTH | SPRHPTL | SPRHSTRT | SPRHSTOP | BPLCON0 | DMACON | Vlo..Vhi |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| U1 | 0x0015 | 0xAAAB | 0x0010 | 0x0050 | 0x0070 | 0x0013 | 0x2469 | 0x0058 | 0x0068 | 0x0080 | 0x8320 | 0x040..0x080 |
| U2 | 0x000A | 0x5555 | 0xFFF0 | 0x0050 | 0x0070 | 0x000C | 0xDB97 | 0x0058 | 0x0068 | 0x0080 | 0x8320 | 0x040..0x080 |
| U3 | 0xFFF5 | 0xAAAB | 0x0010 | 0x0050 | 0x0070 | 0xFFF3 | 0x2469 | 0x0058 | 0x0068 | 0x0080 | 0x8320 | 0x040..0x080 |
| U4 | 0x0015 | 0xAAAB | 0x0010 | 0x00F8 | 0x0108 | 0x0013 | 0x2469 | 0x0100 | 0x0110 | 0x0080 | 0x8320 | 0x0E8..0x120 |
| U0 | as U1 | | | | | | | | | **0x0000** | 0x8320 | as U1 |
| U0d | as U1 | | | | | | | | | 0x0080 | **0x8120** (no DMAEN) | as U1 |
| UR | as U1 | | | | | | | | | **0x0000** | **0x8000** (no channel) | **0x040..0x120** |
| UW | 0x0015 | 0xAAAB | 0x0000 | 0x0050 | 0x007A | 0x0013 | 0x2469 | 0x0050 | 0x007A | 0x0080 | 0x8320 | 0x040..0x080 |

UR is the refresh cross-check of R2, not a UHRES run: it enables no DMA channel, and its marker window covers the latest line any run observes.

**Run UW (the walking cases of P3 and P6).** UW is U1 with BPLHMOD = 0, BPLHSTRT = SPRHSTRT = 0x0050 and BPLHSTOP = SPRHSTOP = 0x007A, whose marker loop writes the four UHRES pointer registers again after the marker of every line V with 0x050 ≤ V < 0x078, taking the values of the next case from a table in the program image. The pointer is "reloaded by a processor" (p.6), and the write falls in the middle of line V (the marker is taken at H ≥ 0x60), long before the chain of line V+1. The table holds 40 cases, case n = 0..39, which govern line 0x51 + n. The 20 pointer bits are numbered j = 0..19: for j = 0..14 bit j is DB(j+1) of the low word (address bit A(j+1)); for j = 15..19 bit j is DB(j-15) of the high word (address bit A(j+1)). The expected address of a case is (high word bits 4-0) << 16 | (low word & 0xFFFE), as in P3.
- **Bit-plane pointer of case n.** For n = 0..19 (walking one) the bit j = n is 1 and every other bit of both words, used or unused, is 0. For n = 20..39 (walking zero) the bit j = n - 20 is 0 and every other bit of both words, used or unused, is 1.
- **Sprite pointer of case n.** The bit-plane case of index (n + 20) mod 40, so that on one line one pointer walks a one and the other walks a zero and their addresses differ.
- Each of the 20 used bits is therefore a 1 among 0s and a 0 among 1s, once for each pointer, and the unused bits (DB0 of the low word, DB5-DB15 of the high word) are 0 in the walking-one cases and 1 in the walking-zero cases.
- No case address is in the program image, Z, L, the results block or the stack (section 2); the address of a case is the only thing that tells its access from the CPU's.

Expected first addresses: in U1, U3 and U4, P = 0x15AAAA and S = 0x132468; in U2, P = 0x0A5554 and S = 0x0CDB96. In U2 every one of the 20 pointer bits of both pointers is the complement of U1 (0x15AAAA XOR 0x0A5554 = 0x132468 XOR 0x0CDB96 = 0x1FFFFE). BPLHPTL and SPRHPTL are written with bit 0 set to exercise "Low word uses DB1-DB15" (p.6). The RAM port carries word addresses (`ram_address[23:1]`), so it shows that DB15-DB1 land on A15-A1; whether DB0 is dropped has no counterpart there. U4's ranges cross line 0x100, so they need V8 of the 11-bit comparison.

**Preconditions common to P3-P7** (each gets a contrasting case). PR-U1 to PR-U3 and the cross-check of R2 are made over U1, U2, U3, U4, U0, U0d, UW and UR; PR-U4 is run U0's.

- **PR-U1.** The end marker is present. *Contrast:* omit the epilogue's store; the check reports it missing.
- **PR-U2.** The R1 anchor checks pass. *Contrast:* make the loop skip every second V; the "marker for every V" check fails.
- **PR-U3.** The line in the setup-done record is below Vlo (setup finished before the window). *Contrast:* in a copy of U1, wait for line 0x060 before the setup; the check fails.
- **PR-U4.** Run U0, with the UHRES bit clear, has no UHRES and no stray access: from the setup-done record to the end marker, every access of U0 is a refresh, CPU or Z access (R2 accounts for every access when UHRES is off). Refresh is identified by the `refresh` port and cross-checked against UR, not inferred from U0, so the check can fail. *Contrast:* U1 itself has UHRES accesses; the same check, applied to U1, reports them.
- **PR-U5.** Every UHRES access of U1, U2, U3, U4 and UW lies on the line R1 assigns it to, by the H counter itself (R1, "The H counter as the line-start reference"): for the line V of the access, the marker loop's first instruction was fetched for the sample of marker V after the access (t < t_s(V)), and the position of the beam at the access, p = h(V) - (t_s(V) - t)/K, is not negative. No distance band is graded: the position is read from the beam counter, whose H field wraps at the start of a line. It is graded only where these runs have UHRES accesses; with none, a non-graded line prints that the population is empty, and the lines that depend on PR-U5 do not wait for it (they grade their own populations). An access outside every marker interval, or a run whose marker loop head is not found in its program, fails the line. *Contrast:* gate the marker loop at H in [0xB0, 0xE0) instead of [0x60, 0x90): a window as wide as the normal one, so that the loop still records a marker on every line and PR-U2 still holds. The markers are then sampled at the end of line V and stored next to the start of line V+1, where the page places the UHRES accesses; an access at the start of line V+1 that precedes the store lies after the sample of marker V, and the check must report it (7.2 item 11).
- **PR-U6.** U1, U2 and U3 each have at least one BPLH access and at least one SPRH access. The lines that read a first address, a pointer-to-pointer relation or an increment (P3-E2, P3-E4, P6-E2, P6-E3, P6-E5) are graded only when it holds; otherwise they print NOT GRADED. *Contrast:* U1 with the UHRES bit clear has none.
- **PR-U7.** On every line 0x059 to 0x067 of U1, U2 and U3 there are two SPRH accesses and one BPLH access, so that the order of the accesses of one line is observed. P6-E4 is graded only when it holds. *Contrast:* U1 with SPRHSTRT = 0x0060 has no SPRH access on lines 0x059 to 0x05F.

---

## 5. Propositions

### P1. REFPTR (028) is the refresh address generator: a Copper write to it, allowed by COPCON bit 1, changes the addresses of the refresh accesses that follow

- **Claims:** opus:AA-refptr.c1, opus:AA-refptr.c2, sol:AA-refptr.c1, sol:AA-refptr.c2. The `&` claims (c4) are declined (1.2). Page 6: `REFPTR    & ~028  W    A        Refresh pointer`.
- **Qualifying pages:**
  - p.27: "REFPTR 028 W A Refresh pointer / This register is used as a Dynamic RAM refresh address generator. It is writeable for test purposes only, and should never be written by the microprocesor."
  - p.19: "CDANG Coprocessor danger mode. Allows coprocessor access to all RGA registers if true."
  - p.20: "RD =RAM Data moved by MOVE instruction at IR2 time directly from RAM to the address given by the DA field."

**Stimulus.** p.27 says the microprocessor should never write REFPTR, so the Copper writes it. Copper list at L (instruction format p.20):

| offset | word | meaning |
|---|---|---|
| L+0 | 0x4001 | WAIT IR1: VP7-0 = 0x40, HP8-2 = 0, bit 0 = 1 |
| L+2 | 0xFFFE | WAIT IR2: BFD = 1, all VE and HE enable bits 1, bit 0 = 0 |
| L+4 | 0x0028 | MOVE IR1: DA8-DA1 → register 028 (REFPTR) |
| L+6 | value | MOVE IR2: data |
| L+8 | 0xFFFF | WAIT IR1 (end of list) |
| L+10 | 0xFFFE | WAIT IR2 |

Program: the prologue, then COP1LCH ← L >> 16, COP1LCL ← L & 0xFFFF, a write to COPJMP1 (strobe), COPCON ← 0x0002, and last DMACON ← 0x8280 (SET, DMAEN, COPEN). This follows p.21: "It is important that one of the jump registers be initalized and it's jump strobe address hit, after power up but before Coprocessor DMA is initalized." HRM p.30 prescribes the same. The CPU then polls VPOSR/VHPOSR until the line number is ≥ 0x060 and runs the epilogue.

Runs:

| run | COPCON | L+4 | L+6 (value) |
|---|---|---|---|
| RA | 0x0002 | 0x0028 | 0x5555 |
| RB | 0x0002 | 0x0028 | 0xAAAA |
| RE | 0x0002 | 0x0028 | 0x5555 (contrast: same value as RA) |
| RNA | 0x0002 | 0x01FE | 0x5555 (contrast: MOVE to NO-OP) |
| RNB | 0x0002 | 0x01FE | 0xAAAA (contrast: MOVE to NO-OP) |

**Reference frame.** The write takes place "at IR2 time" (p.20). The anchor is the port time of the Copper's fetch of L+6 in the observed frame.

**Observation.** The refresh accesses (R2) at the Chip-memory port, from the Copper's fetch of L+6 to the end marker (about lines 0x040-0x060).

**Expected relation.**
- In that interval, the sequence of refresh-access word addresses of RA differs from that of RB in at least one position.
- Only "at least one" is graded. The page gives no bit layout for REFPTR and does not say how the written value maps to refresh addresses (section 6, item 9). So it predicts that the generator now holds different contents in RA and RB, but not that every later refresh address differs.

**Preconditions.**
- PR-1a. In RA and RB, the Copper fetches L+0, L+2, L+4 and L+6 in this order, exactly once in the observed frame and before the end marker, with data 0x4001, 0xFFFE, 0x0028 and the run's value. *Contrast:* list at L+0 the end WAIT (0xFFFF, 0xFFFE); the MOVE is then never fetched and the check fails.
- PR-1b. Up to the fetch of L+6, the RA and RB traces agree in time, address and direction for every access. Their data agree except for the value word: the CPU's read of it from the program (the immediate operand, or the data word it is copied from) and its write into L+6. After that fetch, every non-refresh access still agrees in time and address. *Contrast:* in RB, add one CPU NOP right after the vertical-blank wait; the traces then differ in time and the check reports it.
- PR-1c. Every line of the interval has refresh accesses ("There are 4 refresh time slots", p.29): each actual line of the interval is delimited by the H counter (R1, "The H counter as the line-start reference"), and the refresh accesses (R2) are counted in each line separately. The lines are those of run UR, whose line starts T0(V) are read from its markers; R2's cross-check has shown that a refresh slot comes at the same port time in RA and RB as in UR, and every program starts from the same boot and the same step 1, so the beam of RA and RB is the beam of UR. The lines graded are the complete lines V with T0(V) at or after the Copper's fetch of L+6 and T0(V+1) at or before the end marker; each must hold at least one refresh access of the run. The pages give no contrast that removes refresh; this is a presence check, per line, and the numbers of accesses per line are printed.

**Contrasting cases.**
- RE against RA (same value): the refresh address sequences are identical in every position.
- RNA against RNB (MOVE to 1FE, "NO-OP(NULL)", p.10): identical in every position. Only a MOVE to 028 moves the refresh addresses, which is the address claim.
- The old chips' Copper reach, "(on old chips- access to only RGA>3E if CDANG=1)" (p.20), is not graded here: HRM p.21 ("Any register whose address is $20 or above") disagrees with it and with HRM p.26 and p.310, no claim of this package depends on it, and a list the old chips' 18-bit location registers can reach ("(Old chips- 18 bits)", p.6) would have to lie below 0x080000, under the ROM overlay.

### P2. With COPCON bit 1 clear, the Copper cannot write REFPTR: its MOVE to 028 leaves the refresh addresses unchanged

- **Claims:** none. The `~` claims opus:AA-refptr.c5 and sol:AA-refptr.c5 are withdrawn (1.2); the joint observation over P1 and P2 (below) is the only line that varies COPCON bit 1.
- **Stimulus.** As P1, with COPCON ← 0x0000. Run RA0 uses value 0x5555 and run RB0 uses 0xAAAA.
- **Observation.** The refresh accesses (R2) from the Copper's fetch of L+4 to the end marker. The anchor is the fetch of the MOVE's first word, whose destination address is "Fetched during IR1 time" (p.20), before the Copper can act on the protection; no page says whether a MOVE the Copper may not perform still fetches its second word (see Open).
- **Expected relation.** The refresh address sequences of RA0 and RB0 are identical in every position. In fact the two port traces agree in every access (time, address, direction, data), except for the value word: the CPU's read of it from the program, its write into L+6, and the Copper's fetch of L+6 if the Copper makes one.
- **Open.** No AA page says what the Copper does with a MOVE it may not perform: whether it still fetches the second word (L+6), and what it does afterwards. The list ends right after the MOVE, and neither the anchor nor PR-2b needs the fetch of L+6, so nothing depends on that.
- **Preconditions.**
  - PR-2a. P1 is green (RA and RB differ). Otherwise an implementation in which REFPTR has no effect at all would make P2 green without testing the protection; P2 is then reported as not gradable. *Contrast:* RB writing RA's value (the selector `PR-2a`) makes the P1 relation false, and P2 must then refuse a verdict.
  - PR-2b. In RA0 and RB0 the Copper fetches L+0, L+2 and L+4 in this order, exactly once in the observed frame and before the end marker, with data 0x4001, 0xFFFE and 0x0028. A fetch of L+6 is neither required nor excluded. *Contrast:* as for PR-1a: with the end WAIT listed at L+0, L+4 is never fetched and the check fails.
- **Contrasting case.** P1 (COPCON bit 1 set): the sequences differ.

### P1+P2. Joint observation: with COPCON bit 1 clear the Copper's MOVE to REFPTR leaves the refresh addresses unchanged, where a MOVE with the bit set is effective

- **Claims:** none. Page 6: `REFPTR    & ~028  W    A        Refresh pointer`, with the legend "~=Address not writable by the Coprocessor unless COPCON bit 1 is set true." p.19 agrees: "(if 0, access to RGA>7E)". The ids opus:AA-refptr.c5 and sol:AA-refptr.c5 are withdrawn (1.2) and are printed on a RECORDED line; neither P1, P2 nor this line carries them.
- **Reading of the page.** The Copper may not write REFPTR's address unless COPCON bit 1 is set. A MOVE to 028 made with COPCON bit 1 clear that changes the refresh addresses would falsify it, and a MOVE can be seen to change them only where one with the bit set does (P1).
- **Assertion.** The refresh address sequences of RA0 (data 0x5555) and RB0 (data 0xAAAA), whose MOVE is made with COPCON bit 1 clear, are identical in every position, so this MOVE has no effect. It is graded under the preconditions PR-1a to PR-1c, PR-2a and PR-2b. PR-2a (RA and RB differ: the MOVE with the bit set is effective) is a precondition of the line, as it is of P2: where REFPTR has no effect at all, nothing could show the protection, and the line prints NOT GRADED.
- **Scope.** One Copper MOVE, at one beam position (the WAIT of L+0 and L+2), with two complementary data words: every data bit is written both as 0 and as 1. This is one observation. The universal protection of the legend (every legal Copper write, whenever COPCON bit 1 is clear) is not established by it, which is why the ids are withdrawn.
- **Contrasting cases.** RA0 and RB0 with COPCON ← 0x0002: the assertion goes red through the COPCON bit alone; the Copper list, the values and the preconditions are unchanged. RA and RB with COPCON ← 0x0000 does not reach the assertion: the Copper fetches no data word of a MOVE it may not perform, PR-1a is not met, and the line prints NOT GRADED (P2, Open: the pages do not say whether a MOVE the Copper may not perform still fetches its data word, so no graded case may depend on it).

### P3. BPLHPTH (1EC) and BPLHPTL (1EE) hold the UHRES bit-plane pointer. With UHRES enabled, the pointer's address is accessed exactly once on every line of the UHRES bit-plane range, starting at the written address

- **Claims:** opus:AA-bplhpth.c1, c2, c4, c6; opus:AA-bplhptl.c1, c2, c4, c6; sol:AA-bplhpth.c1, c2, c4, c6; sol:AA-bplhptl.c1, c2, c4, c6. Page 10 rows: `BPLHPTH   +h  1EC  W    A       VRam (UHRES) bit plane pointer (High 5 bits)` and `BPLHPTL   +h  1EE  W    A       VRam (UHRES) bit plane pointer (Low 15 bits)`. p.6 legend: "+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4." (The p.10 legend does not define `+`.)
- **Qualifying pages.** p.15: "When UHRES is enabled, this pointer comes out on the 2nd 'free' cycle after the start of each horiz. line. Its modulo is added every time it comes out." p.27 (SPRHDAT): "This identifies the cycle when this pointer address is on the bus accessing the memory." The alphabetical entries on p.15 agree with the p.10 rows in address and access letter.
- **Stimulus.** Runs U1, U2, U3, U0 and U0d, and UW for E3.6 (section 4).
- **Observation.** BPLH and stray accesses (R2), per line (R1), from the setup-done record to t(Vhi).
- **Expected.**
  - E3.1 ("each horiz. line"). Every line V with 0x051 ≤ V ≤ 0x06F has exactly one BPLH access. Line 0x050, the line of BPLHSTRT, is open (R1, "The line start is open"); line 0x070 is graded under P5. BEAMCON0 holds 0x0020 from prologue step 1, so DUAL = 0, and the chain starts at the normal counter's line start (p.12).
  - E3.2. The first BPLH access of the run lies on line 0x050 or 0x051 and is at P: 0x15AAAA in U1 and U3, 0x0A5554 in U2. The expected address is (BPLHPTH bits 4-0) << 16 | (BPLHPTL & 0xFFFE): high word DB0-DB4 → A20-A16, low word DB1-DB15 → A15-A1 (p.6).
  - E3.3. U1 and U2 write complementary patterns: together they set and clear every one of the 20 address bits, and each run must hit its own P. This exercises "(High 5 bits)" (the page's "old chips- 18 bits", p.6, would drop A20-A19). It does not tell which pointer bit drives which address bit: two bits of equal value in both runs can be exchanged without changing either address. E3.6 grades that.
  - E3.4. U3 (BPLHPTH written 0xFFF5) produces the same BPLH access times and addresses as U1, because the high word uses only DB0-DB4. This check is weak: bits above A20 do not exist in a 2 MB Chip memory, so it only catches DB5-DB15 leaking into A20-A1.
  - E3.5. No run has a stray access. It is graded only where U1 to U4 have UHRES accesses; with none, a non-graded line prints the empty population.
  - E3.6 (every used bit). In run UW (section 4), for each of the 40 cases n, the line 0x51 + n has a UHRES access at the case's expected BPLH address, (BPLHPTH bits 4-0) << 16 | (BPLHPTL & 0xFFFE) of the case written for that line. The 20 walking-one cases set each of DB1-DB15 of the low word and DB0-DB4 of the high word alone, and the 20 walking-zero cases clear each of them alone, so an access at the expected address shows that this bit, and no other, drives its address bit, with the unused bits (DB0 of the low word, DB5-DB15 of the high word) at 0 and at 1. The address of every case is checked, on its own line; a line without it fails the clause, an empty population included.
- **Preconditions.** PR-U1 to PR-U5; for E3.2 and E3.4 also PR-U6. E3.6 grades its own population.
- **Contrasting cases.**
  - U0 (BPLCON0 UHRES bit clear): no BPLH or SPRH access anywhere in the run. p.16: "UHRES= ultrahi res enables the UHRES pointers".
  - U0d (DMACON without DMAEN): none either. p.22: "Enable all DMA below (also UHRES dma)".
  - U2: the first address is 0x0A5554, not 0x15AAAA.
- **Open, not graded.**
  - Which memory cycle within the line. The page says "2nd 'free' cycle" and defines "'Free' means priority above the copper and below the fixed stuff (audio,sprites...)", but draws no slot diagram for AA and does not say where the line starts relative to the refresh slots.
  - Whether the access is a read or a write (ARW* polarity, BPLHWRM).
  - Which DMACON bits besides DMAEN are needed.
  - What data the access carries.

### P4. BPLHMOD (1E6) is the UHRES bit-plane modulo: from line to line, the BPLH address advances by the sign-extended modulo plus 2

- **Claims:** opus:AA-bplhmod.c1, c2, c5; sol:AA-bplhmod.c1, c2. Page 10: `BPLHMOD    h  1E6  W    A       UHRES bit plane modulo`.
- **Qualifying pages.**
  - p.15: "This is the number (sign extended) that is added to the UHRES bit plane pointer (BPLHPTL,H) every line, and then another 2 is added, just like the other modulos."
  - p.15: "Its modulo is added every time it comes out."
  - p.6: "MOD=15 bit Modulo ... Uses 15 bits, plus sign extend."
- **Stimulus.** U1 (BPLHMOD = 0x0010) and U2 (BPLHMOD = 0xFFF0 = -16). Both values are even and representable in 15 bits plus sign.
- **Observation.** The BPLH access of each line, lines 0x051-0x06F.
- **Expected.** Every line of 0x051..0x06F has exactly one BPLH access in U1 and in U2 (E3.1; the step is read between the accesses of two lines), and for every pair of consecutive lines V, V+1 in that range the address on V+1 minus the address on V is 2 + sign-extended BPLHMOD: +18 in U1 and -14 in U2. Graded on every pair. A run with no BPLH access has no modulo to show: the line fails, and does not print NOT GRADED.
- **Preconditions.** PR-U1 to PR-U5 hold. E3.1 is a clause of the line, not a precondition: it is the existence of the accesses the modulo is added to.
- **Contrasting case.** U2 against U1: the step changes from +18 to -14 and the addresses run downward from 0x0A5554.

### P5. BPLHSTRT (1D4) and BPLHSTOP (1D6) bound the lines on which the UHRES bit-plane pointer is fetched

- **Claims:** opus:AA-bplhstrt.c1, c2, c5; opus:AA-bplhstop.c1, c2, c5; sol:AA-bplhstrt.c1, c2; sol:AA-bplhstop.c1, c2. Page 10: `BPLHSTRT   h  1D4  W    A       UHRES bit plane vertical start` and `BPLHSTOP   h  1D6  W    A       UHRES bit plane vertical stop`.
- **Qualifying pages.** p.15: "BPLHSTRT ... This controls the line when the data fetch starts for the BPLHPTH,L pointers. V10-V0 on DB10-0." p.15: "BPLHSTOP ... This controls the line when the data fetch stops for the BPLHPTH,L pointers. ... 10-0 V10-V0". The comparison is with the vertical beam counter, the line number of R1.
- **Stimulus.** U1 (start 0x050, stop 0x070) and U4 (start 0x0F8, stop 0x108).
- **Observation.** BPLH accesses per line, from the setup-done record to t(Vhi).
- **Expected**, for each run with its own start and stop:
  - E5.1. No BPLH access between the setup-done record and t(start-1), that is, on no line before start. This covers every line of the observed frame from the setup up to start-1.
  - E5.2. The first BPLH access of the run lies on line start or on line start+1 ("the line when the data fetch starts"; R1, "The line start is open").
  - E5.3. Every line from start+1 to stop-1 has one (E3.1). In U4 this includes lines 0x100-0x107, which need V8.
  - E5.4. No BPLH access after t(stop), that is, on no line from stop+1 to Vhi.
- **Open, not graded.** Which of the lines start and start+1 carries the first access. Line "stop" itself: "the line when the data fetch stops" does not say whether that line still has a fetch.
- **Preconditions.** PR-U1 to PR-U5. E3.1 is not a precondition here: a start or stop on the wrong line breaks it, so E5.3 grades it as part of P5.
- **Contrasting case.** U4 against U1: there are no BPLH accesses on lines 0x050-0x06F (inside E5.1 for U4), and they appear on 0x0F8-0x107. U1 with BPLHSTRT = 0x058 against the expectation start 0x050: the first access lies on line 0x058 or 0x059, outside E5.2.

### P6. SPRHPTH (1E8) and SPRHPTL (1EA) hold the UHRES sprite pointer. It comes out twice on each line of its range, before and after the bit-plane pointer, starting at the written address and moving to higher addresses on each following line

- **Claims:** opus:AA-sprhpth.c1, c2, c4, c6; opus:AA-sprhptl.c1, c2, c4, c6; sol:AA-sprhpth.c1, c2, c4, c6; sol:AA-sprhptl.c1, c2, c4, c6. Page 10: `SPRHPTH   +h  1E8  W    A       UHRES sprite pointer (High 5 bits)` and `SPRHPTL   +h  1EA  W    A       UHRES sprite pointer (Low 15 bits)`. p.6 legend `+` as in P3.
- **Qualifying pages.**
  - p.27: "This pointer is activated in the 1st & 3rd 'free' cycles (see BPLHPTH,L) after horiz line start. It increments for the next line."
  - p.15: "The SHRHDAT gets the first and third free cycles." SHRHDAT names no register on any page; it is read as SPRHDAT, the UHRES sprite identifier (pp.7, 27).
  - p.27: SPRHDAT "identifies the cycle when this pointer address is on the bus accessing the memory."
- **Stimulus.** U1, U2, U3 and U0, and UW for E6.6.
- **Observation.** SPRH, BPLH and stray accesses per line, from the setup-done record to t(Vhi).
- **Expected.**
  - E6.1 ("1st & 3rd", two activations per line). Every line strictly inside the SPRH range, 0x059..0x067 in U1 to U3, has exactly two SPRH accesses.
  - E6.2. The first SPRH access of the run is at S: 0x132468 in U1 and U3, 0x0CDB96 in U2. The expected address is (SPRHPTH bits 4-0) << 16 | (SPRHPTL & 0xFFFE).
  - E6.3 ("increments for the next line"). For every pair of consecutive lines V, V+1 that both have SPRH accesses, every SPRH address on V+1 is greater than every SPRH address on V.
  - E6.4 (1st free / 2nd free / 3rd free). On every line with both SPRH and BPLH accesses (0x059..0x067), the time order is SPRH, BPLH, SPRH.
  - E6.5. U3 (SPRHPTH = 0xFFF3) gives the same SPRH addresses and times as U1, with the same limited sensitivity as E3.4.
  - E6.6 (every used bit). In run UW (section 4), for each of the 40 cases n, the line 0x51 + n has a UHRES access at the case's expected SPRH address, (SPRHPTH bits 4-0) << 16 | (SPRHPTL & 0xFFFE) of the case written for that line. The walking-one and walking-zero cases are those of E3.6 for the sprite pointer, which carries the case of index (n + 20) mod 40 so that the bit-plane and sprite addresses of one line differ. A line without the address fails the clause, an empty population included.
- **Open, not graded.** The size of the increment. Whether the two accesses of one line use the same address or consecutive addresses (E6.6 asks for an access at the case's address on the line, not for both). Which memory cycle within the line.
- **Preconditions.** PR-U1 to PR-U5; for E6.2, E6.3 and E6.5 also PR-U6, for E6.4 also PR-U7. E6.6 grades its own population.
- **Contrasting cases.** U0: no SPRH accesses. U2: the first address is 0x0CDB96. Lines 0x051-0x056 of U1 lie inside the BPLH range but at or below SPRHSTRT-2: they carry a BPLH access and no SPRH access (E7.1), so E6.1 is not satisfied by accesses that come on every line anyway.

### P7. SPRHSTRT (1D0) and SPRHSTOP (1D2) bound the lines on which the UHRES sprite pointer comes out

- **Claims:** opus:AA-sprhstrt.c1, c2, c5; opus:AA-sprhstop.c1, c2, c5; sol:AA-sprhstrt.c1, c2; sol:AA-sprhstop.c1, c2. Page 10: `SPRHSTRT   h  1D0  W    A       UHRES sprite vertical start` and `SPRHSTOP   h  1D2  W    A       UHRES sprite vertical stop`.
- **Qualifying pages.** p.28: "SPRHSTOP H 1D2 W A UHRES sprite vertical display stop", bit layout "SPRHWRM x x x x v10 ... v0". p.28: "SPRHSTRT H 1D0 W A UHRES sprite vertical display start", bit layout "x x x x x v10 ... v0".
- **Stimulus.** U1 (0x058-0x068) and U4 (0x100-0x110).
- **Observation.** SPRH accesses per line, from the setup-done record to t(Vhi).
- **Expected**, for each run with its own start and stop:
  - E7.1. No SPRH access before t(start-2), that is, on no line up to start-2.
  - E7.2. Every line from start+1 to stop-1 has exactly two SPRH accesses (E6.1). In U4 these are lines 0x101-0x10F, which need V8.
  - E7.3. No SPRH access after t(stop), that is, on no line from stop+1 on.
- **Open, not graded.** Lines start-1, start and stop. Unlike BPLHSTRT, p.28 calls these the sprite's vertical *display* start and stop and does not say on which line the fetch for the first displayed line happens, nor whether the stop line still has a fetch.
- **Preconditions.** PR-U1 to PR-U5.
- **Contrasting case.** U4 against U1: no SPRH accesses on 0x059-0x067 (inside E7.1 for U4); they appear on 0x101-0x10F.

### P8. HHPOSW (1D8) writes and HHPOSR (1DA) reads the DUAL-mode horizontal beam counter, which counts up to HTOTAL and then starts again

- **Claims:** opus:AA-hhposw.c1, c2, c5; opus:AA-hhposr.c1, c2, c5; sol:AA-hhposw.c1, c2, c5; sol:AA-hhposr.c1, c2, c5. Page 10: `HHPOSW     h  1D8  W    A       DUAL mode hires H beam counter write` and `HHPOSR     h  1DA  R    A       DUAL mode hires H beam counter read`.
- **Qualifying pages.**
  - p.24: "HHPOSR H 1DA R A DUAL mode hires H beam counter read / HHPOSW H 1D8 W A DUAL mode hires H beam counter write / This is the secondary beam counter for the faster mode, triggering the UHRES pointers & doing the comparisons for HBSTRT,STOP,HTOTAL,HSSTRT,HSSTOP (See HTOTAL for bits)".
  - p.24 HTOTAL: bits 7-0 = h8-h1; "Horiz line has this many + 1 280ns increments. If the PAL bit & LOLDIS are not high, long line/short line toggle will occur ... Active if VARBEAMEN=1 or DUAL=1."
  - p.12 BEAMCON0: "LPENDIS= ... When the bit is a high the light-pen latched value is ignored and the actual beam counter position is read by VHPOSR, VPOSR and HHPOSR."
- **Stimulus.**
  1. Prologue, and DMACON stays 0.
  2. HTOTAL ← N.
  3. BEAMCON0 ← 0x2860: LPENDIS (bit 13), LOLDIS (bit 11), DUAL (bit 6), PAL (bit 5). All other bits are 0, VARBEAMEN among them. PAL is written, not taken from the harness: with the PAL bit and LOLDIS both high, no long line/short line toggle occurs under any reading of "If the PAL bit & LOLDIS are not high, long line/short line toggle will occur" (p.24), so the modulus stays N+1.
  4. HHPOSW ← X.
  5. At once, 64 times: read HHPOSR and store the word to BUF + 2i.
  6. Epilogue.

  | run | N | X |
  |---|---|---|
  | HA | 0x0071 | 0x0010 |
  | HB | 0x0071 | 0x0040 |
  | HC | 0x0051 | 0x0010 (contrast) |
  | HD | 0x0071 | 0x0010 (contrast: the same as HA) |
  | HW | 0x0071 | 0x0069 (N - 8: the counter must pass N and start again before the first read) |

- **Observation.** The 64 sample stores: their data r[i] (bits 7-0 graded; bits 15-8 are not stated on any page) and their port times T[i], converted to mc. Graded interval: the first to the last sample store.
- **Expected.** The counter's modulus is N+1 (0x72 = 114 for HA and HB).
  - E8.1 (write sets the counter; R3 pair HA/HB). For every i, (rB[i] - rA[i]) mod (N+1) = (0x40 - 0x10) mod (N+1) = 0x30.
  - E8.2 (HTOTAL is the highest count, and the count after it is 0). (i) Every sample in HA, HB, HC and HW satisfies r[i] ≤ N of its run. (ii) HW writes X = N - 8 = 0x69. Its first read follows the write by Λ_W mc (PR-8d), and Λ_W is more than 8, so a counter that counts up to N and then starts again has passed N and restarted before the first read: r[0] < X, and (r[0] - X) mod (N+1) ≤ Λ_W + 1 as in E8.3. (iii) In HW every d[i] ≥ 1 (E8.4's count difference): a counter that does not move reaches no highest count and has none to start again from. A sample at 0 on every read satisfies (i) and (ii) and fails (iii).
  - E8.3 (the write is the starting point). In each run, (r[0] - X) mod (N+1) ≤ Λ + 1, where Λ is the latency, in mc, from the port fetch of the last word of the instruction that writes HHPOSW to the first sample store (PR-8d; Λ is printed). The write happens after that fetch and the first read before that store, so the counter has advanced by at most Λ counts at the first read, and one more for the quantization of the count.
  - E8.4 (it counts). Let d[i] = (r[i+1] - r[i]) mod (N+1). Every d[i] ≥ 1 in HA, HB and HC. This is the whole of the graded line. The rate, one count per 280 ns, is not graded: Σ d[i] and the elapsed time T[63] - T[0] in mc are printed per run (MEASURED). The page states the step (p.24, "this many + 1 280ns increments") but no tolerance, and the only agreement the package can state between the two numbers rests on a tolerance built from its own jitter and conversion measurements. The one reference without such a tolerance would be the H field of VHPOSR, which also counts 280 ns steps (p.29), read in the same loop; but two reads one instruction apart straddle a step in some passes, so that comparison carries a unit of quantization per segment that no page states. The rate clause is withdrawn to MEASURED.
- **Preconditions.**
  - PR-8a. HA and HB agree in time, address and direction for every port access. Their data differ only in the fetch of the immediate operand X and in the sample stores. Then the counter is read at the same moments in both runs, as R3 requires. *Contrast:* add one NOP before the loop in HB only; PR-8a fails.
  - PR-8b. The largest sample interval, max(T[i+1] - T[i]), is below (N+1)/2 mc in HA, HB, HC and HW, so each d[i] is unambiguous. *Contrast:* insert a delay of about 100 mc into the loop; PR-8b fails.
  - PR-8c. The end marker is present and 64 sample stores are seen, in HA, HB, HC and HW.
  - PR-8d. In HA, HB, HC and HW the port shows the fetch of the last word of the instruction that writes HHPOSW, the first sample store follows it, and the latency Λ from that fetch to the first sample store is below (N+1)/2 mc, so that the count difference E8.3 reads is unambiguous; in HW Λ_W is also more than N - X = 8 mc, so that E8.2 (ii) expects a restart. *Contrast:* a delay of about 100 mc between the write and the first read; PR-8d fails.
- **Contrasting cases.** HD against HA (same X): rD[i] = rA[i] for every i, a predicted difference of 0. HC (N = 0x51): every sample is ≤ 0x51, and E8.1, E8.3 and E8.4 hold with modulus 0x52. HC writing X = 0x30 against the expectation X = 0x10: the first sample lies 32 counts beyond the expected start, outside E8.3. HW writing HTOTAL = 0x00A1 against the expectation N = 0x71: the first sample is X + Λ_W = about 0x7D, above N, outside E8.2 (i).
- **Open, not graded.**
  - Bits 15-8 of HHPOSR.
  - What HHPOSR returns and whether the counter runs when DUAL = 0; the page names it the DUAL-mode counter only.
  - The name says "hires H beam counter", but its bits are HTOTAL's, which counts 280 ns steps. E8.1 to E8.4 hold for any count rate; the rate is measured and not graded (E8.4, section 6, item 10).

### P9. In DUAL mode the counter of P8 restarts the UHRES pointer chain at each of its line starts, so the UHRES bit-plane pointer comes out once every HTOTAL+1 memory cycles: more than once per ordinary line

- **Claims:** none. P9 grades p.12's DUAL sentence and p.24's "triggering the UHRES pointers", which no claim of this package quotes. It neither writes HHPOSW nor reads HHPOSR, so it does not grade the c5 claims of those registers (the functions "DUAL mode hires H beam counter write" and "DUAL mode hires H beam counter read"); P8 writes and reads the counter and grades them whole. The BPLH pointer claims are graded whole by P3.
- **Qualifying pages.**
  - p.24: "This is the secondary beam counter for the faster mode, triggering the UHRES pointers".
  - p.12: "DUAL= Run the horizontal comparators with the alternate horizontal beam counter, and starts the UHRES pointer chain with the reset of this counter rather than the normal one. This allows the UHRES pointers to come out more than once in a horizontal line, assuming there is some memory bandwidth left (It doesn't work in 640*400*4 interlace mode)".
  - p.24 HTOTAL as in P8.
- **Stimulus.** Run D1: as U1, except BPLHMOD = 0x0000 and SPRHSTRT = 0x00F0, SPRHSTOP = 0x00F4 (outside the window). Between setup steps 3 and 4 the program writes HTOTAL ← 0x0071 and BEAMCON0 ← 0x2860 (as P8). N+1 = 114 is even, so the alternate line keeps the parity of the memory cycles. Only refresh, null-sprite DMA and the CPU compete, so there is "memory bandwidth left". Run D0 is identical except BEAMCON0 ← 0x2820 (DUAL = 0). Run DH is D1 with the marker loop replaced by the sampling of P8: after the setup-done record the CPU polls VPOSR and VHPOSR until the line number is ≥ 0x054 (the wait of P1), reads HHPOSR 64 times at once, storing each word to BUF + 2i, and runs the epilogue. DH reads the alternate counter while the chain runs, so that its resets are seen directly.
- **Observation.** D1: the BPLH accesses strictly between t(0x051) and t(0x06F), each assigned to its horizontal line by the H counter (R1): the line V of the marker interval it lies in, or V - 1 when its beam position p is negative. D0: the same count. DH: the 64 samples of HHPOSR (r[i], bits 7-0, and port times T[i]) and the BPLH accesses of DH between T[0] and T[63].
- **Expected.** No count is derived from a window length, and no band is placed around a count.
  - E9.1a ("more than once in a horizontal line"). In D1 at least one horizontal line of 0x052..0x06F has two or more BPLH accesses.
  - E9.1b ("starts the UHRES pointer chain with the reset of this counter"). In DH, with at least three BPLH accesses a[0] < a[1] < ... between T[0] and T[63], the alternate counter is reset exactly once between every two consecutive accesses: each reset starts the chain, and the chain gives one BPLH access. The resets between two accesses are counted from HHPOSR. For an access a, with i the last sample with T[i] < a, the phase of the alternate counter at a is φ(a) = (r[i] + (a - T[i])/K) mod (N+1), in counts of 280 ns (K, section 2). The number of resets between a[k] and a[k+1] is n[k] = round((φ(a[k]) + (a[k+1] - a[k])/K - φ(a[k+1])) / (N+1)), the whole number of periods N+1 = 114 that the phase passed. Graded: n[k] = 1 for every k. The read of a sample precedes the store of its word by a fixed delay, which shifts every φ alike and drops out of n[k]; the rounding is to a whole number of periods, not a tolerance on a time. An implementation that restarts once per ordinary line gives n[k] = 2, one that restarts twice per period gives n[k] = 0.
  - E9.2. In D1, each BPLH address is the previous one + 2, for every consecutive pair. BPLHMOD is 0, so both p.15 sentences ("every line, and then another 2 is added" and "added every time it comes out") give the same step.
- **Preconditions.**
  - PR-U1, PR-U2, PR-U3 and PR-U4 as for the UHRES runs. PR-U5 does not apply: the accesses now occur throughout the line; E9.1a assigns them to lines by the H counter itself. The fixture, R2's cross-check and the end marker are made over D1, D0 and DH.
  - PR-9-U5. D1 has at least two BPLH accesses between t(0x051) and t(0x06F); E9.2 reads a step between two of them and is graded only when it holds. *Contrast:* D1 with the UHRES bit clear has none.
  - PR-9b. The marker loop's first instruction (R1) is found in the programs of D1 and D0 and is fetched before the stores of markers 0x040 and 0x080, so that the sample times t_s exist. *Contrast:* D1 with one NOP between the loop's first and second instruction: the instruction sequence of R1 is not found and PR-9b fails.
  - PR-9c. DH has 64 sample stores and its largest sample interval is below (N+1)/2 = 57 mc, so each phase step is unambiguous. *Contrast:* DH with a delay of about 100 mc in its loop; PR-9c fails.
  - PR-9a. The ordinary line length is unchanged by DUAL. The length of the main counter's line is measured from the H counter and the markers 0x040 and 0x080 of D1 and of D0, which sample VHPOSR on 64 consecutive lines: L = ((t_s(0x080) - t_s(0x040))/K - (h(0x080) - h(0x040))) / 64 mc; the H fields remove the position at which each sample was taken. Both runs set LOLDIS, so no long line/short line toggle occurs (p.24), and a line has a whole number of 280 ns steps (p.24, "this many + 1 280ns increments"). Graded: L of D1 and L of D0, each rounded to a whole number of steps, are equal. D0 (DUAL = 0) is the reference: its main counter is the ordinary one. L of U1 is printed. A DUAL that changed the main counter would change the number of steps in a line, which no page lets it do. *Contrast:* D1 with BEAMCON0 ← 0x28E0 (VARBEAMEN added, "allowing different beam counter total values ... on the main horiz counter", p.12) must fail PR-9a. VARBEAMEN also puts the field length under VTOTAL ("Highest numbered Vertical line (VARBEAMEN=1)", p.10; "there's this many + 1 in a field", p.29), and no page gives VTOTAL's value after reset, so this contrast writes VTOTAL (1C8) ← 0x0100 before BEAMCON0: every marker line 0x040-0x080 then stays in the field, the run still reaches its end marker, and PR-U1 and PR-U2 still hold.
- **Contrasting case.** D0 (DUAL = 0): one BPLH access on each of the 30 lines 0x052..0x06F as in E3.1 (K = 30). D1 has a line with two or more.
- **Open, not graded.**
  - The exact offset of each access after a reset of the alternate counter.
  - In DUAL mode with a non-zero BPLHMOD, the step between two accesses of one ordinary line. The two p.15 sentences differ there (section 6, item 4).

---

## 6. What the pages leave open or state inconsistently

1. **Chip-set marks.** The p.6 legend reads "h=new for HiRes chip set" and "p=new for IAA chip set". The p.10 legend reads "P= new register in Pandora Chip Set", "p= stuff added or changedin hires chips", "H= new register in hires chips", "h= stuff added or changed in hires chips". So p.10 gives lowercase `p` and lowercase `h` the same meaning, and gives `p` a different meaning than p.6 does. BPLHSTOP is marked `h` in the address list (p.10) and `p` in the alphabetical list (p.15). The other UHRES registers carry `H` there (pp.15, 24, 27, 28). Every reading places these registers in the hires chips that AA extends, so no expected value here depends on the choice.
2. **Undefined legend symbols.** The p.10 legend does not define `+`, `&` or `~`. Only p.6 does.
3. **"SHRHDAT"** (p.15) is not a register name anywhere. It is read as SPRHDAT; p.27 independently gives the UHRES sprite pointer the 1st and 3rd free cycles.
4. **Modulo in DUAL mode.** p.15 says the modulo is added "every line, and then another 2 is added" (BPLHMOD) and also "Its modulo is added every time it comes out" (BPLHPTH,L). With one access per line the two agree. In DUAL mode they may not, so P9 grades the step only with BPLHMOD = 0.
5. **BPLHSTOP.** "the line when the data fetch stops" leaves open whether the stop line itself is fetched.
6. **SPRHSTRT/SPRHSTOP** are called "vertical display start" and "vertical display stop" (p.28) without saying on which line the fetch happens. **SPRHPTH,L** "increments for the next line" (p.27) with no step and no statement about the two accesses within one line.
7. **UHRES needs DMA enables.** "(also needs bits in DMACON)" (p.16) does not name the bits. p.22 names only DMAEN ("also UHRES dma").
8. **"Free" cycle** (p.15) is defined only by priority. No AA page gives the slot positions of the "fixed stuff" or of the line start, so the exact memory cycle of a UHRES access cannot be predicted.
9. **REFPTR.** p.27 gives no bit layout and no rule for how the written value becomes refresh addresses. It calls the register "writeable for test purposes only" and says it "should never be written by the microprocesor". P1 therefore writes it with the Copper and grades only a difference between two written values.
10. **HHPOSR/HHPOSW** are called a "hires H beam counter" (p.10, p.24) but refer to HTOTAL for their bits, and HTOTAL counts 280 ns increments. Bits 15-8 of HHPOSR are not stated. The HTOTAL row prints bit "10" where bit 01 is meant, and the HSSTRT row repeats "for HSYNC stop" (p.24); neither affects this package.
11. **Copper access limits.** The AA pages agree with each other: p.6 `~` on 000-07E, and p.19 "(if 0, access to RGA>7E)". Two HRM statements differ from them, and neither is used:
    - HRM p.21, checked against the print, says the Copper can store into "Any register whose address is $20 or above", which would make 028 writable without CDANG.
    - HRM p.310 (ECS) says that with CDANG clear the Copper can reach $03E-$07E. This conflicts with the AA page for 040-07E, but not for 028 or 038-03C.
12. **Alphabetical entries.** Every claim says its register's alphabetical entry lies outside its slice. In fact the entries are on pp.15, 24, 27, 28 and 29, and they agree with the address-list rows in address and access letter for all 15 registers. The function wording differs only slightly ("vertical display start" and "vertical display stop" for the sprite pair; "UHRES (VRAM) bit plane pntr" for the bit-plane pair).
13. **The start of the UHRES chain.** p.12 says DUAL "starts the UHRES pointer chain with the reset of this counter rather than the normal one", which puts the chain at the reset of the counter in use. No page says at which colour clock after that reset the free cycles fall, at which clock the V field steps after the H field wraps, or whether the chain's vertical comparison sees the new line's V. R1 therefore assigns the first accesses of a line to the line the page names or to the next one, and P3, P4 and P5 leave the line of BPLHSTRT open (R1, "The line start is open").

---

## 7. Build

### 7.1 Files and programs

`make -k -B test` assembles every program, builds nanomig.v once per CPU core and runs `test_aa_integ_s7_rest` on each; the test takes the package directory as its argument. Every run loads one program, selects the AA chip set (`chipset_config` = $10), boots, and runs until the program stores the end marker or the budget of 7 000 000 clk_sys cycles after the boot is used; the port log, the `refresh` output of nanomig.v (stepped with `run_cycles(1)` and read in each step) and the words the program stored are all the test sees.

| file | role |
|---|---|
| `Makefile` | `TEST_CPP`, `PROGS`, `include ../harness/nanomig.mk` |
| `test_aa_integ_s7_rest.cpp` | `main`; the `// MUTATION` records |
| `s7r_core.h` | the run, the port log without held bus cycles, the `refresh` mark, the classification of R2, the markers of R1, the result cache |
| `s7r_hv.h` | run HV's conversion, `graded()` (a line is NOT GRADED when its preconditions do not hold) |
| `s7r_p8.h` | run HV, P8 |
| `s7r_u.h` | the UHRES run model, run UR, the cross-check of R2 |
| `s7r_p1.h` | P1, P2, P1+P2 |
| `s7r_p3.h` | P3 to P7, P9 |
| `s7r.i` | the common prologue, the vertical-blank wait, the marker loop of R1, the setup-done record, the epilogue |
| `prog_samp.s` | run HV and the P8 runs (parameters `SAMP_*`) |
| `prog_ref.s` | the P1 and P2 runs (parameters `REF_*`) |
| `prog_u.s` | the UHRES runs, UR, UW and the dual-mode runs D1, D0 and DH (parameters `U_*`) |
| `walk.i` | the 40 cases of run UW (section 4): the four pointer words of each case |
| `prog_<run>.s` | a wrapper: the run's symbols, then `include` of its family's source |

Run to wrapper: HV `prog_hv`; HA, HB, HC, HW `prog_ha`, `prog_hb`, `prog_hc`, `prog_hw` (HD runs `prog_ha` again); RA `prog_ra` (RE runs it again), RB `prog_rb`, RNA `prog_rna`, RNB `prog_rnb`, RA0 `prog_ra0`, RB0 `prog_rb0`; U1 to U4 `prog_u1` to `prog_u4`, U0 `prog_u0`, U0d `prog_u0d`, UR `prog_ur`, UW `prog_uw`; D1 `prog_d1`, D0 `prog_d0`, DH `prog_dh`. The other wrappers are the mutants of 7.6.

### 7.2 What the build adds to sections 2 to 5

1. **Fixture.** Every program stores the constant $BEEF at FIXTURE = 0x0C0800 before anything else, and the test asserts it (`FIXTURE-*`) before any other line of the family. FIXTURE + 2 receives the word 1 as the last store of prologue step 1; its port time is "the end of prologue step 1" of R2. The setup-done record sits at MK - 4 = 0x0C0000 (words v, h). The fixture lines say nothing about the end marker; each family's own preconditions do (PR-U1, PR-8c, PR-1a, PR-9-U1).
2. **Zero block.** Z is zero because the harness memory starts at zero; no program writes it.
3. **Marker loop.** The loop of R1 samples VHPOSR in a tight loop (about 15 memory cycles per pass) and tests the low byte against the gate with a subtract and an unsigned compare; the anchor of R1 takes the gate from the program (the loop of the PR-U5 contrast uses [$B0, $E0), item 11). Its first instruction (`move.w (a2),d0`) is fetched once per pass; PR-U5, PR-9a and PR-9b find it in the program binary (the four words $3012 $1200 $9205 $B206 of `MARKLOOP`) and take the sample time t_s of R1 from its fetches. In UW the loop also writes the four pointer registers of the next case after the marker store of each line 0x050 to 0x077, from a table addressed by A4.
4. **Run HV pairing.** Two consecutive samples with the same V field whose H field decreased straddle a line start (the V field of VHPOSR changes after the H field wraps, one sample later); such a pair is not a pair of one line and is counted apart, and no V value may have more than one of them. The anchor needs at least 16 pairs, each with an H advance of at least 1. The pairs that remain form the S_HV segments of section 2 (a segment ends at a pair that is left out); S_HV, J_HV and the largest number of decreasing pairs for one V value are printed with the conversion.
5. **R2.** An access is a refresh access if the `refresh` output was high in the clk_sys cycle of its log sample or in one of the three before it. A stretched bus cycle is one access (the entries `held_continuations` marks are dropped). The cross-check compares the port times of the refresh-marked accesses of a run with the port times of UR's non-CPU accesses between the end of prologue step 1 and the run's end marker, and requires the lists to be equal and not empty. R2-UR requires a refresh access between every two consecutive UR markers.
6. **P1 and P2 programs.** The CPU waits for line $60 by reading VPOSR (bits 2-0) and VHPOSR (bits 15-8). PR-1b also requires that the refresh accesses after the fetch of L+6 agree in count and port time in RA and RB, so that the positions of the two sequences correspond. PR-1c: the refresh accesses of RA and of RB are counted in each complete line V of run UR with the start T0(V) of R1 (section 3), from the line whose start follows the fetch of L+6 to the last line whose end precedes the end marker; the least and the greatest count per line are printed. PR-2a is printed as a precondition line: it fails whenever P1 fails, and P2 is then NOT GRADED.
7. **P8.** E8.3's latency Λ is measured from the port read of the last word of the instruction `move.w #X,HHPOSW(a0)` (opcode $317C, displacement $01D8), found in the program binary, to the port write of the first sample; it is converted with the conversion of run HV and printed per run. E8.4 grades the count differences only; Σ d and the elapsed time of each run are printed. E8.2 (ii) uses the same latency Λ for run HW.
8. **P9.** PR-9b finds the loop's first instruction in the programs of D1 and D0 and its last fetch before each of the markers 0x040 and 0x080; PR-9a prints L of D1, D0 and U1 and compares the rounded L of D1 and D0. DH is the program of D1 with `U_HSAMP` set. The preconditions PR-U1, PR-U2, PR-U3 and PR-U4 of P9 are printed as PR-9-U1 to PR-9-U4, over D1 and D0 (PR-9-U4 is run U0's result; the end marker of PR-9-U1 is also required of DH), PR-9-U5 is the population of E9.2, PR-9c is DH's sample population and interval, and R2-XCHK-D is the cross-check of D1, D0 and DH. E9.1b prints the number of accesses and the numbers n[k] of DH.
9. **BPLH and SPRH ranges.** For a run the BPLH range is the smallest to the largest expected BPLH address over the lines start to stop-1, widened by $100 bytes on each side; an access of the UHRES class inside it is a BPLH access, inside [S, S + $1000) an SPRH access, any other a stray. The UHRES class is every access between the setup-done record and the end marker that is not a refresh, CPU or zero-block access (a read inside L counts here as well: no UHRES run enables the Copper).
10. **UR's window.** UR's marker window is $040 to $121, one line beyond the table of section 4, so that UR ends after every run its refresh accesses are compared with (U4 ends inside line $120); its 226 markers end at 0x0C038B.
11. **The PR-U5 contrast.** The loop is gated at H in [$B0, $E0) instead of [$60, $90). A sample at H below 3 shows the previous line's V field, so a gate that starts at 0 skips lines (a skipped line is found one frame later and the run does not end within its budget). With the gate at the end of the line the sample shows line V and the store of its marker falls in the first part of line V+1, next to the line start, where the page places the UHRES accesses; the gate is as wide as the normal one. An access of the start of line V+1 that precedes the store of marker V then lies after the fetch of the loop's first instruction that sampled marker V, and PR-U5 reports it.
12. **The contrast of P1+P2 for RA and RB with COPCON = $0000.** The Copper does not fetch the data word of a MOVE it may not perform on either core (RA0 and RB0 show L+0, L+2 and L+4 fetched, L+6 not), so PR-1a is not met and the joint assertion is NOT GRADED in that run (selector `P1P2-JOINT-A`). The joint is contrasted by RA0 and RB0 with COPCON = $0002 (selector `P1P2-JOINT-B`); PR-2a is contrasted by RB writing RA's value (selector `PR-2a`), which makes the joint NOT GRADED.
13. **Empty populations.** PR-U5 and P3-E5 are graded only where U1 to U4 and UW (PR-U5) or U1 to U4 (P3-E5) have UHRES accesses; with none, a RECORDED line `NOT GRADED PR-U5` or `NOT GRADED P3-E5` prints and the population is printed (`MEASURED: PR-U5 population`). The lines that need accesses and grade their own population (P3-E1, P3-E6, P4, P5, P6-E1, P6-E6, P7, P9-E1) fail when there are none; the lines that read a value, an increment or an order from accesses (P3-E2, P3-E4, P6-E2, P6-E3, P6-E4, P6-E5, P9-E2) are graded only when PR-U6, PR-U7 or PR-9-U5 shows that the accesses exist, and print NOT GRADED otherwise.

### 7.3 Selectors

`NMH_MUTATE=<selector>` selects a mutant (7.6); with it unset no mutant runs. `NMH_ONLY=<part>[,<part>]` restricts a run to the parts `HV` (run HV, UR and the lines of section 2), `REF`, `P8` and `U` (P3 to P7 and P9); each part needs the HV and UR runs. `NMH_CACHE=<directory>` keeps the capture of every run of a program on a design: a simulation of one program on one design is deterministic, so a mutant run reads the captures of the runs it leaves unchanged and simulates only the runs whose program it changed. The entry carries the program image and the run settings, and the directory is emptied whenever the design under test changes. The normal run (`make test`) sets none of them.

### 7.4 Measured (the final run of every core)

Shipped design (`make -k -B test`): TG68K 36 passed, 18 failed, fx68k 36 passed, 18 failed. Page-built copy (7.5): TG68K 66 passed, 0 failed, fx68k 66 passed, 0 failed. A precondition that fails is counted as a failed line; a graded line whose preconditions fail prints `NOT GRADED`.

| line | shipped TG68K | shipped fx68k | copy TG68K | copy fx68k |
|---|---|---|---|---|
| FIXTURE-HV | PASS | PASS | PASS | PASS |
| HV-ANCHOR | PASS | PASS | PASS | PASS |
| FIXTURE-UR | PASS | PASS | PASS | PASS |
| R2-UR | PASS | PASS | PASS | PASS |
| FIXTURE-REF | PASS | PASS | PASS | PASS |
| R2-XCHK-REF | PASS | PASS | PASS | PASS |
| PR-1a | PASS | PASS | PASS | PASS |
| PR-1b | PASS | PASS | PASS | PASS |
| PR-1c | PASS | PASS | PASS | PASS |
| P1 | FAIL | FAIL | PASS | PASS |
| P1-RE | PASS | PASS | PASS | PASS |
| P1-RN | PASS | PASS | PASS | PASS |
| PR-2b | PASS | PASS | PASS | PASS |
| PR-2a | FAIL | FAIL | PASS | PASS |
| P2 | NOT GRADED | NOT GRADED | PASS | PASS |
| P1P2-JOINT | NOT GRADED | NOT GRADED | PASS | PASS |
| FIXTURE-U | PASS | PASS | PASS | PASS |
| R2-XCHK-U | PASS | PASS | PASS | PASS |
| PR-U1 | PASS | PASS | PASS | PASS |
| PR-U2 | PASS | PASS | PASS | PASS |
| PR-U3 | PASS | PASS | PASS | PASS |
| PR-U4 | PASS | PASS | PASS | PASS |
| PR-U5 | NOT GRADED | NOT GRADED | PASS | PASS |
| PR-U6 | FAIL | FAIL | PASS | PASS |
| PR-U7 | FAIL | FAIL | PASS | PASS |
| P3-E1 | FAIL | FAIL | PASS | PASS |
| P3-E2 | NOT GRADED | NOT GRADED | PASS | PASS |
| P3-E4 | NOT GRADED | NOT GRADED | PASS | PASS |
| P3-E5 | NOT GRADED | NOT GRADED | PASS | PASS |
| P3-AGG | FAIL | FAIL | PASS | PASS |
| P3-U0 | PASS | PASS | PASS | PASS |
| P3-U0d | PASS | PASS | PASS | PASS |
| P4 | NOT GRADED | NOT GRADED | PASS | PASS |
| P5 | FAIL | FAIL | PASS | PASS |
| P6-E1 | FAIL | FAIL | PASS | PASS |
| P6-E2 | NOT GRADED | NOT GRADED | PASS | PASS |
| P6-E3 | NOT GRADED | NOT GRADED | PASS | PASS |
| P6-E4 | NOT GRADED | NOT GRADED | PASS | PASS |
| P6-E5 | NOT GRADED | NOT GRADED | PASS | PASS |
| P6-CTRL | FAIL | FAIL | PASS | PASS |
| P6-U0 | PASS | PASS | PASS | PASS |
| P6-AGG | FAIL | FAIL | PASS | PASS |
| P7 | FAIL | FAIL | PASS | PASS |
| FIXTURE-D | PASS | PASS | PASS | PASS |
| R2-XCHK-D | PASS | PASS | PASS | PASS |
| PR-9-U1 | PASS | PASS | PASS | PASS |
| PR-9-U2 | PASS | PASS | PASS | PASS |
| PR-9-U3 | PASS | PASS | PASS | PASS |
| PR-9-U4 | PASS | PASS | PASS | PASS |
| PR-9b | PASS | PASS | PASS | PASS |
| PR-9a | PASS | PASS | PASS | PASS |
| PR-9-U5 | FAIL | FAIL | PASS | PASS |
| P9-E1 | FAIL | FAIL | PASS | PASS |
| P9-E2 | NOT GRADED | NOT GRADED | PASS | PASS |
| P9-D0 | FAIL | FAIL | PASS | PASS |
| FIXTURE-P8 | PASS | PASS | PASS | PASS |
| PR-8a | PASS | PASS | PASS | PASS |
| PR-8b | PASS | PASS | PASS | PASS |
| PR-8c | PASS | PASS | PASS | PASS |
| PR-8d | PASS | PASS | PASS | PASS |
| P8-E1 | FAIL | FAIL | PASS | PASS |
| P8-E2 | PASS | PASS | PASS | PASS |
| P8-E3 | FAIL | FAIL | PASS | PASS |
| P8-E4 | FAIL | FAIL | PASS | PASS |
| P8-HD | PASS | PASS | PASS | PASS |
| P8-AGG | FAIL | FAIL | PASS | PASS |

The lines that print MEASURED (final run), shipped design then page-built copy:

shipped design, TG68K:

```text
MEASURED: HV: 58 pairs in 5 segments (S_HV), 1 same-V pair(s) with a decreasing H field left out, at most 1 for one V value; J_HV 1.001 memory cycles; conversion 7.990 clk_sys cycles per memory cycle (tg68k)
MEASURED: UR: 66497 accesses, markers 226 of 226 (tg68k)
MEASURED: P1: refresh accesses between the fetch of L+6 and the end marker: RA 129, RB 129; first RA address $000000 RB $000000 (tg68k)
MEASURED: PR-U5 population: 0 UHRES accesses in U1, U2, U3 and U4 (tg68k)
MEASURED: U1: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (tg68k)
MEASURED: U2: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (tg68k)
MEASURED: U3: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (tg68k)
MEASURED: U4: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 15, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $0F8), first SPRH access on line none (SPRHSTRT $100) (tg68k)
MEASURED: U0: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (tg68k)
MEASURED: U0d: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (tg68k)
MEASURED: PR-9b: marker-loop head fetches between the first and the last marker: U1 841, D1 841, D0 841; largest pass 56.069 memory cycles (tg68k)
MEASURED: PR-9a: t($80) - t($40) in memory cycles: U1 14543.889, D1 14543.889, D0 14543.889; largest difference of D1 or D0 from U1 0.000, bound twice the largest pass 112.138 (tg68k)
MEASURED: P9: D1 has 0 BPLH accesses in a window of 6828.399 memory cycles (HTOTAL+1 = 114); D0 has 0 in 6828.399 (tg68k)
MEASURED: P8: latency from the fetch of the last word of the HHPOSW instruction to the first sample store, in memory cycles: HA 20.025 HB 20.025 HC 20.025 (tg68k)
MEASURED: P8: largest sample interval in memory cycles HA 15.018 HB 15.018 HC 15.018, smallest HA 14.017 (tg68k)
MEASURED: P8: first samples (bits 7-0) HA: 00 00 00 00 00 00 HB: 00 00 00 00 00 00 HC: 00 00 00 00 00 00 HD: 00 00 00 00 00 00
MEASURED: P8-E4 values (memory cycles), S_HV 5 J_HV 1.001: HA: sum d 0 elapsed 887.091 J 1.001 tolerance 11.919 HB: sum d 0 elapsed 887.091 J 1.001 tolerance 11.919 HC: sum d 0 elapsed 887.091 J 1.001 tolerance 11.919 (tg68k)
```

shipped design, fx68k:

```text
MEASURED: HV: 58 pairs in 5 segments (S_HV), 1 same-V pair(s) with a decreasing H field left out, at most 1 for one V value; J_HV 1.000 memory cycles; conversion 8.000 clk_sys cycles per memory cycle (fx68k)
MEASURED: UR: 58429 accesses, markers 226 of 226 (fx68k)
MEASURED: P1: refresh accesses between the fetch of L+6 and the end marker: RA 129, RB 129; first RA address $000000 RB $000000 (fx68k)
MEASURED: PR-U5 population: 0 UHRES accesses in U1, U2, U3 and U4 (fx68k)
MEASURED: U1: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (fx68k)
MEASURED: U2: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (fx68k)
MEASURED: U3: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (fx68k)
MEASURED: U4: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 15, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $0F8), first SPRH access on line none (SPRHSTRT $100) (fx68k)
MEASURED: U0: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (fx68k)
MEASURED: U0d: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (fx68k)
MEASURED: PR-9b: marker-loop head fetches between the first and the last marker: U1 682, D1 683, D0 683; largest pass 81.000 memory cycles (fx68k)
MEASURED: PR-9a: t($80) - t($40) in memory cycles: U1 14517.000, D1 14532.000, D0 14532.000; largest difference of D1 or D0 from U1 15.000, bound twice the largest pass 162.000 (fx68k)
MEASURED: P9: D1 has 0 BPLH accesses in a window of 6810.000 memory cycles (HTOTAL+1 = 114); D0 has 0 in 6810.000 (fx68k)
MEASURED: P8: latency from the fetch of the last word of the HHPOSW instruction to the first sample store, in memory cycles: HA 20.000 HB 20.000 HC 20.000 (fx68k)
MEASURED: P8: largest sample interval in memory cycles HA 14.000 HB 14.000 HC 14.000, smallest HA 13.000 (fx68k)
MEASURED: P8: first samples (bits 7-0) HA: 00 00 00 00 00 00 HB: 00 00 00 00 00 00 HC: 00 00 00 00 00 00 HD: 00 00 00 00 00 00
MEASURED: P8-E4 values (memory cycles), S_HV 5 J_HV 1.000: HA: sum d 0 elapsed 820.000 J 1.000 tolerance 11.875 HB: sum d 0 elapsed 820.000 J 1.000 tolerance 11.875 HC: sum d 0 elapsed 820.000 J 1.000 tolerance 11.875 (fx68k)
```

page-built copy, TG68K:

```text
MEASURED: HV: 58 pairs in 5 segments (S_HV), 1 same-V pair(s) with a decreasing H field left out, at most 1 for one V value; J_HV 1.001 memory cycles; conversion 7.990 clk_sys cycles per memory cycle (tg68k)
MEASURED: UR: 66497 accesses, markers 226 of 226 (tg68k)
MEASURED: P1: refresh accesses between the fetch of L+6 and the end marker: RA 129, RB 129; first RA address $00AAAA RB $015554 (tg68k)
MEASURED: PR-U5 population: 240 UHRES accesses in U1, U2, U3 and U4 (tg68k)
MEASURED: U1: BPLH 32 SPRH 32 stray 0; lines with one BPLH access 31 of 31, with two SPRH accesses 15 of 15; first BPLH access on line $051 (BPLHSTRT $050), first SPRH access on line $059 (SPRHSTRT $058) (tg68k)
MEASURED: U2: BPLH 32 SPRH 32 stray 0; lines with one BPLH access 31 of 31, with two SPRH accesses 15 of 15; first BPLH access on line $051 (BPLHSTRT $050), first SPRH access on line $059 (SPRHSTRT $058) (tg68k)
MEASURED: U3: BPLH 32 SPRH 32 stray 0; lines with one BPLH access 31 of 31, with two SPRH accesses 15 of 15; first BPLH access on line $051 (BPLHSTRT $050), first SPRH access on line $059 (SPRHSTRT $058) (tg68k)
MEASURED: U4: BPLH 16 SPRH 32 stray 0; lines with one BPLH access 15 of 15, with two SPRH accesses 15 of 15; first BPLH access on line $0F9 (BPLHSTRT $0F8), first SPRH access on line $101 (SPRHSTRT $100) (tg68k)
MEASURED: U0: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (tg68k)
MEASURED: U0d: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (tg68k)
MEASURED: P4 steps between the BPLH addresses of consecutive lines: U1: 18 expected, first steps 18 18 18 U2: -14 expected, first steps -14 -14 -14 (tg68k)
MEASURED: PR-9b: marker-loop head fetches between the first and the last marker: U1 839, D1 840, D0 841; largest pass 57.070 memory cycles (tg68k)
MEASURED: PR-9a: t($80) - t($40) in memory cycles: U1 14547.894, D1 14541.887, D0 14543.889; largest difference of D1 or D0 from U1 6.007, bound twice the largest pass 114.140 (tg68k)
MEASURED: P9: D1 has 60 BPLH accesses in a window of 6826.397 memory cycles (HTOTAL+1 = 114); D0 has 30 in 6828.399 (tg68k)
MEASURED: P8: latency from the fetch of the last word of the HHPOSW instruction to the first sample store, in memory cycles: HA 20.025 HB 20.025 HC 20.025 (tg68k)
MEASURED: P8: largest sample interval in memory cycles HA 15.018 HB 15.018 HC 15.018, smallest HA 14.017 (tg68k)
MEASURED: P8: first samples (bits 7-0) HA: 1E 2C 3A 48 56 64 HB: 4E 5C 6A 06 14 22 HC: 1E 2C 3A 48 04 12 HD: 1E 2C 3A 48 56 64
MEASURED: P8-E4 values (memory cycles), S_HV 5 J_HV 1.001: HA: sum d 886 elapsed 887.091 J 1.001 tolerance 11.919 HB: sum d 886 elapsed 887.091 J 1.001 tolerance 11.919 HC: sum d 886 elapsed 887.091 J 1.001 tolerance 11.919 (tg68k)
```

page-built copy, fx68k:

```text
MEASURED: HV: 58 pairs in 5 segments (S_HV), 1 same-V pair(s) with a decreasing H field left out, at most 1 for one V value; J_HV 1.000 memory cycles; conversion 8.000 clk_sys cycles per memory cycle (fx68k)
MEASURED: UR: 58429 accesses, markers 226 of 226 (fx68k)
MEASURED: P1: refresh accesses between the fetch of L+6 and the end marker: RA 129, RB 129; first RA address $00AAAA RB $015554 (fx68k)
MEASURED: PR-U5 population: 240 UHRES accesses in U1, U2, U3 and U4 (fx68k)
MEASURED: U1: BPLH 32 SPRH 32 stray 0; lines with one BPLH access 31 of 31, with two SPRH accesses 15 of 15; first BPLH access on line $051 (BPLHSTRT $050), first SPRH access on line $059 (SPRHSTRT $058) (fx68k)
MEASURED: U2: BPLH 32 SPRH 32 stray 0; lines with one BPLH access 31 of 31, with two SPRH accesses 15 of 15; first BPLH access on line $051 (BPLHSTRT $050), first SPRH access on line $059 (SPRHSTRT $058) (fx68k)
MEASURED: U3: BPLH 32 SPRH 32 stray 0; lines with one BPLH access 31 of 31, with two SPRH accesses 15 of 15; first BPLH access on line $051 (BPLHSTRT $050), first SPRH access on line $059 (SPRHSTRT $058) (fx68k)
MEASURED: U4: BPLH 16 SPRH 32 stray 0; lines with one BPLH access 15 of 15, with two SPRH accesses 15 of 15; first BPLH access on line $0F9 (BPLHSTRT $0F8), first SPRH access on line $101 (SPRHSTRT $100) (fx68k)
MEASURED: U0: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (fx68k)
MEASURED: U0d: BPLH 0 SPRH 0 stray 0; lines with one BPLH access 0 of 31, with two SPRH accesses 0 of 15; first BPLH access on line none (BPLHSTRT $050), first SPRH access on line none (SPRHSTRT $058) (fx68k)
MEASURED: P4 steps between the BPLH addresses of consecutive lines: U1: 18 expected, first steps 18 18 18 U2: -14 expected, first steps -14 -14 -14 (fx68k)
MEASURED: PR-9b: marker-loop head fetches between the first and the last marker: U1 680, D1 682, D0 683; largest pass 82.000 memory cycles (fx68k)
MEASURED: PR-9a: t($80) - t($40) in memory cycles: U1 14527.000, D1 14532.000, D0 14532.000; largest difference of D1 or D0 from U1 5.000, bound twice the largest pass 164.000 (fx68k)
MEASURED: P9: D1 has 60 BPLH accesses in a window of 6811.000 memory cycles (HTOTAL+1 = 114); D0 has 30 in 6810.000 (fx68k)
MEASURED: P8: latency from the fetch of the last word of the HHPOSW instruction to the first sample store, in memory cycles: HA 20.000 HB 20.000 HC 20.000 (fx68k)
MEASURED: P8: largest sample interval in memory cycles HA 14.000 HB 14.000 HC 14.000, smallest HA 13.000 (fx68k)
MEASURED: P8: first samples (bits 7-0) HA: 1E 2B 38 45 52 5F HB: 4E 5B 68 03 10 1D HC: 1E 2B 38 45 00 0D HD: 1E 2B 38 45 52 5F
MEASURED: P8-E4 values (memory cycles), S_HV 5 J_HV 1.000: HA: sum d 820 elapsed 820.000 J 1.000 tolerance 11.875 HB: sum d 820 elapsed 820.000 J 1.000 tolerance 11.875 HC: sum d 820 elapsed 820.000 J 1.000 tolerance 11.875 (fx68k)
```

### 7.5 Implementation record

On the shipped design the refresh addresses are the same whatever the Copper writes to 028, no run has an access of the UHRES kind, and every HHPOSR sample reads 0 (7.4), so the lines that need these registers print FAIL, or NOT GRADED where a precondition needs them. So that every red line has a green control through the same comparator, the test was built against private copies of the design, outside the repository (`make test-<core> NANOMIG=<copy>`), written from the sentences of the pages named in each line and not from any run of this package:

- **REFPTR** (p.27, "used as a Dynamic RAM refresh address generator"; p.6 `~`): a register of word addresses that a Copper MOVE with COPCON bit 1 set writes; the refresh accesses take their address from it and it advances by one word after each refresh access.
- **UHRES pointers** (p.16, p.22, p.15, p.27, p.28, p.12): BPLCON0 bit 7 together with DMACON bit 9 enables the chain. The chain starts at the reset of the beam counter in use (the normal one; with BEAMCON0 DUAL the alternate one: "starts the UHRES pointer chain with the reset of this counter", p.12): it is armed in the tick in which that counter's count falls back, and a write of HHPOSW does not arm it. It then takes the next three "free" memory cycles, those that no refresh, disk, audio, bit-plane or sprite channel uses: the first for the sprite pointer, the second for the bit-plane pointer, the third for the sprite pointer again. The vertical comparisons (BPLHSTRT <= V < BPLHSTOP, SPRHSTRT <= V < SPRHSTOP) are made when the chain is armed, with the vertical counter as it stands there, so the first access of a run lies on line start+1 in the numbering of R1 (the page's line start is open, R1). Each pointer is a pair of 5 and 15 bits (p.6 `+`). The bit-plane pointer advances after each access by 2 plus the sign-extended BPLHMOD, the sprite pointer by 2.
- **HHPOSW and HHPOSR** (p.24): a counter of memory cycles that HHPOSW sets, that counts up to HTOTAL and then starts again, and that HHPOSR returns in bits 7-0.

Against this copy `make test` prints, for every line of the table in 7.4, the verdict of the "copy" columns: every line prints PASS, on TG68K and on fx68k, including the lines that are FAIL or NOT GRADED on the shipped design (P1, PR-2a, P2, P1P2-JOINT, PR-U5, PR-U6, PR-U7, P3-E1, P3-E2, P3-E4, P3-E5, P3-AGG, P4, P5, P6-E1, P6-E2, P6-E3, P6-E4, P6-E5, P6-CTRL, P6-AGG, P7, PR-9-U5, P9-E1, P9-E2, P9-D0, P8-E1, P8-E3, P8-E4, P8-AGG).

Further copies change one sentence each (they differ from the first in the file `agnus_refresh.v` only, and `agnus_beamcounter.v` for the last):

- A copy whose chain is armed at the count 0 of the counter in use instead of at the fall of its count (the first free cycle comes one colour clock later): every line prints PASS, 66 passed and 0 failed on TG68K and on fx68k, as on the first copy. Its first BPLH access lies on line BPLHSTRT+1 and its first SPRH access on line SPRHSTRT+1, as on the first copy.
- A copy whose chain is armed at the count 3, after the vertical counter has stepped: every line prints PASS, 66 passed and 0 failed on TG68K and on fx68k, with the first BPLH and SPRH accesses on the lines BPLHSTRT and SPRHSTRT themselves. The same lines are green whether the first access lies on line start or on line start+1 (R1, "The line start is open").
- A copy in which the bit-plane pointer comes out on the first free cycle and the sprite pointer on the second and third (the pages give the order sprite, bit plane, sprite: p.27 "1st & 3rd", p.15 "2nd"): P6-E4 and P6-AGG print FAIL on TG68K and on fx68k, and every other line prints PASS (part `U`: 41 passed, 2 failed on each core).
- A copy without refresh accesses (part `REF`): PR-1c, R2-UR and R2-XCHK-REF print FAIL on TG68K and on fx68k, PR-2a prints FAIL, and P1, P1-RE, P1-RN, P2 and P1P2-JOINT print NOT GRADED (7 passed, 4 failed on each core).
- A copy in which bit 6 of the H field of VHPOSR is inverted (`agnus_beamcounter.v`), so that the H field falls back twice inside a line (part `HV`): HV-ANCHOR prints FAIL on TG68K and on fx68k through its clause on decreasing pairs alone (at least 16 pairs remain, each with an advance of at least 1; the measured line prints the decreasing pairs, three for one V value on fx68k) and FIXTURE-HV prints PASS (3 passed, 1 failed on each core).

### 7.6 Mutations

Each mutant changes the stimulus (a program, or which program a run uses) and leaves the comparator and the expected values alone. Every mutant program reaches its end marker, except those whose point is a missing end marker (PR-U1, PR-8c, PR-9-U1). `NMH_MUTATE=<selector>` with `NMH_ONLY` of the line's family runs a mutant. The columns give the verdict of the labelled line in the mutant run on the shipped design and on the page-built copy of 7.5, whose unchanged run prints PASS on every line (7.4). A mutant that needs a REFPTR, UHRES or HHPOSR effect cannot change a verdict on the shipped design, which shows none of them; there the column repeats the unchanged verdict. PR-1c and P6-E4 have no stimulus mutant (the pages give no stimulus that removes refresh or reorders the free cycles); their mutants are the private copies of 7.5, `IMPL-NOREF` and `IMPL-E4` below, and the clause of HV-ANCHOR on decreasing pairs, which no stimulus produces, has `IMPL-HBACK`. The contrast of P1+P2 with COPCON = $0000 for RA and RB (`P1P2-JOINT-A`) prints NOT GRADED (7.2 item 12). Several mutants turn other lines red or NOT GRADED as well, because the runs they change (U1, UR, D1, HB, RB) are shared by several lines; the `// MUTATION` records in the test header list, per selector, every other line that moves on the page-built copy.

| line | selector | what changes | shipped TG68K | shipped fx68k | copy TG68K | copy fx68k |
|---|---|---|---|---|---|---|
| FIXTURE-HV | `FIXTURE-HV` | run HV is run as prog_hv_nofix, which leaves out the store of the constant $BEEF | FAIL | FAIL | FAIL | FAIL |
| HV-ANCHOR | `HV-ANCHOR` | run HV samples VPOSR instead of VHPOSR (prog_hv_c); bits 7-0 of VPOSR do not count | FAIL | FAIL | FAIL | FAIL |
| FIXTURE-UR | `FIXTURE-UR` | run UR is run as prog_ur_nofix, which leaves out the store of the constant | FAIL | FAIL | FAIL | FAIL |
| R2-UR | `R2-UR` | run UR is run with DMACON = $8320 (prog_ur_dma): DMAEN, BPLEN and SPREN set, so sprite DMA reads inside Z | FAIL | FAIL | FAIL | FAIL |
| FIXTURE-REF | `FIXTURE-REF` | run RA is run as prog_ra_nofix, which leaves out the store of the constant | FAIL | FAIL | FAIL | FAIL |
| R2-XCHK-REF | `R2-XCHK-REF` | run UR is run with DMACON = $8320 (prog_ur_dma): its sprite-DMA reads are non-CPU accesses that the refresh output does not mark | FAIL | FAIL | FAIL | FAIL |
| PR-1a | `PR-1a` | RA and RB list the end WAIT ($FFFF, $FFFE) at L+0 (prog_ra_end, prog_rb_end), so the MOVE is never fetched | FAIL | FAIL | FAIL | FAIL |
| PR-1b | `PR-1b` | RB executes one NOP right after the vertical-blank wait (prog_rb_nop), so its trace differs in time from RA's | FAIL | FAIL | FAIL | FAIL |
| P1 | `P1` | RB writes the value of RA, $5555 (prog_rbm) | FAIL | FAIL | FAIL | FAIL |
| P1-RE | `P1-RE` | RE runs RB's program ($AAAA) instead of RA's | PASS | PASS | FAIL | FAIL |
| P1-RN | `P1-RN` | RNA and RNB run RA's and RB's programs (MOVE to 028, $5555 and $AAAA) instead of the MOVE to 1FE | PASS | PASS | FAIL | FAIL |
| PR-2a | `PR-2a` | RB writes the value of RA, $5555 (prog_rbm), so the P1 relation is false | FAIL | FAIL | FAIL | FAIL |
| PR-2b | `PR-2b` | RA0 and RB0 list the end WAIT at L+0 (prog_ra0_end, prog_rb0_end), so the MOVE is never fetched | FAIL | FAIL | FAIL | FAIL |
| P2 | `P2` | RA0 and RB0 run with COPCON = $0002 (prog_ra, prog_rb) | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P1P2-JOINT | `P1P2-JOINT-B` | RA0 and RB0 run with COPCON = $0002 (prog_ra, prog_rb) | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P1P2-JOINT | `P1P2-JOINT-A` | RA and RB run with COPCON = $0000 (prog_ra_c0, prog_rb_c0); the Copper fetches no data word of a MOVE it may not perform, so PR-1a is not met and the line prints NOT GRADED (not exercised, by a scope decision: the pages leave open whether the Copper fetches the data word of a MOVE it may not perform, P2 Open, so no graded case may depend on it) | NOT GRADED | NOT GRADED | NOT GRADED | NOT GRADED |
| FIXTURE-P8 | `FIXTURE-P8` | run HC is run as prog_hc_nofix, which leaves out the store of the constant | FAIL | FAIL | FAIL | FAIL |
| PR-8a | `PR-8a` | HB executes one NOP before the loop (prog_hb_nop) | FAIL | FAIL | FAIL | FAIL |
| PR-8b | `PR-8b` | HA waits about 100 memory cycles in each loop pass (prog_ha_slow) | FAIL | FAIL | FAIL | FAIL |
| PR-8c | `PR-8c` | HA leaves out the end-marker store (prog_ha_noend) | FAIL | FAIL | FAIL | FAIL |
| PR-8d | `PR-8d` | HA waits about 100 memory cycles between the HHPOSW write and its first read (prog_ha_wait) | FAIL | FAIL | FAIL | FAIL |
| P8-E1 | `P8-E1` | HB runs HA's program (HHPOSW = $0010 in both) | FAIL | FAIL | FAIL | FAIL |
| P8-E2 | `P8-E2` | HC writes HTOTAL = $00A1 (prog_hc_n) while the expectation stays N = $51 | PASS | PASS | FAIL | FAIL |
| P8-E3 | `P8-E3` | HC writes HHPOSW = $0030 (prog_hc_x) while the expectation stays X = $10 | FAIL | FAIL | FAIL | FAIL |
| P8-E4 | `P8-E4` | HC samples VPOSR instead of HHPOSR (prog_hc_v); bits 7-0 of VPOSR do not count | FAIL | FAIL | FAIL | FAIL |
| P8-HD | `P8-HD` | HD runs HB's program (HHPOSW = $0040) instead of HA's | PASS | PASS | FAIL | FAIL |
| P8-AGG | `P8-AGG` | HB runs HA's program (HHPOSW = $0010 in both) | FAIL | FAIL | FAIL | FAIL |
| FIXTURE-U | `FIXTURE-U` | run U1 is run as prog_u1_nofix, which leaves out the store of the constant | FAIL | FAIL | FAIL | FAIL |
| R2-XCHK-U | `R2-XCHK-U` | run UR is run with DMACON = $8320 (prog_ur_dma) | FAIL | FAIL | FAIL | FAIL |
| PR-U1 | `PR-U1` | U1 leaves out the end-marker store (prog_u1_noend) | FAIL | FAIL | FAIL | FAIL |
| PR-U2 | `PR-U2` | U1's marker loop steps V by 2 (prog_u1_skip), so the odd lines have no marker | FAIL | FAIL | FAIL | FAIL |
| PR-U3 | `PR-U3` | U1 waits for line $60 before its setup (prog_u1_late) | FAIL | FAIL | FAIL | FAIL |
| PR-U4 | `PR-U4` | U0 runs U1's program (UHRES bit set) | PASS | PASS | FAIL | FAIL |
| PR-U5 | `PR-U5` | U1's marker loop is gated at H in [$B0, $E0) instead of [$60, $90) (prog_u1_gateB0): the markers are taken at the end of line V and stored at the start of the next line; the anchor of R1 uses the loop's gate | NOT GRADED | NOT GRADED | FAIL | FAIL |
| PR-U6 | `P3-E1` | U1 runs the program of U0 (BPLCON0 UHRES bit clear), so it has no BPLH and no SPRH access | FAIL | FAIL | FAIL | FAIL |
| PR-U7 | `P7` | U1 writes SPRHSTRT = $0060 (prog_u1_ss60), so lines $59 to $5F have no SPRH access | FAIL | FAIL | FAIL | FAIL |
| P3-E1 | `P3-E1` | U1 runs U0's program (BPLCON0 UHRES bit clear) | FAIL | FAIL | FAIL | FAIL |
| P3-E2 | `P3-E2` | U1 writes BPLHPTL = $AAEB (prog_u1_bpl40): its first BPLH address is $15AAEA, 0x40 bytes beyond the expected $15AAAA and still inside the run's BPLH range | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P3-E4 | `P3-E4` | U3 writes BPLHPTL = $AAEB (prog_u3_bpl40), so its BPLH addresses differ from U1's | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P3-E5 | `P3-E5` | U1 writes SPRHPTH = $0016 (prog_u1_sph16), so its SPRH accesses lie outside the SPRH range of the run | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P3-AGG | `P3-AGG` | U1 runs U0's program (BPLCON0 UHRES bit clear) | FAIL | FAIL | FAIL | FAIL |
| P3-U0 | `P3-U0` | U0 runs U1's program (UHRES bit set) | PASS | PASS | FAIL | FAIL |
| P3-U0d | `P3-U0d` | U0d runs U1's program (DMAEN set) | PASS | PASS | FAIL | FAIL |
| P4 | `P4` | U2 writes BPLHMOD = $0000 (prog_u2_m0) while the expectation stays -14 | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P5 | `P5` | U1 writes BPLHSTRT = $0058 (prog_u1_s58) while the expectation stays start $50 | FAIL | FAIL | FAIL | FAIL |
| P6-E1 | `P6-E1` | U1 runs U0's program (BPLCON0 UHRES bit clear) | FAIL | FAIL | FAIL | FAIL |
| P6-E2 | `P6-E2` | U1 writes SPRHPTL = $24A9 (prog_u1_spl40): its first SPRH address is $1324A8, 0x40 bytes beyond the expected $132468 | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P6-E3 | `P6-E3` | U1 writes SPRHPTH, SPRHPTL = $0013, $2469 again right after the marker of line $5F (prog_u1_rw), so the SPRH addresses fall back to the start | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P6-E5 | `P6-E5` | U3 writes SPRHPTL = $24A9 (prog_u3_spl40), so its SPRH addresses differ from U1's | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P6-CTRL | `P6-CTRL` | U1 writes SPRHSTRT = $0050 (prog_u1_sstrt50), so lines $51 to $56 are inside the SPRH range | FAIL | FAIL | FAIL | FAIL |
| P6-U0 | `P6-U0` | U0 runs U1's program (UHRES bit set) | PASS | PASS | FAIL | FAIL |
| P6-AGG | `P6-AGG` | U1 runs U0's program (BPLCON0 UHRES bit clear) | FAIL | FAIL | FAIL | FAIL |
| P7 | `P7` | U1 writes SPRHSTRT = $0060 (prog_u1_ss60) while the expectation stays start $58 | FAIL | FAIL | FAIL | FAIL |
| FIXTURE-D | `FIXTURE-D` | run D1 is run as prog_d1_nofix, which leaves out the store of the constant | FAIL | FAIL | FAIL | FAIL |
| R2-XCHK-D | `R2-XCHK-D` | run UR is run with DMACON = $8320 (prog_ur_dma) | FAIL | FAIL | FAIL | FAIL |
| PR-9-U1 | `PR-9-U1` | D1 leaves out the end-marker store (prog_d1_noend) | FAIL | FAIL | FAIL | FAIL |
| PR-9-U2 | `PR-9-U2` | D1's marker loop steps V by 2 (prog_d1_skip) | FAIL | FAIL | FAIL | FAIL |
| PR-9-U3 | `PR-9-U3` | D1 waits for line $60 before its setup (prog_d1_late) | FAIL | FAIL | FAIL | FAIL |
| PR-9-U4 | `PR-9-U4` | U0 runs U1's program (UHRES bit set) | PASS | PASS | FAIL | FAIL |
| PR-9-U5 | `PR-9-U5` | D1 runs with the UHRES bit clear (prog_d1_nobit), so it has no BPLH access | FAIL | FAIL | FAIL | FAIL |
| PR-9b | `PR-9b` | the marker loop of D1 waits about 70 memory cycles after the gate test (prog_d1_pad) | FAIL | FAIL | FAIL | FAIL |
| PR-9a | `PR-9a` | D1 runs with BEAMCON0 = $28E0 (VARBEAMEN set as well) after VTOTAL = $0100 (prog_d1_var) | FAIL | FAIL | FAIL | FAIL |
| P9-E1 | `P9-E1` | D1 runs D0's program (DUAL = 0) | FAIL | FAIL | FAIL | FAIL |
| P9-E2 | `P9-E2` | D1 writes BPLHMOD = $0010 (prog_d1_m16) | NOT GRADED | NOT GRADED | FAIL | FAIL |
| P9-D0 | `P9-D0` | D0 runs D1's program (DUAL = 1) | FAIL | FAIL | FAIL | FAIL |
| HV-ANCHOR | `IMPL-HBACK` | private copy in which bit 6 of the H field of VHPOSR is inverted, so that the H field falls back twice inside a line (part HV) | - | - | FAIL | FAIL |
| PR-1c | `IMPL-NOREF` | private copy without refresh accesses (part REF) | - | - | FAIL | FAIL |
| P6-E4 | `IMPL-E4` | private copy in which the bit-plane pointer comes out on the first free cycle (part U) | - | - | FAIL | FAIL |
