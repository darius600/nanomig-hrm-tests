# ch8-joystick-switch-decode

Fixture: `userio.v`, compiled alone. Claims: `ch8-s1-build/ch8-joystick-switch-decode.jsonl`
(52 claims, two readers, printed pages 232, 233, 281).

This document is written from the quoted manual text alone (`extract.quote` /
`extract.restatement` in the claims file). No RTL file has been opened yet. Where a
proposition needs a concrete register or bit name, that name is one the manual itself
prints in the quoted passage (`JOY0DAT`, `JOY1DAT`, bit numbers, or the appendix's
`X1`/`X0`/`Y1`/`Y0`/`BIT#nn` notation) — never an RTL signal name.

## Grouping uids into propositions

52 claims, 27 propositions. Two readers (`opus`, `sol`) frequently extracted the same
sentence independently; where their `restatement` says the same testable thing, one
assertion grades both uids (label = uids joined by `+`). Where a bare id repeats between
readers but the *content* differs, or where a reader has no counterpart, the claim stands
on its own. Grouping was decided by comparing `restatement` text pairwise, not by trusting
the `classify.twin` field, which is classification-time metadata and in three page-281
pairs (`opus:...c87`/`sol:...c52`, `opus:...c88`/`sol:...c53`, and the three "To detect"
table rows) does not link claims that in fact restate the same sentence. Two claims on
different pages are never merged even when the underlying fact overlaps (page 232's
Table 8-3 and page 281's Appendix A describe the same bit positions from two different
printed passages — each gets its own assertion).

A table row is graded together with its legend/caption, never alone: Table 8-3's rows
("1 → true logic state of 'right' switch") only become a testable fact once composed with
the table's own title ("Interpreting Data from JOY0DAT and JOY1DAT"), so every Table 8-3
proposition below is phrased as a JOY0DAT/JOY1DAT bit fact, not a bare "bit 1 = right".
Appendix A's "To detect / Read these counter bits" table is self-contained (its quote
already includes the header naming the counter bits), and separately, "LEFT/RIGHT ... are
directly available on the Y1 and X1 bits of each counter" is a distinct prose sentence
that happens to state the same X1/Y1 mapping the table states — kept as its own
proposition since it is its own quote.

`classify.uid` is cited on every graded line, never the bare `id` — 304 ids in this slice
are shared by two readers and most carry different wording.

## Not in scope for this unit

- **Fire button.** Checked every quote and restatement in the 52 claims for a primary
  fire-button proposition (joystick pin 6 / `JOY*DAT` is not where that lives). None of
  the 52 claims mention it; the only button-related claim here (page 233) is the
  joystick's *second* button, which the manual ties to the same read mechanism as the
  mouse's right button — a `userio`-side fact, not a `ciaa` one. Nothing to decline by
  name for `ch8-ciaapra-fire-buttons`; noted here so the check is on record rather than
  silently assumed.
- **Controller port numbering (KNOWN CONFLICT 3).** Page 233 and page 240 number the
  physical controller ports differently (0/1 vs 1/2, per the build brief). None of this
  unit's 52 quotes assert a physical port number — they name registers (`JOY0DAT`,
  `JOY1DAT`, `POT0DAT`) whose trailing digit is part of the register name, not a
  contested port number, and the appendix table says "each counter" generically. No
  proposition below asserts a port-number claim; where an assertion needs one of a pair of
  registers, it is written against the register name only, on purpose.

## Propositions

Each row: label (the exact second token that will be passed to `expect()`), page, the
quotation's testable content. "Register" below means `JOY0DAT`/`JOY1DAT` — the manual's
own names; page 281 calls them "each counter" (same registers, per L09 below).

### Page 232 — main text and Table 8-3

