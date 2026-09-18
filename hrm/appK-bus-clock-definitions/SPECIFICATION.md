# SPECIFICATION — `appK-bus-clock-definitions`

Source: *Amiga Hardware Reference Manual*, 3rd ed., Appendix K, "Signal
Descriptions" — printed page 393 (Zorro II "CLOCK SIGNALS") and printed page
410 (Zorro III "CLOCK SIGNALS"). Two copies of the same five-entry glossary
(/C1, /C3, CDAC — Zorro III only — E, 7M), read twice each (`sol`, `opus`).

Claims: `hrm-testpipeline/work/appK-build/appK-bus-clock-definitions.jsonl`,
31 rows, `classify.uid` (`reader:id`) verbatim throughout — never the bare
`id`, since `sol` and `opus` id spaces collide across this file.

**Module boundary**: `amiga_clk.v` (`nanomig/src/minimig-aga/amiga_clk.v`),
compiled alone. Every claim carries `"observable": "yes, at amiga_clk"`.
Ports: `clk_28` (in), `clk7_en`, `clk7n_en`, `c1`, `c3`, `cck`, `eclk[9:0]`
(out), `reset_n` (in, async, active low). No video-mode input exists on this
module at all — confirmed by reading the full port list above, not assumed
from the build brief.

## Two boundary facts that apply across most of this file

**No video-mode input.** Every printed frequency in this glossary is a pair,
NTSC/PAL ("7.16 MHz ... 7.09 MHz on PAL systems"), and `amiga_clk` has no
port that selects between them — the pair is a property of which crystal
drives the board's `clk_28`, external to this module. So no claim below is
graded against one specific absolute-Hz figure. What is graded is the
**ratio to `clk_28`**, which the RTL fixes independently of which crystal is
fitted (a divide-by-N built from a free-running counter, not a mode-selected
one) — `clk_28`/4 reads as 7.16 MHz on an NTSC board and 7.09 MHz on a PAL
one, the same hardware ratio either way. This is recorded here, once, rather
than as 22 separate declines, and repeated as a `note()` in the harness.

**`eclk` is ten-phase one-hot, not a level.** `amiga_clk.v:12` types it
`[9:0]`; `:22,36-37` show a ten-bit register with exactly one bit set,
rotated once per `clk_7` period (`if(!shifter) shifter<=1` is a self-healing
guard, not the normal path); `:64 assign eclk = shifter`. A printed duty
cycle ("four high, six low") describes a two-level signal. Reducing a
one-hot vector to a level requires picking a bit (or a set of bits) and a
convention for what "high" means across them — a choice this module's port
alone does not supply, and inventing one would be inventing a requirement,
which the build brief forbids. The harness proves the one-hot invariant by
measurement (every sample, every phase) rather than citing the RTL alone,
and the four `E`-clock-duty uids below are declined on that measurement.

## Fixture

One top module, `amiga_clk`, driven at its own two inputs (`clk_28`,
`reset_n`) and read at its five outputs. `clk_28` is toggled by `tick()`;
`reset_n` is held low for the first few ticks then released. All 31 claims
are graded (or declined) from one continuous post-reset trace — there is no
data stimulus to vary claim-by-claim, since this module's only inputs are
the clock it divides and its own reset.

## What each claim asserts, and what will be measured

### `/C1` and `/C3` frequency (`sol:K-0123`, `sol:K-0126`, `sol:K-0402`, `sol:K-0405`)

Quote: "This is a 3.58 MHz clock (3.55 MHz on PAL systems)." Asserts that
`/C1` (resp. `/C3`) has **half** the period-ratio of the page's own `7M`
entry (3.58 is 7.16/2). Measured: the number of `clk_28` edges between two
same-direction transitions of `c1` (resp. `c3`), against the same count for
`7M`'s own reference (see below). Fails if the ratio measured is 1:1 rather
than 1:2.

### `/C1` and `/C3` edge synchronisation (`sol:K-0124`, `sol:K-0127`, `sol:K-0403`, `sol:K-0406`)

