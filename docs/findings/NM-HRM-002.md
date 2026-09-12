# NM-HRM-002 — audio channels on the wrong stereo side

| | |
|---|---|
| **Status** | open · red |
| **Module** | `paula_audio_mixer.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_mixer.v#L76-L77) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_mixer.v#L76-L77) |
| **Upstream** | inherited — `paula_audio_mixer.v` is content-identical to upstream |
| **Report to** | **nowhere.** The manual contradicts itself and the implementations split evenly — see *Suggested change* |
| **Test** | [`hrm/paula_audio_mixer`](../../hrm/paula_audio_mixer), [`hrm/ch5-audio-volume-scale`](../../hrm/ch5-audio-volume-scale) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **two against two** — nothing outside the manual settles it |

---

## What the HRM says — both things

**Chapter 5, printed p. 137:**

> channels 1 + 2 feed **left**, channels 0 + 3 feed **right**

**The appendix pin list, the opposite:**

> "AUDO — Audio output from left (channels 0, 3) port"

**The manual says both, in both editions.** It cannot decide this on its own, and
an implementation has to pick a side.

---

## What NanoMig does

It follows the pin list:

[`src/minimig-aga/paula_audio_mixer.v#L76-L77`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_mixer.v#L76-L77)

```verilog
  ldatasum <= #1 {msample0[13], msample0} + {msample3[13], msample3};
  rdatasum <= #1 {msample1[13], msample1} + {msample2[13], msample2};
```

Channels 0 and 3 to the left sum, 1 and 2 to the right.

---

## What the others say

| | code | side |
|---|---|---|
| **WinUAE** | — hands channel 0 to `put_sound_word_right` | **chapter 5** |
| **vAmiga** | — default panning puts channels 0 and 3 at `pan = 0.85`, on an axis where 1 is fully right | **chapter 5** |
| **Denise** | [`emulation/libami/paula/paula.cpp:354-355`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L354-L355) | <pre>int32_t sampleL = channels[0].sample + channels[3].sample;<br>int32_t sampleR = channels[1].sample + channels[2].sample;</pre> **pin list** |
| **Copperline** | [`src/chipset/paula.rs:2178-2182`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2178-L2182) | <pre>// Paula stereo routing follows the common A500/A600/A1200 and<br>// Minimig mapping: channels 0 and 3 go left, 1 and 2 right.<br>// Some HRM prose and motherboard jack labels describe the<br>// opposite physical side; keep this as channel-to-DAC routing.</pre> **pin list, knowingly** |

**Two against two.** Copperline's comment is the most useful line on this page:
it knows the HRM prose describes the opposite side, names the conflict, and keeps
the pin-list routing deliberately.

---

## Suggested change

**None.** Swapping the sums would satisfy chapter 5 and violate the appendix, and
would move NanoMig from one half of the implementations to the other. There is no
change that makes this green against both sentences.

What the entry is for is to record that **the manual contradicts itself here**,
and that the contradiction is inherited by every implementation, each of which
has picked a side. NanoMig's side is the one Denise and Copperline are on, and
Copperline documents the choice.

---

## A correction this entry carries

An earlier version of this entry said two emulators settle it against the RTL.
**They do not.** Reading the third and fourth witnesses changed the picture from
2–0 to 2–2.

The red stands — the manual contradicts itself and an implementation has to pick
a side, so a conformance test against either sentence must fail against the other
— but nothing outside the manual settles which side is right.
