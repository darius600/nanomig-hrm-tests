# NM-HRM-084 — the length counter also counts the first data word after audio DMA starts: the first pass is one word short of AUDxLEN, and at AUDxLEN = 3 runs two words past the data set [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option, a one-line change; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_audio_channel.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L358) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L358) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**. NanoMig has `paula_audio_channel.v` from there unchanged and TC64 carries the same line, so there is no fix to port; WinUAE, vAmiga, Denise and Copperline all fetch AUDxLEN words on the first pass — see *What the others say* |
| **Test** | [`hrm/ch5-audio-dma-first-pass`](../../hrm/ch5-audio-dma-first-pass) — the package's own wrapper around `paula_audio` and `agnus_audiodma`, graded at `agnus_audiodma`'s address port, no CPU core: `FIRST-PASS-LEN-<n>`, claims `5-sum.c3`, `5-p139.c9`, `5-len.c1` (pp. 139, 145); `NO-FETCH-BEYOND-DATA-SET-LEN-3`, claims `5-p139.c7`, `5-loc.c7`, `5-p164.c23`, `5-p164.c25`, `5-sm.c26`, `5-p164.c26`, `5-sm.c27`, `5-p147.c8`, `5-p147.c9`, `5-join.c6` (pp. 139, 147, 164). [`hrm/ch5-audio-channel-registers`](../../hrm/ch5-audio-channel-registers) — `paula_audio_channel` alone: `AUDXLEN-BLOCK-1-INCLUDES-DISCARD` and `INTERRUPT-CADENCE-BLOCK-1-INCLUDES-DISCARD` (pp. 145, 164, Figure 5-8; no claim id). [`hrm/ch5-audio-channel-state`](../../hrm/ch5-audio-channel-state) — `paula_audio_channel` alone, reading its internal signals through `--public-flat-rw`: `F58.t001_101 F58.t101_010`, claims `F58.t001_101`, `F58.t101_010`, and `5-p166.c4 F58.t101_010`, claims `5-p166.c4`, `F58.t101_010` (Figure 5-8, p. 166). [`integration/ch5-audio-output-scanrate`](../../integration/ch5-audio-output-scanrate) shows the same short first pass through its precondition `A1` — see *Where else the suite meets it* |
| **Chapter** | 5 — Audio, printed pp. 139, 145–147, 164–166 (Figure 5-8); Appendix A, printed pp. 265–266; Appendix B, printed p. 288 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 7 red in `hrm/ch5-audio-dma-first-pass` (15 passed, 7 failed): `FIRST-PASS-LEN-3` (pass 1 measured offsets 0–4), `FIRST-PASS-LEN-4`, `FIRST-PASS-LEN-5`, `FIRST-PASS-LEN-7`, `FIRST-PASS-LEN-8`, `FIRST-PASS-LEN-16` (each one word short), `NO-FETCH-BEYOND-DATA-SET-LEN-3` (offsets 3 and 4 of a three-word set). 2 red in `hrm/ch5-audio-channel-registers` (19 passed, 2 failed): `AUDXLEN-BLOCK-1-INCLUDES-DISCARD` (5 words from DMA start to the first length-counter reload, where 6 is expected) and `INTERRUPT-CADENCE-BLOCK-1-INCLUDES-DISCARD` (4 words to the first `intreq2` rise, where 5 is expected), both reading their boundary inside the module. 2 red in `hrm/ch5-audio-channel-state` (33 passed, 10 failed; the other eight are [NM-HRM-026](NM-HRM-026.md)'s state codes), both inside the module: `F58.t001_101 F58.t101_010` (`lencnt` 4, 3, 2 at the load, the thrown-away word and offset 0, where the figure gives 4, 3, 3) and `5-p166.c4 F58.t101_010` (`lencount` = 1 on the 101 → 010 arc, whose bracket prints none). In the same runs `FIRST-PASS-LEN-2`, all seven `LATER-PASSES-LEN-<n>`, the between-reloads `AUDXLEN-SETS-WORDS-PER-BLOCK` and `INTERRUPT-CADENCE-TIED-TO-LENGTH`, and the main-loop `5-p164.c22 5-sm.c24 5-p165.c14` are green |
| **Verdict of the four** | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline all count the thrown-away start-up word and not the word that takes the channel from 101 to 010, as Figure 5-8's brackets do, so their first pass is AUDxLEN words like every later one and never runs past the data set |
| **See also** | [NM-HRM-026](NM-HRM-026.md) — the same state machine's codes: NanoMig's `3'b011` is the figure's 101 |

---

## What the HRM says

**The length register holds the number of words the channel retrieves before
it starts again at the location**, and no page limits that to later passes.
**Printed p. 145**, steps 2 and 4 of the procedure for a steady tone:

> 2. Create the data set containing the pairs of data samples (data words). Normally, a data set contains the definition of one waveform.

> 4. Set the length register, AUDxLEN, to the number of data words to be retrieved before starting at the address currently in AUDxLC.

**Printed p. 139**:

> The length of the data is the number of samples in your waveform divided by 2, or the number of words in the data set.

> Each time the system retrieves the last audio word from the data area, it uses the contents of these location registers to again find the start of the data.

**Printed p. 266**, AUDxLEN:

> This register contains the length (number of words) of audio channel x DMA data.

**Printed p. 147**:

> If you do not rewrite the registers, the current waveform will be repeated. Each time the length counter reaches zero, both the location and length registers are reloaded with the same values to continue the audio output.

**Printed p. 288**, Appendix B's note on the location registers:

> Chip memory location (starting address) of DMA data. Used to automatically restart pointers, such as the Copper program counter (during vertical blank) and the audio sample counter (whenever the audio length count is finished).

**Printed p. 265**, AUDxDAT:

> When the DMA data is finished (words outputted=length) and the data in this register has been used, an audio channel interrupt request is set.

The chapter's worked example on p. 146, *read from the printed page*, plays
an eight-byte sine table, `DC.B 0, 90, 127, 90, 0, -90, -127, -90`, four
words, with `MOVE.W #4,AUD0LEN(a0) ;Set length in words`.

**Printed p. 164** walks through the start-up:

> In DMA-driven operation, transition to the 001 state occurs and DMA requests are sent to Agnus as soon as DMA is turned on. Because of pipelining in Agnus, the first data word must be thrown away. State 101 is entered as soon as this word arrives; a request for the next data word has already gone out. When the data arrives, state 010 is entered and the main loop continues until the DMA is turned off. The length counter counts down once with each word that comes in. When it finishes, a DMA restart request goes to Agnus along with the regular DMA request. This tells Agnus to reset the pointer to the beginning of the table of data. Also, the length counter is reloaded and an interrupt request goes out soon after the length counter finishes (counts to one).

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

### Where NanoMig's count comes from, and why it does not decide the case

**One sentence gives NanoMig's count**: p. 164's *"The length counter counts
down once with each word that comes in."* Applied to every word that arrives
after the enable, the thrown-away word and offset 0 both, it takes the
counter to L − 2 before the main loop, and the first pass is L − 1 words. It is
the likely source of the RTL's count. Applied to one of those two words and
not the other — the figure counts the thrown-away word; a reading of "word"
as data word counts offset 0 — it gives L. Only the reading that counts both
at once gives L − 1, and it does not decide the case:

- **The sentence describes the main loop.** It follows *"When the data
  arrives, state 010 is entered and the main loop continues until the DMA is
  turned off"*, and even there it is not a rule for every word: the word that
  arrives when the counter has finished reloads it instead (*"Also, the
  length counter is reloaded"*; the figure's `lencount if (lenfin-bar ...)`).
  It states no count for the first pass.
- **p. 164 sends the reader to Figure 5-8**, and the figure prints a count on
  the thrown-away word and none on the next.
- **pp. 139, 145 and 266 and the p. 146 example give the count positively**,
  AUDxLEN words retrieved before the channel starts again at AUDxLC, with no
  exception for the first pass. Under L − 1 each of them fails on it.

No reading of the pages gives what NanoMig does at AUDxLEN = 3, five words,
the last two past the data set: p. 139 returns to the start once the last word
of the data area is retrieved, p. 147 and p. 288 restart whenever the count is
finished, and p. 164 sends the restart request when the counter finishes.

---

## What NanoMig does

**It counts the length down on both start-up words, where Figure 5-8 counts
only the first.** In state `001`, the arrival of the thrown-away word —
[`src/minimig-aga/paula_audio_channel.v#L312-L321`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L312-L321):

```verilog
if (AUDxON && AUDxDAT) //requested data has arrived
begin
  audio_next = AUDIO_STATE_2;
  AUDxDR = 1'b1;
  AUDxIR = 1'b1;
  lencount = ~lenfin;
  pbufld1 = 1'b0;  //first data received, discard it since first data access is used to reload pointer
```

and in `AUDIO_STATE_2`, coded `3'b011` and the figure's 101, the arrival of
offset 0 —
[`#L353-L362`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L353-L362):

```verilog
if (AUDxON && AUDxDAT) //requested data has arrived
begin
  audio_next = AUDIO_STATE_3;
  AUDxDR = 1'b1;
  AUDxIR = 1'b0;
  lencount = ~lenfin;
  pbufld1 = 1'b1;  //new data has been just received so put it in the output buffer
```

Line 358 is the count the figure's 101 → 010 bracket does not print.

**The request that goes out on the same edge is built from the counter as it
stood before that edge's count**,
[`#L217-L221`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L217-L221), `dmas <= dmasen | lenfin;`,
with `lenfin` true only at one,
[`#L187`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L187), `assign lenfin = (lencnt[15:0]==1 && cck) ? 1'b1 : 1'b0;`.
A restart request makes Agnus fetch from the current pointer and then load
the location,
[`src/minimig-aga/agnus_audiodma.v#L137-L138`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_audiodma.v#L137-L138), `audpt[channel] <= #1 dmas ? audlcout[20:1] : audptout[20:1] + 1'b1;`.
In the main loop each arriving word counts, and the one that finds the counter
at one reloads it,
[`#L391-L393`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L391-L393), `lencount = ~lenfin & AUDxON & AUDxDAT;` and `lencntrld = lenfin & AUDxON & AUDxDAT;`.

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
priming run left, outside the data set (word `$4001` at every length traced
below), and the next is offset 0. `NO-FETCH-BEYOND-DATA-SET-LEN-3` is red
on the fetches at offsets 3 and 4; at every other length no fetch reaches the
sixteen words after the set.

**Where the restart goes.** The same stimulus with the restart bit Paula sends
on each fetch logged beside it (`R`) — a measurement for this page, not a
graded assertion:

```
AUDxLEN 2:  $4001R  0  1R  0  1R  0  1R
AUDxLEN 3:  $4001R  0  1  2  3  4R  0  1  2R  0  1  2R
AUDxLEN 4:  $4001R  0  1  2R  0  1  2  3R  0  1  2  3R
AUDxLEN 5:  $4001R  0  1  2  3R  0  1  2  3  4R
AUDxLEN 8:  $4001R  0  1  2  3  4  5  6R  0  1  2  3  4  5  6  7R
```

**One line decides it.** A copy of the RTL with only line 358 changed to
`lencount = 1'b0;`, the figure's bracket, run through the unchanged package,
reads `22 passed, 0 failed`: pass 1 is offsets 0 to L − 1 at every length and
no fetch passes the set's end. The package records the same result in its
`implementation` mutation lines.

**Inside the module.**
[`hrm/ch5-audio-channel-registers`](../../hrm/ch5-audio-channel-registers)
drives `paula_audio_channel` alone and answers each request with a word. It
reads the block boundary on the internal `lencnt` and `intreq2` through
`--public-flat-rw` and counts the words answered at the ports:

- `AUDXLEN-BLOCK-1-INCLUDES-DISCARD`
  (`test_ch5_audio_channel_registers.cpp:974-981`), AUDxLEN = 5: from DMA
  start to the first reload the pages give the thrown-away word and five data
  words, 6; NanoMig counts 5.
- `INTERRUPT-CADENCE-BLOCK-1-INCLUDES-DISCARD` (`:1211-1218`), AUDxLEN = 4:
  from DMA start to the first `intreq2` rise the figure gives 5, the rise
  coming with block 1's last data word; NanoMig counts 4.

The run ends `19 passed, 2 failed`; between reloads,
`AUDXLEN-SETS-WORDS-PER-BLOCK` and `INTERRUPT-CADENCE-TIED-TO-LENGTH` are
green at 5 and 4. On the copy with line 358 changed the two block-1 spans
measure 6 and 5 and the package reads `21 passed, 0 failed`.

[`hrm/ch5-audio-channel-state`](../../hrm/ch5-audio-channel-state) drives
the same module alone, with AUDxLEN = 4, and reads the counter and the arc's
actions directly:

- `F58.t001_101 F58.t101_010` (`test_ch5_audio_channel_state.cpp:1305-1318`)
  samples `lencnt` at the enable's load, at the thrown-away word's arrival and
  at offset 0's, and requires one step down on the first arc and none on the
  second. NanoMig reads `4 3 2`: the counter moves on 101 → 010.
- `5-p166.c4 F58.t101_010` (`:1136-1161`) samples the actions on the 101 → 010
  arc and requires the figure's four, `volcntrld`, `percntrld`, `pbufld1` and
  `AUDxDR`, with no `lencount`. NanoMig reads all four set and `lencount=1`.

The run ends `33 passed, 10 failed`; the other eight reds read the state
register's codes and are [NM-HRM-026](NM-HRM-026.md)'s. The main-loop
`5-p164.c22 5-sm.c24 5-p165.c14`, which grades each arriving word from the
third on, is green; its full sequence reads `4 3 2 1 4 3 2 1 4`. On the copy
with line 358 changed the start-up reads `4 3 3`, the full sequence
`4 3 3 2 1 4 3 2 1`, the arc `lencount=0`, and the package reads
`35 passed, 8 failed`: the two reds above turn green and nothing else moves.

---

## Where else the suite meets it

- **[`integration/ch5-audio-output-scanrate`](../../integration/ch5-audio-output-scanrate)
  sees the short first pass through a precondition.** Its analysis window
  starts at the second read of the table's first word, t_2, and leaves the
  first pass out, but its anchor `A1` requires t_2 to be preceded by a read
  of the table's last word, which is the first pass's end. For table T1, four
  words and the first DMA start after reset, `out_fx68k.txt` and
  `out_tg68k.txt` both read, under ECS/PAL and OCS/PAL alike,
  `t_2 (word_addr=0x1000) at cycle 2282633 is preceded by 0x1002, not the table's last word (0x1003)`:
  the first pass ends one word short. For T3, 256 words, fx68k reads t_2
  preceded by `0x11FE`, not `0x11FF`. `A1` is red there, and P1, P2 and, on
  fx68k, P4 are not graded.
  T2's anchor holds on both cores and T3's on TG68K; each of those runs
  re-enables the channel a few instructions after switching it off, and
  p. 145 lets a channel disabled for less than two sampling periods stay on.
  Which of those runs went through the start-up sequence is not traced here.
- **Four places grade around the first pass and so do not see it.**
  [`integration/ch5-audio-attach-modulation`](../../integration/ch5-audio-attach-modulation)'s
  `stepping_cycles()` (`test_ch5_audio_attach_modulation.cpp:356-373`), used by
  `PC3` and by `P9` (a) and (b), grades each channel from its first restart on,
  the first fetch at offset 0 after its first fetch, and leaves the first pass
  to `hrm/ch5-audio-dma-first-pass`; `PC3` reads the channel's own 4 KB region
  (`:391-395`), `P9` (a) and (b) only the set's own words.
  [`integration/ch2-copper-move`](../../integration/ch2-copper-move)'s `P7`
  leaves the first pass out by its own specification (section 3.6) and grades
  every block end after it.
  [`integration/ch5-audio-registers-dma`](../../integration/ch5-audio-registers-dma)'s
  `P1` counts reads at each channel's start address, at least two per window,
  which a short first pass does not change.
  [`hrm/ch5-audio-period-and-length`](../../hrm/ch5-audio-period-and-length)'s
  `G_LEN_VALUE` grades the last two completed intervals, and its comment
  (`test_ch5_audio_period_and_length.cpp:1070-1072`) calls the first interval
  one word short.

---

## What the others say

The question put to each: does the length counter count the word that takes
the channel from 101 to 010, and how many words does the first pass fetch?
Each was read from its source and followed by hand for AUDxLEN 2, 3 and a
general L; none of the four was run.

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:1808-1820`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1808-L1820), [`:1868-1872`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1868-L1872), [`:1892-1896`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1892-L1896), [`:2537-2551`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L2537-L2551), [`:1619-1631`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1619-L1631), [`custom.cpp:11826-11836`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11826-L11836) | <pre>setdr(nr, false);<br>if (cdp-&gt;wlen != 1)<br>&nbsp;&nbsp;&nbsp;&nbsp;cdp-&gt;wlen = (cdp-&gt;wlen - 1) &amp; 0xffff;<br>cdp-&gt;state = 5;</pre> the thrown-away word, arriving in state 1, counts the length down; the next, arriving in state 5, loads the word, requests another, `if (napnav)` `setdr(nr, false);`, and sets `cdp->state = 2;` **with no length term**. In states 2 and 3 every word counts, and the one that finds `cdp->wlen == 1` reloads it, `cdp->wlen = cdp->len;`. The request made while the count is 1 is the restart, `if (!startup && cdp->wlen == 1) {`, and the slot that serves it reads the current pointer and then takes the location, `pt = audio_getloadpt(num);`. The enable requests a restart too, `cdp->dsr = true;`, so the thrown-away word comes from the old pointer, as in NanoMig. The first pass is AUDxLEN words |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachine.cpp:215-227`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L215-L227), [`:240-254`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L240-L254), [`:267-282`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L267-L282), [`Core/Components/Paula/Audio/StateMachineRegs.cpp:64-78`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachineRegs.cpp#L64-L78), [`Core/Components/Paula/Audio/StateMachine.h:229-241`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.h#L229-L241), [`Core/Components/Agnus/Agnus.h:683`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L683), [`Core/Components/Agnus/AgnusDma.cpp:98-111`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L98-L111) | <pre>AUDxIR();<br>AUDxDR();<br>AUDxDSR();<br>if (!lenfin()) lencount();</pre> `move_001_101()`, taken when the thrown-away word arrives, counts the length down and points the channel back at its location at once, `void AUDxDSR() { agnus.reloadAUDxPT<nr>(); }`, `audpt[x] = audlc[x];`. `move_101_010()` loads the next word and requests another, `if (napnav()) AUDxDR();`, **with no length term**. In states 010 and 011 every word counts, `if (!lenfin()) {` `lencount();`, and the one that finds the counter at one, `return audlen == 1;`, reloads it and restarts the pointer, `lencntrld();` `AUDxDSR();`. The enable, `move_000_001()`, loads the counter and requests a word, `lencntrld();` `AUDxDR();`, and the fetch reads the pointer where it stands and steps it, `audpt[channel] += 2;`. The first pass is AUDxLEN words |
| **Denise** | [`emulation/libami/paula/audio.cpp:5-9`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L5-L9), [`:52-68`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L52-L68), [`:85-117`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L85-L117), [`:194-203`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L194-L203), [`emulation/libami/agnus/dma.cpp:328-339`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L328-L339) | <pre>} else if (cha.state == 1) {<br>&nbsp;&nbsp;&nbsp;&nbsp;AUDxIR(nr)<br>&nbsp;&nbsp;&nbsp;&nbsp;dmasen()<br><br>&nbsp;&nbsp;&nbsp;&nbsp;if (notLenfin())<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;lencount()<br><br>&nbsp;&nbsp;&nbsp;&nbsp;cha.state = 5;</pre> the thrown-away word, in state 1, counts the length down; the next, in state 5, loads the buffer and requests again, `if (cha.napnav) {` `dmasen()`, **with no length term**. Every word in states 2 and 3 counts, `if (notLenfin()) {` `lencount()`, or reloads the counter, `lencntrld()`. A request made while the counter is at one is the restart, `#define dmasen() if (cha.len == 1) cha.dsr = true;`, and the slot serving it reads the current pointer and then takes the latch, `cha.ptr = cha.ptrLatch;`. The enable requests a restart too, `cha.dsr = true;`, so the thrown-away word comes from the old pointer. The first pass is AUDxLEN words |
| **Copperline** | [`src/chipset/paula.rs:1849-1854`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1849-L1854), [`:1867-1880`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1867-L1880), [`:1882-1893`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1882-L1893), [`:1962-1974`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1962-L1974), [`:1805-1812`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1805-L1812), [`:2107-2117`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2107-L2117) | <pre>fn aud_move_001_101(&amp;mut self, ch: usize) -&gt; u16 {<br>&nbsp;&nbsp;&nbsp;&nbsp;self.chans[ch].sm_dr = true;<br>&nbsp;&nbsp;&nbsp;&nbsp;self.aud_dsr(ch);<br>&nbsp;&nbsp;&nbsp;&nbsp;if !self.aud_lenfin(ch) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;self.aud_lencount(ch);<br>&nbsp;&nbsp;&nbsp;&nbsp;}</pre> the thrown-away word counts the length down and resets the pointer; its comment says *"this first fetch used the stale pointer; its word is never played"*. The next word, under *"the second word arrived; output begins"*, requests another, `if self.aud_napnav(ch) {`, **with no length term**. Every word in 010 and 011 counts, or reloads the counter and resets the pointer, `self.aud_lencntrld(ch);` `self.aud_dsr(ch);`, under *"count the word; on the final one reload the length/pointer"*. The reset is immediate, `self.chans[ch].ptr = self.chans[ch].lc & self.dma_ptr_mask();`, and a grant steps the pointer before the word goes in, `ch.ptr = ch.ptr.wrapping_add(2) & ptr_mask;`. The enable loads the counter and requests a word, `self.aud_lencntrld(ch);` `self.chans[ch].sm_dr = true;`. The first pass is AUDxLEN words |
| **MiSTer** | [`rtl/paula_audio_channel.v:317`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L317), [`:358`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L358), [`rtl/agnus_audiodma.v:137-138`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L137-L138) | descendant — identical to NanoMig, both whole files: `lencount = ~lenfin;` on the arrival of the thrown-away word and of offset 0, and the same pointer update, `audpt[channel] <= #1 dmas ? audlcout[20:1] : audptout[20:1] + 1'b1;` |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:294`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L294), [`:335`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L335), [`rtl/minimig/agnus_audiodma.v:137-138`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_audiodma.v#L137-L138) | descendant — the same two lines, `lencount = ~lenfin;` on both start-up arcs; the channel file lacks NanoMig's PWM volume gating, its declarations and its `sample_okk` output, none of which reach the length counter, and otherwise differs only in comments and blank lines; `agnus_audiodma.v` is identical |

**All four implement the page.** Each counts the thrown-away word, unless the
counter already stands at one, and none counts the word that takes the channel
from 101 to 010, which is Figure 5-8's bracket. Followed by hand, each gives a
first pass of AUDxLEN words at every length and returns to the location after
the data set's last word, at AUDxLEN = 3 as elsewhere. WinUAE and Denise, like
NanoMig, send the restart with a request and let the fetch that serves it load
the location; vAmiga and Copperline load it when the last word arrives. Both
give the same passes, and all four, like NanoMig, take the thrown-away word
from wherever the pointer stood.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 34 — agrees with the manual:

  > "Used to automatically restart pointers, such as the co-processor program counter (during vertical blank) and the Audio sampler counter (whenever the audio length count is finished."
  <!-- corpus: agnus-8371-spec/p034.txt -->

  It says what p. 288 says: Agnus restarts the audio pointer whenever the length count is finished. At AUDxLEN = 3 NanoMig's count finishes and reloads with no restart, and the pointer runs on two words past the set. The specification gives no start-up sequence and no count for the first pass.

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 17 — agrees with the manual:

  > "In the case of audio DMA, the backup value is reloaded into the pointer register whenever the audio waveform being output is finished."
  <!-- corpus: patent-us4874164-agnus-denise/p017.txt -->

  The pointer returns to the location when the waveform is finished, which excludes a fetch past the data set's end. Like the Agnus specification, it does not say when, after a DMA start, the count finishes.

Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989 — it names a length register, a length counter and the load and count signals that drive it, and no start-up sequence; 6502.org dissection of Paula 8364R7, 2023 — it names a decrementing 16-bit length counter "which defines the amount of data words to be read" and leaves the PLA that controls each channel, DMA requests included, undecoded; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

---

## Suggested change

**One option: drop the count from the 101 → 010 arc**, as Figure 5-8 prints
it. In `paula_audio_channel.v`, `AUDIO_STATE_2`, line 358:

```verilog
lencount = 1'b0;   // was ~lenfin -- Figure 5-8's 101->010 bracket has no lencount
```

The count on the thrown-away word, line 317, stays: the figure prints it.
What it changes, measured on a copy of the RTL with only this line changed:

- pass 1 is AUDxLEN words at every length, and at AUDxLEN = 3 no fetch passes
  the set's end: `hrm/ch5-audio-dma-first-pass` reads `22 passed, 0 failed`,
  `hrm/ch5-audio-channel-registers` `21 passed, 0 failed`;
- block 1 grows by one word: with AUDxLEN = 5 the first reload comes 6 words
  after the enable instead of 5, and with AUDxLEN = 4 the first `intreq2` 5
  words after instead of 4;
- the counter holds on the 101 → 010 arc, `lencnt` reading `4 3 3` over the
  start-up: `hrm/ch5-audio-channel-state` goes from `33 passed, 10 failed` to
  `35 passed, 8 failed`, the eight left being
  [NM-HRM-026](NM-HRM-026.md)'s state codes, which this line does not touch;
- the requests at the enable, the thrown-away word and every later pass are
  unchanged;
- NanoMig would agree with all four implementations; both Minimig lines carry
  the current line.

---

## What holds the finding up

- **The pages give the count positively, for every pass.** p. 145 step 4,
  p. 139, p. 266 and the p. 146 example set AUDxLEN to the words retrieved
  before the channel starts again at AUDxLC, and Figure 5-8, to which p. 164
  defers, gives the same count by its own brackets.
- **The reds are at the pins.** `hrm/ch5-audio-dma-first-pass` grades the
  addresses `agnus_audiodma` drives; `LATER-PASSES-LEN-<n>` is green at all
  seven lengths and `FIRST-PASS-LEN-2` is green, so the grader, the stimulus
  and the pass boundaries work where NanoMig follows the pages.
- **One line decides it**, and changing it to the figure's bracket turns all
  eleven reds green with no green going red in any of the three packages.
- **At AUDxLEN = 3 no reading of the pages permits the overread**: p. 139,
  p. 147, p. 164, p. 288 and the figure's NOTE all tie the return to the start
  to the end of the count or of the data.
- **All four implementations implement the page**, and two Commodore
  documents state the restart the same way.

## What weakens it

- **p. 164's sentence, read at its literal extent, gives NanoMig's count** for
  AUDxLEN 4 and up, and is the likely source of line 358. It describes the
  main loop, defers to Figure 5-8, and leaves the four positive statements of
  the count failing on the first pass — see *Where NanoMig's count comes
  from*.
- **The start-up count is graded inside the module only.** The four reds in
  `hrm/ch5-audio-channel-registers` and `hrm/ch5-audio-channel-state` read
  `lencnt`, `intreq2` and the arc's action signals through
  `--public-flat-rw`; at the pins the finding rests on
  `hrm/ch5-audio-dma-first-pass`'s fetch addresses.
- **The four implementations were read and followed by hand**, not run, and
  nothing here was measured on an Amiga.

---

## For the specialists

1. **Does Paula count the word that takes the channel from 101 to 010?** A
   four-word table played from a DMA start on real hardware, with its fetch
   addresses logged or its first audio interrupt timed against the words
   delivered, would settle it: by the figure the first reload follows five
   deliveries, by NanoMig four.
2. **Is p. 164's "once with each word that comes in" meant for the main loop
   alone?** The sentence and Figure 5-8 are the chapter's two descriptions of
   the counter; the figure prints no count on the 101 → 010 arc, and no page
   states a first pass shorter than AUDxLEN.
