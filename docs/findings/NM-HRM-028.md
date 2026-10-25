# NM-HRM-028 — ADKCON's audio attach bits are stored and reach nothing [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `paula.v` bring `adkcon[7:0]` (ATVOL0–3, ATPER0–3) to the four audio channels, so that a channel whose attach bit is set stops producing sound and its words modulate the next channel's volume or period, as pp. 149–151 print, where NanoMig's `ADKCON = 0x80FF` leaves both channel peaks at 8001 instead of 0? It closes when the attach logic is in and `ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS` and the 37 integration reds listed under *Where else the suite meets it* are green on both cores, or when the specialist declines and the omission is documented at the register map, with the reason recorded here |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | Paula's four audio channels and the eight attach bits, ADKCON bits 0–7; the hrm package sets all eight from reset, the integration package one pair or one bit at a time. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 5 — Audio, printed pp. 149–151 (Table 5-5, p. 151) and p. 164, and Appendix A, printed p. 265 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (130)</summary> claim ids by assertion. `ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS`: `5-p149.c12`, `5-mod.c6` (p. 149); `5-p150.c24`, `5-p150.c27`, `5-mod.c21`, `5-mod.c24` (p. 150); `5-p151.c1`, `5-p151.c5`, `5-t55.b7`, `5-t55.b3` (p. 151); and the chapter-8 ids its message also prints: `opus:8-adkcon.c30`, `sol:8-adkcon.c26`, `sol:8-adkcon.c27` (p. 249); `opus:8-appA-adkcon.c28`, `opus:8-appA-adkcon.c29`, `opus:8-appA-adkcon.c30`, `opus:8-appA-adkcon.c31`, `opus:8-appA-adkcon.c32`, `opus:8-appA-adkcon.c33`, `opus:8-appA-adkcon.c34`, `opus:8-appA-adkcon.c35`, `opus:8-appA-adkcon.c36`, `sol:8-appa-adk.c4`, `sol:8-appa-adkr.c4`, `sol:8-appa-adkbit.c22`, `sol:8-appa-adkbit.c23`, `sol:8-appa-adkbit.c24`, `sol:8-appa-adkbit.c25`, `sol:8-appa-adkbit.c26`, `sol:8-appa-adkbit.c27`, `sol:8-appa-adkbit.c28`, `sol:8-appa-adkbit.c29`, `sol:8-appa-adkbit.c30`, `sol:8-appa-adkbit.c31`, `sol:8-appa-adkbit.c32` (p. 265); `sol:8-appb-adkconr.c5` (p. 289); `sol:8-appb-adkcon.c4` (p. 290). `P1-A`: `5-hw.c3`, `5-p136.c3` (p. 136); `5-p149.c5`, `5-mod.c3`, `5-p149.c7` (p. 149); `5-mod.c15`, `5-p150.c2`, `5-p150.c8`, `5-p150.c12`, `5-p150.c16`, `5-p150.c20`, `5-t54.w1.c4`, `5-t54.w2.c4`, `5-t54.w3.c4`, `5-t54.w4.c4` (p. 150); `5-p151.c8`, `5-t55.b0` (p. 151); `5-appA-p265.c29`, `A-adkcon.b00` (p. 265). `P2-A`: `5-mod.c4`, `5-p149.c8`, `5-p149.c4`, `5-mod.c13`, `5-p149.c19` (p. 149); `5-mod.c16`, `5-p150.c3`, `5-p150.c7`, `5-p150.c11`, `5-p150.c15`, `5-p150.c19`, `5-t54.w1.c3`, `5-t54.w2.c3`, `5-t54.w3.c3`, `5-t54.w4.c3` (p. 150); `5-p151.c4`, `5-t55.b4` (p. 151); `5-appA-p265.c25`, `A-adkcon.b04` (p. 265). `P2-JOINT`: `5-p149.c9` (p. 149). `P3-JOINT`: `5-mod.c8`, `5-mod.c9`, `5-p149.c14`, `5-p149.c15` (p. 149). `P4-A`: `5-mod.c2`, `5-p149.c6` (p. 149); `5-mod.c15`, `5-mod.c16`, `5-p150.c2`, `5-p150.c3`, `5-mod.c17`, `5-p150.c4`, `5-p150.c6`, `5-p150.c10`, `5-p150.c14`, `5-p150.c18`, `5-t54.w1.c2`, `5-t54.w2.c2`, `5-t54.w3.c2`, `5-t54.w4.c2` (p. 150); `5-appA-p265.c30`, `5-appA-p265.c32`, `A-adkcon.note1` (p. 265). `P4-JOINT`: `5-mod.c14`, `5-p150.c1` (p. 150). `P5-AP`: `5-p165.c18`, `5-sig.pbufld2` (p. 165). `P5-BOTH`: `5-mod.c10`, `5-p149.c16` (p. 149). `P6-A` bit 1: `5-p151.c7`, `5-t55.b1` (p. 151), `5-appA-p265.c28`, `A-adkcon.b01` (p. 265); bit 2: `5-p151.c6`, `5-t55.b2` (p. 151), `5-appA-p265.c27`, `A-adkcon.b02` (p. 265); bit 5: `5-p151.c3`, `5-t55.b5` (p. 151), `5-appA-p265.c24`, `A-adkcon.b05` (p. 265); bit 6: `5-p151.c2`, `5-t55.b6` (p. 151), `5-appA-p265.c23`, `A-adkcon.b06` (p. 265). `P6-JOINT`: `5-mod.c20`, `5-p150.c23` (p. 150). `P7-JOINT-V`: `A-adkcon.b03`; `P7-JOINT-P`: `A-adkcon.b07` (p. 265). `P10-AP`: `5-p164.c34`, `5-sm.c34` (p. 164), `F58.loop.attach` (p. 166). `P10-BOTH`: `5-p164.c35`, `5-sm.c35` (p. 164). No claim id: `P1-B`, `P1-C`, `P1-JOINT`, `P2-B`, `P3-SILENT`, `P3-WHOLE`, `P3-SP2`, `P4-B`, `P4-C`, `P5-AV`, `P6-A` bits 0 and 4, `P8-ATT` for `S-AV`, `S-AP`, `S-BOTH`, `S-DAC3-V` and `S-DAC3-P`, `P8-JOINT-AP`, `P9-C` for `S-LEN-A` and `S-LEN-B`, and the green control `ADKCON-ATTACH-MEASUREMENT-BASIS-CONTROL` </details> |
| **Module** | `paula.v`, `paula_audio_channel.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_channel.v#L1) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L1); inherited |
| **Test** | [`hrm/ch5-audio-dmacon-adkcon`](../../hrm/ch5-audio-dmacon-adkcon): `make -C hrm/ch5-audio-dmacon-adkcon test`, `paula.v` alone, no CPU core, the modulator's own output. [`integration/ch5-audio-attach-modulation`](../../integration/ch5-audio-attach-modulation): `make -k -C integration/ch5-audio-attach-modulation test`, programs on `nanomig.v` through the CPU on fx68k and TG68K, graded at `audio_left` and `audio_right` and at the reads of each channel's data on the RAM port. An expected red exits nonzero |
| **Assertions** | hrm: 1 red, `ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS` — expected both peaks 0 with `ADKCON = 0x80FF`, observed `ldata` and `rdata` peaks 8001; the package reports `19 passed, 2 failed`, the other red being `DMACONR-NOT-DECODED-BY-PAULA`, which is [NM-HRM-030](NM-HRM-030.md)'s. The integration package is red the same way on fx68k and TG68K, 37 lines on each core (`174 passed, 37 failed`), listed under *Where else the suite meets it*. |
| **Verdict of the four** | **all four route a modulator's words into the next channel, and on the package's stimulus all four leave the modulators silent**; they split 2-2 on p. 150's rule that either attach bit disables the channel's output: WinUAE ([`audio.cpp:2761-2769`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L2761-L2769)) and Copperline ([`paula.rs:2326-2328`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2326-L2328)) mute a modulator's output, while vAmiga ([`StateMachine.cpp:118-135`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L118-L135)) and Denise ([`audio.cpp:158-165`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L158-L165)) only stop loading its output buffer under a volume attach, so a period-only modulator still plays its words. Hardware: a third-party dissection of one Paula revision records a modulation source's DAC as muted, as the manual prints |
| **Effort** | M — `paula.v` brings `adkcon[7:0]` into `paula_audio` and down to each channel; `paula_audio_channel.v` suppresses the sample output and routes the `AUDxDAT` word to the neighbouring channel's period or volume register. These steps do not describe the request and load timing of an attached channel (pp. 164–165) that `P5-AP`, `P5-BOTH`, `P10-AP` and `P10-BOTH` grade |

---

## Source requirement

**Printed pp. 149–151** describe modulating one channel with another. With an
attach bit set, the modulating channel's own output is silenced and its words are
used as period or volume data for the next channel:

> "If the 'attach' bit for an audio channel is set, that channel ceases to
> produce sound…"

> "If you set either the volume or period attach bits for a channel, that
> channel's audio output will be disabled…"

The words are used whole, and each one lands when the modulator's period
times out (p. 149):

> "When a channel is used as a modulator, the words in its data set are no
> longer treated as two individual bytes. Instead, they are used as
> 'modulator' words. The data words from the modulator channel are written
> into the corresponding registers of the modulated channel each time the
> period register of the modulator channel times out."

**Printed p. 164** puts an attached channel's requests on the transitions of
its state machine:

> "In attach volume, requests occur as they do in normal operation (on the
> 011->010 transition). In attach period, a set of requests occurs on the
> 010->011 transition. When both attach period and attach volume are high,
> requests occur on both transitions."

**Appendix A, printed p. 265** gives `ADKCON` bits 7–0 as `USE3PN`–`USE0P1` and
`USE3VN`–`USE0V1`; Table 5-5 on printed p. 151 names the same bits `ATPER3–0`
and `ATVOL3–0`.

## RTL and measurement

NanoMig stores the bits and wires none of them to the audio path. `paula.v`
instantiates the audio block with no ADKCON port; `adkcon[14:0]` is latched and
read back through `adkconr`, and only bits 11 (`uartbrk`) and 10 (`wordsync`)
leave the register, both disk and UART. The channel module says so in its first
line, [`paula_audio_channel.v#L1`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_audio_channel.v#L1):

