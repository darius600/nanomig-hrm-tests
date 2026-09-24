# NM-HRM-029 — Figure 5-8's `dmasen` note does not hold outside its own exception

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. The assertion was logically false. |
| **Module** | `paula_audio_channel.v` |
| **Test** | [`hrm/ch5-audio-channel-state`](../../hrm/ch5-audio-channel-state) — the assertion is gone |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Error class** | **a one-way implication tested as a biconditional** |

---

## Why it was withdrawn

**Figure 5-8's NOTE says, after its exception:**

> "dmasen is true only when LENFIN=1"

That is the implication `dmasen → LENFIN`. It is falsified by exactly one thing:
a sample where `dmasen` is true and `LENFIN` is not.

**The assertion searched for the opposite** — at least one later sample where
*both* were true — and went red when none was found:

```cpp
if (s.lenfin && s.dmasen) dmasen_true_elsewhere_at_lenfin = true;
...
CHECK("... the note's remaining claim requires 'yes'", dmasen_true_elsewhere_at_lenfin);
```

**Absence of later `dmasen` pulses satisfies "only when". It does not violate
it.** The red reported the opposite of what the figure says.

---

## What replaced it

The test now evaluates the implication `dmasen → lenfin`, which passes vacuously
when no non-excepted `dmasen` occurs. The companion formula
`AUDxDSR = AUDxDR · dmasen` became an informational `MEASURED` note.

If the package cannot produce a `dmasen` at all, that is a green with a caveat —
recorded, not asserted, because an implication no stimulus ever triggers has not
been tested.

---

## What the manual actually says, and what the RTL does

**Figure 5-8's boxed NOTE, printed p. 166:**

> "Except for this case, dmasen is true only when LENFIN=1. Also,
> AUDxDSR=AUDxDR · dmasen"

The excepted case is the `000→001` transition, whose action bracket is
`[lencntrld, AUDxDR, dmasen]`.

**Measured:** `dmasen` never correlates with `lenfin` anywhere outside that
stated exception. The note's own AND equation holds at the first restart and at
neither later, `lenfin`-driven one.

**RTL:** `dmasen` is raised only in the idle-state DMA branch
([`paula_audio_channel.v:276`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L276)); the block-end restart comes from
[`:220`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L220), `dmas <= dmasen | lenfin`. The implemented relation is therefore

    AUDxDSR = AUDxDR ∧ (dmasen ∨ lenfin)

— the `lenfin` term sits **one register level further out** than the figure puts
it, computing the same output.

## The sibling assertion is a separate question, and also not a failure

The `AUDxDSR = AUDxDR · dmasen` equation compares the public `dmas` port against
**two internal signals** reached through `--public-flat-rw`. So it asks whether
NanoMig arranges two terms the way the figure draws them, not whether the chip
behaves as the manual requires. An implementation is free to place the term
wherever it likes if the output is the same, and here it is.

Whether the printed arrangement is normative is a question about the figure, not
about the design.

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

---

## Why this entry is kept

Four other withdrawals in this register read a **permission or a silence** as a
requirement. This is the same error **in the direction nobody was watching for**:
a one-way implication tested both ways.

It is the first of its kind here, and it is the reason the playbook now says:
write the claim as a formula before writing the predicate.
