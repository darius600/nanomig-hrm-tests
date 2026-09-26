# NM-HRM-042 — the disk DMA pointer stores 20 word-address bits where two pages print 32 [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_diskdma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L56) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_diskdma.v#L56) |
| **Report to** | **nobody yet** — read *For the specialists* first. A pointer register pair narrower than the address space its caption names is not by itself a defect |
| **Test** | [`hrm/ch8-disk-dma-pointer`](../../hrm/ch8-disk-dma-pointer) — assertion `P11A` |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red of 7 graded. Six greens in the same run use the identical write-and-observe basis |
| **Verdict of the four** | **nobody reaches 32 bits either.** Two keep a 32-bit variable they can never fill; two mask the stored value to 21 bits or fewer |
| **See also** | [NM-HRM-043](NM-HRM-043.md) — the *chipset-conditioned* width of the same register. A different claim on a different page, and the two are not to be merged |

---

## What the HRM says

Two pages give the pointer a 32-bit width.

**Chapter 8, printed p.241**, in the list of registers the disk subsystem uses:

> `DSKPTH     ($DFF020)    DMA pointer (32 bits)`

**Chapter 8, printed p.246**, under *DSKPTH — Pointer to Data*:

> "You specify the 32-bit wide address from which or to which the data is to be
> transferred. The lowest bit of the address must be zero, and the buffer must be
> in Chip memory. The value must be written as a single long word to the DSKPTH
> register ($DFF020)."

Both sentences are about the same register pair and both name 32.

**The manual does not hold that width anywhere else.** Appendix A's own entry for
the pair, printed p.279, calls it *"the 18-bit address of disk DMA data"*, and
appendix B, printed p.289, splits it as *"high 3 bits, 5 bits if ECS"* plus
*"low 15 bits"*. This finding is against the p.241/p.246 wording and only that.
The 18-bit reading and the OCS/ECS conditioning are [NM-HRM-043](NM-HRM-043.md)'s subject.

## What NanoMig does

The pointer is assembled from two register writes into a 20-bit field, and no
path exists for anything wider.

[`src/minimig-aga/agnus_diskdma.v#L18`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L18)
and
[`#L56`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L56):

```verilog
	output	reg [20:1] address_out	//chip address out current disk dma pointer
...
assign address_outnew[20:1] = dma ? address_out[20:1]+1'b1 : {data_in[4:0],data_in[15:1]};
```

Five bits of DSKPTH and fifteen of DSKPTL, concatenated. The port is declared
`[20:1]`, so those twenty bits are the whole of it: no bit beyond position 20
exists to be written, and bit 0 has no storage at all.

### Measured

    P11A: all-ones write to DSKPTH+DSKPTL gives address_out=0xFFFFF,
          representable in 20 bits

> **FAIL: P11A** (`opus:8-dsk.c34` p.241, `opus:8-dma.c7`/`sol:8-dskpth.c1`
> p.246, "32-bit wide address"): the maximum value reachable by writing all-ones
> to DSKPTH+DSKPTL is `0xFFFFF`, representable in 20 bits, not 32 — no bus path
> on this boundary can ever deposit a bit beyond position 20 into the pointer.

Read as the byte address the page is talking about, twenty bits at positions
20:1 reach `$1FFFFE` — two megabytes less two — and no further.

**The control is green on the identical basis.** The fixture's liveness check
(`DSKPTH<=0x00015`, `DSKPTL<=0x02C6A` giving `address_out=0xA9635`, expected
`0xA9635`) and the greens P1, P2 and P5 all measure the same way: write the
register, inspect `address_out`. The red is the width claim failing, not a dark
fixture.

## What the others say

