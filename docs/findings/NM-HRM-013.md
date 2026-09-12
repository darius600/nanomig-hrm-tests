# NM-HRM-013 — SPRxCTL bits 3 and 4 are read nowhere

| | |
|---|---|
| **Status** | **WITHDRAWN, split into [NM-HRM-019](NM-HRM-019.md)** |
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
