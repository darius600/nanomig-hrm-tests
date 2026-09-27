# NM-HRM-019 — SPRxCTL bit 4 (SHSH1) is not implemented

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | seen in the RTL source; no test yet |
| **What is asked of a reader** | Nothing yet. This was read from the source rather than measured by a graded assertion, and needs a test before it can ask for anything. The package's one red assertion sets bit 4 outside SuperHires, where p. 303 gives it no position; *What the test measures* says what it must measure instead. |
| **Module** | `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers) — `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` is red, `10 passed, 1 failed`, but sets bit 4 at the lores sprite rate, outside SuperHires |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **all four implement it**, and on ECS all four confine its effect on position to SuperHires, as the page's Warning states |
| **Origin** | the surviving half of the withdrawn [NM-HRM-013](NM-HRM-013.md) |

---

## What the HRM says

**Printed p. 303** gives bit 4 a name *and a behaviour*, which is what makes this
a requirement rather than a mention:

```
04      SHSH1     Start horizontal (SHR mode) 70ns increment
00      SH0       Start horiz. value Low bit 140 ns increment
```

scoped by the page's own note and Warning:

> "Note: bits 3 and 4 are in the ECS chips only."

> "70ns sprite positions are only available in SuperHires mode. Attempting to
> use 70ns sprite positioning with Hires mode under the current system may lead
> to unpredictable results."

So the row asks one thing: in SuperHires, bit 4 moves the sprite's start by 70
ns, half of SH0's 140 ns. Outside SuperHires the page gives it no position, and
in Hires it promises nothing at all.

**Compare bit 3 on the same page**, which the manual prints as `(unimplemented)`.

**Three other pages give the bit no function**, without naming a chip set:
chapter 4's sprite-control word on printed p. 107,

```
          Bits 6-3        Unused (make zero)
```

its summary on p. 128,

> "Bits 6-3 are reserved for future use (make zero)."

and Appendix A's SPRxCTL table on p. 284, on a row marked `D( E )`:

```
                 06-04    X        Not used
```

Appendix A itself sends the reader to Appendix C for what the ECS changed
(printed p. 264):

> "Registers denoted with an "(E)" in the chip column means that those
> registers have been changed the Enhanced Chip Set(ECS)."

> "See the register map in Appendix C for information on which ECS registers
> are new and which have been modified."

So for an ECS Denise the manual's word on bit 4 is p. 303's.

---

## What NanoMig does

Nothing with it. A write to SPRxCTL keeps bits 7 and 0 and no other, in both
builds of the shifter, with and without `DENISE_EBR`:

[`src/minimig-aga/denise_sprites_shifter.v#L133-L138`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L133-L138)

```verilog
// CTL register
always @(posedge clk)
  if (clk7_en) begin
    if (aen && address==CTL)
      {attach,hstart[0]} <= {data_in[7],data_in[0]};
  end
```

In SuperHires the start still moves only in 140 ns steps. `denise.v` hands the
sprite module `spres` = `2'b10` whenever BPLCON0's SHRES bit is set and
BPLCON3's SPRES field reads 0, which without AGA it always does
([`denise.v#L252`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L252)):

[`src/minimig-aga/denise.v#L390-L391`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L390-L391)

```verilog
wire [1:0] spres_d;
assign spres_d = (spres == 2'b00) ? shres ? 2'b10 : 2'b01 : spres;
```

