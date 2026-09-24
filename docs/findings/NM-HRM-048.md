# NM-HRM-048 — a documented hardware bug that does not occur here

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red, inverted — see below |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. The manual documents a defect and NanoMig does not reproduce it, so the decision is whether the quirk matters, not whether to fix a fault. |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L865-L882) · no counterpart upstream |
| **Report to** | **nobody, as a defect.** An implementation without the bug is arguably the better hardware; what needs a ruling is whether software that was written around the bug still works |
| **Test** | [`hrm/ch8-dsklen-dma-arming`](../../hrm/ch8-dsklen-dma-arming) — `A11-DSKBUG-LAST-THREE-BITS-LOST-ON-WRITE` |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 1 red. A control assertion drives the identical apparatus with a different payload and reports it correctly in the same run |
| **Verdict of the four** | **3–1, and the one is the interesting one.** Copperline reproduces the bug on purpose, with a named constant for it. WinUAE, vAmiga and Denise all write whole bytes or words and lose nothing — but WinUAE alone emulates the *other* sentence of the same printed paragraph, and calls it a Paula bug in its own comment |

---

## This one runs the other way

Every other finding in this register says the implementation does less than the
manual describes. This one says the opposite: the printed page documents a
hardware **defect**, and NanoMig does not have it. The assertion is red because
the page states the defect unconditionally and the measurement contradicts it —
which is exactly what a conformance test should report — but nothing here should
be changed to make the red go away. The entry exists so that the divergence is on
record, and so that the one question it does raise is asked of the right people.

The substance of this entry is therefore not the RTL. It is what four
independent implementations decided to do about a bug the manual documents.

## What the HRM says

**p.247**, the last paragraph of the DSKLEN section, both sentences:

> *"There is a hardware bug that causes the last three bits of data sent to
> the disk to be lost. Also, the last word in a disk-read DMA operation may
> not come in (that is, one less word may be read than you asked for)."*

The first sentence is unconditional — *causes*, not *may cause*. The second is
explicitly a possibility — *may not come in* — and that difference decides how
each is treated here.

## What NanoMig does

The write path is byte-buffered end to end. A CPU write to DSKDAT goes into the
FIFO; the write state machine strips clock bits into a byte and hands whole bytes
to the sector buffer:

```verilog
// seperate data and ignore clock bits
wire [7:0] dbyte = { fifo_out[14], fifo_out[12], fifo_out[10], fifo_out[8],
                     fifo_out[6],  fifo_out[4],  fifo_out[2], fifo_out[0] };   // :776-777
...
      // trigger dma buffer write
      fd_dma_buffer_trigger_wr <= !fd_dma_buffer_trigger_wr;
      fd_dma_buffer_data_wr <= dbyte;                                          // :875-876
```

and the sector buffer is recombined into whole bytes for the card at
[`paula_floppy.v:228-236`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L228-L236).

**There is no bit-serial shift stage anywhere on the write side** — no place for
the documented timing race to happen in. The design never emits a bit stream
toward a medium at all; it emits sectors toward an SD card.

### Measured

    A11-MEASURED: full write transfer, uniform payload 0xFF,
    sdc_byte_out_data at offset 511 (the last byte of the sector) = 0x00FF
    (upper 5 bits intact=1, low 3 bits lost=0)

The last byte of the sector arrives whole. The page requires the low three bits
to be gone; all eight are there.

The same apparatus, fed a payload of `0x00` instead of `0xFF`, reports `0x0000`
at the same offset in the same run (`A13-DSKDAT-WRITE-BYTE-COMPARATOR-LIVE`) —
so the readback is content-dependent and live, and is not the reason A11 reads
red.

## What the others say

This is the question the entry was written for: each of the four had to decide
whether to emulate a documented hardware bug, and they did not decide the same
way.

