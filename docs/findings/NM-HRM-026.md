# NM-HRM-026 — the audio state codes are not the ones Figure 5-8 prints

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a source observation, not test-backed |
| **What is asked of a reader** | Nothing yet. This was read from the source rather than measured by a graded assertion, and needs a test before it can ask for anything. |
| **Module** | `paula_audio_channel.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L242-L246) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L242-L246) |
| **Report to** | **nowhere, as it stands** — see *Suggested change* |
| **Test** | [`hrm/ch5-audio-channel-state`](../../hrm/ch5-audio-channel-state) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **all four use the figure's own numbering**, `101` included |
| **Boundary** | **inside the module.** All eight red assertions read `audio_state` through `--public-flat-rw` and do not exist at the pins |

---

## Why this is a source observation and not a defect

**Nothing outside the chip can observe `sq2,1,0`.** All eight of this entry's red
assertions read `audio_state` directly; without Verilator's `--public-flat-rw`
they would not compile. Reformulated at the ports they **pass**: `sample`,
`dmareq`, `dmas` and `intreq` carry the figure's actions, ordering and timing
exactly as printed.

A behavioural implementation is free to re-encode an internal state machine
unless state-identical reconstruction is an explicit requirement, and this
project has no such requirement. So the disagreement is confined to which bit
pattern happens to be stored, and the entry is a note for whoever reads Figure
5-8 as a specification — not a defect at the pins.

It is kept because the figure is the only place chapter 5 prints a state encoding
at all, and because four independent implementations read those patterns as
specification.

---

## What the HRM says

**Figure 5-8, printed p. 166** labels its states `000`, `001`, `101`, `010`,
`011`, and names the flip-flops in its own legend:

> `sq2,1,0` — "The name of the state flip-flops, MSB to LSB"

So the printed patterns are **bit values**, not merely node names. That legend is
what makes this readable as a claim at all.

---

## What NanoMig does

[`src/minimig-aga/paula_audio_channel.v#L242-L246`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L242-L246)

```verilog
parameter AUDIO_STATE_0 = 3'b000;
parameter AUDIO_STATE_1 = 3'b001;
parameter AUDIO_STATE_2 = 3'b011;
parameter AUDIO_STATE_3 = 3'b010;
parameter AUDIO_STATE_4 = 3'b110;
```

| | figure | NanoMig |
|---|---|---|
| second DMA state | `101` | `011` |
| lower-byte output | `011` | `110` |

### Measured

An 80 000-tick sweep reaches `000`, `001`, `010`, `011`, `110` and **never**
`100`, `101` or `111`. The figure's `101` is unreachable; `110`, which the figure
does not print, is live. "In the 011 state the lower byte is output" measures
`sample = 0x0` there, and the `010→011→010` loop measures its destination as `6`.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | `audio.cpp` | states 0, 1, 2, 3 |
| **vAmiga** | `StateMachine.cpp` | `0b000`, `0b001`, `0b010`, `0b101` |
| **Denise** | `emulation/libami/paula/audio.cpp` | states 0, 1, 2, 3 and **5** |
| **Copperline** | [`src/chipset/paula.rs:107-111`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L107-L111) | <pre>const AUD_IDLE: u8 = 0b000;<br>const AUD_DMA_FIRST: u8 = 0b001;<br>const AUD_DMA_SECOND: u8 = 0b101;<br>const AUD_OUT_HI: u8 = 0b010;<br>const AUD_OUT_LO: u8 = 0b011;</pre> |
| **MiSTer** | [`rtl/paula_audio_channel.v:242-246`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L242-L246) | descendant — byte-identical to NanoMig; the whole file matches, confirmed by a full-file comparison. The same encoding mismatch against Figure 5-8 |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:219-223`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L219-L223) | descendant — the same five `parameter AUDIO_STATE_*` lines verbatim, though the file around them differs: TC64 lacks the ~29-line PWM volume-gating block (see `NM-HRM-025`) that sits between the audio-state-machine's neighbours in NanoMig, which is why the line numbers shift from NanoMig's 242-246 to TC64's 219-223. The encoding mismatch against Figure 5-8 is inherited by both lines equally |

**All four independent implementations use the figure's numbering, including
`101`.** Copperline writes the five constants out with the manual's bit patterns
*and* the manual's meanings. That is what lifts this above an internal-encoding
quibble: four independent implementations treat the printed patterns as the
specification. MiSTer and TC64 do not — they carry NanoMig's own mismatched
encoding unchanged, which is the source observation propagating, not a second
reading of the figure.

---

## Suggested change

Renumbering two parameters would match the figure and change no behaviour:

```verilog
parameter AUDIO_STATE_2 = 3'b101;   // was 3'b011 -- Figure 5-8's second DMA state
parameter AUDIO_STATE_4 = 3'b011;   // was 3'b110 -- Figure 5-8's lower-byte output
```

**But there is no reason to, and filing it upstream would be noise.** The
encoding is unobservable, the behaviour is already correct, and a change to
working RTL that no pin can detect carries risk without benefit. The
one place it would matter is a rewrite that uses Figure 5-8 as its
specification — which is what this entry exists to warn.

---

## What holds it up

- **The behaviour is right and only the encoding is not.** Every affected
  assertion is otherwise green: correct actions, correct timing, correct
  ordering. They fail on the bit pattern alone, and that distinction *is* the
  finding.
- The package's reachability proof is not a tautology. Reachable states are
  proven by legal stimulus; the unused codes `100`, `101`, `111` are force-seeded
  and clocked through the real next-state logic; the two sets are asserted
  disjoint and their union asserted to be all eight codes — because `5 + 3 = 8`
  is satisfied by a coincidence.
