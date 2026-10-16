# NM-HRM-084 — the length counter also counts the first data word after audio DMA starts: the first pass is one word short of AUDxLEN, and at AUDxLEN = 3 runs two words past the data set [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `paula_audio_channel.v` stop counting the length down on the 101 → 010 arc (line 358), so that the first pass after a DMA start fetches AUDxLEN words? Closes when line 358 no longer counts and `FIRST-PASS-LEN-3`, `FIRST-PASS-LEN-4`, `FIRST-PASS-LEN-5`, `FIRST-PASS-LEN-7`, `FIRST-PASS-LEN-8`, `FIRST-PASS-LEN-16`, `NO-FETCH-BEYOND-DATA-SET-LEN-3`, `AUDXLEN-BLOCK-1-INCLUDES-DISCARD`, `INTERRUPT-CADENCE-BLOCK-1-INCLUDES-DISCARD`, `F58-LENCOUNT-STARTUP-ARCS`, `5-p166.c4 F58.t101_010`, `5-p164.c23 5-p164.c24 5-sm.c25` and `5-p164.c28 5-sm.c29` of the three `hrm/` packages and `P8-RESTART` of `integration/ch2-copper-move` turn green, or when an Amiga specialist rejects the change; if that means p. 164 and Figure 5-8 are wrong, this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | NanoMig's `paula_audio_channel` on the first pass after audio DMA is enabled, AUDxLEN of 3 and more (at AUDxLEN = 2 it agrees with Figure 5-8); Amiga chip revision unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 5 — Audio, printed pp. 139, 145–147, 164–166 (Figure 5-8); Appendix A, printed pp. 265–266; Appendix B, printed p. 288 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (40)</summary> claim ids by assertion: `FIRST-PASS-LEN-3`, `FIRST-PASS-LEN-4`, `FIRST-PASS-LEN-5`, `FIRST-PASS-LEN-7`, `FIRST-PASS-LEN-8`, `FIRST-PASS-LEN-16` and the green `FIRST-PASS-LEN-2`, each: `5-sum.c3`, p. 145; `5-p139.c9`, `5-len.c1`, p. 139; `NO-FETCH-BEYOND-DATA-SET-LEN-3` and the green `LATER-PASSES-LEN-2`, `LATER-PASSES-LEN-3`, `LATER-PASSES-LEN-4`, `LATER-PASSES-LEN-5`, `LATER-PASSES-LEN-7`, `LATER-PASSES-LEN-8`, `LATER-PASSES-LEN-16`, each: `5-p139.c7`, `5-loc.c7`, p. 139; `5-p164.c23`, `5-p164.c25`, `5-sm.c26`, `5-p164.c26`, `5-sm.c27`, p. 164; `5-p147.c8`, `5-p147.c9`, `5-join.c6`, p. 147; `F58-LENCOUNT-STARTUP-ARCS`: `F58.t001_101`, `F58.t101_010`, p. 166; `5-p166.c4 F58.t101_010`: `5-p166.c4`, `F58.t101_010`, p. 166; `5-p164.c23 5-p164.c24 5-sm.c25`: `5-p164.c23`, `5-p164.c24`, `5-sm.c25`, p. 164; `5-p164.c28 5-sm.c29`: `5-p164.c28`, `5-sm.c29`, p. 164; the green `AUDXLEN-SETS-WORDS-PER-BLOCK`: `5-p139.c8`, `5-p139.c9`, `5-p139.c10`, `5-len.c1`, p. 139; `5-appA-p266.c18`, `A-audxlen.c1`, p. 266; `5-p147.c2`, `5-p147.c9`, `5-join.c6`, p. 147; the green `INTERRUPT-CADENCE-TIED-TO-LENGTH`: `5-appA-p265.c46`, `5-appA-p265.c47`, `A-audxdat.c8`, `A-audxdat.c9`, p. 265; the green `5-p164.c22 5-sm.c24 5-p165.c14`: `5-p164.c22`, `5-sm.c24`, p. 164; `5-p165.c14`, p. 165; `P8-RESTART`: `appB-note.c9`, `appB-note.c11`, p. 288. `AUDXLEN-BLOCK-1-INCLUDES-DISCARD` and `INTERRUPT-CADENCE-BLOCK-1-INCLUDES-DISCARD` carry no claim id</details> |
| **Module** | `paula_audio_channel.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L358) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L358); inherited (TC64 carries the same line at `rtl/minimig/paula_audio_channel.v:335`) |
| **Test** | `make -C hrm/ch5-audio-dma-first-pass test` — [`hrm/ch5-audio-dma-first-pass`](../../hrm/ch5-audio-dma-first-pass), the package's own wrapper around `paula_audio` and `agnus_audiodma`, graded at `agnus_audiodma`'s address port, no CPU core. Module packages, `paula_audio_channel` alone: `make -C hrm/ch5-audio-channel-registers test` — [`hrm/ch5-audio-channel-registers`](../../hrm/ch5-audio-channel-registers); `make -C hrm/ch5-audio-channel-state test` — [`hrm/ch5-audio-channel-state`](../../hrm/ch5-audio-channel-state). An expected red exits nonzero. [`integration/ch5-audio-output-scanrate`](../../integration/ch5-audio-output-scanrate): `make -k -C integration/ch5-audio-output-scanrate test` records the first pass on MEASURED lines naming this finding, not graded (table T1: `3 reads of 4 words before t_2`). [`integration/ch2-copper-move`](../../integration/ch2-copper-move): `make -k -C integration/ch2-copper-move test` (or `test-fx68k` and `test-tg68k`); an expected red exits nonzero |
| **Assertions** | 7 red in `hrm/ch5-audio-dma-first-pass` (15 passed, 7 failed): `FIRST-PASS-LEN-3`, `FIRST-PASS-LEN-4`, `FIRST-PASS-LEN-5`, `FIRST-PASS-LEN-7`, `FIRST-PASS-LEN-8`, `FIRST-PASS-LEN-16`, `NO-FETCH-BEYOND-DATA-SET-LEN-3`. 2 red in `hrm/ch5-audio-channel-registers` (19 passed, 2 failed): `AUDXLEN-BLOCK-1-INCLUDES-DISCARD`, `INTERRUPT-CADENCE-BLOCK-1-INCLUDES-DISCARD`. 4 red in `hrm/ch5-audio-channel-state` (31 passed, 12 failed; the other eight are [NM-HRM-026](NM-HRM-026.md)'s state codes): `F58-LENCOUNT-STARTUP-ARCS`, `5-p166.c4 F58.t101_010`, `5-p164.c23 5-p164.c24 5-sm.c25`, `5-p164.c28 5-sm.c29`. Expected: pass 1 is AUDxLEN words; observed: one word fewer from AUDxLEN = 4 up, and at AUDxLEN = 3 five words, offsets 0 to 4, the last two past the set. In the module packages, expected → observed: `AUDXLEN-BLOCK-1-INCLUDES-DISCARD`, 6 → 5 words from DMA start to the first reload (AUDxLEN = 5); `INTERRUPT-CADENCE-BLOCK-1-INCLUDES-DISCARD`, 5 → 4 words to the first `intreq2` rise (AUDxLEN = 4); `F58-LENCOUNT-STARTUP-ARCS`, `lencnt` 4 3 3 → 4 3 2 as the thrown-away word, offset 0 and offset 1 arrive; `5-p166.c4 F58.t101_010`, no `lencount` → `lencount=1` on the 101 → 010 arc; `5-p164.c23 5-p164.c24 5-sm.c25` and `5-p164.c28 5-sm.c29`, a restart request between offsets 2 and 3 and an interrupt request from offset 3 on → none. Green in the same runs: `FIRST-PASS-LEN-2`, the seven `LATER-PASSES-LEN-<n>`, `AUDXLEN-SETS-WORDS-PER-BLOCK`, `INTERRUPT-CADENCE-TIED-TO-LENGTH` and the main-loop `5-p164.c22 5-sm.c24 5-p165.c14`. In `integration/ch2-copper-move`, 1 red of 2 on each core (77 passed, 2 failed on fx68k; 85 passed, 2 failed on TG68K; the other red is `P5-LOCATION-R2`, [NM-HRM-073](NM-HRM-073.md)'s): `P8-RESTART`, expected `$44000 $44002 $44004 $44006` as the first run of AUD0LEN = 4, observed `$44000 $44002 $44004`, in both of its runs; `P8-CTRL`, which grades the runs after the first (a last run cut off by marker 3 is graded on the fetches it contains), is green |
| **Verdict of the four** | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline all count the thrown-away start-up word and not the word that takes the channel from 101 to 010, as Figure 5-8's brackets do, so their first pass is AUDxLEN words like every later one and never runs past the data set. Hardware: not measured on an Amiga |
| **Effort** | S — one line in `paula_audio_channel.v` (line 358, `AUDIO_STATE_2`) |
| **See also** | [NM-HRM-026](NM-HRM-026.md) — the same state machine's codes: NanoMig's `3'b011` is the figure's 101 |

