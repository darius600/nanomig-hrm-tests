# Specification: integration package aa-s7 (Copper-protected, DMA-only and address-pair registers of the AA register list)

This document specifies one integration test package for an FPGA re-implementation of the Amiga chip set, checked against Commodore's *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91. It was written only from the package's claims (`claims-spec-stage.jsonl`) and the pages of that specification. Where the AA specification builds on the original chip set, it also uses a few pages of the *Amiga Hardware Reference Manual*, 3rd edition. The author never saw the implementation or any test. Page numbers "p.N" are the AA specification's pages (corpus `pNN.txt`, one file per page). "HRM p.N" is a printed page of the Hardware Reference Manual. Expected values come only from the AA specification. HRM pages are used only for procedures and encodings the AA specification takes over without restating them, and each such use is named.

---

## 0. Claims not turned into a proposition

The package has 160 claims. 80 of them are graded whole by the propositions C1-C11, F1, F4, F5 and F7 in sections 2 and 3. The 80 below are declined: 36 support no proposition (0.1, 0.2), 38 support a proposition that grades only part of them (0.3), and 6 say something the ports cannot show (0.4). C12, F2, F3, F6, F8 and F9 grade what can be graded and carry no claim id.

### 0.1 `~` on a read address or a dummy address: no effect of a write is stated anywhere (28 claims)

| Register | Address, type (p.6/p.7) | Claims |
|---|---|---|
| ADKCONR | 010 R | opus:AA-adkconr.c4, sol:AA-adkconr.c4 |
| DSKBYTR | 01A R | opus:AA-dskbytr.c4, sol:AA-dskbytr.c4 |
| INTENAR | 01C R | opus:AA-intenar.c4, sol:AA-intenar.c4 |
| INTREQR | 01E R | opus:AA-intreqr.c4, sol:AA-intreqr.c4 |
| JOY0DAT | 00A R | opus:AA-joy0dat.c4, sol:AA-joy0dat.c4 |
| JOY1DAT | 00C R | opus:AA-joy1dat.c4, sol:AA-joy1dat.c4 |
| POT0DAT | 012 R | opus:AA-pot0dat.c4, sol:AA-pot0dat.c4 |
| POT1DAT | 014 R | opus:AA-pot1dat.c4, sol:AA-pot1dat.c4 |
| POTINP | 016 R | opus:AA-potinp.c4, sol:AA-potinp.c4 |
| VHPOSR | 006 R | opus:AA-vhposr.c4, sol:AA-vhposr.c4 |
| VPOSR | 004 R | opus:AA-vposr.c4, sol:AA-vposr.c4 |
| LISAID | 07C R | opus:AA-lisaid.c5, sol:AA-lisaid.c5 |
| DSKDATR | 008 ER (dummy address) | opus:AA-dskdatr.c5, sol:AA-dskdatr.c5 |
| BLTDDAT | 000 ER (dummy address) | opus:AA-bltddat.c5, sol:AA-bltddat.c5 |

Reason. Each claim says the Coprocessor cannot write the address unless COPCON bit 1 is set (p.6: "~=Address not writable by the Coprocessor unless COPCON bit 1 is set true."). Every one of these addresses is a read address or an early-read dummy address. No page says what a write to such an address does, by the CPU or by anyone else. BLTDDAT is "a dummy address and cannot be read by the micro" (p.13), and DSKDATR is the "early read dummy address" (p.23). So "writable" (COPCON bit 1 set) and "not writable" (bit 1 clear) predict the same outcome at every output a test can see. The only thing that might differ is what the Copper does after a write is refused, and the AA specification does not say. p.10 mentions "the restart of the COPPER after lockup" at the NULL address 1FE, but it does not say what causes a lockup. A test must not invent that behaviour.

sol:AA-adkconr.c4 also reports a "CONFLICT" because the alphabetical entry on p.10 carries no `~`. The alphabetical list prints no `~` for any register, and its legend (p.10) does not define the symbol. That is silence, not a contrary statement. The COPCON entry (p.19-20) supports the address list: "(if 0, access to RGA>7E)".

### 0.2 The effect needs data read from, or written to, a disk surface (8 claims)

| Register, mark | Claims | Reason |
|---|---|---|
| DSKDAT `~` | opus:AA-dskdat.c5, sol:AA-dskdat.c5 | A write to DSKDAT fills the disk data buffer. Its only stated effect is on the data "sent to (write) ... the disk" (p.23), which needs a disk surface to observe. |
| DSKSYNC `~` | opus:AA-dsksync.c4, sol:AA-dsksync.c4 | DSKSYNC acts only against disk data: DSKBYTR bit 12 WORDEQUAL is "true only while DSKSYNC register equals the data from disk" (p.23), and INTREQ bit 12 DSKSYN is "Disk Sync register(DSKSYNC)matches Disk data" (p.25). This needs data read from a disk. |
| DSKDATR `&` | opus:AA-dskdatr.c4, sol:AA-dskdatr.c4 | The only use the page states is the disk-to-RAM DMA transfer in read mode (p.23), which needs data read from a disk. |
| DSKDATR `ER` | opus:AA-dskdatr.c2, sol:AA-dskdatr.c2 | "ER=Early read. This is a DMA data transfer to RAM, from either the Disk or from the Blitter" (p.6). For DSKDATR the data comes from the disk, so this also needs a disk surface. |

### 0.3 A proposition grades only part of the claim (38 claims)

A claim id goes on a proposition only if the proposition grades the whole claim. For the claims below, the propositions named grade the part every page agrees on, and the part left over cannot be graded. Each id is therefore declined whole, with the reason in the last column. The propositions stay, because the part they grade is page-predicted, but they carry no claim credit.

| Register, mark | Claims | Graded part (proposition) | Part not graded, and why |
|---|---|---|---|
| BLTADAT, BLTBDAT, BLTCDAT `~` | opus:AA-bltadat.c5, sol:AA-bltadat.c5, opus:AA-bltbdat.c5, sol:AA-bltbdat.c5, opus:AA-bltcdat.c5, sol:AA-bltcdat.c5 | With COPCON bit 1 clear, a Copper write does not take effect (C12) | With COPCON bit 1 set: p.6 marks these registers `&` "used by DMA channel only", while p.19 gives the Copper "access to all RGA registers" with CDANG = 1. The pages disagree on what a Copper write does then (section 4, D1). |
| BPL2DAT-BPL8DAT `&` | opus:AA-bpl2dat.c4, opus:AA-bpl3dat.c4, opus:AA-bpl4dat.c4, opus:AA-bpl5dat.c4, opus:AA-bpl6dat.c4, opus:AA-bpl7dat.c4, opus:AA-bpl8dat.c4, sol:AA-bpl2dat.c4, sol:AA-bpl3dat.c4, sol:AA-bpl4dat.c4, sol:AA-bpl5dat.c4, sol:AA-bpl6dat.c4, sol:AA-bpl7dat.c4, sol:AA-bpl8dat.c4 | The DMA channel loads the register (F6) | "only": p.15 says "They may also be written by either micro" (D1). |
| AUD0DAT-AUD3DAT `&` | opus:AA-aud0dat.c4, sol:AA-aud0dat.c4, opus:AA-aud1dat.c4, sol:AA-aud1dat.c4, opus:AA-aud2dat.c4, sol:AA-aud2dat.c4, opus:AA-aud3dat.c4, sol:AA-aud3dat.c4 | The DMA channel loads the register (F7) | "only": p.11 says "The Processor can also write directly to this register" (D1). |
| BLTADAT, BLTBDAT, BLTCDAT `&` | opus:AA-bltadat.c4, sol:AA-bltadat.c4, opus:AA-bltbdat.c4, sol:AA-bltbdat.c4, opus:AA-bltcdat.c4, sol:AA-bltcdat.c4 | The DMA channel loads the register (F8) | "only": p.13 says "it may also be preloaded by the microprocessor" (D1). |
| BLTDDAT `&` | opus:AA-bltddat.c4, sol:AA-bltddat.c4 | The destination data reaches RAM by blitter DMA (F8) | "only": p.13 says BLTDDAT "cannot be read by the micro", and no page says what a CPU read of 0xDFF000 returns, so no processor access to the register can be graded. |
| DSKDAT `&` | opus:AA-dskdat.c4, sol:AA-dskdat.c4 | Disk DMA feeds DSKDAT from RAM (F9) | "only": a processor write to DSKDAT has its only stated effect on the disk surface (section 0.2). |

### 0.4 The `h` mark: whether a register is new or changed in the hires chips is not observable (6 claims)

| Register, mark | Claims | Graded part (proposition) | Part not graded, and why |
|---|---|---|---|
| BLTSIZV, BLTSIZH `h` | opus:AA-bltsizv.c4, sol:AA-bltsizv.c4, opus:AA-bltsizh.c4, sol:AA-bltsizh.c4 | With the AA chip set selected the registers work as p.14 describes (F2) | The claims read the mark as "a register new in the hires chip set" (p.6) or "an existing register with something added or changed for the hires chips" (p.10); p.14 prints capital `H`, "new register in hires chips" (p.10). Whether a register is new or changed relative to an earlier chip set is not observable at the ports of one chip set (section 4, D2). |
| BLTCON0L `h` | opus:AA-bltcon0l.c4, sol:AA-bltcon0l.c4 | With the AA chip set selected the register replaces the minterm byte of BLTCON0 as p.13 describes (F3) | The same. |

---

## 1. Conventions shared by all propositions

### 1.1 System configuration

