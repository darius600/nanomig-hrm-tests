# Specification: integration package `aa-s7-rest` (UHRES registers, DUAL-mode beam counter, REFPTR, horizontal-sync strobes)

This specification was written from the package's claims (`claims-spec-stage.jsonl`) and the pages of Commodore's *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, by an author who never saw the implementation or any test. Page numbers "p.N" are the corpus pages of that specification (`corpus\pNN.txt`), and quotations are given as printed. Pages of the *Amiga Hardware Reference Manual*, 3rd edition, are cited as "HRM p.N" only where the AA specification builds on the original chip set; no expected value comes from them.

The package holds 107 claims about 15 registers. Nine propositions (P1-P9) are specified. Thirty claims (the three horizontal-sync strobes) cannot be turned into a proposition; they are listed first.

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
- c5 (`~`, "Address not writable by the Coprocessor unless COPCON bit 1 is set true", p.6): an effective write can be told from an ineffective one only by its effect, and no page gives an effect for these three addresses. For REFPTR the write has an observable effect, so the same legend entry becomes proposition P2.

No claim in this package needs a disk image.

### 1.2 Parts of used claims that are not graded

- opus:AA-refptr.c4, sol:AA-refptr.c4 (`&`, "Register used by DMA channel only"). P1 uses them for "the refresh DMA channel takes its addresses from REFPTR". The quantifier "only", meaning that no processor uses the register, is not graded: no stimulus makes it observable.
- The chip-set sentence in every page-10 claim (the `h` mark, which the p.6 and p.10 legends define differently). The claims themselves decline to name a chip set, and no proposition depends on it: the tests run with AA selected, and both legends place these registers in the "hires" (ECS) chips that AA extends (section 6, item 1).
- The access letters `W` (c2 of every write register) are exercised by the writes each proposition makes. What a read of a write-only address returns is not stated on any page, so no check reads one.

---

## 2. System, units and conventions

- **Configuration.** AA chip set selected (`Config.chipset_config = 0x10`), PAL, 2 MB Chip memory. Every run is made once on each CPU core in its default 68000 mode. Any comparison between two runs uses the same core and the same configuration.
- **Register names** are the offsets from 0xDFF000 given in the address list (pp.6-10). The program writes them as 16-bit words.
- **Addresses** below are byte addresses. The Chip-memory port reports word addresses (byte address / 2).
- **Time unit.** One memory cycle (mc) is 280 ns: "memory cycles are 280 ns" (p.21). The same unit counts HTOTAL and the DUAL-mode counter: "Horiz line has this many + 1 280ns increments" (p.24), "Highest color clock count in horiz line" (p.24). The builder converts system clock cycles to mc with the harness's fixed ratio. Check the ratio on every run that does not set VARBEAMEN: the refresh accesses repeat from line to line with the same whole-number period in mc.
- **Unused bits** are written as 0: "x= don't care; but drive to 0 for upward compatibility !" (p.16); "(x's should be driven to 0 for upward compatibility)" (p.24); the x bits of SPRHSTRT and SPRHSTOP (p.28); bits 14-11 "unused" of BPLHSTOP (p.15). Bit 15 of BPLHSTOP (BPLHWRM) and of SPRHSTOP (SPRHWRM) is always written 0. These bits "Swap the polarity of ARW*" (pp.15, 28); no claim in this package covers them, and no check grades whether a UHRES access is a read or a write.
- **Memory layout.** The program, its data, the marker buffer MK, the Copper list L, the end-marker word END and a zero block Z of 4096 bytes of 0x0000 all lie below byte address 0x080000. The UHRES pointer targets in section 4 lie above 0x080000, and nothing else uses them.
- **Common prologue**, the same in every run:
  1. DMACON ← 0x7FFF, INTENA ← 0x7FFF, INTREQ ← 0x7FFF (clear all enables and requests; bit 15 SET/CLR = 0, p.22, p.25). BPLCON0 ← 0x0000. COPCON ← 0x0000.
  2. Prepare the run's data (zero block, Copper list, buffers).
  3. Wait for the start of the vertical blank: INTREQ ← 0x0020, then poll INTREQR until bit 5 is 1 ("05 VERTB 3 Start of Vertical blank", p.25). The run's setup writes follow immediately. The frame that begins here is **the observed frame**. Pointer registers are therefore loaded in the vertical blank: "PTL,PTH=20 bit Pointer that addresses DMA data. Must be reloaded by a processor before use (Vertical blank for Bit Plane and Sprite pointers ...)" (p.6).
