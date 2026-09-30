# NM-HRM-026 — the audio state codes are not the ones Figure 5-8 prints

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | seen in the RTL source and measured inside the module; no assertion at the ports fails |
| **What is asked of a reader** | Nothing. This entry's eight red assertions read the internal state register `audio_state` through Verilator's `--public-flat-rw`; the package's two reds at the module's ports are [NM-HRM-084](NM-HRM-084.md)'s length count and pass on a copy of the RTL that changes that count and keeps these codes, so no pin shows the difference. The entry is a note for whoever reads Figure 5-8 as a specification of the state encoding. |
| **Module** | `paula_audio_channel.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L242-L246) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L242-L246) |
| **Report to** | **nowhere, as it stands** — see *Suggested change* |
| **Test** | [`hrm/ch5-audio-channel-state`](../../hrm/ch5-audio-channel-state) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 8 red, all reading `audio_state` inside the module: `5-p164.c4`, `5-p166.c1`, `5-p164.c18`, `5-p164.c9`, `5-p164.c10`, `5-p166.c6`, `5-sm.c8`, `5-p166.c3` (31 passed, 12 failed; the other four reds are [NM-HRM-084](NM-HRM-084.md)'s: `F58-LENCOUNT-STARTUP-ARCS` and `5-p166.c4 F58.t101_010` grade the length count on the 101 → 010 arc inside the module, `5-p164.c23 5-p164.c24 5-sm.c25` and `5-p164.c28 5-sm.c29` when the first pass's restart and interrupt requests leave it) |
| **Verdict of the four** | **all four use the figure's own numbering**, `101` included |
| **Boundary** | **inside the module.** This entry's eight red assertions read `audio_state` through `--public-flat-rw` and do not exist at the pins |
| **See also** | [NM-HRM-084](NM-HRM-084.md) — the same state machine's 101 → 010 arc, where NanoMig counts the length down and the figure prints no lencount: a difference in behaviour, seen at the pins, which a renumbering of the states leaves as it is |

---

## Why this is a source observation and not a defect

**Nothing outside the chip can observe `sq2,1,0`.** All eight of this entry's red
assertions read `audio_state` directly; without Verilator's `--public-flat-rw`
they would not compile. Re-pointed at the RTL's own state codes they **pass**, and
the package's assertions at the ports, on `sample`, `dmareq`, `dmas` and
`intreq`, are green but for two, on when the first pass's restart and interrupt
requests go out, which are the length count below. One departure at a port,
`dmas` computed one register stage later than the NOTE's
`AUDxDSR=AUDxDR·dmasen`, is recorded there, not graded.

**The actions on the arcs are a different question, and on the 101 → 010 arc
NanoMig's depart from the figure's.** In the state NanoMig codes `3'b011`, the
figure's 101, the word that arrives after the thrown-away one counts the length
down,
[`src/minimig-aga/paula_audio_channel.v#L358`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L358), `lencount = ~lenfin;`,
where the figure's bracket on that arc, read from the drawing, is
[volcntrld, percntrld, pbufld1, AUDxDR if napnav] and prints no lencount. The
count reaches the pins: after a DMA start the first pass is one word short of
AUDxLEN from 4 up, and at AUDxLEN = 3 runs past the data set. That is
[NM-HRM-084](NM-HRM-084.md), and a renumbering of the states does not touch it.
This package's assertions at the ports see it too: with AUDxLEN = 4, no restart
on `dmas` comes between the arrivals of offsets 2 and 3, and no interrupt on
`intreq` from offset 3's arrival to the next pass's first word
(`test_ch5_audio_channel_state.cpp:1396-1435`, `:1476-1514`). Both are red, and
on a copy of the RTL with line 358 changed to `lencount = 1'b0;`, the state
codes left as they are, both are green. Its two assertions that grade the
arc's count read the length counter and the arc's action signals inside the
module and are red as well. All four are NM-HRM-084's.

A behavioural implementation is free to re-encode an internal state machine
unless state-identical reconstruction is an explicit requirement, and this
project has no such requirement. So the disagreement this entry records is
confined to which bit pattern happens to be stored, and the entry is a note for
whoever reads Figure 5-8 as a specification of the encoding — not a defect at
the pins.

It is kept because the figure, with the p. 164 prose that walks through it, is the
only place chapter 5 prints a state encoding, and because four independent
implementations read those patterns as specification.

---

## What the HRM says

