# NM-HRM-043 — DSKPTH carries five high bits in both chipsets [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should DSKPTH hold only its high three bits on OCS (`ecs` = 0), as p.289 prints, where NanoMig loads five (`P5` red), by one Agnus-revision mask shared with [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md), [NM-HRM-024](NM-HRM-024.md) and [NM-HRM-088](NM-HRM-088.md)? Closes when that mask makes DSKPTH load three high bits at `ecs` = 0 (`P4` and `P5` turn green), or when an Amiga specialist rejects it, also by declaring the core ECS-and-later and recording that on those five pages; if the rejection means p.289 is wrong, the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | DSKPTH written at the `agnus` boundary with the `ecs` port at 0 and at 1, read back as the pointer the disk DMA slot drives; the page names no chip revision, so `unknown` |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix B, printed p.289 (DSKPTH and DSKPTL rows) and p.288 (the `(E)` mark); p.270 and p.266 are named for comparison · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `P4` `sol:8-appb-dskpth.c8` (p.289 composed with p.288), `P5` `sol:8-appb-dskpth.c9` (p.289), `P2` `opus:8-appB-dskpth.c6` (p.289), `P3` `opus:8-appB-dskpth.c7` / `sol:8-appb-dskpth.c10` (p.289), `P1` `sol:8-appa-dskptr.c3` (p.279, graded on [NM-HRM-044](NM-HRM-044.md)); `LIVENESS` and `COPLIVE` carry no claim id |
| **Module** | `agnus_diskdma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L56) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_diskdma.v#L56); inherited (byte-identical in MiSTer and TC64) |
| **Test** | [`hrm/ch8-disk-pointer-ecs-and-copper`](../../hrm/ch8-disk-pointer-ecs-and-copper): `make -C hrm/ch8-disk-pointer-ecs-and-copper test` · [`hrm/ch8-disk-dma-pointer`](../../hrm/ch8-disk-dma-pointer): `make -C hrm/ch8-disk-dma-pointer test`; an expected red exits nonzero |
| **Assertions** | 2 red of this page: `P4` expected the same `DSKPTH` write of `0x0018` to load differently at `ecs` 0 and 1, observed no difference; `P5` expected bits 4:3 clear after `0x001F` at `ecs` 0, observed them set. The package reports `4 passed, 3 failed`: the third red, `P1`, is graded on [NM-HRM-044](NM-HRM-044.md); `P2`, `P3`, `LIVENESS` and `COPLIVE` are green |
| **Verdict of the four** | **three of the four narrow by chipset**: Copperline masks the stored pointer; WinUAE (`memory.cpp:2619-2623`) and vAmiga (`Memory.cpp:1621-1623`) mask the address the DMA drives, by Agnus revision. Denise does not — it masks to the installed chip RAM, not by Agnus revision. Here Copperline's has a test naming DSKPT explicitly. Hardware: not measured |
| **Effort** | M — one central mask derived from the Agnus revision in `agnus.v`, or an `ecs` port into `agnus_diskdma` and `agnus_audiodma`; none if the core is declared ECS-and-later |
| **See also** | [NM-HRM-003](NM-HRM-003.md) (Copper), [NM-HRM-022](NM-HRM-022.md) (blitter), [NM-HRM-024](NM-HRM-024.md) (audio) |

---

## Source requirement

**Appendix B, printed p.289**, the DSKPTH and DSKPTL rows:

> `DSKPTH     + *020   W     A (E)      Disk pointer (high 3 bits, 5 bits if ECS)`

> `DSKPTL     + *022   W     A          Disk pointer (low 15 bits)`

Three high bits, five if ECS: word for word what p.270 says of the blitter
pointers and p.266 of `AUDxLCH`. **Printed p.288** defines the `(E)` mark the
DSKPTH row carries:

> "Registers denoted with an '(E)' in the chip column means that those registers
> have been changed in the Enhanced Chip Set (ECS). … Certain ECS registers are
> completely new, others have been extended in their functionality."

DSKPTL, on the line below, has no `(E)`: only the high half of the pair differs
between the two chipsets.

## RTL and measurement

```verilog
assign address_outnew[20:1] = dma ? address_out[20:1]+1'b1 : {data_in[4:0],data_in[15:1]};
```

[`agnus_diskdma.v:56`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L56)
loads five bits, always, from a module with no chipset input; the blitter's
line loads its pointer with the same expression. The pointer sites of six
engines take their high bits from `data_in[4:0]` with no `ecs` term:

| engine | line |
|---|---|
| audio | [`agnus_audiodma.v:82`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_audiodma.v#L82) |
| bitplane | [`agnus_bitplanedma.v:220`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L220) |
| blitter | [`agnus_blitter_adrgen.v:62`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter_adrgen.v#L62) |
| Copper | [`agnus_copper.v:144`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L144) and [`:161`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L161) |
| **disk** | [`agnus_diskdma.v:56`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L56) |
| sprite | [`agnus_spritedma.v:179`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_spritedma.v#L179) |

`agnus.v` threads `ecs` to `agnus_bitplanedma`, `agnus_spritedma`, `agnus_copper`,
`agnus_blitter` and `agnus_beamcounter`, and none of them uses it for a pointer.
`agnus_diskdma` and `agnus_audiodma` have no such port:
[`agnus.v:326-340`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L326-L340)
instantiates the disk engine with twelve connections and no `ecs`.

The test measures at the `agnus` boundary, where `ecs` is a drivable port, not at
`agnus_diskdma`, which cannot put the question:

    FAIL: P4  the (E) mark: driving the real ecs port at the agnus boundary
              changes what DSKPTH loads   [sol:8-appb-dskpth.c8, p.289 composed with p.288]
    FAIL: P5  on OCS, DSKPTH holds only the high three bits
                                          [sol:8-appb-dskpth.c9, p.289]

    PASS: P2  the high three pointer bits ARE carried by DSKPTH
    PASS: P3  under ECS, DSKPTH holds five bits

P2 and P3 use the same register, write path and observation point and pass, so
the red is the missing conditioning, not a measurement that sees nothing. The
sibling package `ch8-disk-dma-pointer` measured the same unconditional width one
level down (`DSKPTH<=0x1F` gives high field `0x0001F`, `DSKPTH<=0x07` gives
`0x00007`) and declined to grade the OCS reading there, since that boundary has
no `ecs` port to drive.

## Risks and unknowns

- This page is weakened identically by everything that weakens [NM-HRM-003](NM-HRM-003.md),
  [NM-HRM-022](NM-HRM-022.md) and [NM-HRM-024](NM-HRM-024.md): it is the same question.
- One of the four fetches as NanoMig does: Denise drives the stored pointer with no
  chipset term. That WinUAE, vAmiga and Denise keep all five bits in the register
  weakens nothing, since DSKPTH is write-only and the stored value shows only in
  the address the DMA drives.
- WinUAE's bus mask is a memory-map behaviour, not a model of the register:
  `cs_agnussize` overrides it in either direction
  ([`memory.cpp:2627-2644`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2627-L2644)),
  and with JIT enabled
  [`memory.cpp:2646-2648`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2646-L2648)
  resets it to the allocated chip-RAM size, dropping the chipset term.
- [NM-HRM-014](NM-HRM-014.md) records that the manual applies its own ECS pointer extension
  inconsistently; if so, the findings that each quote one of its sentences are
  views of one unsettled question.

## Independent evidence

### What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`memory.cpp:2619-2624`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2624) | narrows the bus, not the register: `DSKPTH` stores all sixteen bits ([`disk.cpp:5208-5211`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L5208-L5211)), and every Agnus-side chip-RAM access masks with `chipmem_full_mask`, set to `0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear |
| **vAmiga** | [`Memory.cpp:1621-1623`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1621-L1623) | narrows the bus, not the register: `setDSKPTH` keeps the whole high word ([`AgnusRegs.cpp:713`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L713), and `ptrMask` there only feeds a log line ([`AgnusRegs.cpp:715-717`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L715-L717)); the disk fetch goes through an accessor that begins `addr &= agnus.ptrMask;` ([`AgnusDma.cpp:84-96`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L84-L96)), and `ptrMask` is `0x07FFFF` for an OCS Agnus ([`Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194)) |
| **Denise** | [`memory.cpp:693-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L693-L698) | masks `dskpt` at the DSKPTH write with `dmaChipMemMask`, the installed chip-RAM mask; the one `ecs()` term there widens it for the Slow-RAM mirror. No chipset narrowing |
| **Copperline** | [`agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118) | narrows DSKPT by Agnus revision: `dma_addr_capability_mask()` gives `Ocs => 0x0007_FFFF`, reaching the floppy ([`floppy/mod.rs:1250-1252`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1250-L1252)) through `configure_chip_dma_masks` ([`bus.rs:4991-5002`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L4991-L5002)) |
| **MiSTer** | [`agnus_audiodma.v:82`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L82), [`agnus_bitplanedma.v:220`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L220), [`agnus_blitter_adrgen.v:62`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter_adrgen.v#L62), [`agnus_copper.v:144,161`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L144), [`agnus_diskdma.v:56`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_diskdma.v#L56), [`agnus_spritedma.v:179`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_spritedma.v#L179) | descendant — the same six statements, byte-identical; `rtl/agnus.v:281-295` instantiates `agnus_diskdma` with the same twelve ports and no `.ecs` |
| **TC64** | [`agnus_audiodma.v:82`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_audiodma.v#L82), [`agnus_bitplanedma.v:194`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L194), [`agnus_blitter_adrgen.v:62`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter_adrgen.v#L62), [`agnus_copper.v:144,161`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L144), [`agnus_diskdma.v:57`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_diskdma.v#L57), [`agnus_spritedma.v:179`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_spritedma.v#L179) | descendant — the same six statements, byte-identical; `rtl/minimig/agnus.v:327-342` adds a thirteenth port, `.turbo(turbo)`, unrelated to ECS, and still no `.ecs`. Both forks thread `ecs` to the same five modules NanoMig does and to neither disk nor audio, so a fix at any of the six sites diverges from neither |

Copperline states the rule in a test that names this register:
[`src/bus/tests.rs:2785-2802`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L2785-L2802)
writes `$001F`/`$FFFE` into every DMA pointer at three Agnus revisions and asserts
`bus.floppy.dskpt() == mask`, with `0x0007_FFFE` for OCS. Copperline is the only
one that narrows the pointer itself.

### What other Commodore documents say

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "the disk and refresh DMA control logic 61 (2 channels), and the Blitter DMA control logic 67 (four channels). For each of the 25 DMA channels, an 18 bit RAM address pointer is stored in a designated pointer register 138"
  <!-- corpus: patent-us4874164-agnus-denise/p016.txt -->

  An 18-bit word pointer is the manual's OCS split, 3 high bits plus 15 low, and the disk channel is one of the 25. The patent predates ECS, so it has no chipset condition; p023 says '19 bits wide' for the same registers.

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 29 — agrees with the manual:

  > "DSKPTH Disk pointer (high 3 bits) DSKPTL Disk pointer (low 15 bits) This pair of registers contains the 18 bit address of Disk DMA data."
  <!-- corpus: agnus-8371-spec/p029.txt -->

  On the OCS 8371, DSKPTH holds three high bits (Table 2, p031: 'Disk pointer (High (3) bits)'). That is the manual's OCS half and exactly assertion P5, against NanoMig's five at ecs=0. The pin table (p004) limits device DMA to the lower 512K bytes. The five-bit ECS half is outside this document.

## Decision

This is one decision not to model OCS pointer narrowing anywhere in Agnus, not
four independent omissions. The disk pointer is the fourth sighting of the same
construction after [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md) and
[NM-HRM-024](NM-HRM-024.md), and [NM-HRM-088](NM-HRM-088.md) is the fifth; it adds
that two of the six engines have no chipset input at all, so the decision is about
the design, not about the disk. Report it once, for all five.

Suggested change, one for the five pages:

1. **Model it once, centrally**, as Copperline does: a mask derived from the Agnus
   revision, applied to every assembled DMA pointer. That closes the five findings,
   NM-HRM-088's bitplane pointer included. Here it means routing `ecs` into
   `agnus_diskdma` and `agnus_audiodma`, or masking `address_dsk` and `address_aud`
   where `agnus.v` already has `ecs` in scope.
2. **Declare the core ECS-and-later**, making the manual's OCS pointer sentences
   out of scope, and record all five as one scope decision.

Four separate patches in four engines is the option to avoid.
