# NM-HRM-002 — audio channels 0 and 3 play on the left, where p. 137 says right and p. 326 says left

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. Question: is p. 137's assignment (channels 1 and 2 to the left jack, 0 and 3 to the right) a misprint of the assignment p. 326 prints (0 and 3 left, 1 and 2 right), which NanoMig follows? The red is against p. 137. Closes when an Amiga specialist records p. 137 as a misprint, or confirms p. 137 and records p. 326 as wrong, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Chapter 5 of the *Amiga Hardware Reference Manual*, 3rd ed. This is a defect in one printed page, contradicted by p. 326 of the same manual |
| **Scope** | which side each of Paula's four audio channels reaches, at the mixer's left and right sums with all four volumes at 1 (`hrm/ch5-audio-stereo-routing` drives one channel at a time, `hrm/ch5-audio-volume-scale` a pair) and, for the whole design, at `nanomig.v`'s `audio_left` and `audio_right` on both CPU cores; the manual names no chip revision; the one silicon witness is the 8364R7 |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 5 — Audio, printed p. 137; Appendix E — I/O Connectors and Interfaces, printed p. 326; also quoted: Chapter 1, printed p. 4, and Appendix J, printed p. 380 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `CH1-CH2-ROUTE-LEFT` `sol:5-p137.c7` / `opus:5-chan.c2`, `CH0-CH3-ROUTE-RIGHT` `sol:5-p137.c8` / `opus:5-chan.c3` (both p. 137); the four assertions of `hrm/ch5-audio-stereo-routing` carry no claim id, and no assertion on this page carries one for p. 326, p. 4 or p. 380 |
| **Module** | `paula_audio_mixer.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_mixer.v#L76-L77) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_mixer.v#L76-L77); inherited (MiSTer and TC64 have the same two lines) |
| **Test** | [`hrm/ch5-audio-stereo-routing`](../../hrm/ch5-audio-stereo-routing): `make -C hrm/ch5-audio-stereo-routing test`; [`hrm/ch5-audio-volume-scale`](../../hrm/ch5-audio-volume-scale): `make -C hrm/ch5-audio-volume-scale test`; [`integration/ch5-audio-attach-modulation`](../../integration/ch5-audio-attach-modulation): `make -k -C integration/ch5-audio-attach-modulation test`. An expected red exits nonzero; the integration package exits nonzero on other claims' reds, and its lines for this page, `PC5-VOL`, `PC5-PER` and the `PC5 measured sides` line, are green or measured |
| **Assertions** | 6 red, each expecting the side p. 137 prints and reading the opposite sum (values under *RTL and measurement*): `channel 0 routes right`, `channel 1 routes left`, `channel 2 routes left` and `channel 3 routes right` in `hrm/ch5-audio-stereo-routing` (0 passed, 4 failed); `CH1-CH2-ROUTE-LEFT` and `CH0-CH3-ROUTE-RIGHT` in `hrm/ch5-audio-volume-scale` (69 passed, 5 failed; the other three reds are NM-HRM-025's, per the test file), which read the same two p. 137 sides on the mixer's multiply-then-sum path |
| **Verdict of the four** | **three against one**: WinUAE, Denise and Copperline with the pin list, vAmiga with chapter 5; outside the emulators, the 8364R7 silicon sides with the pin list, and Commodore's A500, A2000 and B2000 schematics wire the board as p. 326 describes — see [Independent evidence](#independent-evidence) |
| **Effort** | none — no RTL change; if p. 137 is a misprint, the correction exchanges "left" and "right" in its two sentences |

---

## Source requirement

**Chapter 5, printed p. 137:**

> Channels 1 and 2 are connected to the left-side stereo output jack. Channels 0 and 3 are connected to the right-side output jack.

**Appendix E, printed p. 326**, the pin list, opposite on both of the serial connector's audio rows:

> AUDO — "Audio output from left (channels 0, 3) port, intended to send audio to the modem."

> AUDI — "Audio input to right (channels 1, 2) port, intended to receive audio from the modem; this input is mixed with the analog output of the right (channels 1, 2)."

No other page numbers the channels on a side. Chapter 1 says only that they are split two and two (printed p. 4): "Two channels are directed into each of the two stereo audio outputs." Appendix J's Paula pin assignment (printed p. 380) names pin 31, AUDA, "Left audio" and pin 30, AUDB, "Right audio", and does not say which channels drive either.

## RTL and measurement

NanoMig follows the pin list:
[`src/minimig-aga/paula_audio_mixer.v#L76-L77`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_mixer.v#L76-L77)