---

## Source requirement

**The length register holds the number of words the channel retrieves before
it starts again at the location**, and no page limits that to later passes.
**Printed p. 145**, steps 2 and 4 of the procedure for a steady tone:

> 2. Create the data set containing the pairs of data samples (data words). Normally, a data set contains the definition of one waveform.

> 4. Set the length register, AUDxLEN, to the number of data words to be retrieved before starting at the address currently in AUDxLC.

The same count is given by printed p. 139 (twice), p. 266, p. 147, p. 288 and
p. 265, and by the chapter's worked example on p. 146, *read from the printed
page*: an eight-byte sine table, four words, with `MOVE.W #4,AUD0LEN(a0)`.

**Printed p. 164** walks through the start-up:

> In DMA-driven operation, transition to the 001 state occurs and DMA requests are sent to Agnus as soon as DMA is turned on. Because of pipelining in Agnus, the first data word must be thrown away. State 101 is entered as soon as this word arrives; a request for the next data word has already gone out. When the data arrives, state 010 is entered and the main loop continues until the DMA is turned off. The length counter counts down once with each word that comes in. When it finishes, a DMA restart request goes to Agnus along with the regular DMA request. This tells Agnus to reset the pointer to the beginning of the table of data. Also, the length counter is reloaded and an interrupt request goes out soon after the length counter finishes (counts to one). The request goes out just as the last word of the waveform starts its output.

