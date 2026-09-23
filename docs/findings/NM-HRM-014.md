# NM-HRM-014 — the manual applies its own ECS pointer extension inconsistently

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a source observation, not test-backed |
| **What is asked of a reader** | Nothing yet. This was read from the source rather than measured by a graded assertion, and needs a test before it can ask for anything. |
| **Module** | — |
| **Test** | [`hrm/ch3-register-map-agnus`](../../hrm/ch3-register-map-agnus) — recorded as a `note()`, not an assertion |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | one of four extends every DMA pointer by chipset; **no implementation can settle a question about the manual's own text** |

---

## What this is

A disagreement **inside the manual**, about how widely its own ECS pointer
extension applies. Three sources give three widths for the same class of
register.

It is recorded in the package as `checks.note(...)`, whose own text says
`Not folded into any PASS/FAIL above`.

---

## Why there is no test

**There is nothing to test.** The disagreement is between printed pages, not
between a page and the RTL. An assertion would have to pick one page as the
oracle and measure the RTL against it, which answers a different question and
would produce a red or green that says nothing about the inconsistency itself.

No assertion, red result or control stands behind this entry, and the entry says
so. It is listed for completeness of the audit.

---

## Why this entry is kept

Because the alternative is worse. A manual conflict that is noticed and not
recorded gets re-discovered by the next reader, who has no way to tell whether
anyone looked.

And it is the reason [NM-HRM-003](NM-HRM-003.md), [NM-HRM-022](NM-HRM-022.md) and [NM-HRM-024](NM-HRM-024.md) have to be read
together: if the manual applies its own extension inconsistently, then three
findings that each quote one of its sentences are three views of one unsettled
question.
