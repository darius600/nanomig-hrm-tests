# NM-HRM-028 — ADKCON's audio attach bits are stored and reach nothing

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula.v`, `paula_audio_channel.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L1) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L1) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch5-audio-dmacon-adkcon`](../../hrm/ch5-audio-dmacon-adkcon) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **all four implement attach and modulation** — the best-corroborated finding in chapter 5 |

---

## What the HRM says

**Printed pp. 149–151** describe modulating one channel with another. With an
attach bit set, the modulating channel's own output is silenced and its words are
used as period or volume data for the next channel:

> "If the 'attach' bit for an audio channel is set, that channel ceases to
> produce sound…"

> "If you set either the volume or period attach bits for a channel, that
> channel's audio output will be disabled…"

**Appendix A, printed p. 265** gives `ADKCON` bits 7–0 as `ATPER3–0` and
`ATVOL3–0`.

---

## What NanoMig does

It stores the bits and wires none of them to the audio path. `paula.v`
instantiates the audio block with **no ADKCON port at all**; `adkcon[14:0]` is
latched and read back through `adkconr`, and only bits 11 (`uartbrk`) and 10
(`wordsync`) leave the register — both of them disk and UART, neither audio.

The channel module states it in its own first line:

[`src/minimig-aga/paula_audio_channel.v#L1`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L1)

```verilog
// This module handles a single amiga audio channel. attached modes are not supported
```

### Measured

With **every** attach bit set — `ADKCON = 0x80FF`, all of `ATPER3–0` and
`ATVOL3–0` — a channel armed for a peaked output still peaks:

| | |
|---|---|
| `ldata` peak | 8001 |
| `rdata` peak | 8001 |
| expected if the manual held | 0 and 0 |

8001 is sample 127 × effective volume 63 — the manual's own arithmetic for an
**unmodulated** channel. Nothing was silenced.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a checkout at
the pinned revision and the quotation verified there.

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:2761-2767`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L2761-L2767) | <pre>void audio_update_adkmasks (void)<br>{<br>&nbsp;&nbsp;&nbsp;&nbsp;unsigned long t = adkcon \| (adkcon &gt;&gt; 4);<br>&nbsp;&nbsp;&nbsp;&nbsp;audio_channel[0].data.adk_mask = (((t &gt;&gt; 0) & 1) - 1);</pre> a per-channel mask built from both attach fields |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachine.cpp:118-131`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L118-L131) | <pre>if (!AUDxAV()) { buffer = auddat; return; }<br>if constexpr (nr == 0) paula.channel1.pokeAUDxVOL(auddat);<br>…<br>if constexpr (nr == 0) paula.channel1.pokeAUDxPER(auddat);</pre> the modulator's word is written into the **next** channel's volume or period |
| **Denise** | [`emulation/libami/paula/audio.cpp:79-81`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L79-L81) | <pre>cha.audav = adkcon & (0x01 &lt;&lt; nr);<br>cha.audap = adkcon & (0x10 &lt;&lt; nr);<br>cha.napnav = !cha.audap \|\| cha.audav;</pre> |
| **Copperline** | [`src/chipset/paula.rs:1190-1192`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1190-L1192) | <pre>fn channel_attached_as_modulator(&self, ch_idx: usize) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;let volume_attach = 1u16 &lt;&lt; ch_idx;<br>&nbsp;&nbsp;&nbsp;&nbsp;let period_attach = 1u16 &lt;&lt; (ch_idx + 4);</pre> |
| **MiSTer** | [`rtl/paula_audio_channel.v:1`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L1) | descendant — carries the identical first-line comment, *"attached modes are not supported"*. No ADKCON port on the audio-channel module here either |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:1`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L1) | descendant — carries it too: `//This module handles a single amiga audio channel. attached modes are not supported`, word for word, even though the rest of the file has drifted by about 29 lines (see `NM-HRM-025`/`NM-HRM-026`). Neither MiSTer's nor TC64's audio-channel module has an ADKCON port either, and neither `paula.v` wires one in — the missing attach/modulation logic is a property of the interface in all three lines |

**Four out of the four independent implementations implement attach and
modulation. NanoMig alone does not**, and its own source says so. This is the
best-corroborated finding chapter 5 produced. MiSTer and TC64 do not implement it
either — the missing interface is a property of `paula.v` and
`paula_audio_channel.v` in all three trees, so the two descendant rows repeat
NanoMig's absence rather than adding a fifth and sixth vote against the manual.

---

## Suggested change

Attach is a feature, not a bug fix, and adding it is real work: the modulating
channel must be silenced, and its fetched word must be routed into the next
channel's `AUDxPER` or `AUDxVOL` instead of its own sample path. vAmiga's
`pbufld1`/`pbufld2` split is the clearest small model of it.

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
- The assertion's stimulus mutation is recorded and red.

---

## What is asserted, and what is deliberately not

Only the half whose stimulus **and** consequence are both on real ports of
`paula.v`: write `ADKCON`, observe that the modulating channel is not silenced.

The other half — a modulator's word reaching another channel's period or volume
register — is not testable at any boundary in this design, because no audio
module has an attach port to observe it on. It is recorded rather than asserted.

**This is an absent feature, of the same character as `NM-HRM-015` and
`NM-HRM-021`:** declared in a register, accepted by the decode, reaching nothing.
The three together are the register's clearest group — each is a bit the manual
specifies, the RTL stores, and no logic consumes.