and opens its section with *"For an explanation of the various states, refer
to Figure 5-8."* **Printed p. 165** names the signals:

> lencount Count length counter down one notch.
> lenfin Length counter finished (value = 1).

> AUDxDSR Audio DMA request to Agnus to reset pointer to start of block.
> dmasen Restart request enable.

**Figure 5-8, p. 166**, *read from the drawing, not quoted* (the legend:
brackets are "action on condition", parentheses "cause of state transition"):

| arc | cause | actions |
|---|---|---|
| 000 → 001 | (AUDxON) | [lencntrld, AUDxDR, dmasen], with [percntrld] printed under it |
| 001 → 101, the thrown-away word | (AUDxON · AUDxDAT) | [AUDxIR, lencount if lenfin-bar] |
| 101 → 010, the next word | (AUDxON · AUDxDAT) | [volcntrld, percntrld, pbufld1, AUDxDR if napnav] — **no lencount** |
| a word arriving in 010 or 011 | — | [lencntrld if (lenfin & AUDxON & AUDxDAT), lencount if (lenfin-bar & AUDxON & AUDxDAT), intreq2 if (lenfin & AUDxON & AUDxDAT)] |

and its NOTE: "Except for this case, dmasen is true only when LENFIN=1. Also,
AUDxDSR=AUDxDR · dmasen".

**Counted under the figure, the first pass is AUDxLEN words, the same as
every later pass.** With the counter loaded with L at the enable:

| word arriving | counter after it |
|---|---|
| the thrown-away word (001 → 101) | L − 1 |
| offset 0 (101 → 010) | L − 1, not counted |
| offset k in the main loop, k = 1 … L − 2 | L − 1 − k, reaching 1 at offset L − 2 |
| offset L − 1 | reloaded to L; its request, made while the counter stood at 1, was the restart |

The request for offset L − 1 goes out while `lenfin` holds, so by the NOTE it
is AUDxDSR, and the pass after it starts again at offset 0. Every later pass
counts L words between reloads in the same way.

---

## RTL and measurement

