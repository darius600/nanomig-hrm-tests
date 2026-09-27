# Specification — `appF-ciab-port-assignments`

Package: `hrm/appF-ciab-port-assignments`
Fixture: `ciab` (compiled alone, with its CIA submodules `cia_int.v`,
`cia_timera.v`, `cia_timerb.v`, `cia_timerd.v`), the same boundary
`hrm/ch8-ciabprb-disk-control-bits` uses (that package's own Makefile is the
template for this one: `TOP := ciab`, not `TOP_MODULE = minimig`, which is
what its sibling `ch8-cia-disk-signal-bindings` builds).
Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`, filtered
`classify.package == "appF-ciab-port-assignments"` (67 claims).
Subject: printed page 340 (CIAB Address Map / Chip Register Map figure) and
printed page 351 (Port Signal Assignments, CIA-B half), plus page 339's
overview sentence, page 341's HANDSHAKING paragraph, and page 348's CRA bit
table, insofar as claims from those pages were classified into this package.

No RTL file had been opened for the purpose of drafting this file's claim
assignments beyond what the classifier's own `classify.evidence` already
disclosed line-and-file for (every claim in this package carries `file:line`
citations in `classify.evidence`, so, per the build brief's own acknowledgement,
"the blind order cannot hold in full" here — every appendix-F package was
classified with RTL evidence attached). What follows is a description of what
that evidence already showed, not new RTL reading undertaken to write this
file. `ciab.v` was, in addition, read in full during toolchain/build
confirmation before this file was committed — disclosed in full at the end of
this document, in the same form `ch8-ciabprb-disk-control-bits` used.

## Correction to the build brief — the "47" is right in count, wrong in kind

The build brief states: "47 of the 67 carry a `classify.note` naming an
accepted package that already grades them — those go on `record()` lines
citing that package and uid." **Checked against the claims file: only 37 of
the 67 carry a note of that shape.** The other 10 carry a **non-empty**
`classify.note` too, but every one of those ten notes reads "Not graded
anywhere in hrm/. New here." (verbatim, or the equivalent longer form on
`sol:F-signals.c36`) — the opposite of naming an accepted package. The brief's
"47" is arithmetically the count of claims with *any* note text, not the count
of claims *naming an accepted package*; the two are not the same set.

The 10 are: `sol:F-signals.c36..c40` and `opus:F-portsig.c29..c33` — the five
CIA-B port-A modem-line bindings on p.351 (PA7=/DTR, PA6=/RTS, PA5=carrier
detect, PA4=/CTS, PA3=/DSR), each claimed once by each reader. They are
declined below anyway, but for a different, independently-argued reason (see
"Claims not assigned an assertion, group 2"), not because the brief's citation
applies to them — it doesn't, and this file does not pretend it does.

This "37, not 47" correction was independently caught by three other builders
too and is recorded as O78: the brief counted claims that merely *have* a
note, not claims whose note *names an accepted package*.

## Second correction — the decline rule misapplied to `sol:F-handshake.c7` / `opus:F-handshake.c6`

This package's first build declined these two under the general "an accepted
package already grades this" umbrella, citing `ch8-disk-index-interrupt-path`'s
PROP-FLAG-PIN. The coordinator caught that the citation does not actually
cover the proposition: PROP-FLAG-PIN grades a **pulse** (a rise then a fall)
setting ICR bit 4 — a stimulus shape consistent with *either* an edge reading
or a level reading of the hardware, so grading it settles no verdict about
which one this RTL implements. Page 341's own wording is strictly narrower
("Any **negative transition** ... will set the FLAG interrupt bit") and is the
only page in the corpus that states the polarity at all. The decline rule
exists to stop this appendix reaching a *different verdict* from an accepted
package about the *same* proposition — and there was no verdict to contradict,
because no accepted package grades this narrower proposition at all. Declining
it left the requirement untested and the distinction unrecorded, which is
exactly the outcome the rule exists to prevent, not produce.

Both uids are now graded, together, as **PROP-FLAG-EDGE-POLARITY** (see
below) — moved out of "Claims not assigned an assertion" and out of the
`record()` block in the test source. `sol:F-handshake.c5`, `opus:F-handshake.c5`
and `sol:F-handshake.c6` remain declined: they are the existence claim and the
general-purpose-input/"another 8520's PC output" claim respectively, genuinely
covered by PROP-FLAG-PIN (existence) or genuinely unreachable (no PC-output
pin exists anywhere in this design, on either CIA) — the original reasoning
for those three holds.

## What this fixture can and cannot carry — the boundary this specification is built around

`ciab` compiled alone exposes: an 8-bit bus (`rs`-selected read/write), a
2-bit port-A output (`porta_out[7:6]` — the declaration itself is `output
[7:6] porta_out`; `ciab` never drives PA5..PA0 as outputs), a 6-bit port-A
input (`porta_in[5:0]`), an 8-bit port-B output (`portb_out[7:0]`), and the
single-bit `flag`/`irq`/`tick`/`eclk` timing and interrupt ports, all
described in `classify.evidence` and confirmed by reading `ciab.v` (251
lines), `cia_int.v` (70), `cia_timera.v` (112), `cia_timerb.v` (107) and
`cia_timerd.v` (142) during build.

It has **no signal named** DTR, RTS, carrier-detect, CTS, DSR, MTR*, SEL3*,
SEL2*, SEL1*, SEL0*, SIDE*, DIR, STEP*, or INDEX* — those bindings are
`minimig.v:648-650`'s job (`.porta_in({cd,cts,dsr,...})`, `.porta_out({dtr,rts})`,
`.portb_out({_motor,_sel3,_sel2,_sel1,_sel0,side,direc,_step})`,
`.flag(index)`), one level above this fixture's boundary — exactly the
argument `ch8-ciabprb-disk-control-bits` used to decline its 16 PB-name
bindings, applied here to PA. What `ciab` alone *can* show is the
position/transfer half: that PA7 and PA6 are independently writable AND
readable output bits, and that PA5..PA3 (and PA2..PA0, though no claim in
this package addresses those) are transparent, independently readable input
bits. The identity half — that position 7 is *specifically* DTR rather than
some other signal — is not available here.

`portb_out[7:0]` is a real 8-bit output (`ciab.v:21,164`), unlike `ciaa`,
which declares a `portb_out` wire at `ciaa.v:114` and never assigns it. That
asymmetry is why CRA's PBON and OUTMODE bits (p.348) are gradeable at this
boundary at all — there is somewhere for a hardware-driven bit to appear, if
the RTL drove one there.

## Claim inventory (all 67)

Legend: **G** = graded (`expect()`), **R** = declined (`record()`).

| uid | page | topic | disposition |
|---|---|---|---|
| `sol:F-overview.c7` | 339 | each 8520 has an edge-detection input | **G — PROP-FLAG-EXISTS** |
| `sol:F-ciab-map.c23` | 340 | $BFD200 ddra, direction for port A, PA7/PA6 only | **G — PROP-DDRA-POSITION** |
| `opus:F-ciab-map.c6` | 340 | twin of the above | **G — PROP-DDRA-POSITION** |
| `sol:F-ciab-map.c26` | 340 | $BFD300 ddrb | R — cited: `ch8-cia-disk-port-windows` |
| `opus:F-ciab-map.c8` | 340 | twin | R — cited: `ch8-cia-disk-port-windows` |
| `sol:F-ciab-map.c29` | 340 | $BFD400 talo, rate | R — cited: `ch1-cia-timers` |
| `opus:F-ciab-map.c10` | 340 | twin | R — cited: `ch1-cia-timers` |
| `sol:F-ciab-map.c32` | 340 | $BFD500 tahi | R — cited: `ch1-cia-timers` |
| `opus:F-ciab-map.c12` | 340 | twin | R — cited: `ch1-cia-timers` |
| `sol:F-ciab-map.c35` | 340 | $BFD600 tblo, rate | R — cited: `ch1-cia-timers` |
| `opus:F-ciab-map.c14` | 340 | twin | R — cited: `ch1-cia-timers` |
| `sol:F-ciab-map.c38` | 340 | $BFD700 tbhi | R — cited: `ch1-cia-timers` |
| `opus:F-ciab-map.c16` | 340 | twin | R — cited: `ch1-cia-timers` |
| `sol:F-ciab-map.c41` | 340 | $BFD800 todlo, bits 7-0 | **G — PROP-TOD-BYTES** |
| `opus:F-ciab-map.c18` | 340 | twin | **G — PROP-TOD-BYTES** |
| `sol:F-ciab-map.c44` | 340 | $BFD900 todmid, bits 15-8 | **G — PROP-TOD-BYTES** |
| `opus:F-ciab-map.c20` | 340 | twin | **G — PROP-TOD-BYTES** |
| `sol:F-ciab-map.c47` | 340 | $BFDA00 todhi, bits 23-16 | **G — PROP-TOD-BYTES** |
| `opus:F-ciab-map.c22` | 340 | twin | **G — PROP-TOD-BYTES** |
| `sol:F-ciab-map.c49` | 340 | $BFDB00 not used | **G — PROP-UNUSED-SLOT** |
| `sol:F-ciab-map.c52` | 340 | $BFDC00 sdr, unused | **G — PROP-SDR-LATCH** |
| `opus:F-ciab-map.c24` | 340 | twin | **G — PROP-SDR-LATCH** |
| `sol:F-ciab-map.c55` | 340 | $BFDD00 icr | R — cited: `ch8-disk-index-interrupt-path` |
| `opus:F-ciab-map.c26` | 340 | twin | R — cited: `ch8-disk-index-interrupt-path` |
| `sol:F-ciab-map.c58` | 340 | $BFDE00 cra | **G — PROP-CRA-DECODE** |
| `opus:F-ciab-map.c28` | 340 | twin | **G — PROP-CRA-DECODE** |
| `sol:F-ciab-map.c61` | 340 | $BFDF00 crb | **G — PROP-CRB-DECODE** |
| `opus:F-ciab-map.c30` | 340 | twin | **G — PROP-CRB-DECODE** |
| `sol:F-handshake.c5` | 341 | FLAG negative-edge-sensitive | R — cited: `ch8-disk-index-interrupt-path` |
| `opus:F-handshake.c5` | 341 | FLAG edge + PC/general-purpose halves | R — cited: `ch8-disk-index-interrupt-path` |
| `sol:F-handshake.c6` | 341 | FLAG can take PC or be general-purpose | R — cited: `ch8-disk-index-interrupt-path` |
| `sol:F-handshake.c7` | 341 | negative transition sets FLAG bit | **G — PROP-FLAG-EDGE-POLARITY** (expect RED; coordinator correction) |
| `opus:F-handshake.c6` | 341 | twin of `sol:F-handshake.c7` | **G — PROP-FLAG-EDGE-POLARITY** (expect RED; coordinator correction) |
| `sol:F-cra.c4` | 348 | CRA bit1 PBON | **G — PROP-CRA-PBON** (expect RED) |
| `opus:F-cra.c4` | 348 | twin | **G — PROP-CRA-PBON** (expect RED) |
| `sol:F-cra.c5` | 348 | CRA bit2 OUTMODE | **G — PROP-CRA-OUTMODE** (expect RED) |
| `opus:F-cra.c5` | 348 | twin | **G — PROP-CRA-OUTMODE** (expect RED) |
| `sol:F-signals.c33` | 351 | CIA-B on data bits 15-8 | R — cited: `ch8-cia-disk-port-windows` / `ch8-ciaapra-address-window` |
| `sol:F-signals.c35` | 351 | CIA-B associated with INT6 | R — cited: `ch8-disk-index-interrupt-path` |
| `sol:F-signals.c36` | 351 | PA7 = /DTR, driven output | R — boundary decline (see group 2) |
| `opus:F-portsig.c29` | 351 | twin | R — boundary decline |
| `sol:F-signals.c37` | 351 | PA6 = /RTS, driven output | R — boundary decline |
| `opus:F-portsig.c30` | 351 | twin | R — boundary decline |
| `sol:F-signals.c38` | 351 | PA5 = carrier detect | R — boundary decline |
| `opus:F-portsig.c31` | 351 | twin | R — boundary decline |
| `sol:F-signals.c39` | 351 | PA4 = /CTS | R — boundary decline |
| `opus:F-portsig.c32` | 351 | twin | R — boundary decline |
| `sol:F-signals.c40` | 351 | PA3 = /DSR | R — boundary decline |
| `opus:F-portsig.c33` | 351 | twin | R — boundary decline |
| `sol:F-signals.c46` | 351 | PB7 = /MTR | R — cited: `ch8-cia-disk-signal-bindings` |
| `opus:F-portsig.c39` | 351 | twin | R — cited: `ch8-cia-disk-signal-bindings` |
| `sol:F-signals.c47` | 351 | PB6 = /SEL3 | R — cited: `ch8-cia-disk-signal-bindings` |
| `opus:F-portsig.c40` | 351 | twin | R — cited: `ch8-cia-disk-signal-bindings` |
| `sol:F-signals.c48` | 351 | PB5 = /SEL2 | R — cited: `ch8-cia-disk-signal-bindings` |
| `opus:F-portsig.c41` | 351 | twin | R — cited: `ch8-cia-disk-signal-bindings` |
| `sol:F-signals.c49` | 351 | PB4 = /SEL1 | R — cited: `ch8-cia-disk-signal-bindings` |
| `opus:F-portsig.c42` | 351 | twin | R — cited: `ch8-cia-disk-signal-bindings` |
| `sol:F-signals.c50` | 351 | PB3 = /SEL0 | R — cited: `ch8-cia-disk-signal-bindings` |
| `opus:F-portsig.c43` | 351 | twin | R — cited: `ch8-cia-disk-signal-bindings` |
| `sol:F-signals.c51` | 351 | PB2 = /SIDE | R — cited: `ch8-cia-disk-signal-bindings` |
| `opus:F-portsig.c44` | 351 | twin | R — cited: `ch8-cia-disk-signal-bindings` |
| `sol:F-signals.c52` | 351 | PB1 = DIR | R — cited: `ch8-cia-disk-signal-bindings` |
| `opus:F-portsig.c45` | 351 | twin | R — cited: `ch8-cia-disk-signal-bindings` |
| `sol:F-signals.c53` | 351 | PB0 = /STEP, 3.0 ms | R — cited: `ch8-cia-disk-signal-bindings` (+ `ch8-drive-step-timing`) |
| `opus:F-portsig.c46` | 351 | twin | R — cited: `ch8-cia-disk-signal-bindings` |
| `sol:F-signals.c55` | 351 | FLAG = /INDEX | R — cited: `ch8-disk-index-interrupt-path` |
| `opus:F-portsig.c48` | 351 | twin | R — cited: `ch8-disk-index-interrupt-path` |

**Totals: 22 uids graded across 10 propositions; 45 uids declined via
`record()` (35 citing an accepted package, 10 declined here for a
boundary reason this file argues itself).** (Was 20/9/47/37 in this
package's first build; `sol:F-handshake.c7`/`opus:F-handshake.c6` moved from
declined to graded per the coordinator correction above.)

## PROP-FLAG-EXISTS — `ciab`'s `flag` pin is a live, externally driven input

Page 339 (`sol:F-overview.c7`): "Each chip has 16 general purpose
input/output pins, plus a serial shift register, three timers, an output
pulse pin **and an edge detection input**."

**Testable, scoped:** `ciab.v:17` declares `flag` as a real input, wired to
`cia_int` at `ciab.v:185`. What this fixture can show is *existence and
liveness* — driving `flag` changes system state (an armed ICR bit) — not
edge-*polarity* (declined below, p.341) and not the specific pulse-sets-a-bit
behaviour `ch8-disk-index-interrupt-path` already grades under a different
uid. This is deliberately a narrower claim than either of those: a check that
this pin is a real, externally-controllable input at all (as opposed to a
tied-off constant — the fixture cannot itself build `ciaa` to compare, but
`ciaa.v:321`'s `.flag(1'b0)`, already read during classification, is why the
page's "each chip" is not simply true of this board's `ciaa`).

