# NM-HRM-041 — DSKBYTR is a constant word in three of its five fields [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should DSKBYTR's bit 15 (DSKBYT) read clear on the second of two consecutive reads, bit 12 (WORDEQUAL) read 1 only while DSKSYNC equals the incoming disk word, and bits 7-0 (DATA) carry the byte just received, where `paula_floppy.v` returns the constants 1, 1 and `0x00`? Closes when `paula_floppy.v` drives the three fields (the seven reds under *Assertions* turn green), or when an Amiga specialist rejects the change, also as out of scope for a core that moves whole sectors; if a rejection means p.248 and p.278 are wrong, the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | DSKBYTR bits 15, 12 and 7-0, read at $DFF01A; chip revision unknown (the page names none); measured on NanoMig's `paula_floppy` at the pinned revision. Bits 14 and 13 are the green controls; bits 11-8 are not graded |
| **Software impact** | Rob Northen's Copylock, which a Copperline source comment says waits for WORDEQUAL and then reads the sector byte by byte through DSKBYTR; against this RTL it would see `0x00` for every byte and a WORDEQUAL that is never false. Derived from the RTL, not run on NanoMig. NanoMig's README names ADF as its floppy image format (["A default ADF disk image named `df0.adf`"](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/README.md#L121)) |
| **Chapter** | 8 — Interface Hardware, printed pp. 241, 247, 248 (Table 8-7), Appendix A p. 278, Appendix B3 p. 289 · *Amiga Hardware Reference Manual*, 3rd ed., 1991. <details><summary>claim ids by assertion (35)</summary> Claim ids by assertion: `DSKBYT-NOT-CLEARED-ON-READ`: `opus:8-dskbytr.c4`, `opus:8-dskbytr.c7`, `sol:8-dskbytr.c8`, `opus:8-appA-dskbytr.c9`, `sol:8-appa-dskbytr.c10`, pp. 248, 278; `WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH`: `opus:8-dma.c30`, `sol:8-dskdma.c8`, `opus:8-dskbytr.c11`, `sol:8-dskbytr.c15`, `opus:8-dskbytr.c12`, `sol:8-dskbytr.c16`, `opus:8-appA-dskbytr.c13`, `sol:8-appa-dskbytr.c14`, pp. 247, 248, 278; `DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES`: `opus:8-dskbytr.c2`, `opus:8-dskbytr.c14`, `sol:8-dskbytr.c2`, `sol:8-dskbytr.c20`, `opus:8-appA-dskbytr.c6`, `opus:8-appA-dskbytr.c15`, `sol:8-appa-dskbytr.c7`, `sol:8-appa-dskbytr.c16`, pp. 248, 278; `opus:8-dsk.c9` and `sol:8-dsk.c9` (twin ids, independent readers), p. 241; `B4-DSKBYTR-DATA-BYTE-HALF`: `sol:8-reglist.c19`, `sol:8-appa-dskbytr.c4`, `opus:8-appB-dskbytr.c5`, `sol:8-appb-dskbytr.c5`, pp. 241, 278, 289; `B5-DSKBYTR-COMPOUND-BYTE-AND-STATUS`: `opus:8-dsk.c38`, `opus:8-appA-dskbytr.c4`, `sol:8-appa-dskbytr.c6`, pp. 241, 278. Every red assertion carries a claim id. The AA package's two reds carry `opus:AA-dskbytr.c7`, `sol:AA-dskbytr.c7`, `sol:AA-dskbytr.c8` (`DSKBYT-IS-CLEARED-BY-THE-READ`) and `opus:AA-dskbytr.c10`, `sol:AA-dskbytr.c11` (`WORDEQUAL-IS-TRUE-ONLY-WHILE-DSKSYNC-MATCHES`), AA specification PDF p. 23 (the document prints no page numbers; the package's messages cite `[p.22]`) </details> |
| **Module** | `paula_floppy.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L699) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L397); inherited |
| **Test** | `make -C hrm/ch8-dskbytr-status-register test` ([package](../../hrm/ch8-dskbytr-status-register)); `make -C hrm/ch8-disk-controller-overview test` ([package](../../hrm/ch8-disk-controller-overview)); `make -C hrm/ch8-disk-register-map-paula-floppy test` ([package](../../hrm/ch8-disk-register-map-paula-floppy)); AA corroboration `make -C aa/aa-disk-status-bits test`. An expected red exits nonzero |
| **Assertions** | 7 red across three packages. In `hrm/ch8-dskbytr-status-register` (3 passed, 3 failed): `DSKBYT-NOT-CLEARED-ON-READ`, bit 15 expected clear on the second read, observed `read1=0x9000 read2=0x9000`; `WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH`, bit 12 expected 1 at a proved match and 0 through a proved mismatch, observed 1 at both; `DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES`, DATA expected to read the known byte `0xFF`, observed `dskbytr=0xd000`. In `hrm/ch8-disk-controller-overview` (5 passed, 2 failed): `opus:8-dsk.c9` and `sol:8-dsk.c9`, DATA expected to read the known byte `0xFF`, observed DATA `0x0000` (`DSKBYTR=0xD000`). In `hrm/ch8-disk-register-map-paula-floppy` (10 passed, 2 failed): `B4-DSKBYTR-DATA-BYTE-HALF`, DATA expected to read the same known byte, observed DATA `0x0000`; `B5-DSKBYTR-COMPOUND-BYTE-AND-STATUS`, expected the status and byte portions both live, observed the byte portion fails as in B4. The AA package `aa/aa-disk-status-bits` (4 passed, 2 failed) is red on `DSKBYT-IS-CLEARED-BY-THE-READ` and `WORDEQUAL-IS-TRUE-ONLY-WHILE-DSKSYNC-MATCHES` |
| **Verdict of the four** | **unanimous against.** All four clear DSKBYT on the read, drive WORDEQUAL from a live comparator and put a real disk byte in DATA; two of them implement the page's own "as little as two microseconds" window. Hardware: one silicon dissection (8364R7) reads all three as live; no chip was measured here |
| **Effort** | `L` — DATA needs a byte-at-a-time stage that NanoMig's sector-to-FIFO path does not have; WORDEQUAL alone is one term (`sync_match`) and DSKBYT one register cleared by a read strobe, both in `paula_floppy.v` |

---

## Source requirement

**p.248**: *"In read mode, data from the disk is placed into this register one
byte at a time. As each byte is received into the register, the DSKBYT bit is
set true. DSKBYT is cleared when the DSKBYTR register is read."* Table 8-7, the
three fields at issue, and the unused bits:

     15      DSKBYT     When set, indicates that this register contains
                        a valid byte of data (reset by reading this
                        register).
     12      WORDEQUAL  Indicates the  DSKSYNC  register equals the disk
                        input stream.  This bit is true only while the
                        input stream matches the sync register (as
                        little as two microseconds).
     7-0     DATA       Disk byte data.

Bits 11-8 are *"Currently unused; don't depend on read value"*, so a constant
there conforms and is not graded. **p.278**, Appendix A, in independent wording:
bit 15 *"Disk byte ready (reset on read)"*; bit 12 *"This bit true only while
the DSKSYNC register equals the data from disk."*; bits 07-00 *"DATA  Disk byte
data"*. **p.241** lists *"DSKBYTR ($DFF01A) Disk data byte and status read"* and
says *"Special registers allow the CPU to synchronize with specific data, or read
input a byte at a time."* **p.289**, Appendix B3, gives the same cell — `DSKBYTR
*01A R P Disk data byte and status read` — a status portion and a data-byte
portion, which `B5` grades. **p.247**: *"There is a register that may be polled
to examine the disk input stream."*

## RTL and measurement

One purely combinational line, with no state and no read side effect; an
address this module does not decode returns `0x0000`
([`paula_floppy.v#L699`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L699)):

```verilog
assign dskbytr = reg_address_in[8:1]==DSKBYTR[8:1]
               ? {1'b1,(trackrd|trackwr),dsklen[14],5'b1_0000,8'h00} : 16'h00_00;   // :699
```

`sync_match` already exists further down the same file
([`paula_floppy.v:914-915`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L914-L915)),
drives `syncint` and `trackrdok`, and does not drive bit 12:

```verilog
assign sync_match = dsksync[15:0]==floppy_data && fifo_reading_sector && trackrd;
```

Each value is a genuine bus read: the address is held across a
`clk7_en`-qualified rising edge. DATA is graded against a byte known to be current
at the sampling instant, bound through the DSKDATR/FIFO marker path.

| bits | name | source | measured |
|---|---|---|---|
| 15 | DSKBYT | literal `1'b1` | `read1=0x9000 read2=0x9000` |
| 14 | DMAON | `trackrd \| trackwr`, live | `before14=0x9000 after14=0xd000` |
| 13 | DISKWRITE | `dsklen[14]`, live | `before13=0x9000 after13=0xb000` |
| 12 | WORDEQUAL | the `1` of `5'b1_0000` | match trial `bit12=1`; mismatch trial `bit12=1` |
| 11-8 | unused | the `0000` of `5'b1_0000` | not graded |
| 7-0 | DATA | literal `8'h00` | `known_word=0xffff`, `dskbytr=0xd000`, DATA field `0x0000` |

## Risks and unknowns

- Nothing weakens the reading of the page. What weakens the *change* is cost:
  DATA and DSKBYT ask for a byte-level disk model NanoMig deliberately does not
  have, and a core that only moves whole sectors may decide the polled byte
  interface is out of scope. That is a scope decision for the developers, not a
  reading of p.248.
- p.278 defines no reset or idle value for DSKBYTR, so the constant `1` at bit 15
  is falsifiable only through the page's own clearing action. The package grades
  that action and declines the receipt-transition claim rather than inventing a
  precondition the manual does not state.
- The silicon witnesses are one chip revision, 8364R7, and not measured here. The
  6502.org note does not tie its comparator to bit 12 or describe DSKBYT's clear,
  and its author warns of possible errors in that part of the schematic.
- The AA suite reaches two of the three fields, not DATA.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:4571-4593`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4571-L4593) | all three: clears DSKBYT on the read, compares the live shifter for WORDEQUAL, and takes the byte from [`loaddskbytr()`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3989-L3994) |
| **vAmiga** | [`DiskControllerRegs.cpp:106-142`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskControllerRegs.cpp#L106-L142) | all three, and p.248's parenthetical literally: `if (agnus.clock - syncCycle <= USEC(2)) SET_BIT(result, 12);`; `peekDSKBYTR()` clears the bit and DATA is the low half of the same value |
| **Denise** | [`fdc.cpp:211-226`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L211-L226) | all three; the CPU read clears bit 15, WORDEQUAL is a timed window in byte mode and the live comparator otherwise, and the byte loads at [`fdc.cpp:315-316`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L315-L316); a separate `peekDskBytR()` keeps debugger inspection from clearing bit 15 |
| **Copperline** | [`mod.rs:1428-1455`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1428-L1455) | all three, fed by a bit shifter ([`mod.rs:4360-4382`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L4360-L4382)); its comment: *"WORDEQUAL is the comparator's live level: true for the one cell in which the shift register equals DSKSYNC and gone once the next cell shifts in ... Only DSKBYT is reset by the read."* |
| **MiSTer** | [`rtl/paula_floppy.v:397`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L397) | descendant — byte-identical to NanoMig's `:699` |
| **TC64** | [`rtl/minimig/paula_floppy.v:633`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L633) | descendant — the identical line, though the file differs elsewhere by 685 (MiSTer) and 1093 (TC64) lines |

The Copperline comment above `read_dskbytr` ([`mod.rs:1420-1423`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1420-L1423)) names a consequence: *"Rob Northen's
Copylock reads its key sector this way: it waits for WORDEQUAL, then reads the
sector byte by byte through DSKBYTR while counting poll iterations, and rejects
the read unless the first two bytes are the MFM of the sector's first data
byte."*

**The 1991 AA functional specification** prints the same bit table for a different
chip set: a loaded disk byte sets bit 15 and a read resets it; bit 12 is true only
while the incoming disk data equals DSKSYNC. `aa/aa-disk-status-bits` holds $01A
across a qualifying `clk7_en` edge with no drive selected and no motor running:

```
MEASURED: two consecutive bus reads of DSKBYTR return 0xD000 and 0xD000;
          bit 15 is still set after the first read
MEASURED: with DSKSYNC = 0x4489 and no disk data arriving at all,
          WORDEQUAL reads 1
```

Four assertions in the package are green: bit 13 follows DSKLEN's WRITE bit, bit 14
follows its DMA enable, the unused cells read the printed zero, and the word at
$01A changes with DSKLEN. This is corroboration, not a second finding; the
requirement is registered here.

### What other Commodore documents say

- **techtravels, *Inside Paula's Floppy Controller*, part I, 2026 — third-party, silicon**, PDF page 29 — agrees with the manual:

  > "DSKBYTR gives software the most recently received byte and selected status bits. The dissection also confirms that its DSKBYT flag, which is generated in module 10q rather than with the byte latches, is set when a byte is received and cleared when the register is read."
  <!-- corpus: techtravels-paula-floppy/p029.txt -->

  8364R7 silicon: DSKBYT is set per byte and cleared by the read, and DATA is the received byte (B.3, p048: FDC0-FDC15, 'the low eight are the DSKBYTR byte'). p028 adds that the continuously running comparator's 'match output is also visible as the WORDEQUAL status in DSKBYTR', so all three fields are live and none is a constant.

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — agrees with the manual:

  > "The 8 lower outputs of said shift register I had labeled FDC0..FDC7, and that's what you get when reading the DSKBYT register."
  <!-- corpus: paula-8364r7-dissection/10_10_txt.txt -->

  Silicon: DATA reads the live receive shift register, and 10_10 attaches 'a 16 Bit comparator, which scans for the sync word' to it.

## Decision

Three changes of very different size, not one job.

**WORDEQUAL is one term.** Replacing the literal `1` at bit 12 with `sync_match`,
already present at `paula_floppy.v:915`, makes the bit mean what both pages say. The reds it addresses are `WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH` and, in the AA package, `WORDEQUAL-IS-TRUE-ONLY-WHILE-DSKSYNC-MATCHES`; the change is not built or run here.

**DSKBYT needs a register.** The bit must be set when a byte arrives and cleared
by a read of this address, so `dskbytr` stops being a continuous assignment.
`busrd`, the DSKDATR decode at `paula_floppy.v:756`, is the existing read-strobe
pattern in this file. The reds it addresses are `DSKBYT-NOT-CLEARED-ON-READ` and, in the AA package, `DSKBYT-IS-CLEARED-BY-THE-READ`; the change is not built or run here.

**DATA needs a byte-level model this design does not have.** NanoMig moves whole
sectors from SD into a 16-bit FIFO; nothing observes a byte at a time. It is
larger work than the other two and should be costed separately. The reds it addresses are `DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES`, the two DATA reds of `hrm/ch8-disk-controller-overview`, `B4-DSKBYTR-DATA-BYTE-HALF` and `B5-DSKBYTR-COMPOUND-BYTE-AND-STATUS`.