**NanoMig counts the length down on both start-up words, where Figure 5-8
counts only the first.** The arrival of the thrown-away word in state `001`,
[`src/minimig-aga/paula_audio_channel.v#L312-L321`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L312-L321),
and the arrival of offset 0 in `AUDIO_STATE_2`, coded `3'b011` and the figure's
101,
[`#L353-L362`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L353-L362),
both set `lencount = ~lenfin;`. Line 358 is the count the figure's 101 → 010
bracket does not print.

**The request that goes out on the same edge is built from the counter as it
stood before that edge's count**,
[`#L217-L221`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L217-L221), `dmas <= dmasen | lenfin;`,
with `lenfin` true only at one,
[`#L187`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L187).
A restart request makes Agnus fetch from the current pointer and then load
the location,
[`src/minimig-aga/agnus_audiodma.v#L137-L138`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_audiodma.v#L137-L138).
In the main loop each arriving word counts, and the one that finds the counter
at one reloads it,
[`#L391-L393`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L391-L393).

For AUDxLEN of 4 and more:

| word arriving | NanoMig's counter | Figure 5-8's counter |
|---|---|---|
| the thrown-away word | L − 1 | L − 1 |
| offset 0 | L − 2 | L − 1 |
| offset k in the main loop | L − 2 − k | L − 1 − k |
| the restart goes out with the fetch of | offset L − 2 | offset L − 1 |

- **From AUDxLEN = 4 up**, the first pass is offsets 0 to L − 2. Every later
  pass counts from a reload in the main loop and is L words.
- **At AUDxLEN = 2** the counter already stands at one when offset 0 arrives,
  `lencount = ~lenfin` counts nothing, the request that goes out is the
  restart, and NanoMig agrees with the figure.
- **At AUDxLEN = 3** line 358 takes the counter from 2 to 1 on the edge on
  which the request for offset 1 goes out, and that request is built from the
  2: it is no restart. Offset 1 arrives with the counter at one and reloads it
  (line 393) with no restart request having gone out. The counter runs down
  again over offsets 2 and 3, and the restart goes with the fetch of offset 4:
  five words, the last two past the data set.

### Measured

[`hrm/ch5-audio-dma-first-pass`](../../hrm/ch5-audio-dma-first-pass) wires
NanoMig's `paula_audio` to its `agnus_audiodma` and a RAM model, primes the
channel's pointer on another data set, then writes AUD0LC, AUD0LEN, AUD0VOL
and AUD0PER in p. 145's order and enables channel 0. It logs every address
`agnus_audiodma` fetches, and a pass runs from a fetch at offset 0 to the next
one. The graders are `test_ch5_audio_dma_first_pass.cpp:383-428`. The run
ends `15 passed, 7 failed`:

| AUDxLEN | pass 1 measured | the pages | passes 2 and 3 measured (`LATER-PASSES-LEN-<n>`) |
|---:|---|---|---|
| 2 | 0, 1 | 0, 1 | 0, 1 |
| 3 | **0 – 4** | 0 – 2 | 0 – 2 |
| 4 | **0 – 2** | 0 – 3 | 0 – 3 |
| 5 | **0 – 3** | 0 – 4 | 0 – 4 |
| 7 | **0 – 5** | 0 – 6 | 0 – 6 |
| 8 | **0 – 6** | 0 – 7 | 0 – 7 |
| 16 | **0 – 14** | 0 – 15 | 0 – 15 |

At every length the first fetch after the enable goes to the pointer the
priming run left, outside the data set (word `$4001` at AUDxLEN = 2, as the fixture-liveness line prints), and the next is offset 0. `NO-FETCH-BEYOND-DATA-SET-LEN-3` is red
on the fetches at offsets 3 and 4; at every other length no fetch reaches the
sixteen words after the set.

Inside the module, [`hrm/ch5-audio-channel-registers`](../../hrm/ch5-audio-channel-registers)
and [`hrm/ch5-audio-channel-state`](../../hrm/ch5-audio-channel-state) read
the same counter and its start-up arcs through `--public-flat-rw` and at the
module's request outputs, and five of their six reds carry implementation records on line 358; `5-p164.c28 5-sm.c29` has none. At the module's outputs, with AUDxLEN = 4, `5-p164.c23 5-p164.c24 5-sm.c25` finds no restart request between the arrivals of offsets 2 and 3, and `5-p164.c28 5-sm.c29` no interrupt request from offset 3's arrival to the next pass's first word.

[`integration/ch2-copper-move`](../../integration/ch2-copper-move), `P8`, reaches the same first pass through a 68000 program on fx68k and TG68K. After `BASELINE` and ADKCON = $00FF the program writes AUD0LC = $00044000, AUD0LEN = 4, AUD0VOL = 32 and AUD0PER = 256 (`prog_p8.s:31-34`), waits for the beam to leave line $40 and writes DMACON = $8201 (`:39-41`); no register of the program is read except VHPOSR in the line wait. `P8-PRE-B` holds: no channel-0 fetch before marker 2. A run is a maximal sequence of channel-0 fetches in `$44000`–`$441FF` each at the previous address + 2, and the comparator takes every run against four words (`test_ch2_copper_move.cpp:1363-1366`, graded at `:1427`). On fx68k and TG68K, in run M and in run C:

```text
MEASURED: P8 [run M] runs (length@first address): [3@44000] [4@44000] [4@44000] ...
FAIL: P8-RESTART: ... with AUD0LEN = 4 every run of fetches is $44000 $44002 $44004 $44006 ...
```

The first run is offsets 0 to 2, the AUDxLEN = 4 row of the table above; the printed runs after it are four words and, in run C, start at $44100 after the CPU writes AUD0LC = $44100.

---

## Risks and unknowns

- **p. 164's sentence, read at its literal extent, gives NanoMig's count**,
  and is the likely source of line 358: *"The length counter counts down once
  with each word that comes in."* Applied to every word that arrives after the
  enable, the thrown-away word and offset 0 both, it takes the counter to
  L − 2 before the main loop, and the first pass is L − 1 words. Applied to
  one of those two words and not the other — the figure counts the
  thrown-away word; a reading of "word" as data word counts offset 0 — it
  gives L. Only the reading that counts both at once gives L − 1, and it does
  not decide the case:
  - **The sentence describes the main loop.** It follows *"When the data
    arrives, state 010 is entered and the main loop continues until the DMA
    is turned off"*, and even there it is not a rule for every word: the word
    that arrives when the counter has finished reloads it instead (*"Also,
    the length counter is reloaded"*; the figure's `lencount if
    (lenfin-bar ...)`). It states no count for the first pass.
  - **p. 164 sends the reader to Figure 5-8**, and the figure prints a count
    on the thrown-away word and none on the next.
  - **pp. 139, 145 and 266 and the p. 146 example give the count
    positively**, AUDxLEN words retrieved before the channel starts again at
    AUDxLC, with no exception for the first pass. Under L − 1 each of them
    fails on it.
- **At AUDxLEN = 3 no reading of the pages permits the overread**: p. 139
  returns to the start once the last word of the data area is retrieved,
  p. 147 and p. 288 restart whenever the count is finished, and p. 164 sends
  the restart request when the counter finishes.
- **The start-up count is graded inside the module only.** Four of the six
  reds in `hrm/ch5-audio-channel-registers` and `hrm/ch5-audio-channel-state`
  read `lencnt`, `intreq2` and the arc's action signals through
  `--public-flat-rw`; the other two read when the restart and interrupt
  requests leave `paula_audio_channel`, and at the Agnus address port the
  finding rests on `hrm/ch5-audio-dma-first-pass`'s fetch addresses and, with the 68000
  writing the registers, on `integration/ch2-copper-move`'s channel-0 fetches at AUDxLEN = 4.
- **The four implementations were read from source and followed by hand**,
  for AUDxLEN 2, 3 and a general L, not run, and nothing here was measured on
  an Amiga.

For the specialists:

1. **Does Paula count the word that takes the channel from 101 to 010?** A
   four-word table played from a DMA start on real hardware, with its fetch
   addresses logged or its first audio interrupt timed against the words
   delivered, would settle it: by the figure the first reload follows five
   deliveries, by NanoMig four.
2. **Is p. 164's "once with each word that comes in" meant for the main loop
   alone?** The sentence and Figure 5-8 are the chapter's two descriptions of
   the counter; the figure prints no count on the 101 → 010 arc, and no page
   states a first pass shorter than AUDxLEN.

---

## Independent evidence

The question put to each: does the length counter count the word that takes
the channel from 101 to 010, and how many words does the first pass fetch?

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:1892-1896`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1892-L1896) | counts the thrown-away word; the 101 → 010 arc has no length term. The first pass is AUDxLEN words |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachine.cpp:267-282`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L267-L282) | counts the thrown-away word; `move_101_010()` has no length term. The first pass is AUDxLEN words |
| **Denise** | [`emulation/libami/paula/audio.cpp:85-117`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L85-L117) | counts the thrown-away word; the state-5 arc has no length term. The first pass is AUDxLEN words |
| **Copperline** | [`src/chipset/paula.rs:1882-1893`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1882-L1893) | counts the thrown-away word; `aud_move_101_010` has no length term. The first pass is AUDxLEN words |
| **MiSTer** | [`rtl/paula_audio_channel.v:358`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L358) | descendant — identical to NanoMig, both whole files: `lencount = ~lenfin;` on the arrival of the thrown-away word (`:317`) and of offset 0 |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:335`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L335) | descendant — the same line, `lencount = ~lenfin;` on both start-up arcs (`:294` and `:335`) |

