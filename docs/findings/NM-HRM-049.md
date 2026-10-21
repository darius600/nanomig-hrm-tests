# NM-HRM-049 — the MFM encoder is context-free [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Does p.250's table (`1 -> 01`; `0 -> 00` if following a 1) require NanoMig's payload encoder to set each clock cell from the preceding data bit, where it now emits `11` for a data 1 and `10` for a 0 after a 1? Closes when the payload encoder sets each clock cell from the preceding data bit and `A2-MFM-ONE-ENCODES-01` and `A4-MFM-ZERO-AFTER-ONE-ENCODES-00` turn green, or when a specialist rejects that change and records the constant clock as acceptable. |
| **Report to** | `NanoMig direct` — harbaum/NanoMig |
| **Scope** | The payload path (`data_odd` / `data_even`) of NanoMig's `paula_floppy.v` at the pinned revision, read at DSKDATR pop #33, the first payload word, for uniform fed bytes `0xFF`, `0x00` and `0x80`; chip revision unknown. The header path is read from the RTL, not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed p.250 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims `A1-MFM-DOUBLES-SIZE`: `opus:8-mfm.c1` / `sol:8-mfm.c1`; `A2-MFM-ONE-ENCODES-01`: `opus:8-mfm.c2` / `sol:8-mfm.c2`; `A3-MFM-ZERO-AFTER-ZERO-ENCODES-10`: `opus:8-mfm.c3` / `sol:8-mfm.c3`; `A4-MFM-ZERO-AFTER-ONE-ENCODES-00`: `opus:8-mfm.c4` / `sol:8-mfm.c4`; the `A0` liveness note carries no claim id |
| **Module** | `paula_floppy.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L467-L477); MiSTer and TC64 have no counterpart in RTL; NanoMig's own |
| **Test** | [`hrm/ch8-mfm-cell-encoding`](../../hrm/ch8-mfm-cell-encoding): `make -C hrm/ch8-mfm-cell-encoding test`; an expected red exits nonzero |
| **Assertions** | 2 red of 4 graded (`2 passed, 2 failed`): `A2-MFM-ONE-ENCODES-01` — expected cell `01`, observed `11` (uniform `0xFF`, word `0xFFFF`); `A4-MFM-ZERO-AFTER-ONE-ENCODES-00` — expected `00`, observed `10` (uniform `0x80`, word `0xEAEA`). `A1-MFM-DOUBLES-SIZE` and `A3-MFM-ZERO-AFTER-ZERO-ENCODES-10` are green; A3 is green by coincidence |
| **Verdict of the four** | **unanimous against.** All four implement the page's three rules exactly, and all four carry the previous data bit across the word or byte boundary to do it. Denise prints the manual's table as the comment above its encoder. Hardware: not measured; real Paula does not encode MFM |
| **Effort** | S — the payload path of `paula_floppy.v` only: clock cells from two neighbouring data bits plus one carry register at the word boundary; the header path is a separate job |

---

## Source requirement

**p.250**:

> "The raw MFM data that must be presented to the disk controller will be
> twice as large as the unencoded data. The following table shows the
> relationship:
>
>                1 -> 01
>                0 -> 10   ;if following a 0
>                0 -> 00   ;if following a 1"

> "With clever manipulation, the blitter can be used to encode and decode the
> MFM."

Three rules, and two of them name a condition: a cell's clock bit depends on the
bit before it, which is what stops two flux transitions falling adjacent. The
first rule, `1 -> 01`, carries no condition at all.

## RTL and measurement

The payload path emits a clock bit of `1` for every data bit
([`:467-477`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L467-L477),
`data_odd` and `data_even`: `{1'b1, fd_dma_buf_out[15], 1'b1, fd_dma_buf_out[13], ...}`).
The sector header path above it emits `0` instead
([`mfm_encoder_odd` / `mfm_encoder_even`, :453-462](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L453-L462)).
Neither reads the previous bit, and the file says so twice: *"Clock bits are
always zero just like minimig firmware did"* (:440-441) and *"minimig sets the
clock bits to zero for the header and to one for the sector data"* (:451-452).

| data bit | preceding bit | p.250 | payload path (clock = 1) | header path (clock = 0) |
|---|---|---|---|---|
| 1 | either | `01` | `11` | `01` |
| 0 | 0 | `10` | `10` | `00` |
| 0 | 1 | `00` | `10` | `00` |

| assertion | fed | payload word | cell | page requires |
|---|---|---|---|---|
| `A2-MFM-ONE-ENCODES-01` | uniform `0xFF` | `0xFFFF` | `11` | `01` |
| `A3-MFM-ZERO-AFTER-ZERO-ENCODES-10` | uniform `0x00` | `0xAAAA` | `10` | `10` — **green** |
| `A4-MFM-ZERO-AFTER-ONE-ENCODES-00` | uniform `0x80` | `0xEAEA` | `10` | `00` |

A conforming encoder fed uniform `0xFF` produces `0x5555`; `0xFFFF` has a flux
transition in every cell, which is not legal MFM. A3 is green because a clock
stuck at 1 agrees with the page on exactly the 0-after-0 row; its own message
says a green there does not certify context-sensitivity. It reads the same
payload word through the same two-bit comparator as the two reds, and the same
cell as `A4`, so they are not a stuck check. `0xEAEA` is what the RTL predicts for a `0x8080` buffer word. `A1` counts
512 payload words of `0xAAAA` for uniform `0x00`, which p.250 and this encoder
both give.

## Risks and unknowns

- **The comparison is architectural.** Real Paula does not MFM-encode: p.250
  adds that the blitter can encode and decode it (quoted above). The four witnesses are image-layer encoders that turn stored
  sectors into a cell stream, as NanoMig's RTL does; they are not Paula models.
- **The header column is read from the RTL, not measured.** The package grades
  the payload path only. `mfm_encoder_checksum` forces its clock cell to `1`
  through `| 16'haaaa`, so the file holds three different constant treatments;
  the `16'haaaa` gap, preamble and label defaults are page-legal runs of zeros.
