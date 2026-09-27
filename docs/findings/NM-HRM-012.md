# NM-HRM-012 — SPRxCTL bit 0 (SH0) is honoured at every resolution, not only SuperHires

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. The assertion imported a restriction the manual does not state. |
| **Module** | `denise_sprites.v` |
| **Test** | [`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers) — now green |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Error class** | **reading the page backwards** |

---

## Why it was withdrawn

**Printed p. 303** reads:

```
00      SH0       Start horiz. value Low bit 140 ns increment
```

SH0 is the ordinary **140 ns low bit**. The SuperHires positions are bits 4
(70 ns) and 3 (35 ns), printed four and three rows above it.

The assertion demanded that SH0 work only in SuperHires — which is not merely
unstated, it is the opposite of what the row says.

---

## What replaced it

The assertion now requires only what the row states: toggling `SPRxCTL` bit 0
moves the start by exactly one raw unit, with **no mode gate**. It is **green**.

The implementation was right; the oracle was not.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — agrees with the manual:

  > "00 SH0 Start horiz. value low bit."
  <!-- corpus: denise-spec-252126/p010.txt -->

  The ECS Denise specification, which also defines SHRES, gives SH0 as the ordinary start-horizontal low bit with no SuperHires condition (SPRxPOS: 'Low bit (SH0) is in SPRxCTL reg below.'). Manual and NanoMig already agree here; this is one more voice for the same reading.

Silent on this point: Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989.

---

## Why this entry is kept

All four implementations take SH0 at every resolution, as NanoMig does. The
entry records that the disagreement was in the reading, and prevents the same
row being read the same way twice.
