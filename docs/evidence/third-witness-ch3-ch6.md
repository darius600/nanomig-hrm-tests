# Chapters 3 and 6: what two emulators do about ten unregistered reds

A report for whoever decides what NanoMig should do. **Nothing here changes a
test**, and no expectation in the suite comes from an emulator.

> **Read the A/B/C letters as "two of four", not "the answer".** This document
> compares WinUAE and vAmiga only. Two further implementations have since been
> read against the same findings in
> [`fourth-witness-denise-copperline.md`](fourth-witness-denise-copperline.md),
> and they moved three of the verdicts below: **NM-HRM-016 and NM-HRM-017 are
> not A1 but a two-against-two split**, and **NM-HRM-006 and NM-HRM-008 are not
> B-with-two but B-with-four.** The sections here are left as they were written,
> because what two emulators did is still what two emulators did; the register
> table in [`findings.md`](../findings.md) carries the current count.

## Why this exists

`tools/list_reds.py` counts 67 failing assertions in 32 families.
**That figure is stale and is left as written.** It predates the runner
defect described in the README, where 59 packages wrote their summary to
`out.txt` and printed it on the next `make` line — so a package that failed
stopped before the print and vanished from the count exactly when it went
red. The suite-wide figure since that fix is **187 failing assertions**.
The count for these two chapters was never recomputed; take the numbers
below as the shape of the problem, not as its size, and read the current
count from the register table in [`findings.md`](../findings.md). Chapters 2 and
5 have every family registered in [`findings.md`](../findings.md) — NM-HRM-003,
NM-HRM-004, NM-HRM-005, NM-HRM-002 — and three of those already carry an emulator
comparison in [`third-witness.md`](third-witness.md).

Chapters 3 and 6 have seventeen families between them and **not one is in the
register**. They exist only as a FAIL line. Both chapters were accepted with
those reds present, so this is not new breakage; it is a register that was never
written.

## The rule, unchanged

An emulator is **not** an oracle. It is a witness about the hardware, which is a
different thing from a witness about the manual, and it never cancels a finding
or supplies an expectation. Read the arithmetic, not the comments; cite the
commit, not the branch. Both trees were read at the same commits
[`third-witness.md`](third-witness.md) already names — vAmiga `35de004a`,
WinUAE `ccc407e9` — so this report and that one compare like with like.

---

## A. Both emulators side with the manual, against NanoMig

*The strongest case for looking at the RTL.*

### A1 — Super-hires does not reinterpret the colour registers (35 assertions)

| | |
|---|---|
| Manual | pp.297, 301: in SHRES the same COLORxx registers are reinterpreted — lower sixteen registers, two bits of resolution per component |
| NanoMig | plain reading in every mode: `0x0C39` comes out R=$C, G=$3, B=$9 |
| Tests | `ch3-shres-bitplane-colour-encoding` (19), `ch3-shres-sprite-colour-encoding` (16) |

**Both emulators reinterpret, and their arithmetic is the same.**

vAmiga, `Core/Components/Denise/Colors.cpp:126-148` — each channel's nibble split
into its top two and bottom two bits, each replicated:

```cpp
// shresHi: bits 7 and 6 of each channel, replicated (0,1,2,3 -> $00,$55,$AA,$FF)
return AmigaColor { u8(((r >> 6) & 3) * 0x55), ... };
// shresLo: bits 5 and 4, the same way
return AmigaColor { u8(((r >> 4) & 3) * 0x55), ... };
```

WinUAE, `drawing.cpp:2991-3001`, the same mapping written as masks:

```c
uae_u8 off = ((p1 & 3) * 4) + (p0 & 3) + ((p0 | p1) & 16);
v = (denise_colors.color_regs_ecs[off] & 0xccc) << 0;  v |= v >> 2;   // top two bits
v = (denise_colors.color_regs_ecs[off] & 0x333) << 2;  v |= v >> 2;   // bottom two
```

Both restrict it to the lower sixteen registers, and both pair two adjacent
super-hires pixels into one 4-bit index — vAmiga `PixelEngine.h:301-305`, WinUAE
the `((p1&3)*4)+(p0&3)` term above. Arithmetically identical.

**The chipset condition matters and the tests already get it right.** Both
emulators apply this to **ECS Denise only**; for AGA both fall back to a plain
per-pixel lookup (vAmiga `PixelEngine.cpp:214-222`, `shresModeEcs()` requires
`revision == DeniseRev::ECS`; WinUAE uses a separate `linetoscr_aga_fm0.cpp`
family). These packages drive `ecs = 1, aga = 0` and name AGA's separate path as
an explicit non-goal in their own headers, so they sit squarely in the case
where both emulators reinterpret.