Quote: "synched to the falling [rising] edge of" 7M. `7M` itself is not a
port; `clk7_en`/`clk7n_en` (port comments: "7MHz output clock enable" /
"7MHz negedge output clock enable") are this module's own, port-legal stand-
ins for 7M's rising and falling edge respectively — the same two signals
the claim's own citation in `classify.evidence` names. Asserts that `c1`'s
(resp. `c3`'s) transitions occur **only** in step with the falling (resp.
rising) marker, never the other one. Measured: for every `clk7_en` pulse and
every `clk7n_en` pulse in the trace, whether `c1` (resp. `c3`) changes value
on the following `clk_28` edge. Fails if the target signal follows both
markers, not just its claimed one.

### `/C3` as "the complementary half-rate bus clock" (`sol:K-0404`)

Heading-only quote ("/C3 Clock"), but this reader's restatement adds
"complementary half-rate" — a specific ratio-and-phase claim, not a bare
existence claim (contrast `sol:K-0122`/`sol:K-0125` below, whose restatement
adds nothing beyond "is a bus clock"). Asserts `c3` runs at half `c1`'s rate
and 180° (half of `c3`'s own period) out of phase with it. Measured: the
`c1`:`c3` period ratio (expect 2:1) and the `c1`-to-`c3` transition offset in
`clk_28` edges relative to `c3`'s own half-period (expect exactly half).

### `/C1` and `/C3` headings alone (`sol:K-0122`, `sol:K-0125`)

Quotes: "/C1 Clock" / "/C3 Clock" (headings, `corpus_damage` on `K-0122`:
p.393 prints "/C l"). Restatement adds only "is a bus clock" — no
frequency, no ratio, no phase. Asserts existence: `amiga_clk` (the clock-
generation module) produces `c1` (resp. `c3`) as a genuine, live, periodic
signal, not a stuck one. Measured: `c1`/`c3` each show a well-defined,
non-zero period in the trace.

### `E` clock identity — who generates it, who it drives (`sol:K-0130`, `sol:K-0131`, `opus:K-0139`, `sol:K-0409`, `opus:K-0419`)

Quotes: "the 68000 generated 'E' clock" / "used for 6800 family peripherals
driven by 'E' ... 6502 peripherals driven by 02." Both halves name something
outside `amiga_clk`'s own boundary: the source (a 68000, or any CPU
signal at all — this module has no CPU-originated input of any kind, only
`clk_28` and `reset_n`) and the consumers (6800/6502-style peripherals —
`ciaa`/`ciab`, not instantiated in a single-top-module `amiga_clk` fixture).
**Declined.** `eclk`'s own liveness (that it is a real, rotating, non-stuck
output) is measured and noted regardless, so the decline is not silent
about what the module does show.

### `E` clock duty cycle (`sol:K-0133`, `opus:K-0140`, `sol:K-0410`, `opus:K-0420`)

Quote: "four 7M clocks high, six clocks low." **Declined** — see "`eclk` is
ten-phase one-hot" above. The one-hot invariant is measured across the full
trace before the decline is written.

### `7M` frequency (`sol:K-0135`, `opus:K-0142`, `sol:K-0411`, `opus:K-0421`)

Quote: "the 7.16 MHz system clock (7.09 MHz on PAL systems)." `7M` is not
itself a port; its own rising-edge marker, `clk7_en`, pulses once per `7M`
period by construction (`amiga_clk.v:43-44`), so the `clk_28`-edge distance
between successive `clk7_en` pulses **is** `7M`'s own period, read without
touching anything inside the module. Measured: that distance, expected 4
(`clk_28`/4).

### `CDAC` frequency and phase (`sol:K-0407`, `sol:K-0408`, `opus:K-0418`)

Quotes: "the 7.16 MHz system clock (7.09 MHz on PAL systems) which trails
the 7M clock by 90° (approximately 35ns)." No port is named `CDAC`
(`grep -rniE 'CDAC' nanomig/src/minimig-aga` returns nothing) — Zorro III
only, page 410 alone. `amiga_clk.v:58` makes exactly one 90°-delayed copy of
`7M` in this design: `c3 <= clk_7`, one `clk_28` period (35.2 ns at
28.375160 MHz) after `clk_7` itself. Asserts `CDAC`'s frequency matches
`7M`'s own ratio and its phase trails by one quarter of that shared period.
Measured: `c3`'s period against `7M`'s reference period (expect equal, 1:1 —
**not** the 1:2 a proper "half-rate `/C3`" would need), and the `clk_28`-edge
offset from a `c1` transition (`c1` is shown below to be `7M`'s own
transition timing) to the next `c3` transition (expect exactly 1 edge = 90°
of a 4-edge period).