```verilog
//This module handles a single amiga audio channel. attached modes are not supported
```

This is an absent feature, of the same character as [NM-HRM-015](withdrawn/NM-HRM-015.md)
and [NM-HRM-021](NM-HRM-021.md): declared in a register, accepted by the decode,
reaching nothing.

**Measured, hrm package.** With every attach bit set, `ADKCON = 0x80FF`, a channel
armed for a peaked output still peaks:

| | |
|---|---|
| `ldata` peak | 8001 |
| `rdata` peak | 8001 |
| expected if the manual held | 0 and 0 |

8001 is sample 127 × effective volume 63, what this path gives an unmodulated
channel at `AUDxVOL = 64`, which it treats as 63 ([NM-HRM-025](NM-HRM-025.md)).
`ADKCON-ATTACH-MEASUREMENT-BASIS-CONTROL` sets `AUDxVOL = 0` instead of the attach
bits on the same stimulus and reads 0, so the peak measurement does report silence.

### Where else the suite meets it

`integration/ch5-audio-attach-modulation`: 37 red, the same lines on fx68k and TG68K (174 passed, 37 failed on each core):

- **a modulator's words reaching the next channel's volume or period**, each line needing words applied to it: `P1-A`, `P1-B` and `P1-C`, channel 0's volume words in S-AV; `P2-A`, `P2-B` and `P2-JOINT`, its period words in S-P2, `P2-JOINT` joining `P2-A` to the waveform half `P2-C`, which is green; `P4-A`, `P4-B`, `P4-C` and `P4-JOINT`, words alternately volume and period in S-BOTH, `P4-JOINT` joining `P4-A` to `P4-VOLONLY`, which is green; `P6-A` for each of the six attach bits 0, 1, 2, 4, 5 and 6, and `P6-JOINT`, the six bits in one line (bits 0 and 4 from `P1-A` and `P2-A`); `P9-C` in S-LEN-A and S-LEN-B, the modulator's data set once longer and once shorter than the modulated channel's;
- **the words used whole, not as two bytes**: `P3-SILENT`, `P3-WHOLE` and `P3-SP2`, and `P3-JOINT` across the volume and period modes, each needing a word interval or a period run that a modulator word applied;
- **a word at each time-out of the modulator's period**: `P5-AV`, `P5-AP` and `P5-BOTH`, each needing at least eight applied changes on the time-out grid of p. 164;
- **the modulator's own output**: `P8-ATT` for `S-AV`, `S-AP`, `S-BOTH`, `S-DAC3-V` and `S-DAC3-P`; the joint lines `P1-JOINT` and `P8-JOINT-AP`; and `P7-JOINT-V` and `P7-JOINT-P`, each putting channel 3's attach bit modulating nothing (`P7-CONST`, `P7-SPACING`, both green) and disabling channel 3's output (`P8-ATT` for `S-DAC3-V` and `S-DAC3-P`) into one line;
- **the request timing of an attached channel**: `P10-AP` and `P10-BOTH`.