**In NanoMig the SHRES path exists for geometry but not for colour.**
`denise.v:187` decodes `shres = bplcon0[6]` and `:362` feeds `shres & ecs` to the
bitplane shifter, so pixel timing follows it — but `denise_colortable.v` does not
contain the string `shres` at all. The colour lookup is resolution-blind.

### A2 — BLTSIZV is not gated on ECS, while its two siblings are

| | |
|---|---|
| Manual | Appendix C pp.309/310 marks BLTSIZV new for ECS, the same status as BLTSIZH and BLTCON0L |
| NanoMig | BLTSIZH and BLTCON0L are gated; BLTSIZV takes effect with `ecs = 0` |
| Test | `ch6-blitter-ecs` / `BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS` |

Both emulators gate all three identically. vAmiga `BlitterRegs.cpp` lines 35,
324, 347 — `if (agnus.isOCS()) return;` before the value can reach the register.
WinUAE `custom.cpp` lines 3776, 3915, 3922 — `if (!ecs_agnus) return;` and
`if (ecs_agnus) { ... }`.

**In NanoMig this is one missing term.** `agnus_blitter.v`:

```verilog
259:  … reg_address_in[8:1]==BLTCON0L[8:1] && ecs     ← gated
500:  … reg_address_in[8:1]==BLTSIZH[8:1]  && ecs     ← gated
523:  … reg_address_in[8:1]==BLTSIZH[8:1]  && ecs     ← gated
548:  … reg_address_in[8:1]==BLTSIZV[8:1]             ← not gated
557:  … reg_address_in[8:1]==BLTSIZH[8:1]  && ecs     ← gated
```

Line 548's own comment reads `// ECS BLTSIZV register`. Every sibling carries
`&& ecs`; this one does not.

---

## B. Both emulators side with NanoMig, against the manual

*Here the question stops being "does the RTL follow the manual" and becomes "is
the manual complete about the hardware" — which is not this project's to answer.*

### B1 — Playfield-1 collisions are gated by playfield 2 (4 assertions)

| | |
|---|---|
| Manual | Appendix A p.273: PF1 with SPR01/23/45/67 sets CLXDAT bits 1/2/3/4, each pair its own proposition |
| NanoMig | none of the four is set with `dblpf = 0` |
| Test | `ch3-collision-registers` / `BITMAP-PAIR` |

NanoMig, `denise_collision.v:62-63`:

```verilog
assign oddmatch  = bm[6] & bm[4] & bm[2] & bm[0] & (dblpf | evenmatch);
assign evenmatch = bm[7] & bm[5] & bm[3] & bm[1];
```

In single-playfield mode PF1 only matches if PF2 also matches. The RTL's own
comment calls this *"an undocumented function in the real Denise chip … trigger
was the game Rotor"*.

**Both emulators implement the same gate.** vAmiga `Denise.cpp:1554-1561`:
`else { if (!(zBuffer[pos] & Z_DPF)) continue; }` — PF2 failing to match skips
the PF1 test entirely unless the pixel is dual-playfield. WinUAE `drawing.cpp:
4510-4536`: in the non-`bpldualpf` branch the PF1 test is *nested inside* the PF2
test.

Everything else about the four cases agrees across all three: the CLXCON odd/even
bitplane split, the per-pair sprite enable, the bit assignment.

**Worth knowing if you reproduce this:** vAmiga gates playfield-sprite collision
behind `config.clxSprPlf`, which defaults to **false**
(`Core/Infrastructure/Defaults.cpp:174`). WinUAE's `collision_level` defaults to
3, on.

### B2 — BLTxPTH carries five high bits in both chipsets

| | |
|---|---|
| Manual | Appendix A p.270: "Blitter pointer to x (high 3 bits, 5 bits if ECS)" |
| NanoMig | five in both |
| Test | `ch6-blitter-ecs` / `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` |

Neither emulator narrows the **register**. vAmiga stores the full 16-bit value
and only logs a diagnostic (`BlitterRegs.cpp:148-162`); its chipset-dependent
`ptrMask` is applied later and generically to *every* Agnus access — Copper and
blitter alike, through the same `Memory::peek16<Accessor::AGNUS>`. WinUAE has no
chipset test at all in that path (`custom.cpp:3800-3805`), only the configured
chip-RAM size.

**This is NM-HRM-003's shape at a second register.** That finding already records
both emulators declining to narrow the *Copper's* pointer by chipset; vAmiga
funnels both through the same masking function, so the two are one behaviour.
Consider registering them together.

### B3 — BRDNTRAN has no effect on the displayed border

