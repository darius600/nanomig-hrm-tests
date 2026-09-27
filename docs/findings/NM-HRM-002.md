# NM-HRM-002 — audio channels on the wrong stereo side [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_audio_mixer.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_mixer.v#L76-L77) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_mixer.v#L76-L77) |
| **Report to** | **nowhere.** The manual contradicts itself and the implementations split, three to one — see *Suggested change* |
| **Test** | [`hrm/ch5-audio-stereo-routing`](../../hrm/ch5-audio-stereo-routing), [`hrm/ch5-audio-volume-scale`](../../hrm/ch5-audio-volume-scale) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **three against one**: WinUAE, Denise and Copperline with the pin list, vAmiga with chapter 5; outside the emulators, the 8364R7 silicon sides with the pin list — see *What other Commodore documents say* |

---

## What the HRM says — both things

**Chapter 5, printed p. 137:**

> Channels 1 and 2 are connected to the left-side stereo output jack. Channels 0 and 3 are connected to the right-side output jack.

**The appendix pin list, the opposite** (appendix E, printed p. 326, the serial connector's audio pins):

> AUDO — "Audio output from left (channels 0, 3) port, intended to send audio to the modem."

**The manual says both.** It cannot decide this on its own, and an
implementation has to pick a side.

---

## What NanoMig does

It follows the pin list:

[`src/minimig-aga/paula_audio_mixer.v#L76-L77`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_mixer.v#L76-L77)

```verilog
  ldatasum <= #1 {msample0[13], msample0} + {msample3[13], msample3};
  rdatasum <= #1 {msample1[13], msample1} + {msample2[13], msample2};
```

Channels 0 and 3 to the left sum, 1 and 2 to the right.

### Measured

[`hrm/ch5-audio-stereo-routing`](../../hrm/ch5-audio-stereo-routing) drives one
sample on one channel at a time and reads both sums. Every channel lands on the
side chapter 5 does not name: channel 0's 17 arrives as L=17 R=0, channel 1's 23
as L=0 R=23, channel 2's 31 as L=0 R=31, channel 3's 47 as L=47 R=0 -- 0 passed,
4 failed.

The four checks are red, so the proof that they can pass runs the other way:
driven on its partner channel instead (`MUTATE=ROUTE-TO-PARTNER`, 0<->1 and
2<->3), each sample reaches the side chapter 5 names and all four go green. The
checks read which side a channel reaches; they do not fail whatever the RTL does.

---

## What the others say

| | code | side |
|---|---|---|
| **WinUAE** | [`audio.cpp:1153-1165`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1153-L1165) | <pre>data1 = datas[0] + datas[3];<br>data2 = datas[1] + datas[2];<br>...<br>put_sound_word_right(data1);<br>put_sound_word_left(data2);</pre> the function is named for the right, but it writes channels 0 and 3 as the **first** sample of each stereo frame ("Always put the right word before the left word", [`:516`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L516)), and the frame is played as `KSAUDIO_SPEAKER_STEREO` ([`od-win32/sounddep/sound.cpp:1026`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/sounddep/sound.cpp#L1026)), whose first sample is the left speaker. WinUAE mixes a CD's left sample into the same slot ([`:3002`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L3002)). Channels 0 and 3 are heard on the left unless *Swap channels* is set. **pin list** |
| **vAmiga** | — default panning puts channels 0 and 3 at `pan = 0.85`, on an axis where 1 is fully right | **chapter 5** |
| **Denise** | [`emulation/libami/paula/paula.cpp:354-355`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L354-L355) | <pre>int32_t sampleL = channels[0].sample + channels[3].sample;<br>int32_t sampleR = channels[1].sample + channels[2].sample;</pre> **pin list** |
| **Copperline** | [`src/chipset/paula.rs:2178-2182`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2178-L2182) | <pre>// Paula stereo routing follows the common A500/A600/A1200 and<br>// Minimig mapping: channels 0 and 3 go left, 1 and 2 right.<br>// Some HRM prose and motherboard jack labels describe the<br>// opposite physical side; keep this as channel-to-DAC routing.</pre> **pin list, knowingly** |
| **MiSTer** | [`rtl/paula_audio_mixer.v:76-77`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_mixer.v#L76-L77) | descendant — byte-identical: `ldatasum <= #1 {msample0[13], msample0} + {msample3[13], msample3};` and the matching `rdatasum` line; confirmed by a full-file comparison, not only this range — `paula_audio_mixer.v` is byte-identical across all three trees. **pin list** |
| **TC64** | [`rtl/minimig/paula_audio_mixer.v:76-77`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_mixer.v#L76-L77) | descendant — the same two lines, unchanged. Both lines pick the pin-list side; neither resolves the chapter-5-vs-appendix contradiction differently, so there is nowhere for a fix to diverge to. **pin list** |

**Three against one among the four independent implementations.** Copperline's
comment is the most useful line on this page: it knows the HRM prose describes
the opposite side, names the conflict, and keeps the pin-list routing
deliberately. MiSTer and TC64 land on the pin-list side too, but only because
`paula_audio_mixer.v` is byte-identical in all three trees — as descendants they
count for nothing in the 3–1 split among the four independent readings.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — **agrees with NanoMig**:

  > "Channel 0 and 3 feed the AUDL output pad, //analog "audio left" output signal channel 1 and 2 feed the AUDR output pad, //analog "audio right" output signal"
  <!-- corpus: paula-8364r7-dissection/7_7_txt.txt -->

  On the 8364R7 die channels 0 and 3 drive the pad the author labels AUDL (audio left) and 1 and 2 drive AUDR: NanoMig's ldatasum/rdatasum split and the appendix pin list, not 3rd-ed. p. 137. The note speaks of chip pads; which jack each pad reaches is board wiring it does not cover.

- **US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989**, PDF page 2 — says something else:

  *Read from the drawing, not quoted:* FIG. 1 draws the D/A converter for channels 0 and 1 (37) feeding LEFT AUDIO (41), and the converter for channels 2 and 3 (39) feeding RIGHT AUDIO (43).
  <!-- drawing: patent-us4829473-paula/p002.txt -->

  The block diagram pairs the channels by converter: 0 and 1 to the left output, 2 and 3 to the right. That is neither chapter 5's assignment (1 and 2 left) nor the appendix pin list's, which NanoMig follows (0 and 3 left). It is a diagram of the patented circuit, not a pin list of the shipped chip; the specification's text calls each side's two channels A and B and does not number them (PDF page 9).

Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989.

---

## Suggested change

**None.** Swapping the sums would satisfy chapter 5 and violate the appendix, and
would move NanoMig from one side of the implementations to the other. There is no
change that makes this green against both sentences.

What the entry is for is to record that **the manual contradicts itself here**,
and that the contradiction is inherited by every implementation, each of which
has picked a side. NanoMig's side is the one WinUAE, Denise and Copperline are
on, and Copperline documents the choice.

---

## Why the red stands

The red stands — the manual contradicts itself and an implementation has to pick
a side, so a conformance test against either sentence must fail against the other
— and the four emulators do not settle which side is right. One source outside
them does take a side, the pin list's: the 8364R7 silicon feeds channels 0 and 3
to the AUDL pad (*What other Commodore documents say*).
