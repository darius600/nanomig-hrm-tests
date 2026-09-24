# NM-HRM-013 — SPRxCTL bits 3 and 4 are read nowhere

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. It was split; the surviving half is [NM-HRM-019](NM-HRM-019.md). |
| **Module** | `denise_sprites.v` |
| **Test** | [`hrm/ch3-sprite-registers`](../../hrm/ch3-sprite-registers) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Error class** | **a scope note read as a requirement** |

---

## Why it was withdrawn

The finding rested on the note:

> "bits 3 and 4 are in the ECS chips only"

That is a statement about **which chips have them**, not a promise that both
alter output. Requiring bit 3 to be a live field contradicts the oracle, because
the same page settles bit 3 in the opposite direction:

```
03      SHSH0     Start horizontal (SHR mode) 35ns (unimplemented)
```

**The manual itself calls bit 3 unimplemented.**

---

## What replaced it

The two bits part company, and the split is what the withdrawal produced:

| bit | what the manual prints | outcome |
|---|---|---|
| 3 | `(unimplemented)` | declined, quoting the manual's own word |
| 4 | `SHSH1  Start horizontal (SHR mode) 70ns increment` | a behaviour — re-asserted on its own as [NM-HRM-019](NM-HRM-019.md), and red for a reason that holds |

**The original finding was right that something is missing, and wrong about
what.** That is the most useful shape a withdrawal can have.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS)**, PDF page 10 — **agrees with NanoMig**:

  > "06-04 X Not used."
  <!-- corpus: denise-spec-252126/p010.txt -->

  This ECS Denise specification (it reports 8373 revisions via DENISEID) prints SPRxCTL bits 06-04 as not used and has no row for bit 03: on bit 4 it sides with NanoMig reading neither bit, against the 3rd edition's SHSH1 70ns increment; bit 3 the manual itself calls unimplemented. The table has the shape of an OCS-era table and lacks even the EV8 row, so it may be an un-updated carry-over. It bears equally on NM-HRM-019, the surviving half.

Silent on this point: Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; US patent 4,874,164, Commodore-Amiga.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