**Figure 5-8, printed p. 166** labels its working states `000`, `001`, `101`,
`010`, `011`, draws `100`, `110` and `111` as unused states that return to `000`,
and names the flip-flops in its own legend:

> `sq2,1,0` — "The name of the state flip-flops, MSB to LSB"

So the printed patterns are **bit values**, not merely node names. That legend is
what makes this readable as a claim at all.

---

## What NanoMig does

[`src/minimig-aga/paula_audio_channel.v#L242-L246`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L242-L246)

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

The package's legal-stimulus runs, one DMA-driven and one direct, reach `000`,
`001`, `010`, `011`, `110` and **never** `100`, `101` or `111`. The figure's `101`
is unreachable; `110`, which the figure draws as unused, is live. "In the 011
state the lower byte is output" measures `sample = 0x0` there, and the `010→011→010` loop measures its destination as `6`.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:1872`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1872) | states 0, 1, **5**, 2, 3 — `cdp->state = 5;` follows the first DMA word, as `101` does in the figure |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachine.cpp:253`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L253) | `0b000`, `0b001`, `0b101`, `0b010`, `0b011` — `state = 0b101;` in `move_001_101()` |
| **Denise** | `emulation/libami/paula/audio.cpp` | states 0, 1, 2, 3 and **5** |
| **Copperline** | [`src/chipset/paula.rs:107-111`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L107-L111) | <pre>const AUD_IDLE: u8 = 0b000;<br>const AUD_DMA_FIRST: u8 = 0b001;<br>const AUD_DMA_SECOND: u8 = 0b101;<br>const AUD_OUT_HI: u8 = 0b010;<br>const AUD_OUT_LO: u8 = 0b011;</pre> |
| **MiSTer** | [`rtl/paula_audio_channel.v:242-246`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L242-L246) | descendant — byte-identical to NanoMig; the whole file matches, confirmed by a full-file comparison. The same encoding mismatch against Figure 5-8 |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:219-223`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L219-L223) | descendant — the same five `parameter AUDIO_STATE_*` lines verbatim, though the file around them differs: TC64 lacks the PWM volume-gating block (NanoMig `:136-150`, see [NM-HRM-025](NM-HRM-025.md)) together with its declarations and the `sample_okk` output, 23 lines in all before this point, which is why the line numbers shift from NanoMig's 242-246 to TC64's 219-223. The encoding mismatch against Figure 5-8 is inherited by both lines equally |

**All four independent implementations use the figure's numbering, including
`101`.** Copperline writes the five constants out with the manual's bit patterns
*and* the manual's meanings. That is what lifts this above an internal-encoding
quibble: four independent implementations treat the printed patterns as the
specification. MiSTer and TC64 do not — they carry NanoMig's own mismatched
encoding unchanged, which is the source observation propagating, not a second
reading of the figure.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

---

## Suggested change

Renumbering two parameters would match the figure's codes and change no behaviour:

```verilog
parameter AUDIO_STATE_2 = 3'b101;   // was 3'b011 -- Figure 5-8's second DMA state
parameter AUDIO_STATE_4 = 3'b011;   // was 3'b110 -- Figure 5-8's lower-byte output
```

**But there is no reason to, and filing it upstream would be noise.** The
encoding is unobservable, the renumbering changes no behaviour, and a change to
working RTL that no pin can detect carries risk without benefit. The
one place it would matter is a rewrite that uses Figure 5-8 as its
specification — which is what this entry exists to warn. The length count on
the 101 → 010 arc is behaviour, not encoding, and its change is
[NM-HRM-084](NM-HRM-084.md)'s.

---

## What holds it up

- **The eight reds fail on the bit pattern alone.** Re-pointed at the RTL's
  own codes each holds: the actions, timing and ordering they grade besides
  the code are what NanoMig does, and that distinction *is* the finding. The
  package's four other reds, on the length count of the 101 → 010 arc and on
  when the first pass's restart and interrupt requests go out, are behaviour
  and are [NM-HRM-084](NM-HRM-084.md)'s. On a copy of the RTL with line 358
  changed to the figure's bracket the package reads `35 passed, 8 failed`:
  those four turn green and these eight stay red.
- The package's reachability proof is not a tautology. Reachable states are
  proven by legal stimulus; the unused codes `100`, `101`, `111` are force-seeded
  and clocked through the real next-state logic; the two sets are asserted
  disjoint and their union asserted to be all eight codes — because `5 + 3 = 8`
  is satisfied by a coincidence.
