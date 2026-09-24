# NM-HRM-041 — DSKBYTR is a constant word in three of its five fields [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L699) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L397) |
| **Report to** | **Minimig-AGA and NanoMig.** All four other implementations implement all three fields |
| **Test** | [`hrm/ch8-dskbytr-status-register`](../../hrm/ch8-dskbytr-status-register), [`hrm/ch8-disk-controller-overview`](../../hrm/ch8-disk-controller-overview), [`hrm/ch8-disk-register-map-paula-floppy`](../../hrm/ch8-disk-register-map-paula-floppy) |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 7 red across three packages, from four printed pages. DISKWRITE and DMAON pass through the identical read cycle in the same run |
| **Verdict of the four** | **unanimous against.** All four clear DSKBYT on the read, drive WORDEQUAL from a live comparator and put a real disk byte in DATA; two of them implement the page's own "as little as two microseconds" window |

---

## Why this is one finding and not seven

Seven red assertions in three packages, citing four printed pages, all land on
one continuous assignment. Grouping by cause rather than by symptom follows the
precedent NM-HRM-040 set.

| package | assertion | pages |
|---|---|---|
| `ch8-dskbytr-status-register` | `DSKBYT-NOT-CLEARED-ON-READ` | p.248, p.278 |
| `ch8-dskbytr-status-register` | `WORDEQUAL-DOES-NOT-TRACK-DSKSYNC-MATCH` | p.247, p.248, p.278 |
| `ch8-dskbytr-status-register` | `DATA-FIELD-DOES-NOT-REFLECT-INCOMING-BYTES` | p.248, p.278 |
| `ch8-disk-controller-overview` | `opus:8-dsk.c9`, `sol:8-dsk.c9` — twin uids, independent readers | p.241 |
| `ch8-disk-register-map-paula-floppy` | `B4-DSKBYTR-DATA-BYTE-HALF` | p.241, p.278, p.289 |
| `ch8-disk-register-map-paula-floppy` | `B5-DSKBYTR-COMPOUND-BYTE-AND-STATUS` | p.241, p.278 |

## What the HRM says

**p.248**, the body text: *"This register is the disk-microprocessor data buffer.
In read mode, data from the disk is placed into this register one byte at a time.
As each byte is received into the register, the DSKBYT bit is set true. DSKBYT is
cleared when the DSKBYTR register is read."*

**p.248**, Table 8-7, the three fields at issue:

     15      DSKBYT     When set, indicates that this register contains
                        a valid byte of data (reset by reading this
                        register).
     12      WORDEQUAL  Indicates the  DSKSYNC  register equals the disk
                        input stream.  This bit is true only while the
                        input stream matches the sync register (as
                        little as two microseconds).
     7-0     DATA       Disk byte data.

**p.278**, Appendix A, in independent wording: bit 15 *"Disk byte ready (reset on
read)"*; bit 12 *"This bit true only while the DSKSYNC register equals the data
from disk."*; bits 07-00 *"DATA  Disk byte data"*.

**p.241** states the same register as a capability of the controller: *"Special
registers allow the CPU to synchronize with specific data, or read input a byte
at a time."*, and lists *"DSKBYTR ($DFF01A) Disk data byte and status read"*.

**p.289**, Appendix B3, gives the same function cell — `DSKBYTR *01A R P Disk
data byte and status read` — a conjunction of a status portion and a data-byte
portion, which is what the `B5` assertion grades.

**p.247** supplies the mechanism WORDEQUAL exists to serve: *"the disk hardware
supports a limited set of sector-searching facilities. There is a register that
may be polled to examine the disk input stream."*

## What NanoMig does

One purely combinational line, with no state and no read side effect anywhere
near it:

```verilog
//disk data byte and status read
assign dskbytr = reg_address_in[8:1]==DSKBYTR[8:1]
               ? {1'b1,(trackrd|trackwr),dsklen[14],5'b1_0000,8'h00} : 16'h00_00;   // :699
```

Field by field:

| bits | name | source | |
|---|---|---|---|
| 15 | DSKBYT | `1'b1` | **literal** |
| 14 | DMAON | `trackrd \| trackwr` | live |
| 13 | DISKWRITE | `dsklen[14]` | live |
| 12 | WORDEQUAL | the `1` of `5'b1_0000` | **literal** |
| 11-8 | *(unused)* | the `0000` of `5'b1_0000` | literal, and the page permits it |
| 7-0 | DATA | `8'h00` | **literal** |

Four fields are constants, but only three of them are findings. p.248 says of
bits 11-8 *"Currently unused; don't depend on read value"*, so a constant there
conforms; the package records that and declines to grade it rather than requiring
a value the manual never states.

For one of the three, nothing is missing from the design. `sync_match` already
exists as a combinational wire further down the same file, at
[`paula_floppy.v:914-915`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L914-L915):

