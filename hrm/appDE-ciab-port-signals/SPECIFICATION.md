# Specification — `appDE-ciab-port-signals`

Package: `hrm/appDE-ciab-port-signals`
Fixture: `ciab` (compiled alone, with its CIA submodules `cia_int.v`,
`cia_timera.v`, `cia_timerb.v`, `cia_timerd.v`), the same boundary
`hrm/appF-ciab-port-assignments` and `hrm/ch8-ciabprb-disk-control-bits` use.
Claims file: `hrm-testpipeline/work/2026-09-17_appDE_packages.jsonl`, filtered
`classify.package == "appDE-ciab-port-signals"` (16 claims).
Subject: printed page 326 (SERIAL INTERFACE CONNECTOR PIN ASSIGNMENT J6, the
DIR column) and printed page 327 ("Modem control signals ... are completely
under software control"), plus printed page 337's CIA-B footer (FLAG =
INDEX*), insofar as claims from those pages were classified into this package.

## The boundary this package is built around

`ciab` compiled alone exposes a 2-bit port-A output (`porta_out[7:6]`,
`ciab.v:20`) and a 6-bit port-A input (`porta_in[5:0]`, `ciab.v:19`). It has
**no signal named** DTR, RTS, CTS, DSR, CD or RI — those name-to-bit bindings
are `minimig.v:658-659`'s job, one level above this fixture's own boundary.
This package's 12 RS-232 claims (6 named signals, 2 readers each) restate only
p.326's DIR column ("<signal> is an output" / "<signal> is an input") — never a
bit position — so what `ciab` alone can show is exactly that: the
direction/position half, not the name identity.

## The stale-decline check (per the build task)

`hrm/appF-ciab-port-assignments` declines the *same five bindings*, read off
appendix F's p.351 printing of this table, with the sentence "no accepted
package currently grades this binding at all (verified: classify.note's own
'Not graded anywhere in hrm/' is correct)" (its own
`test_appF-ciab-port-assignments.cpp`, group-2 record() block,
`sol:F-signals.c36`..`c40` / `opus:F-portsig.c29`..`c33`).

**Checked against the assertions, not the decline text**, per the build
brief's own instruction: `hrm/appF-cia-pin-bindings/test_minimig.cpp` (read in
full) now grades all five as `DTR-IS-PRA-BIT7`, `RTS-IS-PRA-BIT6`,
`CD-IS-PRA-BIT5`, `CTS-IS-PRA-BIT4`, `DSR-IS-PRA-BIT3` (lines 367-401), at the
`minimig` boundary, by driving minimig's own top-level `dtr`/`rts`/`cd`/`cts`/
`dsr` pins and reading PRA through a real CPU bus cycle. **The appF-ciab-
port-assignments sentence is stale for those five bindings.**