| | | |
|---|---|---|
| **Copperline** | [`mod.rs:161-162`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L161-L162) | **reproduces it, deliberately, with a name.** `// Paula's disk write shifter does not emit the final three bits of a write.` / `const DISK_WRITE_LOST_BITS: usize = 3;` |
| **WinUAE** | [`disk.cpp:4062`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4062) | **does not.** The write DMA stores a whole 16-bit word into the track buffer, `drv2->bigmfmbuf[drv2->mfmpos >> 4] = w;`, and nothing trims the tail of the last one |
| **vAmiga** | [`DiskController.cpp:468-472`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskController.cpp#L468-L472) | **does not.** At the end of a write transfer it flushes the FIFO whole — `while (!fifoIsEmpty()) { u8 value = readFifo(); if (drive) drive->write8AndRotate(value); }` — into [`FloppyDrive::write8AndRotate`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L833-L838), which writes eight bits and rotates. A search of `Core/` for the bug turns up nothing |
| **Denise** | [`fdc.cpp:467-533`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L467-L533) | **does not**, and it is the one that could have most easily. `handleFDControllerWrite()` has a genuine bit-serial path for flux and extended images — `disk0.writeBit(state);` — beside the byte path for ADF, and neither drops a tail. A search of `emulation/libami/` for the bug turns up nothing |
| **MiSTer** | [`rtl/paula_floppy.v:234`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L234) | descendant — disk write hands whole FIFO words to host firmware over `IO_DOUT` (`rtl/paula_floppy.v:234`, already cited on this page). **cannot have the bug either.** Neither MiSTer nor TC64 has an in-core bit-serial disk-write stage for the documented race to occur in. |
| **TC64** | [`rtl/minimig/paula_floppy.v:468`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L468) | descendant — the same, over its own host link: `sdo`/`sdi`/`sck` SPI pins to an external flash/SD controller, with whole words handed out through `rtl/minimig/paula_floppy.v:468`, `spi_tx_data_3 = fifo_out;`. Neither line trims a tail of bits on the way out; there is no upstream line in either tree to file this against |

**Copperline's is a decision, not an accident.** The loss is applied where a
write DMA *finishes* ([`finish_write_dma`, `mod.rs:2192-2200`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2192-L2200), passing `lose_tail_bits: true`) and deliberately **not** where a track change
interrupts a write in flight ([`mod.rs:2139-2148`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2139-L2148), passing `false`) —
which is p.247's sentence read precisely: the last three bits *of a write*, not
of every buffer that happens to be emptied.

**WinUAE emulates the paragraph's other sentence.** Its read DMA carries

```cpp
// Paula bug: when dsklength reaches zero, FIFO status is checked too early.
if (disk_fifostatus() < 0 && dsklength == 0) {
    disk_dmafinished();
```

at [`disk.cpp:4284-4286`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4284-L4286) — which is precisely *"one less word may be read
than you asked for"*, modelled with the mechanism that causes it and named a
Paula bug in its own comment. So WinUAE did not decline to emulate documented
Paula bugs in general; it emulated the half its own timing model reaches and not
the half it does not.

**That split is the result.** Two of the four implement one sentence each of the
same printed paragraph, and each implements the sentence its own model has a
mechanism for: Copperline has a write shifter and models the write loss;
WinUAE has cycle-level FIFO status and models the read-side race. vAmiga and
Denise implement neither. No two of the four agree on this paragraph, which is
worth knowing before anyone treats it as settled.

## The other half of the paragraph, and why it is not a red

The package grades only the first sentence. The second is recorded and declined:

> `A12-DSKBUG-ONE-WORD-SHORT-ON-READ` ... declined, not graded. The page says a
> word MAY be lost, not that one IS — "may" is a possibility, and a red requires
> the page to demand something a deterministic trial can violate; this one
> demands nothing.

Measured alongside it and kept as fact rather than finding: the fixture delivers
the full programmed count, sixteen words for sixteen. NanoMig does not exhibit
the possible bug either — which contradicts no page, because no page demanded it.

## Suggested change

**None.** Do not add the bug.

The page describes silicon, not a programmer-visible contract, and a
reimplementation that writes all the bits it was given is doing the better thing.
Adding a three-bit truncation to match a printed defect would be the clearest
possible case of changing the design because a test is red.

What is worth a ruling, and is not a matter for this suite, is the consequence in
the other direction. Period software that padded its writes to work around the
bug wrote trailing bits it expected the hardware to discard. Here those bits
reach the image. In an AmigaDOS sector layout the tail is gap and nothing reads
it; in a custom or copy-protected format it may not be. Whether any real title
depends on those three bits being gone is a question for Amiga specialists, not
one this fixture can answer.

## What holds the finding up

- p.247 states the write-side bug unconditionally — *"causes"* — so it is a
  requirement a deterministic trial can contradict, and this one does.
- The measurement is at the last point the data exists inside the module:
  `sdc_byte_out_data` at sector offset 511, the final byte of a completed write
  transfer.
- The same apparatus, same run, reports a different payload correctly, so the
  readback is live and content-dependent.
- The RTL reason is readable without running anything: there is no bit-serial
  write stage for the race to occur in.
- One of the four implementations reproduces the bug on purpose, which settles
  that it is a real, emulatable behaviour and not a misprint.

## What weakens it

- **The measurement has no medium to be taken at.** p.247 describes bits lost on
  the way to a disk. NanoMig writes sectors to an SD card and has no bit-serial
  disk output at all, so the assertion is graded at the nearest boundary that
  exists rather than at the one the page describes. The finding is sound as
  stated — the last byte of the transfer arrives whole — but it is not the same
  physical question the page is answering.
- Three of the four other implementations agree with NanoMig, and two of those
  three do not mention the bug anywhere in their sources.
- A red here reports a *better* implementation. The register's usual reading —
  red means something to fix — does not apply, and nothing in this entry should
  be quoted as if it did.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **techtravels, *Inside Paula's Floppy Controller*, part I — third-party, silicon**, PDF page 39 — agrees with the manual:

  > "The candidate sequence is specific: the serializer finishes, 10i closes write gate, and delayed transition decisions still in 10v and 10e reach a drive that can no longer record them. This explains a lost tail. The current work has not established why the documented length is exactly three bits."
  <!-- corpus: techtravels-paula-floppy/p039.txt -->

  The paper accepts the documented loss and places a silicon cause: 10i closes write gate while bits are still in the 10v/10e delay chain. It calls this a hypothesis, and says the three-bit length is still open (p043). Its section 6.3 explains the paragraph's other sentence ('may not come in') as a drain race, but the finding does not grade that sentence.

Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; US patent 4,780,844, Commodore-Amiga; US patent 4,829,473, Commodore-Amiga; 6502.org dissection of Paula 8364R7.

