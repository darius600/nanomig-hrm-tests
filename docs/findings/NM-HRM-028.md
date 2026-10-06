# NM-HRM-028 — ADKCON's audio attach bits are stored and reach nothing [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula.v`, `paula_audio_channel.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L1) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L1) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch5-audio-dmacon-adkcon`](../../hrm/ch5-audio-dmacon-adkcon) — `paula.v` alone, no CPU core: the modulator's own output. [`integration/ch5-audio-attach-modulation`](../../integration/ch5-audio-attach-modulation) — programs on `nanomig.v` through the CPU, on fx68k and TG68K, graded at `audio_left` and `audio_right`: the next channel's volume and period, and the modulator's own output |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red in `hrm/ch5-audio-dmacon-adkcon`: `ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS`. 20 red in `integration/ch5-audio-attach-modulation`, the same lines on fx68k and TG68K (173 passed, 20 failed on each core), and every one of them green on a copy of the audio RTL with the attach bits implemented to the pages (209 passed, 0 failed on each core): `P4-C`, both kinds of modulation at once, and `P4-JOINT`, both bits needed for both; `P6-A` for each of the six attach bits, 0, 1, 2, 4, 5 and 6; `P8-ATT` for `S-AV`, `S-AP`, `S-BOTH`, `S-DAC3-V` and `S-DAC3-P`, the modulator's own output, and the joint lines `P1-JOINT` and `P8-JOINT-AP`; `P10-AP` and `P10-BOTH`, the request timing of an attached channel; and `P1-A`, `P2-A` and `P4-A`, which reach FAIL through an empty population, the next channel never changing |
| **Verdict of the four** | **all four route a modulator's words into the next channel, and on the package's stimulus all four leave the modulators silent**; they split 2-2 on p. 150's rule that either attach bit disables the channel's output: WinUAE (`audio.cpp:2761-2769`) and Copperline (`paula.rs:2326-2328`) mute a modulator's output, while vAmiga and Denise only stop loading its output buffer under a volume attach, so a period-only modulator still plays its words |

---

## What the HRM says

**Printed pp. 149–151** describe modulating one channel with another. With an
attach bit set, the modulating channel's own output is silenced and its words are
used as period or volume data for the next channel:

> "If the 'attach' bit for an audio channel is set, that channel ceases to
> produce sound…"

> "If you set either the volume or period attach bits for a channel, that
> channel's audio output will be disabled…"

**Appendix A, printed p. 265** gives `ADKCON` bits 7–0 as `USE3PN`–`USE0P1` and
`USE3VN`–`USE0V1`; Table 5-5 on printed p. 151 names the same bits `ATPER3–0`
and `ATVOL3–0`.

---

## What NanoMig does

It stores the bits and wires none of them to the audio path. `paula.v`
instantiates the audio block with **no ADKCON port at all**; `adkcon[14:0]` is
latched and read back through `adkconr`, and only bits 11 (`uartbrk`) and 10
(`wordsync`) leave the register — both of them disk and UART, neither audio.

The channel module states it in its own first line:

[`src/minimig-aga/paula_audio_channel.v#L1`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L1)

```verilog
//This module handles a single amiga audio channel. attached modes are not supported
```

### Measured

With **every** attach bit set — `ADKCON = 0x80FF`, all of `ATPER3–0` and
`ATVOL3–0` — a channel armed for a peaked output still peaks:

| | |
|---|---|
| `ldata` peak | 8001 |
| `rdata` peak | 8001 |
| expected if the manual held | 0 and 0 |

