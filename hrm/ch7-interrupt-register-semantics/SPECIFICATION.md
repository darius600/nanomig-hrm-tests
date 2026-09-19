# Specification: ch7-interrupt-register-semantics

Fixture: `paula_intcontroller`. Subject: the four interrupt addresses, the
SET/CLR convention, bit 15 reading zero, the master gate, and write-to-clear
stickiness.

Written from the quotations in `ch7-interrupt-register-semantics-spec-stage.jsonl`
alone (41 claims). No RTL has been opened to produce this document. Every
claim is cited by its `uid` (`<reader>:<id>`), never by the bare `id`, per
BUILD-BRIEF.

## Chipset scope

Every one of the 41 claims carries the identical chipset note: chapter 7's
body is the OCS account (printed p.224 hands the Enhanced Chip Set's system
control registers to Appendix C), and Appendix C carries no INTENA / INTENAR
/ INTREQ / INTREQR entry anywhere (checked by both readers, zero hits). So
this register's behaviour is unchanged by ECS, and every proposition below
applies to OCS and ECS alike — one fixture run answers for both.

## Testable propositions

### P1 -- Interrupt sources can be individually masked

> "Control registers within the peripherals chip allow you to mask certain of
> these sources and prevent them from generating a 680x0 interrupt." (printed
> p.217)

Testable form: for an arbitrarily chosen non-master selector bit position
(i.e. not bit 14/INTEN, which P17/P18 cover on its own), with that position's
enable bit clear and a pending request already present at the same position,
the interrupt-priority output does not reflect that request; setting the
enable bit (nothing else changed) lets the same pending request reach the
output. This is the general masking claim, tested at one representative
ordinary bit rather than the full 14-source table (that table, and the
identity of each numbered source, is `hrm/ch5-audio-interrupt`'s
INTERRUPT-SOURCE-BIT-AND-PRIORITY-LEVEL-TABLE assertion, not this package's
— see "Package boundary" below).

Cited by: `sol:7-interrupts.c5`, `opus:7-int.c6` (twin, same proposition,
both taken).

### P2 -- Two logical registers: enable/mask and request/status

> "There are two interrupt registers, interrupt enable (mask) and interrupt
> request (status)." (printed p.217)

Testable form: the module exposes two independently-addressable pieces of
stored state — one gating (enable/mask), one recording pending requests
(request/status) — distinguishable because a write that changes one leaves
the other's readback unaffected.

Cited by: `sol:7-interrupts.c10`, `opus:7-int.c9` (twin, same, both taken;
opus's quotation is the two-sentence form that also covers P3).

### P3 -- Each logical register has a separate read address and write address

> "Each register has both a read and a write address." (printed p.217)

Testable form: four distinct addresses exist in total, forming two
read/write pairs (enable pair, request pair); writing one pair's write
address changes only that pair's paired read address, never the other
pair's.

Cited by: `sol:7-interrupts.c11`; co-cited (same sentence, combined quote):
`opus:7-int.c9`.

### P4 -- INTENA: write-only, sets or clears enable bits

> "INTENA — Interrupt0 enable (mask) - write only. Sets or clears specific
> bits of INTENA." (printed p.217 — "Interrupt0" is the manual's own wording,
> confirmed present on the rendered printed page by the crossread, not a
> corpus typo)

Testable form: the enable register's write address accepts a SET/CLR-encoded
write (P11-P13) that sets or clears selected enable bits, and reading that
same write address back never exposes the stored enable content (the
write-only half).

Cited by: `sol:7-interrupts.c12`, `opus:7-int.c10`.

### P5 -- INTENAR: read-only, reads INTENA's contents

> "INTENAR — Interrupt enable (mask) read - read only. Reads contents of
> INTENA." (printed p.217)

Testable form: reading the enable register's read address returns exactly
what the most recent enable-register write established; this address is
read-only (a write to it, if the fixture allows one to be issued at all, has
no effect on stored state).

