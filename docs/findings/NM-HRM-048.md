# NM-HRM-048 — a documented hardware bug that does not occur here [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails because NanoMig lacks a documented bug |
| **What is asked of a reader** | Should NanoMig reproduce the loss of the last three bits of a disk write that p.247 documents, when its write path delivers all eight bits of the last sector byte (A11 observed `0x00FF`)? *Decision* says no and asks for no RTL change. Closes when an Amiga specialist rejects the change, recording that no software depends on those bits being discarded, or when the write path drops them and `A11-DSKBUG-LAST-THREE-BITS-LOST-ON-WRITE` turns green. |
| **Report to** | `NanoMig direct` — harbaum/NanoMig |
| **Scope** | NanoMig's `paula_floppy.v` write path at the pinned revision: one full write transfer, uniform payload `0xFF`, read at `sdc_byte_out_data` offset 511; Paula chip revision unknown; real-hardware behaviour not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed p.247, the last paragraph of the DSKLEN section · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `A11-DSKBUG-LAST-THREE-BITS-LOST-ON-WRITE` carries `opus:8-dma.c32` / `sol:8-dskbug.c1`; `A12-DSKBUG-ONE-WORD-SHORT-ON-READ` (declined) carries `opus:8-dma.c33` / `sol:8-dskbug.c2`; `A13-DSKDAT-WRITE-BYTE-COMPARATOR-LIVE` carries no claim id |
| **Module** | `paula_floppy.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L865-L882); MiSTer and TC64 have no counterpart of this write path; NanoMig's own |
| **Test** | [`hrm/ch8-dsklen-dma-arming`](../../hrm/ch8-dsklen-dma-arming): `make -C hrm/ch8-dsklen-dma-arming test`; the expected red exits nonzero |
| **Assertions** | 1 red: `A11-DSKBUG-LAST-THREE-BITS-LOST-ON-WRITE` — expected the last three bits of the last byte lost (`0xFF` written must not read back as `0xFF`), observed `0x00FF`, all eight bits intact; the package reports `10 passed, 1 failed`. The control `A13-DSKDAT-WRITE-BYTE-COMPARATOR-LIVE` drives the identical apparatus with payload `0x00` and reads `0x0000` in the same run |
| **Verdict of the four** | **3–1, and the one is the interesting one.** Copperline reproduces the bug on purpose, with a named constant for it, in images that keep raw MFM cells. WinUAE, vAmiga and Denise all write the last word whole and lose nothing — but WinUAE and Denise both emulate the *other* sentence of the same printed paragraph, and WinUAE calls it a Paula bug in its own comment. Hardware: documented defect, not measured here |
| **Effort** | none — the page suggests no RTL change |

---

## Source requirement

**p.247**, the last paragraph of the DSKLEN section, both sentences:

> *"There is a hardware bug that causes the last three bits of data sent to
> the disk to be lost. Also, the last word in a disk-read DMA operation may
> not come in (that is, one less word may be read than you asked for)."*

The first sentence is unconditional (*causes*); the second is explicitly a
possibility (*may not come in*). That difference decides how each is graded.

## RTL and measurement

The write path is byte-buffered end to end. A write to DSKDAT during write DMA
goes into the FIFO, the write state machine strips the clock bits into a byte
([`:776-777`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L776-L777))
and hands whole bytes to the sector buffer
([`:875-876`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L875-L876)),
which is recombined into whole bytes for the card at
[`:228-236`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L228-L236).
There is no bit-serial shift stage on the write side for the documented timing
race to happen in; the design emits sectors toward an SD card, not a bit stream
toward a medium.

    A11-MEASURED: full write transfer, uniform payload 0xFF,
    sdc_byte_out_data at offset 511 (the last byte of the sector) = 0x00FF
    (upper 5 bits intact=1, low 3 bits lost=0)

`A11` is graded and red: the last byte arrives whole. `A13` runs the same
apparatus on payload `0x00` and reports `0x0000` at the same offset, so the
readback is live and content-dependent and is not the reason `A11` is red.

The second sentence is recorded, not graded: `A12-DSKBUG-ONE-WORD-SHORT-ON-READ`
is declined because the page says a word *may* be lost, and a red requires the
page to demand something a deterministic trial can violate. Measured alongside
as fact, not finding: the fixture delivers the full programmed count, sixteen
words for sixteen.

## Risks and unknowns

- **The measurement has no medium to be taken at.** p.247 describes bits lost
  on the way to a disk. NanoMig writes sectors to an SD card and has no
  bit-serial disk output, so `A11` is graded at the nearest boundary that
  exists, not the one the page describes: the last byte of the transfer arrives
  whole, but that is not the same physical question.
- **A red here reports a *better* implementation.** The register's usual
  reading, that red means something to fix, does not apply, and nothing on
  this page should be quoted as if it did.
- Three of the four other implementations agree with NanoMig, and two of those
  three (vAmiga, Denise) do not mention the bug anywhere in their sources.
- Period software that padded its writes to work around the bug wrote trailing
  bits it expected the hardware to discard; here they reach the image. In an
  AmigaDOS sector layout the tail is gap and nothing reads it; in a custom or
  copy-protected format it may not be. Whether any real title depends on those
  bits being gone is unknown, and this fixture cannot answer it.

## Independent evidence

| | | |
|---|---|---|
| **WinUAE** | [`disk.cpp:4062`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4062) | **does not.** The write DMA stores a whole 16-bit word into the track buffer, `drv2->bigmfmbuf[drv2->mfmpos >> 4] = w;`, and nothing trims the tail of the last one. It does emulate the paragraph's other sentence: its read DMA carries `// Paula bug: when dsklength reaches zero, FIFO status is checked too early.` at [`disk.cpp:4284-4286`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4284-L4286), which is *"one less word may be read than you asked for"* |
| **vAmiga** | [`DiskController.cpp:468-472`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskController.cpp#L468-L472) | **does not.** At the end of a write transfer it flushes the FIFO whole into [`FloppyDrive::write8AndRotate`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L833-L838), which writes eight bits and rotates. It implements neither sentence |
| **Denise** | [`fdc.cpp:467-533`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L467-L533) | **does not**, though it has a genuine bit-serial path for flux and extended images (`disk0.writeBit(state);`) beside the byte path for ADF, and neither drops a tail. It emulates the read-side sentence without naming it: when the length reaches zero with the FIFO empty, the transfer ends without queueing the last word, the same condition as WinUAE's ([`fdc.cpp:402-410`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L402-L410); the byte-mode path repeats it at [`:348-356`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L348-L356)) |
| **Copperline** | [`mod.rs:161-162`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L161-L162) | **reproduces it, deliberately, with a name.** `// Paula's disk write shifter does not emit the final three bits of a write.` / `const DISK_WRITE_LOST_BITS: usize = 3;`, applied where the image keeps raw MFM cells. It is applied where a write DMA finishes ([`mod.rs:2192-2200`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2192-L2200), `lose_tail_bits: true`) and not where a track change interrupts one ([`mod.rs:2139-2148`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2139-L2148), `false`). A standard ADF, which stores sectors as NanoMig's SD image does, is decoded into sectors and never consults the flag ([`mod.rs:2315-2325`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2315-L2325)) |
| **MiSTer** | [`rtl/paula_floppy.v:234`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L234) | descendant: disk write hands whole FIFO words to host firmware over `IO_DOUT`; it has no in-core bit-serial disk-write stage for the race to occur in |
| **TC64** | [`rtl/minimig/paula_floppy.v:468`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L468) | descendant: the same over its own host link, `sdo`/`sdi`/`sck` SPI pins to the control CPU, whole words handed out through `spi_tx_data_3 = fifo_out;`; neither line trims a tail of bits |

Copperline's loss follows p.247 precisely: the last three bits *of a write*,
not of every buffer that happens to be emptied. WinUAE and Denise model the
read-side race and Copperline the write-side loss, each where its own model has
a mechanism; only WinUAE and Denise agree on this paragraph.

### What other Commodore documents say

- **techtravels, *Inside Paula's Floppy Controller*, part I, 2026 — third-party, silicon**, PDF page 39 — agrees with the manual:

  > "The candidate sequence is specific: the serializer finishes, 10i closes write gate, and delayed transition decisions still in 10v and 10e reach a drive that can no longer record them. This explains a lost tail. The current work has not established why the documented length is exactly three bits."
  <!-- corpus: techtravels-paula-floppy/p039.txt -->

  The paper accepts the documented loss and places a silicon cause: 10i closes write gate while bits are still in the 10v/10e delay chain. It calls this a hypothesis, and says the three-bit length is still open (p043). Its section 6.3 explains the paragraph's other sentence ('may not come in') as a drain race, but the finding does not grade that sentence.

## Decision

**No change.** Do not add the bug. The page describes silicon, not a
programmer-visible contract, and a reimplementation that writes all the bits it
was given is doing the better thing; adding a three-bit truncation to match a
printed defect would be changing the design because a test is red.

What is worth a ruling, and is not a matter for this suite, is whether software
written around the bug still works with the bits now reaching the image (see
*Risks and unknowns*).