## Grading table

| uid(s) | page | topic | verdict |
|---|---|---|---|
| `sol:K-0122` | 393 | `/C1` is a bus clock (heading only) | **ASSERT — PASS** |
| `sol:K-0123` | 393 | `/C1` = 3.58/3.55 MHz | **ASSERT — RED** |
| `sol:K-0124` | 393 | `/C1` synced to 7M falling edge | **ASSERT — RED** |
| `sol:K-0125` | 393 | `/C3` is a bus clock (heading only) | **ASSERT — PASS** |
| `sol:K-0126` | 393 | `/C3` = 3.58/3.55 MHz | **ASSERT — RED** |
| `sol:K-0127` | 393 | `/C3` synced to 7M rising edge | **ASSERT — RED** |
| `sol:K-0130` | 393 | E clock: 68000-generated, for peripheral interface | **DECLINE — boundary (no CPU input)** |
| `sol:K-0131` | 393 | E clock: used by 6800-family peripherals | **DECLINE — boundary (no peripheral present)** |
| `sol:K-0133` | 393 | E clock: four 7M clocks high, six low | **DECLINE — eclk is one-hot, not a level** |
| `sol:K-0135` | 393 | 7M = 7.16/7.09 MHz | **ASSERT — PASS** |
| `opus:K-0136` | 393 | `/C1` freq + falling-edge sync (combined) | **ASSERT — RED** |
| `opus:K-0137` | 393 | `/C3` freq + rising-edge sync (combined) | **ASSERT — RED** |
| `opus:K-0139` | 393 | E clock identity/consumer (combined) | **DECLINE — boundary** |
| `opus:K-0140` | 393 | E clock duty (four high/six low) | **DECLINE — eclk is one-hot** |
| `opus:K-0142` | 393 | 7M = 7.16/7.09 MHz | **ASSERT — PASS** |
| `sol:K-0402` | 410 | `/C1` = 3.58/3.55 MHz | **ASSERT — RED** |
| `sol:K-0403` | 410 | `/C1` synced to 7M falling edge | **ASSERT — RED** |
| `sol:K-0404` | 410 | `/C3` complementary half-rate bus clock | **ASSERT — RED** |
| `sol:K-0405` | 410 | `/C3` = 3.58/3.55 MHz | **ASSERT — RED** |
| `sol:K-0406` | 410 | `/C3` synced to 7M rising edge | **ASSERT — RED** |
| `sol:K-0407` | 410 | CDAC = 7.16/7.09 MHz | **ASSERT — PASS** |
| `sol:K-0408` | 410 | CDAC trails 7M by 90°/~35ns | **ASSERT — PASS** |
| `sol:K-0409` | 410 | E clock: 68000-generated | **DECLINE — boundary** |
| `sol:K-0410` | 410 | E clock: four high/six low | **DECLINE — eclk is one-hot** |
| `sol:K-0411` | 410 | 7M = 7.16/7.09 MHz | **ASSERT — PASS** |
| `opus:K-0416` | 410 | `/C1` freq + falling sync (combined) | **ASSERT — RED** |
| `opus:K-0417` | 410 | `/C3` freq + rising sync (combined) | **ASSERT — RED** |
| `opus:K-0418` | 410 | CDAC freq + phase (combined) | **ASSERT — PASS** |
| `opus:K-0419` | 410 | E clock identity/consumer (combined) | **DECLINE — boundary** |
| `opus:K-0420` | 410 | E clock duty (combined) | **DECLINE — eclk is one-hot** |
| `opus:K-0421` | 410 | 7M = 7.16/7.09 MHz | **ASSERT — PASS** |

22 ASSERT (9 PASS, 13 RED) + 9 DECLINE = 31. Every uid in the file appears
exactly once above.