- **Common epilogue:** DMACON ← 0x7FFF, BPLCON0 ← 0x0000, then the CPU writes the word 0x5A5A to END (the **end marker**) and stops.

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

Why the rule is sound: each marker is taken in the middle of its line, at H ≥ 0x60 of about 0xE3. The UHRES pointers come out "on the 2nd 'free' cycle after the start of each horiz. line" (p.15) and "in the 1st & 3rd 'free' cycles (see BPLHPTH,L) after horiz line start" (p.27). An access early in line V therefore lies between the markers of V-1 and V, far from both. Precondition PR-U5 (section 4) checks this distance.

### R2. Access classification

- **CPU access:** anything the harness reports as a CPU access (instruction fetches, marker and sample stores).
- **DMA accesses** are sorted by source:
  - Copper: a fetch inside the Copper list L.
  - Sprite or bitplane DMA: an address inside Z.
  - Refresh: the harness's refresh attribution. If the harness names no DMA source, refresh accesses are the DMA accesses that are neither Copper nor Z. In the UHRES runs these are identified in the UHRES-off run U0 and matched by time: refresh slots do not depend on the program.
  - UHRES: every remaining DMA access.
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

Expected first addresses: in U1, U3 and U4, P = 0x15AAAA and S = 0x132468; in U2, P = 0x0A5554 and S = 0x0CDB96. In U2 every one of the 20 pointer bits of both pointers is the complement of U1 (0x15AAAA XOR 0x0A5554 = 0x132468 XOR 0x0CDB96 = 0x1FFFFE). BPLHPTL and SPRHPTL are written with bit 0 set to exercise "Low word uses DB1-DB15" (p.6). U4's ranges cross line 0x100, so they need V8 of the 11-bit comparison.

**Preconditions common to P3-P7** (each gets a contrasting case):

- **PR-U1.** The end marker is present. *Contrast:* omit the epilogue's store; the check reports it missing.
- **PR-U2.** The R1 anchor checks pass. *Contrast:* make the loop skip every second V; the "marker for every V" check fails.
- **PR-U3.** The line in the setup-done record is below Vlo (setup finished before the window). *Contrast:* in a copy of U1, wait for line 0x060 before the setup; the check fails.
- **PR-U4.** Run U0 has no UHRES and no stray access at all (R2 accounts for every access when UHRES is off). *Contrast:* U1 itself has UHRES accesses; the same check, applied to U1, reports them.
- **PR-U5.** Every UHRES access lies at least 0x20 mc from every marker time, so the line assignment of R1 is unambiguous. *Contrast:* gate the marker loop at H in [0x00, 0x08) instead of [0x60, 0x90). The markers then sit next to the line start, where the page places the UHRES accesses, and the check must report the collision.

---

## 5. Propositions

### P1. REFPTR (028) is the refresh address generator: a Copper write to it, allowed by COPCON bit 1, changes the addresses of the refresh accesses that follow

- **Claims:** opus:AA-refptr.c1, opus:AA-refptr.c2, opus:AA-refptr.c4 (see 1.2), sol:AA-refptr.c1, sol:AA-refptr.c2, sol:AA-refptr.c4. Page 6: `REFPTR    & ~028  W    A        Refresh pointer`.
- **Qualifying pages:**
  - p.27: "REFPTR 028 W A Refresh pointer. This register is used as a Dynamic RAM refresh address generator. It is writeable for test purposes only, and should never be written by the microprocessor."
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
- PR-1b. Up to the fetch of L+6, the RA and RB traces agree in time, address and direction for every access. Their data agree except for the CPU's write of the value word into L+6. After that fetch, every non-refresh access still agrees in time and address. *Contrast:* in RB, add one CPU NOP right after the vertical-blank wait; the traces then differ in time and the check reports it.
- PR-1c. Every line of the interval has refresh accesses. The pages give no contrast that removes refresh; this is a presence check only.