| Label | uids | Proposition |
|---|---|---|
| `L01` | opus:8-joy.c1, sol:8-joy.c1 | A digital joystick has four directional switches — the interface exposes four independent direction stimuli, each with its own decode. |
| `L02` | opus:8-joy.c2, sol:8-joy.c2 | Each of the four switches can be activated on its own: driving one direction alone asserts only that direction's decode, the other three stay unasserted. |
| `L03` | opus:8-joy.c3, sol:8-joy.c3 | A diagonal stick position activates two *adjacent* switches together: driving two adjacent direction lines together asserts both decodes at once. |
| `L04` | opus:8-joy.c4, sol:8-joy.c4 | The joystick can indicate 8 distinct directions in total: the four cardinal and four diagonal stimuli produce 8 mutually distinct decode patterns. |
| `L05` | opus:8-joy.c6, sol:8-joy.c6 | Digital joystick switches are normally open: with nothing driving a switch line to its "pressed" level, that switch's true logic state reads not-closed. |
| `L06` | opus:8-joy.c7, sol:8-joy.c7 | Pressing a switch shorts its input line to ground: the stimulus that produces a "closed" reading is specifically the low (grounded) level on that line, not any other condition. |
| `L07` | opus:8-joy.c8, sol:8-joy.c8 | An open switch reads as "1": at the switch's own input line (before the register's documented 1=closed inversion), the open/idle level is logic 1. sol's wording locates this "at the connector input" explicitly; opus's identical source quote is read the same way, since Table 8-3 (L11) states the opposite convention at the *register* level and the two can only both be true if this one is about the pre-register line level. |
| `L08` | opus:8-joy.c9, sol:8-joy.c9 | A closed switch reads as "0" at that same input-line level — the low/grounded level, per L06. |
| `L09` | opus:8-joy.c10, sol:8-joy.c10 | The joystick data registers are the very same counters used for the mouse/trackball controllers — one register, two meanings, so a precondition (whatever selects "joystick meaning") must hold for the rest of this table's assertions to mean what they say. |
| `L10` | opus:8-joy.c11, sol:8-joy.c11 | The joystick registers are named JOY0DAT and JOY1DAT. |
| `L11` | opus:8-joy.c12, sol:8-joy.c12 | In JOY0DAT/JOY1DAT the true logic state of the switch data is "1 = switch closed" (register-level convention, the mirror of L07/L08's line-level one). |
| `L12` | opus:8-joy.c13, sol:8-joy.c13 | Table 8-3 + its title: JOY0DAT/JOY1DAT bit 1 is the true logic state of the "right" switch. |
| `L13` | opus:8-joy.c14, sol:8-joy.c14 | Table 8-3 + its title: JOY0DAT/JOY1DAT bit 9 is the true logic state of the "left" switch. |
| `L14` | opus:8-joy.c15, sol:8-joy.c15 | Table 8-3 + its title: the "back" switch's logic state is bit 1 XOR bit 0 of JOY0DAT/JOY1DAT. |
| `L15` | opus:8-joy.c16, sol:8-joy.c16 | Table 8-3 + its title: the "forward" switch's logic state is bit 9 XOR bit 8 of JOY0DAT/JOY1DAT. |
| `L16` | opus:8-joy.c17 (solo) | Table 8-3's title says it interprets data from *both* JOY0DAT and JOY1DAT — the same bit formula (L12-L15) holds on JOY1DAT, not just JOY0DAT. |

### Page 233 — second button

| Label | uids | Proposition |
|---|---|---|
| `L17` | opus:8-joy2.c5, sol:8-joy.c21 | The joystick's second button is read in the same way as the right mouse button: whatever observable reflects "right mouse button pressed" reflects "joystick second button pressed" identically, under matching stimulus procedure. |

### Page 281 — Appendix A

| Label | uids | Proposition |
|---|---|---|
| `L18` | opus:8-appa-joy.c15, sol:8-appa-joy.c17 | Bits 1 and 0 of each counter (register) can be read to determine the state of that counter's two clock pins. |
| `L19` | opus:8-appa-joy.c16, sol:8-appa-joy.c18 | Because bits 1/0 expose the clock-pin states, those same pins double as joystick switch inputs — i.e. the mouse-clock-pin path and the joystick-switch path are the same observable bits. |
| `L20` | opus:8-appa-joy.c53, sol:8-appa-joy.c26 | The joystick functions are all active-low *at the connector pins* (same line-level convention as L06/L07/L08, restated in the appendix). |
| `L21` | opus:8-appa-joy.c87, sol:8-appa-joy.c52 | The LEFT joystick function, active-high, is directly available on the Y1 bit of each counter. |
| `L22` | opus:8-appa-joy.c88, sol:8-appa-joy.c53 | The RIGHT joystick function, active-high, is directly available on the X1 bit of each counter. |
| `L23` | opus:8-appa-joy.c89 (solo) | FORWARD and BACK are *not* directly available on a single bit; recreating them requires exclusive-ORing the lower two bits of each counter. |
| `L24` | opus:8-appa-joy.c90, sol:8-appa-joy.c54 | Forward is detected as Y1 xor Y0, i.e. BIT#09 xor BIT#08. |
| `L25` | opus:8-appa-joy.c91, sol:8-appa-joy.c55 | Left is detected as Y1 alone. |
| `L26` | opus:8-appa-joy.c92, sol:8-appa-joy.c56 | Back is detected as X1 xor X0, i.e. BIT#01 xor BIT#00. |
| `L27` | opus:8-appa-joy.c93, sol:8-appa-joy.c57 | Right is detected as X1 alone. |

Cross-page note (not itself a graded proposition): page 281's X1/X0/Y1/Y0/BIT#nn notation
and page 232's Table 8-3 bit numbers describe the same four register bits (X1=bit1,
X0=bit0, Y1=bit9, Y0=bit8) — both tables are graded against JOY0DAT/JOY1DAT bits 0, 1, 8,
9, which is why L12-L16 and L21-L27 can share a stimulus vocabulary even though they come
from different printed pages and stay separate assertions.