- **The payload word is located from the RTL's own layout**, not from what any
  cell encodes to: DSKDATR pop #33 (`FIRST_PAYLOAD_POP_INDEX` in
  [`test_ch8-mfm-cell-encoding.cpp`](../../hrm/ch8-mfm-cell-encoding/test_ch8-mfm-cell-encoding.cpp)),
  words 0 to 31 being framing (`paula_floppy.v:484-497`). A2 grades the first
  cell of that word (bits [15:14]), whose rule does not depend on the preceding
  bit; A3 and A4 grade the second (bits [13:12]), whose preceding data bit is in
  the same word; no assertion grades a context-dependent cell at a word boundary.
- **Nothing in the normal path notices.** Nothing in AmigaDOS decodes clock
  bits, so the stream reads back correctly through `trackdisk.device`. The case
  for a change rests on software that validates or re-encodes the stream or
  checks bit-cell spacing; none is named and nothing has been observed to break.
- **Hardware not measured**; chip revision unknown.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:2061-2072`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L2061-L2072) | bit arithmetic carried across the word boundary by `lastword`: `uae_u32 lv = (lastword << 16) \| v; uae_u32 nlv = 0x55555555 & ~lv; uae_u32 mfmbits = (nlv << 1) & (nlv >> 1);`. The PC-DOS track encoder repeats the boundary case at [`:2092-2100`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L2092-L2100) |
| **vAmiga** | [`MFM.cpp:89-105`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/rvlib/Images/Encoders/MFM.cpp#L89-L105) | `u8 addClockBits(u8 value, u8 previous)`: the previous byte is a parameter; `rShifted = (value >> 1 \| previous << 7);` |
| **Denise** | [`adf.cpp:181-203`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/adf.cpp#L181-L203) | explicit state machine, `bool dataZeroBefore = false;` carried across the sector's words; `if (dataZeroBefore) word \|= 2 << i;`. The comment above ([`:173-176`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/adf.cpp#L173-L176)) reprints the manual's table |
| **Copperline** | [`mod.rs:4535-4543`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L4535-L4543) | per 32-bit long with an explicit cross-long carry: `encoded \|= ((inv >> 1) \| 0x8000_0000) & (inv << 1);`, then the top clock bit is cleared if the previous long ended on a data 1 |
| **MiSTer** | [`rtl/paula_floppy_fifo.v:23`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy_fifo.v#L23) | descendant — no MFM encoder in RTL; the only mention is the FIFO-size comment `(for 2 MFM-encoded sectors)`. The core takes ready-made MFM words from host firmware, which is not in this tree |
| **TC64** | [`rtl/minimig/paula_floppy_fifo.v:23`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy_fifo.v#L23) | descendant — the identical comment, no encoder in RTL. Its host firmware `SendSector` ([`fw/ctrl_832/fdd.c:56-161`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/fw/ctrl_832/fdd.c#L56-L161)) writes the header with clock bits 0 and the data field with clock bits forced to 1 (`SPI(*p++ >> 1 \| 0xAA);`), *"we do not insert clock bits because they will be stripped by the Amiga software anyway"* |

Four independent implementations, four idioms, one rule: none encodes a cell
without consulting the bit before it. NanoMig is the only one of the three
trees that synthesises a cell stream in RTL.

### What other Commodore documents say

- **techtravels, *Inside Paula's Floppy Controller*, part I, 2026 — third-party, silicon**, PDF page 49 — agrees with the manual:

  > "Each data bit becomes a clock/data pair of channel bits, so the encoded stream contains twice as many bits as the unencoded data: Data bit Recorded pair Condition 1 01 always 0 10 when it follows a 0"
  <!-- corpus: techtravels-paula-floppy/p049.txt -->

  This is a glossary statement of general MFM, not a silicon finding; the paper says Paula does not encode ('Software builds or interprets the recorded format, including encoding', p018). The table continues on p050 with '0 00 when it follows a 1'. p028's '$A1 data → normal MFM $44A9' is an encoding that only the context rule produces.

## Decision

Whether to change is a question about the guest, not about the page. A data 1
encoded as `11` puts two flux transitions in adjacent cells, which no drive
would produce and no real disk carries.

**Suggested change.** Inside a word this is combinational rearrangement, not new
state: `data_odd` emits cells MSB first, so the bit preceding
`fd_dma_buf_out[13]` is `fd_dma_buf_out[15]`, already in the same vector. Every
clock cell but the first follows from two bits of the word, the form WinUAE and
Copperline use: one shift each way and an AND. Only the word boundary needs a
register, one bit holding the last data bit emitted. The odd and even halves of
a sector are 256 words apart in the stream, so the carry must follow emitted
order, not buffer order. The header path is a separate job.

A repaired encoder is measured by the same package unchanged.