```verilog
  ldatasum <= #1 {msample0[13], msample0} + {msample3[13], msample3};
  rdatasum <= #1 {msample1[13], msample1} + {msample2[13], msample2};
```

[`hrm/ch5-audio-stereo-routing`](../../hrm/ch5-audio-stereo-routing) drives one sample on one channel at a time and reads both sums. Every channel lands on the side chapter 5 does not name: channel 0's 17 arrives as L=17 R=0 (expected L=0 R=17), channel 1's 23 as L=0 R=23 (expected L=23 R=0), channel 2's 31 as L=0 R=31 (expected L=31 R=0), channel 3's 47 as L=47 R=0 (expected L=0 R=47). In `hrm/ch5-audio-volume-scale`, channels 1 and 2 (23 and 31) give ldatasum=0 where 54 is expected, and channels 0 and 3 (17 and 47) give rdatasum=0 where 64 is expected.

The whole design measures the same side. [`integration/ch5-audio-attach-modulation`](../../integration/ch5-audio-attach-modulation) runs programs through the CPU and reads `nanomig.v`'s `audio_left` and `audio_right`: `PC5 measured sides: ch0=0 ch1=1 ch2=1 ch3=0 (0=left,1=right)`, the same on fx68k and TG68K. Its `PC5` lines grade only the pairing, 0 with 3 and 1 with 2, on which pp. 137 and 326 agree.

## Risks and unknowns

- The manual says both: p. 137 and p. 326 contradict each other, and it cannot decide this on its own. An implementation has to pick a side.
- The split is three to one among the four independent implementations. MiSTer and TC64 land on the pin-list side only because `paula_audio_mixer.v` is byte-identical in all three trees; as descendants they count for nothing in it.
- p. 137 speaks of output jacks, and the chain from channel to jack is read in two halves from two sources. The dissection names its pad AUDL and gives no pin number, so its pad is identified with appendix J's pin 31, AUDA, "Left audio", by name, and it reads one revision, 8364R7. It speaks of chip pads; which jack each pad reaches is board wiring it does not cover.
- The schematics read are the A500's, the A2000's and the B2000's; the A1000 and A3000 boards are not traced. Copperline's comment records that some "motherboard jack labels describe the opposite physical side", which none of the three sheets read does.
- The patent's block diagram is of the patented circuit, not a pin list of the shipped chip; its specification calls each side's two channels A and B and does not number them (PDF page 9).

## Independent evidence

