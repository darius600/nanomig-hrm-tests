# NM-HRM-049 — the MFM encoder is context-free [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L467-L477) · no counterpart upstream |
| **Report to** | **NanoMig.** There is no ancestor line to file this against |
| **Test** | [`hrm/ch8-mfm-cell-encoding`](../../hrm/ch8-mfm-cell-encoding) — `A2-MFM-ONE-ENCODES-01`, `A4-MFM-ZERO-AFTER-ONE-ENCODES-00` |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red of four. One of the two greens is green by coincidence and says so in its own message |
| **Verdict of the four** | **unanimous against.** All four implement the page's three rules exactly, and all four carry the previous data bit across the word or byte boundary to do it. Denise prints the manual's table verbatim as the comment above its encoder |

---

## What the HRM says

**p.250**: *"The raw MFM data that must be presented to the disk controller will
be twice as large as the unencoded data. The following table shows the
relationship:"*

               1 -> 01
               0 -> 10   ;if following a 0
               0 -> 00   ;if following a 1

Three rules, and two of them name a condition. The scheme is defined by that
condition: a cell's clock bit depends on the bit before it, which is what stops
two flux transitions falling adjacent and is the entire reason MFM doubles the
density of FM.

## What NanoMig does

Two encoders, in the same file, each with a **constant** clock cell — and not
even the same constant.

The payload path emits a clock bit of `1` for every data bit:

```verilog
wire [15:0] data_odd = {
   1'b1, fd_dma_buf_out[15], 1'b1, fd_dma_buf_out[13],
   1'b1, fd_dma_buf_out[11], 1'b1, fd_dma_buf_out[ 9],
   1'b1, fd_dma_buf_out[ 7], 1'b1, fd_dma_buf_out[ 5],
   1'b1, fd_dma_buf_out[ 3], 1'b1, fd_dma_buf_out[ 1] };   // :467-471
```

`data_even` at [`:473-477`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L473-L477) is the same construction on the even bits. The sector
header path immediately above emits a clock bit of `0` instead
([`mfm_encoder_odd` / `mfm_encoder_even`, :453-462](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L453-L462)).

**Neither reads the previous bit.** The file says so itself, twice:

```verilog
// Encode 16 bits into MFM. Clock bits are always zero just like minimig
// firmware did.                                                            // :440-441
...
// for some reason, minimig sets the clock bits to zero for the header and
// to one for the sector data                                               // :451-452
```

Against the page's three rules:

| data bit | preceding bit | p.250 | NanoMig payload path (clock = 1) | NanoMig header path (clock = 0) |
|---|---|---|---|---|
| 1 | either | `01` | `11` | `01` |
| 0 | 0 | `10` | `10` | `00` |
| 0 | 1 | `00` | `10` | `00` |

The payload path gets one row of three right; the header path gets a different
one right. Each is wrong in a different place, and neither can be right in all
three, because the page's table is not a function of the current bit alone.

*The header column is read from the RTL and is not measured — the package grades
the payload path only. It is included because it shows the defect is a choice of
constant rather than a single slip.*

### Measured

| assertion | fed | payload word | cell | page requires |
|---|---|---|---|---|
| `A2-MFM-ONE-ENCODES-01` | uniform `0xFF` | `0xFFFF` | `11` | `01` |
| `A3-MFM-ZERO-AFTER-ZERO-ENCODES-10` | uniform `0x00` | `0xAAAA` | `10` | `10` — **green** |
| `A4-MFM-ZERO-AFTER-ONE-ENCODES-00` | uniform `0x80` | `0xEAEA` | `10` | `00` |

A conforming encoder fed a uniform `0xFF` byte would produce `0x5555`, not
`0xFFFF`; `0xFFFF` is a word in which every single cell is a flux transition,
which is not legal MFM at all.

**A3 is green by coincidence, and its own message says so**: *"matching the page,
but ... for the wrong reason: the clock cell is a hardwired 1 regardless of
context ..., so this row happens to agree with a context-blind encoder. A green
here does not by itself certify context-sensitivity."* A context-free encoder
with the clock stuck at 1 necessarily agrees with the page on exactly the
0-after-0 row and disagrees on the other two. That is the shape the measurement
found.

## What the others say

**First, what is being compared.** Real Paula does not MFM-encode: on an Amiga
the encoding is done by the CPU or the blitter, which is why p.250 prints a table
for a programmer and adds *"With clever manipulation, the blitter can be used to
encode and decode the MFM."* NanoMig stores plain sectors on an SD card and so
must synthesise a legal MFM stream in RTL; the emulators store plain sectors in
an ADF and must synthesise the same stream in their image layer. That layer is
the like-for-like comparison, and it is the one cited below. No emulator's
*Paula* model encodes MFM, because the part does not.

