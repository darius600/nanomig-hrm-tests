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

Read 12.09.2026 at those commits. Every citation below is a file and line in
them. Nothing was run; these are source readings, and they carry the assurance a
source reading carries and no more.

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

Against NanoMig's `denise_sprites.v:484`, `attach1 || (!aga && attach0)`.

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

## What was not checked

NM-HRM-005 (WAIT/SKIP cycle times), NM-HRM-006 (DDFSTRT bit H3),
NM-HRM-007 (49-word hires fetch), NM-HRM-008 (playfield-1 collision),
NM-HRM-011 (PAL vertical blank), NM-HRM-016 and NM-HRM-017 (SuperHires colour
encoding). Those need a reading rather than a grep, and this pass did not do one.
Saying so is cheaper than a half-answer.
