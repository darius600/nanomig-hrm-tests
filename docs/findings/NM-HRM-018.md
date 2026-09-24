# NM-HRM-018 — ECS BPLCON0 bit 4 (SPRHWRM) has no effect

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | a source observation, not test-backed |
| **What is asked of a reader** | Nothing yet. This was read from the source rather than measured by a graded assertion, and needs a test before it can ask for anything. Withdrawn as a conformance finding; the source observation remains. |
| **Module** | `denise.v` |
| **Test** | [`hrm/ch3-bplcon0-display-modes`](../../hrm/ch3-bplcon0-display-modes) — 225/2 became 225/0 |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Error class** | **a name read as a requirement** |

---

## Why it was withdrawn

**Printed p. 300's row reads, in full:**

```
04       SPRHWRM
```

A mnemonic with **no description column** — unlike `06  SHRES  SuperHires 35ns
pixel enable bit` three rows above, which shows what the manual looks like when
it actually specifies something.

The manual gives bit 4 a name and no behaviour, so there is **no printed
behaviour for NanoMig to diverge from**.

The fixture could not have observed it in any case: it drives bitplanes only,
while the mnemonic names a sprite-horizontal function.

---

## What replaced it

Both claims are now **declined** rather than asserted, with the reason printed:
the manual states no behaviour for this bit, and the fixture drives bitplanes
while the mnemonic names a sprite function. Neither is asserted true nor false —
the source simply does not define anything to test.

---

## What remains true

A source observation: `denise.v:181` gives the bit its **AGA** meaning
(`bplcon0[4] & aga`) and no ECS meaning. That is worth knowing and is not a
conformance failure, because the manual covers OCS and ECS and prints no ECS
behaviour for this bit.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS)**, PDF page 5 — says something else:

  > "04 y PF2H0 PF2P1 BRDNTRAN"
  <!-- corpus: denise-spec-252126/p005.txt -->

  In the ECS Denise specification BPLCON0 bit 4 is 'y', and 'y= register bits contained in AGNUS, not defined here.': no SPRHWRM name and no Denise function. That is compatible with the withdrawal (no printed behaviour) and with denise.v giving the bit no ECS meaning; what it adds is that the bit belongs to Agnus, where a Denise-only bitplane fixture could not observe it.

Silent on this point: Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; US patent 4,874,164, Commodore-Amiga.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

---

## Why this entry is kept

**The overreach was the lead's**, recorded earlier the same day and corrected the
same day. A row carrying only a mnemonic was turned into a conformance finding;
the contrast with the `SHRES` row three lines above is what settles it.

This is the sharpest of the four "silence is not a requirement" withdrawals,
because the counter-example sits on the same page.