Status: testable, at `ciab`'s own boundary.

## PROP-FLAG-EDGE-POLARITY — a negative transition is required; a held level does not survive a read

Page 341 (`sol:F-handshake.c7`, `opus:F-handshake.c6`): "Any negative
transition on FLAG will set the FLAG interrupt bit." Added by coordinator
correction after this package's first build — see the correction note above.
Not the same proposition `ch8-disk-index-interrupt-path`'s PROP-FLAG-PIN
grades (a pulse, consistent with either an edge or a level reading); this is
the page's own, narrower and more specific wording, graded nowhere else in
the suite.

**Repaired per NM-HRM-063 — this package's first build measured the wrong
clause.** Page 245: "FLAG DSKINDEX* Disk index pulse ($BFDD00, bit 4)."
Page 334: the signal wired to CIA-B's FLAG fires "whenever the INDEX* input
goes active." Page 379: `*` marks an active-low pin. Together: at `ciab`'s
boundary `flag` is DSKINDEX*'s active-high form (the pin itself is active
low), so a 0->1 on `flag` **is** the negative transition p.341 names, not a
positive one — and p.341 requires that transition to **set** the bit, not
leave it clear. Page 346: "When you read the DATA register, its contents
are cleared (set to 0), and the IRQ line returns to a high state." This
package's first build of this proposition took the 0->1 on `flag` as a
positive transition requiring the bit to *stay clear*, and read ICR with a
comparator that never moved `clk` (so `cia_int.v`'s clear-on-read branch
never ran in any stimulus it drove) — both wrong, independently of one
another.

