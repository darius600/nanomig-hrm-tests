# NM-HRM-009 — CLXDAT bit 15 reads as 1, not 0

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. The expected value was invented by the assertion. |
| **Module** | `denise_collision.v` |
| **Test** | [`hrm/ch3-collision-registers`](../../../hrm/ch3-collision-registers) — now green |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Error class** | **a silence read as a requirement** |

---

## Why it was withdrawn

**Printed p. 273** lists bit 15 as `not used`, under the heading **COLLISIONS
REGISTERED**.

The heading scopes it: it says **no collision is registered there**. It does not
say the bit reads zero.

NanoMig wires it high, which contradicts nothing printed. The expected zero came
from the assertion, not from the page — so the red measured the oracle, not the
RTL.

---

## What replaced it

The assertion now requires bit 15 to hold the **same value** before and after
maximal collision stimulus, without claiming which value. That is what the page
supports, and it **passes**: 31/5 became 32/4.

The implementation was right all along.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](../README.md#second-documents).

- **US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989**, PDF page 29 — agrees with the manual:

  > "Fifteen bits in the collision storage register are utilized to indicate whether any of several types of overlaps has occurred"
  <!-- corpus: patent-us4874164-agnus-denise/p029.txt -->

  Fifteen of sixteen bits carry collisions, the manual's 'not used' for the remaining bit in other words; like the manual it gives that bit no value (p020.txt calls the input 'A 16-bit output from the collision detection logic'). Same passage in patent-us5103499-copper/p029.txt.

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 7 — agrees with the manual:

  > "15 Not Used"
  <!-- corpus: denise-spec-252126/p007.txt -->

  The ECS Denise specification repeats the manual's row under the same heading 'COLLISIONS REGISTERED' and also gives no read value for bit 15.

Silent on this point: Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988.

---

## Why this entry is kept

It is one of four withdrawals that are the same mistake in different clothes: a
name, a scope note, an absence or a permission read as a requirement. Together
they are the clearest statement in this project of how a false finding gets made.

**A heading is not a specification of the bits underneath it.**
