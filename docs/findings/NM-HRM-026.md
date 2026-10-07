# NM-HRM-026 — the audio state codes are not the ones Figure 5-8 prints

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | seen in the RTL source and measured inside the module; no assertion at the ports fails |
| **What is asked of a reader** | Does Figure 5-8's numbering (second DMA state `101`, lower-byte output `011`) bind the encoding of `audio_state` in `paula_audio_channel.v`, where NanoMig codes them `3'b011` and `3'b110`? This page's answer is no, because no pin shows the encoding; nothing is asked unless a specialist answers yes. Closes when an Amiga specialist records that the figure's patterns are not a requirement on the encoding, or that they are, in which case the two-line renumbering under *Decision* is the change. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer, whose `paula_audio_channel.v` is byte-identical at these lines; the page does not recommend filing it unless the answer above is yes |
| **Scope** | unknown: the figure names no chip revision; measured on NanoMig's `paula_audio_channel` at the pinned revision, in the package's two legal-stimulus runs (one DMA-driven, one direct), reading `audio_state` inside the module |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 5 — Audio, printed p. 164 (the state prose) and p. 166 (Figure 5-8) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · the package prints claim ids without an `opus:` / `sol:` prefix; each of the eight reds carries one: `5-p164.c4 5-sm.c4` (p. 164), `5-p166.c1 F58.unused` (p. 166), `5-p164.c18 5-sm.c20` (p. 164), `5-p164.c9 5-sm.c11` (p. 164), `5-p164.c10 5-sm.c12` (p. 164), `5-p166.c6 F58.loop.percount` (p. 166), `5-sm.c8` (p. 164), `5-p166.c3 F58.t001_101` (p. 166); [NM-HRM-084](NM-HRM-084.md)'s four: `F58-LENCOUNT-STARTUP-ARCS`, `5-p166.c4 F58.t101_010` (p. 166), `5-p164.c23 5-p164.c24 5-sm.c25` and `5-p164.c28 5-sm.c29` (p. 164) |
| **Module** | `paula_audio_channel.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L242-L246) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L242-L246); inherited |
| **Test** | [`hrm/ch5-audio-channel-state`](../../hrm/ch5-audio-channel-state): `make -C hrm/ch5-audio-channel-state test`; the expected reds exit nonzero |
| **Assertions** | 8 red of 43 graded (31 passed, 12 failed), all reading `audio_state` inside the module: `5-p164.c4` and `5-p166.c1`, expected 100, 110 and 111 all return to 000, observed 110 is the RTL's live loop state, `AUDIO_STATE_4`; `5-p164.c18`, expected discard state 101, observed 3; `5-p164.c9`, expected the lower byte (0x44) output in state 011, observed `sample = 0x0`; `5-p164.c10`, expected the `010 → 011 → 010` loop, observed destination 6; `5-p166.c6`, expected `percount` self-loops in 010 and 011, observed no self-loop in 011 (the steady low-byte code measures 6); `5-sm.c8`, expected main loop 010 and 011, observed 010 and 6; `5-p166.c3`, expected the 001 exit to reach 101, observed 3. The other four reds are [NM-HRM-084](NM-HRM-084.md)'s |
| **Verdict of the four** | **unanimous: all four use the figure's numbering**, `101` included; hardware status not measured |
| **Effort** | none: no RTL change is asked; were the answer yes, the renumbering is two `parameter` lines in `paula_audio_channel.v` and changes no behaviour |
| **See also** | [NM-HRM-084](NM-HRM-084.md) — the same state machine's 101 → 010 arc, where NanoMig counts the length down and the figure prints no lencount: a difference in behaviour, seen at the pins, which a renumbering leaves as it is |

---

## Source requirement

**Figure 5-8, printed p. 166** labels its working states `000`, `001`, `101`,
`010`, `011`, draws `100`, `110` and `111` as unused states that return to `000`,
and names the flip-flops in its own legend:

> `sq2,1,0` — "The name of the state flip-flops, MSB to LSB"

So the printed patterns are bit values, not merely node names. The figure, with
the p. 164 prose that walks through it, is the only place chapter 5 prints a
state encoding.

## RTL and measurement

[`paula_audio_channel.v#L242-L246`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L242-L246):

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