## Test harness plan (no RTL-specific names yet)

- **Liveness first.** Before any of the 27 propositions are graded, the harness must show
  it can deliver a distinct, known word to the fixture and read it back changed — feeding
  two different stimuli and observing two different results — so that a later red result
  is never mistaken for a dead data path (this cost chapter 6 a harness once already).
- **Control case.** At least one assertion must be expected to PASS on the same
  measurement basis as the switch-decode assertions, so a red finding elsewhere cannot be
  waved off as a units error. L02 (single-switch independence) or L12/L13 (direct-bit
  mapping) are the natural candidates — whichever turns out cleanest once the register is
  actually read is fine as the control, decided in the build step, not here.
- **Precondition (L09).** Since JOY0DAT/JOY1DAT are shared with the mouse/trackball
  counters, whatever mechanism selects "this register currently means joystick data" is
  itself asserted once, separately from L12-L16, L21-L27, before those propositions are
  graded under it — per the brief's "assert the precondition too."
- **Idle/reset care.** "Normally open" (L05) and "active-low at the connector" (L07, L08,
  L20) all describe the *unpressed* stimulus level. The unpressed level must be driven
  explicitly (not left at whatever an unconnected simulation input defaults to) before any
  assertion reads it as a baseline — an unasserted input is not the same thing as an input
  correctly driven high, and only one of those two is "normally open" in the manual's
  sense.
- **JOY1DAT parity (L16).** Requires repeating the same stimulus battery used for JOY0DAT
  against the second port's lines and reading JOY1DAT instead.
- **Second button (L17).** Requires identifying, once the RTL is open, which observable
  reflects "right mouse button" so the joystick's second-button stimulus can be compared
  against it for equivalence — this is the one proposition where the concrete observable
  genuinely isn't named in the quoted text and is deferred to the build step, not invented
  here.

## Mutation plan

Every graded proposition gets a stimulus mutation recorded in the file header, per the
brief's fixed line form. Default plan: change the stimulus that proposition depends on
(flip which line is driven, drive an adjacent instead of the intended line, etc.), leave
the expected value alone, confirm red, revert. Where a mutation would only be possible by
degenerating into "flip the expected value" (which proves nothing), that will be recorded
as `impossible` with the required reason (`unfalsifiable at this boundary` vs `not
exercised, by a scope decision`) rather than forced.

One thing already known and carried forward, so it is not rediscovered the expensive way:
a mutation that flips both operands of an XOR (e.g. both bits feeding L14/L15/L24/L26)
leaves the XOR's output invariant — `ch8-mouse-quadrature-decode` found exactly this on
the same fixture's quadrature XOR. Mutations against L14, L15, L24, L26 will each flip
only one side of their XOR, not both.

## HALT check

Nothing here met a HALT condition. The manual text is not ambiguous at any of the 27
propositions' point; every one of them is decidable from the quoted sentence plus, where
noted, the row's own table legend. No proposition needed a port or signal name beyond what
the manual itself prints (`JOY0DAT`, `JOY1DAT`, bit numbers, `X1`/`X0`/`Y1`/`Y0`) to be
stated testably — the RTL step still has to bind these to the fixture's actual ports, but
that binding is exactly what step 2 is for, not a precondition for writing this document.