| | | |
|---|---|---|
| **Denise** | [`adf.cpp:181-203`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/adf.cpp#L181-L203) | the page's rule as an explicit state machine, with an explicit carry: `bool dataZeroBefore = false;` across the whole track, and `if (dataZeroBefore) word \|= 2 << i; // in MFM clock bit becomes "one" only, if data bit 0 follows another data bit 0`. The comment directly above the function ([`:173-176`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/adf.cpp#L173-L176)) reprints the manual's table: `1 => 01`, `0 => 10 or 00 (if previous data bit is 1)` |
| **WinUAE** | [`disk.cpp:2061-2072`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L2061-L2072) | the same rule as bit arithmetic, carried across the word boundary by `lastword`: `uae_u32 lv = (lastword << 16) \| v; uae_u32 nlv = 0x55555555 & ~lv; uae_u32 mfmbits = (nlv << 1) & (nlv >> 1);` — a clock bit is set only where the data bit *and* the data bit before it are both zero. The header encoder repeats the boundary case by hand at [`:2092-2100`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L2092-L2100) |
| **vAmiga** | [`MFM.cpp:89-105`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/rvlib/Images/Encoders/MFM.cpp#L89-L105) | `u8 addClockBits(u8 value, u8 previous)` — the previous byte is a parameter, so the context cannot be dropped by accident. `rShifted = (value >> 1 \| previous << 7);` supplies the bit before the first one in the byte |
| **Copperline** | [`mod.rs:4535-4543`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L4535-L4543) | the same rule per 32-bit long, with the cross-long carry written out: `encoded \|= ((inv >> 1) \| 0x8000_0000) & (inv << 1);` then `if dest.last().copied().unwrap_or(0) & 1 != 0 { encoded &= 0x7FFF_FFFF; }` — the top clock bit of a long is cleared when the previous long ended on a data 1 |
| **MiSTer** | [`rtl/paula_floppy_fifo.v:23`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy_fifo.v#L23) | descendant — **no counterpart: neither pinned Minimig tree contains an MFM encoder.** The only mention of MFM here is the FIFO-size comment at this line, `reg [15:0] mem [2047:0]; // 2048 words by 16 bit wide fifo memory (for 2 MFM-encoded sectors)`. This line leaves MFM encoding to the CPU or blitter, as real Paula does |
| **TC64** | [`rtl/minimig/paula_floppy_fifo.v:23`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy_fifo.v#L23) | descendant — the identical comment at the identical line. Both lines leave MFM encoding to the CPU or blitter; **NanoMig is the only one of the three trees that synthesises a cell stream in RTL**, and so the only one that has to get the context-sensitivity right. This defect belongs to NanoMig alone, with no ancestor line to file it against |

**Four independent implementations, four different idioms, one rule.** A state
machine with a boolean, 32-bit mask arithmetic, a function whose signature
takes the previous byte, and per-long arithmetic with an explicit carry. Not
one of them encodes a cell without consulting the bit before it, and two of
them reprint the manual's table in their own comments while doing it. There is
no disagreement to report here among the four — which is itself worth saying,
because on most findings in this register there is. Both maintained Minimig
lines sidestep the question entirely: with no encoder in either, there is
nothing for them to get right or wrong, and no disagreement to have.

## The package used to require the defect it reports

This belongs in the record. Before repair lot F (independent validation,
2026-09-15) two of this package's four graded assertions were built on `0xFF ->
0xFFFF` — a value that only *this* encoder produces, because p.250's `1 -> 01` is
unconditional and a uniform `0xFF` byte page-encodes to `0x5555`. A conforming
encoder would have made both of them fail.

| | before | after |
|---|---|---|
| `A0` fixture liveness | graded through `expect()` on `0xFF -> 0xFFFF` | demoted to `note()`. It carries no claim uid, so it was never conformance evidence and removing the grade cost no coverage |
| `A1-MFM-DOUBLES-SIZE` | counted occurrences of `0xFFFF` across the whole drained stream | counts `0xAAAA` within the known payload index range. `0x00` page-encodes to `0xAAAA` under p.250's own `0 -> 10 ;if following a 0`, and under this RTL's context-free encoder as well, so the count is correct against either |

A test suite that encodes the defect it reports cannot report it: on the day the
encoder is fixed, the old `A1` would have counted zero and announced a doubling
defect that was not there.

**One dependency survives the repair, and whoever fixes the RTL needs to know
about it.** The helper that locates the payload word,
`find_ff_marker_pop_index()` ([`test_ch8-mfm-cell-encoding.cpp:326-339`](../../hrm/ch8-mfm-cell-encoding/test_ch8-mfm-cell-encoding.cpp)), still
finds it by searching for exactly `0xFFFF` in a uniform-`0xFF` sector, and `A2`,
`A3` and `A4` all call it. No graded assertion requires the defect any more, but
that calibration does: against a conforming encoder the search returns `-1` and
the package stops locating anything. **A repair to the encoder must be made
together with a new calibration marker** — one that is page-legal under both
schemes, as `A1`'s `0x00 -> 0xAAAA` already is.

## Suggested change

**Inside a word this is combinational rearrangement, not new state.** Within
`data_odd` the cells are emitted MSB first, so the bit preceding
`fd_dma_buf_out[13]` in the stream is `fd_dma_buf_out[15]`, which is already in
the same vector. Every clock cell but the first can be computed from two bits of
the word it is already built from — the form WinUAE and Copperline use, one shift
each way and an AND.

**Only the word boundary needs a register**: one bit holding the last data bit of
the word just emitted, to supply the context for the first cell of the next. The
odd and even halves of a sector are 256 words apart in the stream, so the carry
must follow the emitted order, not the buffer order.

**The header path is a separate job.** `mfm_encoder_odd` / `mfm_encoder_even`
hold the clock cell at `0`, while `mfm_encoder_checksum` forces it to `1`
through its `| 16'haaaa` — so the file contains three different constant
treatments, not one. The gap, preamble and label defaults of `16'haaaa` are
already page-legal, being a run of zeros. *(Read from the RTL; only the payload
path is measured.)*

Whether the change is worth making is a question about the guest, not about the
page. Nothing in AmigaDOS decodes clock bits — `trackdisk.device` masks them off
— so this stream reads back correctly through the normal path. What it is not is
**legal MFM**: a data 1 encoded as `11` puts two flux transitions in adjacent
cells, which no drive would produce and no real disk carries. Software that
validates the cell stream, re-encodes it, or checks bit-cell spacing sees
something that could not come off a disk.

## What holds the finding up

- p.250's first rule, `1 -> 01`, carries no condition at all: a data 1 encodes as
  `01` unconditionally, and this encoder never produces `01` on the payload path.
- Both reds come from the same measured word, read at a payload index calibrated
  on that same build in that same run, not from a hard-coded offset.
- `A3` passes on the identical mechanism — same payload-word search, same
  `bits[13:12]` extraction, same two-bit comparator — which is what stops the two
  reds being a stuck check, and its own message says why its green proves less
  than it looks.
- The `0xEAEA` that `A4` measures is exactly what the RTL predicts for a uniform
  `0x80` sector: `{1,b15, 1,b13, ...}` over a `0x8080` buffer word gives
  `1110_1010_1110_1010`. The measurement and the source agree bit for bit.
- The RTL says so itself, in its own comments, twice.
- All four other implementations implement all three rules, in four different
  idioms, and two reprint the manual's table while doing it.

## What weakens it

- **The comparison is architectural, not part-for-part.** Real Paula does not
  encode MFM, so the four citations above are to image-layer encoders, not to
  Paula models. They are the right comparison for what NanoMig's RTL is doing —
  turning stored sectors into a cell stream — but nobody should read the verdict
  as "four Paula implementations disagree with NanoMig's Paula".
- **Nothing in the normal path notices.** The guest masks clock bits away, so
  for AmigaDOS-shaped use the defect is invisible. The case for fixing it rests
  on software that inspects the stream rather than on anything that has been
  observed to break.
- The package's payload-word calibration still depends on the defect, as recorded
  above. That does not weaken the finding — no graded assertion relies on it any
  more — but it does mean the measurement and the repair have to move together.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **techtravels, *Inside Paula's Floppy Controller*, part I, 2026 — third-party, silicon**, PDF page 49 — agrees with the manual:

  > "Each data bit becomes a clock/data pair of channel bits, so the encoded stream contains twice as many bits as the unencoded data: Data bit Recorded pair Condition 1 01 always 0 10 when it follows a 0"
  <!-- corpus: techtravels-paula-floppy/p049.txt -->

  This is a glossary statement of general MFM, not a silicon finding; the paper says Paula does not encode ('Software builds or interprets the recorded format, including encoding', p018). The table continues on p050 with '0 00 when it follows a 1'. p028's '$A1 data → normal MFM $44A9' is an encoding that only the context rule produces.

Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

