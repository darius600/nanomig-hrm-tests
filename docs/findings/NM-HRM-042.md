# NM-HRM-042 — the disk DMA pointer stores 20 word-address bits where two pages print 32 [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should the disk DMA pointer pair DSKPTH/DSKPTL keep the 20 word-address bits `address_out[20:1]` (an all-ones write reads back `0xFFFFF`) although printed p.241 and p.246 call it 32 bits? The likely answer is yes, because the two pages describe the long-word write rather than the stored width. Closes when the pointer stores the width p.241 and p.246 print (`P11A` turns green), or when an Amiga specialist rejects that change; if the rejection means the two pages are wrong (they describe the long-word write, not the stored width), the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | unknown chip revision; the width stored by DSKPTH/DSKPTL in `agnus_diskdma`, isolated by an all-ones write to both registers and a read of `address_out`. The chipset-conditioned width is [NM-HRM-043](NM-HRM-043.md)'s |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed p.241 (register list) and p.246 (*DSKPTH — Pointer to Data*); p.279 (Appendix A) and p.289 (Appendix B) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims by assertion: `P11A`: `opus:8-dsk.c34` (p.241), `opus:8-dma.c7` / `sol:8-dskpth.c1` (p.246); `P1`: `opus:8-dsk.c32` (p.241), `opus:8-dma.c11` / `sol:8-dskpth.c7` (p.246), `opus:8-appA-dskpt.c1` / `sol:8-appa-dskpth.c1` (p.279), `opus:8-appB-dskpth.c1` / `sol:8-appb-dskpth.c1`, `opus:B-dskpth.addr` (p.289); `P2`: `opus:8-appA-dskpt.c6` / `sol:8-appa-dskptl.c1` (p.279), `opus:8-appB-dskptl.c1` / `sol:8-appb-dskptl.c1`, `opus:B-dskptl.addr` (p.289); `P5`: `opus:8-appA-dskpt.c9` / `sol:8-appa-dskptl.c4` (p.279), `opus:8-appB-dskptl.c6` / `sol:8-appb-dskptl.c8` (p.289); `P8`: `opus:8-dma.c26` / `sol:8-dsklen.c15`, `opus:8-dma.c27` / `sol:8-dsklen.c16` (p.247), `opus:8-dsk.c33` (p.241), `opus:8-dma.c3` / `sol:8-dskdma.c2` (p.246); `LIVENESS` and `P9` carry no claim id in their messages |
| **Module** | `agnus_diskdma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L56) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_diskdma.v#L56); inherited (MiSTer and TC64 have the same code) |
| **Test** | [`hrm/ch8-disk-dma-pointer`](../../hrm/ch8-disk-dma-pointer): `make -C hrm/ch8-disk-dma-pointer test`; the expected red exits nonzero |
| **Assertions** | `P11A`, red: expected 32 bits representable, observed `0xFFFFF` (20 bits). The package grades 7: `LIVENESS`, `P1`, `P2`, `P5`, `P8` and `P9` green, this one red |
| **Verdict of the four** | **nobody reaches 32 bits either, outside one emulator-only option.** Two keep a 32-bit variable and mask it on the DMA path — WinUAE's UAE-only Mega Chip RAM leaves it unmasked; two mask the stored value to 21 bits or fewer. Hardware: not measured here |
| **Effort** | none: no RTL change is suggested; any change is to the reading of the page |
| **See also** | [NM-HRM-043](NM-HRM-043.md) — the *chipset-conditioned* width of the same register, a different claim on a different page |

---

## Source requirement

Two pages give the pointer a 32-bit width.

**Chapter 8, printed p.241**, in the list of registers the disk subsystem uses:

> `DSKPTH     ($DFF020)    DMA pointer (32 bits)`

**Chapter 8, printed p.246**, under *DSKPTH — Pointer to Data*:

> "You specify the 32-bit wide address from which or to which the data is to be
> transferred. The lowest bit of the address must be zero, and the buffer must be
> in Chip memory. The value must be written as a single long word to the DSKPTH
> register ($DFF020)."

**The manual does not hold that width anywhere else.** Appendix A, printed p.279:

> "This pair of registers contains the 18-bit address of disk DMA data."

Appendix B, printed p.289:

> `DSKPTH     + *020   W     A (E)      Disk pointer (high 3 bits, 5 bits if ECS)`

> `DSKPTL     + *022   W     A          Disk pointer (low 15 bits)`

This finding is against the p.241/p.246 wording and only that.

## RTL and measurement

The pointer is assembled from two register writes into a 20-bit field.
[`src/minimig-aga/agnus_diskdma.v#L18`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L18)
and
[`#L56`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L56):

```verilog
	output	reg [20:1] address_out	//chip address out current disk dma pointer
...
assign address_outnew[20:1] = dma ? address_out[20:1]+1'b1 : {data_in[4:0],data_in[15:1]};
```

Five bits of DSKPTH and fifteen of DSKPTL, concatenated. The port is declared
`[20:1]`, so no bit beyond position 20 exists to be written and bit 0 has no
storage at all. Read as a byte address, those bits reach `$1FFFFE`.

    P11A: all-ones write to DSKPTH+DSKPTL gives address_out=0xFFFFF,
          representable in 20 bits