| | |
|---|---|
| Manual | Appendix C p.307 names BPLCON3 bit 4 BRDNTRAN; p.296 "BorderNotTransparent makes an opaque 'frame' surrounding the active area" |
| NanoMig | setting it changes nothing observable |
| Test | `ch3-horizontal-display-window` / `D-BRDNTRAN-HAS-NO-EFFECT` |

The two emulators differ in mechanism and agree in effect.

vAmiga has **no accessor for bit 4 at all**. `Denise.h:1058-1072` names
`colorBank`, `pf2of`, `loct`, `brdrblnk` (bit 5) and `brdsprt` (bit 1); the
border-colour decision `Denise.cpp:1309-1329` consults only `brdrblnk`.

WinUAE decodes it — `drawing.cpp:3117`, `bordertrans = (bplcon3_denise & 0x10)
&& ecsena;` — but its only consumer is `get_genlock_transparency_border()`
(`drawing.cpp:1246-1256`), which writes into `xlinebuffer_genlock`, a buffer
distinct from the RGB output and populated only when a genlock peripheral is
emulated. `update_bordercolor()` never reads bit 4.

**What that clarifies:** the manual's "opaque frame" is a genlock keying concept,
not a paintable colour change. NanoMig's behaviour matches both emulators in
ordinary use. Whether NanoMig should implement genlock keying at all is a product
question, not a conformance one.

---

## C. The two emulators disagree with each other

*Unresolved by this method. Settling these needs real hardware, not a third
reading of the manual.*

### C1 — DOFF

| | |
|---|---|
| NanoMig | BLTCON1's DOFF bit has no observable effect |
| Test | `ch6-blitter-ecs` / `DOFF-NO-OBSERVABLE-EFFECT` |

**vAmiga: inert.** `BlitterTypes.h:33` defines `BLTCON1_DOFF = 0x0080` and
nothing in the tree ever reads it. The D-channel write
(`SlowBlitter.cpp:1114`) is unconditional.

**WinUAE: a real, ECS-gated effect.** `blitter.cpp:1541`,
`blit_dof = ecs_agnus && (bltcon1_next & BLTDOFF);` and its one consumer,
`blit_chipmem_agnus_wput` at `:467-478`:

```c
if (blit_dof) {
    w = regs.chipset_latch_rw;
}
chipmem_wput_indirect(addr, w);
```

The write cycle still happens, with its address stepping and timing; the value
stored is whatever was last on the chip bus instead of the blit result.

NanoMig matches vAmiga. Which of the two is right about the silicon is not
something this project can decide.

### C2 — SPRxCTL bit 4, SHSH1

| | |
|---|---|
| Manual | Appendix C p.303: `04 SHSH1 Start horizontal (SHR mode) 70ns increment` |
| NanoMig | setting SPR0CTL bit 4 alone, SH0 at 0, changes the sprite trace — live outside SHR |
| Test | `ch3-sprite-registers` / `SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE` |

**vAmiga: extracted, then discarded.** `Denise.cpp:167-177` packs the bit into
`sprhpos` unconditionally, but the comparator masks it away — `sprPixelMask()`
(`Denise.h:865-872`) clears bit 0 for HIRES and bits 0-1 for LORES, and
`sprPixelWidth()` (`DeniseRegs.cpp:817-855`) never returns 1 for non-AGA. On
ECS/OCS the bit is inert in every mode, SHR included.

**WinUAE: live, and specifically outside SHR.** `drawing.cpp:2822-2837` extracts
it gated on chipset only. `matchsprites2` at `:5326-5329`:

```c
// ECS Denise + hires sprite bit and hires: leftmost pixel is missing
if (ecs_denise_only && denise_res == RES_HIRES && (sp->ctl & 0x10)) {
    sp->dataas &= 0x7fff;
    sp->databs &= 0x7fff;
}
```

`denise_res` comes straight from BPLCON0, so this fires in **HIRES**, not SHRES.

NanoMig's report of "live outside SHR" is closer to WinUAE's shape, though
WinUAE ties the liveness to HIRES specifically. **Neither emulator gates the bit
as "SHR mode only"** the way the manual's one-line gloss implies — each makes it
conditional on more than that, in its own way.

---

### B4 — Hires fetches 50 words at both hardware limits, not 49 (2 assertions)

| | |
|---|---|
| Manual | p.80: "In high resolution mode the maximum here is 49 words, because the rightmost limit remains ($D8) and only one word is fetched at this limit" |
| NanoMig | 50, with DDFSTRT=$0018 and DDFSTOP=$00D8 |
| Tests | `ch3-ddf-fetch-window` (1), `ch3-bitplane-memory-layout` (1); a third, which required the count one start earlier to differ from 49, is declined -- p.80 fixes 49 only at $18/$D8 |