8001 is sample 127 × effective volume 63 — what this path gives an
**unmodulated** channel at `AUDxVOL = 64`, which it treats as 63
([NM-HRM-025](NM-HRM-025.md)). Nothing was silenced.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a checkout at
the pinned revision and the quotation verified there.

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:2761-2769`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L2761-L2769)<br>[`audio.cpp:592`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L592)<br>[`audio.cpp:1640-1657`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1640-L1657) | <pre>void audio_update_adkmasks (void)<br>{<br>&nbsp;&nbsp;&nbsp;&nbsp;static int prevcon = -1;<br>&nbsp;&nbsp;&nbsp;&nbsp;unsigned long t = adkcon \| (adkcon &gt;&gt; 4);<br>&nbsp;&nbsp;&nbsp;&nbsp;audio_channel[0].data.adk_mask = (((t &gt;&gt; 0) & 1) - 1);</pre> a per-channel mask built from both attach fields, zero when either is set, and applied to the channel's output, `int output = (acd->current_sample *vol) & acd->adk_mask;`: **either attach bit mutes the modulator**, as p. 150 prints. `loaddat` routes its word to the next channel's period or volume |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachine.cpp:118-135`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L118-L135)<br>[`Core/Components/Paula/Audio/StateMachine.cpp:149-195`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L149-L195) | <pre>if (!AUDxAV()) { buffer = auddat; return; }<br>if constexpr (nr == 0) paula.channel1.pokeAUDxVOL(auddat);<br>…<br>if constexpr (nr == 0) paula.channel1.pokeAUDxPER(auddat);</pre> the modulator's word is written into the **next** channel's volume or period. **The modulator is not muted**: `penhi` and `penlo` send its `buffer` to the sampler whatever ADKCON holds, `i16 scaled = (i16)(sample * audvol);`. Under a volume attach `buffer` is never reloaded, so the modulator keeps its last word, zero from reset; under a period attach alone `buffer = auddat` still runs and the period words play as samples |
| **Denise** | [`emulation/libami/paula/audio.cpp:79-81`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L79-L81)<br>[`emulation/libami/paula/audio.cpp:158-165`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L158-L165)<br>[`emulation/libami/paula/paula.cpp:354-355`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L354-L355) | <pre>cha.audav = adkcon & (0x01 &lt;&lt; nr);<br>cha.audap = adkcon & (0x10 &lt;&lt; nr);<br>cha.napnav = !cha.audap \|\| cha.audav;</pre> `pbufld1` routes the word to the next channel's volume under `audav` and otherwise loads it, `cha.buffer = cha.dat;`. **The modulator is not muted**: `penhi`/`penlo` put the buffer on the channel's output whatever ADKCON holds, and the mixer adds every channel, `channels[0].sample + channels[3].sample`. As in vAmiga, a volume attach leaves the last word, zero from reset, and a period attach alone plays the words |
| **Copperline** | [`src/chipset/paula.rs:1190-1192`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1190-L1192)<br>[`src/chipset/paula.rs:2326-2328`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2326-L2328)<br>[`src/chipset/paula.rs:1817-1832`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1817-L1832) | <pre>fn channel_attached_as_modulator(&self, ch_idx: usize) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;let volume_attach = 1u16 &lt;&lt; ch_idx;<br>&nbsp;&nbsp;&nbsp;&nbsp;let period_attach = 1u16 &lt;&lt; (ch_idx + 4);</pre> and the mixer returns 0 for any channel so attached, `if self.channel_muted[ch_idx] \|\| self.channel_attached_as_modulator(ch_idx) {`: **either attach bit mutes the modulator**, as p. 150 prints. `aud_pbufld1` and `aud_pbufld2` route its word to the next channel's volume or period |
| **MiSTer** | [`rtl/paula_audio_channel.v:1`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L1) | descendant — carries the identical first-line comment, *"attached modes are not supported"*. No ADKCON port on the audio-channel module here either |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:1`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L1) | descendant — carries it too: `//This module handles a single amiga audio channel. attached modes are not supported`, word for word, even though the rest of the file has drifted by about 29 lines (see [NM-HRM-025](NM-HRM-025.md)/[NM-HRM-026](NM-HRM-026.md)). Neither MiSTer's nor TC64's audio-channel module has an ADKCON port either, and neither `paula.v` wires one in — the missing attach/modulation logic is a property of the interface in all three lines |

**All four independent implementations route a modulator's words into the next
channel. NanoMig does not**, and its own source says so. On the half this package
asserts, silencing the modulator, the four agree on the package's own stimulus —
every attach bit set from reset, where WinUAE and Copperline mute the modulators
and vAmiga and Denise never load their output buffers — and split 2-2 on the
rule p. 150 prints: only WinUAE and Copperline mute a modulator for either bit;
in vAmiga and Denise a period-only modulator still plays its words, and a volume
attach set while a channel holds a word keeps that word playing. MiSTer and TC64
do not implement it either — the missing interface is a property of `paula.v`
and `paula_audio_channel.v` in all three trees, so the two descendant rows repeat
NanoMig's absence rather than adding a fifth and sixth vote against the manual.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — agrees with the manual:

  > "nevertheless when a channel (including channel 3) is enabled as a modulation source, the DAC of the channel which is the source is disabled/muted."
  <!-- corpus: paula-8364r7-dissection/7_7_txt.txt -->

  On the 8364R7 die a channel enabled as a modulation source has its DAC muted, channel 3 included although its modulation outputs are open traces: the manual's attached channel that stops producing sound. 6_6 adds 'ADKCON controls the rest of the Paula functionality: audio channel modulation,'. NanoMig stores the bits and silences nothing.

Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989.

---

## Suggested change

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
attaches is a defensible scope decision — it is only undocumented at the level of
the register map, where `ADKCON` accepts the bits and reads them back as though
they did something.

---

## What holds the finding up

- **The control is on the same stimulus and the same measurement basis.**
  `ADKCON-ATTACH-MEASUREMENT-BASIS-CONTROL` sets `AUDxVOL = 0` instead of the
  attach bits and the same peak detection reads silence — so the technique
  reports silence when a channel is genuinely silent, which rules out a
  units error in the peak measurement.
- The assertion's stimulus mutation is recorded and turns it green: with the
  `AUD0DAT`/`AUD1DAT` writes withheld, both peaks read 0.

---

## What is asserted, and what is deliberately not

Only the half whose stimulus **and** consequence are both on real ports of
`paula.v`: write `ADKCON`, observe that the modulating channel is not silenced.

The other half — a modulator's word reaching another channel's period or volume
register — is not asserted in this package: no audio module has an attach port,
so it could show only as a change in the next channel's output.
[`integration/ch5-audio-attach-modulation`](../../integration/ch5-audio-attach-modulation)
grades that output on `nanomig.v`'s audio ports, with programs run through the
CPU. Its reference captures (`PC4-VOL`, `PC4-DUR`) and its side assignment (`PC5-VOL`, `PC5-PER`) are green on
both cores, so the reds above are read on a basis that is shown to work. With an
attach bit set, the next channel keeps the volume and period the program wrote
to it, and the modulator's own output keeps playing its words.

**This is an absent feature, of the same character as [NM-HRM-015](withdrawn/NM-HRM-015.md) and
[NM-HRM-021](NM-HRM-021.md):** declared in a register, accepted by the decode, reaching nothing.
The three together are the register's clearest group — each is a bit the manual
specifies, the RTL stores, and no logic consumes.