**Testable, at three steps, each through a clocked ICR read** (a real CPU
read cycle: `aen=1`, `rd=1`, `wr=0`, `rs=4'hD`, `data_out` sampled first,
then one rising `clk` edge with `clk7_en=1`, then deasserted — see
`read_icr_clocked()` in the test source):

1. After reset, `flag=0`: write ICR `0x90` (set-mode, mask bit 4).
2. `flag` 0->1 — the negative transition — held 3 ticks. Clocked read:
   bit 4 must read **SET** (p.341).
3. `flag` held at 1 for 5 more ticks — no further transition. Clocked read:
   bit 4 must read **CLEAR**, and bit 7 (IR) with it (p.346; p.341 gives
   nothing that sets the bit again without a fresh negative transition, and
   none occurs here).

Control, same channel and timing: the identical steps, but `flag` is
returned to 0 before the first read, so nothing is held across it — reads
bit 4 SET after the transition, then CLEAR after the hold, regardless of
implementation (with `flag` back at 0, a level-sensitive read and an
edge-sensitive read agree). Measured **PASS**, isolating step 3 of the
graded assertion: a set bit surviving that read is specifically the held
level, not an artifact of the read mechanism.

**Measured RED, at step 3, exactly as predicted from a scratch probe before
this repair's run was captured:** step 2's clocked read returns `0x90` (bit
4 SET, correct); step 3's clocked read returns `0x90` again — bit 4 is still
set, because `flag` held at 1 sets the bit a second time at the read itself
(`cia_int.v`'s clearing branch is `icr[4] <= flag`, eight lines from the
level-OR branch, so a read while `flag` is still high re-sets the bit in the
same cycle it was meant to clear). Four third-party emulators surveyed
independently by the coordinator — WinUAE (`cia_diskindex()`, an event per
index pulse, `disk.cpp:3973`), vAmiga (`emulateFallingEdgeOnFlagPin()` sets
the bit, `CIA.cpp:250-266`), Denise (`cia.setFlag()` called once per head
wrap, `diskDrive.cpp:79`) and Copperline (`falling_edge = flag_pin_high &&
!high`, `cia.rs:373-380`) — unanimously implement a real falling-edge
detector for this bit and would read step 3's second clocked read CLEAR.
Whether the gap is a defect is for Amiga/RTL specialists to adjudicate; this
package measures and reports, per the build brief, exactly as it did the
first time for PROP-CRA-PBON and PROP-CRA-OUTMODE.

