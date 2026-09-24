# NM-HRM-025 — volume 64 and volume 63 are the same gain on the multiply path

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula_audio_mixer.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_mixer.v#L23-L33) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_mixer.v#L23-L33) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch5-audio-volume-scale`](../../hrm/ch5-audio-volume-scale), [`hrm/ch5-audio-register-map`](../../hrm/ch5-audio-register-map) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | all four clamp at 64 and keep it distinct from 63 |

---

## What the HRM says

**Table 5-9, printed p. 163** gives each volume setting a decibel value. The two
at the top are distinct:

| volume | decibel value |
|---|---|
| 64 | 0.0 |
| 63 | -0.1 |

**Appendix A, printed p. 266**, on `AUDxVOL`:

> "65 linear volume levels"

and describes bit 6 as forcing maximum output.

So the manual asks for 65 distinct levels, and for 63 and 64 to differ.

---

## What NanoMig does

`paula_audio_mixer` wires bit 6 of the seven-bit volume register *down into* the
six-bit input of the volume helper, by OR-ing it into every bit:

[`src/minimig-aga/paula_audio_mixer.v#L23-L33`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_mixer.v#L23-L33)

```verilog
// when volume MSB is set, volume is always maximum
paula_audio_volume sv0
(
  .sample(sample0),
  .volume({  (vol0[6] | vol0[5]),
            (vol0[6] | vol0[4]),
            (vol0[6] | vol0[3]),
            (vol0[6] | vol0[2]),
            (vol0[6] | vol0[1]),
            (vol0[6] | vol0[0]) }),
  .out(msample0)
```

With `vol0 = 64` (`1000000`), bit 6 is set, so every one of the six bits handed
to the helper is forced high — the helper sees `111111`, which is **63**.

**The same design implements volume a second time, and that one is correct.**
The PWM path keeps the register at its full seven bits and compares across
widths, so 64 stays above 63:

[`src/minimig-aga/paula_audio_channel.v#L101-L103`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L101-L103)

```verilog
      audvol[6:0] <= 7'b000_0000;
    else if (aen && (reg_address_in[3:1]==AUDVOL[3:1]))
      audvol[6:0] <= data[6:0];
```

[`src/minimig-aga/paula_audio_channel.v#L149`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_audio_channel.v#L149)

```verilog
assign volgate = (volcnt < audvol);
```

### Measured

| path | volumes 63, 64, 65, 96, 127 | levels |
|---|---|---|
| multiply (`ldata`/`rdata`) | `ldatasum = 4032` for **all five** | 64 distinct |
| PWM (`ldata_okk`/`rdata_okk`) | open-tick counts 0, 4, 128, 252, 256 for volumes 0, 1, 32, 63, 64 — exactly 4× each written value | **65 distinct** |

---

## What the others say

Nobody decides this but the manual. Every line below was read in a checkout at
the pinned revision and the quotation verified there.

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:1540-1543`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1540-L1543) | <pre>// 7 bit register in Paula.<br>v &= 127;<br>if (v > 64)<br>&nbsp;&nbsp;&nbsp;&nbsp;v = 64;</pre> |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachineRegs.cpp:36-38`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachineRegs.cpp#L36-L38) | <pre>// 2. All values greater than 64 are treated as 64 (max volume)<br>audvolLatch = (u16)std::min(value & 0x7F, 64);</pre> |
| **Denise** | [`emulation/libami/paula/audio.cpp:153-154`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L153-L154) | <pre>value &= 127;<br>if (value & 0x40) value = 0x40;</pre> |
| **Copperline** | [`src/chipset/paula.rs:227-229`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L227-L229) | <pre>fn paula_volume_from_word(word: u16) -> u8 {<br>&nbsp;&nbsp;&nbsp;&nbsp;((word & 0x007F) as u8).min(64)<br>}</pre> |
| **MiSTer** | [`rtl/paula_audio_mixer.v:23-33`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_mixer.v#L23-L33) | descendant — byte-identical to NanoMig, the same OR-into-six-bits construction; `paula_audio_mixer.v` is byte-identical across all three trees, confirmed by a full-file comparison, so the 64-collapses-to-63 defect is inherited by both. MiSTer also carries NanoMig's second, correct path unchanged — the PWM `volgate` block in `paula_audio_channel.v:149` |
| **TC64** | [`rtl/minimig/paula_audio_mixer.v:23-33`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_mixer.v#L23-L33) | descendant — the same construction, unchanged. The second, correct implementation cited above for contrast is not inherited symmetrically: `paula_audio_channel.v`'s `audvol` write (NanoMig `:101-103`) is byte-identical in MiSTer (same lines) and in TC64 `rtl/minimig/paula_audio_channel.v:95-97`, but the PWM `volgate` block that makes that second path correct (NanoMig `:136-150`, including the `:149` line quoted above) **does not exist in TC64 at all** — TC64's audio channel goes straight from the `intreq` assign to the period counter, with only a vestigial, unused `volcntrld` signal left behind. TC64 has only the defective multiply path; MiSTer carries both, exactly as NanoMig does |

**All four independent implementations clamp at 64 and keep it distinct from
63.** Two say why in their own comments. MiSTer and TC64 do not belong in that
count: `paula_audio_mixer.v` is byte-identical in both, so both collapse 64 into
63 exactly as NanoMig does — as descendants they inherit the defect, not a
correction of it.

Denise's line is the one to put beside NanoMig's, because both implement
appendix A's "bit 6 forces maximum" and they arrive at different answers: Denise
masks to seven bits and then forces the **value** to `0x40` — 64 — while
`paula_audio_mixer` ORs bit 6 down into the low six bits and arrives at 63.

*Citation corrected when the sources were checked out: the register previously
gave Denise's line as `paula/audio.cpp:153`. The clamp is at **154**; 153 is the
seven-bit mask above it.*


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

---

## Suggested change

Clamp rather than OR, which is what the other four do and what the manual's
"65 levels" requires. The helper's input is six bits, so the seventh level needs
the helper itself to widen, or the mixer to special-case it:

```verilog
// 64 is a level of its own, not a saturated 63.
.volume(vol0[6] ? 7'd64 : {1'b0, vol0[5:0]}),
```

with `paula_audio_volume`'s `volume` port widened to `[6:0]`. That change makes
the multiply path agree with the PWM path already in the same file.

**This is a suggestion, not a decision.** Whether it matters depends on which
path reaches the DAC in a real build — see the closing note.

### Confirming a fix, and what stays red afterwards

Rerun the two packages named at the top of this page **against the tree you
changed**. Both Makefiles default to `NANOMIG ?= ../../nanomig`, the pinned
submodule, so a bare `make` here rebuilds the unmodified RTL and reproduces the
reds below no matter what you fixed:

```sh
make -C hrm/ch5-audio-register-map test NANOMIG=/path/to/your/NanoMig
make -C hrm/ch5-audio-volume-scale  test NANOMIG=/path/to/your/NanoMig
```

Leave `NANOMIG` off only when you deliberately want the pinned revision — for
instance to reproduce the before-state in the table below.

**Before any change, measured 19.09.2026 against the pinned submodule:**

| Package | Result | Which reds |
|---|---|---|
| `ch5-audio-register-map` | `8 passed, 1 failed` | `VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH` — this finding |
| `ch5-audio-volume-scale` | `69 passed, 5 failed` | three for this finding — `MULTIPLY-PATH-COLLAPSES-VOLUME-64-ONTO-63`, `TABLE-5-9-VOLUME-TO-DECIBEL vol0=64`, `TABLE-5-9-HAS-65-VOLUME-LEVELS`. **Two are not**: `CH1-CH2-ROUTE-LEFT` and `CH0-CH3-ROUTE-RIGHT` belong to **NM-HRM-002**, the stereo routing |

**After a correct fix**, expect `ch5-audio-register-map` fully green and
`ch5-audio-volume-scale` down to its two routing reds — `9 passed, 0 failed`
and `72 passed, 2 failed`.

**So `ch5-audio-volume-scale` staying red is not a failed fix.** Its two
remaining reds are a different finding about which channel lands on which side,
and nothing suggested here touches them. Read the assertion label before
concluding anything: each one names the finding it belongs to.

---

## What holds the finding up

- **A control on the same fixture and the same measurement basis.**
  `MULTIPLY-PATH-DISTINGUISHES-ADJACENT-VOLUMES` compares 62 against 63 and is
  **green**, so the collapse at 64 is a property of that value and not a dead
  sweep.
- **The other path in the same design.** `VOLUME-65-LEVELS` is green on the PWM
  output, which rules out the test's own arithmetic.
- The collapse assertion's stimulus mutation is recorded `impossible`: the OR is
  unconditional and no port gates it, so nothing can be changed to make the
  assertion go red for a different reason. The adjacency control stands in its
  place.

---

## For the specialists

The whole of Table 5-9 runs **+0.10 to +0.18 dB high** on the multiply path and
reproduces to the printed decimal on the PWM path. Which of the two reaches the
DAC in a real build decides whether this is audible or bookkeeping.
