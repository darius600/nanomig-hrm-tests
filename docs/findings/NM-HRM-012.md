# NM-HRM-012 — SPRxCTL bit 0 (SH0) is honoured at every resolution, not only SuperHires

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. The assertion imported a restriction the manual does not state. |
| **Module** | `denise_sprites.v` |
| **Test** | [`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers) — now green |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Error class** | **reading the page backwards** |

---

## Why it was withdrawn

**Printed p. 303** reads:

```
00      SH0       Start horiz. value Low bit 140 ns increment
```

SH0 is the ordinary **140 ns low bit**. The SuperHires positions are bits 4
(70 ns) and 3 (35 ns), two rows further down.

The assertion demanded that SH0 work only in SuperHires — which is not merely
unstated, it is the opposite of what the row says.

---

## What replaced it

The assertion now requires only what the row states: toggling `SPRxCTL` bit 0
moves the start by exactly one raw unit, with **no mode gate**. It is **green**.

The implementation was right; the oracle was not.

---

## Why this entry is kept

Three of four implementations take SH0 at every resolution, as NanoMig does. The
entry records that the disagreement was in the reading, and prevents the same
row being read the same way twice.
