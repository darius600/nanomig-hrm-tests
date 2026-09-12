# NM-HRM-018 — ECS BPLCON0 bit 4 (SPRHWRM) has no effect

| | |
|---|---|
| **Status** | **WITHDRAWN as a conformance finding** — a source observation remains |
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

## Why this entry is kept

**The overreach was the lead's**, recorded earlier the same day and corrected the
same day. A row carrying only a mnemonic was turned into a conformance finding;
the contrast with the `SHRES` row three lines above is what settles it.

This is the sharpest of the four "silence is not a requirement" withdrawals,
because the counter-example sits on the same page.
