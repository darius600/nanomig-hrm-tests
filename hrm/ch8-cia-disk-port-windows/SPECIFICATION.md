# Specification — `ch8-cia-disk-port-windows`

Written from the seven claims in
`hrm-testpipeline/work/ch8-s2-build/ch8-cia-disk-port-windows.jsonl`, the
build task (`work/2026-09-15_ch8_s2_build_brief.md`) and the finding
candidates file (`work/2026-09-15_ch8_s2_finding_candidates.md`, candidate
C12), alone. **No RTL file — `gary.v`, `ciaa.v`, `ciab.v`, `cia_int.v`,
`cia_timera.v`, `cia_timerb.v`, `cia_timerd.v` or `minimig.v` — has been
opened for the purpose of drafting this document.** Every RTL line cited
below is a *disclosed* citation: it was handed to this package already, by
the claims file's own `classify.evidence` fields or by the build task's own
quoted text, and is used here on that basis, not on independent verification.
That verification is exactly what step 2 (opening the RTL, writing the rig
and the test) is for, and is disclosed there.

Package: `hrm/ch8-cia-disk-port-windows`
Fixture: a **rig over three pinned modules** — `gary` + `ciaa` + `ciab` —
with the CIA submodules `cia_int.v`, `cia_timera.v`, `cia_timerb.v`,
`cia_timerd.v` compiled alongside `ciaa`/`ciab`, the way
`hrm/ch8-ciaa-port-direction/Makefile` lists them for `ciaa` alone (read for
the *build pattern only*, per the build task's explicit instruction — its
Makefile is a mechanical list of source files, not a manual-page oracle, and
citing it for that shape is not citing it as an oracle for any assertion
below).
Claims file: `ch8-cia-disk-port-windows.jsonl` (7 claims).

## Why this fixture must be three modules, not one or two