**All four implement the page.** Each counts the thrown-away word, unless the
counter already stands at one, and none counts the word that takes the channel
from 101 to 010, which is Figure 5-8's bracket. Followed by hand, each gives a
first pass of AUDxLEN words at every length and returns to the location after
the data set's last word, at AUDxLEN = 3 as elsewhere. WinUAE and Denise, like
NanoMig, send the restart with a request and let the fetch that serves it load
the location; vAmiga and Copperline load it when the last word arrives. Both
give the same passes, and all four, like NanoMig, take the thrown-away word
from wherever the pointer stood.

### What other Commodore documents say

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 34 — agrees with the manual:

  > "Used to automatically restart pointers, such as the co-processor program counter (during vertical blank) and the Audio sampler counter (whenever the audio length count is finished."
  <!-- corpus: agnus-8371-spec/p034.txt -->

  It says what p. 288 says: Agnus restarts the audio pointer whenever the length count is finished. At AUDxLEN = 3 NanoMig's count finishes and reloads with no restart, and the pointer runs on two words past the set. The specification gives no start-up sequence and no count for the first pass.

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 17 — agrees with the manual:

  > "In the case of audio DMA, the backup value is reloaded into the pointer register whenever the audio waveform being output is finished."
  <!-- corpus: patent-us4874164-agnus-denise/p017.txt -->

  The pointer returns to the location when the waveform is finished, which excludes a fetch past the data set's end. Like the Agnus specification, it does not say when, after a DMA start, the count finishes.