None of the four independent implementations reaches 32 bits of usable
pointer. They divide on *where* the ceiling is imposed — at the register, or
on the road to memory.

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:5208-5216`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L5208-L5216) | <pre>void DSKPTH(uae_u16 v)<br>{<br>&nbsp;&nbsp;dskpt = (dskpt & 0x0000ffff) \| ((uae_u32) v << 16);<br>}<br>void DSKPTL(uae_u16 v)<br>{<br>&nbsp;&nbsp;dskpt = (dskpt & 0xffff0000) \| (v & 0x0000fffe);<br>}</pre> **stores all 32 bits**, unmasked, bit 0 excepted. The ceiling arrives later: disk DMA reads through [`memory.cpp:808-817`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L808-L817), whose first act is `addr &= chipmem_full_mask` |
| **vAmiga** | [`AgnusRegs.cpp:704-718`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L704-L718) | <pre>dskpt = REPLACE_HI_WORD(dskpt, value);<br>if (dskpt & ~agnus.ptrMask) {<br>&nbsp;&nbsp;xfiles("DSKPT %08x out of range\n", dskpt);<br>}</pre> also **stores all 32 bits**. It knows the value can be too wide — `ptrMask` is 19, 20 or 21 bits by Agnus revision ([`Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194)) — and logs it rather than truncating |
| **Denise** | [`emulation/libami/agnus/dma.cpp:96-105`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L96-L105) | <pre>auto Agnus::setDskPtH(uint16_t value) -&gt; void {<br>&nbsp;&nbsp;dskpt &= 0xffff;<br>&nbsp;&nbsp;dskpt \|= value &lt;&lt; 16;<br>&nbsp;&nbsp;dskpt &= dmaChipMemMask;<br>}</pre> **masks the stored value at the write.** `dmaChipMemMask` is the installed chip-RAM mask ([`memory.cpp:693-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L693-L698)), at most 21 bits. `setDskPtL` does not mask, because the low word cannot carry a bit that high |
| **Copperline** | [`src/floppy/mod.rs:1250-1257`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1250-L1257) | <pre>pub fn set_dskpt_high(&mut self, val: u16) {<br>&nbsp;&nbsp;self.dskpt =<br>&nbsp;&nbsp;&nbsp;&nbsp;((self.dskpt & 0x0000_FFFE) \| (((val as u32) & 0x001F) &lt;&lt; 16)) & self.dma_ptr_mask();<br>}</pre> **takes five high bits, exactly as NanoMig does**, then narrows again by Agnus revision. Ceiling `0x001F_FFFE` — the same 21-bit byte address NanoMig's `[20:1]` reaches |
| **MiSTer** | [`rtl/agnus_diskdma.v:56`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_diskdma.v#L56) | descendant — byte-identical to NanoMig: `assign address_outnew[20:1] = dma ? address_out[20:1]+1'b1 : {data_in[4:0],data_in[15:1]};`; the file has no differences from it at all |
| **TC64** | [`rtl/minimig/agnus_diskdma.v:57`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_diskdma.v#L57) | descendant — the same statement, byte for byte (the line moves by one because TC64 adds an unrelated `turbo` input port above it). Both ports remain `[20:1]`; twenty bits is what either tree can ever hold. A fix here, if the page's "32-bit" reading were ever acted on, diverges from neither line |

**NanoMig and Copperline agree on the ceiling**, and arrive at it the same way: a
five-bit high field concatenated with a fifteen-bit low one. WinUAE and vAmiga
hold a 32-bit variable, but neither can *use* a bit above 21 — WinUAE masks the
address on the Agnus DMA path before it leaves, and vAmiga's Agnus bank table
(`agnusMemSrc`, indexed by `(addr >> 16) & 0xFF`,
[`Memory.cpp:833-837`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L833-L837))
resolves anything past the installed banks to `MemSrc::NONE`, which returns the
floating data bus.

### Does any of them narrow by chipset?

Asked here only of the *total* width; the conditioning itself is [NM-HRM-043](NM-HRM-043.md).

| | narrows the disk pointer by chipset? |
|---|---|
| **Copperline** | **yes, at the register.** `dma_addr_capability_mask()` selected by `AgnusRevision`, with a test that walks DSKPT across all three revisions ([`src/bus/tests.rs:2787-2802`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L2787-L2802)) |
| **WinUAE** | **yes, but not at the register.** `} else { chipmem_full_mask = 0x80000 - 1; }` for a non-ECS Agnus ([`memory.cpp:2619-2624`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2624)) — an OCS Agnus sees 19 bits on the DMA path. `cs_agnussize` can override it, and with JIT enabled [`memory.cpp:2646-2648`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2646-L2648) resets the mask to the allocated size |
| **vAmiga** | **no.** The per-revision `ptrMask` reaches an `assert` ([`Memory.cpp:1590-1596`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1590-L1596)), the inspector panel and the `XFILES` log — and nothing that changes a fetched address |
| **Denise** | **no.** `dmaChipMemMask` follows installed chip RAM; the single `ecs()` term in its derivation *widens* it, for the ECS Slow-RAM mirror, and never narrows |

## Suggested change

**None on this claim alone.** Twenty stored bits at positions 20:1 reach two
megabytes, which is every address an Agnus of any revision can drive. A 32-bit
pointer register would be 4 GB of address space on a machine whose largest
chip-RAM configuration is 2 MB. No implementation of the four provides a usable
bit above 21 either, and the two that keep a 32-bit variable do so by not
masking rather than by intending to address that far.

If anything is to change here it is the reading of the page, not the RTL, and
that is not this project's call.

## What holds the finding up

- **Both printed sentences say 32**, in two different sections, and one of them
  spells out the consequence (*"written as a single long word"*).
- **The measurement is exhaustive by construction.** All ones into both halves is
  the largest value the register pair can take; the result is `0xFFFFF`.
- **The port declaration agrees with the measurement.** `address_out` is
  `[20:1]`. This is not a masking behaviour that could be conditioned away — the
  bits do not exist.
- **The control and five further greens share the measurement basis** and pass in
  the same run.

## What weakens it

- **The manual contradicts itself about this width**, and p.241/p.246's "32" is
  the outlier: p.279 says 18, p.289 says 3-or-5 plus 15. A test written from a
  different page would grade the same RTL on a different number. That is why
  this finding is stated as against p.241 and p.246, and why the chipset
  question is filed separately.
- **No implementation of the four is on the page's side.** None offers a
  reachable 32-bit disk pointer.

## For the specialists

The likely reading is that p.241 and p.246 describe how the address is *written*
— one 32-bit long word to $DFF020, which is literally what a `move.l` does — not
how many bits Agnus keeps. On that reading the pages are loose rather than
wrong, and NanoMig implements them correctly.

**Neither page states that reading.** That is why the assertion is graded as
printed. Confirming it would close this finding as documentation rather than as a
defect, and would leave [NM-HRM-043](NM-HRM-043.md) untouched, because that one turns on a width
the manual states twice and conditions on the chipset.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — says something else:

  > "Each of the pointer registers contains an 18-bit address, which points to the location in memory of data to be fetched next for the particular DMA channel."
  <!-- corpus: patent-us4874164-agnus-denise/p016.txt -->

  All 25 DMA pointer registers, the disk channel among them ('the disk and refresh DMA control logic 61 (2 channels)'), hold 18 bits, which address 256K words. p023 says of the same registers 'The pointers in the registers 138 are 19 bits wide'. The patent gives neither the page's 32 nor NanoMig's 20.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 29 — says something else:

  > "This pair of registers contains the 18 bit address of Disk DMA data."
  <!-- corpus: agnus-8371-spec/p029.txt -->

  The disk pointer is 18 bits: a DSKPTH with high 3 bits plus a DSKPTL with low 15 bits, on p029 and in Table 2 (p031). That is neither the 32 of p.241/p.246 nor NanoMig's 20. It matches the manual's own p.279 wording, and p019 has the disk controller reading or writing blocks of up to 128K anywhere in 512K of memory.

Silent on this point: US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988; US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

