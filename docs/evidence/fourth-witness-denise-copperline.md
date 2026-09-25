# Two more implementations, read against the findings

Information for developers. **Neither of these is an oracle**, and neither
decides anything — the manual does. They are here because when four independent
implementations do the same thing and the printed sentence says otherwise, that
is worth knowing before anyone changes RTL; and because when they disagree with
each other, the finding is weaker than it looked.

[`third-witness.md`](third-witness.md) sets out the rules for consulting an
implementation at all. They apply unchanged here.

| | Source | Commit | Language |
|---|---|---|---|
| **Denise** | [piciji/denise](https://github.com/piciji/denise) | `1cb7d45` | C++, cycle-accurate, A1000/A500/A600 |
| **Copperline** | [CopperlineHQ/Copperline](https://github.com/CopperlineHQ/Copperline) | `726bed0` | Rust, OCS/ECS/AGA on one clock timeline |

Every citation below is a file and line at those commits. Nothing was run;
these are source readings, and they carry the assurance a source reading carries
and no more.

## The one that changes an existing finding

### NM-HRM-002 — audio stereo sides: the 2–1 became 2–2

The register currently says two emulators settle this against the RTL. That is no
longer supportable as written.

| | channels 0 and 3 go | agrees with |
|---|---|---|
| chapter 5, printed p.137 | **right** | — |
| appendix pin list | **left** | — |
| WinUAE | right | chapter 5 |
| vAmiga | right | chapter 5 |
| **Denise** `paula/paula.cpp:354` | **left** | the pin list — and NanoMig |
| **Copperline** `chipset/paula.rs:2180` | **left** | the pin list — and NanoMig |
| NanoMig | left | the pin list |

```cpp
// Denise, paula.cpp:354
int32_t sampleL = channels[0].sample + channels[3].sample;
```

Copperline says why, in the code:

> "Paula stereo routing follows the common A500/A600/A1200 and Minimig mapping:
> channels 0 and 3 go left, 1 and 2 right. **Some HRM prose and motherboard jack
> labels describe the opposite physical side**; keep this as channel-to-DAC
> routing."

So one of the four knows about the contradiction and chose the pin list
deliberately. **Two implementations on each side.** The finding's red stands —
the manual still contradicts itself and NanoMig still has to pick a side — but
the sentence "two emulators settle it" has to go.

## Where a fourth implementation matches the manual for the first time

### NM-HRM-003 and NM-HRM-022 — the pointer width

Until now no implementation narrowed a DMA pointer by chipset. Copperline does,
and its OCS width is exactly the manual's.

```rust
// Copperline, chipset/agnus.rs:113
pub fn dma_addr_capability_mask(self) -> u32 {
    match self {
        Self::Ocs                     => 0x0007_FFFF,   // 19 bits = 3 high + 16
        Self::Ecs8372Rev4             => 0x000F_FFFF,   // 20 bits
        Self::Ecs8375 | Self::AgaAlice=> 0x001F_FFFF,   // 21 bits = 5 high + 16
    }
}
```

Applied in `bus.rs:4990` to Agnus, Denise, the blitter, Paula and the floppy
alike, under a comment that states the intent plainly:

> "The writable DMA pointer high bits follow the Agnus revision's address-bus
> reach as well as the installed chip RAM: an 8372A drops bit 20 even with 2 MB
> fitted, and **OCS stops at 512 KiB**."

**Denise does the opposite and applies no mask at all** —
`agnus/copper.cpp:347` is `cop1lc = (cop1lc & 0xffff) | (value << 16);`, which
takes the whole sixteen-bit write.

| | narrows the pointer by chipset |
|---|---|
| WinUAE | no |
| vAmiga | no (narrowed on access, not by chipset) |
| Denise | **no** — no mask on the high word at all |
| **Copperline** | **yes, to 19 bits on OCS** |
| NanoMig | no |

One implementation of four now does what the manual describes. That does not
settle NM-HRM-003 or NM-HRM-022, and it is the first evidence that the manual's
sentence is implementable rather than merely printed.

## Where the additional witnesses strengthen a finding

### NM-HRM-020 — BLTSIZV is not gated on ECS

Both gate it, and Denise gates all three ECS blitter registers in one block:

```cpp
// Denise, agnus/register.cpp:220-230
case 0x5a: if (ecsAndHigher()) blitter.setBltCon0L(value); break;
case 0x5c: if (ecsAndHigher()) blitter.setBltSizeV(value); break;
case 0x5e: if (ecsAndHigher()) { blitter.setBltSizeH(value); ... } break;
```

```rust
// Copperline, bus/custom_regs.rs:787
0x05C => {
    if !self.blitter_ecs_registers_enabled() { return false; }
    ...
    self.blitter.bltsizv = val & 0x7FFF;
}
```

**Four implementations gate BLTSIZV on ECS. NanoMig alone does not** — and its
own comment on `agnus_blitter.v:548` calls the register ECS while the code does
not gate it. This is now the best-corroborated finding in the register.

### NM-HRM-019 — SPRxCTL bit 4 (SHSH1) is not implemented

Both implement it, from bit 4 exactly:

```cpp
// Denise, video/denise.cpp:299
spr.shsh10 = (value & 0x10) >> 3;
```
```rust
// Copperline, video/bitplane/sprite.rs:1857
pub(super) fn sprite_hsub_70ns(ctl: u16) -> bool { ctl & 0x0010 != 0 }
```

And **Denise scopes its use exactly as page 303's Warning requires** — the
sub-pixel phase is compared only in SuperHires:

```cpp
// video/denise.cpp:574
if (spr0.armed && (hPos == spr0.x) && (!_shres || ((p & 2) == spr0.shsh10)))
```

This was previously the weakest of the chapter-3 sprite findings, because the
first two emulators disagreed with each other. Two more implement it.

### NM-HRM-023 — the ATTACH bit is honoured in the even sprite

Denise applies the manual's rule in one line, and the `nr & 1` is the whole
difference:

```cpp
// Denise, video/denise.cpp:300
spr.attached = (nr & 1) && (value & 0x80);
```

Against NanoMig's `denise_sprites.v:474`, `attach1 || (!aga && attach0)`.

Copperline reads the bit without the odd-sprite condition —
`video/bitplane/sprite.rs:322`, `attached: ctl & 0x0080 != 0`, and
`collect_attached_beams` at `:1102` chains the even and odd lines and filters on
`attached`, so an even sprite's bit counts there too. **That is a source reading,
not a measurement**, and it should be confirmed by running before it is quoted as
Copperline's behaviour.

So: one implementation matches the manual exactly, one appears to do what NanoMig
does.

### NM-HRM-021 — DOFF has no observable effect

Denise implements it **and** gates it on ECS, which is both halves of what
appendix C describes:

```cpp
// Denise, agnus/blitter.cpp:297
doff = agnus.ecsAndHigher() && (bltcon1 & 0x80);
```

In Copperline the string appears only in a comment in `chipset/blitter.rs:359`;
no decode was found for BLTCON1 bit 7. Recorded as not-found rather than
not-implemented — a grep is weaker than a reading.

WinUAE implements DOFF, vAmiga does not. With Denise, that is two of four
implementing it.

### NM-HRM-015 — BRDNTRAN is declared and never connected

**Copperline implements it and has a test for it** —
`video/bitplane/tests.rs:1389`, `bplcon3_brdntran_keeps_border_opaque_for_color_key`,
driving `BPLCON3_BRDNTRAN`. Denise does not mention the name anywhere in its
Amiga core.

The earlier reading recorded that both of the first two emulators show no RGB
effect and called it a genlock concept. One of the two new implementations
treats it as a real, testable border behaviour.

## Where nothing was found, and that is the report

`grep` over both cores found no chipset-revision branch in Denise's Agnus at all,
which is why its pointer takes the full write. For NM-HRM-004 both new
implementations agree with NanoMig and against the printed sentence:

```rust
// Copperline, bus/dma_slots.rs:697
pub(super) fn copper_can_write_custom(&self, off: u16) -> bool {
    let off = off & 0x01FE;
    if off <= 0x03E {
        return !matches!(self.agnus.revision(), AgnusRevision::Ocs)
            && self.agnus.copper_danger_enabled();
    }
    if (0x040..=0x07E).contains(&off) {
        return self.agnus.copper_danger_enabled();
    }
    true
}
```
```cpp
// Denise, agnus/copper.cpp:342
cdang = (value & 2) ? (agnus.ecsAndHigher() ? 0 : 0x40) : 0x80;
```

Both require the danger bit **set** to reach `$040`–`$07E`. Appendix C p310 says
that on ECS a **clear** danger bit reaches `$03E`–`$07E`. **Four implementations
now read that sentence the other way**, which is what `findings.md` already says
about it: read literally, clearing the bit would grant the range that setting it
grants, leaving the bit with nothing to do.

Copperline adds one nuance nobody else has: `$000`–`$03E` is reachable on
**non-OCS only**, and with the danger bit set. That is a chipset distinction the
manual does not make in that range.

## The second reading, and the finding it splits

Eight findings carried dashes after the first pass. They were read rather than
searched, and seven of the eight came back with an answer.

### NM-HRM-016 and NM-HRM-017 — SuperHires colour: a second 2–2

The register said *"both side with the manual; NanoMig is the outlier"*, on the
strength of WinUAE and vAmiga. That is no longer the whole picture.

**Denise keeps one colour per register in every mode.** `setColor` is three lines
and none of them looks at the resolution:

```cpp
// Denise, video/denise.cpp:191-200
auto Denise::setColor( uint8_t pos, uint16_t value ) -> void {
    process();
    colors[pos] = value;
```

`colors[]` is the only colour array in the core. Every use of `_shres` in
`Denise::process` is pixel rate, a playfield mask, the sprite sub-pixel phase
comparison, or the line-position wrap — never a colour-table read.

**Copperline does not pair-encode either, and says so in a test's own words:**

```rust
// Copperline, video/bitplane/tests.rs:7563
fn shres_playfield_output_resolves_each_35ns_sample_through_the_palette() {
    // A solid run of colour 1 keeps colour 1: each 35 ns half resolves
    // palette[1], never a pair-encoded entry.
```

| | reinterprets the colour registers in SuperHires |
|---|---|
| WinUAE | yes, ECS Denise only |
| vAmiga | yes, ECS Denise only |
| **Denise** | **no** — one table, no mode |
| **Copperline** | **no** — stated deliberately, with a test |
| NanoMig | no |

**Two against two.** The second finding after NM-HRM-002 where the four split
evenly, and the same lesson: *two implementations agreeing is not four.* The reds
stand — the manual describes a feature NanoMig has not built — but "both side with
the manual" was a statement about how many had been read.

### NM-HRM-006 — the DDF mask: four of four, and one of them says why

All four gate the DDF register granularity on the **chipset**. Not one of them
gates it on resolution.

```cpp
// WinUAE, custom.cpp:1210 -- applied at :1113-1114 and at the DDFSTRT/DDFSTOP
// writes, :3663 and :3674
ddf_mask = ecs_agnus ? 0xfe : 0xfc;
```
```cpp
// Denise, agnus/register.cpp:293 and :306
ddfStart = value & (ecsAndHigher() ? 0xfe : 0xfc);
ddfStop  = value & (ecsAndHigher() ? 0xfe : 0xfc);
```
```rust
// Copperline, chipset/agnus.rs:490
pub(crate) fn ddf_register_mask(revision: AgnusRevision) -> u16 {
    if matches!(revision, AgnusRevision::Ocs) { 0x00FC } else { 0x00FE }
}

pub fn effective_bitplane_ddf_start_hpos(revision: AgnusRevision, bplcon0: u16, raw: u16) -> u16 {
    let _ = bplcon0;
    raw & ddf_register_mask(revision)
}
```

**Copperline takes the resolution register as a parameter and throws it away.**
`let _ = bplcon0;` is as explicit as a source can be: the signature admits that
resolution is the thing one might expect to matter here, and the body says it
does not.

Printed page 91's *"bit H3 only respected in Hires Mode"* now stands against four
independent implementations and NanoMig. The finding's red is correct — the manual
is this project's oracle — but the question for the specialists has moved: it is
whether that sentence survived from an OCS-era draft, not whether NanoMig is wrong.

### NM-HRM-008 — playfield-1 collision: four of four

NanoMig's `oddmatch = ... & (dblpf | evenmatch)` is not a NanoMig peculiarity.

```cpp
// Denise, video/denise.cpp:478-480 -- the SPF macro
if (!clxNoCol)                                          clxDat |= (v1 << 5) | (v1 << 1) | 1;
else if ((clxNoCol & 2) == 0)                           clxDat |= (v1 << 5);
else if (((clxNoCol & 1) == 0) && _doublePlayfield)     clxDat |= (v1 << 1);
```
```rust
// Copperline, video/bitplane.rs:6797
let odd_match = odd_match_raw && (dual_playfield || even_match);
```

Copperline has a test named for exactly this:
`collision_pixel_single_playfield_odd_match_requires_even_match`.

**All four make playfield 1's collision depend on playfield 2 when a single
playfield is displayed.** The RTL comment crediting an undocumented Denise
function is corroborated by every other implementation. Appendix A's table of
independent collision bits is the thing that is incomplete.

### NM-HRM-009 — CLXDAT bit 15: unanimous

```
WinUAE     custom.cpp:4016      uae_u16 v = clxdat | 0x8000;
vAmiga     DeniseRegs.cpp:319   u16 result = clxdat | 0x8000;
Denise     denise.cpp:313       uint16_t out = clxDat | 0x8000;
Copperline denise.rs:538        std::mem::take(&mut self.clxdat) | 0x8000
```

Four independent implementations, one line each, the same constant. The
withdrawal of this finding was right and is now unanimous.

### NM-HRM-012, NM-HRM-013 and NM-HRM-018 — the SPRxCTL and BPLCON0 bits

WinUAE computes the sprite control mask per chipset in one place:

```cpp
// WinUAE, custom.cpp:1198-1203
if (aga_mode)        sprite_sprctlmask = 0x01 | 0x08 | 0x10;
else if (ecs_denise) sprite_sprctlmask = 0x01 | 0x10;
else                 sprite_sprctlmask = 0x01;
```

Bit 0 in every chipset, bit 4 from ECS Denise, bit 3 only on AGA — which is the
split NM-HRM-013 was withdrawn into and NM-HRM-019 carries. **But
`sprite_sprctlmask` is written at those three lines and read nowhere else in this
checkout**, so it records an intent and is not evidence of behaviour. Said plainly
because a citation that looks decisive and is dead is worse than a dash.

Denise and Copperline both take SH0 unconditionally — `spr.x |= value & 1;`
(`denise.cpp:298`) and `((pos & 0x00FF) << 1) | (ctl & 0x0001)`
(`sprite.rs:1854`) — with no resolution test anywhere near them, exactly as
NanoMig does. Neither reads bit 3.

For NM-HRM-018, Denise's `setBplCon0` never mentions bit 4 at all. Copperline
does use it — as AGA's BPU3, requesting eight bitplanes — and only inside
`if aga`:

```rust
// Copperline, chipset/denise.rs:345
let planes = if bplcon0 & 0x0010 != 0 { 8 } else { code };
```

That is a different register function from the SPRHWRM the ECS appendix names.
**Neither gives bit 4 an effect on ECS.**

### NM-HRM-001, NM-HRM-010, NM-HRM-011 — the beam

- **313 PAL lines.** Copperline states it as a constant, `PAL_LINES = 313`
  (`agnus.rs:7`), and non-interlaced fields are all long frames. Denise reaches
  the same number the other way: `lines = ntsc ? 261 : 311` (`agnus.cpp:178`) with
  the frame restarting at `vPos == (lines + lof)`. All four run a 313-line field.
- **The half colour clock is an alternating whole line, not a fraction.** Denise
  computes NTSC as `227.5 * 263.0` *with the long/short line flip-flop doing the
  alternation*, and PAL as a flat `227.0 * 313.0` (`agnus.cpp:1030-1032`);
  Copperline carries an integer `COLORCLOCKS_PER_LINE` plus a separate
  `NTSC_LONG_COLORCLOCKS_PER_LINE`. Both say what the source observation says.
- **The PAL vertical blank does not settle.** Denise's fixed blank clears at
  `vPos == (ntsc ? 19 : 24)` (`agnus.cpp:448`) — a PAL-minus-NTSC gap of five
  lines, which is NanoMig's gap and not the manual's eight. Copperline models the
  *programmable* blank under VARVBEN and anchors its render window at vpos `$2C`,
  which is the display window's top, a different quantity. **Not comparable
  without running it**, and recorded that way rather than forced into a number.

## What is still a dash, and why

| finding | missing | why |
|---|---|---|
| NM-HRM-005 | WinUAE, Denise, Copperline | Copper WAIT/SKIP cycle *times*. A count in a source is not a time on a bus; this one needs the emulators run, not read. |
| NM-HRM-007 | Denise | its fetch unit was not traced far enough to say whether an odd hires word count is producible. |
| NM-HRM-010 | WinUAE, vAmiga | not read for this; Denise and Copperline were enough to make the point. |
| NM-HRM-012, NM-HRM-013 | vAmiga | its SPRxCTL path defers the decode to a scheduled register change, and following it to the display was more than the question needed. |
| NM-HRM-018 | WinUAE, vAmiga | not read for this. |

**A dash in a finding's table means nobody has read that source for that
finding.** It does not mean the source is silent, and it should not be quoted as
if it were.