---

## Decision

**One option: drop the count from the 101 → 010 arc**, as Figure 5-8 prints
it. In `paula_audio_channel.v`, `AUDIO_STATE_2`, line 358:

```verilog
lencount = 1'b0;   // was ~lenfin -- Figure 5-8's 101->010 bracket has no lencount
```

The count on the thrown-away word, line 317, stays. To repeat the check, copy `nanomig/` to a scratch directory, change line 358 there to `lencount = 1'b0;`, copy the three packages (`hrm/ch5-audio-dma-first-pass`, `hrm/ch5-audio-channel-registers`, `hrm/ch5-audio-channel-state`) next to it, and run `make -B -C hrm/<package> NANOMIG=<absolute path of the copy> test` in each. On that copy they print `22 passed, 0 failed`, `21 passed, 0 failed` and `35 passed, 8 failed`, the eight being [NM-HRM-026](NM-HRM-026.md)'s; the thirteen reds of those packages listed in row 4 turn green. `integration/ch2-copper-move` is not part of that copy run: its `P8-RESTART` has the first run of AUD0LEN = 4 as its subject, and with line 358 no longer counting it would read `$44000 $44002 $44004 $44006`, as `FIRST-PASS-LEN-4` does on the copy; that reading of the change was not run. `make test` on the repository's RTL does not run this copy, so the counts are a reading of the change, not a result of `nanomig/`.