That staleness belongs to a *different* package's decline of *different*
claims (appendix F's p.351 PA-name claims), not to this package's own
appendix E p.326 direction-only claims — this package never repeats or leans
on that sentence, and nothing here needed correcting because of it. It is
recorded here because the build task named it as a check to make explicit.

**The CONDITION this package's own claims turn on:** appF-cia-pin-bindings
grades the bindings at `minimig`, driving minimig's real pins — a stronger,
name-plus-position claim, one level above this fixture. This package's own
claims (p.326's DIR column, no bit number stated) are narrower and on a
different axis: the plain hardware-wiring fact that `porta_out` is a real,
always-driven 2-bit *output* port and `porta_in` a real 6-bit *input* port at
`ciab`'s own boundary, independent of which named signal sits where. Per the
build brief ("grade it when this page states something narrower, or states it
on a different axis, and say in the label what the difference is"): **graded
here**, not declined, with the name-to-bit citation to appF-cia-pin-bindings
carried in each assertion's own label rather than re-derived.

## Claim inventory (all 16)

Legend: **G** = graded (`expect()`), **R** = declined (`record()`).

| uid | page | topic | disposition |
|---|---|---|---|
| `sol:E-rs232.c157` | 326 | RTS is an output | **G — PROP-PA-OUTPUT-BITS** |
| `sol:E-rs232.c165` | 326 | DTR is an output | **G — PROP-PA-OUTPUT-BITS** |
| `opus:E-serial-iface.c10` | 326 | RTS is an output (twin) | **G — PROP-PA-OUTPUT-BITS** |
| `opus:E-serial-iface.c18` | 326 | DTR is an output (twin) | **G — PROP-PA-OUTPUT-BITS** |
| `sol:E-rs232.c158` | 326 | CTS is an input | **G — PROP-PA-INPUT-BITS** |
| `sol:E-rs232.c159` | 326 | DSR is an input | **G — PROP-PA-INPUT-BITS** |
| `sol:E-rs232.c161` | 326 | CD is an input | **G — PROP-PA-INPUT-BITS** |
| `sol:E-rs232.c166` | 326 | RI is an input | **G — PROP-PA-INPUT-BITS** |
| `opus:E-serial-iface.c11` | 326 | CTS is an input (twin) | **G — PROP-PA-INPUT-BITS** |
| `opus:E-serial-iface.c12` | 326 | DSR is an input (twin) | **G — PROP-PA-INPUT-BITS** |
| `opus:E-serial-iface.c14` | 326 | CD is an input (twin) | **G — PROP-PA-INPUT-BITS** |
| `opus:E-serial-iface.c19` | 326 | RI is an input (twin) | **G — PROP-PA-INPUT-BITS** |
| `sol:E-rs232.c170` | 327 | modem control signals under software control | **G — PROP-MODEM-SOFTWARE-CONTROL** |
| `opus:E-serial-timing.c5` | 327 | twin | **G — PROP-MODEM-SOFTWARE-CONTROL** |
| `sol:E-portsig.c42` | 337 | CIA-B FLAG carries INDEX*, the disk index pulse | R — cited: `ch8-disk-index-interrupt-path` PROP-FLAG-PIN |
| `opus:E-portsig.c44` | 337 | twin | R — cited: `ch8-disk-index-interrupt-path` PROP-FLAG-PIN |

**Totals: 14 uids graded across 3 propositions; 2 uids declined via a single
`record()` line (both citing the same accepted assertion).**

## PROP-PA-OUTPUT-BITS / PROP-PA-INPUT-BITS

See the doc comments directly above `prop_pa_output_bits()` and
`prop_pa_input_bits()` in `test_appDE-ciab-port-signals.cpp` for the full
argument. In short: `porta_out[7:6]` (PA7=DTR's bit, PA6=RTS's bit, per
appF-cia-pin-bindings, cited not re-derived) is graded as a driven output —
independently writable and readable on both bits; `porta_in[5:0]`'s bits 5..2
(PA5=CD, PA4=CTS, PA3=DSR, all three per appF-cia-pin-bindings; PA2=RI, fresh,
no accepted-package citation) are graded as plain read-only pass-through
inputs. Both measured PASS at baseline; both carry a stimulus mutation
(documented and actually run — see the `// MUTATION` comments and the RUN
lines immediately after each, filled in from what `make test MUT=<NAME>`
actually printed, not predicted in advance).

## PROP-MODEM-SOFTWARE-CONTROL

p.327: "Modem control signals (CTS, RTS, DTR, DSR, CD) are completely under
software control." Decidable at `ciab`'s own boundary for the two output bits
(DTR/RTS): `regporta`/`ddrporta` — the only state feeding `porta_out[7:6]` —
are assigned *only* at `ciab.v:97-103` (`wr && pra`) and `ciab.v:106-112`
(`wr && ddra`); a full-file grep of `ciab.v` found no other assignment to
either register anywhere. Graded as a negative control (a hands-off window
containing real Timer A and ICR/FLAG activity, with no PRA/DDRA access, must
leave `porta_out[7:6]` unchanged) paired with a positive control on the same
Harness (a direct PRA write immediately afterward does move it) — the pairing
rules out a dead read channel explaining the negative result. Measured PASS at
baseline; carries a stimulus mutation (a spurious PRA write injected mid-window
goes RED), actually run.

## Declined claim: CIA-B FLAG = INDEX* (p.337)

`sol:E-portsig.c42` / `opus:E-portsig.c44`. Read directly from
`figures/p337.png` per the build task (p.337's text layer displaces this
gloss onto `PC...not used` — confirmed against the render, not taken from
`corpus-defects.md`'s note alone): the CIA-B footer prints `F....INDEX*` with
the gloss "disk index pulse*".

`hrm/ch8-disk-index-interrupt-path`'s PROP-FLAG-PIN (`opus:8-cia.c56`,
`opus:8-dskint.c9`, `sol:8-int.c9`) already grades that a **pulse** on
`paula_floppy`'s real `index` output, carried across `minimig.v:536+:657` onto
`ciab`'s `flag` input, sets ICR bit 4 (FLG) and raises `irq`. p.337's own
wording states only that the FLAG input carries a *pulse* — it makes no
edge-polarity claim of the kind printed p.341 does ("Any negative transition
on FLAG will set the FLAG interrupt bit," the NM-HRM-063 trap
`hrm/appF-ciab-port-assignments`'s own `PROP-FLAG-EDGE-POLARITY` exists to
grade separately, because there a pulse stimulus is consistent with *either*
an edge or a level reading). Here, unlike appendix F's p.341 case, the
accepted assertion's pulse stimulus **is** the page's own proposition, so
this is a genuine same-proposition decline per the build brief's own
instruction ("decline when a prior assertion grades the SAME proposition") —
not a re-grade avoided by a stale citation of the appendix-F kind.

## Corpus defects checked directly against the render

Both `appD/corpus-defects.md` and `appE/corpus-defects.md` were read in full
before this file was drafted. Two points specific to this package's own pages
were independently verified against the renders, not taken on the notes'
word:

- **p.326** carries the NOTES-column displacement `corpus-defects.md`
  describes (the trailing description sentence sits one row off from its
  NAME/DIR row — e.g. "Signal ground" printed on RTS's row). Verified against
  `appE/p326.txt` directly: the **NAME/DIR pairing itself is not displaced**
  (TXD O, RXD I, RTS O, CTS I, DSR I, CD I, DTR O, RI I read in that order,
  correctly paired) — only the trailing NOTES sentence is. This package's
  claims use the DIR column only, which is unaffected.
- **p.337** was read from `figures/p337.png` directly, per the build task's
  explicit instruction, confirming all three of `corpus-defects.md`'s
  described shuffle blocks, including the third (CIA-B footer: "disk index
  pulse*" belongs to `F....INDEX*`, not `PC...not used`).

## On a control case and mutation

- **Fixture liveness (porta).** A basic liveness proof (`fixture_live_porta`)
  precedes any claim-grading assertion — reused from
  `hrm/appF-ciab-port-assignments`'s own Harness/liveness pattern, same
  boundary.
- **All three graded propositions PASS at baseline** and each carries its own
  actually-run stimulus mutation (see `// MUTATION` comments and RUN lines in
  the test source) — no proposition in this package is expected RED, unlike
  several in the sibling `appF-ciab-port-assignments` package (whose RED
  propositions concern CRA's PBON/OUTMODE bits and FLAG's edge polarity, none
  of which this package's own 16 claims touch).
- **`check_uid_placement.py work/appDE-runs`**: `packages asserting AND
  declining one : 0`.
- **`check_mutation_labels.py appDE-ciab-port-signals`**: `PAIRED
  appDE-ciab-port-signals: 3 assertion(s), 3 record(s)` — every graded label's
  leading token is named by exactly one `// MUTATION` comment.

## Disclosure

RTL read in full before claim-grading assertions were written: `ciab.v` (251
lines), `cia_int.v` (70 lines). `cia_timera.v`/`cia_timerb.v`/`cia_timerd.v`
are present only so Verilator can elaborate `ciab`; no claim in this package
concerns CRA/CRB/timer/TOD behaviour. Also read in full:
`hrm/appF-ciab-port-assignments/test_appF-ciab-port-assignments.cpp` and its
`SPECIFICATION.md` (Harness plumbing, the stale-decline text checked above),
`hrm/appF-cia-pin-bindings/test_minimig.cpp` (the name-to-bit citations this
package's labels lean on), and
`hrm/ch8-disk-index-interrupt-path/test_ch8-disk-index-interrupt-path.cpp`
(PROP-FLAG-PIN, cited on the one decline). `minimig.v:658-660` was read for
citation only (lines already disclosed in the claims' own
`classify.evidence`), not as this package's own fixture boundary.