```verilog
wire sync_match;
assign sync_match = dsksync[15:0]==floppy_data && fifo_reading_sector && trackrd;
```

It drives `syncint` and `trackrdok`. It does not drive bit 12.

### Measured

Every value below comes from a genuine bus read: the address is held across a
real `clk7_en`-qualified rising edge, not sampled from a combinational `eval()`.

| assertion | what the page requires | measured |
|---|---|---|
| DSKBYT cleared by a read | bit 15 clear on the second of two consecutive read cycles | `read1=0x9000 read2=0x9000` |
| WORDEQUAL tracks the comparator | 1 at a proved match, 0 through a proved sustained mismatch | match trial `bit12=1`; mismatch trial `bit12=1` |
| DATA carries the incoming byte | the byte known to be current at the sampling instant | `known_word=0xffff`, `dskbytr=0xd000`, DATA field `0x0000` |

The known byte is bound to the sampling instant through the DSKDATR/FIFO marker
path, calibrated in the same run, rather than compared between two unbound
snapshots — a conforming register is under no obligation to make two arbitrary
samples of a periodic stream differ.

## What the others say

| | | |
|---|---|---|
| **WinUAE** | [`disk.cpp:4571-4593`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4571-L4593) | all three. `v = dskbytr_val; dskbytr_val &= ~0x8000;` clears DSKBYT on the read; `if (word == dsksync) v \|= 0x1000;` compares the live shifter; the byte arrives from [`loaddskbytr()`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3989-L3994) as `dskbytr_val = word & 0xff; dskbytr_val \|= 0x8000;` |
| **vAmiga** | [`DiskControllerRegs.cpp:106-142`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskControllerRegs.cpp#L106-L142) | all three, and p.248's parenthetical literally: `if (agnus.clock - syncCycle <= USEC(2)) SET_BIT(result, 12);`. The clearing sits in `peekDSKBYTR()` — `incoming &= 0x7FFF;`, under the comment *"Clear the DSKBYT bit, so it won't show up in the next read"* — and DATA is the low half of that same `incoming` |
| **Denise** | [`fdc.cpp:211-226`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L211-L226) | all three, in two forms. `getDskBytR()`, the CPU read, does `dskBytr &= ~0x8000;`; WORDEQUAL is a timed window in byte mode and the live comparator `dskShifter == dskSync` otherwise. The byte loads at [`fdc.cpp:315-316`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L315-L316): `if ((dskShifterPos & 7) == 7) dskBytr = (dskShifter & 0xff) \| 0x8000;`. A separate `peekDskBytR()` exists so that debugger inspection does not clear bit 15 — the side effect is modelled precisely enough to need an exemption |
| **Copperline** | [`mod.rs:1428-1455`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1428-L1455) | all three, fed by a real bit shifter at [`mod.rs:4360-4382`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L4360-L4382). Its own comment states the rule this finding measures: *"WORDEQUAL is the comparator's live level: true for the one cell in which the shift register equals DSKSYNC and gone once the next cell shifts in ... Only DSKBYT is reset by the read."* |
| **MiSTer** | [`rtl/paula_floppy.v:397`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L397) | descendant — byte-identical to NanoMig's `:699`: `assign dskbytr = reg_address_in[8:1]==DSKBYTR[8:1] ? {1'b1,(trackrd\|trackwr),dsklen[14],5'b1_0000,8'h00} : 16'h00_00;` |
| **TC64** | [`rtl/minimig/paula_floppy.v:633`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L633) | descendant — the identical line, even though the surrounding file diverges by 809 (MiSTer) and 1243 (TC64) lines elsewhere. All three constants graded here — DSKBYT, WORDEQUAL, DATA — are inherited unchanged; a fix belongs to all three lines equally, matching the "Report to: Minimig-AGA and NanoMig" already on this page |

**Not one of the four independent implementations treats any of these three
fields as a constant**, and this is the unusual case where they do not split
at all. Two of them go past the table and implement the parenthetical *"as
little as two microseconds"* as a timed window rather than a level. All three
constants graded here are inherited unchanged by both maintained Minimig
lines, so — as descendants — they add nothing to what the four independent
implementations already say about the manual, and confirm only that no fix
has travelled to either ancestor yet.

One corroboration worth passing on, because it names a concrete consequence
rather than appealing to the page again. Copperline records, in the comment above
`read_dskbytr`, a title that reads the register exactly this way: *"Rob Northen's
Copylock reads its key sector this way: it waits for WORDEQUAL, then reads the
sector byte by byte through DSKBYTR while counting poll iterations, and rejects
the read unless the first two bytes are the MFM of the sector's first data
byte."* Against this RTL such a loop sees `0x00` for every byte and a WORDEQUAL
that is never false.

