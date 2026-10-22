# NM-HRM-092 — below the minimum period a pass of audio DMA does not always start again at AUDxLC: channel 0 reads past its table, fetches the first word twice or ends a pass one word short, where pp. 139 and 147 restart it each time [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should audio channel 0, at a period below the minimum, still fetch its data in passes of exactly AUD0LEN words that each start at AUD0LC, as p. 139's "Each time the system retrieves the last audio word from the data area, it uses the contents of these location registers to again find the start of the data" says, where NanoMig with a 64-word table reads past the table at P = 110 and 116, fetches the first word twice at 110 and 118, and at 118 ends the pass after that at T+124? The question is the restart only, not the late sample p. 141 predicts below 124 ([NM-HRM-093](NM-HRM-093.md)). Closes when the restart no longer moves with a late word and `MP6 NTSC` and `MP6 PAL` of `integration/ch5-audio-output-scanrate` are green on both cores, or when an Amiga specialist rejects this reading of pp. 139 and 147 below the minimum period, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | ECS Agnus, NTSC and PAL, 2 MB Chip memory, both CPU cores (fx68k, TG68K) in 68000 mode; audio channel 0 alone, a 64-word table at $2000, AUD0VOL = 64; every period from 1 to 123 and fourteen from 124 to 428 (137 runs per standard), each read over a window of 60,000 ticks, 2 to 3 complete passes at the failing periods; red at P = 110, 116 and 118 in both standards; channels 1 to 3, attach modes and OCS are not run; chip revision: unknown |
| **Software impact** | ProTracker's period table, as WinUAE's ProWizard converter lists it ("all the pitch for PTK", [`prowizard/misc/misc.c:496-541`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/prowizard/misc/misc.c#L496-L541)), goes below 124 at its two lowest periods, `0x78` and `0x71` (120 and 113); MP6 is green at both within its window, and no module was played here |
| **Chapter** | 5 — Audio, printed pp. 139, 140, 141, 147, 164, 166 (Figure 5-8) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · `MP6 NTSC` and `MP6 PAL` carry no claim id |
| **Module** | `paula_audio_channel.v`, `agnus_audiodma.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_channel.v#L217-L226) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L217-L226); inherited (MiSTer has the same code in both files and in `paula_audio.v`; TC64 the same request logic at `rtl/minimig/paula_audio_channel.v:194-203`, its channel file lacking only NanoMig's PWM-gated sample output) |
| **Test** | [`integration/ch5-audio-output-scanrate`](../../integration/ch5-audio-output-scanrate): `make -k -C integration/ch5-audio-output-scanrate test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero |
| **Assertions** | `MP6 NTSC` and `MP6 PAL` red on both cores, two of the package's 17 reds on each (2678 passed, 17 failed; the other fifteen are [NM-HRM-093](NM-HRM-093.md)'s): expected, in every run, channel 0's reads at its slot offset in passes of exactly 64 words, T to T+126 in order, each followed by T; observed at P = 110, 116 and 118, the same in both standards and on both cores, reads past T+126 (110, 116), the first word read twice in a row (110, 118) and a pass that ends at T+124 (118). The other 134 runs of each standard are green on the same comparator |
| **Verdict of the four** | **split 2–2, read, not run**: vAmiga and Copperline reload the pointer from AUDxLC when the word that finds the length counter at one comes in, so every pass is AUDxLEN words from AUDxLC at any period; WinUAE and Denise, like NanoMig and Figure 5-8, send the restart with the request that goes out while the counter stands at one and count the words that come in, so a late word can move their restart too; unlike NanoMig they keep the restart and the plain request as two flags. Hardware: not measured |
| **Effort** | S — the restart Agnus applies at channel 0's fetch (`agnus_audiodma.v` line 138) taken from the length counter as it stands at that fetch (`lenfin`, `paula_audio_channel.v` line 187) instead of from the bit stored with the request (line 220); see *Decision*; not run here |
| **See also** | [NM-HRM-093](NM-HRM-093.md), [NM-HRM-084](NM-HRM-084.md) |

---

## Source requirement

**Printed p. 139**, on the location registers:

> These registers are location registers, as distinguished from pointer registers. You need to specify the contents of these registers only once; no resetting is necessary when you wish the audio channel to keep on repeating the same waveform. Each time the system retrieves the last audio word from the data area, it uses the contents of these location registers to again find the start of the data.

**Printed p. 147**, on joining tones:

> If you do not rewrite the registers, the current waveform will be repeated. Each time the length counter reaches zero, both the location and length registers are reloaded with the same values to continue the audio output.

Neither page names an exception for the period. The three failing periods lie below the minimum of p. 140 and p. 141, 124 (PAL 123), where p. 141 has the DMA fall behind ([NM-HRM-093](NM-HRM-093.md)); no page says that the restart then fails.

### How the pages request the restart

**Printed p. 164**:

> The length counter counts down once with each word that comes in. When it finishes, a DMA restart request goes to Agnus along with the regular DMA request. This tells Agnus to reset the pointer to the beginning of the table of data.

> DMA requests and restart requests are transferred to Agnus once each horizontal line, and the data comes back about 14 clock cycles later (the duration of a clock cycle is 280 ns).

Figure 5-8, p. 166, *read from the drawing, not quoted*, prints the same in its NOTE: "Except for this case, dmasen is true only when LENFIN=1. Also, AUDxDSR=AUDxDR · dmasen".

## RTL and measurement

**NanoMig decides the restart when it asks for a word, and counts the length when a word comes in.** The request stores the restart bit, `dmas <= dmasen | lenfin;` ([`src/minimig-aga/paula_audio_channel.v#L217-L226`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_channel.v#L217-L226)), from `assign lenfin = (lencnt[15:0]==1 && cck) ? 1'b1 : 1'b0;` ([`#L187`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_channel.v#L187)); in the main loop the counter counts, or reloads, on the word's arrival, `lencount = ~lenfin & AUDxON & AUDxDAT;` and `lencntrld = lenfin & AUDxON & AUDxDAT;` ([`#L391-L393`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_channel.v#L391-L393), [`#L415-L417`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_channel.v#L415-L417)). Paula hands the request to Agnus once a line, `dmal <= (dmareq);` and `dmas <= (dmaspc);` on `strhor` ([`src/minimig-aga/paula_audio.v#L171-L179`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio.v#L171-L179)), and Agnus fetches at channel 0's slot and then reloads the pointer or steps it, `audpt[channel] <= #1 dmas ? audlcout[20:1] : audptout[20:1] + 1'b1;` ([`src/minimig-aga/agnus_audiodma.v#L135-L140`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_audiodma.v#L135-L140)).

**Below the minimum period a word can come in after the next request has gone out**, and the counter is then one count behind at that request (the output of the same runs has 7, 6 and 5 sample periods at P = 110, 116 and 118 that play the buffered samples again where the next was due):

- if that request is the one that should carry the restart, it goes out without it: T+126 is fetched as an ordinary word, the pointer steps to T+128, and the channel reads on past the table until the counter next stands at one (P = 110 and 116);
- if the late word was the restart's own, the next request goes out with the counter still at one and carries a second restart: T is fetched twice, both count, and the pass after it ends at T+124 (P = 118, and the double read at P = 110);
- at P = 110 two requests can also fall into one line (2P = 220, a line 227): Agnus fetches once for both, and the second request rewrites the first one's restart bit (line 220).

The module's header gives channel 0's minimum period as *"121 (120)"*, for the case it calls *"no length reload"* ([`paula_audio.v#L46-L50`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio.v#L46-L50)).

### Measured

[`integration/ch5-audio-output-scanrate`](../../integration/ch5-audio-output-scanrate) writes a 64-word table at T = $2000, every byte a different non-zero value, then AUD0LC = T, AUD0LEN = 64, AUD0VOL = 64, AUD0PER = P and DMACON = $8201, and waits by the beam; the only registers it reads are DMACONR, ADKCONR, VPOSR and VHPOSR, and every period is its own simulation. `MP6` takes channel 0's reads at its slot offset in the port window, at any address, and requires each read to follow the one before it, T+2i by T+2(i+1) and T+126 by T (`test_ch5_audio_min_period.cpp:488-522`); it is one line per standard over all of its runs (`:525-537`). Times are ticks after the start of the port window:

| P | reads past T+126 | first word read twice | pass ending early | `MP6` |
|---|---|---|---|---|
| 110 | 63, the first T+128 at 28,829 | T+0 at 14,301 | — | red |
| 116 | 5, the first T+128 at 59,020 | — | — | red |
| 118 | — | T+0 at 14,982 | T follows T+124 at 29,737 | red |
| the other 134 | none | none | none | green |

On fx68k, NTSC; PAL and TG68K print the same lines:

```text
MEASURED: MP6 NTSC P=118: 255 reads at the channel offset, 3 complete passes; off the table 0; read twice in a row 1 (first: T+0 read twice in a row at 14982.0 ticks after the start of the port window); short passes 1 (first: T follows T+124 at 29737.0 ticks after the start of the port window); long passes 0; other gaps 0
MEASURED: MP6 NTSC: 137 runs graded, 525 complete passes in their windows, 3 runs fail
FAIL: MP6 NTSC (p.139, p.147): in every run channel 0's reads at its slot offset form passes of exactly 64 words, T to T+126 in order, each followed by T; periods that fail: 110, 116, 118
```

What lies past the table is played: at P = 110 the output of the same run holds 130 sample periods whose level is no value of the table, and at 116 seven.

## Risks and unknowns

- **The walk from the lines to the three periods is a reading of the RTL.** The test prints the addresses, not the length counter or the request bits.
- **NanoMig's arrangement is Figure 5-8's.** The restart goes with the request made while LENFIN = 1, and p. 164 counts the words that come in; read with p. 164's once-a-line transfer, a late word leaves the counter behind at the next request as above. No page says what then becomes of the restart, and a reader may hold that pp. 139 and 147 promise nothing below the minimum of pp. 140 and 141.
- **Within the window only.** At the failing periods a run holds 2 or 3 complete passes, and the phases at which a restart can meet the line need 227 (`SPECIFICATION-min-period.md`, section 3.3); a green period is green within its window.
- **WinUAE and Denise were read, not run**: whether their own timing makes them read past a table is not measured.
- **Not graded:** channels 1 to 3, whose slots lie later in the line, and OCS. No hardware was measured.

## Independent evidence

### What the others say

The question put to each: is the pointer reloaded with the request that goes out while the length counter stands at one, or when the word that finds it at one comes in?

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:1619-1631`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1619-L1631), [`:2537-2551`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L2537-L2551), [`:1547-1558`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1547-L1558), [`custom.cpp:11826-11838`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11826-L11838) | **with the request, as NanoMig.** A request made while the counter stands at one sets the restart flag, `if (!startup && cdp->wlen == 1) {`, any other the plain one, `cdp->dr = true;`; the counter counts at the word's arrival, `cdp->wlen = (cdp->wlen - 1) & 0xffff;`, and reloads there, `cdp->wlen = cdp->len;`. Both flags go to Agnus once a line and are cleared, `cdp->dr = cdp->dsr = false;`, and the fetch that carries the restart loads the location, `pt = audio_getloadpt(num);`. They are two flags: a second request in the same line does not clear the restart |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachineRegs.cpp:69-78`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachineRegs.cpp#L69-L78), [`Core/Components/Agnus/Agnus.h:683`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L683), [`Core/Components/Agnus/AgnusDma.cpp:98-112`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L98-L112), [`Core/Components/Agnus/AgnusEvents.cpp:737-742`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusEvents.cpp#L737-L742) | **when the last word comes in.** The fetch steps the pointer, `audpt[channel] += 2;`, and hands the word to Paula at once, `paula.channel0.pokeAUDxDAT(doAudioDmaRead<0>());`; the word that finds the counter at one reloads the counter and the pointer, `lencntrld();` and `AUDxDSR();`, which is `audpt[x] = audlc[x];`. Every pass is AUDxLEN fetched words and starts at AUDxLC, however late a word comes |
| **Denise** | [`emulation/libami/paula/audio.cpp:5-7`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L5-L7), [`:89-96`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L89-L96), [`emulation/libami/paula/paula.cpp:31-46`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L31-L46), [`emulation/libami/agnus/dma.cpp:328-341`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L328-L341) | **with the request, as NanoMig.** `#define dmasen() if (cha.len == 1) cha.dsr = true;`, else `cha.dr = true;`; the counter counts at the word's arrival, `lencount()` or `lencntrld()` in states 2 and 3. Paula's flags go to Agnus once a line and are cleared, `cha.dsr = false;` and `cha.dr = false;`, and the fetch that carries the restart loads the latch, `cha.ptr = cha.ptrLatch;`. Two flags, as in WinUAE |
| **Copperline** | [`src/chipset/paula.rs:1962-1972`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1962-L1972), [`:1809-1812`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1809-L1812), [`:2107-2120`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2107-L2120) | **when the last word comes in, as vAmiga.** The grant steps the pointer and feeds the word to the state machine, `ch.ptr = ch.ptr.wrapping_add(2) & ptr_mask;` and `let irq = self.aud_poke_dat(ch_idx, word, dmacon);`; the final word reloads, `self.aud_lencntrld(ch);` and `self.aud_dsr(ch);`, which is `self.chans[ch].ptr = self.chans[ch].lc & self.dma_ptr_mask();`. Its comment: *"Steady-state fetch: count the word; on the final one reload the length/pointer and arm the rollover interrupt for the next word-start transition."* |
| **MiSTer** | [`rtl/paula_audio_channel.v:217-226`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L217-L226), [`rtl/paula_audio.v:171-179`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio.v#L171-L179), [`rtl/agnus_audiodma.v:135-140`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_audiodma.v#L135-L140) | descendant — identical to NanoMig in all three files: `dmas <= dmasen \| lenfin;` with each request, `dmal <= (dmareq);` once a line, and `audpt[channel] <= #1 dmas ? audlcout[20:1] : audptout[20:1] + 1'b1;` at the fetch |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:194-203`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L194-L203), [`rtl/minimig/paula_audio.v:162-170`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio.v#L162-L170), [`rtl/minimig/agnus_audiodma.v:135-140`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_audiodma.v#L135-L140) | descendant — the same request register, `dmas <= dmasen \| lenfin;`, the same handover, `dmal <= (dmareq);`, and the same fetch, `audpt[channel] <= #1 dmas ? audlcout[20:1] : audptout[20:1] + 1'b1;` |

**The four split two and two.** vAmiga and Copperline tie the restart to the arrival of the word that ends the pass, so their passes are AUDxLEN words from AUDxLC whatever the period; WinUAE and Denise keep Figure 5-8's arrangement, the restart sent with the request, and with it the dependence on when the words come in. MiSTer and TC64, descendants, carry NanoMig's code, so the finding is inherited and fixed in neither line.

## Decision

**One option: take the restart from the length counter at the fetch, as vAmiga and Copperline do.** Agnus applies the restart at channel 0's slot (`agnus_audiodma.v` line 138); let it be `lenfin` as the counter stands then (`paula_audio_channel.v` line 187), joined to the start-up restart that `dmasen` stores with the first request, instead of the bit stored when each request went out (line 220). Every word fetched comes in and is counted before the next fetch, so the fetch made while the counter stands at one is always the last word of the pass. What it would change:

- by this reading `MP6 NTSC` and `MP6 PAL` would turn green on both cores; the late samples below 124 stay, so the `MP2` and `MP4` lines would not change;
- NanoMig would move away from Figure 5-8's "AUDxDSR=AUDxDR · dmasen" and from WinUAE and Denise, towards vAmiga and Copperline;
- not run here.
