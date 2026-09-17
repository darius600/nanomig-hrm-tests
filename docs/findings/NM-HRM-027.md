# NM-HRM-027 — in direct mode the interrupt is raised before a word is output

| | |
|---|---|
| **Status** | open · red |
| **Module** | `paula_audio_channel.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L283-L285) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L283-L285) |
| **Upstream** | **MiSTer** `rtl/paula_audio_channel.v:283-285` and `:133` — byte-identical to NanoMig, same line numbers. **TC64** `rtl/minimig/paula_audio_channel.v:260-262` — the same idle-exit branch, `audio_next = AUDIO_STATE_3; AUDxDR = 1'b0; AUDxIR = 1'b1;`, and `:127` carries the same `assign intreq = AUDxIR;`. The interrupt-before-output timing this entry measures is inherited by both lines; a fix belongs upstream of all three |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's, and a fix there reaches NanoMig |
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
`sample` are both top-level ports of `paula_audio_channel`; unlike `NM-HRM-026`
in the same chapter, nothing here depends on reading inside the module.

---

## What the others say

| WinUAE | vAmiga | Denise | Copperline | MiSTer | TC64 |
|---|---|---|---|---|---|
| — | — | — | — | descendant — [`rtl/paula_audio_channel.v:283-285`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L283-L285) and [`:133`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L133): byte-identical to NanoMig, same line numbers | descendant — [`rtl/minimig/paula_audio_channel.v:260-262`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L260-L262) and [`:127`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L127): the same idle-exit branch, `audio_next = AUDIO_STATE_3; AUDxDR = 1'b0; AUDxIR = 1'b1;`, and the same `assign intreq = AUDxIR;` |

Nobody has read any of the four independent implementations for this finding. It
is the one chapter-5 entry with no third-party reading among the four at all, and
it is the one where such a reading would be most useful — see below. MiSTer and
TC64 are not that reading: both inherit NanoMig's interrupt-before-output timing
unchanged, so they confirm the ancestry rather than add a witness.

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