Expected the next channel's volume or period to follow the modulator's words and the modulator's own side to hold one value; observed the next channel keeping the volume and period the program wrote, 0 applied changes in S-AV, S-P2, S-BOTH, S-AP, the six S-PAIR sessions, S-LEN-A and S-LEN-B, and the modulator's own output playing its words. `P10-AP` expected every S-AP fetch less than one line (1816 clk) from R-MOD's phase plus D_mod (32000 clk) and measured one 31992 clk from it, 8 clk from R-MOD's own phase; `P10-BOTH` expected S-BOTH's fetches D_mod apart from F_3 on and measured consecutive fetches that are not.

**Measured, integration package.** The hrm package asserts only the half whose
stimulus and consequence are both on real ports of `paula.v`: write `ADKCON`,
observe that the modulating channel is not silenced. A modulator's word reaching
another channel's period or volume can show only in that channel's output, which
the integration package grades on `nanomig.v`'s audio ports. Its reference
captures (`PC4-VOL`, `PC4-DUR`) and side assignment (`PC5-VOL`, `PC5-PER`) are
green on both cores, so the reds are read on a basis shown to work. So is each
session's setup: the stored attach mask, the DMA enable and every enabled
channel's fetch stepping (`PC1`, `PC2`, `PC3`) and the windows (`P1-WINDOW`,
`P9-WINDOW`) are green, so in every attached session the modulator's words are
read from memory.