- The AA chip set is selected (`Config.chipset_config = 0x10`), with 2 MB of Chip memory (`Config.memory_config` default). No proposition depends on whether the harness runs PAL or NTSC.
- Drive 0 exists: `Config.floppy_config` is set to a value, taken from the harness's documented `Config` values, that configures drive 0. The builder states the value chosen in this specification. No disk image is mounted: the harness serves none, and `sdc_img_mounted` stays 0. With a drive configured, p.23's DMAON, "DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)", which has no drive term, decides whether disk DMA runs (C2, F9). Only if no documented value configures a drive at all are C2 and F9 NOT-CHECKABLE. No other proposition depends on the drive.
- The package is graded on both CPU cores, fx68k and TG68K. Every run sets `Config.chipset_config = 0x10`, which selects the AA chip set on either core; the harness default is 0x08 (ECS). The define `ENABLE_AGA` appears only in the board top files, where it sets the AGA menu parameter of the system control: neither `src/minimig-aga` nor the harness uses it, and the AA selection of the chip set is the `chipset_config` port. Both cores run as a 68000 (`Config.cpu_config = 0`).
- Programs are assembled for the 68000 (vasm's default CPU, no `-m68010` or higher), so every register write of 0x0000 in this document is a `MOVE`, a single write cycle, and never a `CLR`, which reads the address first. The propositions write 0 to write-only registers (DSKLEN, VPOSW, POTGO, JOYTEST, COPCON), and no page says what a read of such an address does.
- Custom registers are addressed at 0xDFF000 plus the offset printed on p.6-10.
- Addresses in this document are byte addresses. The memory port reports word addresses; compare a byte address `a` with the port's word address `a/2`. The harness guarantees that equality only in the identity-mapped Chip RAM 0x080000-0x1FFFFF (`nanomig_harness.h`, `kChipIdentityLo`/`kChipIdentityHi`), which is why every address in this document lies there. `[a, b)` is the byte range from `a` up to, but not including, `b`.

### 1.2 Memory map

- Everything lies in the identity-mapped Chip RAM 0x080000-0x1FFFFF (`nanomig.i`, `nanomig_harness.h`). Chip RAM below 0x080000 is not used: after reset "an additional copy of the system ROM responds starting at memory location $00000000" (HRM p.223), and the harness states that physical and logical addresses differ outside 0x080000-0x1FFFFF (`nanomig_harness.h`, `peek_logical`).
- The harness fixes three places:
  - the program at `PROGRAM_ORG` = 0x080400; it must end below 0x08CC36, the first byte of the S3 regions;
  - the stack below `PROGRAM_STACK` = 0x0FFF00; it must stay inside [0x0FC200, 0x0FFF00), above the S7 regions;
  - the result area RES at `RESULTS` = 0x0C0000, up to `DONE_ADDR` = 0x0C0FFE.
- The address families S0-S9 (section 1.8) are fixed. Their regions, listed there, must be zero-filled and used for nothing else. The harness's memory starts at zero, and nothing writes these regions.
- Every other region is placed by the builder in 0x080000-0x1FFFFF, disjoint from the three places above, from the S regions and from every other region. That covers the marker word MK, the Copper lists, and the blitter, bitplane, audio and disk buffers. Data regions are zero-filled unless stated otherwise.

### 1.3 Prologue

Every proposition starts from the following state, established by CPU writes in this order:

1. INTENA = 0x7FFF, INTREQ = 0x7FFF, DMACON = 0x7FFF. With bit 15 clear, each write clears every bit written as 1 (p.22, p.25). No interrupt reaches the CPU in any proposition.
2. ADKCON = 0x7FFF (p.10: bit 15 SET/CLR).
3. COPCON = 0x0000, FMODE = 0x0000, BPLCON0 = 0x0000, BPLCON1 = 0x0000, BPLCON2 = 0x0000, BPLCON3 = 0x0C00, BPLCON4 = 0x0011. These are the RST_ values printed on p.16, set explicitly. BPLCON0 = 0 keeps LPEN clear, so VHPOSR, VPOSR and HHPOSR read the beam counter, not a light-pen latch (p.12, LPENDIS).
4. POTGO = 0x0000. AUD0VOL through AUD3VOL = 0x0000.
5. Copper start, in the order p.21 prescribes: "It is important that one of the jump registers be initalized and it's jump strobe address hit, after power up but before Coprocessor DMA is initalized."
   - Place the idle list IDLE = {0xFFFF, 0xFFFE}.
   - Write COP1LCH/COP1LCL = IDLE.
   - Write any value to COPJMP1 (088).
   - Write DMACON = 0x8280 (SET, DMAEN, COPEN).

The Copper stays on in every proposition. `0xFFFF, 0xFFFE` is a WAIT (p.20: IR1 bit 0 = 1, IR2 bit 0 = 0) with BFD = 1 for vertical 0xFF and horizontal 0xFE. No line has more than 228 color clocks (p.29, STRLONG "(228 CC)"), so the horizontal count never reaches 0xFE and the WAIT never ends. At each vertical blank the Copper restarts at COP1LC: "COP1LC is automatically used at the beginning of each vertical blank time." (p.21).

### 1.4 Markers and results

- The CPU writes a distinct marker value to the word MK at each point named `M_...`. The memory port shows each marker write with its time; `t(M_x)` is the time of that write.
- Values the CPU reads from registers go into RES. The harness reads RES after the program ends.
- The program writes a final marker when it finishes: the `DONE` macro of `nanomig.i`. A run without the final marker is NOT-CHECKABLE.

### 1.5 CPU helpers (every loop has a timeout; a timeout is stored in RES)

- **VBL_SYNC:**
  1. INTREQ = 0x0020.
  2. Poll INTREQR until bit 5 (VERTB, "Start of Vertical blank", p.25) is 1. Timeout: 3 frames.
  3. INTREQ = 0x0020.
- **WAIT_LINE(v):** poll VHPOSR until bits 15-8 (V7-V0, p.29) are ≥ v. Use it only when the current line is below v and below 256.
- **LINES(n):** poll VHPOSR until the value of bits 15-8 has changed n times.
- **BLIT_DONE:** LINES(1), then poll DMACONR until bit 14 (BBUSY, "Blitter busy status bit", p.22) is 0. Timeout: 2 frames.
- **DMA_CHECK(bits):** read DMACONR, store it in RES, and require that the named enable bits and bit 9 (DMAEN) read 1 (p.22). This is a precondition wherever it is named.

### 1.6 Memory-port attribution

The RAM-port log gives, for each access, the cycle, the word address, the data, read or write, and the byte lanes (`nanomig_harness.h`, `RamAccess`). It does not say which requester made the access. Accesses are attributed by address and by nanomig.v's `refresh` output:

- **CPU:** reads of the program image, [0x080400, image end + 16), and of the script it executes, [script start, script end + 16), each of which covers the instruction or operand prefetch past its last word and holds nothing else; and accesses to the stack, MK and RES. Inside any window a proposition grades, the program accesses nothing else.
- **Refresh:** a read sampled while the `refresh` output is high. The harness does not log that output; the test reads it through `ports()` while stepping with `run_cycles(1)`, at the cycle of each log entry.
- **Copper:** reads of a Copper list the proposition names (IDLE, CASE, the F6 list).
- **A DMA channel:** accesses to a region the proposition assigns to that channel. "Blitter write" means a write to one of the proposition's blitter destination regions, "audio read" a read of its audio region, "disk read" a read of its disk region, and so on.
- **Stray DMA read:** a read that is none of the above. In each window a proposition grades, only the DMA channels it enables in DMACON run (the prologue clears the others), so a stray read is one of those channels reading outside the regions assigned to it.

Where a proposition says that every read of a channel lies in its regions, the check is that the window holds no stray DMA read. Where a proposition needs to know that a channel ran at all, whatever address it used, it counts the channel's region reads plus the stray DMA reads.

Precondition wherever stray DMA reads are graded: the refresh-flagged reads of the window come in groups of four, one group per line ("There are 4 refresh time slots", p.29; "4 cycles for memory refresh" per line, HRM p.194). This shows that the `refresh` output marks the refresh reads, so that none of them is counted as stray.

### 1.7 Outcomes

- **PASS:** every precondition holds and every expected value holds.
- **FAIL:** every precondition holds and an expected value does not.
- **NOT-CHECKABLE:** any precondition fails or any timeout occurred, except the DSKBLK-poll timeouts that C2 (step 8) and F9 (step 6) grade themselves.

NOT-CHECKABLE is never counted as PASS.

### 1.8 Address families S0-S9 (used by F4 and F5)

| S0 | S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8 | S9 |
|---|---|---|---|---|---|---|---|---|---|
| 0x0EAB64 | 0x11549A | 0x173388 | 0x08CC76 | 0x0C3C0E | 0x13C3F0 | 0x103E30 | 0x0FC1CE | 0x0BC000 | 0x143FFE |

Properties:

- S(2k+1) is the bitwise complement of S(2k) within A20-A1 (mask 0x1FFFFE). So every address bit A1-A20 is 1 in one address of each pair and 0 in the other.
- Across the ten addresses, each bit A1-A20 has its own on/off pattern, so a bit that is dropped, stuck, shifted or swapped gives a wrong address in at least one family member.
- The property still holds after XOR with x<<6 for x = 0-7 (used by F5).
- A20 and A19 differ in every address, so every address, and every region below, lies in the identity-mapped Chip RAM 0x080000-0x1FFFFF (section 1.2).
- S9's regions cross 0x144000, so reading inside them needs the pointer's increment to carry from A13 into A14. No region crosses a 64 KB boundary, so no check depends on a carry from the low word (A15-A1) into the high word (A20-A16).

The regions of each address, as used by F4 ([a, a+8)) and F5 ([a ^ (x<<6), +32) for x = 0-7), lie inside these byte ranges, which nothing else may use:

| S0 | S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8 | S9 |
|---|---|---|---|---|---|---|---|---|---|
| [0x0EAA24, 0x0EAC04) | [0x11541A, 0x1155FA) | [0x173208, 0x1733E8) | [0x08CC36, 0x08CE16) | [0x0C3C0E, 0x0C3DEE) | [0x13C230, 0x13C410) | [0x103E30, 0x104010) | [0x0FC00E, 0x0FC1EE) | [0x0BC000, 0x0BC1E0) | [0x143E3E, 0x14401E) |

### 1.9 Copper-case procedure (used by C1-C12)

**Case list** at CASE, in Chip memory:

| Offset | Word 1 | Word 2 | Meaning |
|---|---|---|---|
| +0 | 0x2001 | 0xFFFE | WAIT for line 0x20, horizontal 0 (p.20 format) |
| +4 | reg | V1 | MOVE: IR1 = register offset (bit 0 = 0), IR2 = data (p.20) |
| +8 | 0xFFFF | 0xFFFE | end |

C4 uses a different list (see C4).

A DMACON enable or ADKCON setting a C proposition names is written once, before frame A: frame A's observation runs the same DMA as frame B's, and DMA runs only with its channel bit and DMAEN set (p.22).

**Frame A, calibration** (once per register; the Copper runs IDLE):

1. VBL_SYNC.
2. Establish V0 with CPU writes, then write V1 with the CPU, and take the observation. Store it as R_cal1.
3. Establish V0 with CPU writes and take the observation. Store it as R_cal0.

V0 comes first in step 2 because V1 writes one register, while the observation also depends on registers only V0 sets: BLTCON1, the masks and BLTDMOD for C7, or DSKPTL for the DSKPTH case of C2. No page gives their reset values, and a DMA run leaves its pointer advanced ("After the Blitter is finished it will contain the last data address", p.13; the disk pointer "is incremented" after each transfer, HRM p.247).

**Frame B, the case** (once for each COPCON value c in {0x0000, 0xFFFD, 0x0002}, except C12, which runs only 0x0000 and 0xFFFD):

1. COP1LCH/L = CASE, written in the frame before.
2. VBL_SYNC. At this vertical blank the Copper restarts at CASE (p.21) and waits until line 0x20.
3. COPCON = c.
4. Establish V0 with CPU writes (per proposition).
5. M_start.
6. WAIT_LINE(0x22).
7. M_wait.
8. Observation (per proposition). Results go into RES.
9. COP1LCH/L = IDLE; COPCON = 0x0000.
10. Store INTREQR bit 5 as F_vb.
11. M_end.

**Preconditions for each Frame B:**

- PC1: at least one Copper read of CASE+4 (the MOVE's first word) lies in (t(M_start), t(M_wait)). This proves the Copper reached the MOVE. Whether the second word (CASE+6) is fetched is recorded but not graded, because no page says what a refused MOVE fetches.
- PC2: F_vb = 0, so no vertical blank started during the case and the case list ran exactly once.
- PC3: no timeout.
- PC4: R_cal1 = W and R_cal0 = U from frame A, where W and U are the proposition's "written" and "unwritten" values, and W ≠ U. This proves the observation can tell the two apart.
- PC5 (blitter cases only): DMA_CHECK(BLTEN). No blitter access lies between t(M_end) of one case and t(M_start) of the next, so every blit completed inside its case. The only blitter accesses between t(M_start) and t(M_end) are the ones the proposition names.

**Expected (C1-C11):** c = 0x0000 gives U; c = 0xFFFD gives U; c = 0x0002 gives W.

- c = 0xFFFD sets every COPCON bit except bit 1. COPCON "is a 1 bit register", and p.6 names bit 1 as the one that matters.
- Grounds for c = 0x0002: p.19 describes CDANG, bit 01 of COPCON: "Coprocessor danger mode. Allows coprocessor access to all RGA registers if true. (if 0, access to RGA>7E)". p.20 adds: "(on old chips- access to only RGA>3E if CDANG=1)".

**Contrasting cases for the procedure:**

- c = 0x0002 and c = 0x0000 each contrast the other: the page predicts W for one and U for the other.
- PC1: clear COPEN (DMACON = 0x0080) after step 1 and set it again after step 11. There is no Copper fetch, so PC1 fails and the program still finishes.
- PC2: between steps 7 and 8, wait for a full frame by polling INTREQR bit 5 without clearing it. F_vb = 1, so PC2 fails.
- PC4: calibrate with V1 replaced by V0. Then W = U, so PC4 fails.
- PC5: DMACON without BLTEN. DMA_CHECK fails.

**Why the check does not depend on the Copper's behaviour after a refused write:** each case runs one protected MOVE. The next case starts from a fresh vertical-blank restart at a newly written COP1LC. A Copper that stops after the refused MOVE and one that continues give the same observation.

**Note on the manual.** HRM p.310 (Appendix C) says that on the ECS, with the danger bit clear, "the Copper can access the address range from $DFF03E through $DFF07E". The AA specification says the opposite for 040-07E. The package follows p.19 and p.6 (see section 4, D4).

---

## 2. Propositions C1-C12: `~`, "Address not writable by the Coprocessor unless COPCON bit 1 is set true" (p.6)

All C propositions rest on the p.6 legend line quoted above and on the COPCON entry, p.19-20. Each follows section 1.9. Only the parts specific to each register are given below.

### C1. DSKLEN (024)

1. **Proposition.** A Copper MOVE to DSKLEN does not change DSKLEN while COPCON bit 1 is clear, and does change it while bit 1 is set. Claims: opus:AA-dsklen.c4, sol:AA-dsklen.c4 (p.6).
2. **Stimulus.** V0: DSKLEN = 0x0000. V1 = 0x4000 (bit 14 WRITE = 1, bit 15 DMAEN = 0, LENGTH = 0). After step 11: DSKLEN = 0x0000. DMAEN stays 0 throughout, so no disk DMA can start. p.23 gives DMAON = "DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)".
3. **Observation.** Step 8: read DSKBYTR and store bit 13.
4. **Expected.** U = 0, W = 1. From p.23: "13 DISKWRITE Mirror of bit 14 (WRITE) in DSKLEN", and DSKLEN "14 WRITE Disk Write(RAM to Disk ) if 1".
5. **Preconditions.** PC1-PC4.
6. **Contrasting cases.** As in section 1.9.
7. **Reference frame.** The markers and the case list (section 1.9).

### C2. DSKPTH (020), DSKPTL (022)

Drive 0 is configured and no disk image is mounted (section 1.1). If no documented `floppy_config` value configures a drive at all, C2 is NOT-CHECKABLE.

1. **Proposition.** A Copper MOVE to DSKPTH or DSKPTL does not change the address disk DMA reads while COPCON bit 1 is clear, and does change it while bit 1 is set. Claims: opus:AA-dskpth.c5, sol:AA-dskpth.c5, opus:AA-dskptl.c5, sol:AA-dskptl.c5 (p.6).
2. **Stimulus.**
   - Before frame A: DMACON = 0x8210 (SET, DMAEN, DSKEN); ADKCON = 0x9100 (SET, MFMPREC, FAST, p.10-11).
   - Regions: Rd holds 4 words. Re = Rd + 0x10000 holds 4 words; choose Rd with A16 = 0 so the two addresses differ in A16. Re' = Rd + 0x0100 holds 4 words; choose Rd with (Rd & 0xFFFF) ≤ 0xFEF8, so that Re' has Rd's high word and the DSKPTL case changes only the low word. Rd, Re and Re' are the disk regions of section 1.6.
   - V0: DSKPTH/DSKPTL = Rd.
   - DSKPTH case: V1 = Re >> 16. DSKPTL case: V1 = Re' & 0xFFFF.
   - Step 8 starts a disk write DMA with the sequence HRM p.247 prescribes ("the DMAEN bit in the DSKLEN register must be turned on twice"; the AA pages give no procedure):
     1. DSKLEN = 0x4000.
     2. DSKLEN = 0xC004.
     3. DSKLEN = 0xC004.
     4. Read DSKBYTR and store bit 14 (DMAON, "DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)", p.23).
     5. Poll INTREQR bit 1 (DSKBLK) until set. Timeout: 20 lines.
     6. DSKLEN = 0x4000; INTREQ = 0x0002.
   - Frame A's observations run the same sequence.
   - A timeout of the DSKBLK poll is stored in RES and does not make the case NOT-CHECKABLE (section 1.7). Disk DMA that never runs would otherwise hide behind that timeout; the reads are graded instead ("Disk DMA runs" below).
   - No drive is selected; the CIAs are not touched. p.23's DMAON has no drive term, so selection does not enter.
3. **Observation.** The first read in (t(M_wait), t(M_end)) that is a disk read or a stray DMA read (section 1.6).
4. **Expected.**
   - Disk DMA runs: every run of the step-8 sequence with DMAON = 1, in frame A and in frame B, shows at least one disk read or stray DMA read. A run with DMAON = 1 and no such read is a FAIL of C2. It is graded first, because PC4 needs frame A's reads.
   - U: at Rd.
   - W: at Re for the DSKPTH case, at Re' for the DSKPTL case.
   - From p.23: "This pair of registers contains the 20 bit address of Disk DMA data." p.6 row function: "Disk pointer (High 5 bits)" / "(Low 15 bits)".
   - Calibration (frame A, V0 first as section 1.9 says): R_cal1, with the CPU writing DSKPTH/DSKPTL = Rd and then V1, first reads at Re (resp. Re'); R_cal0, with the CPU writing DSKPTH/DSKPTL = Rd, first reads at Rd.
5. **Preconditions.**
   - PC1-PC5, with the disk channel in place of the blitter in PC5, and DMA_CHECK(DSKEN).
   - PC6: DMAON = 1 in every run (the stored DSKBYTR bit 14).
   - A drive is configured (section 1.1).
   - The refresh precondition of section 1.6.
6. **Contrasting cases.**
   - As in section 1.9.
   - PC6, in frame B only: DSKLEN's DMAEN is written once, not twice. DMAON reads 0 (HRM p.247: "the DMAEN bit in the DSKLEN register must be turned on twice in order to actually enable the disk DMA hardware"; HRM p.248: DMAON "Indicates when DMA is actually enabled"), the stored bit 14 reads 0 and PC6 fails.
  - PC5: DMACON without DSKEN: DMA_CHECK fails.
7. **Reference frame.** The markers.

### C3. VPOSW (02A)

1. **Proposition.** A Copper MOVE to VPOSW does not change the frame flop LOF while COPCON bit 1 is clear, and does change it while bit 1 is set. Claims: opus:AA-vposw.c4, sol:AA-vposw.c4 (p.6).
2. **Stimulus.** V0: VPOSW = 0x0000. V1 = 0x8000.
   - Both writes, the case list's MOVE (line 0x20) and the calibration writes happen at lines below 256. Writing 0 to bits 2-0 (V10-V8) there leaves the line number unchanged.
   - Bits 14-7 (chip identification and LOL) are written as 0 and not observed.
3. **Observation.** Step 8: read VPOSR and store bit 15.
4. **Expected.** U = 0, W = 1. From p.29: "VPOSW 02A W A Write Vert most sig. bits (and frame flop)"; bit 15 is "LOF=Long frame".
5. **Preconditions.** PC1-PC4. PC2 matters here: LOF is an "auto toggle" bit (p.29), so the write and the read must fall in the same frame.
6. **Contrasting cases.** As in section 1.9.
7. **Reference frame.** The markers and the VERTB frame boundary (PC2).

### C4. VHPOSW (02C)

1. **Proposition.** A Copper MOVE to VHPOSW does not move the beam counter while COPCON bit 1 is clear, and does move it while bit 1 is set. Claims: opus:AA-vhposw.c4, sol:AA-vhposw.c4 (p.6).
2. **Stimulus.** The case list is {0x4041, 0xFFFE, 0x002C, 0xC000, 0xFFFF, 0xFFFE}:
   - WAIT for line 0x40, horizontal 0x40;
   - then MOVE VHPOSW = 0xC000, which gives V7-V0 = 0xC0 and H8-H1 = 0.
   
   There is no V0. Step 6 is replaced: the CPU polls VHPOSR until bits 15-8 are ≥ 0x41 and stores that value as R. It starts polling at a line below 0x40.
3. **Observation.** R.
4. **Expected.**
   - U: R ∈ {0x41, 0x42}. W: R ∈ {0xC0, 0xC1}.
   - p.29 bit usage of VHPOSR/VHPOSW: "V7 V6 V5 V4 V3 V2 V1 V0,H8 H7 H6 H5 H4 H3 H2 H1".
   - Calibration, frame A, at a line between 0x08 and 0x3F:
     1. Read VHPOSR; bits 15-8 must be < 0x40 (this is R_cal0).
     2. Write VHPOSW = 0xC000.
     3. Read VHPOSR; bits 15-8 must be in {0xC0, 0xC1} (this is R_cal1).
5. **Preconditions.** PC1-PC4. BPLCON0 LPEN = 0 (from the prologue; p.12).
6. **Contrasting cases.** As in section 1.9.
7. **Reference frame.** The VHPOSR line number (p.29). The Copper WAIT position (p.20). The 256-line wrap of V7-V0 is never reached, because the case runs at lines below 0x43.

### C5. POTGO (034)

1. **Proposition.** A Copper MOVE to POTGO does not change the pot-pin drive while COPCON bit 1 is clear, and does change it while bit 1 is set. Claims: opus:AA-potgo.c4, sol:AA-potgo.c4 (p.6).
2. **Stimulus.** V0: POTGO = 0xFF00 (OUTRY, DATRY, OUTRX, DATRX, OUTLY, DATLY, OUTLX, DATLX all 1). V1 = 0xAA00 (all four outputs enabled, data 0). START (bit 0) stays 0. After step 11: POTGO = 0x0000.
   - The harness keeps the second and third buttons of both controller ports released throughout, so no pot line is pulled low from outside.
3. **Observation.** Step 8: read POTINP and store it ANDed with 0x5500.
4. **Expected.** U = 0x5500, W = 0x0000. p.26-27 bit table: "15 OUTRY Output enable for Paula pin 33, 14 DATRY I/O data Paula pin 33, ... 08 DATLX I/O data Paula pin 35"; POTINP is "Pot pin data read".
5. **Preconditions.** PC1-PC4. PC4 also shows that a pin driven low by Paula reads back 0 in this harness.
6. **Contrasting cases.**
   - As in section 1.9.
   - For the released-button precondition: the harness holds the second button of port 0 (pin 9, left Y, Paula pin 36, DATLY bit 10; p.26) pressed during frame A. R_cal0 then shows bit 10 = 0 and PC4 fails.
7. **Reference frame.** The markers.

### C6. JOYTEST (036)

1. **Proposition.** A Copper MOVE to JOYTEST does not change the mouse counters while COPCON bit 1 is clear, and does change them while bit 1 is set. Claims: opus:AA-joytest.c4, sol:AA-joytest.c4 (p.6).
2. **Stimulus.** V0: JOYTEST = 0x0000. V1 = 0xFCFC.
   - The harness drives no mouse movement and keeps all direction lines released during frames A and B.
3. **Observation.** Step 8: read JOY0DAT and JOY1DAT; after LINES(1), read both again. Store each value ANDed with 0xFCFC.
4. **Expected.** U = 0x0000 for both registers, W = 0xFCFC for both.
   - p.26: "JOYTEST 036 W D Write to all 4 Joystick-mouse counters at once", with test-data rows "JOY0DAT Y7 Y6 Y5 Y4 Y3 Y2 xx xx X7 X6 X5 X4 X3 X2 xx xx" and the same for JOY1DAT.
   - Bits 9-8 and 1-0 are masked because they show "the state of the related input signal pair" (p.25).
5. **Preconditions.** PC1-PC4. PC7: the two reads of each register are equal, so nothing but the write moved the counters.
6. **Contrasting cases.**
   - As in section 1.9.
   - PC7: the harness sends a mouse movement of 8 counts on port 0's horizontal axis between the two reads. JOY0DAT changes and PC7 fails.
7. **Reference frame.** The markers.

### C7. BLTCON0 (040) and BLTCON0L (05A)

1. **Proposition.** A Copper MOVE to BLTCON0 or BLTCON0L does not change the blitter's logic function while COPCON bit 1 is clear, and does change it while bit 1 is set. Claims: opus:AA-bltcon0.c4, sol:AA-bltcon0.c4, opus:AA-bltcon0l.c5, sol:AA-bltcon0l.c5 (p.6, p.7).
2. **Stimulus.**
   - Before frame A: DMACON = 0x8240 (SET, DMAEN, BLTEN).
   - V0: BLTCON0 = 0x0100 (USED = 1, LF = 0x00), BLTCON1 = 0x0000, BLTAFWM = BLTALWM = 0xFFFF, BLTDMOD = 0.
   - V1: BLTCON0 case, MOVE BLTCON0 = 0x01FF. BLTCON0L case, MOVE BLTCON0L = 0x00FF.
   - Step 8: BLTDPTH/L = DST; BLTSIZE = 0x0044 (h = 1, w = 4); BLIT_DONE.
3. **Observation.** Blitter writes in (t(M_wait), t(M_end)).
4. **Expected.**
   - Four writes at DST, DST+2, DST+4, DST+6.
   - U: all data 0x0000. W: all data 0xFFFF.
   - Why: LF = 0x00 selects no minterm and LF = 0xFF selects all eight, so D is 0 or all ones whatever A, B and C hold. p.14: "LF7-0 Logic function minterm select lines"; BLTCON0L writes the "lower 8 bits(minterms)" (p.7).
5. **Preconditions.** PC1-PC5. Exactly four blitter writes in the window.
6. **Contrasting cases.** As in section 1.9.
7. **Reference frame.** The markers.

### C8. BLTCON1 (042)

1. **Proposition.** A Copper MOVE to BLTCON1 does not change the blitter's address direction while COPCON bit 1 is clear, and does change it while bit 1 is set. Claims: opus:AA-bltcon1.c4, sol:AA-bltcon1.c4 (p.6).
2. **Stimulus.**
   - DMACON = 0x8240.
   - V0: BLTCON0 = 0x01FF, BLTCON1 = 0x0000, BLTAFWM = BLTALWM = 0xFFFF, BLTDMOD = 0.
   - V1 = 0x0002 (DESC; LINE = 0).
   - Step 8: BLTDPTH/L = DST, where the word below DST is reserved; BLTSIZE = 0x0042 (w = 2, h = 1); BLIT_DONE. C8's blitter destination region (section 1.6) is [DST-2, DST+4), so the writes of both outcomes are attributed to the blitter.
3. **Observation.** Addresses of the blitter writes in (t(M_wait), t(M_end)).
4. **Expected.** U: DST, then DST+2. W: DST, then DST-2. From p.14: "DESC Descending (decreasing address)control bit", bit 01 of BLTCON1 in area mode.
5. **Preconditions.** PC1-PC5. Exactly two blitter writes in the window.
6. **Contrasting cases.** As in section 1.9.
7. **Reference frame.** The markers.

### C9. BLTAFWM (044) and BLTALWM (046)

1. **Proposition.** A Copper MOVE to BLTAFWM or BLTALWM does not change the source A masks while COPCON bit 1 is clear, and does change them while bit 1 is set. Claims: opus:AA-bltafwm.c4, sol:AA-bltafwm.c4, opus:AA-bltalwm.c4, sol:AA-bltalwm.c4 (p.6, p.7).
2. **Stimulus.**
   - DMACON = 0x8240.
   - V0: BLTAFWM = BLTALWM = 0xFFFF, BLTCON0 = 0x09F0 (USEA, USED, LF = 0xF0, "D = A" per HRM p.177, Table 6-1), BLTCON1 = 0, BLTAMOD = BLTDMOD = 0.
   - V1 = 0x0F0F, written to BLTAFWM or BLTALWM.
   - Step 8: BLTAPTH/L = ASRC (three words, each 0xFFFF); BLTDPTH/L = DST; BLTSIZE = 0x0043 (w = 3, h = 1); BLIT_DONE.
3. **Observation.** Data of the three blitter writes in (t(M_wait), t(M_end)).
4. **Expected.**
   - U: 0xFFFF, 0xFFFF, 0xFFFF.
   - W for BLTAFWM: 0x0F0F, 0xFFFF, 0xFFFF. W for BLTALWM: 0xFFFF, 0xFFFF, 0x0F0F.
   - From p.13: "The patterns in these two registers are "anded" with the first and last words of each line of data from Source A into the Blitter."
5. **Preconditions.** PC1-PC5. Three blitter reads in ASRC and three blitter writes in the window.
6. **Contrasting cases.** As in section 1.9.
7. **Reference frame.** The markers. The order of writes gives "first" and "last" word.

### C10. BLTSIZE (058) and BLTSIZH (05E)

1. **Proposition.** A Copper MOVE to BLTSIZE or BLTSIZH does not start the blitter while COPCON bit 1 is clear, and does start it while bit 1 is set. Claims: opus:AA-bltsize.c4, sol:AA-bltsize.c4, opus:AA-bltsizh.c5, sol:AA-bltsizh.c5 (p.6, p.7).
2. **Stimulus.**
   - DMACON = 0x8240.
   - V0, established before M_start: BLTCON0 = 0x01FF, BLTCON1 = 0, BLTAFWM = BLTALWM = 0xFFFF, BLTDMOD = 0, BLTDPTH/L = DST. For the BLTSIZH case also BLTSIZV = 1.
   - V1: BLTSIZE = 0x0044, or BLTSIZH = 0x0004.
   - Step 8: BLIT_DONE only. The CPU starts nothing.
   - Calibration: R_cal1 is the CPU writing the same value (then BLIT_DONE); R_cal0 is no write at all.
3. **Observation.** Blitter writes in (t(M_start), t(M_end)).
4. **Expected.**
   - U: no blitter write.
   - W: four blitter writes at DST to DST+6, data 0xFFFF.
   - From p.14: "Writing to this register will start the Blitter" (BLTSIZE) and "BLTSIZH, which starts the blitter".
5. **Preconditions.** PC1-PC5.
6. **Contrasting cases.** As in section 1.9.
7. **Reference frame.** The markers.

### C11. BLTSIZV (05C)

1. **Proposition.** A Copper MOVE to BLTSIZV does not change the blit height while COPCON bit 1 is clear, and does change it while bit 1 is set. Claims: opus:AA-bltsizv.c5, sol:AA-bltsizv.c5 (p.6, p.7).
2. **Stimulus.**
   - DMACON = 0x8240.
   - V0: the same blitter set-up as C10, plus BLTSIZV = 2.
   - V1 = 5.
   - Step 8: BLTDPTH/L = DST; BLTSIZH = 0x0001; BLIT_DONE.
3. **Observation.** Number of blitter writes in (t(M_wait), t(M_end)).
4. **Expected.** U = 2 writes, W = 5 writes. From p.14: BLTSIZV holds "h14 ... h0", and "BLTSIZV should be written first, followed by BLTSIZH, which starts the blitter."
5. **Preconditions.** PC1-PC5.
6. **Contrasting cases.** As in section 1.9.
7. **Reference frame.** The markers.

### C12. BLTCDAT (070), BLTBDAT (072), BLTADAT (074): only the COPCON-bit-1-clear half

1. **Proposition.** While COPCON bit 1 is clear, a Copper MOVE to BLTxDAT (x = A, B, C) does not change the data the blitter uses from that source (p.6, p.7). Claims: none. The six `~` claims for these registers also state the COPCON-bit-1-set half, which C12 does not grade, so they are declined whole (section 0.3).
2. **Stimulus.**
   - DMACON = 0x8240. BLTCON1 = 0, BLTAFWM = BLTALWM = 0xFFFF, all modulos 0.
   - Per source x, the minterm codes are from HRM p.177: LF_A = 0xF0, LF_B = 0xCC, LF_C = 0xAA. The use bits are USEA = bit 11, USEB = bit 10, USEC = bit 9 (p.14).
   - Establishing V0 uses DMA only, because processor writes to BLTxDAT are disputed (section 4, D1). Preload blit:
     - BLTCON0 = USEx | USED | LF_x;
     - BLTxPTH/L = PX, holding the single word V0 = 0x3C3C, followed by one reserved zero word (the region is [PX, PX+4));
     - BLTDPTH/L = DST2; BLTSIZE = 0x0041; BLIT_DONE.
     - p.13: BLTxDAT "is normally loaded by the Blitter DMA channel".
   - V1 = 0x5A5A.
   - Step 8, observation blit:
     - BLTCON0 = USED | LF_x, with every USE source bit 0;
     - BLTDPTH/L = DST; BLTSIZE = 0x0042; BLIT_DONE.
     - With a source channel disabled, "the constant value stored in the data register of that channel will be used" (HRM p.170). That is the original-chip behaviour p.13 relies on when it says the register "may also be preloaded".
3. **Observation.** Data of the two blitter writes in (t(M_wait), t(M_end)).
4. **Expected.**
   - U = 0x3C3C, 0x3C3C, for c = 0x0000 and c = 0xFFFD.
   - c = 0x0002 is not run: p.6 marks these registers `&` "used by DMA channel only", while p.19 gives the Copper "access to all RGA registers" with CDANG = 1. The pages disagree on what a Copper write does then.
5. **Preconditions.**
   - PC1-PC3, PC5.
   - PC4 is replaced. Frame A runs the preload blit with V0 and then with V1 (0x5A5A in PX), each followed by the observation blit. The results must be 0x3C3C ×2 (U) and 0x5A5A ×2 (W_dma).
   - The preload blit shows exactly one read of [PX, PX+2) and no other read of [PX, PX+4).
   - The observation blit shows no read of [PX, PX+4). The word after PX is part of C12's blitter source region, so a source pointer left running after the preload is seen.
6. **Contrasting cases.**
   - The frame-A run with V1 preloaded by DMA gives 0x5A5A where the case gives 0x3C3C. All pages agree the DMA channel loads the register, so this is page-predicted.
   - Precondition contrasts as in section 1.9.
7. **Reference frame.** The markers.

---

## 3. Propositions F1-F9: row functions, `h` marks and the DMA channel's use of `&` registers

Every F proposition starts from the prologue, and the Copper runs IDLE unless stated otherwise.

### F1. BLTSIZE starts a blit of w words by h lines

1. **Proposition.** A write to BLTSIZE starts the blitter, and the blit covers w words (bits 5-0) on each of h lines (bits 15-6). Claims: opus:AA-bltsize.c5, sol:AA-bltsize.c5 (p.7, row function "Blitter start and size (window width, height)").
2. **Stimulus.**
   - Set-up: DMACON = 0x8240; BLTCON0 = 0x01FF; BLTCON1 = 0; BLTAFWM = BLTALWM = 0xFFFF; BLTDMOD = 0.
   - For each (w, h) in (1,1), (2,1), (4,1), (8,1), (16,1), (32,1), (63,1), (1,2), (1,4), (1,8), (1,16), (1,32), (1,64), (1,128), (1,256), (1,512), (1,1023), (5,3):
     1. BLTDPTH/L = DST (at least 1023 words). The pointer is reloaded before each blit: "prior to starting the Blitter for Blitter pointers" (p.6).
     2. M_start.
     3. LINES(2).
     4. M_go.
     5. BLTSIZE = (h << 6) | w. This is written last: "should be done last, after all pointers and control registers have been initialized" (p.14).
     6. BLIT_DONE.
     7. M_end.
3. **Observation.** Blitter writes in (t(M_start), t(M_end)).
4. **Expected.**
   - No blitter write in (t(M_start), t(M_go)).
   - Exactly w·h blitter writes in (t(M_go), t(M_end)). The i-th (from 0) is at DST + 2i with data 0xFFFF.
   - Page grounds:
     - p.14: "This register contains the width and height of the blitter operation", "Writing to this register will start the Blitter", bit row "h9 ... h0,w5 ... w0", "h=Height=Vertical lines", "w=Width =Horiz pixels (6 bits=64 words=1024 pixels max)". The width is in words.
     - p.13: the pointer "must be preloaded with the starting address".
     - p.6: the modulo is "added to the memory address at the end of each line". With 0, each line follows the previous one.
     - p.14: DESC = 0, so addresses do not decrease.
   - Quantifiers: every one of the 16 bits is set alone in some case, and (63,1) and (1,1023) set all bits of each field.
   - Value 0 in either field is not used. "(10 bits=1024 lines max)" and "(6 bits=64 words ...)" do not say what 0 encodes.
5. **Preconditions.** DMA_CHECK(BLTEN) before the first case. No timeout. No blitter access in (t(M_end), next t(M_start)).
6. **Contrasting cases.**
   - Omit the BLTSIZE write: no blitter write, since the page says the write starts the blit.
   - (5,3) changed to (6,3): 18 writes instead of 15.
   - Precondition: DMACON = 0x8200 (without BLTEN). DMA_CHECK fails, and BLIT_DONE ends on BBUSY or on its timeout, so the program finishes.
7. **Reference frame.** The markers.

### F2. BLTSIZV and BLTSIZH, the hires-chip size registers, work with the AA chip set

1. **Proposition.** With the AA chip set selected:
   - BLTSIZV holds a 15-bit height;
   - a following write to BLTSIZH (11-bit width) starts a blit of w words on h lines;
   - the write to BLTSIZV alone starts nothing;
   - BLTSIZV keeps its value for later BLTSIZH writes.
   
   Claims: none. The `h` ids of BLTSIZV and BLTSIZH (p.7) are declined (section 0.4): whether a register is new or changed in the hires chips is not observable at the ports. F2 grades what is: that the registers work with the AA chip set selected, which extends the hires chips (section 1.1). The two legends define `h` differently: "h=new for HiRes chip set" (p.6) against "h= stuff added or changed in hires chips" (p.10); p.14 prints capital `H`, "new register in hires chips" (p.10).
2. **Stimulus.**
   - Set-up as F1; destination DST2 of at least 32767 words.
   - Per case:
     1. BLTDPTH/L = DST2.
     2. M_start.
     3. BLTSIZV = v.
     4. LINES(2).
     5. M_v.
     6. BLTSIZH = w.
     7. BLIT_DONE.
     8. M_end.
   - Cases:
     - v = 2^j for j = 0-14, each with w = 1;
     - w = 2^i for i = 0-10, each with v = 1;
     - (v = 0x7FFF, w = 1) and (v = 1, w = 0x07FF);
     - reuse: (v = 3, w = 2), then a second case with BLTSIZV not written (steps 3-5 skipped, M_v written in place of step 5) and w = 5.
   - Bits printed as x are written 0 ("x s should be written to 0", p.14).
3. **Observation.** Blitter writes in (t(M_start), t(M_end)).
4. **Expected.**
   - No blitter write in (t(M_start), t(M_v)).
   - v·w writes in (t(M_v), t(M_end)) at DST2 + 2i with data 0xFFFF.
   - For the reuse case: 6, then 15.
   - Page grounds, p.14:
     - "BLTSIZH H 05E W A Blitter H size & start (11 bit width)", bits "w10 ... w0";
     - "BLTSIZV H 05C W A Blitter V size (15 bit height)", bits "h14 ... h0";
     - "BLTSIZV should be written first, followed by BLTSIZH, which starts the blitter. BLTSIZV need not be rewritten for subsequent blits if the vertical size is the same."
   - Value 0 is not used.
5. **Preconditions.** As F1.
6. **Contrasting cases.**
   - Omit BLTSIZH: no writes.
   - Rewrite BLTSIZV = 4 before the second reuse blit: 20 instead of 15.
   - v = 8 changed to 9: 9 writes.
   - Precondition contrast as F1.
   - Not used as a contrast: selecting OCS behaviour. The pages do not say what the harness's OCS selection removes.
7. **Reference frame.** The markers.

### F3. BLTCON0L writes only the minterm byte of BLTCON0

1. **Proposition.** With the AA chip set selected, a write to BLTCON0L replaces LF7-LF0 of BLTCON0 and leaves bits 15-8 (ASH3-0, USEA-USED) unchanged, whatever the upper byte of the written word. Claims: none. The `h` ids of BLTCON0L (p.7 mark `h`; p.13 capital `H` "new register in hires chips") are declined (section 0.4); F3 grades the register's effect with the AA chip set selected.
2. **Stimulus.**
   - DMACON = 0x8240. BLTCON1 = 0. BLTAFWM = BLTALWM = 0xFFFF. All modulos 0.
   - Pointers: BLTAPTH/L = ASRC (0x00FF, 0x0F0F); BLTBPTH/L = BSRC (2 words 0); BLTCPTH/L = CSRC (2 words 0); BLTDPTH/L = DST.
   - Sequence:
     1. BLTCON0 = 0x09F0 (USEA, USED, LF = 0xF0 "D = A").
     2. BLTCON0L = 0xFF0F.
     3. M_start.
     4. BLTSIZE = 0x0042.
     5. BLIT_DONE.
     6. M_end.
3. **Observation.** Blitter reads and writes in (t(M_start), t(M_end)).
4. **Expected.**
   - Exactly two blitter reads, at ASRC and ASRC+2. None in BSRC or CSRC.
   - Two blitter writes, at DST and DST+2, with data 0xFF00 and 0xF0F0: D = NOT A, LF 0x0F per HRM p.177, unshifted.
   - p.13: "BLTCON0L H 05A W A Blitter control register 0 (write lower 8 bits only) This is to speed up software-- the upper bits are often the same."
   - p.7: "Blitter control 0 lower 8 bits(minterms)".
   - p.14 BLTCON0 bits: 15-12 ASH3-0, 11-8 USEA-USED, 7-0 LF7-0.
5. **Preconditions.** As F1.
6. **Contrasting cases.**
   - Omit the BLTCON0L write: data 0x00FF, 0x0F0F.
   - Write 0xFF0F to BLTCON0 (040) instead: reads appear in BSRC and CSRC (USEB and USEC set), so the "only A reads" check fails.
7. **Reference frame.** The markers.

### F4. AUD0LCH/AUD0LCL and AUD1LCH/AUD1LCL hold A20-A16 and A15-A1 of the channel's start address

1. **Proposition.** Let a = `(AUDcLCH[4:0] << 16) | (AUDcLCL & 0xFFFE)` (c = 0, 1). The channel's first or second DMA read after its DMA is enabled is at a, and at most one read of the channel in the window lies outside [a, a+8), the first. Every one of the 5 high bits and 15 low bits takes part, and bits 15-5 of AUDcLCH and bit 0 of AUDcLCL do not.

   The pages leave the addresses of the first two reads open (section 4, D7), and the proposition holds under both readings.

   Claims: opus:AA-aud0lch.c5, sol:AA-aud0lch.c5, opus:AA-aud0lcl.c5, sol:AA-aud0lcl.c5, opus:AA-aud1lch.c5, sol:AA-aud1lch.c5, opus:AA-aud1lcl.c5, sol:AA-aud1lcl.c5 (p.7).
2. **Stimulus.** For c in {0, 1} and a in S0-S9 (section 1.8), each region [a, a+8) zero:
   1. AUDcLEN = 4; AUDcPER = 400 (at least 124, p.11); AUDcVOL = 0.
   2. AUDcLCH = a >> 16; AUDcLCL = a & 0xFFFF.
   3. M_start.
   4. LINES(2).
   5. M_go.
   6. DMACON = 0x8200 | (1 << c).
   7. DMA_CHECK(AUDcEN): read DMACONR and store it.
   8. LINES(20).
   9. M_end.
   10. DMACON = (1 << c).
   11. LINES(4).

   Extra case for a = S0: AUDcLCH = 0xFFE0 | (S0 >> 16) and AUDcLCL = (S0 & 0xFFFF) | 1.
3. **Observation.** Audio reads (reads of [a, a+8), the case's audio region) and stray DMA reads (section 1.6) in (t(M_start), t(M_end)), in time order.
4. **Expected.**
   - The first or the second read in (t(M_go), t(M_end)) that is an audio read or a stray DMA read is at a; this includes the extra case.
   - At most one stray DMA read lies in (t(M_go), t(M_end)), and if there is one, it is the first read of the window.
   - Page grounds:
     - p.7 row functions "Audio channel 0 location (High 5 bits)" / "(Low 15 bits)";
     - p.6: "+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4." and "LCL,LCH=20 bit Location (starting address) of DMA data";
     - p.11: "This pair of registers contains the 20 bit starting address (location) of Audio channel x (x=0,1,2,3) DMA data.";
     - HRM p.164: "Because of pipelining in Agnus, the first data word must be thrown away." (the reason one read outside [a, a+8) is allowed).
5. **Preconditions.**
   - DMA_CHECK(AUDcEN) in every case.
   - Only channel c is enabled: DMACONR reads no enable bit of another audio channel.
   - No audio read and no stray DMA read in (t(M_start), t(M_go)).
   - At least two audio reads or stray DMA reads in (t(M_go), t(M_end)). This counts the channel's reads wherever they go, so a wrong address fails the expected values, not this precondition.
   - The refresh precondition of section 1.6.
   - No timeout.
6. **Contrasting cases.**
   - A bit of the written address flipped (A4, in the second case): every read of the channel lies outside [a, a+8), so the first-or-second clause and the one-stray clause both fail, and the population precondition still holds. Each address and its complement in the family predict different reads.
   - The one-stray clause is moved on its own by F7's STRAY contrast, which uses the same predicate.
   - Preconditions: omit the channel's DMACON enable (DMA_CHECK fails and there are no audio reads); enable the other channel of the pair too (ONLY); write the enable before M_start (QUIET); write M_end at once after the enable (POP); write M_end before M_go (REF, an empty window); a wait that ends by its timeout (TMO).
7. **Reference frame.** The markers and the address family S0-S9 with its per-bit codes (section 1.8).

### F5. SPRxPTH/SPRxPTL hold A20-A16 and A15-A1 of sprite x's DMA address (x = 0-7)

1. **Proposition.** In a frame whose sprite pointers were written in the preceding vertical blank, sprite DMA reads from the address `(SPRxPTH[4:0] << 16) | SPRxPTL[15:1]` of each sprite x, with all 20 bits taking part. Claims (32, p.8):
   - opus:AA-spr0pth.c5, opus:AA-spr0ptl.c5, opus:AA-spr1pth.c5, opus:AA-spr1ptl.c5, opus:AA-spr2pth.c5, opus:AA-spr2ptl.c5, opus:AA-spr3pth.c5, opus:AA-spr3ptl.c5, opus:AA-spr4pth.c5, opus:AA-spr4ptl.c5, opus:AA-spr5pth.c5, opus:AA-spr5ptl.c5, opus:AA-spr6pth.c5, opus:AA-spr6ptl.c5, opus:AA-spr7pth.c5, opus:AA-spr7ptl.c5;
   - sol:AA-spr0pth.c5, sol:AA-spr0ptl.c5, sol:AA-spr1pth.c5, sol:AA-spr1ptl.c5, sol:AA-spr2pth.c5, sol:AA-spr2ptl.c5, sol:AA-spr3pth.c5, sol:AA-spr3ptl.c5, sol:AA-spr4pth.c5, sol:AA-spr4ptl.c5, sol:AA-spr5pth.c5, sol:AA-spr5ptl.c5, sol:AA-spr6pth.c5, sol:AA-spr6ptl.c5, sol:AA-spr7pth.c5, sol:AA-spr7ptl.c5.
2. **Stimulus.**
   - DMACON = 0x8220 (SET, DMAEN, SPREN). FMODE = 0 ("as before", p.23). The Copper runs IDLE.
   - For each Si (i = 0-9), with the regions [Si ^ (x << 6), +32) for x = 0-7 zero-filled:
     1. VBL_SYNC.
     2. M0.
     3. For x = 0-7: SPRxPTH = (Si ^ (x << 6)) >> 16; SPRxPTL = (Si ^ (x << 6)) & 0xFFFF.
     4. M1.
     5. VBL_SYNC.
     6. M2.
   - p.28: "These address registers must be initialized by the processor or coprocessor every vertical blank time." The zero control words keep every sprite off the screen.
3. **Observation.** Sprite reads (reads of the case's eight regions [Si ^ (x << 6), +32)) and stray DMA reads (section 1.6) in (t(M0), t(M2)). Reads of another family member's regions are stray.
4. **Expected.**
   - For every x there is at least one sprite read at exactly Si ^ (x << 6) in (t(M1), t(M2)).
   - No stray DMA read in (t(M1), t(M2)): every sprite read lies in the union of the eight regions.
   - Page grounds:
     - p.8 row functions "Sprite x pointer (High 5 bits)" / "(Low 15 bits)";
     - p.6 `+` legend;
     - p.6 "PTL,PTH=20 bit Pointer that addresses DMA data";
     - p.28 "This pair of registers contains the 20 bit address of Sprite x (x=0,1,2,3,4,5,6,7) DMA data."
   - The eight regions differ only in A8-A6, so a bit error in one sprite's pointer leaves that sprite's region start unread in at least one family member.
5. **Preconditions.**
   - DMA_CHECK(SPREN).
   - No sprite read and no stray DMA read in (t(M0), t(M1)).
   - At least eight reads in (t(M1), t(M2)) that are sprite reads or stray DMA reads. This counts the sprite reads wherever they go, so a wrong address fails the expected values, not this precondition.
   - The refresh precondition of section 1.6.
   - No timeout.
6. **Contrasting cases.**
   - Si and its complement predict disjoint address sets.
   - Precondition: DMACON without SPREN. DMA_CHECK fails and there are no sprite reads.
7. **Reference frame.** The VERTB frame boundary (p.25, bit 5 "Start of Vertical blank"), the markers, and the address family.
   - Not graded: which of the eight sprite channels made a given read. The AA pages give no sprite slot order, and the port does not report the requester.
   - Not graded: how many reads each sprite makes per frame. The page does not say.

### F6. BPL2DAT-BPL8DAT receive bitplane x's DMA data, which reaches the display as colour-index bit x-1

1. **Proposition.** In an 8-bitplane lores display, the data bitplane DMA fetches for plane x (x = 2-8) goes into BPLxDAT and appears on the video output as colour index 2^(x-1) (p.8, p.15). Claims: none. The 14 `&` claims for BPL2DAT-BPL8DAT also state "only", which is contradicted (section 4, D1) and not graded, so they are declined whole (section 0.3).
   
   Only the DMA part is specified.
2. **Stimulus.**
   - Plane memory: eight regions B1-B8 of 40 × 256 bytes. Plane x = 2-8 has 0xFFFF in all 20 words of rows 20 + 8(x-2) through 23 + 8(x-2) and 0 elsewhere. Plane 1 is all 0.
   - Palette: all 256 entries are written with LOCT = 0. For bank b = 0-7: BPLCON3 = (b << 13) | 0x0C00, then COLOR00-COLOR31 (180-1BE) for the indices 32b+r. Values:
     - index 0 = 0x000;
     - index 2 = 0xF00, 4 = 0x0F0, 8 = 0x00F, 16 = 0xFF0, 32 = 0x0FF, 64 = 0xF0F, 128 = 0xFFF (these are C2-C8);
     - every other index = 0x555.
     
     Then BPLCON3 = 0x0C00. Grounds: p.5 bank table; p.19: with LOCT = 0 "the 4 bit values are automatically extended to 8 bits". So each 8-bit gun holds the nibble twice and the 4-bit output equals the nibble.
   - Registers:
     - BPLCON0 = 0x0010 (BPU3 = 1, BPU2-0 = 0: 8 planes, lores, no HAM, no DPF; p.16);
     - BPLCON1 = 0, BPLCON2 = 0 (RDRAM = 0), BPLCON4 = 0x0011 (BPLAM = 0), FMODE = 0;
     - DIWSTRT = 0x2C81, DIWSTOP = 0x2CC1 (p.22);
     - DDFSTRT = 0x0038, DDFSTOP = 0x00D0 (the "normal" rows of the p.21 tables);
     - BPL1MOD = BPL2MOD = 0.
     
     "8 Bitplane 256 colours" in LORES needs "1x Bandwidth" (p.30-31).
   - A Copper list writes BPL1PTH/L-BPL8PTH/L = B1-B8 (offsets 0E0-0FE, all above 07E) and ends with 0xFFFF, 0xFFFE. COP1LC points to it, so the pointers are reloaded at every vertical blank (p.15, p.21).
   - DMACON = 0x8300 (SET, DMAEN, BPLEN).
   - VBL_SYNC twice, then M_ready, then VBL_SYNC four times, then M_done. Afterwards COP1LC = IDLE.
   - Four vertical blanks between M_ready and M_done give at least three complete VSYNC-to-VSYNC frames inside (t(M_ready), t(M_done)), whatever the phase of VSYNC relative to VERTB. No page fixes that phase.
3. **Observation.** The video output of the second and third complete frames (VSYNC to VSYNC) that begin after t(M_ready); both end before t(M_done). Bitplane reads (reads of B1-B8) during those frames.
4. **Expected** (in each of the two frames):
   - Every pixel is black (0,0,0) or one of C2-C8. The colour 0x555 never appears.
   - The non-black output lines form exactly seven bands. From top to bottom their colours are C2, C3, C4, C5, C6, C7, C8, each band a single colour.
   - Every band has the same height in output lines, and every gap between bands has the same height.
   - The non-black pixels of every line in every band span the same first and last column.
   - The two frames are identical.
   
   Page grounds:
   - p.15: "These registers receive the DMA data fetched from RAM by the Bit Plane address pointers described above ... They act as a 8 word parallel to serial buffer for up to 8 memory "Bit Planes"".
   - p.16: BPU "0000-1000 (NONE thru 8 inclusive)".
   - Plane x has weight 2^(x-1) in the colour address: p.17's PF2OF table gives "AFFECTED BITPLANE" 2 an offset of 2, plane 3 an offset of 4, and so on up to plane 8 = 128. p.18 describes BPLAM as XORed with "the 8 bitplane color address".
5. **Preconditions.**
   - DMA_CHECK(BPLEN).
   - In each graded frame there are bitplane reads in each of B1-B8.
   - The Copper list was read in the vertical blank before each graded frame.
   - No timeout.
6. **Contrasting cases.**
   - Swap the band rows of planes 3 and 4. The order becomes C2, C4, C3, C5, C6, C7, C8. Every precondition still holds, so the case fails through the band order.
   - Precondition "reads in each of B1-B8": BPLCON0 = 0x7000 (BPU = 7, p.16). Plane 8 is not fetched, so there are no reads in B8 and the precondition fails. This is a precondition contrast only: it removes a fetch, so it cannot show the colour clause failing.
   - Precondition: DMACON without BPLEN. DMA_CHECK fails and there are no bitplane reads.
7. **Reference frame.** Frame boundaries come from VSYNC on the video output. Band order is relative, top to bottom; no absolute line or column is graded. Plane memory row r is display line 0x2C + r, because vertical bitplane timing follows the display window (p.21), but this is used only to place the bands.

### F7. AUD0DAT-AUD3DAT are written by the channel's audio DMA with RAM data, output as signed bytes

1. **Proposition.** With channel c's audio DMA on, the words the channel reads from RAM are written into AUDcDAT and output as two's-complement bytes, so the channel's output level follows the sign and order of the fetched bytes. Claims: opus:AA-aud0dat.c2, sol:AA-aud0dat.c2, opus:AA-aud1dat.c2, sol:AA-aud1dat.c2, opus:AA-aud2dat.c2, sol:AA-aud2dat.c2, opus:AA-aud3dat.c2, sol:AA-aud3dat.c2 (access `W`) (p.7, p.8).
   - Only the DMA writer is specified. The processor writer is disputed (section 4, D1), so the eight `&` claims for AUD0DAT-AUD3DAT, whose "only" is not graded, are declined whole (section 0.3).
   - The eight `.c2` claims state the access type the address list prints, `W` ("W=Write", p.6), and nothing more. None of them says "only" or "write only", so F7 grades each whole: the register is written, and the written data reaches the output. Reads are not graded; the row gives no `R`, and no page says what a read returns.
2. **Stimulus.** For each channel c = 0-3 separately:
   - Region AD holds 0x4040, 0xC0C0, 0x2020, 0x0000. Each word has two equal bytes, because p.11 says the bytes are "outputed sequentially" but not which comes first.
   - AUDcLCH/L = AD; AUDcLEN = 4; AUDcPER = 400; AUDcVOL = 0x0040 (bit 6 "Forces volume to max", p.11). The other channels have DMA off and volume 0.
   - ADKCON modulation bits 7-0 are clear (from the prologue).
   - Sequence:
     1. M_start.
     2. DMACON = 0x8200 | (1 << c).
     3. DMA_CHECK(AUDcEN).
     4. LINES(100).
     5. M_end.
     6. DMACON = (1 << c); AUDcVOL = 0.
   - Why LINES(100): each byte is output for one period ("The minimum period is 124 color clocks ... This corresponds to a maximum sample frequency of 28.86 khz", p.11), so one word lasts 2 × 400 and the four-word data set 3200 color clocks. Three passes, plus up to one pass before the first L1, take 12800 color clocks, besides the start of output, whose latency no page gives. 100 lines of at least 227 color clocks (HRM p.194: "227.5 cycles per horizontal line") are at least 22700.
3. **Observation.**
   - Audio reads (reads of [AD, AD+8), the audio region) and stray DMA reads (section 1.6) in (t(M_start), t(M_end)).
   - The audio output samples in (t(M_start), t(M_end)), on every output signal the harness provides. On each signal, a run is a maximal stretch of equal samples. Runs shorter than T/4 are dropped, and neighbouring runs of equal value are then merged. T is the mean interval between consecutive audio reads in the window: the time from the first to the last, divided by the number of reads less one. The channel reads one word for every two bytes it outputs (p.11), so a level held for a word lasts about T. T is measured at the ports, so no conversion between clk_sys and color clocks is assumed. T/4 only separates transitions from levels; no expected value depends on it.
4. **Expected.**
   - On at least one output signal, the runs, taken in time order, contain the sequence (L1, L2, L3, L4) three times in succession (twelve consecutive runs) with the same four values each time, where L1 > L3 > L4 > L2.
   - These are the levels of bytes +64, -64, +32 and 0.
   - Page grounds, p.11:
     - "This register is the Audio channel x (x=0,1,2,3) DMA data buffer. It contains 2 bytes of data (each byte is a twos complement signed integer) that are outputed sequentially (with digital to analog conversion) to the audio output pins."
     - "The audio DMA channel controller automatically transfers data to this register from RAM."
   - Not graded:
     - how long each level lasts (that belongs to AUDxPER);
     - absolute amplitude (the page gives only "0.8 volts(peak to peak, typ)");
     - which output carries which channel.
5. **Preconditions.**
   - DMA_CHECK(AUDcEN).
   - At least eight audio reads in the window, and each of the four addresses of [AD, AD+8) at least twice.
   - At most one read of the window lies outside [AD, AD+8), and if there is one it is the channel's first read after the enable (HRM p.164: "Because of pipelining in Agnus, the first data word must be thrown away."; the pages leave its address open, section 4, D7).
   - DMA_CHECK(AUDcEN) in every case, read from DMACONR after the enable.
   - The refresh precondition of section 1.6.
   - ADKCONR & 0x00FF = 0, read before M_start (p.11: USExVy / USExPy clear).
   - No timeout.
6. **Contrasting cases.**
   - The second word 0xC0C0 changed to 0x6060 (+96): now the second level is the highest, and L1 > L3 > L4 > L2 fails, as the page predicts.
   - Preconditions: omit the DMACON enable (DMA_CHECK fails and there are no audio reads); set USE0V1 in ADKCON before the read of ADKCONR (ADK); a window of five lines (POP); rewrite AUDcLCL to AD + 0x100 sixty lines after the enable, so that the next restart (AA p.6: the location registers restart the pointer "whenever the audio length count is finished") reads outside [AD, AD+8) (STRAY); write M_end before the enable (REF, an empty window); a wait that ends by its timeout (TMO).
7. **Reference frame.** The audio reads of the window give the time scale T. No absolute latency between a read and its output is assumed, because the page gives none. The twelve runs may start anywhere in the window, so neither the start of output nor the length of the first pass through the data affects the check.

### F8. BLTADAT/BLTBDAT/BLTCDAT are loaded by the blitter's source DMA; BLTDDAT goes to RAM by DMA

1. **Proposition.** With source x enabled, the blitter loads BLTxDAT from the source pointer's RAM words and computes D from them. The destination words reach RAM as blitter DMA writes, with no processor access (p.6-7, p.13). Claims: none. The eight `&` claims for BLTADAT, BLTBDAT, BLTCDAT and BLTDDAT also state "only", which F8 does not grade, so they are declined whole (section 0.3).
2. **Stimulus.**
   - DMACON = 0x8240. BLTCON1 = 0. BLTAFWM = BLTALWM = 0xFFFF. All modulos 0.
   - Source regions: SRC_A, SRC_B, SRC_C, 4 words each. The source under test holds 0x8001, 0x4002, 0x2004, 0x1008; the other two hold 0xFFFF ×4.
   - BLTAPT/BPT/CPT = SRC_A/B/C. BLTDPT = DST.
   - For x = A, B, C:
     1. BLTCON0 = 0x09F0 (A), 0x05CC (B) or 0x03AA (C). These are USEx | USED | the "D = x" code of HRM p.177.
     2. M_start.
     3. BLTSIZE = 0x0044.
     4. BLIT_DONE.
     5. M_end.
     
     Reload all pointers before each blit.
3. **Observation.** Reads of SRC_A, SRC_B and SRC_C, and writes to [DST, DST+8), in (t(M_start), t(M_end)). These are the blitter's regions; the program does not access them inside the window (section 1.6).
4. **Expected.**
   - Four blitter writes at DST to DST+6 with data 0x8001, 0x4002, 0x2004, 0x1008 in that order.
   - No other write to [DST, DST+8) in the window. The port does not report the requester, so "no processor access" is graded as: the program makes none there, and the only writes are the blit's four.
   - Page grounds:
     - p.13: BLTxDAT "It is normally loaded by the Blitter DMA channel".
     - p.13: BLTDDAT "holds the data resulting from each word of Blitter operation until it is sent to a RAM destination ... The transfer is automatic during Blitter operation."
     - p.6 ER: "a DMA data transfer to RAM, from either the Disk or from the Blitter ... initiated by Agnus timing".
5. **Preconditions.** DMA_CHECK(BLTEN). Exactly four blitter reads in SRC_x in the window. No timeout. No blitter access after t(M_end).
6. **Contrasting cases.**
   - The third source word changed to 0x0000: the third D word is 0x0000.
   - Precondition: USEx cleared. There are no reads in SRC_x and the precondition fails.
7. **Reference frame.** The markers. Write order gives word order.

### F9. DSKDAT is fed by the disk DMA channel from RAM for LENGTH words

Drive 0 is configured and no disk image is mounted (section 1.1). If no documented `floppy_config` value configures a drive at all, F9 is NOT-CHECKABLE.

1. **Proposition.** A disk write DMA of LENGTH words reads exactly LENGTH consecutive words from RAM, starting at DSKPT, into the disk data buffer DSKDAT, and then raises the disk block request (p.6, p.23). Claims: none. The two `&` claims for DSKDAT also state "only", which F9 does not grade, so they are declined whole (section 0.3).
2. **Stimulus.**
   - DMACON = 0x8210. ADKCON = 0x9100. INTREQ = 0x0002.
   - Region DB holds 0x1111, 0x2222, 0x3333, 0x4444, followed by the guard words 0x5555, 0x6666. DSKPTH/L = DB.
   - Sequence (HRM p.247 order):
     1. M_start.
     2. DSKLEN = 0x4000.
     3. DSKLEN = 0xC004.
     4. DSKLEN = 0xC004.
     5. Read DSKBYTR and store bit 14 (DMAON, "DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)", p.23).
     6. Poll INTREQR bit 1 until set. Timeout: 1 frame. Store in RES whether the bit was seen set.
     7. M_blk.
     8. DSKLEN = 0x4000.
     9. LINES(4).
     10. M_end.
     11. INTREQ = 0x0002.
   - No drive is selected. p.23's DMAON has no drive term, so selection does not enter.
   - The timeout of step 6 does not make the case NOT-CHECKABLE (section 1.7), because DSKBLK is part of the clause under test: when the preconditions below hold, a timeout there fails the expected value "INTREQR bit 1 becomes set".
3. **Observation.** Disk reads (reads of [DB, DB+12), the disk region) and stray DMA reads (section 1.6) in (t(M_start), t(M_end)). The polling result.
4. **Expected.**
   - Exactly four disk reads, at DB, DB+2, DB+4, DB+6 in that order, none at the guard words, and no stray DMA read in the window. With DMAON = 1, no read at all is a FAIL: p.23's DMAON has no drive term.
   - INTREQR bit 1 becomes set, with t(4th read) < t(M_blk).
   - Page grounds:
     - p.23 DSKLEN: "13-0 LENGTH Length (# of words) of DMA data".
     - p.23 DSKDAT: "This register is the Disk-DMA data buffer ... The DMA controller automatically transfers data to or from this register and RAM, and when the DMA data is finished (Length=0) it causes a Disk Block Interrupt."
     - p.25: bit 1 "DSKBLK Disk Block finished".
5. **Preconditions.**
   - DMA_CHECK(DSKEN).
   - DMAON = 1 (the stored DSKBYTR bit 14).
   - A drive is configured (section 1.1).
   - The refresh precondition of section 1.6.
6. **Contrasting cases.**
   - DSKLEN = 0xC003 (both writes): three reads.
   - Precondition: DMACON without DSKEN. DMA_CHECK fails, DMAON reads 0, and there are no disk reads.
7. **Reference frame.** The markers.

---

## 4. Disagreements between pages, and what is left open

- **D1. `&` against the register descriptions.**
  - p.6 says "&=Register used by DMA channel only." and separately "%=Register used by DMA channel usually, processors sometimes."
  - p.11 (AUDxDAT): "The Processor can also write directly to this register."
  - p.13 (BLTxDAT): "it may also be preloaded by the microprocessor."
  - p.15 (BPLxDAT): "They may also be written by either micro."
  
  These are the `%` description, not the `&` one. Every page agrees the DMA channel loads the register, and that is all F6, F7, F8 and C12 grade. The `&` claims, and the `~` claims of BLTADAT, BLTBDAT and BLTCDAT, are declined whole for that reason (section 0.3). For DSKDAT, DSKDATR and BLTDDAT no page contradicts "only", but nothing about processor writes to them is observable without a disk, or is stated at all (section 0).
- **D2. The `h` legend.** p.6 says "h=new for HiRes chip set". p.10 says "h= stuff added or changed in hires chips" and "H= new register in hires chips". For BLTCON0L, BLTSIZV and BLTSIZH, the alphabetical list prints capital `H` (p.13-14). Under either legend the mark says how the register relates to earlier chip sets, which no port shows; the six `h` ids are declined (section 0.4), and F2 and F3 grade that the registers work with the AA chip set selected. p.10 also defines lowercase `p` as "stuff added or changedin hires chips", while p.6 defines `p` as "new for IAA chip set". No proposition depends on `p`.
- **D3. BLTDDAT's access type.** p.6 prints "ER" and p.13 prints "W". Neither is graded (section 0.1).
- **D4. The Copper's reach with COPCON bit 1 clear.**
  - AA p.19: "(if 0, access to RGA>7E)".
  - HRM p.310 (ECS): "If this bit is clear, the Copper can access the address range from $DFF03E through $DFF07E".
  
  This is a disagreement between the manual and the oracle, not within the oracle. C7-C12 follow the AA page. The two sources agree that with the bit set the Copper reaches every register, and that 000-03C are closed with it clear (HRM p.310 opens the range from 03E).
- **D5. Left open by the pages, and not graded:**
  - what the Copper does after a refused write (only p.10's "restart of the COPPER after lockup" is printed);
  - what a CPU read of a write-only or dummy address returns;
  - what value 0 means in BLTSIZE, BLTSIZH and BLTSIZV;
  - which byte of AUDxDAT is output first;
  - which sprite channel makes which sprite read.
- **D7. The addresses of the first two audio reads after the enable.** HRM p.164: "In DMA-driven operation, transition to the 001 state occurs and DMA requests are sent to Agnus as soon as DMA is turned on. Because of pipelining in Agnus, the first data word must be thrown away. State 101 is entered as soon as this word arrives; a request for the next data word has already gone out." Figure 5-8 (HRM p.166) loads the output buffer only from the second word that arrives. HRM p.145: "Enabling an audio channel almost always starts the data output again from the top of the list of data pointed to by the location registers for that channel." HRM p.148 lists "Copy to "back-up" location register from AUD0LCL (will be used as a pointer showing current data word to fetch)" before "Start retrieving audio data each allocated DMA time slot". Read with p.164 and p.145, the thrown-away word is a read before the one at the location; read with p.148, the first read is at the location. The pages do not say which, so F4 and F7 allow the address of the first read to be open: F4 grades that the first or the second read is at a, and F4 and F7 allow one read outside the channel's region, the first.
- **D6. Outside the claims.** SPRHDAT (078) is printed without `~` (p.7), although p.19 closes everything up to 07E to a Copper with CDANG clear. No claim of this package concerns it.

---

## 5. Claim index

| Register | Mark | Claims | Where |
|---|---|---|---|
| ADKCONR, DSKBYTR, INTENAR, INTREQR, JOY0DAT, JOY1DAT, POT0DAT, POT1DAT, POTINP, VHPOSR, VPOSR | `~` | opus:/sol:AA-<name>.c4 | 0.1 |
| LISAID, DSKDATR, BLTDDAT | `~` | opus:/sol:AA-<name>.c5 | 0.1 |
| DSKDAT | `~` | opus:/sol:AA-dskdat.c5 | 0.2 |
| DSKSYNC | `~` | opus:/sol:AA-dsksync.c4 | 0.2 |
| DSKDATR | `&`, `ER` | opus:/sol:AA-dskdatr.c4, .c2 | 0.2 |
| DSKLEN | `~` | opus:/sol:AA-dsklen.c4 | C1 |
| DSKPTH, DSKPTL | `~` | opus:/sol:AA-dskpth.c5, -dskptl.c5 | C2 |
| VPOSW | `~` | opus:/sol:AA-vposw.c4 | C3 |
| VHPOSW | `~` | opus:/sol:AA-vhposw.c4 | C4 |
| POTGO | `~` | opus:/sol:AA-potgo.c4 | C5 |
| JOYTEST | `~` | opus:/sol:AA-joytest.c4 | C6 |
| BLTCON0, BLTCON0L | `~` | opus:/sol:AA-bltcon0.c4, -bltcon0l.c5 | C7 |
| BLTCON1 | `~` | opus:/sol:AA-bltcon1.c4 | C8 |
| BLTAFWM, BLTALWM | `~` | opus:/sol:AA-bltafwm.c4, -bltalwm.c4 | C9 |
| BLTSIZE, BLTSIZH | `~` | opus:/sol:AA-bltsize.c4, -bltsizh.c5 | C10 |
| BLTSIZV | `~` | opus:/sol:AA-bltsizv.c5 | C11 |
| BLTADAT, BLTBDAT, BLTCDAT | `~` | opus:/sol:AA-bltadat.c5, -bltbdat.c5, -bltcdat.c5 | 0.3 (declined; C12 grades the bit-1-clear half) |
| BLTSIZE | function | opus:/sol:AA-bltsize.c5 | F1 |
| BLTSIZV, BLTSIZH | `h` | opus:/sol:AA-bltsizv.c4, -bltsizh.c4 | 0.4 (declined; F2 grades the registers' work) |
| BLTCON0L | `h` | opus:/sol:AA-bltcon0l.c4 | 0.4 (declined; F3 grades the register's effect) |
| AUD0LCH, AUD0LCL, AUD1LCH, AUD1LCL | function | opus:/sol:AA-aud0lch.c5, -aud0lcl.c5, -aud1lch.c5, -aud1lcl.c5 | F4 |
| SPR0PTH-SPR7PTL | function | opus:/sol:AA-spr<n>pth.c5, -spr<n>ptl.c5 | F5 |
| BPL2DAT-BPL8DAT | `&` | opus:/sol:AA-bpl<n>dat.c4 | 0.3 (declined; F6 grades the DMA part) |
| AUD0DAT-AUD3DAT | `&` | opus:/sol:AA-aud<n>dat.c4 | 0.3 (declined; F7 grades the DMA part) |
| AUD0DAT-AUD3DAT | `W` | opus:/sol:AA-aud<n>dat.c2 | F7 |
| BLTADAT, BLTBDAT, BLTCDAT, BLTDDAT | `&` | opus:/sol:AA-bltadat.c4, -bltbdat.c4, -bltcdat.c4, -bltddat.c4 | 0.3 (declined; F8 grades the DMA part) |
| DSKDAT | `&` | opus:/sol:AA-dskdat.c4 | 0.3 (declined; F9 grades the DMA part) |


---

## 6. Build

This section states what the programs, the test and the placement of the regions add to sections 0-5. It changes no proposition, stimulus or expected value of those sections.

### 6.1 Files

| File | Content |
|---|---|
| `prog_run.s`, `prog_run_nofix.s` | The one stimulus program. It executes a script that the test places in Chip memory before the run, and ends with `DONE`. `prog_run_nofix.s` is the same program without the store of the constant `$BEEF` at `RESULTS`. Assembled for the 68000. Every custom register is written by one `MOVE.W` of a script word, or by one `MOVE.L` on a pointer pair; no `CLR` and no read-modify-write instruction is used. |
| `s7_core.h` | The script builder, a run that captures the RAM port, the `refresh` output, the video and audio samples, the markers, and the attribution of reads of section 1.6. |
| `s7_c.h`, `s7_ceval.h`, `s7_cprops.h`, `s7_c12.h` | The Copper-case procedure of section 1.9 and the propositions C1-C12. |
| `s7_f1.h`, `s7_f2.h`, `s7_f6.h` | The propositions F1-F9. |
| `test_aa_integ_s7.cpp` | `main`, the claim declines of section 0, and the `// MUTATION` records. |

The script ops are `W16`, `W32` (one `MOVE.L` on a pointer pair), `DLY`, `RD` (store a register read in the log), `MARK`, `VBLSYNC`, `WAITLINE`, `LINES`, `BLITDONE`, `POLLB`, `WM` (a `MOVE.W` to a Chip RAM word), `W16R` and `WAITB`. `VBLSYNC`, `WAITLINE`, `LINES`, `BLITDONE` and `WAITB` add 1 to the word at `RESULTS`+2 when they end by their timeout; `POLLB` ends after a given number of reads and stores the last read. A timeout is at least as long as the section 1.5 timeout of the helper: the loops count at least eight colour clocks per iteration (a `DBRA` loop of a register read takes at least 36 CPU clocks), so 15000 iterations of `VBLSYNC` last more than three frames, and 30000 iterations of `BLITDONE` more than two.

The program stores the log words (the values the CPU read from registers, in order) from `RESULTS`+$10 on, and the marker word MK is written with the marker value 1, 2, 3, ... at each `M_...` point, so each marker value is distinct and its first write to MK gives `t(M_x)`.

`Config.floppy_config` is 0, the value that `nanomig_harness.h` documents as one drive, DF0, write enabled, normal speed. No disk image is mounted.

### 6.2 Placement of the regions

Everything lies in the identity-mapped Chip RAM, outside the three places and the S regions of section 1.

| Region | Bytes |
|---|---|
| Program image, and the script | The image starts at 0x080400 and the script at 0x081000; both end below 0x08CC36, the first byte of the S3 regions. The CPU attribution of section 1.6 is [0x080400, image end + 16) and [0x081000, script end + 16) of the run, the stack, MK and RES. |
| MK | 0x0B8000 |
| RES and the log | [0x0C0000, 0x0C1000) |
| IDLE | 0x090000; the Copper list region is 16 bytes (the two list words and the zero words after them). AA p.20, WAIT: "(keeps Coprocessor off of bus until beam position has been reached)". The Copper nevertheless reads the word after the final WAIT of a list, IDLE + 4 after nearly every vertical-blank restart and the word after the end WAIT of a case list; the 16-byte region keeps these reads in the Copper class, and the runs print their counts per core as MEASURED lines (not graded). |
| Case lists | 0x090100 + 0x40 k for the k-th register case of a proposition; C12 uses 0x090140, 0x090180, 0x0901C0 for BLTADAT, BLTBDAT, BLTCDAT; each list region is 16 bytes |
| DST (C7-C12, F3, F8), C12's preload destination | 0x1B0200, 0x1B0400. They lie in the top of Chip memory, away from RES, the stack and the S regions, so that a blit that does not stop at its specified size overwrites no result. |
| DST2 (F1, F2) | 0x1A0000, 65534 bytes |
| ASRC, BSRC, CSRC, PX | 0x0B2000, 0x0B2100, 0x0B2200, 0x0B2300 |
| F8 sources | 0x0B3000 + 0x100 (3 x + k) for source k of the case x |
| Rd, Re, Re' (C2) | 0x0E0000, 0x0F0000, 0x0E0100 |
| AD (F7), DB (F9) | 0x0D8000, 0x0D2000 |
| Bitplanes B1-B8 (F6), the F6 Copper list | 0x180000 + 0x4000 (x - 1), 0x090400 |

A write to Chip RAM that is not an access of the program (section 1.6, CPU) is a blitter write, wherever it lands: the blitter is the only other writer. Every observation of blitter writes counts all of them, so a blit that writes outside the proposition's destination region is seen. The regions of a proposition are used to attribute reads. C12's program writes to PX are the preload of V1 and are not blitter accesses.

### 6.3 Reading of the pages the build fixes

- Marker windows are open intervals: an access at the cycle of a marker write is in no window.
- Attribution tolerances of the test, which no page states:
  - A bus cycle stretched by wait states appears in the port's log once per bus period. A log entry that repeats the address, direction, data and byte lanes of the entry before it exactly one bus period later continues that cycle and is not a new access; nothing else is merged, so two accesses to one word with equal data that are not exact continuations are two accesses. The bus period is the smallest spacing of two consecutive log entries of the run (4 clk_sys cycles on both cores).
  - A read counts as a refresh read when the `refresh` output was high in any of the four clk_sys cycles up to its log entry. The refresh precondition below guards this tolerance: a read wrongly flagged would break the groups of four.
- A *colour clock* K is the median spacing of consecutive refresh-flagged reads of the run divided by two, measured in the run (the four refresh reads of a line are two colour clocks apart, "There are 4 refresh time slots", p.29). It is used only in the refresh precondition, together with 228 colour clocks per line (p.29, STRLONG "(228 CC)").
- The refresh precondition (labels `C2-PCR`, `F4-REF`, `F5-REF`, `F7-REF` and `F9-REF`): the refresh-flagged reads of the window form groups, a gap of more than 8 K starting a new group; every group has four reads, except a group cut by the window's edge, which may have fewer; at least one whole group lies in the window. A window of n lines holds at least n - 1 whole groups. The clause that the groups have four reads, one group per line, is unfalsifiable at this boundary: no stimulus changes how the refresh output groups its reads. The records of the precondition (an empty window) move its population clause, that at least one whole group lies in the window.
- C2's observation is the first read in the window that is a disk read or a stray read. The stray class includes every read that is none of CPU, refresh, Copper and the disk regions.
- F6's bands. The video output is the scan-doubled output of the harness, so a line is an output line (a rising edge of `hs`). A pixel is black when its three 4-bit components are 0. C2-C8 are (15,0,0), (0,15,0), (0,0,15), (15,15,0), (0,15,15), (15,0,15), (15,15,15) (the 4-bit output of the p.19 extension of the 4-bit values). A band is a maximal run of output lines that hold a non-black pixel; its height is the number of lines; a gap is the number of black lines between two bands; the first and last column of a line are the sample offsets from the start of the line of its first and last non-black pixel.
- F1 and F2 case (w, h): `BLTSIZE` is written as `(h << 6) | w`; the case (63, 1) and (1, 1023) of F1 and (v = 0x7FFF) and (w = 0x07FF) of F2 set every bit of their fields.

### 6.4 Labels, selectors and mutations

Every label starts with the proposition's id. The C propositions print `Cn-FIXTURE`, `Cn-PC1` to `Cn-PC5` as section 1.9 names them (PC5 in the propositions that run a DMA channel), `Cn-PC6` (`C6-PC7` for the counters' stillness, as section 2 names it; `C2-PC6` for DMAON), `C2-PCR` for the refresh precondition, and the proposition's own line `Cn`. The F propositions print `Fn-FIXTURE`, `Fn-DMA` (DMA_CHECK), `Fn-TMO` (no timeout) and the preconditions of their sections, under the names in the mutation records of `test_aa_integ_s7.cpp`.

`NMH_MUTATE=<label>` runs the proposition that owns the label with one changed stimulus and the unchanged comparator; `NMH_ONLY=Cn,Fm` restricts a run to the named propositions. The normal run runs no mutant. A mutation run of a C proposition runs the first register case of the proposition only, and of C12 the first source (BLTADAT) only. The population contrasts of C2 (PC6), C6 (PC7) and C7, C8, C9 (PC6) apply to frame B only. A label may have more than one record when its line has more than one clause a stimulus can move (F5, F9: the stray clause).


---

## 7. Recorded

The final `make -k -B test` ran on both cores; every run selects the AA chip set (`chipset_config = 0x10`) and configures one drive (`floppy_config = 0`).

| | fx68k | TG68K |
|---|---|---|
| Graded lines (`PASS:` / `FAIL:`) | 142 passed, 0 failed | 142 passed, 0 failed |
| `RECORDED:` lines | 7 (the seven declined groups of section 0) | 7 |
| Mutation records in `test_aa_integ_s7.cpp` | 145, each prints `FAIL:` on its label | 145, each prints `FAIL:` on its label |
| Colour clock K, bus period | 8 and 4 clk_sys cycles | 8 and 4 clk_sys cycles |
| Claims asserted | 80 of 160 (the 80 of section 0) | 80 of 160 |

Measured values, per core, in the order of the propositions:

- C1, C3, C5, C6 and C8-C11: for COPCON $0000 and $FFFD the observation equals U and for COPCON $0002 it equals W, on both cores.
- C2: the first disk read is at Rd (0x0E0000) for COPCON $0000 and $FFFD; with COPCON $0002 it is at Re (0x0F0000) for DSKPTH and at Re' (0x0E0100) for DSKPTL. A disk DMA with DMAON set reads RAM in every run; the window of each case is two lines long and holds two or three whole groups of refresh reads.
- C4: with COPCON $0000 and $FFFD the poll ends on a line in $41-$42, with COPCON $0002 on a line in $C0-$C1.
- C7, C9, C10, C12: the blitter writes of every case are those the propositions list, with no write outside the four, three or two words counted; C12's observation blit writes $3C3C twice in both frames B, and $5A5A twice after a DMA preload of V1 in frame A.
- Copper reads past the final WAIT of a list (AA p.20, not graded), the same count on both cores: the word IDLE + 4 (0x090004) is read 3 times in C2, 4 in C12, 20 in F5 and 0 in F9; the word after the end WAIT of C2's DSKPTH case list (0x09010C) is read once, and the words after the other case lists of C2 and C12 are not read.
- F1: every case writes exactly w x h words at DST + 2 i with $FFFF, and nothing before the BLTSIZE write. F2: the same with v x w words, and nothing before the BLTSIZH write.
- F3: the blit reads ASRC and ASRC + 2 and nothing of BSRC and CSRC, and writes $FF00 and $F0F0.
- F4: on both cores 0 of 22 cases read the written location first and 22 read it second; the first read is at the address the channel's pointer held before the case ($000000 in the first case), and it is the only read outside [a, a+8). A private copy of the chip set whose channel reads the location first (SPECIFICATION.md 4, D7) also passes: 22 of 22 first reads at the location, no read outside.
- F5: for each of the ten families every sprite pointer is read at its exact address; there is no stray read.
- F6: seven bands of 8 output lines in the order C2-C8 in both graded frames, gaps of 8 lines, every band spanning the same columns: 142-780 on fx68k, 141-780 on TG68K; no $555 pixel.
- F7: each channel's window holds 29 or 30 reads of [AD, AD + 8) and one read outside it, the channel's first, at 0x000000; the level sequence of the proposition is present on one output signal (the left output for channels 0 and 3, the right output for channels 1 and 2). The private copy of the chip set of F4 also passes F7: no read outside [AD, AD + 8).
- F8: four reads of SRC_x and the four writes $8001, $4002, $2004, $1008 for each of A, B and C.
- F9: four disk reads at DB, DB + 2, DB + 4, DB + 6; DSKBYTR bit 14 reads 1; INTREQR bit 1 is set after the fourth read.