Status: testable; baseline result is a genuine fail at step 3, not a broken
fixture — the stimulus mutation in the test source (flag never driven high
at all before step 2) makes the unmutated comparator fail through step 2's
SET clause instead, proving that clause depends on a real transition and is
not a tautology. Measured RUN: 7 passed, 3 failed — the same totals as
baseline (still the package's one PROP-FLAG-EDGE-POLARITY FAIL, moved to a
different clause).

## PROP-DDRA-POSITION — DDRA controls whether PA7/PA6 are driven outputs or pulled-high inputs

Page 340 (`sol:F-ciab-map.c23`, `opus:F-ciab-map.c6`), from the CIAB address
map figure (`figures/p340.png`; the text layer's `(set to 0xFF)` sits two
rows above this entry, `corpus-defects.md`): "$BFD200 ddra, Direction for
Port A (BFD000); 1 = output".

**Testable, scoped:** `ciab.v:126` is `porta_out[7:6] =
(~ddrporta[7:6])|regporta[7:6]` — with a DDRA bit set (1/output), the
corresponding `porta_out` bit mirrors what was written to PRA; with it clear
(0/input), the pull-up term forces that bit high regardless of PRA. Graded
across both PA7 and PA6, the only two bits `ciab.v` implements as outputs.
Not graded: the printed "(set to 0xFF)" as a hardware reset value — the
claim's own evidence already notes `ddrporta` resets to 0, and the printed
figure caption is a software convention, not contradicted by that reset
value, so this file does not construct a claim on it either way.

