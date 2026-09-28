# ch5-audio-dma-first-pass -- SPECIFICATION

The question: how many words of an audio data set channel 0 fetches on its
first pass after DMA is enabled, before the first restart at AUDxLC, and
whether any fetch lies beyond the data set.

The printed Amiga Hardware Reference Manual, Third Edition, is the only
oracle. Every expected value below comes from the pages quoted here.

## Source claims (verbatim quotations, whitespace normalised)

Ids are chapter-5 claim ids, carried from the chapter's own claim records by
matching each quoted sentence to its recorded quote text. Several sentences
were independently extracted more than once (different reader lots reading
the same page), so more than one id can name the identical sentence; every id
found is listed and every one is cited by an assertion below.

| id(s) | page | quote |
|---|---|---|
| `5-sum.c3` | 145 | "4. Set the length register, AUDxLEN, to the number of data words to be retrieved before starting at the address currently in AUDxLC." (step 4 of the printed enable procedure) |
| `5-p139.c8`, `5-p139.c9`, `5-len.c1` | 139 | "The length of the data is the number of samples in your waveform divided by 2, or the number of words in the data set." |
| `5-p139.c7`, `5-loc.c7` | 139 | "Each time the system retrieves the last audio word from the data area, it uses the contents of these location registers to again find the start of the data." |
| `5-p164.c15`, `5-p164.c16`, `5-sm.c17`, `5-sm.c18` | 164 | "In DMA-driven operation, transition to the 001 state occurs and DMA requests are sent to Agnus as soon as DMA is turned on." |
| `5-p164.c17`, `5-sm.c19` | 164 | "Because of pipelining in Agnus, the first data word must be thrown away." |
| `5-p164.c18`, `5-p164.c19`, `5-sm.c20`, `5-sm.c21` | 164 | "State 101 is entered as soon as this word arrives; a request for the next data word has already gone out." |
| `5-p164.c20`, `5-p164.c21`, `5-sm.c22`, `5-sm.c23` | 164 | "When the data arrives, state 010 is entered and the main loop continues until the DMA is turned off." |
| `5-p164.c22`, `5-sm.c24` | 164 | "The length counter counts down once with each word that comes in." |
| `5-p164.c23`, `5-p164.c24`, `5-sm.c25` | 164 | "When it finishes, a DMA restart request goes to Agnus along with the regular DMA request." |
| `5-p164.c25`, `5-sm.c26` | 164 | "This tells Agnus to reset the pointer to the beginning of the table of data." |
| `5-p164.c26`, `5-p164.c27`, `5-sm.c27`, `5-sm.c28` | 164 | "Also, the length counter is reloaded and an interrupt request goes out soon after the length counter finishes (counts to one)." |
| `F58.t101_010` | 166 | Figure 5-8, the 101->010 arc: cause "(AUDxON . AUDxDAT)", action bracket "[volcntrld, percntrld, pbufld1, AUDxDR if napnav]" -- no `lencount` term on this arc. |
| `F58.loop.lenfin` | 166 | Figure 5-8, the 010/011 main-loop arcs: the bracket group "[lencntrld if (lenfin & AUDxON & AUDxDAT)], [lencount if (lenfin-bar & AUDxON & AUDxDAT)], [intreq2 if (lenfin & AUDxON & AUDxDAT)]". |
| `5-p147.c8`, `5-p147.c9`, `5-join.c6` | 147 | "Each time the length counter reaches zero, both the location and length registers are reloaded with the same values to continue the audio output." |
| `5-appA-p265.c46`, `5-appA-p265.c47`, `A-audxdat.c8`, `A-audxdat.c9` | 265 | AUDxDAT: "When the DMA data is finished (words outputted=length) and the data in this register has been used, an audio channel interrupt request is set." |

## Reading

Figure 5-8's 101->010 arc counts the discarded first word (001->101) and not
the second (101->010, which carries volcntrld/percntrld/pbufld1/AUDxDR but no
`lencount`); every word that reaches the main loop (010/011) counts once,
per the main-loop bracket's own `lencount if lenfin-bar`. With the length
counter loaded with L (AUDxLEN) at DMA-enable, the pass from the first fetch
at AUDxLC to the next restart is therefore offsets 0..L-1, the same as every
later pass -- p.145 step 4 sets AUDxLEN to the word count fetched before
restarting at AUDxLC, p.139 defines that count as the number of words in the
data set, and p.139 separately states the channel returns to the start of
the data once the last word of the data area has been retrieved, so no fetch
lies beyond offset L-1.