Neither emulator has a formula the answer can be read off — vAmiga has none at
all, and WinUAE's is a secondary buffer-sizing calculation. So the reading
re-implemented **both** cycle-by-cycle DDF state machines, OCS and ECS branches
separately, and ran them at that register pair rather than asserting a result.

**Four independent paths, all 50.** vAmiga OCS, vAmiga ECS, WinUAE OCS, WinUAE
ECS/AGA. WinUAE's separate closed-form `storelinestate` (`custom.cpp:10435-10444`)
lands on 50 as well:

```c
int len = ((stop - ddfstrt) + fetchunit - 1) / fetchunit + 1;   // (192+7)/8+1 = 25
len = len * fetchunit / fetchstart;                              // 25*8/4      = 50
```

**Both give the same mechanism**, and it explains the manual's error rather than
merely contradicting it: `$D8 − $18 = 192`, an exact multiple of the 8-CCK fetch
block. The stop request therefore lands *on* a block boundary and only arms the
last fetch unit, so one whole further block completes — two more fetches, 48 + 2.
The manual's "only one word is fetched at this limit" describes a stop that
lands mid-block, which this register pair never produces.

### B5 — DDFSTRT bit H3 is respected in lores too

| | |
|---|---|
| Manual | pp.91, 277: "(bit H3 only respected in Hires Mode.)" |
| NanoMig | changing it moves fetch timing in lores as well |
| Test | `ch3-ddf-fetch-window` / `DDFSTRT-H3-BIT-RESPECTED-ONLY-IN-HIRES-MODE` |

Both emulators mask DDFSTRT and DDFSTOP **by chipset generation only**, never by
resolution, and both masks keep bit 3:

```cpp
// vAmiga, Agnus.h:405        0xFC = 11111100, 0xFE = 11111110
u16 ddfMask() const { return isOCS() ? 0xFC : 0xFE; }
// WinUAE, custom.cpp:1210
ddf_mask = ecs_agnus ? 0xfe : 0xfc;
```

`isOCS()` and `ecs_agnus` are both chipset-model settings, unrelated to BPLCON0.
The reading searched both trees for any resolution-conditional masking of that
bit and found none in either.

### B6 — PAL vertical blank lasts 26 lines, not 29

| | |
|---|---|
| Manual | Table 3-13: PAL "Vertical Blank Stop" $1D — 29 blanked lines, printed with 312 − 29 = 283 displayable |
| NanoMig | vbl reads high for 26 consecutive lines from vpos=0 |
| Test | `ch3-beam-counters-and-blanking` / `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL` |

Both hardcode 26, neither derives it from a register:

```cpp
// vAmiga, Agnus.h:429 -- true for vpos 0..25
bool inVBlankArea(isize posv) const { return posv < (isPAL() ? 26 : 20); }
// WinUAE, custom.cpp:10088 in the PAL branch, plus
// include/custom.h:122   #define VBLANK_ENDLINE_PAL 26
if (vpos == 25) { agnus_vb = 0; agnus_vb_end_line = true; }
```

In vAmiga this gates real DMA behaviour, not just display geometry — sprite DMA
enable is conditioned on it at `Agnus.cpp:854` and `AgnusRegs.cpp:489`, `:543`.

---

## The tally

| | findings | assertions |
|---|---:|---:|
| **A** — both emulators with the manual, against NanoMig | 2 | 36 |
| **B** — both emulators with NanoMig, against the manual | 6 | 10 |
| **C** — the emulators disagree with each other | 2 | 2 |

Six of the ten reds are places where two independently written implementations
do what NanoMig does. That does not make the manual wrong — **this project does
not get to decide that**, and a red stays a red until someone who can, does. What
it does mean is that six of these are not usefully described as "the RTL departs
from the specification"; the honest description is that the printed sentence and
three implementations disagree, and only one of the four can be checked against
silicon.

Two are the other shape, and those are where an RTL change would be worth
discussing: **super-hires colour reinterpretation**, absent from
`denise_colortable.v` entirely, and **BLTSIZV's missing `&& ecs`**, a single term
its own comment implies should be there.

---

## What was not resolved, stated by the readers themselves

- Whether WinUAE's "fast" line-buffer SHRES path can ever select colour registers
  16-31 through the `+ ((p0|p1) & 16)` term. The cycle-exact path provably
  cannot; the fast path was left untraced rather than guessed.
- Why the manual's SHRES colour reinterpretation applies to ECS and not AGA.
  Both emulators encode the split structurally, neither states the reason in
  anything but comments, which this method excludes as evidence.