**Contrasting cases.**
- RE against RA (same value): the refresh address sequences are identical in every position.
- RNA against RNB (MOVE to 1FE, "NO-OP(NULL)", p.10): identical in every position. Only a MOVE to 028 moves the refresh addresses, which is the address claim.
- Additional contrast, predicted by p.19/p.20 for chips before the hires chips: "(on old chips- access to only RGA>3E if CDANG=1)". Repeat RA and RB with OCS behaviour selected: the refresh sequences are then identical, because 028 is below 03E. This rests on reading "old chips" as the harness's OCS mode. COPCON carries `h` on p.19, so the change came with the hires chips.

### P2. With COPCON bit 1 clear, the Copper cannot write REFPTR: its MOVE to 028 leaves the refresh addresses unchanged

- **Claims:** opus:AA-refptr.c5, sol:AA-refptr.c5. Page 6: "~=Address not writable by the Coprocessor unless COPCON bit 1 is set true." p.19 agrees: "(if 0, access to RGA>7E)".
- **Stimulus.** As P1, with COPCON ← 0x0000. Run RA0 uses value 0x5555 and run RB0 uses 0xAAAA.
- **Observation.** As P1: the refresh accesses from the Copper's fetch of L+6 to the end marker.
- **Expected relation.** The refresh address sequences of RA0 and RB0 are identical in every position. In fact the two port traces agree in every access (time, address, direction, data), except for the CPU's write of the value word into L+6 and the Copper's fetch of it.
- **Quantifier.** "not writable ... unless" covers every Copper write. This check covers one write of each of two values at one beam position. It does not establish the statement for every timing.
- **Open.** No AA page says what the Copper does after a MOVE it may not perform. The list ends right after the MOVE, so nothing depends on that.
- **Preconditions.**
  - PR-2a. P1 is green (RA and RB differ). Otherwise an implementation in which REFPTR has no effect at all would make P2 green without testing the protection; P2 is then reported as not gradable. *Contrast:* P1's runs RE or RNA/RNB make the P1 relation false, and P2 must then refuse a verdict.
  - PR-2b. PR-1a holds for RA0 and RB0. *Contrast:* as for PR-1a.
- **Contrasting case.** P1 (COPCON bit 1 set): the sequences differ.

### P3. BPLHPTH (1EC) and BPLHPTL (1EE) hold the UHRES bit-plane pointer. With UHRES enabled, the pointer's address is accessed exactly once on every line of the UHRES bit-plane range, starting at the written address

