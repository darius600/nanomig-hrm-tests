# NM-HRM-043 — DSKPTH carries five high bits in both chipsets

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_diskdma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_diskdma.v#L56) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_diskdma.v#L56) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, and **only together with [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md) and [NM-HRM-024](NM-HRM-024.md)** — see *Suggested change* |
| **Test** | [`hrm/ch8-disk-pointer-ecs-and-copper`](../../hrm/ch8-disk-pointer-ecs-and-copper) — assertions `P4` and `P5` |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 2 red. Two greens on the same page's other half (`P2`, `P3`) pass in the same run |
| **Verdict of the four** | **two of the four narrow by chipset**: Copperline masks the stored pointer, WinUAE masks the address the DMA drives (`memory.cpp:2619-2623`), by Agnus revision. vAmiga and Denise do not. Here Copperline's has a test naming DSKPT explicitly. **The first three sightings said Copperline alone; all four are corrected — O59** |
| **See also** | [NM-HRM-003](NM-HRM-003.md) (Copper), [NM-HRM-022](NM-HRM-022.md) (blitter), [NM-HRM-024](NM-HRM-024.md) (audio) — **this is the fourth sighting of the same cause.** And [NM-HRM-014](NM-HRM-014.md), on the manual's own inconsistency about this extension |

---

## This is one cause in four engines, not four findings

[NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md) and [NM-HRM-024](NM-HRM-024.md) establish that the Copper, blitter
and audio pointers each latch five high bits with no chipset condition. **Their
reasoning is not repeated here.** What this entry adds is that the disk DMA
pointer is the fourth instance of the identical construction, and that the count
of affected engines is in fact larger than four.

Every DMA pointer in this Agnus takes its high bits from `data_in[4:0]`,
unconditionally. Six sites, no `ecs` term among them:

| engine | line |
|---|---|
| audio | [`agnus_audiodma.v:82`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_audiodma.v#L82) — `audlch[audlcsel] <= #1 data_in[4:0];` |
| bitplane | [`agnus_bitplanedma.v:220`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_bitplanedma.v#L220) — `assign bplpth_in = dma ? newpt[20:16] : data_in[4:0];` |
| blitter | [`agnus_blitter_adrgen.v:62`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_blitter_adrgen.v#L62) — `assign bltptr_in[20:1] = enaptr ? newptr[20:1] : {data_in[4:0], data_in[15:1]};` |
| Copper | [`agnus_copper.v:144`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L144) and [`:161`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v#L161) — `cop1lch[20:16] <= data_in[4:0];` |
| **disk** | [`agnus_diskdma.v:56`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_diskdma.v#L56) — `assign address_outnew[20:1] = dma ? address_out[20:1]+1'b1 : {data_in[4:0],data_in[15:1]};` |
| sprite | [`agnus_spritedma.v:179`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_spritedma.v#L179) — `assign sprpth_in = ackdma ? newptr[20:16] : data_in[4:0];` |

The disk line and the blitter line are the same expression, a space apart.

**The `ecs` signal exists and is threaded to four of these modules.** `agnus.v`
passes it to `agnus_bitplanedma`, `agnus_spritedma`, `agnus_copper`,
`agnus_blitter` and `agnus_beamcounter`, and each of those uses it — for DIWHIGH
and the DDF H3 bit, for the sprite VSTART/VSTOP ninth bit, for the Copper's own
danger range, for `BLTCON0L`/`BLTSIZH`. **None of them uses it for a pointer.**

`agnus_diskdma` and `agnus_audiodma` do not even have the port.
[`agnus.v:326-340`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus.v#L326-L340)
instantiates the disk engine with twelve connections and no `ecs` among them.

So the honest statement is the one [NM-HRM-024](NM-HRM-024.md) already reaches and this finding
confirms from a fourth direction: **this is one decision not to model OCS pointer
narrowing anywhere in Agnus**, not four independent omissions. It is not a
mistake repeated; it is a feature not implemented.

## What the HRM says

**Appendix B, printed p.289**, the DSKPTH row:

> `DSKPTH     + *020   W     A (E)      Disk pointer (high 3 bits, 5 bits if ECS)`

> `DSKPTL     + *022   W     A          Disk pointer (low 15 bits)`

Three high bits, five if ECS — word for word what p.270 says of the blitter
pointers and p.266 of `AUDxLCH`.

**Appendix B, printed p.288**, defining the `(E)` mark the DSKPTH row carries in
its chip column:

> "Registers denoted with an '(E)' in the chip column means that those registers
> have been changed in the Enhanced Chip Set (ECS). … Certain ECS registers are
> completely new, others have been extended in their functionality."

DSKPTH carries it; DSKPTL, on the line below, does not. The manual is saying
that the high half of this pair, and only the high half, differs between the two
chipsets.

## What NanoMig does

```verilog
//address_out input multiplexer and ALU
assign address_outnew[20:1] = dma ? address_out[20:1]+1'b1 : {data_in[4:0],data_in[15:1]};
```

Five bits, always, from a module that has no chipset input to condition them on.

### Measured

At the `agnus` boundary, where `ecs` is a real, drivable port — not at
`agnus_diskdma`, which cannot put the question:

    FAIL: P4  the (E) mark: driving the real ecs port at the agnus boundary
              changes what DSKPTH loads   [sol:8-appb-dskpth.c8, p.289 with p.288]
    FAIL: P5  on OCS, DSKPTH holds only the high three bits
                                          [sol:8-appb-dskpth.c9, p.289]

    PASS: P2  the high three pointer bits ARE carried by DSKPTH
    PASS: P3  under ECS, DSKPTH holds five bits

The sibling package measured the same unconditional width one level down —
`DSKPTH<=0x1F` gives high field `0x0001F`, `DSKPTH<=0x07` gives `0x00007` — and
declined to grade the OCS reading there precisely because that boundary has no
`ecs` port to drive.

**P2 and P3 are what stop this being a dark fixture.** They are the same
register, the same write path and the same observation point, and they pass. The
red is the conditioning that is absent, not a measurement that cannot see
anything.

## What the others say

A dash means the source was read and no such code was found, not that it was not
looked for.

| | code | |
|---|---|---|
| **Copperline** | [`src/chipset/agnus.rs:113-118`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L113-L118)<br>[`src/floppy/mod.rs:1250-1252`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1250-L1252) | the same central `dma_addr_capability_mask()` the other three findings cite (`Ocs => 0x0007_FFFF`), reaching the floppy through `configure_chip_dma_masks` at [`src/bus.rs:4991-5002`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L4991-L5002). **It narrows DSKPT by Agnus revision** |
| **WinUAE** | [`memory.cpp:2619-2624`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2624) | **not in the register — on the bus.** `DSKPTH` stores all sixteen bits ([`disk.cpp:5208-5211`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L5208-L5211)), but every Agnus-side chip-RAM access masks with `chipmem_full_mask`, and that is set to `0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear. See the caveat below |
| **vAmiga** | — | no narrowing of the pointer. `ptrMask` is per-revision ([`Agnus.cpp:190-194`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L194)) but reaches only an `assert`, the inspector and an `xfiles("DSKPT %08x out of range")` log line at [`AgnusRegs.cpp:715-717`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L715-L717). The fetched address is `dskpt` unmasked ([`AgnusDma.cpp:84-96`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L84-L96)) |
| **Denise** | — | masks `dskpt` at the DSKPTH write, but with `dmaChipMemMask`, which is the installed chip-RAM mask ([`memory.cpp:693-698`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L693-L698)). The one `ecs()` term there *widens* it, for the Slow-RAM mirror. No chipset narrowing |
| **MiSTer** | six sites, byte-identical to NanoMig's: [`rtl/agnus_audiodma.v:82`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L82), [`rtl/agnus_bitplanedma.v:220`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L220), [`rtl/agnus_blitter_adrgen.v:62`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter_adrgen.v#L62), [`rtl/agnus_copper.v:144,161`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L144), [`rtl/agnus_diskdma.v:56`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_diskdma.v#L56), [`rtl/agnus_spritedma.v:179`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_spritedma.v#L179) | descendant — : `rtl/agnus_audiodma.v:82` (`audlch[audlcsel] <= #1 data_in[4:0];`), `rtl/agnus_bitplanedma.v:220` (`assign bplpth_in = dma ? newpt[20:16] : data_in[4:0];`), `rtl/agnus_blitter_adrgen.v:62` (`assign bltptr_in[20:1] = enaptr ? newptr[20:1] : {data_in[4:0], data_in[15:1]};`), `rtl/agnus_copper.v:144,161` (`cop1lch`/`cop2lch[20:16] <= data_in[4:0];`) and `rtl/agnus_diskdma.v:56` — five statements, byte-identical to NanoMig's, no `ecs` term at any of them — plus `rtl/agnus_spritedma.v:179` (`assign sprpth_in = ackdma ? newptr[20:16] : data_in[4:0];`), also identical. `rtl/agnus.v:281-295` instantiates `agnus_diskdma` with NanoMig's same twelve ports and no `.ecs`. none of the six carries an `ecs` term. |
| **TC64** | the same six statements, byte-identical (line numbers move only where an unrelated rewrite precedes the site): [`rtl/minimig/agnus_audiodma.v:82`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_audiodma.v#L82), [`rtl/minimig/agnus_bitplanedma.v:194`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L194), [`rtl/minimig/agnus_blitter_adrgen.v:62`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter_adrgen.v#L62), [`rtl/minimig/agnus_copper.v:144,161`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L144), [`rtl/minimig/agnus_diskdma.v:57`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_diskdma.v#L57), [`rtl/minimig/agnus_spritedma.v:179`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_spritedma.v#L179) | descendant — : `rtl/minimig/agnus_audiodma.v:82`, `rtl/minimig/agnus_bitplanedma.v:194`, `rtl/minimig/agnus_blitter_adrgen.v:62`, `rtl/minimig/agnus_copper.v:144,161`, `rtl/minimig/agnus_diskdma.v:57` and `rtl/minimig/agnus_spritedma.v:179` — the same six statements, byte-identical (line numbers move only where an unrelated rewrite precedes the site). `rtl/minimig/agnus.v:327-342` instantiates `agnus_diskdma` with a thirteenth port, `.turbo(turbo)`, unrelated to ECS, and still no `.ecs`. Both forks thread `ecs` to exactly the same five modules NanoMig does — bitplane, sprite, Copper, blitter and beamcounter — and to neither disk nor audio. A fix at any of the six sites, in any of the seven files, diverges from neither line |

**Copperline states the rule in a test, on this register by name.**
[`src/bus/tests.rs:2787-2802`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L2787-L2802)
walks `$001F`/`$FFFE` into every DMA pointer at each of three Agnus revisions and
asserts `bus.floppy.dskpt() == mask`, with `0x0007_FFFE` for OCS. Its comment
reads *"walk every DMA pointer register and check the writable high bits follow
the Agnus revision's address-bus reach"*. The disk pointer is not an afterthought
there.

### One refinement to the sibling entries' WinUAE row

[NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md) and [NM-HRM-024](NM-HRM-024.md) each record WinUAE as *"no chipset
narrowing"*, with a dash — which in [NM-HRM-024](NM-HRM-024.md)'s own convention means the
source had not been read for that finding. **Read for this one, WinUAE does
narrow by chipset, just not where those entries looked.** The register keeps
every bit; the Agnus-side access mask does the work, and an OCS configuration
gets 19 bits — the manual's OCS reach exactly.

Two caveats keep this from being a clean fifth vote for the manual: `cs_agnussize`
overrides the mask in either direction
([`memory.cpp:2627-2644`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2627-L2644)),
and with JIT enabled
[`memory.cpp:2646-2648`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2646-L2648)
resets it to the allocated chip-RAM size, dropping the chipset term altogether.
It is a memory-map behaviour that happens to produce the pointer width, not a
model of the register.

**This does not change the verdict of the four for any of the four findings** —
Copperline is still the only one that narrows the pointer itself — but it does
change what "nobody else models this" is worth as an argument. Whoever revisits
[NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md) or [NM-HRM-024](NM-HRM-024.md) should carry this across rather than
re-derive it.

## Suggested change

**The same one, and it is one change, not four.** [NM-HRM-003](NM-HRM-003.md) and [NM-HRM-022](NM-HRM-022.md)
already set out the two coherent actions; this finding adds only that the count
is six pointer sites in six modules, two of which have no chipset input at all:

1. **Model it once, centrally**, the way Copperline does — a mask derived from the
   Agnus revision and applied to every assembled DMA pointer. That closes four
   findings with one change. In this RTL it means routing `ecs` into
   `agnus_diskdma` and `agnus_audiodma`, which today have no such port, or
   masking `address_dsk`/`address_aud` where `agnus.v` already has `ecs` in scope.
2. **Declare the core ECS-and-later**, making the manual's OCS pointer sentences
   out of scope, and record all four as one scope decision.

**Four separate patches in four engines is the option to avoid**, and a fourth
finding is the argument against it rather than for it.

## What holds the finding up

- **The manual gives two widths for two chipsets**, on this register as on three
  others. The finding does not depend on agreeing that OCS is three bits — only
  on the widths differing, which p.289 prints plainly and p.288's `(E)` mark
  confirms for the high half alone.
- **Measured at the boundary that has the port.** `agnus`, not `agnus_diskdma` —
  the same rule that moved [NM-HRM-022](NM-HRM-022.md)'s measurement to `agnus_blitter`. A test
  cannot show that a chipset input is ignored from inside a module the input
  never reaches.
- **Two greens on the same row, the same write path and the same observation
  point** pass in the same run.
- **The RTL was read directly for this page**, including the instantiation in
  `agnus.v` and the five other pointer sites.

## What weakens it

Everything that weakens [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md) and [NM-HRM-024](NM-HRM-024.md) weakens this
one identically, and for the same reason: it is the same question.

- **Three of four implementations do what NanoMig does** at the register.
- **[NM-HRM-014](NM-HRM-014.md) records that the manual applies its own ECS pointer extension
  inconsistently.** If the manual is not self-consistent about how widely the
  extension reaches, then four findings each quoting one of its sentences are
  four views of one unsettled question.

## For the specialists

There is nothing here that the first three sightings do not already put before
you. What the fourth adds is the shape of the answer: the narrowing is missing
from every DMA pointer in this Agnus, including two engines with no chipset
signal in their port lists, so the decision to be taken is about the design, not
about the disk.

Take it once, for all four.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore-Amiga, *Amiga Hardware Manual*, 1985 — the oracle's first edition**, PDF page 211 — agrees with the manual:

  > "DSKPTH gets the high 3 bits of the pointer, DSKPTL gets the low 16 bits of the pointer."
  <!-- corpus: hw-manual-1985/p211.txt -->

  The 1985 manual describes only the original chip set and gives DSKPTH three bits, as do its Appendix A (p232) and B (p235): 'Disk pointer (High 3 bits)'. That is the manual's OCS branch, against NanoMig's unconditional five. It predates ECS, so it says nothing about the (E) conditioning itself.

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga — one shared specification, counted once**, PDF page 16 — agrees with the manual:

  > "the disk and refresh DMA control logic 61 (2 channels), and the Blitter DMA control logic 67 (four channels). For each of the 25 DMA channels, an 18 bit RAM address pointer is stored in a designated pointer register 138"
  <!-- corpus: patent-us4874164-agnus-denise/p016.txt -->

  An 18-bit word pointer is the manual's OCS split, 3 high bits plus 15 low, and the disk channel is one of the 25. The patent predates ECS, so it has no chipset condition; p023 says '19 bits wide' for the same registers.

Silent on this point: Commodore-Amiga, *Amiga Hardware Manual* errata, October 1985; US patent 4,777,621, Commodore-Amiga; US patent 4,780,844, Commodore-Amiga; US patent 4,829,473, Commodore-Amiga; 6502.org dissection of Paula 8364R7; techtravels, *Inside Paula's Floppy Controller*, part I.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.