Cited by: `sol:7-interrupts.c13`, `opus:7-int.c11`.

### P6 -- INTREQ: write-only, can force a software-triggered request

> "INTREQ — Interrupt request (status) - write only. Used by the processor to
> force a certain kind of interrupt to be processed (software interrupt)."
> (printed p.218; sol's quotation crosses the page-217/218 render boundary
> and was split into two claim rows with a reformatting defect the crossread
> restored without changing the proposition — see `restored_quote` on
> `sol:7-interrupts.c14`)

Testable form: a SET-encoded write to the request register's write address
can itself set a pending-request bit with no external hardware pulse
involved (the manual's own example of a purely software-driven request), and
reading that write address back never exposes stored content.

Cited by: `sol:7-interrupts.c14` (restored quote), `sol:7-interrupts.c15`,
`opus:7-int.c12` (restored quote, complete sentence).

### P7 -- INTREQ write also clears pending flags; a pending flag does not clear itself

> "Also used to clear interrupt request flags once the interrupt process is
> completed." (printed p.218)

Split (per the claims' own `split` field on both readers' rows): asserted =
"a CLR write to INTREQ naming a bit clears that pending flag"; excluded =
"once the interrupt process is completed" — the moment of servicing, for
which this module has no interrupt-acknowledge/IACK-equivalent signal to
test against (absent signal, not a HALT: the rest of the proposition is
still testable).

Testable form (two-sided, one proposition — the "stickiness" the package is
named for): (a) a pending request bit, once set, stays set across an
arbitrary number of idle cycles with no other stimulus applied — nothing
clears it on its own; (b) a CLR-encoded write to INTREQ naming that bit
clears it. Both must hold for this proposition to be green.

Cited by: `sol:7-interrupts.c16`, `opus:7-int.c13` (twin, same split).

Excluded remainder (not asserted, not measured — no signal exists to
measure it against): the "once the process is completed" timing.

### P8 -- INTREQR: read-only, reports which sources are requesting service

> "INTREQR — Interrupt request (status) read - read only. Contains the bits
> that define which items are requesting interrupt service." (printed p.218)

Testable form: reading the request register's read address returns exactly
the pending bits established by request-writes and by any hardware-latched
requests this fixture can drive; the address is read-only.

Cited by: `sol:7-interrupts.c17`, `sol:7-interrupts.c18`, `opus:7-int.c14`
(combined quote).

### P9 -- Bit positions correspond between the request and enable registers

> "The bit positions in the interrupt request register correspond directly
> to those same positions in the interrupt enable register." (printed p.218)

**Already asserted elsewhere — not duplicated in this package.**
`sol:7-interrupts.c19`'s own note says so explicitly: "Already asserted from
appendix A by hrm/ch5-audio-interrupt assertion 3; ledger cross-reference
only." `hrm/ch5-audio-interrupt`'s SHARED-BIT-LAYOUT-INTENA-INTREQ assertion
tests exactly this fact against the same module (its own claim ids
`5-appA-p280.c17`, `5-appA-p280.c40`, `A-intena.c1`, `A-intreq.c5` — all
distinct from, but the same real-world proposition as, this pair). This
package records the cross-reference and asserts nothing new for it.

Cited (ledger cross-reference, not asserted here): `sol:7-interrupts.c19`,
`opus:7-int.c15` (same proposition, no fresh assertion).

### P10 -- Bit 15 has no meaning on the read-only addresses

> "The only difference between the read-only and the write-only addresses
> shown above is that bit 15 has no meaning in the read-only addresses."
> (printed p.218; also stated earlier and more briefly on the same page:
> "bit 15 has no meaning in the read-only addresses.")

Unique to this package: `sol:7-interrupts.c20`'s own note says "Chapter 7's
own sentence; appendix A does not carry it and hrm/ch5-audio-interrupt does
not assert it."

Testable form, held strictly to what the quote supports: the manual commits
only to bit 15 carrying **no defined meaning** on INTENAR/INTREQR reads, not
to any specific value. The proposition this package can actually grade is
that bit 15 of a read from either read-only address does not track or vary
with stored register content — i.e. whatever fixed behaviour it shows is
independent of what has been written. The package's own subject line calls
this "bit 15 reading zero"; this document deliberately does not pre-commit
to that specific constant, because the quotation itself does not name one —
whatever constant it turns out to be will be measured in step 3 and used as
the concrete expected value there, not assumed here. (See "A note on how
this proposition was written" below.)

Cited by: `sol:7-interrupts.c20`, `opus:7-int.c16`.

### P11 -- Bit 15 is the SET/CLR bit (of INTENA and INTREQ; DMACON excluded)

> "Bit 15 of these registers is called the SET/CLR bit." (printed p.218; the
> fuller sentence, quoted by opus, is "The interrupt registers, as well as
> the DMA control register, use a special way of selecting which of the bits
> are to be set or cleared. Bit 15 of these registers is called the SET/CLR
> bit.")

Split (both readers' rows carry the same split): asserted = "bit 15 is the
SET/CLR bit of INTENA and INTREQ"; excluded = "as well as the DMA control
register" — DMACON is not a port of this module (absent signal, out of this
package's boundary, not a HALT).

Testable form: bit 15 of a write to INTENA or INTREQ selects between two
write-encodings (P12/P13) for the other 15 bits of that same write.

Cited by: `sol:7-interrupts.c21`, `opus:7-int.c17`.

### P12 -- SET encoding

> "When you wish to set a bit (make it a 1), you must place a 1 in the
> position you want to set and a 1 into position 15." (printed p.218)

Testable form: a write with bit 15=1 and exactly one selector bit b=1 sets
stored bit b to 1, confirmed by reading it back.

Cited by: `sol:7-interrupts.c22`, `opus:7-int.c18`.

### P13 -- CLR encoding

> "When you wish to clear a bit (make it a 0), you must place a 1 in the
> position you wish to clear and a 0 into position 15." (printed p.218)

Testable form: a write with bit 15=0 and exactly one selector bit b=1 clears
stored bit b to 0, confirmed by reading it back.

Cited by: `sol:7-interrupts.c23`, `opus:7-int.c19`.

### P14 -- Positions 14-0 are bit selectors, and several may be selected at once

> "Positions 14-0 are bit selectors." (printed p.218; opus's fuller quotation
> adds "You write a 1 to any one or more bits to select that bit.")

Testable form: any subset of bits 14-0 written as 1 in a single write,
together with that write's one shared bit-15 choice, selects the whole
subset for the same set-or-clear operation (the multi-bit form of P12/P13).

Cited by: `sol:7-interrupts.c24`, `opus:7-int.c20`.

### P15 -- Selector positions written 0 are left unaffected

> "Positions 14-0 that have 0 value will not be affected when you do the
> write." (printed p.218)

Testable form: any selector bit written 0 keeps its previously stored value
regardless of bit 15. This is also the control/precondition that makes
P12-P14 mean "selected bits only, nothing else disturbed" rather than a
whole-register overwrite — needed for those propositions' green to be
meaningful, so it is asserted in its own right, per BUILD-BRIEF's rule for
preconditions a green depends on.

Cited by: `sol:7-interrupts.c25`, `opus:7-int.c21`.

### P16 -- Setting some bits and clearing others in the same operation takes two writes

> "If you want to set some bits and clear others, you will have to write this
> register twice (once for setting some bits, once for clearing others)."
> (printed p.218)

Crossread flagged this quotation "borderline" (reads as programming advice)
but accepted it as "the falsifiable consequence of the SET/CLR encoding" —
it is a plain, testable consequence of P11-P13 (bit 15 is single-valued per
write), not merely advice: no single write can leave the register holding a
target pattern that both sets bits beyond what a single shared bit-15 value
allows to be set AND clears bits that same value does not allow to be
cleared; reaching such a mixed pattern needs two writes.

Cited by: `opus:7-int.c22` (no sol counterpart for this specific claim).

### P17 -- Bit 14 (INTEN) is the master interrupt enable

> "Bit 14 of the interrupt registers (INTEN) is for interrupt enable."
> (printed p.218; opus's fuller quotation adds "This is the master interrupt
> enable bit.")

Testable form: bit 14 of the enable register is distinguished by function
from the other 14 enable bits (tested jointly with P18, its behavioural
half).

Cited by: `sol:7-interrupts.c26`, `opus:7-int.c23`.

### P18 -- INTEN as master gate over every other enable (the undisputed half)

> "If this bit is a 0, it disables all other interrupts." (printed p.218)

This is the undisputed half of a claim group where the manual disagrees with
itself (see P19). Per the task's explicit resolution: assert this half,
because appendix A (p.280, "Master interrupt (enable only, no request)") and
Figure 7-4's own gloss (p.221, "special (master enable)") agree with it —
nothing printed contradicts the gate function itself, only whether INTEN can
also raise a request of its own (P19).

Testable form: with at least one ordinary (non-master) source already
enabled and pending — the control case, so the test measures gating rather
than the trivial absence of any request at all — clearing INTEN (bit 14 of
INTENA) alone drives the interrupt-priority output to its inactive/no-request
state; setting INTEN alone (nothing else changed) then releases exactly that
already-enabled, already-pending source to the output. Both directions are
one proposition and both must hold for a green.

Cited by: `sol:7-interrupts.c27`, `opus:7-int.c24`. Corroborated (undisputed
half only, not a primary citation) by `sol:7-appA-int.c10` and
`opus:7-appA-int.c6` (appendix A p.280's "master interrupt... enable only"
clause) and by `opus:7-fig7-4.c45` (Figure 7-4 p.221's "special (master
enable)" description text only, not its hardware-priority-6 placement, which
belongs to P19).

### P19 -- [RECORD ONLY, no pass/fail] Disputed half: can INTEN raise a request of its own?

Printed p.218: "This bit is used for enable/disable only. It creates no
interrupt request." Appendix A printed p.280: "Master interrupt (enable
only, no request)" with a blank LEVEL cell — agrees with p.218. Figure 7-4
printed p.221: places INTEN inside the hardware-priority-6 bracket ("Exec
software priority 15, special (master enable), INTEN, at hardware priority
6") — assigns it a 68000 interrupt level, i.e. treats it as a source capable
of raising a request in its own right. p.218 and p.280 contradict p.221.

Per the task's explicit instruction: do not put a red on a proposition one
printed page contradicts. Neither reading is asserted; this row is measured
and reported (`record`, never `expect`) once the fixture can show what the
RTL actually does here.

Cited (record, not assert): `sol:7-interrupts.c28`, `opus:7-int.c25`,
`sol:7-appA-int.c10`, `opus:7-appA-int.c6` (this last row's own crossread
notes that the "spurious-conflict-flag" objection to treating p.280 as
conflicting was itself overruled once p.280's blank LEVEL cell is read
beside p.221's assignment of level 6 — the conflict is real), and
`opus:7-fig7-4.c45` (hardware-priority-6 placement half only; its "special
(master enable)" description half corroborates P18 instead, see above).

## Fixture data-path liveness precondition

Per BUILD-BRIEF's silence-trap requirement, before any assertion above that
depends on a read reflecting a write (P4-P10, P12-P19 all do), the harness
must first prove the fixture's write/read path is alive: write a distinct,
non-trivial 15-bit pattern to the enable register and confirm the paired
read address returns it, and separately do the same for the request
register, before trusting any of P4-P19. This is not sourced from a
quotation — it is the engineering precondition every package in this tree
carries — and is asserted as such, not cited to a page.

## Not turned into assertions

- The "once the interrupt process is completed" clause of P7 (the moment of
  servicing) — no interrupt-acknowledge/IACK-equivalent signal exists on
  this claim's evidence to measure that moment against. Excluded, not
  recorded (nothing to measure).
- The "as well as the DMA control register" clause of P11 — DMACON is not a
  port of this module. Excluded, not recorded.
- P9's proposition (bit-position correspondence) — already asserted by
  `hrm/ch5-audio-interrupt`; cross-referenced, not duplicated.
- P19 (whether INTEN can raise its own request) — recorded, not asserted,
  per the manual's self-contradiction.

## A note on how P10 was written

The task briefing's own subject line for this package says "bit 15 reading
zero." The quotation this package is restricted to only supports "bit 15 has
no meaning" — it does not commit to which constant that is. Committing to
"zero" in this document, ahead of any RTL, would be writing an expected
value that starts life as a fact about the implementation rather than the
manual — the exact defect BUILD-BRIEF and the task briefing both warn
against. P10 is written to the weaker, quote-supported form; the concrete
constant is left for step 3 to measure and report against that form.

## Package boundary — prior coverage in this tree

Two existing, accepted packages already test this module:

- **`hrm/ch5-audio-interrupt`** (chapter 5, appendix A p.280 / appendix B
  pp.289-290-sourced, 106 claims, its own `expect()`-based harness) already
  asserts: the four-address register map and write-only-reads-as-constant
  behaviour, the shared bit-layout between INTENA and INTREQ (this
  package's P9), the full SET/CLR bit-15 mechanics including the two-write
  requirement, the 14-source bit/priority-level table, pending-bits
  pollable independent of enable, sticky-until-explicit-clear, and the
  INTEN master-gate / no-request split (its assertions 5 and 5B — 5B is
  asserted and EXPECTED RED there, a different disposition than this
  package's `record` for the same disputed fact).
- **`hrm/paula_intcontroller`** (no `ch7-` prefix; chapter 7, printed
  pp.217-222, pre-`expect()`-harness, no claim citations, "FPGA-038/039/040"
  numbering — found by name inside `ch5-audio-interrupt`'s own header while
  reading it per this package's task instructions, not part of the 41
  claims; **that package has since been removed** — see
  `docs/legacy-names.md`): a single monolithic pass/fail test covering one INTENA/INTREQ
  write+readback+one-clear round trip (038), a VBLANK hardware pulse
  latching into INTREQ (039), and four `_ipl` priority-level scenarios
  (040: none enabled=7, VBLANK alone=level 3, VBLANK+COPER=still level 3,
  EXTERNAL alone=level 6).

What this package adds beyond both: an independent, chapter-7-body-sourced
(pp.217-218, plus Figure 7-4 p.221) reading of the same register semantics,
which is valuable as cross-chapter corroboration in its own right; P1's
general masking statement (present in neither prior package); P10, bit 15's
"no meaning" on the read-only addresses (explicitly absent from both, per
`sol:7-interrupts.c20`'s own note); and a `record`, not `expect`, disposition
for the disputed INTEN-self-request half, folding in Figure 7-4's
corroborating (and, per `opus:7-appA-int.c6`'s crossread, genuinely
conflicting) placement — material neither prior package's claims name.

## Claims accounted for

All 41 claims in `ch7-interrupt-register-semantics-spec-stage.jsonl` are
accounted for above:

- P1: 2 claims — P2: 2 — P3: 1 (+1 co-cited from P2's row) — P4: 2 — P5: 2
- P6: 3 — P7: 2 — P8: 3 — P9: 2 (ledger cross-reference, not asserted)
- P10: 2 — P11: 2 — P12: 2 — P13: 2 — P14: 2 — P15: 2 — P16: 1 — P17: 2
- P18: 2 primary + 3 corroborating (shared with P19) — P19: 5 (record only,
  3 shared with P18's corroboration)

Total distinct claim rows: 41 (`sol:` 21, `opus:` 20), matching the task
sheet's count.