- **Claims:** opus:AA-bplhpth.c1, c2, c4, c6; opus:AA-bplhptl.c1, c2, c4, c6; sol:AA-bplhpth.c1, c2, c4, c6; sol:AA-bplhptl.c1, c2, c4, c6. Page 10 rows: `BPLHPTH   +h  1EC  W    A       VRam (UHRES) bit plane pointer (High 5 bits)` and `BPLHPTL   +h  1EE  W    A       VRam (UHRES) bit plane pointer (Low 15 bits)`. p.6 legend: "+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4." (The p.10 legend does not define `+`.)
- **Qualifying pages.** p.15: "When UHRES is enabled, this pointer comes out on the 2nd 'free' cycle after the start of each horiz. line. Its modulo is added every time it comes out." p.27 (SPRHDAT): "This identifies the cycle when this pointer address is on the bus accessing the memory." The alphabetical entries on p.15 agree with the p.10 rows in address and access letter.
- **Stimulus.** Runs U1, U2, U3, U0 and U0d (section 4).
- **Observation.** BPLH and stray accesses (R2), per line (R1), from the setup-done record to t(Vhi).
- **Expected.**
  - E3.1 ("each horiz. line"). Every line V with 0x050 ≤ V ≤ 0x06F has exactly one BPLH access. Line 0x070 is graded under P5. BEAMCON0 is not written, so DUAL = 0, and the chain starts at the normal counter's line start (p.12).
  - E3.2. The BPLH access on line 0x050 is at P: 0x15AAAA in U1 and U3, 0x0A5554 in U2. The expected address is (BPLHPTH bits 4-0) << 16 | (BPLHPTL & 0xFFFE): high word DB0-DB4 → A20-A16, low word DB1-DB15 → A15-A1 (p.6).
  - E3.3. U1 and U2 together set and clear every one of the 20 address bits, and each run must hit its own P. This exercises "(High 5 bits)" (the page's "old chips- 18 bits", p.6, would drop A20-A19).
  - E3.4. U3 (BPLHPTH written 0xFFF5) produces the same BPLH access times and addresses as U1, because the high word uses only DB0-DB4. This check is weak: bits above A20 do not exist in a 2 MB Chip memory, so it only catches DB5-DB15 leaking into A20-A1.
  - E3.5. No run has a stray access.
- **Preconditions.** PR-U1 to PR-U5.
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
- **Observation.** The BPLH access of each line, lines 0x050-0x06F.
- **Expected.** For every pair of consecutive lines V, V+1 in 0x050..0x06F, the address on V+1 minus the address on V is 2 + sign-extended BPLHMOD: +18 in U1 and -14 in U2. Graded on every pair.
- **Preconditions.** E3.1 holds (one BPLH access on every line of the range), and PR-U1 to PR-U5 hold.
- **Contrasting case.** U2 against U1: the step changes from +18 to -14 and the addresses run downward from 0x0A5554.

### P5. BPLHSTRT (1D4) and BPLHSTOP (1D6) bound the lines on which the UHRES bit-plane pointer is fetched

- **Claims:** opus:AA-bplhstrt.c1, c2, c5; opus:AA-bplhstop.c1, c2, c5; sol:AA-bplhstrt.c1, c2; sol:AA-bplhstop.c1, c2. Page 10: `BPLHSTRT   h  1D4  W    A       UHRES bit plane vertical start` and `BPLHSTOP   h  1D6  W    A       UHRES bit plane vertical stop`.
- **Qualifying pages.** p.15: "BPLHSTRT ... This controls the line when the data fetch starts for the BPLHPTH,L pointers. V10-V0 on DB10-0." p.15: "BPLHSTOP ... This controls the line when the data fetch stops for the BPLHPTH,L pointers. ... 10-0 V10-V0". The comparison is with the vertical beam counter, the line number of R1.
- **Stimulus.** U1 (start 0x050, stop 0x070) and U4 (start 0x0F8, stop 0x108).
- **Observation.** BPLH accesses per line, from the setup-done record to t(Vhi).
- **Expected**, for each run with its own start and stop:
  - E5.1. No BPLH access between the setup-done record and t(start-1), that is, on no line before start. This covers every line of the observed frame from the setup up to start-1.
  - E5.2. Line "start" has a BPLH access ("the line when the data fetch starts"), and it is the first BPLH access of the run.
  - E5.3. Every line from start to stop-1 has one (E3.1). In U4 this includes lines 0x100-0x107, which need V8.
  - E5.4. No BPLH access after t(stop), that is, on no line from stop+1 to Vhi.
- **Open, not graded.** Line "stop" itself. "the line when the data fetch stops" does not say whether that line still has a fetch.
- **Preconditions.** PR-U1 to PR-U5, and E3.1 holds for the run.
- **Contrasting case.** U4 against U1: there are no BPLH accesses on lines 0x050-0x06F (inside E5.1 for U4), and they appear on 0x0F8-0x107.

### P6. SPRHPTH (1E8) and SPRHPTL (1EA) hold the UHRES sprite pointer. It comes out twice on each line of its range, before and after the bit-plane pointer, starting at the written address and moving to higher addresses on each following line

- **Claims:** opus:AA-sprhpth.c1, c2, c4, c6; opus:AA-sprhptl.c1, c2, c4, c6; sol:AA-sprhpth.c1, c2, c4, c6; sol:AA-sprhptl.c1, c2, c4, c6. Page 10: `SPRHPTH   +h  1E8  W    A       UHRES sprite pointer (High 5 bits)` and `SPRHPTL   +h  1EA  W    A       UHRES sprite pointer (Low 15 bits)`. p.6 legend `+` as in P3.
- **Qualifying pages.**
  - p.27: "This pointer is activated in the 1st & 3rd 'free' cycles (see BPLHPTH,L) after horiz line start. It increments for the next line."
  - p.15: "The SHRHDAT gets the first and third free cycles." SHRHDAT names no register on any page; it is read as SPRHDAT, the UHRES sprite identifier (pp.7, 27).
  - p.27: SPRHDAT "identifies the cycle when this pointer address is on the bus accessing the memory."
- **Stimulus.** U1, U2, U3 and U0.
- **Observation.** SPRH, BPLH and stray accesses per line, from the setup-done record to t(Vhi).
- **Expected.**
  - E6.1 ("1st & 3rd", two activations per line). Every line strictly inside the SPRH range, 0x059..0x067 in U1 to U3, has exactly two SPRH accesses.
  - E6.2. The first SPRH access of the run is at S: 0x132468 in U1 and U3, 0x0CDB96 in U2. The expected address is (SPRHPTH bits 4-0) << 16 | (SPRHPTL & 0xFFFE).
  - E6.3 ("increments for the next line"). For every pair of consecutive lines V, V+1 that both have SPRH accesses, every SPRH address on V+1 is greater than every SPRH address on V.
  - E6.4 (1st free / 2nd free / 3rd free). On every line with both SPRH and BPLH accesses (0x059..0x067), the time order is SPRH, BPLH, SPRH.
  - E6.5. U3 (SPRHPTH = 0xFFF3) gives the same SPRH addresses and times as U1, with the same limited sensitivity as E3.4.
- **Open, not graded.** The size of the increment. Whether the two accesses of one line use the same address or consecutive addresses. Which memory cycle within the line.
- **Preconditions.** PR-U1 to PR-U5.
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
  3. BEAMCON0 ← 0x2860: LPENDIS (bit 13), LOLDIS (bit 11), DUAL (bit 6), PAL (bit 5). All other bits are 0, VARBEAMEN among them. Use 0x2840 if the harness runs NTSC.
  4. HHPOSW ← X.
  5. At once, 64 times: read HHPOSR and store the word to BUF + 2i.
  6. Epilogue.

  | run | N | X |
  |---|---|---|
  | HA | 0x0071 | 0x0010 |
  | HB | 0x0071 | 0x0040 |
  | HC | 0x0051 | 0x0010 (contrast) |
  | HD | 0x0071 | 0x0010 (contrast: the same as HA) |

- **Observation.** The 64 sample stores: their data r[i] (bits 7-0 graded; bits 15-8 are not stated on any page) and their port times T[i], converted to mc. Graded interval: the first to the last sample store.
- **Expected.** The counter's modulus is N+1 (0x72 = 114 for HA and HB).
  - E8.1 (write sets the counter; R3 pair HA/HB). For every i, (rB[i] - rA[i]) mod (N+1) = (0x40 - 0x10) mod (N+1) = 0x30.
  - E8.2 (HTOTAL is the highest count). Every sample in HA, HB and HC satisfies r[i] ≤ N of its run.
  - E8.3 (the write is the starting point). In each run, (r[0] - X) mod (N+1) ≤ max_i(T[i+1] - T[i]). The first read follows the write by less than one loop pass.
  - E8.4 (it counts, one count per 280 ns). Let d[i] = (r[i+1] - r[i]) mod (N+1). Every d[i] ≥ 1. Also |Σ d[i] - (T[63] - T[0])| ≤ J, where J = max_i(T[i+1] - T[i]) - min_i(T[i+1] - T[i]). J is the run's own jitter of the read-to-store delay; the page gives no tolerance, and J bounds how far that delay can differ between the first and the last sample.
- **Preconditions.**
  - PR-8a. HA and HB agree in time, address and direction for every port access. Their data differ only in the fetch of the immediate operand X and in the sample stores. Then the counter is read at the same moments in both runs, as R3 requires. *Contrast:* add one NOP before the loop in HB only; PR-8a fails.
  - PR-8b. The largest sample interval, max(T[i+1] - T[i]), is below (N+1)/2 mc, so each d[i] is unambiguous. *Contrast:* insert a delay of about 100 mc into the loop; PR-8b fails.
  - PR-8c. The end marker is present and 64 sample stores are seen.
- **Contrasting cases.** HD against HA (same X): rD[i] = rA[i] for every i, a predicted difference of 0. HC (N = 0x51): every sample is ≤ 0x51, and E8.1 to E8.4 hold with modulus 0x52.
- **Open, not graded.**
  - Bits 15-8 of HHPOSR.
  - What HHPOSR returns and whether the counter runs when DUAL = 0; the page names it the DUAL-mode counter only.
  - The name says "hires H beam counter", but its bits are HTOTAL's, which counts 280 ns steps. E8.1 to E8.3 hold for any count rate; only E8.4 rests on the 280 ns step (section 6, item 10).

### P9. In DUAL mode the counter of P8 restarts the UHRES pointer chain at each of its line starts, so the UHRES bit-plane pointer comes out once every HTOTAL+1 memory cycles: more than once per ordinary line

- **Claims:** opus:AA-hhposw.c5, opus:AA-hhposr.c5, sol:AA-hhposw.c5, sol:AA-hhposr.c5 (the function "DUAL mode hires H beam counter"). Together with the BPLH pointer claims of P3.
- **Qualifying pages.**
  - p.24: "This is the secondary beam counter for the faster mode, triggering the UHRES pointers".
  - p.12: "DUAL= Run the horizontal comparators with the alternate horizontal beam counter, and starts the UHRES pointer chain with the reset of this counter rather than the normal one. This allows the UHRES pointers to come out more than once in a horizontal line, assuming there is some memory bandwidth left (It doesn't work in 640*400*4 interlace mode)".
  - p.24 HTOTAL as in P8.
- **Stimulus.** Run D1: as U1, except BPLHMOD = 0x0000 and SPRHSTRT = 0x00F0, SPRHSTOP = 0x00F4 (outside the window). Between setup steps 3 and 4 the program writes HTOTAL ← 0x0071 and BEAMCON0 ← 0x2860 (as P8). N+1 = 114 is even, so the alternate line keeps the parity of the memory cycles. Only refresh, null-sprite DMA and the CPU compete, so there is "memory bandwidth left". Run D0 is identical except BEAMCON0 ← 0x2820 (DUAL = 0).
- **Observation.** BPLH accesses strictly between t(0x051) and t(0x06F). Every one of them belongs to lines 0x052-0x06F, which lie inside the range [0x050, 0x070). Let W = t(0x06F) - t(0x051) in mc, and K the number of BPLH accesses in that interval.
- **Expected.**
  - E9.1. In D1, ⌊W/114⌋ - 1 ≤ K ≤ ⌈W/114⌉ + 1. Why: the chain restarts every 114 mc and each restart gives one access. The unknown phase of the restarts, and the few cycles between a restart and its access, can shift at most one access across each end of the interval. With PAL lines this puts K near twice the number of lines.
  - E9.2. In D1, each BPLH address is the previous one + 2, for every consecutive pair. BPLHMOD is 0, so both p.15 sentences ("every line, and then another 2 is added" and "added every time it comes out") give the same step.
- **Preconditions.**
  - PR-U1, PR-U2, PR-U3 and PR-U4 as for the UHRES runs. PR-U5 does not apply: the accesses now occur throughout the line, and no per-line assignment is made.
  - PR-9a. The ordinary line length is unchanged by DUAL. In D1 and D0, each interval t(V) - t(V-1) differs from U1's by no more than U1's own spread of that interval. Nothing on p.12 or p.24 lets DUAL alone change the main counter. *Contrast:* D1 with BEAMCON0 ← 0x28E0 (VARBEAMEN added, "allowing different beam counter total values ... on the main horiz counter", p.12) must fail PR-9a.
- **Contrasting case.** D0 (DUAL = 0): K = 30, one per line 0x052..0x06F as in E3.1. This lies far outside the D1 range of about 60.
- **Open, not graded.**
  - The exact offset of each access after a restart, beyond the ±1 per end of E9.1.
  - In DUAL mode with a non-zero BPLHMOD, the step between two accesses of one ordinary line. The two p.15 sentences differ there (section 6, item 4).

---

## 6. What the pages leave open or state inconsistently

1. **Chip-set marks.** The p.6 legend reads "h=new for HiRes chip set" and "p=new for IAA chip set". The p.10 legend reads "P= new register in Pandora Chip Set", "p= stuff added or changedin hires chips", "H= new register in hires chips", "h= stuff added or changed in hires chips". So p.10 gives lowercase `p` and lowercase `h` the same meaning, and gives `p` a different meaning than p.6 does. BPLHSTOP is marked `h` in the address list (p.10) and `p` in the alphabetical list (p.15). The other UHRES registers carry `H` there (pp.15, 24, 27, 28). Every reading places these registers in the hires chips that AA extends, so no expected value here depends on the choice.
2. **Undefined legend symbols.** The p.10 legend does not define `+`, `&` or `~`. Only p.6 does.
3. **"SHRHDAT"** (p.15) is not a register name anywhere. It is read as SPRHDAT; p.27 independently gives the UHRES sprite pointer the 1st and 3rd free cycles.
4. **Modulo in DUAL mode.** p.15 says the modulo is added "every line, and then another 2 is added" (BPLHMOD) and also "Its modulo is added every time it comes out" (BPLHPTH,L). With one access per line the two agree. In DUAL mode they may not, so P9 grades the step only with BPLHMOD = 0.
5. **BPLHSTOP.** "the line when the data fetch stops" leaves open whether the stop line itself is fetched.
6. **SPRHSTRT/SPRHSTOP** are called "vertical display start/stop" (p.28) without saying on which line the fetch happens. **SPRHPTH,L** "increments for the next line" (p.27) with no step and no statement about the two accesses within one line.
7. **UHRES needs DMA enables.** "(also needs bits in DMACON)" (p.16) does not name the bits. p.22 names only DMAEN ("also UHRES dma").
8. **"Free" cycle** (p.15) is defined only by priority. No AA page gives the slot positions of the "fixed stuff" or of the line start, so the exact memory cycle of a UHRES access cannot be predicted.
9. **REFPTR.** p.27 gives no bit layout and no rule for how the written value becomes refresh addresses. It calls the register "writeable for test purposes only" and says it "should never be written by the microprocessor". P1 therefore writes it with the Copper and grades only a difference between two written values.
10. **HHPOSR/HHPOSW** are called a "hires H beam counter" (p.10, p.24) but refer to HTOTAL for their bits, and HTOTAL counts 280 ns increments. Bits 15-8 of HHPOSR are not stated. The HTOTAL row prints bit "10" where bit 01 is meant, and the HSSTRT row repeats "for HSYNC stop" (p.24); neither affects this package.
11. **Copper access limits.** The AA pages agree with each other: p.6 `~` on 000-07E, and p.19 "(if 0, access to RGA>7E)". Two HRM statements differ from them, and neither is used:
    - HRM p.21, checked against the print, says the Copper can store into "Any register whose address is $20 or above", which would make 028 writable without CDANG.
    - HRM p.310 (ECS) says that with CDANG clear the Copper can reach $03E-$07E. This conflicts with the AA page for 040-07E, but not for 028 or 038-03C.
12. **Alphabetical entries.** Every claim says its register's alphabetical entry lies outside its slice. In fact the entries are on pp.15, 24, 27, 28 and 29, and they agree with the address-list rows in address and access letter for all 15 registers. The function wording differs only slightly ("vertical display start/stop" for the sprite pair; "UHRES (VRAM) bit plane pntr" for the bit-plane pair).