A second reading of the same page-164 material is possible: "the length
counter counts down once with each word that comes in", read as covering the
101->010 word too (not only the main-loop words), gives a first pass of L-1
words instead of L. This package does not grade that reading. Figure 5-8's
own printed bracket on the 101->010 arc carries no `lencount` term, so the
figure itself does not support counting that word, and p.139 and p.145 step
4 both fail on the first pass under an L-1 reading (an L-1-word first pass
would not match "the number of data words to be retrieved" nor "the number
of words in the data set" for AUDxLEN=L). The package grades the figure's
own reading: pass 1 is exactly L words.

Whether the discarded first fetch comes from the previously-loaded pointer
or already from AUDxLC, and which word is output first, the quoted pages
leave open; this package grades neither.

## Fixture

A package wrapper, `audio_dma_first_pass_rig`, instantiates `paula_audio`
(which itself instantiates `paula_audio_channel` for each of the four
channels, `paula_audio_mixer` and `paula_audio_volume`) and
`agnus_audiodma`, both from the pinned NanoMig submodule, and wires them --
this is the wrapper's own structure, not a claim under test:

- `strhor = (hpos == 9'd13)`, matching the Agnus top's own generation of
  that pulse.
- `paula_audio`'s `dmal`/`dmas` outputs feed `agnus_audiodma`'s
  `audio_dmal`/`audio_dmas` inputs directly.
- The shared Paula register bus (`paula_audio.reg_address_in`/`data_in`):
  on a cycle where `agnus_audiodma.dma` = 1, it carries
  `agnus_audiodma.reg_address_out` with data taken from the fixture's own
  RAM at `agnus_audiodma.address_out`, in the same clk7 cycle; on every
  other cycle it carries the harness's own register-write address/data, with
  idle address `8'hFF` when the harness is not writing.
- `agnus_audiodma.reg_address_in` always carries the harness's own
  register-write address (idle `8'hFF` when not writing) -- never the
  DMA-muxed value, since during a granted DMA cycle `agnus_audiodma` is
  itself the one producing `reg_address_out`.
- `dmaena` presented to `paula_audio` is channel 0 only (`{3'b0,en}`);
  `audpen` likewise (`{3'b0,pend}`).
- Clocking: `clk7_en` once every cycle (a 4-tick raw-clock divider), `cck`
  alternating once every 2 clk7_en ticks; `hpos[8:1]` increments on the edge
  where `cck`=1 and wraps 226->0, `hpos[0]` = `cck`. Register writes are
  driven for exactly one clk7_en cycle each, outside the audio DMA slot.
- RAM: a small combinational fixture memory, addressed by
  `agnus_audiodma.address_out` (a word address), holding: the priming set P
  at word address `$4000` (`$AAAA`, `$BBBB`); the graded data set D at word
  address `$8000`, word k = `$1050 + $0101*k` for k < the run's own AUDxLEN;
  sentinels at D+LEN..D+LEN+15 (word offsets), value `$EE00+j`; any other
  address reads a fixed, otherwise-unused fill value.

Only `agnus_audiodma.address_out` and `agnus_audiodma.dma` are graded.
`paula_audio`'s own `dmal[0]`/`dmas[0]` are brought out for a fixture-
liveness check only, not for any claim-based assertion.

## Stimulus (channel 0), per LEN in {2, 3, 4, 5, 7, 8, 16}

1. `rst` high for 20 cycles, then 20 idle cycles.
2. Priming, so the Agnus pointer sits somewhere other than D:
   AUD0LCH($0A0)<-$0000, AUD0LCL($0A2)<-$8000, AUD0LEN($0A4)<-2,
   AUD0VOL($0A8)<-64, AUD0PER($0A6)<-200; en=1 for 8 lines; then en=0,
   pend=1, wait 4*200+1000 colour clocks.
3. Graded run, in p.145's own order: AUD0LCH<-$0001, AUD0LCL<-$0000,
   AUD0LEN<-LEN, AUD0VOL<-64, AUD0PER<-200; pend stays 1; then en<-1 (p.144:
   "When both DMAEN and AUDxEN are set, the DMA will start for channel x").
4. Run (4*LEN+6)*400 + 4*227 colour clocks (cck ticks).

## Observation

At the RAM port: `agnus_audiodma.address_out`, sampled on every cycle with
`agnus_audiodma.dma`=1, after `en` rises in step 3 -- giving fetches F_1,
F_2, ... in order, each with `offset = address_out - $8000` (word address).
`j` is the index of the first fetch at offset 0. Pass n runs from an
offset-0 fetch up to, not including, the next offset-0 fetch. The output
side (`sample`, `ldata`/`rdata`) is not graded for pass length.

## Assertions

One instance of each of the following three per LEN in {2, 3, 4, 5, 7, 8,
16} -- 21 assertions.

- `FIRST-PASS-LEN-<n>`: precondition -- j in {1,2}, and if j=2, F_1's own
  address lies outside the byte range D..D+2*(LEN+16) (word range
  $8000..$8000+LEN+16). Expected, when the precondition holds: pass 1 (the
  fetches from F_j up to, not including, the next offset-0 fetch) is exactly
  the offsets 0, 1, ..., LEN-1, in order. Cites `5-sum.c3`, `5-p139.c8`,
  `5-p139.c9`, `5-len.c1`, `5-p164.c15`, `5-p164.c16`, `5-sm.c17`,
  `5-sm.c18`, `5-p164.c17`, `5-sm.c19`, `5-p164.c18`, `5-p164.c19`,
  `5-sm.c20`, `5-sm.c21`, `5-p164.c20`, `5-p164.c21`, `5-sm.c22`,
  `5-sm.c23`, `5-p164.c22`, `5-sm.c24`, `F58.t101_010`.
- `NO-FETCH-BEYOND-DATA-SET-LEN-<n>`: no fetch anywhere in the whole run
  (not just pass 1) has offset in LEN..LEN+15. Cites `5-p139.c7`,
  `5-loc.c7`, `5-p164.c23`, `5-p164.c24`, `5-sm.c25`, `5-p164.c25`,
  `5-sm.c26`, `5-p164.c26`, `5-p164.c27`, `5-sm.c27`, `5-sm.c28`,
  `F58.loop.lenfin`, `5-p147.c8`, `5-p147.c9`, `5-join.c6`,
  `5-appA-p265.c46`, `5-appA-p265.c47`, `A-audxdat.c8`, `A-audxdat.c9`.
- `LATER-PASSES-LEN-<n>` (green control, same basis as
  `NO-FETCH-BEYOND-DATA-SET-LEN-<n>`): passes 2 and 3 each equal exactly the
  offsets 0, 1, ..., LEN-1, in order. Cites `5-p164.c23`, `5-p164.c24`,
  `5-sm.c25`, `5-p164.c25`, `5-sm.c26`, `5-p164.c26`, `5-p164.c27`,
  `5-sm.c27`, `5-sm.c28`, `F58.loop.lenfin`, `5-p147.c8`, `5-p147.c9`,
  `5-join.c6`, `5-appA-p265.c46`, `5-appA-p265.c47`, `A-audxdat.c8`,
  `A-audxdat.c9`.

## Mutation plan

Stimulus mutations only -- the RTL and every expected value are never
touched.

- Control stimulus, LEN=2: in the LEN=2 run, write AUD0LEN<-3 instead of 2
  (keeping every other write and the run duration as the LEN=2 recipe
  gives them), and grade the SAME captured run against the LEN=2
  expectation. Both `FIRST-PASS-LEN-2` and `NO-FETCH-BEYOND-DATA-SET-LEN-2`
  must go red. The same mutated capture is also graded against
  `LATER-PASSES-LEN-2`'s own LEN=2 expectation, as a further check.
- `FIRST-PASS-LEN-<n>`, every n: feed pass 2 of the SAME (unmutated)
  capture through the SAME comparator FIRST-PASS-LEN-n uses for pass 1. No
  new stimulus and no changed comparator -- the comparator is expected to
  still accept it (green), since pass 2 is what `LATER-PASSES-LEN-<n>`
  already grades.
- `NO-FETCH-BEYOND-DATA-SET-LEN-<n>`, every n (LEN=4 is the worked
  example): write AUD0LEN<-(n+1) instead of n in that LEN's graded run, and
  grade the mutated run against LEN=n's own forbidden region (offsets
  n..n+15). Expected red.
- `LATER-PASSES-LEN-<n>`, every n: the same AUD0LEN<-(n+1) mutated run used
  for `NO-FETCH-BEYOND-DATA-SET-LEN-<n>`, graded against LEN=n's own
  `LATER-PASSES` expectation. Expected red.