`ciab` alone (the boundary `hrm/ch8-ciabprb-disk-control-bits` already uses)
exposes `portb_out` but has no CPU address bus — only `aen` and `rs[3:0]`
(disclosed by `opus:8-cia.c5`'s own evidence: "ciab has no address bus, only
`aen` and `rs[3:0]` (ciab.v:8, 12)"). Every claim in this package is an
**address** claim — $BFD100, $BFDD00 — not a register-index claim, so a
fixture that cannot itself be asked "does this CPU address select you" would
make every assertion vacuous with respect to what is actually being claimed,
exactly as `ch8-ciaapra-address-window`'s own specification already argued
for the $BFE001 case. That decode is `gary`'s: the claims' own evidence cites
`gary.v:51` (`cpu_address_in`) and `gary.v:190-192` (`sel_cia` /
`sel_cia_a` / `sel_cia_b`), and the build task quotes the two decode lines
directly:

    sel_cia_a = sel_cia & ~cpu_address_in[12]
    sel_cia_b = sel_cia & ~cpu_address_in[13]

`ciaa` is required as the third module because one of this package's two
propositions (the "two ports" claim, `opus:8-dsk.c18` / `sol:8-reglist.c1`)
is specifically about there being a port on **each** chip — one on `ciaa`,
one on `ciab` — and grading "two ports" from a fixture that only contains one
of them would make the count unfalsifiable by construction. The build task
assigns this fixture explicitly ("a RIG over THREE modules — gary + ciaa +
ciab"); this section records the reasoning, not a decision made here.

## The fact this whole package rests on — disclosed by the build task, not derived here

The build task states directly, and the finding-candidates file's candidate
C12 states the same thing independently: `gary.v:191-192`'s two decode lines
each test **one address bit**, not a full compare. `sel_cia_a` depends only
on `cpu_address_in[12]`; `sel_cia_b` depends only on `cpu_address_in[13]`.
Bits 15 and 14 (within the `$BFxxxx` byte-select window that `sel_cia`
itself fully compares at `[23:16]`) do not appear in either line at all, and
`cpu_address_in[12]` does not appear in `sel_cia_b`'s expression, nor does
`[13]` appear in `sel_cia_a`'s. Two consequences follow, both **stated by
the task, not measured by me**, and both re-verified against the actual RTL
in step 2 before any assertion is built on them:

1. **$BFD100 and $BFDD00 are each one member of a larger alias class.** Any
   address sharing `[23:16]=$BF`, the relevant one of `[13]`/`[12]`, and the
   same `rs=cpu_address_out[11:8]` field, but differing in bits 15/14 (and,
   below `rs`, in bits 7:1), answers identically. A claim that "$BFD100 is
   CIABPRB" is therefore a claim about the *window*, and grading only the
   single literal value would not distinguish "the manual's address claim
   is true of this decoder" from "I happened to pick a value inside a much
   larger true set without saying so."
2. **The two selects are not mutually exclusive.** An address with both
   `[13]=0` and `[12]=0` asserts `sel_cia_a` and `sel_cia_b` simultaneously
   — CIA-A and CIA-B chip-enabled at once, for one CPU cycle. This is the
   fact that makes "the WINDOW, not the single address" the correct unit of
   measurement for every address claim on these pages, and it is this
   package's job to make it checkable rather than asserted.

## The corpus conflict already resolved — not reopened here

p.244 prints `CIAAPRB ($BFD100)`; p.241 gives the same address as `CIABPRB`.
This was resolved during classification and is recorded as settled in the
finding-candidates file, on three independent grounds, one of which is the
very decode this package measures: `$BFD100` has `[12]=1` (so `sel_cia_a`,
which needs `~[12]`, is low) and `[13]=0` (so `sel_cia_b`, which needs
`~[13]`, is high) — CIA-B, not CIA-A. `opus:8-cia.c5` and `sol:8-cia.c4`
carry the `corpus_damage` flag for exactly this; their restatements already
read `CIABPRB`. This package does not re-argue the chip letter; it measures
the same decode fact that settled it, from the address-window angle rather
than the register-list angle.

## Claim inventory (all 7) and the propositions they resolve to

| # | uid | page | quote (short) | proposition |
|---|---|---|---|---|
| 1 | `opus:8-dsk.c18` | 241 | "the disk subsystem uses two ports on the ... 8520 CIA chips" | **PROP-TWO-PORTS** |
| 2 | `sol:8-reglist.c1` | 241 | same quote, independent reader (twin of #1) | **PROP-TWO-PORTS** |
| 3 | `opus:8-dsk.c23` | 241 | "CIABPRB ($BFD100) eight output bits for disk selection, control and stepping" | **PROP-CIABPRB-WINDOW** |
| 4 | `opus:8-cia.c5` | 244 | "PB are output bits located in CIAAPRB ($BFD100)" [corpus defect; register is CIABPRB] | **PROP-CIABPRB-WINDOW** |
| 5 | `sol:8-cia.c4` | 244 | twin of #4, restatement already corrected to CIABPRB | **PROP-CIABPRB-WINDOW** |
| 6 | `opus:8-cia.c57` | 245 | "FLAG DSKINDEX* Disk index pulse ($BFDD00, bit 4)" | **PROP-DSKINDEX-WINDOW** |
| 7 | `sol:8-cia.c51` | 245 | twin of #6 | **PROP-DSKINDEX-WINDOW** |

## PROP-TWO-PORTS — the disk subsystem's signals ride on two distinct, independently-addressed CIA ports, one per chip

**Testable at this rig, restricted to what a three-module boundary can
show:** the claims' own evidence (`minimig.v:622`: `porta_in` of `ciaa` ←
`{_fire1,_fire0,_ready,_track0,_wprot,_change}`; `minimig.v:649`:
`portb_out` of `ciab` → `{_motor,_sel3,_sel2,_sel1,_sel0,side,direc,_step}`)
names one input-direction port on `ciaa` and one output-direction port on
`ciab` as the two ports the disk subsystem uses. What this rig can show
positively: a word driven into `ciaa`'s `porta_in` arrives, distinctly and
correctly, when read through this rig's own address-decode path via
`sel_cia_a`; a byte written through this rig's address-decode path via
`sel_cia_b` arrives, distinctly and correctly, on `ciab`'s `portb_out`; and
these two paths are reached through **mutually exclusive** chip-selects for
the addresses used (one asserts only `sel_cia_a`, the other only
`sel_cia_b`) — i.e., two genuinely separate, independently-reachable ports,
not one port answering under two different addresses.

**What this rig cannot show, and does not grade:** that these are the
*only* two CIA ports the disk subsystem uses anywhere in the design — that
would require the full wiring enumeration one level up
(`minimig.v`, in full) and at `paula_floppy` (the claims' own evidence notes
the twelve signals are also ports there, "which is where the count is
checkable without the CIAs" — i.e. the classifier's own view is that the
*exhaustive* count is more directly checkable at a boundary this package
does not include). Grading exclusivity from a boundary that cannot see
`ciaa`'s Port B or `ciab`'s Port A used for anything *else* would be the same
overreach the build task calls out for the DSKDATR "only" claims: the
register/port boundary carries no completeness guarantee by itself. This
package grades the existence-and-distinctness half; the exclusivity half is
left unasserted, named here rather than silently folded into a pass.

Assertions: **TWOPORTS-CIAA-PORTA-OPUS-DSK-C18** (`opus:8-dsk.c18`, p.241),
**TWOPORTS-CIAB-PORTB-SOL-REGLIST-C1** (`sol:8-reglist.c1`, p.241, same
measurement, independent citation).

## PROP-CIABPRB-WINDOW — CIABPRB answers throughout an address window, not only at $BFD100

The window, disclosed by the build task and re-verified in step 2: every
address with `[23:16]=$BF`, `[13]=0` and `rs=cpu_address_out[11:8]=4'h1`
answers as CIABPRB, regardless of bits 15, 14 and (subject to `[12]`, see
below) regardless of the low bits below `rs`. `$BFD100` (`[15:8]=0xD1`,
i.e. `[15]=1,[14]=1,[13]=0,[12]=1`) is one member of that class.

**Testable proposition, three parts, one per citing claim:**

- **CIABPRB-LITERAL-OPUS-DSK-C23** (`opus:8-dsk.c23`, p.241): the literal
  printed address $BFD100, driven through this rig, selects `ciab`
  (`sel_cia_b` asserted, `sel_cia_a` not) and reads back a byte just written
  to `portb_out` through that same address — the eight bits the page
  claims are there, are there.
- **CIABPRB-ALIAS-OPUS-CIA-C5** (`opus:8-cia.c5`, p.244, corpus-damage
  claim, corrected reading): a *different* address — same `[23:16]`,
  `[13]=0`, `rs=4'h1`, but differing from $BFD100 in bits 15/14 — answers
  **identically** to $BFD100. This is the window claim itself: the register
  this page's corrected reading names is not pinned to one literal address.
- **CIABPRB-CHIPLETTER-SOL-CIA-C4** (`sol:8-cia.c4`, p.244, twin of the
  above, restatement already corrected to CIABPRB): at $BFD100 specifically,
  `sel_cia_b=1` **and** `sel_cia_a=0` — the decode-level fact that is one of
  the three independent grounds the corpus conflict was already resolved on,
  measured directly rather than cited secondhand.

## PROP-DSKINDEX-WINDOW — DSKINDEX* (bit 4, read through the ICR) answers throughout an address window, not only at $BFDD00

Same shape, one bit higher up the register map: every address with
`[23:16]=$BF`, `[13]=0` and `rs=4'hD` answers with the interrupt-control
register whose bit 4 is `flag`'s latch (disclosed evidence:
`cia_int.v:49`, `icr[4] <= flag`; `cia_int.v:22`,
`data_out = {irq,2'b00,icr[4:0]}`). $BFDD00 (`[15:8]=0xDD`, i.e.
`[15]=1,[14]=1,[13]=0,[12]=1`) is one member of that class.

- **DSKINDEX-LITERAL-OPUS-CIA-C57** (`opus:8-cia.c57`, p.245): a pulse on
  `flag`, read back through the literal address $BFDD00 driven through this
  rig, sets bit 4 (and bit 7, IR) of the byte returned — the disk index
  pulse the page names, observable at the address the page names.
- **DSKINDEX-ALIAS-SOL-CIA-C51** (`sol:8-cia.c51`, p.245, twin): a different
  address — same `[23:16]`, `[13]=0`, `rs=4'hD`, differing from $BFDD00 in
  bits 15/14 — answers identically, the window claim for this register.

## Mechanism assertions — no manual citation, needed to support the propositions above

Per precedent (`ch8-ciaapra-address-window`'s A0/PRE/PRE-NEG/CTRL, none of
which carry a page citation), several facts underlie every cited assertion
above without themselves being a page claim. Each still gets its own
`expect()` and its own mutation record — the build task is explicit that
"per assertion" includes the fixture-liveness proof and the control case.

- **LIVE-CIAA-PORTA-DISTINCT** — two distinct, known words driven onto
  `ciaa`'s `porta_in` both arrive, distinctly, when read through this rig's
  address decode (at an alias inside `ciaa`'s PRA window, **not** the
  literal $BFE001 — that address is `ch8-ciaapra-address-window`'s own
  subject, and is not re-cited or re-graded here; a different address
  reaching the same chip register is used instead, so this package's
  liveness proof is independent of that package's assertions rather than a
  restatement of them).
- **LIVE-CIAB-PORTB-DISTINCT** — two distinct, known bytes written to
  CIABPRB through this rig both arrive, distinctly, on `portb_out`.
- **LIVE-CIAB-ICR-FLAG-DISTINCT** — a pulsed `flag` and an unpulsed `flag`,
  both read back through the ICR at `rs=4'hD`, give distinct bit-4 values —
  the mechanism PROP-DSKINDEX-WINDOW depends on is not stuck at one value.
- **PRE-SELCIAB-ASSERTED** — precondition every CIABPRB/DSKINDEX assertion
  above depends on: presenting $BFD100 to `gary` actually asserts
  `sel_cia_b`. Asserted, not assumed.
- **PRE-NEG-NONCIA-DESELECTED** — isolation control: an address outside
  `$BFxxxx` entirely leaves both `sel_cia_a` and `sel_cia_b` deasserted,
  proving the decode discriminates rather than floating high.
- **OVERLAP-BOTH-SELECTS-ACTIVE** — the central mechanism fact this whole
  package exists to make checkable: an address exists (inside `$BFxxxx`,
  with both `[13]=0` and `[12]=0`) for which `sel_cia_a` and `sel_cia_b` are
  **both** asserted at once. This is not a page claim — no printed page
  asserts or denies it — and it is what makes every address claim on these
  pages a claim about a window rather than a point. Expected **green**: the
  build task states this as a fact about `gary.v`'s decode, and this
  assertion exists to make that fact checkable rather than merely cited.
- **CTRL-DISTINCT-REGISTER-SAME-CHIP** — control case, same rig and
  measurement basis as the assertions above: at the *same* chip-select
  family used for CIABPRB (`[23:16]=$BF`, `[13]=0`), a *different* `rs`
  value (`ciab`'s PRA, `rs=4'h0`, instead of PRB's `4'h1`) reads back a
  distinct, independently-written byte — proof the address-to-register
  routing discriminates registers within a chip, not just chips, and is not
  stuck answering one register for every `rs`.

14 assertions total: 7 citing a claim (one per uid), 7 supporting the
measurement without a page citation.

## Control case (build-brief sense: at least one PASS on the same basis)

**CTRL-DISTINCT-REGISTER-SAME-CHIP**, above, is that control: same rig, same
measurement basis (an address driven through `gary`'s decode, a byte written
and read back through the selected chip's data path), expected to PASS
independent of how any cited assertion turns out.

## Mutation plan

Every one of the 14 assertions gets its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what
changed>` line in the C++ source, `<label>` being the exact leading
hyphenated identifier of the string passed to `expect()` — not a `-D` define,
not an index, not a runtime-built string. All 14 are expected **green** at
baseline (nothing in this package's evidence suggests any of the seven
claims, or the two mechanism facts OVERLAP/CTRL, disagrees with the RTL at
the literal address — the open question these claims raise is *scope*, not
*truth*, which is exactly why the window is being measured rather than
assumed). Each mutation is therefore the ordinary stimulus form: change what
is driven into the rig (the address, for the window/two-ports/overlap
assertions; the driven word, for the liveness/control assertions), leave the
expected value alone, confirm the targeted assertion alone goes red, revert.
Concrete stimulus values (exact hex addresses, exact bytes) are step-2
material, fixed once the RTL is open, per the build task's order of work —
the shape of each mutation is already fixed by the proposition it tests:

- TWOPORTS-\*: redirect the read address away from the chip/port being
  proven, onto an address for the *other* chip, while still expecting the
  first chip's word — must redden.
- CIABPRB-LITERAL / DSKINDEX-LITERAL: redirect the literal address to a
  non-CIA address (or the wrong `rs`) while still expecting the register's
  value — must redden.
- CIABPRB-ALIAS / DSKINDEX-ALIAS: redirect the alias address back to a value
  outside the window (e.g. `[13]=1` or the wrong `rs`) while still expecting
  the window's value — must redden.
- CIABPRB-CHIPLETTER: redirect to an address with `[12]=0` while still
  expecting `sel_cia_a=0` — must redden.
- LIVE-\*: drive the second word/pulse identical to the first, so
  `distinct` is false — must redden.
- PRE-SELCIAB-ASSERTED: redirect to a non-CIA address while still expecting
  `sel_cia_b=1` — must redden.
- PRE-NEG-NONCIA-DESELECTED: present the CIA address itself while still
  expecting both selects deasserted — must redden.
- OVERLAP-BOTH-SELECTS-ACTIVE: revert `[12]` to 1 (no overlap) while still
  expecting both selects asserted — must redden.
- CTRL-DISTINCT-REGISTER-SAME-CHIP: write the same byte to both registers
  (destroy the distinctness) while still expecting them to differ — must
  redden.

If, once built, any assertion is itself red against the RTL, the build
brief's substitute rule applies instead of mutating an already-red check: a
different mechanism, proven live elsewhere in the same run, is put through
the same comparator and shown to go green.

## What happens next

Only after this file is committed: read
`hrm/ch8-ciaa-port-direction/Makefile` for the build pattern (already named
above); read `gary.v`, `ciaa.v`, `ciab.v`, `cia_int.v` in full; read the
`gary`/`ciaa`/`ciab` instantiations in `minimig.v` for the exact join this
package's own rig file reproduces (the `.aen`/`.rs` wiring and the
`porta_in`/`portb_out` signal lists cited above by line number but not yet
independently confirmed); and write the rig Verilog, the Makefile and the
test source. Every port name, exact hex address and concrete stimulus value
first appears in that step, and the rig's own header will disclose exactly
which minimig.v lines its wiring reproduces, per the build task's
instruction.