At `2'b10` the sprite's pixels shift out at 70 ns, `2'b10   : shift = ~c1 ^ c3;`
([`denise_sprites.v#L161-L164`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L161-L164)),
but the start is compared once per 7 MHz cycle against the 9-bit `hstart`,
whose low bit is SH0
([`denise_sprites_shifter.v#L113-L116`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites_shifter.v#L113-L116)).
Nothing places the start on the 70 ns position between two SH0 steps.

---

## What the test measures

`SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` in `hrm/ch3-sprite-registers` arms
sprite 0 at SPR0POS = `$40`, writes SPR0CTL = `$10` against `$00`, and requires
the `sprdata` trace to change (`test_denise_sprites.cpp:1017-1019`):

```cpp
Meas base = measure(/*pos=*/0x40, /*ctl=*/0x00, /*spres=*/0, /*aga=*/0);
Meas mut = measure(/*pos=*/0x40, static_cast<uint8_t>(1u << 4), /*spres=*/0, /*aga=*/0);
bool differs = base.trace != mut.trace;
```

It is red, one of the package's `10 passed, 1 failed`, but it does not grade the
row:

- `spres` = 0 is the module's default case, a 140 ns lores shift
  (`denise_sprites.v:164`), and a value `denise.v:391` never supplies. p. 303
  gives bit 4 no position outside SuperHires, and none of the four
  implementations gives it one there or changes a lores sprite for it. An
  implementation that does what p. 303 describes leaves this trace identical
  and fails the assertion too.
- `measure()` samples `sprdata` once per `hpos` step, 140 ns apart
  (`test_denise_sprites.cpp:631-636`), so even in SuperHires it could not see a
  start that moves by 70 ns.

**What it must measure instead**, on the same `denise_sprites` fixture with
`aga` = 0:

- `spres` = `2'b10`, the value `denise.v:391` supplies when SHRES is set;
- sprite 0 armed as now (SPR0POS `$40`, SPR0DATB `$0000`, SPR0DATA `$8000`),
  once each with SPR0CTL `$00`, `$01` (SH0 alone) and `$10` (SHSH1 alone), each
  on a fresh DUT;
- `sprdata` sampled at every 28 MHz clock, 35 ns apart, not once per `hpos`
  step;
- expected, from the two rows' "70ns increment" and "140 ns increment": SHSH1
  alone moves the first non-zero sample later by half as many samples as SH0
  alone does, two against four.

Outside SuperHires the page asks only that bit 4 give no 70 ns position, and in
Hires nothing at all. The package's own `superhires_spres` cannot stand in for
`2'b10`: it is the first `spres` at which SH0 moves the start
(`test_denise_sprites.cpp:848-852`), SH0 moves it at every value, and so it is
1, the lores default.

[`integration/ch4-sprite-registers`](../../integration/ch4-sprite-registers)
specifies the same measurement at the video output: P1 sweeps the start in 70
ns steps with SHRES set and expects each step to move the sprite's edge by one
70 ns output pixel, and P2 expects only 140 ns multiples in Lores. It has no
test source yet.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2825-2836`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2825-L2836) | <pre>} else if (ecs_denise) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (s-&gt;ctl & 0x10) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;sprxp \|= 2;</pre> bit 4 (`0x10`) from ECS Denise on; bit 3 (`0x08`) only on AGA. Outside SuperHires the match drops it ([`:5318`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5318), `sp->xpos & ~3`); in ECS hires it only clips the sprite's leftmost pixel ([`:5325-5329`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L5325-L5329)) |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:171-173`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L171-L173) | <pre>(sprctl[x] & 0x10) &gt;&gt; 4);  // SHSH1 (ECS only)</pre> the lowest bit of the sprite's start; the start is then masked to the sprite pixel's width ([`:968`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L968), [`DeniseRegs.cpp:854`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L854)), which on OCS and ECS keeps the bit only in SuperHires |
| **Denise** | [`emulation/libami/video/denise.cpp:299`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L299) | <pre>spr.shsh10 = (value & 0x10) &gt;&gt; 3;</pre> compared only when `_shres` |
| **Copperline** | [`src/video/bitplane/sprite.rs:1857-1859`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1857-L1859) | <pre>pub(super) fn sprite_hsub_70ns(ctl: u16) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;ctl & 0x0010 != 0<br>}</pre> |
| **MiSTer** | — | descendant — `rtl/denise_sprites.v` — the same absence: no `sprctl[4]`, `SHSH1` or `shsh1` anywhere in the file |
| **TC64** | — | descendant — `rtl/minimig/denise_sprites.v` — likewise absent. A fix is new work on both lines; neither reads bit 4 for NanoMig to have dropped |

**All four independent implementations implement it**, and on ECS all four
confine its effect on position to SuperHires, as p. 303's Warning states: Denise reads it
always and compares it only in SuperHires; vAmiga and WinUAE mask it out of the
start position outside SuperHires; Copperline adds it only while SHRES is
set.
MiSTer and TC64 have never read the bit either, which is the same absence
NanoMig has, not a vote against it.

WinUAE's decode is the clearest statement of the ECS-versus-AGA split: `0x10` is
read from ECS Denise onwards, `0x08` only on AGA. That is the split the
withdrawn [NM-HRM-013](NM-HRM-013.md) was broken into.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — **agrees with NanoMig**:

  > "07 ATT Sprite attach control bit (odd Sprites) 06-04 X Not used."
  <!-- corpus: denise-spec-252126/p010.txt -->

  In Commodore's ECS Denise specification (it defines SHRES and names the 8373 revisions) the SPRxCTL table gives bits 06-04 as 'Not used' and has no row for bit 3; SHSH1 and 70 ns sprite positioning appear nowhere in its 14 pages. That sides with NanoMig not reading bit 4. Caveat: the table has the shape of an OCS-era SPRxCTL table, without even the EV8 row, so it may have been carried over rather than written for ECS.

Silent on this point: US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989; US patent 5,103,499, Commodore-Amiga, filed 1989 as a division of the 1986 application, granted 1992.

---

## Suggested change

Read bit 4 into the sprite's horizontal position as a 70 ns sub-position, and
apply it only in SuperHires. Denise's one-liner plus its `_shres` guard is the
smallest complete model; Copperline's named predicate is the tidiest.

```verilog
wire sprite_hsub_70ns = sprctl[4];   // SHSH1, appendix p.303 -- SuperHires only
```

with the position arithmetic taking it only when the sprite resolution is
SuperHires.

---

## Why this entry exists, and what it replaced

[NM-HRM-013](NM-HRM-013.md) asserted that `SPRxCTL` bits 3 **and** 4 are live fields, resting on
the note *"bits 3 and 4 are in the ECS chips only"* — a statement about **which
chips have them**, not about what they do. That is not a requirement, and the
finding was withdrawn.

The two bits then part company:

| bit | what the manual prints | outcome |
|---|---|---|
| 3 | `(unimplemented)` | the manual settles it in the RTL's favour |
| 4 | `SHSH1  Start horizontal (SHR mode) 70ns increment` | a behaviour — this finding |

The withdrawal was right and the split is what it produced.