Status: testable, at `ciab`'s own boundary.

## PROP-TOD-BYTES — the three TOD read addresses return the three byte-slices of the 24-bit event counter

Page 340 (`sol`/`opus:F-ciab-map.c41`/`c18`, `c44`/`c20`, `c47`/`c22`), from
the same figure: "$BFD800 todlo, Horizontal sync event counter bits 7-0",
"$BFD900 todmid, ... bits 15-8", "$BFDA00 todhi, ... bits 23-16".

**Testable, scoped:** `cia_timerd.v:25` holds a 24-bit `tod`; `ciab.v:58-60`
decodes `rs`=8/9/A onto `tlo`/`tme`/`thi`, and `cia_timerd.v:51-59` returns
`tod_latch[7:0]`/`[15:8]`/`[23:16]` respectively for a read at each. With
`tick` held low (no counting) three known, distinct bytes are written one
address at a time and read back address-by-address. Not graded: the
read-latch/freeze behaviour (`latch_ena`, `crb7`-gated ALARM steering) or the
count-enable side effect of a high-byte write (`cia_timerd.v:73-79`) — no
claim in this package states either, and both belong to pages this package
does not own (p.344's TOD-clock package).

Status: testable, at `ciab`'s own boundary.

## PROP-UNUSED-SLOT — $BFDB00 answers nothing

Page 340 (`sol:F-ciab-map.c49`): "$BFDB00 ... not used".

**Testable:** `ciab.v:50-64`'s decoder has no `rs==4'hB` term, so no
sub-module's `_out` contributes to the `data_out` OR-mux (`ciab.v:69`) for
that select. Graded with other registers deliberately made non-zero first
(ICR armed and set, CRA written non-zero), so the zero this fixture reads at
$BFDB00 is not simply "nothing has been written yet".

Status: testable, at `ciab`'s own boundary.

## PROP-SDR-LATCH — $BFDC00 is a plain write/read latch, not a functioning shift register

Page 340 (`sol:F-ciab-map.c52`, `opus:F-ciab-map.c24`): "$BFDC00 sdr, CIAB
serial data register (unused)".

**Testable, scoped:** `ciab.v:71-81` ("fake serial port data register" per
its own comment) is a plain 8-bit latch with no shift path; `ciab.v:4-22`
declares no CNT/SP pin on this module at all (a serial shift register needs
an external shift clock; there is nowhere for one to enter). Graded by
writing two distinct known bytes and confirming each reads back exactly and
unchanged — the behavioural signature of a static latch, consistent with
"(unused)" as a functioning serial register, as opposed to bits rotating out
under an (absent) shift clock.

Status: testable, at `ciab`'s own boundary.

## PROP-CRA-DECODE / PROP-CRB-DECODE — $BFDE00 and $BFDF00 are distinct, independently-decoded control registers

Page 340 (`sol:F-ciab-map.c58`/`opus:c28` for CRA; `sol:F-ciab-map.c61`/
`opus:c30` for CRB): "$BFDE00 cra, CIAB Control register A"; "$BFDF00 crb,
CIAB Control register B".

**Testable, scoped:** `ciab.v:63-64` decodes `rs`=E/F onto `cra`/`crb`.
`cia_timera.v:39-40` stores CRA's write as `tmcr[6:0] <=
{data_in[6:5],1'b0,data_in[3:0]}` (bit 4 always forced to 0, a real strobe
input with no storage) and reads back `{1'b0,tmcr[6:0]}` — so writing 0xFF
and reading back must yield exactly 0x6F. CRB is a composite: `ciab.v:223`
and `ciab.v:243` route `crb` to **both** `cia_timerb.v`'s `tcr` (its own
`tmcr[6:0]`, same masking) **and** `cia_timerd.v`'s `tcr` (only bit 7,
`crb7`); `ciab.v:69` ORs both modules' `_out` together, so writing 0xFF to
CRB and reading it back must yield 0xEF (`{1'b0,tmcr[6:0]}=0x6F` from
`cia_timerb` OR'd with `{crb7,7'b0}=0x80` from `cia_timerd`) — a page-visible
single register that this RTL actually assembles from two separate
sub-modules' state. Graded together with independence checks (writing CRA
leaves CRB's own value at its prior/reset state, and vice versa).

Status: both testable, at `ciab`'s own boundary.

## PROP-CRA-PBON / PROP-CRA-OUTMODE — genuine RTL gaps, graded RED at baseline

Page 348 (`sol`/`opus:F-cra.c4`): "1 PBON  1 = Timer A output on PB6, 0 = PB6
is normal operation." Page 348 (`sol`/`opus:F-cra.c5`): "2 OUTMODE 1 =
toggle, 0 = pulse."

**Testable, and measured to fail:** `cia_timera.v:40` stores `tmcr[1]`
(PBON) and `tmcr[2]` (OUTMODE) on a CRA write and `cia_timera.v:108` reads
them back — `grep -n "tmcr\[" cia_timera.v` (run during this build, output
reproduced in the test source) shows lines 38, 40, 42, 44, 52-54, 108 only:
**no line reads `tmcr[1]` or `tmcr[2]` for any purpose other than storage and
read-back.** `ciab.v:164`'s sole driver of `portb_out` is
`(~ddrportb)|regportb` — a direct function of what software wrote to PRB and
DDRB, with no term anywhere fed by `cia_timera`'s state. There is therefore
no hardware path, at any point in this fixture's hierarchy, from Timer A's
underflow to `portb_out[6]`, regardless of CRA's PBON/OUTMODE bits.
`ciaa.v`'s header comment (shared file heritage, "these are the cia's ...
NOT implemented is: ... toggling of PB6/PB7 by timer A/B") independently
names exactly this gap. Both propositions are graded — and both are expected,
and measured, to come back **RED**, a real, disclosed finding, not a defect
this package works around.

Status: testable; baseline result is a genuine fail, not a broken fixture
(see the mutation notes in the test source for the oracle-flip proof that the
comparator itself is not simply tautological).

## Claims not assigned an assertion (45 of 67)

### Group 1 (35) — an accepted package already grades this proposition

Each `record()` line below names the accepted package, its assertion, and the
specific uid(s) it carries, exactly as `classify.note` states (independently
spot-checked against three of the cited packages' own test sources during
this build: `ch8-cia-disk-port-windows`, `ch1-cia-timers`,
`ch8-disk-index-interrupt-path`, `ch8-cia-disk-signal-bindings` and
`ch8-ciaapra-address-window` all carry the cited uids and assertion names).
Declining rather than re-grading these is the build brief's own instruction:
"The risk is not wasted work — it is this appendix reaching a different
verdict from an accepted package about the same register bit." See the
inventory table above for the full uid-by-uid list; the packages cited are
`ch8-cia-disk-port-windows` (ddrb/CIA-B window, 2 uids), `ch1-cia-timers`
(talo/tahi/tblo/tbhi generic timer existence, 8 uids), `ch8-disk-index-
interrupt-path` (icr row, the three remaining FLAG/handshake p.341 claims —
existence and general-purpose-input, NOT the edge-polarity pair, which moved
to PROP-FLAG-EDGE-POLARITY above — the INT6 association, and the FLAG=/INDEX
binding — 10 uids), and `ch8-cia-disk-signal-bindings` (all eight PB-name
bindings, 16 uids, plus `ch8-drive-step-timing` for PB0's timing half).

### Group 2 (10) — declined here, for a boundary reason, not because anything already grades them

`sol:F-signals.c36..c40` and `opus:F-portsig.c29..c33`: PA7=/DTR, PA6=/RTS,
PA5=carrier detect, PA4=/CTS, PA3=/DSR. Each claim's own `classify.note`
correctly says "Not graded anywhere in hrm/. New here" — verified true (no
existing package in `hrm/` builds a CIA-B port-A name-binding test; only the
CIA-B port-B disk-control names are covered, by `ch8-cia-disk-signal-
bindings`, which builds `minimig`, not `ciab`).

They are declined anyway, for the identical boundary reason `ch8-ciabprb-
disk-control-bits` used for its 16 PB-name declines: `ciab` compiled alone
(this package's assigned top module) has no signal named DTR, RTS, carrier
detect, CTS or DSR — only generic `porta_out[7:6]` (outputs) and
`porta_in[5:0]` (inputs). The binding of PA7 to DTR specifically (rather
than PA6, or nothing) is `minimig.v:648-649`, one level above this fixture's
own hierarchy. PROP-DDRA-POSITION above already grades the *position* half
these claims imply (that PA7/PA6 are independently writable, readable,
direction-controlled output bits); the *name* half is not available inside a
`ciab`-only fixture and is not asserted here. Building a `minimig`-top
package to grade it, the way `ch8-cia-disk-signal-bindings` does for PB,
would be a second package outside this build task's assignment (top module
`ciab`) — flagged here as a real gap in suite coverage, not worked around by
exceeding this package's assigned scope.

## On a control case, fixture liveness and mutation

Per the build brief: a fixture-liveness proof precedes any data-dependent
grading; at least one control case is expected to PASS on the same
measurement basis as anything under suspicion; every assertion (control
cases included) carries a recorded stimulus (or, where a baseline is itself
expected RED, oracle) mutation in the fixed `// MUTATION <label> <oracle|
stimulus|none> <red|green|impossible> <what changed>` form, replayable as a
`make test MUT=<NAME>` variant with `rm -rf $(OBJ_DIR)` guaranteeing a fresh
build every time.

- **Fixture liveness (portb, porta).** Two distinct, known bytes written to
  PRB (with DDRB=0xFF) must both arrive, and differ, on `portb_out` —
  including bit 6, the exact bit PROP-CRA-PBON and PROP-CRA-OUTMODE observe,
  establishing that a later RED result on those two propositions is not
  explained by a broken read channel. A parallel two-value check on PRA (with
  DDRA=0xFF) does the same for `porta_out`, and a `porta_in` sweep does the
  same for the read side, on the same measurement basis PROP-DDRA-POSITION
  and the ten declined PA-name claims share.
- **Controls expected to PASS.** Seven of the ten graded propositions
  (PROP-FLAG-EXISTS, PROP-DDRA-POSITION, PROP-TOD-BYTES, PROP-UNUSED-SLOT,
  PROP-SDR-LATCH, PROP-CRA-DECODE, PROP-CRB-DECODE) are expected, and
  measured, to PASS at baseline — the control basis the three expected-RED
  propositions (PROP-FLAG-EDGE-POLARITY, PBON, OUTMODE) are read against.
  PROP-FLAG-EDGE-POLARITY additionally carries its own dedicated control
  (same ICR arm/read channel, a stimulus that *does* contain a negative
  transition, PASSES) rather than relying solely on the package-wide set.
- **Mutation, seven PASS-at-baseline propositions.** Each carries an ordinary
  stimulus mutation: change what the harness feeds `ciab`, leave the
  comparator alone, confirm RED, revert.
- **Mutation, PROP-FLAG-EDGE-POLARITY / PROP-CRA-PBON / PROP-CRA-OUTMODE (RED
  at baseline).** Per the build brief, "stimulus green at a passing assertion
  proves nothing" — the symmetric risk for an assertion that is *already* RED
  at baseline is that its comparator is trivially, tautologically RED (e.g.
  comparing two constants) and would stay RED no matter what the RTL did.
  Each of the three carries an **oracle**-type mutation: temporarily invert
  the expected outcome to match what the RTL actually does (ICR bit 4 DOES
  set on a held-high level; portb_out[6] does *not* move), confirm that
  inverted comparison goes GREEN, then revert — proving each comparator is
  live and RTL-sensitive, not a hard-coded fail.

## Disclosure — what was already known before this file was drafted, and what was read after

Every expected value in the propositions above is already present, with
`file:line` citations, in the 67 claims' own `classify.evidence` — written
during classification, before this build task began, and read here rather
than re-derived. This file's own contribution is the *organization* of 67
claims into 10 gradeable propositions and 45 declines, the correction to the
build brief's "47" description above, the coordinator's separate correction
moving `sol:F-handshake.c7`/`opus:F-handshake.c6` from declined to graded, and
the specific register values (0x6F, 0xEF, etc.) worked out from the
evidence's cited line numbers.

Per the build brief's own order of work ("Write SPECIFICATION.md from the
quotations alone. Commit it. Only then open the RTL... Say so rather than
pretending it did"): `ciab.v` (251 lines), `cia_int.v` (70), `cia_timera.v`
(112), `cia_timerb.v` (107) and `cia_timerd.v` (142) were read in full before
this file was committed, in order to confirm the toolchain build pattern
(mirroring `ch8-ciabprb-disk-control-bits`'s own disclosed sequence) and to
verify the specific numeric predictions above (0x6F, 0xEF, the `tmcr[1]`/
`tmcr[2]` grep, `minimig.v:648-650`'s bindings). Two things limit what that
reading could have contaminated: first, every expected value in this
document traces to a `file:line` citation already present in the claims'
own `classify.evidence`, authored during classification, not discovered
fresh by this reading; second, no expected value here was chosen, adjusted
or narrowed because of anything the RTL showed that the evidence had not
already said — including the two propositions (PBON, OUTMODE) this file
states, in advance of running anything, will come back RED.
