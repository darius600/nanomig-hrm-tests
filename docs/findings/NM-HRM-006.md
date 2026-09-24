# NM-HRM-006 — DDFSTRT/DDFSTOP bit H3 is respected in lores, where p.91 says hires only

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a graded red |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault: all four independent implementations do what NanoMig does. |
| **Module** | `agnus_bitplanedma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L339) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L339) |
| **Report to** | **nobody.** NanoMig and all four independent implementations respect H3 at every resolution |
| **Test** | [`hrm/ch3-ddf-fetch-window`](../../hrm/ch3-ddf-fetch-window) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **unanimous with NanoMig**: all four respect H3 in lores and hires, on OCS and ECS. What they gate on the chipset is register bit 1, which p.277 lists as unused |

---

## What the HRM says

**Printed p. 91**, on `DDFSTRT` and `DDFSTOP`:

> Bits 7-2 — pixel position H8-H3 (**bit H3 only respected in Hires Mode.**)

---

## What NanoMig does

It respects H3 **at every resolution, on both chipsets**. The register is
stored one place up:

[`src/minimig-aga/agnus_bitplanedma.v#L250`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L250)

```verilog
      ddfstrt[8:2] <= #1 data_in[7:1];
```

so `ddfstrt[k]` holds register bit k−1 and is compared with `hpos[k]`, which is
Hk (VHPOSR reads `hpos[8:1]` as H8–H1):

[`src/minimig-aga/agnus_bitplanedma.v#L339`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L339) and [`#L349`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L349)

```verilog
      if (hpos[8:1]=={ddfstrt[8:3], ddfstrt[2] & ecs, 1'b0})
      if (hpos[8:1] == {ddfstop[8:3], ddfstop[2] & ecs, 1'b0})
```

**H3 is register bit 2, which lands in `ddfstrt[3]`** and is compared with no
condition at all. The term ANDed with the `ecs` strap is `ddfstrt[2]` -- register
bit 1, which the manual's bit map does not have.

### Measured

First fetch position, one bitplane, from the package's own harness:

| | $0038 | $003C (register bit 2, H3) | $003A (register bit 1) |
|---|---:|---:|---:|
| lores, OCS | 135 | **143** | 135 |
| lores, ECS | 135 | **143** | 139 |
| hires, OCS | 127 | **135** | 127 |
| hires, ECS | 127 | **135** | 3 |

H3 moves the fetch by 8 in every row, lores included. Register bit 1 moves it
only on ECS; the hires/ECS reading of 3 is recorded, not explained.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:1210`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1210) | <pre>ddf_mask = ecs_agnus ? 0xfe : 0xfc;</pre> applied at `:3663` / `:3674` |
| **vAmiga** | [`Core/Components/Agnus/Agnus.h:405`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L405) | <pre>u16 ddfMask() const { return isOCS() ? 0xFC : 0xFE; }</pre> |
| **Denise** | [`emulation/libami/agnus/register.cpp:293`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L293) | <pre>ddfStart = value & (ecsAndHigher() ? 0xfe : 0xfc);</pre> |
| **Copperline** | [`src/chipset/agnus.rs:490-496`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L490-L496), [`:498-499`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L498-L499) | <pre>pub(crate) fn ddf_register_mask(revision: AgnusRevision) -&gt; u16 {<br>&nbsp;&nbsp;&nbsp;&nbsp;if matches!(revision, AgnusRevision::Ocs) { 0x00FC } else { 0x00FE }</pre> and then <pre>pub fn effective_bitplane_ddf_start_hpos(revision: AgnusRevision, bplcon0: u16, raw: u16) -&gt; u16 {<br>&nbsp;&nbsp;&nbsp;&nbsp;let _ = bplcon0;</pre> |
| **MiSTer** | [`rtl/agnus_bitplanedma.v:339`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L339) | descendant — identical to NanoMig: `if (hpos[8:1]=={ddfstrt[8:3], ddfstrt[2] & ecs, 1'b0})`, H3 compared unconditionally, register bit 1 gated on the strap |
| **TC64** | [`rtl/minimig/agnus_bitplanedma.v:313`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L313) | descendant — the same statement, byte for byte (the line number moves because an earlier, unrelated rewrite of the `hde`/`hdiwstrt` display-enable logic removes 26 lines above it). A fix here diverges from neither line |

**Every mask here is 0xFC or 0xFE, and both keep 0x04 -- register bit 2, H3.**
So all four independent implementations respect H3 in lores and in hires, on
both chipsets, as NanoMig does. What they gate on the chipset is 0x02, register
bit 1: kept on ECS and AGA, cleared on OCS -- vAmiga's own comment calls it H2.
None of the four conditions either bit on resolution. Copperline is the sharpest
statement of it: its function *takes* the resolution register as a parameter and
then discards it with `let _ = bplcon0;`. Page 91's "only respected in Hires
Mode" is the outlier.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989**, PDF page 24 — says something else:

  > "the data fetch regis ters have a 4 pixel resolution because only five bits are used in either register"
  <!-- corpus: patent-us4874164-agnus-denise/p024.txt -->

  The shared specification says the fetch registers use five bits, with no resolution or chipset qualifier. It does not name the bits, but five position bits leave no sixth bit for H3 in hires. So it matches neither the 3rd edition's hires-only H3 nor NanoMig. patent-us5103499-copper p024 has the same sentence ('have only a 4 pixel resolution because only five bits').

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 37 — says something else:

  > "at even multiples of 8 clocks"
  <!-- corpus: agnus-8371-spec/p037.txt -->

  A note beside the bit-plane fetch timing diagram says data fetch start can only be specified at even multiples of 8 clocks, and that this is the position to specify for the normal width display, with a 20-word fetch for 320 pixels and a 40-word fetch for 640, as a render of the page shows. The note points at position 38 of the diagram; read in the register's units, a step of 8 leaves no place for H3 in either resolution. That matches neither p.91's hires-only H3 nor NanoMig, and it agrees with the shared patent specification's five bits. The same specification's register bit assignment on p.28 still gives bits 7-2 to H8-H3, as p.91 does, and marks bits 1-0 unused, so the document does not settle the point by itself.

Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS), 1988.

---

## Suggested change

**None.** NanoMig and all four independent implementations respect H3 at every
resolution; the printed sentence is the only source that conditions it on hires.

Settling it needs someone who knows what real Agnus does with H3 in lores, and
whether ECS Agnus uses register bit 1 as the implementations do -- not a patch.

---

## What holds the finding up

The test asserts what page 91 says, with a stimulus that differs only in
register bit 2 -- H3 -- and it is red because lores moves. The question this
finding raises is not whether the test is right but whether the sentence is.
