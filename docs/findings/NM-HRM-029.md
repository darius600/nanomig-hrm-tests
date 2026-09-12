# NM-HRM-029 — Figure 5-8's `dmasen` note does not hold outside its own exception

| | |
|---|---|
| **Status** | **WITHDRAWN** — the assertion was logically false |
| **Module** | `paula_audio_channel.v` |
| **Test** | [`hrm/ch5-audio-channel-state`](../../hrm/ch5-audio-channel-state) — the assertion is gone |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed. |
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

## Why this entry is kept

Four other withdrawals in this register read a **permission or a silence** as a
requirement. This is the same error **in the direction nobody was watching for**:
a one-way implication tested both ways.

It is the first of its kind here, and it is the reason the playbook now says:
write the claim as a formula before writing the predicate.