## Risks and unknowns

- Real Paula's attach behaviour is not measured here. The one silicon reading is a third-party dissection of the 8364R7, one revision.
- The four split 2-2 on p. 150's either-bit rule: in vAmiga and Denise a period-only modulator still plays its words, and a volume attach set while a channel holds a word keeps that word playing.
- MiSTer and TC64 repeat NanoMig's absence, so they are one implementation counted three times, not further votes against the manual.

## Independent evidence

### What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:2761-2769`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L2761-L2769)<br>[`audio.cpp:592`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L592)<br>[`audio.cpp:1640-1657`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1640-L1657) | a per-channel mask built from both attach fields, zero when either is set, applied to the channel's output, `int output = (acd->current_sample *vol) & acd->adk_mask;`: **either attach bit mutes the modulator**, as p. 150 prints. `loaddat` routes its word to the next channel's period or volume |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachine.cpp:118-135`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L118-L135)<br>[`Core/Components/Paula/Audio/StateMachine.cpp:149-195`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L149-L195) | the modulator's word is written into the **next** channel's volume or period. **The modulator is not muted**: `penhi` and `penlo` send its `buffer` to the sampler whatever ADKCON holds, `i16 scaled = (i16)(sample * audvol);`. Under a volume attach `buffer` is never reloaded, so the modulator keeps its last word, zero from reset; under a period attach alone `buffer = auddat` still runs and the period words play as samples |
| **Denise** | [`emulation/libami/paula/audio.cpp:79-81`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L79-L81)<br>[`emulation/libami/paula/audio.cpp:158-165`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L158-L165)<br>[`emulation/libami/paula/paula.cpp:354-355`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L354-L355) | `pbufld1` routes the word to the next channel's volume under `audav` and otherwise loads it, `cha.buffer = cha.dat;`. **The modulator is not muted**: `penhi`/`penlo` put the buffer on the channel's output whatever ADKCON holds, and the mixer adds every channel, `channels[0].sample + channels[3].sample`. As in vAmiga, a volume attach leaves the last word, zero from reset, and a period attach alone plays the words |
| **Copperline** | [`src/chipset/paula.rs:1190-1192`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1190-L1192)<br>[`src/chipset/paula.rs:2326-2328`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2326-L2328)<br>[`src/chipset/paula.rs:1817-1832`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1817-L1832) | the mixer returns 0 for any channel attached as a modulator, `if self.channel_muted[ch_idx] \|\| self.channel_attached_as_modulator(ch_idx) {`: **either attach bit mutes the modulator**, as p. 150 prints. `aud_pbufld1` and `aud_pbufld2` route its word to the next channel's volume or period |
| **MiSTer** | [`rtl/paula_audio_channel.v:1`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L1) | descendant — carries the identical first-line comment, *"attached modes are not supported"*. No ADKCON port on the audio-channel module here either |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:1`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L1) | descendant — carries it too, word for word, even though the rest of the file has drifted by about 29 lines (see [NM-HRM-025](NM-HRM-025.md)/[NM-HRM-026](NM-HRM-026.md)). Neither MiSTer's nor TC64's audio-channel module has an ADKCON port either, and neither `paula.v` wires one in |

**All four route a modulator's words into the next channel. NanoMig does not**,
and its own source says so.

### What other Commodore documents say

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — agrees with the manual:

  > "nevertheless when a channel (including channel 3) is enabled as a modulation source, the DAC of the channel which is the source is disabled/muted."
  <!-- corpus: paula-8364r7-dissection/7_7_txt.txt -->

  On the 8364R7 die a channel enabled as a modulation source has its DAC muted, channel 3 included although its modulation outputs are open traces: the manual's attached channel that stops producing sound. 6_6 adds 'ADKCON controls the rest of the Paula functionality: audio channel modulation,'.

## Decision

Attach is a feature, not a bug fix, and adding it is real work: the modulating
channel must be silenced, and its fetched word must be routed into the next
channel's `AUDxPER` or `AUDxVOL` instead of its own sample path. vAmiga's
`pbufld1`/`pbufld2` split is the clearest small model of the routing, and
Copperline's `channel_mixed_sample` of the silencing; vAmiga does not silence.

Minimum shape in this RTL:

1. bring `adkcon[7:0]` into `paula_audio` and down to each channel;
2. in a channel whose attach bit is set, suppress the sample output;
3. route its `AUDxDAT` word to the neighbouring channel's period or volume
   register at the point the word would otherwise be consumed.

**The honest alternative is to document the omission rather than close it.** The
first line of `paula_audio_channel.v` already does, and a core that never
attaches is a defensible scope decision; it is only undocumented at the level of
the register map, where `ADKCON` accepts the bits and reads them back as though
they did something.