> **FAIL: P11A** (`opus:8-dsk.c34` p.241, `opus:8-dma.c7`/`sol:8-dskpth.c1`
> p.246, "32-bit wide address"): the maximum value reachable by writing all-ones
> to DSKPTH+DSKPTL is `0xFFFFF`, representable in 20 bits, not 32 — no bus path
> on this boundary can ever deposit a bit beyond position 20 into the pointer.

The fixture's liveness check (`DSKPTH<=0x00015`, `DSKPTL<=0x02C6A` giving
`address_out=0xA9635`, expected `0xA9635`) and the greens `P1`, `P2` and `P5`
measure the same way: write the register, inspect `address_out`. The red is the
width claim failing, not a dark fixture.

## Risks and unknowns

- **The manual contradicts itself about this width**, and p.241/p.246's "32" is
  the outlier: p.279 gives 18 bits, p.289 three or five high bits plus fifteen
  low (*Source requirement*). A test written from a different page would grade
  the same RTL on a different number; that is why this finding is stated as
  against p.241 and p.246, and why the OCS/ECS conditioning is filed separately as
  [NM-HRM-043](NM-HRM-043.md).
- **A pointer register pair narrower than the address space its caption names is
  not by itself a defect.** The likely reading is that p.241 and p.246 describe
  how the address is *written* (one 32-bit long word to $DFF020, which is
  literally what a `move.l` does), not how many bits Agnus keeps. On that reading
  the pages are loose rather than wrong, and NanoMig implements them correctly.
  Neither page states that reading, which is why the assertion is graded as
  printed.
- **No implementation of the four is on the page's side.** None offers a reachable
  32-bit disk pointer, except WinUAE under its UAE-only Mega Chip RAM, a
  configuration no Agnus has.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:5208-5216`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L5208-L5216) | **stores all 32 bits**, unmasked, bit 0 excepted. The ceiling arrives later: disk DMA reads through [`memory.cpp:808-817`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L808-L817), whose first act is `addr &= chipmem_full_mask`, which for a non-ECS Agnus is `0x80000 - 1` ([`memory.cpp:2619-2624`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2624)); `cs_agnussize` can override it, and with JIT enabled [`memory.cpp:2646-2648`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2646-L2648) resets it to the allocated size. When Mega Chip RAM or debugger memory is configured, [`chipmem_setindirect()`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L972-L1001) routes it through an unmasked `get_word` instead |
| **vAmiga** | [`AgnusRegs.cpp:704-718`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L704-L718) | **stores all 32 bits** and logs a value wider than `ptrMask` (19, 20 or 21 bits by Agnus revision, [`Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194)) to `XFILES` instead of truncating at the register. Every Agnus DMA access then truncates it, `addr &= agnus.ptrMask;` ([`Memory.cpp:1620-1625`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1620-L1625), [`:2157-2161`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2157-L2161)), and the bank table resolves anything past the installed banks to `MemSrc::NONE`, which returns the floating data bus |
| **Denise** | [`emulation/libami/agnus/dma.cpp:96-105`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L96-L105) | **masks the stored value at the write** in `setDskPtH` with `dmaChipMemMask`, the installed chip-RAM mask ([`memory.cpp:693-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L693-L698)), at most 21 bits. `setDskPtL` does not mask, because the low word cannot carry a bit that high. The single `ecs()` term in the mask's derivation *widens* it, for the ECS Slow-RAM mirror, and never narrows |
| **Copperline** | [`src/floppy/mod.rs:1250-1257`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1250-L1257) | **takes five high bits, exactly as NanoMig does**, then narrows again at the register by Agnus revision (`dma_addr_capability_mask()`, walked across all three revisions by [`src/bus/tests.rs:2787-2802`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L2787-L2802)). Ceiling `0x001F_FFFE`, the same 21-bit byte address NanoMig's `[20:1]` reaches |
| **MiSTer** | [`rtl/agnus_diskdma.v:56`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_diskdma.v#L56) | descendant — byte-identical to NanoMig; the file has no differences from it at all |
| **TC64** | [`rtl/minimig/agnus_diskdma.v:57`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_diskdma.v#L57) | descendant — the same statement, byte for byte (the line moves by one because TC64 adds an unrelated `turbo` input port above it); both ports remain `[20:1]` |

### What other Commodore documents say

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — says something else:

  > "Each of the pointer registers contains an 18-bit address, which points to the location in memory of data to be fetched next for the particular DMA channel."
  <!-- corpus: patent-us4874164-agnus-denise/p016.txt -->

  All 25 DMA pointer registers, the disk channel among them ('the disk and refresh DMA control logic 61 (2 channels)'), hold 18 bits, which address 256K words. p023 says of the same registers 'The pointers in the registers 138 are 19 bits wide'. The patent gives neither the page's 32 nor NanoMig's 20.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 29 — says something else:

  > "This pair of registers contains the 18 bit address of Disk DMA data."
  <!-- corpus: agnus-8371-spec/p029.txt -->

  The disk pointer is 18 bits: a DSKPTH with high 3 bits plus a DSKPTL with low 15 bits, on p029 and in Table 2 (p031). That is neither the 32 of p.241/p.246 nor NanoMig's 20. It matches the manual's own p.279 wording, and p019 has the disk controller reading or writing blocks of up to 128K anywhere in 512K of memory.

## Decision

**No change is suggested on this claim alone.** Twenty stored bits at positions
20:1 reach two megabytes, which is every address an Agnus of any revision can
drive. A 32-bit pointer register would be 4 GB of address space on a machine
whose largest chip-RAM configuration is 2 MB. If anything is to change it is the
reading of the page, not the RTL.
