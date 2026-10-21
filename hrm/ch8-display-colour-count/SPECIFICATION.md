# Specification — `ch8-display-colour-count`

Package: `hrm/ch8-display-colour-count`
Fixture: `denise_colortable` (not `denise`, whose `red`/`green`/`blue`
ports are 8 bits wide, `denise.v:45-47`).
Claims file: `hrm-testpipeline/work/ch8-s4-build/ch8-display-colour-count.jsonl`
(2 claims)
Subject: printed page 260's "up to 4,096 different colors on-screen
simultaneously" figure, from the chapter 8 "Display Output Connections"
section's RGB-monitor bullet.

RTL read for this package: `denise_colortable.v` and
`denise_colortable_ram_mf.v` in full (the `loct` mechanics below and this
package's own control assertion); `denise.v:45-47` for the port widths.

## The two claims

**`opus:8-display.c13`** (p.260):
> RGB Monitors ("analog RGB"). Provides four outputs; Red (R), Green (G),
> Blue (B), and Sync (S). They can generate up to 4,096 different colors
> on-screen simultaneously using the circuitry presently available on the
> Amiga.

Restatement: "Up to 4,096 different colors can be generated on-screen
simultaneously through the analog RGB outputs."

**`sol:8-display.c12`** (p.260, same sentence, independently read):
> They can generate up to 4,096 different colors on-screen simultaneously
> using the circuitry presently available on the Amiga.

Restatement: "The analog-RGB outputs can display up to 4,096 different
colors simultaneously with the then-current Amiga circuitry."

Both claims restate the identical numeric proposition from the identical
sentence. `sol:8-display.c12`'s own note records that the page's hedge
clause ("using the circuitry presently available on the Amiga") was split
off by the other reader as a separate claim, `opus:8-display.c14`, which is
not in this package (it is classified `not-asserted`/`source-silent`
elsewhere, per the slice's finding-candidates note on p.260's "four
outputs... five names" printing defect). This package's 2 claims carry only
the numeric content: up to 4,096 colors, on screen, simultaneously.

## What is testable at this boundary, and what is not

`denise_colortable` is the module that actually holds the 12-bit-per-write
colour code (`data_in[11:0]`, `denise_colortable.v:12,32`) and packs it into
the `rgb[23:0]` output the rest of the display pipeline consumes
(`denise_colortable.v:19,58`). That resolves, directly, the *number*: a
12-bit code space is 2^12 = 4,096 distinct values, and this is where that
number lives in the design (one level below `denise`, whose own
`red`/`green`/`blue` ports are 8 bits each and would answer "16.7 million"
instead — not what page 260 states).

What this boundary cannot resolve on its own is the word "simultaneously".
Read narrowly, p.260's sentence could mean either (a) the palette has 4,096
selectable codes, full stop, or (b) as many as 4,096 *distinct* colors can
appear in a single displayed frame at once. Sense (b), taken literally with
only 32 (or 64 with EHB) colour registers, needs HAM (Hold-And-Modify) to
reach — `denise_hamgenerator`, instantiated in `denise.v`, outside this
package's fixture and outside `denise_colortable` entirely. A HAM rig is
outside this package's boundary.

This split does not leave the claims untestable, though, for the same
reason it does not trouble `hrm/ch3-colour-register-format`'s own citation
of the identical number on p.46 ("Twelve bits of color selection allow you
to define ... one of 4,096 possible colors"): both pages state the number as
a property of the colour-selection mechanism itself, not as a claim that
requires proving a particular frame actually contains 4,096 simultaneously
distinct pixels. p.260's own sentence structure backs this reading — "They
[the four RGB outputs] can generate up to 4,096 different colors" describes
a capability of the output path, the same capability p.46 describes for the
register. The numeric content of both claims is fully and exactly a fact
about the colour table's resolution: 4,096 = 2^12 reachable, distinguishable
codes. That is what is graded below, and that is the honest ceiling of what
this fixture — or, per the paragraph above, any fixture assembled only from
`denise`/`denise_colortable`-family modules without a HAM rig — can show.

## Existing coverage: `hrm/ch3-colour-register-format`, and why this package still grades its own claims

That package already builds `denise_colortable` (plus
`denise_colortable_ram_mf.v`) and already carries an assertion,
`SWEEP-12BIT-DISTINCT`, that writes 9 distinct 12-bit codes to one colour
register and confirms all 9 read back pairwise-distinct through `rgb` —
citing, verbatim, "Twelve bits of color selection allow you to define ...
one of 4,096 possible colors" (p.46). `ADDR-32-DISTINCT` in the same package
additionally confirms all 32 registers are independently addressable and
pairwise-distinct. `SWEEP-12BIT-DISTINCT` passes.

That is the same mechanism (`denise_colortable`, a 12-bit write, an `rgb`
read) answering the same number (4,096 = 2^12), for the same underlying
hardware fact — but it is graded against a different page. This package's
two claims cite p.260; `SWEEP-12BIT-DISTINCT` cites p.46. The pipeline's
discipline is that a claim is graded against the page it cites: neither of
this package's 2 claims carries a `classify.note` naming an accepted
package and uid (the mechanism the 53 already-graded claims elsewhere in
this slice use), and the package list's own count of "already graded
elsewhere" for this package is 0. Citing `SWEEP-12BIT-DISTINCT` as a
*substitute* oracle for a p.260 claim would quietly move the oracle from
p.260 to p.46; if the two pages ever disagreed, nothing here would show it,
and a citation is in any case not a uid on a graded line — the ledger reads
graded lines, not prose. **Both claims are therefore graded below, against
p.260 directly, by `PALETTE-RESOLVES-4096-CODES` (sweep all 4,096 possible
12-bit codes, count pairwise-distinct `rgb` readings). `ch3`'s result is
carried as corroboration in that assertion's own `note()`, not as its
oracle.**

## What this package additionally measures, and why it is not redundant

`ch3-colour-register-format`'s own fixture is not a free pass for this
citation, though, without one more fact established at this boundary
specifically. The colour store is not simply "12 bits per register": it is
**24 bits, written in two 12-bit halves selected by `loct`**
(`denise_colortable.v:17,33`: `wr_ws = loct ? 2'b01 : 2'b11`;
`denise_colortable_ram_mf.v:16-20`: two separate 12-bit RAMs, `ram0`/`ram1`,
each independently write-enabled by one bit of `wr_ws`). With `loct` held
low, every write enables both RAMs together with the same 12-bit value, so
the two halves are always equal and the 4,096-code space page 260 describes
is exactly what is reachable. With `loct` held high, only `ram0` is written,
so a register's high half (set by an earlier `loct`-low write) and low half
(set by the `loct`-high write) can differ — reaching colours outside that
4,096-code space. **Only the `loct`-low reading is page 260's reading**; the
`loct`-high reading is the AGA extended-precision path, which this
package's two claims do not describe and are not about.

`hrm/ch3-colour-register-format`'s own `Harness` does hold `loct` at 0
throughout (set once in its constructor, never varied) — the correct value
for citing it against a page-260, not page-260-plus-AGA, claim — but that
package had no occasion to say so, since none of its own claims (drawn from
chapter 3's general colour-register description) turn on the distinction.
Citing it here without establishing that distinction at this boundary would
be trusting a coincidence of a default reset value, not a measured fact.

So this package adds exactly the missing piece, as its own control
assertion (no claim uid — it grades nothing in the claims file; it grounds
`PALETTE-RESOLVES-4096-CODES` below): drive the same register through a
`loct`-low write, then a `loct`-high write with a different 12-bit value,
then a `loct`-low write again, and confirm the three readings behave exactly
as `denise_colortable.v`'s two-RAM structure predicts — both halves moving
together under `loct`-low, only the low half moving under `loct`-high, and
the register returning to the paired/doubled state on the next `loct`-low
write. The fixture thus holds `loct` deliberately, as a checked,
mutation-tested assertion rather than an unstated default.

## Assertions in this package

- **`FIXTURE-LIVE`** — two distinct, known 12-bit codes, written with `loct`
  held low (the baseline this whole package cares about), read back
  distinctly, before anything else is trusted. No claim uid; infrastructure.
- **`LOCT-SELECTS-4096-READING`** — the `loct`-low / `loct`-high / `loct`-low
  sequence described above, confirming which reading page 260's 4,096 figure
  actually describes and that `PALETTE-RESOLVES-4096-CODES` is graded on
  that basis deliberately, not by accident. No claim uid; measurement-basis
  control for the assertion below.
- **`opus:8-display.c13 sol:8-display.c12 PALETTE-RESOLVES-4096-CODES`** —
  both claim uids graded on one `expect()`: sweep all 4,096 possible 12-bit
  codes into one register, with `loct` held low, and count the
  pairwise-distinct `rgb` readings — measured by sweeping and counting, not
  asserted from `data_in`'s declared width. `hrm/ch3-colour-register-format`'s
  `SWEEP-12BIT-DISTINCT` result is carried in this assertion's own `note()` as corroboration; it is not
  this assertion's oracle, which is p.260 directly.

Both claims in this package's 2-claim inventory are graded with `expect()`
under their own uids, on the single `PALETTE-RESOLVES-4096-CODES` assertion.
Neither appears on a `record()` line.
