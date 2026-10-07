# NM-HRM-029 — Figure 5-8's `dmasen` note does not hold outside its own exception [withdrawn]

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | Figure 5-8's note on p. 166 says `dmasen` is true only when LENFIN=1, an implication that absence of later `dmasen` pulses satisfies; the assertion went red because it searched for a sample where both were true |
| **What is asked of a reader** | Nothing. "Only when" is satisfied by a `dmasen` that never recurs. |
| **Withdrawal reason** | The note is the one-way implication `dmasen → LENFIN`; the assertion graded its converse, so the red reported the opposite of what the figure says |
| **Error class** | **a one-way implication tested as a biconditional** |
| **Replacement** | Green: `5-p166.c7` in [`hrm/ch5-audio-channel-state`](../../../hrm/ch5-audio-channel-state), which tests `dmasen → lenfin` and passes vacuously (0 samples outside the `000→001` exception carry `dmasen`); `5-p165.c9`, a direct capture of `dmasen` high at the `000→001` arc |
| **Module** | `paula_audio_channel.v` — [`:276`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L276), [`:220`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L220); MiSTer has both statements at the same lines, TC64 at lines 253 and 197; inherited |
| **Test** | `make -C hrm/ch5-audio-channel-state test`; the package prints `31 passed, 12 failed`, the twelve reds being other assertions, and an expected red exits nonzero |
| **Chapter** | 5 — Audio, printed p. 166 (Figure 5-8, boxed NOTE) and p. 165 (the `dmasen` signal definition) · *Amiga Hardware Reference Manual*, 3rd ed., 1991. Claim ids: `5-p166.c7` carries `5-p166.c7` and `F58.note.dmasen`, p. 166; `5-p166.c8` carries `5-p166.c8` and `F58.note.dsr`, p. 166; `5-p165.c9` carries `5-p165.c9` and `5-sig.dmasen`, p. 165 |
| **Assertions** | `5-p166.c7` green, vacuous: 0 samples outside the `000→001` exception carry `dmasen`; `5-p165.c9` green: `dmasen` high at the `000→001` arc; `5-p166.c8` informational `MEASURED`, not asserted: 3 `dmas` rising edges, 2 did not match AUDxDR · dmasen |
| **Verdict of the four** | none — `dmasen` is an internal signal that appears at no pin, so the four cannot be compared |

---

## Why it was withdrawn

Figure 5-8's NOTE says, after its exception:

> "dmasen is true only when LENFIN=1"

The excepted case is the `000→001` transition, whose action bracket is
`[lencntrld, AUDxDR, dmasen]`. Outside it the note is the implication
`dmasen → LENFIN`, falsified by exactly one thing: a sample where `dmasen` is
true and `LENFIN` is not.

**The assertion searched for the opposite** — at least one later sample where
*both* were true — and went red when none was found:

```cpp
if (s.lenfin && s.dmasen) dmasen_true_elsewhere_at_lenfin = true;
...
CHECK("... the note's remaining claim requires 'yes'", dmasen_true_elsewhere_at_lenfin);
```

**Absence of later `dmasen` pulses satisfies "only when". It does not violate
it.**

---

## What replaced it

The test evaluates the implication `dmasen → lenfin`, and it passes
vacuously: no sample outside the `000→001` exception carries `dmasen`, and the
package states that none can, because `dmasen` is high only while the state is
`000` and each sample is taken after the edge that leaves it (a control trace
counts 0 of 24 samples, the exception included). `dmasen` is observed by a
direct capture at the `000→001` arc, where it is high. The companion formula
`AUDxDSR = AUDxDR · dmasen` is an informational `MEASURED` note.

## What the manual prints, and what the RTL does

Figure 5-8's boxed NOTE, printed p. 166:

> "Except for this case, dmasen is true only when LENFIN=1. Also,
> AUDxDSR=AUDxDR · dmasen"

`dmasen` is raised only in the idle-state DMA branch ([`paula_audio_channel.v:276`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L276)); the block-end
restart comes from [`:220`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L220), `dmas <= dmasen | lenfin`. The implemented relation is
therefore

    AUDxDSR = AUDxDR ∧ (dmasen ∨ lenfin)

— the `lenfin` term sits **one register level further out** than the figure puts
it, computing the same output. The `AUDxDSR = AUDxDR · dmasen` equation compares
the public `dmas` port against **two internal signals** reached through
`--public-flat-rw`, and holds at the first restart and at neither later,
`lenfin`-driven one. It asks whether NanoMig arranges two terms the way the
figure draws them, not whether the chip behaves as the manual requires.

---

## Risks and unknowns

**Condition for filing again.** A sample outside the `000→001` exception in which
`dmasen` is true and `LENFIN` is not. The periodic trace cannot show one, so the
green on `5-p166.c7` records an implication no sample can trigger: it has not
been tested, and the assertion's own text says so. Whether the printed placement
of the `lenfin` term is normative is a question about the figure, not about the
design.