## A second document says the same thing

**The 1991 AA functional specification reaches two of these three fields
independently**, and the AA suite's own package for DSKBYTR goes red on both.
The AA entry prints the same bit table five years later, for a different chip
set, and says of bit 15 that a loaded disk byte sets it and that reading the
register resets it, and of bit 12 that it is true only while the incoming disk
data equals DSKSYNC.

`aa/aa-disk-status-bits` arms DSKLEN, then holds $01A across a qualifying
`clk7_en` edge -- a bus cycle a synchronous clear could react to, not a
combinational peek:

```
MEASURED: two consecutive bus reads of DSKBYTR return 0xD000 and 0xD000;
          bit 15 is still set after the first read
MEASURED: with DSKSYNC = 0x4489 and no disk data arriving at all,
          WORDEQUAL reads 1
```

No drive is selected, no motor is running, and nothing is arriving that could
equal the sync word. Both bits are the literal one in the module's single
continuous assignment.

Four assertions in the same package are green, which is what makes these two a
reading of those fields rather than of a dead fixture: bit 13 follows DSKLEN's
WRITE bit, bit 14 rises with DSKLEN's DMA enable, the four cells the table
leaves unused read as the printed zero, and the word at $01A changes when
DSKLEN changes. Both red baselines invert under `MUTATE=1` and go green, so
the comparison can see a difference when there is one.

**This is corroboration, not a second finding.** The AA suite files its own
keys as NM-AA-nnn; this one is not among them, because the requirement is
already registered here.

## Suggested change

Three changes of very different size, which should not be treated as one job.

**WORDEQUAL is one term.** Replacing the literal `1` at bit 12 with `sync_match`,
already present at `paula_floppy.v:915`, makes the bit mean what both pages say
it means. Nothing else in the module has to move.

**DSKBYT needs a register.** The bit must be set when a byte arrives and cleared
by a read of this address, which means `dskbytr` stops being a continuous
assignment. `busrd`, the DSKDATR decode at `paula_floppy.v:756`, is the existing
pattern for a read strobe in this file.

**DATA needs a byte-level model this design does not have.** NanoMig's disk path
moves whole sectors from SD into a 16-bit FIFO; there is no byte-at-a-time stage
for DSKBYTR's DATA field to observe. Implementing it honestly is larger work than
the other two and should be costed separately.

## What holds the finding up

- Four printed pages state the three fields, each twice in independent wording —
  chapter 8 and Appendix A — and not one of the three is hedged.
- DISKWRITE (bit 13) and DMAON (bit 14) move correctly through the **identical**
  read cycle in the same run: `before13=0x9000 after13=0xb000`,
  `before14=0x9000 after14=0xd000`. The read path is not the problem.
- Reading an address this module does not decode returns `0x0000`, so the mux
  discriminates by address as well as by state.
- The three literals are readable in the source without running anything.
- All four other implementations implement all three.

## What weakens it

- Nothing weakens the reading of the page. What weakens the *change* is cost:
  DATA and DSKBYT ask for a byte-level disk model NanoMig deliberately does not
  have, and a core that only ever moves whole sectors may reasonably decide the
  polled byte interface is out of scope. That is a scope decision rather than a
  reading of p.248, and it belongs to the developers.
- p.278 defines no reset or idle value for DSKBYTR at all, so the constant `1` at
  bit 15 is falsifiable only through the page's own clearing action. The package
  grades that action and declines the receipt-transition claim rather than
  inventing a precondition the manual does not state.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **techtravels, *Inside Paula's Floppy Controller*, part I, 2026 — third-party, silicon**, PDF page 29 — agrees with the manual:

  > "DSKBYTR gives software the most recently received byte and selected status bits. The dissection also confirms that its DSKBYT flag, which is generated in module 10q rather than with the byte latches, is set when a byte is received and cleared when the register is read."
  <!-- corpus: techtravels-paula-floppy/p029.txt -->

  8364R7 silicon: DSKBYT is set per byte and cleared by the read, and DATA is the received byte (B.3, p048: FDC0-FDC15, 'the low eight are the DSKBYTR byte'). p028 adds that the continuously running comparator's 'match output is also visible as the WORDEQUAL status in DSKBYTR', so all three fields are live and none is a constant.

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — agrees with the manual:

  > "The 8 lower outputs of said shift register I had labeled FDC0..FDC7, and that's what you get when reading the DSKBYT register."
  <!-- corpus: paula-8364r7-dissection/10_10_txt.txt -->

  Silicon: DATA reads the live receive shift register, and 10_10 attaches 'a 16 Bit comparator, which scans for the sync word' to it. The note does not tie that comparator to bit 12 or describe DSKBYT's clear, and the author warns 'it could be that there are errors in this part of my schematic.'

Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989.