| | code | side |
|---|---|---|
| **WinUAE** | [`audio.cpp:1153-1165`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1153-L1165) | <pre>data1 = datas[0] + datas[3];<br>data2 = datas[1] + datas[2];<br>...<br>put_sound_word_right(data1);<br>put_sound_word_left(data2);</pre> the function is named for the right, but it writes channels 0 and 3 as the **first** sample of each stereo frame ("Always put the right word before the left word", [`:516`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L516)), and the frame is played as `KSAUDIO_SPEAKER_STEREO` ([`od-win32/sounddep/sound.cpp:1026`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/sounddep/sound.cpp#L1026)), whose first sample is the left speaker. WinUAE mixes a CD's left sample into the same slot ([`:3002`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L3002)). Channels 0 and 3 are heard on the left unless *Swap channels* is set. **pin list** |
| **vAmiga** | [`Core/Ports/AudioPort.cpp:472-473`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/AudioPort.cpp#L472-L473) | <pre>double l = ch0 * (1 - pan0) + ch1 * (1 - pan1) + ch2 * (1 - pan2) + ch3 * (1 - pan3);<br>double r = ch0 * pan0 + ch1 * pan1 + ch2 * pan2 + ch3 * pan3;</pre> a pan of 1 is fully right. The pan is 0.5 · (sin(setting · π / 200) + 1) ([`:250`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/AudioPort.cpp#L250)), and the default settings are 50, 350, 350 and 50 for channels 0 to 3 ([`Core/Infrastructure/Defaults.cpp:195-198`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Defaults.cpp#L195-L198)): channels 0 and 3 at pan 0.85, mostly right, 1 and 2 at 0.15. The settings pane's *Stereo* preset, 100, 300, 300 and 100 ([AudioSettings.swift:149-153](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/GUI/Settings/ViewControllers/AudioSettings.swift#L149-L153)), puts channels 0 and 3 fully right. **chapter 5** |
| **Denise** | [`emulation/libami/paula/paula.cpp:354-355`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L354-L355) | <pre>int32_t sampleL = channels[0].sample + channels[3].sample;<br>int32_t sampleR = channels[1].sample + channels[2].sample;</pre> **pin list** |
| **Copperline** | [`src/chipset/paula.rs:2178-2182`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2178-L2182) | <pre>// Paula stereo routing follows the common A500/A600/A1200 and<br>// Minimig mapping: channels 0 and 3 go left, 1 and 2 right.<br>// Some HRM prose and motherboard jack labels describe the<br>// opposite physical side; keep this as channel-to-DAC routing.</pre> and its test [`wav_capture_routes_paula_channels_to_stereo_pairs`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L3625-L3669) drives each channel alone and asserts channels 0 and 3 in the first sample of each WAV frame, the left one, and 1 and 2 in the second. **pin list, knowingly** |
| **MiSTer** | [`rtl/paula_audio_mixer.v:76-77`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_mixer.v#L76-L77) | descendant — byte-identical, confirmed by a full-file comparison: `paula_audio_mixer.v` is byte-identical across all three trees. **pin list** |
| **TC64** | [`rtl/minimig/paula_audio_mixer.v:76-77`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_mixer.v#L76-L77) | descendant — the same two lines, unchanged; there is nowhere for a fix to diverge to. **pin list** |

Commodore documents, which can agree or disagree with the manual and decide nothing here:

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — **agrees with NanoMig**:

  > "Channel 0 and 3 feed the AUDL output pad, //analog "audio left" output signal channel 1 and 2 feed the AUDR output pad, //analog "audio right" output signal"
  <!-- corpus: paula-8364r7-dissection/7_7_txt.txt -->

  On the 8364R7 die channels 0 and 3 drive the pad the author labels AUDL (audio left) and 1 and 2 drive AUDR: NanoMig's ldatasum/rdatasum split and the appendix pin list, not 3rd-ed. p. 137.

- **US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989**, PDF page 2 — says something else:

  *Read from the drawing, not quoted:* FIG. 1 draws the D/A converter for channels 0 and 1 (37) feeding LEFT AUDIO (41), and the converter for channels 2 and 3 (39) feeding RIGHT AUDIO (43).
  <!-- drawing: patent-us4829473-paula/p002.txt -->

  The block diagram pairs the channels by converter: 0 and 1 to the left output, 2 and 3 to the right. That is neither chapter 5's assignment (1 and 2 left) nor the appendix pin list's (0 and 3 left).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF pages 259-260, 271-272 and 297-298 — speaks to the board, not to channel numbers, and **agrees with p. 326**:

  *Read from the drawing, not quoted:* on the A2000 schematic (drawing 380710, rev. 6, PDF pages 259-260) Paula's pin 31, AUDA, drives the filter chain that ends at the jack marked L (J15), and AUDOUT is taken from that output; pin 30, AUDB, ends at the jack marked R (J14), and AUDIN is summed into its first amplifier. The B2000 audio sheet (drawing 312726, rev. 2, PDF pages 271-272) and the A500 main board's audio sheet (drawing 312511, rev. 5, PDF pages 297-298) draw the same: Paula's pin 31, labelled LEFT, to the LEFT jack (CN205 on the B2000, CN4 on the A500), with AUDOUT taken from that side; pin 30, labelled RIGHT, to the RIGHT jack (CN204, CN3), with AUDIN summed into that side's amplifier.
  <!-- drawing: a500-a2000-trm-1987/p259.txt -->
  <!-- drawing: a500-a2000-trm-1987/p260.txt -->
  <!-- drawing: a500-a2000-trm-1987/p271.txt -->
  <!-- drawing: a500-a2000-trm-1987/p272.txt -->
  <!-- drawing: a500-a2000-trm-1987/p297.txt -->
  <!-- drawing: a500-a2000-trm-1987/p298.txt -->

  None of the three sheets numbers the channels. What they settle is the board half: the pin appendix J calls "Left audio" reaches the left jack, and the serial port's audio output and input sit on the sides p. 326 gives them. Channels 0 and 3 reach the left jack on these three boards if they leave Paula on pin 31, and that is what the 8364R7 dissection reads from the die, by the pad's name, AUDL.

## Decision

No RTL change. NanoMig already routes channels 0 and 3 to the side p. 326 names, as WinUAE, Denise and Copperline do. Swapping the sums would satisfy p. 137, violate p. 326, and move NanoMig from the three to the one; no change makes this green against both pages.

The change, if one is made, belongs to the manual: p. 137's two sentences are the exact reverse of p. 326's two notes, channel for channel, and if p. 137 is the misprint its correction is to exchange "left" and "right". That is a documentation decision, not a hardware one. Re-run:

```
make -C hrm/ch5-audio-stereo-routing test
make -C hrm/ch5-audio-volume-scale test
make -k -C integration/ch5-audio-attach-modulation test
```
