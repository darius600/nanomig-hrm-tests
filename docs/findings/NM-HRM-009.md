# NM-HRM-009 — CLXDAT bit 15 reads as 1, not 0

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. The expected value was invented by the assertion. |
| **Module** | `denise_collision.v` |
| **Test** | [`hrm/ch3-collision-registers`](../../hrm/ch3-collision-registers) — now green |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
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

## Why this entry is kept

It is one of four withdrawals that are the same mistake in different clothes: a
name, a scope note, an absence or a permission read as a requirement. Together
they are the clearest statement in this project of how a false finding gets made.

**A heading is not a specification of the bits underneath it.**
