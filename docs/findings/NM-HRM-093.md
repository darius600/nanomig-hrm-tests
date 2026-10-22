# NM-HRM-093 — at periods 120 to 123 (PAL 120 to 122) audio DMA retrieves every next sample in time, where p. 141 says that below 124 (PAL 123) the previous sample is reused

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this; the question is what p. 141's minimum describes. Does p. 141's "If the period value is below 124, by the time the cycle count has reached 0, the audio DMA will not have had enough time to retrieve the next data sample and the previous sample will be reused" hold at periods 120 to 123, and its PAL "a value of at least 123 ticks/sample must be written" at 120 to 122, where NanoMig retrieves every next sample in time and p. 164's once-a-line transfer, with its "about 14 clock cycles", puts the limit near 121? Closes when an Amiga specialist records that 124 (PAL 123) is the period of the rate p. 141 says the hardware is designed to handle, 28,867 samples/second, and not the period below which the DMA falls behind, or confirms that Paula plays a sample again at 120 to 123, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: an issue on this repository whose title begins with NM-HRM-093 |
| **Scope** | ECS Agnus, NTSC and PAL, 2 MB Chip memory, both CPU cores (fx68k, TG68K) in 68000 mode; audio channel 0 alone, a 64-word table, AUD0VOL = 64; P = 120 to 123 (NTSC) and 120 to 122 (PAL), each over a window of 60,000 ticks; NanoMig's NTSC lines are all 227 colour clocks ([NM-HRM-010](NM-HRM-010.md)); channels 1 to 3, whose slots come later in the line, and OCS are not run; chip revision: unknown |
| **Software impact** | ProTracker's period table, as WinUAE's ProWizard converter lists it ("all the pitch for PTK", [`prowizard/misc/misc.c:496-541`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/prowizard/misc/misc.c#L496-L541)), holds 120, `0x78`; at P = 120 NanoMig retrieves every next sample within the window in both standards, where p. 141 has the previous sample reused; no module was played here |
| **Chapter** | 5 — Audio, printed pp. 140, 141, 164; 2 — Coprocessor Hardware, printed p. 23; Appendix A, printed p. 266 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims by assertion: `MP2 NTSC P=120 [MP2-hi] (a) and (b)` and the same line at P = 121, 122 and 123, each: `sol:5-p141.c10`, `opus:5-rate.c15`, p. 141. Their `(a)` and `(b)` lines and `MP4 PAL P=120`, `MP4 PAL P=121` and `MP4 PAL P=122` carry no claim id |
| **Module** | `paula_audio.v`, `paula_audio_channel.v`, `agnus_audiodma.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio.v#L171-L179) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio.v#L171-L179); inherited (MiSTer has the same code; TC64 the same handover at `rtl/minimig/paula_audio.v:162-170`) |
| **Test** | [`integration/ch5-audio-output-scanrate`](../../integration/ch5-audio-output-scanrate): `make -k -C integration/ch5-audio-output-scanrate test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero |
| **Assertions** | 15 red on both cores, the package's 17 less [NM-HRM-092](NM-HRM-092.md)'s two (2678 passed, 17 failed on each): `MP2 NTSC P=<P> [MP2-hi] (a)`, `(b)` and `(a) and (b)` at P = 120, 121, 122 and 123, and `MP4 PAL P=120`, `MP4 PAL P=121` and `MP4 PAL P=122`. Expected, within W = 60,000 ticks, at least one sample period that does not carry the next sample ((a) and `MP4`) and at least one that plays a sample already played again ((b)); observed every classified sample period next, 499 of 499 at 120, 494 at 121, 490 at 122 and 486 at 123, the same in both standards, with no sample played again. P = 119 is green in both standards (499 of 503 next, 2 played again) |
| **Verdict of the four** | **unanimous with NanoMig on how a word is timed, read, not run**: WinUAE, vAmiga, Denise and Copperline hand Paula's request to Agnus once a line and fetch it at the channel's slot about 13 colour clocks after the transfer, so a word comes late only where twice the period is shorter than about a line and that wait; none applies p. 141's 124 as the period below which a sample is reused: WinUAE's `PERIOD_LOW` of 124 only adds a one-colour-clock delay to the restart flag and the data below it, vAmiga quotes p. 266's minimum in a comment, and Copperline declares 124 and 123 as constants it does not read. Hardware: not measured |
| **Effort** | none — no RTL change asked: the open question is what p. 141's minimum describes |
| **See also** | [NM-HRM-092](NM-HRM-092.md), [NM-HRM-010](NM-HRM-010.md) |

---

## Source requirement

**Printed p. 141**, the minimum period:

> Therefore, a value of at least 124 must be written into the period register to assure that the audio system DMA will be able to retrieve the next data sample. If the period value is below 124, by the time the cycle count has reached 0, the audio DMA will not have had enough time to retrieve the next data sample and the previous sample will be reused.

> 28,867 samples/second is also the maximum sampling rate for PAL systems. Thus, for PAL systems, a value of at least 123 ticks/sample must be written into the period register.

and, earlier on the same page, where the 28,867 comes from:

> The figure of 31,469 is a theoretical maximum. In order to save buffers, the hardware is designed to handle 28,867 samples/second.

**Printed p. 140**:

> The minimum period value you should use is 124 ticks per sample NTSC (123 PAL) and the maximum is 65535.

### What p. 164 says about the timing

> DMA requests and restart requests are transferred to Agnus once each horizontal line, and the data comes back about 14 clock cycles later (the duration of a clock cycle is 280 ns).

> If the sampling rate is set much higher than the normal maximum sampling rate (approximately 29 KHz), the two samples in the buffer register will be repeated.

Read together with the request on each word and a line of 227 colour clocks (228 on every other NTSC line, p. 23), a word requested just after a transfer waits a line and about 14 colour clocks more, and is late only where twice the period is shorter: below about 121. The pages print no bound on "about 14", so this is the mechanism, not graded. 120 to 123 lie at 29.1 to 29.8 kHz in NTSC.

## RTL and measurement

**NanoMig hands a request to Agnus once a line and fetches it 12 colour clocks later.** Paula latches the channel's request on `strhor`, `dmal <= (dmareq);` ([`src/minimig-aga/paula_audio.v#L171-L179`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio.v#L171-L179)), which comes at `assign strhor_paula = hpos==(6*2+1) ? 1'b1 : 1'b0;` ([`src/minimig-aga/agnus.v#L496`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L496)), and Agnus fetches channel 0 at `9'b0001_0010_1 : dmal = audio_dmal[0];` ([`src/minimig-aga/agnus_audiodma.v#L97-L105`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_audiodma.v#L97-L105)): hpos 13 and 37, whose upper bits count colour clocks, `hpos_r[8:1] <= hpos_r[8:1] + 1'b1;` ([`src/minimig-aga/agnus_beamcounter.v#L256-L275`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L256-L275)). The channel asks for the next word when it moves from its low byte to its high byte, and on that same edge takes the holding register into its buffer, `AUDxDR = AUDxON;` and `pbufld1 = 1'b1;` ([`src/minimig-aga/paula_audio_channel.v#L421-L430`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_channel.v#L421-L430)); the requests are 2P colour clocks apart. So a word is in time where 2P is at least a line and those 12 colour clocks, about 240: from P = 120. The module's own header gives channel 0's minimum period as *"121 (120)"* ([`paula_audio.v#L46-L50`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio.v#L46-L50)).

### Measured

[`integration/ch5-audio-output-scanrate`](../../integration/ch5-audio-output-scanrate) plays a 64-word table on channel 0 at each period, every byte a different non-zero value, and cuts channel 0's output in the window into sample periods. Each is classified against the one before it: next, the previous sample again, both buffered samples again, or a skip (`test_ch5_audio_min_period.cpp:557-585`). `MP2` needs, at each period, one that is not next ((a)), one that plays a sample again ((b)), and both ((a) and (b)) (`:840-847`); `MP4` needs one that is not next (`:848-851`). MC6 holds at every one of these periods: the output changes on the P grid. Both standards and both cores print the same counts:

| P | classified, NTSC and PAL | next | played again | verdict |
|---|---:|---:|---:|---|
| 119 | 503 | 499 | 2 | `MP2`, `MP4` green |
| 120 | 499 | 499 | 0 | red |
| 121 | 494 | 494 | 0 | red |
| 122 | 490 | 490 | 0 | red |
| 123 | 486 | 486 | 0 | `MP2` red; PAL: `MP3` green |
| 124 | 482 | 482 | 0 | `MP1`, `MP3` green |

On fx68k:

```text
MEASURED: MP2 NTSC P=120: 500 sample periods, classified 499: next 499, previous sample again (reuse) 0, both buffered samples again (repeat) 0, skip 0, unmapped 0, after an unmapped period 0, other 0
FAIL: MP2 NTSC P=120 [MP2-hi] (a) and (b) (p.141; sol:5-p141.c10, opus:5-rate.c15): at least one sample period does not carry the next sample and at least one sample period plays a sample already played again within W = 60000 ticks (a stimulus choice)
FAIL: MP4 PAL P=120 (p.141): at least one sample period does not carry the next sample within W = 60000 ticks (a stimulus choice)
```

Below 120 the late words appear: at 114 to 119 the output plays the two buffered samples again 1 to 6 times in the window and skips forward 1 to 7 times.

## Risks and unknowns

- **Within the window only.** It holds 3 to 4 passes, every phase of a word against the line, and not every phase at which a restart meets it (`SPECIFICATION-min-period.md`, section 3.3). Read from the RTL lines above, at P = 120 a word can arrive as late as the edge the next request goes out on, where [NM-HRM-092](NM-HRM-092.md)'s restart moves; a window reaching such a restart could hold the first word fetched twice, a sample played again for that reason and not for a late word. `make test` runs no such window.
- **p. 164 is not graded.** Its "about 14" puts the limit at 121 on a 227-colour-clock line; NanoMig's 12 puts it at 120. On p. 23's 228-colour-clock NTSC lines, which NanoMig does not make, a word could wait one colour clock longer.
- **The four were read, not run.** At 122 and 123 their wait is shorter than 2P on any line; at 120 and 121 it lands within a colour clock or two of it.
- **Channel 0 only.** The module header gives channels 1 to 3 minimums of 122, 123 and 124.
- **No hardware was measured.**

## Independent evidence

### What the others say

The question put to each: when does a requested word reach Paula, and does anything apply 124 as the period below which a sample is reused?

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:1547-1558`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1547-L1558), [`custom.cpp:11667-11670`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11667-L11670), [`custom.cpp:9720-9729`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9720-L9729), [`audio.cpp:62`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L62), [`audio.cpp:1619-1624`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1619-L1624) | **once a line, as NanoMig; 124 selects only a finer timing.** Paula's flags are read and cleared once a line, `cdp->dr = cdp->dsr = false;`, in the cycle after the line strobe, `dmal_next = true;`, and the channel is fetched where the DMAL shift reaches it, `if (dmal_shifter & (DMAL_AUD0 << nr)) {`. `#define PERIOD_LOW 124` sets, below 124 (and at any period in CPU-compatible mode), a one-colour-clock delay of the restart flag and of the data, `event2_newevent_xx(-1, 1 * CYCLE_UNIT, nr, event_setdsr);`; no sample is reused because of the period |
| **vAmiga** | [`Core/Components/Agnus/Agnus.cpp:882-886`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L882-L886), [`Core/Components/Agnus/Sequencer/SequencerDas.cpp:36`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Sequencer/SequencerDas.cpp#L36), [`Core/Components/Agnus/AgnusEvents.cpp:737-742`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusEvents.cpp#L737-L742), [`Core/Components/Paula/Audio/StateMachine.h:91-104`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.h#L91-L104) | **once a line.** The requests go to Agnus at the end of each line, `paula.channel0.requestDMA();`, and channel 0 is fetched at `p[0x0D] = DAS_A0;` of the next, its word handed to Paula at once, `paula.channel0.pokeAUDxDAT(doAudioDmaRead<0>());`. The minimum appears only in a comment quoting p. 266, *"The minimum period is 124 color clocks."*, above two locks that keep identical samples out of its host sample buffer |
| **Denise** | [`emulation/libami/agnus/events.cpp:401-402`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L401-L402), [`emulation/libami/agnus/agnus.cpp:542-546`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L542-L546) | **once a line.** Agnus takes Paula's DMAL at the line strobe, *"DMAL is fetched serial bit by bit (14 cycles)."*, `dmal = paula.dmal();`, and fetches channel 0 at `case 0x11:`, `fetchSample<0>( dmal & 0x1 );`. No period limit |
| **Copperline** | [`src/bus/dma_slots.rs:328-336`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L328-L336), [`:1063-1066`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L1063-L1066), [`src/chipset/paula.rs:18-19`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L18-L19) | **once a line.** The requests are taken at each line end, `self.paula.transfer_audio_dma_requests();`, *"real Paula transfers them once per line; the fixed slots service them on the following line"*, and channel 0 is served at `0x00D => Some(0),`. `pub const PAL_AUDIO_MIN_PERIOD_CCK: u16 = 123;` and `pub const NTSC_AUDIO_MIN_PERIOD_CCK: u16 = 124;` are read nowhere in its sources |
| **MiSTer** | [`rtl/paula_audio.v:171-179`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio.v#L171-L179), [`rtl/agnus.v:483`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L483), [`rtl/agnus_audiodma.v:97-105`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L97-L105) | descendant — identical to NanoMig: `dmal <= (dmareq);` on `strhor`, `assign strhor_paula = hpos==(6*2+1) ? 1'b1 : 1'b0;`, and channel 0 at `9'b0001_0010_1 : dmal = audio_dmal[0];` |
| **TC64** | [`rtl/minimig/paula_audio.v:162-170`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio.v#L162-L170), [`rtl/minimig/agnus.v:513`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L513), [`rtl/minimig/agnus_audiodma.v:97-105`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_audiodma.v#L97-L105) | descendant — the same handover, strobe and slot |

**All four time the word as NanoMig does and as p. 164 describes**, read from source: one transfer a line and a fetch about 13 colour clocks after it, so none of them falls behind at 122 or 123, and none reads p. 141's 124 as a limit; at 120 and 121 the reading does not decide them. MiSTer and TC64, descendants, carry NanoMig's code.

## Decision

**No RTL change is asked.** The page asks what p. 141's 124 is: the period of the 28,867 samples/second the hardware *"is designed to handle"*, which p. 140 gives as the minimum "you should use", or the period below which Paula's DMA falls behind. A 64-word table played on an Amiga at P = 120 to 123 with its output sampled, or its fetch addresses logged against the line, would settle it: by p. 141 a sample is played again at each of these periods, by NanoMig at none, and p. 164's timing, with its "about 14", puts the limit between them, near 121. If a specialist confirms the reuse, this page is refiled against the RTL, with the twelve `MP2` and three `MP4` lines as the reds a change to NanoMig's request-to-data path would have to turn green.
