# NM-HRM-027 — in direct mode the interrupt is raised before a word is output

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_audio_channel.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L283-L285) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L283-L285) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch5-audio-direct-output`](../../hrm/ch5-audio-direct-output) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | not read — and the manual states the moment four different ways |
| **Boundary** | at the pins — `intreq` and `sample` are both top-level ports |

---

## What the HRM says

**Printed p. 157**, on direct (non-DMA, CPU-driven) audio output:

> "the interrupt occurs after one data word has been output"

---

## What NanoMig does

The branch leaving the idle state on a CPU write to `AUDxDAT` raises the
interrupt request in the same step, before any byte has left the channel:

[`src/minimig-aga/paula_audio_channel.v#L283-L285`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L283-L285)

```verilog
else if (AUDxDAT && !AUDxON && !AUDxIP) begin
  audio_next = AUDIO_STATE_3;
  AUDxDR = 1'b0;
  AUDxIR = 1'b1;
```

and `AUDxIR` is the interrupt port itself:

[`src/minimig-aga/paula_audio_channel.v#L133`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L133)

```verilog
assign intreq = AUDxIR;
```

### Measured

| event | tick |
|---|---|
| `intreq` pulses | **80** — coincident with the CPU write to `AUDxDAT` |
| the word's high byte reaches `sample` | 92 |
| its low byte reaches `sample` | 136 |

The interrupt is 12 ticks ahead of the first byte and 56 ahead of the second.
The manual asks for it after the word.

**This finding is observable entirely at the module's pins.** `intreq` and
`sample` are both top-level ports of `paula_audio_channel`; unlike [NM-HRM-026](NM-HRM-026.md)
in the same chapter, nothing here depends on reading inside the module.

---

## What the others say

| WinUAE | vAmiga | Denise | Copperline | MiSTer | TC64 |
|---|---|---|---|---|---|
| — | — | — | — | descendant — [`rtl/paula_audio_channel.v:283-285`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L283-L285) and [`:133`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L133): byte-identical to NanoMig, same line numbers | descendant — [`rtl/minimig/paula_audio_channel.v:260-262`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L260-L262) and [`:127`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L127): the same idle-exit branch, `audio_next = AUDIO_STATE_3; AUDxDR = 1'b0; AUDxIR = 1'b1;`, and the same `assign intreq = AUDxIR;` |

**The timing this entry measures is inherited by both Minimig lines**, so a fix
belongs upstream of all three rather than in NanoMig alone.

Nobody has read any of the four independent implementations for this finding. It
is the one chapter-5 entry with no third-party reading among the four at all, and
it is the one where such a reading would be most useful — see below. MiSTer and
TC64 are not that reading: both inherit NanoMig's interrupt-before-output timing
unchanged, so they confirm the ancestry rather than add a witness.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore-Amiga, *Amiga Hardware Manual*, 1985 — the oracle's first edition**, PDF page 158 — agrees with the manual:

  > "In direct audio output, the interrupt occurs after one data word has been output."
  <!-- corpus: hw-manual-1985/p158.txt -->

  Section 5.5 (printed 5-28) carries the 3rd edition's p. 157 sentence. For the finding's four-way table: the 1985 interrupt chapter (p193) phrases the moment a fifth way, 'In manual mode, it occurs when the audio data register is ready to accept another word of data.', and the same sentence is in the 3rd-edition corpus (amiga-hrm-3rd/text/p220.txt).

- **Commodore-Amiga, *Amiga Hardware Manual* errata, October 1985**, PDF page 26 — agrees with the manual:

  > "In interrupt-driven operation, transfer to the main loop (010,011 states) is immediate upon data written by the processor. In the 010 state the upper byte is output, and in the 011 state the lower byte is output. Transitions 010-011-010 ... occur whenever the period counter coun ts down to one. The period counter is reloaded at these transitions. As long as the in terrupt is cleared by the processor in time, the machine remains in the main loop. Otherwise, it enters the idle state. Inter- rupts are generated on every word (011-010) transition."
  <!-- corpus: hw-manual-errata-1985/p026.txt -->

  The October 1985 state-machine addendum (the paragraph the 3rd edition carries on p. 164) puts the interrupts of interrupt-driven operation on the 011-010 word transitions, after the upper (010) and lower (011) byte have been output, and mentions no request on leaving idle, where NanoMig raises it. The state diagram that would show the idle-exit actions (p029) has no usable text layer, so it cannot be checked here.

Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; US patent 4,829,473, Commodore-Amiga; 6502.org dissection of Paula 8364R7.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

---

## The manual gives four answers, and that is the real problem

The audio interrupt's moment is stated four different ways:

| printed page | the moment |
|---|---|
| 147 | after the location and length are copied to the back-up registers |
| **157** | **after a data word has been output** |
| 164 | as the last word *starts* output |
| appendix A, 265 | when words-output equals length **and** the buffered word has been used |

The classification carries all four. The assertion tests page 157's, because
page 157 is the direct-mode page and this is the direct-mode case.

---

## Suggested change

Defer the request by one word: leave the idle-exit branch as it is but raise
`AUDxIR` where the word is consumed rather than where it arrives — the point the
state machine already reaches once both bytes have been output.

**Do not act on this without reading the four-way conflict above first.** A
manual that gives four answers is not obviously settled by picking one of them,
and a change that satisfies p. 157 may break p. 164. The cheapest next step is
not a patch but a reading of what WinUAE, vAmiga, Denise or Copperline actually
do here, because four implementations agreeing would settle which sentence the
hardware follows.

---

## What holds the finding up

Assertion H — "immediate transfer to the main loop", from the same page and
measured on the same trace event — is **green** and is the control. The trace is
being read correctly; it is the interrupt's position in it that fails.

---

## For the specialists

Read this together with the four-way conflict before deciding anything. If real
Paula raises the request at the write, page 157 is the defect and this entry
becomes a source observation rather than a conformance finding.