## What the reds mean, read together

`c1` and `c3` are not, in this RTL, a proper half-rate `/C1`/`/C3` pair.
Measured (see harness output): `c1` transitions in lock-step with **every**
edge of `7M` (both the `clk7_en` and `clk7n_en` markers), not just its
claimed one, so its own period equals `7M`'s, not double it; `c3` is `7M`
delayed by exactly one `clk_28` edge (90°) and behaves the same way. The
signal in this design that genuinely has `/C1`/`/C3`'s claimed 3.5x MHz-
class period is `cck` (`amiga_clk.v:11`, "colour clock output (3.54 MHz)"
in the port comment) — not exposed under either name on this glossary page,
and not itself asserted by any of this package's 31 claims, so it is
measured and noted for context, never graded. Symmetrically, the signal
that genuinely has CDAC's claimed 90°-trailing 7.16/7.09 MHz-class
behaviour is `c3` — which is also `/C3`'s own name, so `K-0407`/`K-0408`/
`K-0418` (Zorro III, CDAC) PASS on the exact same signal that
`K-0126`/`K-0405` (`/C3`'s own frequency) go RED on. This is not a
harness inconsistency — it is the measured fact that `c3` matches CDAC's
printed definition and not `/C3`'s.

## Control / liveness

One apparatus check, carrying no claim uid: `clk7_en` pulses periodically
after reset release, proving the fixture is not stuck before any of the 31
claims are graded. Labelled `[not graded -- apparatus check, no claim uid]`.

Every `E`-clock decline is preceded by a direct measurement — the one-hot
invariant checked on every sample of the trace, and `eclk`'s own rotation
period — so the decline is backed by what the module does show, not only by
what it cannot.

## MUTATION discipline

Every ASSERT uid gets a `// MUTATION <uid> ...` line in `test_amiga_clk.cpp`.
This module's only inputs are `clk_28` (the reference the harness measures
*in*, not a stimulus to perturb) and `reset_n`. Two families:

- **Baseline-PASS assertions** (`K-0122`, `K-0125`, the four `7M`-frequency
  uids, `K-0407`, `K-0408`, `K-0418`): `stimulus red` — hold `reset_n`
  asserted. `c1`/`c3`/`cck`/`shifter` freeze at reset-forced constants
  (`amiga_clk.v:24-29`; `c1`/`c3`'s own always blocks carry no reset, but
  `clk_7` — their sole input — is held constant by `clk7_cnt`'s reset, so
  they freeze too, one and two edges after release respectively) and
  `clk7_en_reg`/`clk7n_en_reg` are forced to `1` (never toggling), so the
  period/pulse measurements this package depends on find no transitions.
- **Baseline-RED assertions** (both frequency uids, both sync uids, the
  three combined opus uids, `K-0404`, per `/C1`/`/C3`): `oracle green
  substitute`. The frequency comparator is shown live (not vacuously false)
  by applying it to `cck`, whose measured period **is** 8 `clk_28` edges —
  the 3.5x MHz-class ratio the claim asks of `c1`/`c3`. The sync-exclusivity
  comparator is shown live by applying it to `cck` against `clk7_en`/
  `clk7n_en`: `cck` follows `clk7_en` on every pulse and `clk7n_en` on none —
  a real exclusively-one-marker relationship in this design, just not on
  `c1` or `c3`. `K-0404`'s ratio half reuses the same `cck` control (`cck`
  really is 2:1 against `c1`).

Declined uids carry no MUTATION line — nothing is graded for them to
falsify.

## HALT check

No claim in this package needs `amiga_clk.v` changed, a port added, or a
requirement invented. Every ASSERT uid is measurable at the module's
existing five outputs, driven only by its existing two inputs. No HALT.

---
Written after `amiga_clk.v` was read in full (67 lines, no submodule
instantiations) to confirm the port list, the reset polarity, and the
absence of a video-mode input, per "erst prüfen, dann behaupten." The
per-claim measurement plan above was fixed before `test_amiga_clk.cpp` was
written; the exact edge counts asserted are step-3 material confirmed by
running the fixture, not by this document's own arithmetic.