The package's legal-stimulus runs reach `000`, `001`, `010`, `011`, `110` and
**never** `100`, `101` or `111`; the unused codes were force-seeded and clocked
through the real next-state logic. The figure's `101` is unreachable; `110`, which
the figure draws as unused, is live. State 011 measures `sample = 0x0`, and the
`010 → 011 → 010` loop measures its destination as `6`. Graded against the RTL's
own codes instead, the eight pass.

The assertions at the ports, on `sample`, `dmareq`, `dmas` and `intreq`, are
green but for two, which are [NM-HRM-084](NM-HRM-084.md)'s: in the state NanoMig
codes `3'b011`, the figure's 101, the word after the thrown-away one counts the
length down,
[`paula_audio_channel.v#L358`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L358),
`lencount = ~lenfin;`, where the figure's bracket on that arc prints no lencount.
With AUDxLEN = 4, no restart on `dmas` comes between the arrivals of offsets 2 and 3,
and no interrupt on `intreq` from offset 3's arrival to the next pass's first word
(`test_ch5_audio_channel_state.cpp:1396-1435`, `:1476-1514`). With line 358
changed to `lencount = 1'b0;` and the codes left as they are, both are green; with
it changed to the figure's bracket the package reads `35 passed, 8 failed`, and
these eight stay red.

## Risks and unknowns

- **Nothing outside the chip observes `sq2,1,0`.** The eight reds read
  `audio_state` through Verilator's `--public-flat-rw` and do not exist at the pins.
- **A re-encoding is allowed unless state-identical reconstruction is required**,
  and this project has no such requirement; the disagreement is confined to which
  bit pattern is stored.
- **The figure's bracket on the 101 → 010 arc is read from the drawing, not
  quoted.**
- **One departure at a port is recorded, not graded:** `dmas` is computed one
  register stage later than the NOTE's `AUDxDSR=AUDxDR·dmasen`.
- No chip measurement of the original hardware exists on this page.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:1872`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1872) | states 0, 1, **5**, 2, 3 — `cdp->state = 5;` follows the first DMA word, as `101` does in the figure |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachine.cpp:253`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L253) | `0b000`, `0b001`, `0b101`, `0b010`, `0b011` — `state = 0b101;` in `move_001_101()` |
| **Denise** | `emulation/libami/paula/audio.cpp` | states 0, 1, 2, 3 and **5** |
| **Copperline** | [`src/chipset/paula.rs:107-111`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L107-L111) | <pre>const AUD_IDLE: u8 = 0b000;<br>const AUD_DMA_FIRST: u8 = 0b001;<br>const AUD_DMA_SECOND: u8 = 0b101;<br>const AUD_OUT_HI: u8 = 0b010;<br>const AUD_OUT_LO: u8 = 0b011;</pre> |
| **MiSTer** | [`rtl/paula_audio_channel.v:242-246`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L242-L246) | descendant — byte-identical to NanoMig; the whole file matches, confirmed by a full-file comparison. The same encoding mismatch against Figure 5-8 |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:219-223`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L219-L223) | descendant — the same five `parameter AUDIO_STATE_*` lines verbatim, though the file around them differs and the lines sit 23 lines earlier than NanoMig's. The mismatch is inherited |

**All four independent implementations use the figure's numbering, including
`101`.** Copperline writes the five constants out with the manual's bit patterns
*and* the manual's meanings, which lifts this above an internal-encoding quibble:
four independent implementations treat the printed patterns as the specification.
MiSTer and TC64 carry NanoMig's own mismatched encoding unchanged, which is the
source observation propagating, not a second reading of the figure.

## Decision

Renumbering two parameters would match the figure's codes and change no behaviour:

```verilog
parameter AUDIO_STATE_2 = 3'b101;   // was 3'b011 -- Figure 5-8's second DMA state
parameter AUDIO_STATE_4 = 3'b011;   // was 3'b110 -- Figure 5-8's lower-byte output
```

**But there is no reason to, and filing it upstream would be noise.** The
encoding is unobservable, the renumbering changes no behaviour, and a change to
working RTL that no pin can detect carries risk without benefit. It would matter
for a rewrite that uses Figure 5-8 as its specification, which is what this entry
exists to warn. The length count on the 101 → 010 arc is behaviour, not encoding,
and its change is [NM-HRM-084](NM-HRM-084.md)'s. The entry closes as the header's
fourth row states.
