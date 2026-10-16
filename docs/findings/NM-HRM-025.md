# NM-HRM-025 — volume 64 and volume 63 are the same gain on the multiply path [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Question: should `paula_audio_mixer` give AUDxVOL = 64 a gain of its own on the multiply path (`ldata`/`rdata`), distinct from 63, as Table 5-9 prints 0.0 dB against -0.1 dB, where it now reads `ldatasum` 4032 for both? Closes when the change is made and the four reds under *Assertions* turn green, or when an Amiga specialist rejects it; if the rejection means Table 5-9 is wrong, this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | chip revision unknown; the manual names none. Measured on the multiply path with channel 0 only, volume register values 63, 64, 65, 96 and 127 giving the same `ldatasum`; the PWM path is correct and reaches no output |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 5 — Audio, printed p. 163, Table 5-9; Appendix A, printed p. 266, AUDxVOL · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (14)</summary> claim ids by assertion: `VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH`: `5-appA-p266.c32`, `A-audxvol.c3` (p. 266); `MULTIPLY-PATH-COLLAPSES-VOLUME-64-ONTO-63`: `5-p163.c1`, `5-t59.v64`, `5-p163.c3`, `5-t59.v63` (p. 163); `TABLE-5-9-VOLUME-TO-DECIBEL` (`vol0=64`): `5-p163.c1`, `5-t59.v64`, `5-vol.c2`, `5-vol.c6`, `5-p139.c13`, `5-p140.c2` (p. 163, with p. 139 and p. 140 restatements); `TABLE-5-9-HAS-65-VOLUME-LEVELS`: `5-vol.c3`, `5-vol.c4` (p. 139), whose label also cites p. 266's 65 linear levels without an id. All four assertions carry claim ids </details> |
| **Module** | `paula_audio_mixer.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_mixer.v#L23-L33) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_mixer.v#L23-L33); inherited |
| **Test** | [`hrm/ch5-audio-volume-scale`](../../hrm/ch5-audio-volume-scale): `make -C hrm/ch5-audio-volume-scale test`; [`hrm/ch5-audio-register-map`](../../hrm/ch5-audio-register-map): `make -C hrm/ch5-audio-register-map test`; an expected red exits nonzero |
| **Assertions** | 4 red. `VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH` in `hrm/ch5-audio-register-map` (`8 passed, 1 failed`): expected `ldata` at AUDxVOL=64 to differ from AUDxVOL=63, observed 4032 for both. In `hrm/ch5-audio-volume-scale` (`69 passed, 5 failed`): `MULTIPLY-PATH-COLLAPSES-VOLUME-64-ONTO-63`, expected `ldatasum(vol0=63)` and `ldatasum(vol0=64)` to differ, observed identical; `TABLE-5-9-VOLUME-TO-DECIBEL` (`vol0=64`), expected 0.0 dB within ±0.1, observed `ldatasum=4032`, -0.137 dB; `TABLE-5-9-HAS-65-VOLUME-LEVELS`, expected 65 distinct outputs over registers 0..64, observed 64. Its two other reds, `CH1-CH2-ROUTE-LEFT` and `CH0-CH3-ROUTE-RIGHT`, are [NM-HRM-002](NM-HRM-002.md)'s |
| **Verdict of the four** | all four clamp at 64 and keep it distinct from 63. Hardware: not measured |
| **Effort** | S — the `.volume` connection in `paula_audio_mixer.v` (four instances, `sv0` to `sv3`) and the width of `paula_audio_volume`'s `volume` port, as the suggested change states |

---

## Source requirement

**Table 5-9, printed p. 163** gives each volume setting a decibel value. The two
at the top are distinct:

| volume | decibel value |
|---|---|
| 64 | 0.0 |
| 63 | -0.1 |

**Appendix A, printed p. 266**, on `AUDxVOL`:

> "65 linear volume levels"

and describes bit 6 as forcing maximum output. So the manual asks for 65 distinct
levels, and for 63 and 64 to differ.

## RTL and measurement

`paula_audio_mixer` wires bit 6 of the seven-bit volume register *down into* the
six-bit input of the volume helper by OR-ing it into every bit
([`paula_audio_mixer.v#L23-L33`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_mixer.v#L23-L33),
comment "when volume MSB is set, volume is always maximum"). With `vol0 = 64`
(`1000000`) the helper sees `111111`, which is **63**.

The same design implements volume a second time, correctly: the PWM path keeps
`audvol` at seven bits
([`paula_audio_channel.v#L101-L103`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L101-L103))
and compares across widths, `assign volgate = (volcnt < audvol);`
([`#L149`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L149)),
so 64 stays above 63. Only the multiply path leaves the design:
[`nanomig.v#L1535-L1538`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/nanomig.v#L1535-L1538)
connects `ldata`/`rdata` to `audio_left`/`audio_right` and leaves
`ldata_okk`/`rdata_okk` unconnected. `ldata` is the mixer's `ldatasum`
(`paula_audio.v:282`, through `paula.v:303` and `minimig.v:528`). Every board top
in the tree instantiates `nanomig` and takes its audio from `audio_left` and
`audio_right`
([`src/tang/nano20k/top.sv:666-667`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/tang/nano20k/top.sv#L666-L667)
is one of twelve); no file outside `src/minimig-aga/` other than `nanomig.v` names
`ldata_okk` or `rdata_okk`.

| path | volumes 63, 64, 65, 96, 127 | levels |
|---|---|---|
| multiply (`ldata`/`rdata`) | `ldatasum = 4032` for **all five** | 64 distinct |
| PWM (`ldata_okk`/`rdata_okk`) | open-tick counts 0, 4, 128, 252, 256 for volumes 0, 1, 32, 63, 64 — exactly 4× each written value | **65 distinct** by that rule; five sampled |

Controls: `MULTIPLY-PATH-DISTINGUISHES-ADJACENT-VOLUMES` compares 62 against 63 on
the same fixture and is green, and `VOLUME-65-LEVELS` is green on the PWM output.

## Risks and unknowns

- Chip revision and real hardware are not measured, and no affected software is
  on the result.
- Both packages measure 63 against 64 on channel 0 only. `sv1` to `sv3` in
  `paula_audio_mixer.v` carry the same construction but are not measured.
- The PWM path shows 65 levels from five sampled volumes only.
- Measured against the multiply path's own maximum, the whole of Table 5-9 runs
  **+0.09 to +0.18 dB high** there, and reproduces to the printed decimal on the PWM
  path.
- MiSTer carries both paths to its top level and lets a status bit choose:
  [`Minimig.sv:1227-1230`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/Minimig.sv#L1227-L1230)
  sends `ldata_okk` to its output when `paula_pwm` (`status[50]`) is set and `ldata`
  otherwise. NanoMig has no such choice.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:1540-1543`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1540-L1543) | <pre>// 7 bit register in Paula.<br>v &= 127;<br>if (v > 64)<br>&nbsp;&nbsp;&nbsp;&nbsp;v = 64;</pre> |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachineRegs.cpp:36-38`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachineRegs.cpp#L36-L38) | <pre>// 2. All values greater than 64 are treated as 64 (max volume)<br>audvolLatch = (u16)std::min(value & 0x7F, 64);</pre> |
| **Denise** | [`emulation/libami/paula/audio.cpp:153-154`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L153-L154) | <pre>value &= 127;<br>if (value & 0x40) value = 0x40;</pre> |
| **Copperline** | [`src/chipset/paula.rs:227-229`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L227-L229) | <pre>fn paula_volume_from_word(word: u16) -> u8 {<br>&nbsp;&nbsp;&nbsp;&nbsp;((word & 0x007F) as u8).min(64)<br>}</pre> |
| **MiSTer** | [`rtl/paula_audio_mixer.v:23-33`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_mixer.v#L23-L33) | descendant — byte-identical to NanoMig (full-file comparison; also byte-identical in TC64): the same OR-into-six-bits construction, so the collapse is inherited. MiSTer also carries NanoMig's correct second path unchanged, the PWM `volgate` block in [`paula_audio_channel.v:149`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L149) |
| **TC64** | [`rtl/minimig/paula_audio_mixer.v:23-33`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_mixer.v#L23-L33) | descendant — the same construction, unchanged. The `audvol` write (NanoMig `paula_audio_channel.v:101-103`) is byte-identical in MiSTer and in TC64 [`rtl/minimig/paula_audio_channel.v:95-97`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L95-L97), but the PWM `volgate` block (NanoMig `:136-150`) **does not exist in TC64**: its audio channel goes from the `intreq` assign straight to the period counter, with only a vestigial, unused `volcntrld` signal left. TC64 has only the defective multiply path |

Two of the four say why in their own comments. Denise's line sits beside NanoMig's
because both implement appendix A's "bit 6 forces maximum" and reach different
answers: Denise masks to seven bits and forces the **value** to `0x40`, 64, while
`paula_audio_mixer` ORs bit 6 into the low six bits and arrives at 63.

## Decision

Clamp rather than OR, which is what the four witnesses do and what the manual's "65
levels" requires. The helper's input is six bits, so the seventh level needs the
helper itself to widen, or the mixer to special-case it:

```verilog
// 64 is a level of its own, not a saturated 63.
.volume(vol0[6] ? 7'd64 : {1'b0, vol0[5:0]}),
```

with `paula_audio_volume`'s `volume` port widened to `[6:0]`. That makes the
multiply path agree with the PWM path already in the same design.

**Confirming a fix.** Both Makefiles default to `NANOMIG ?= ../../nanomig`, the
pinned submodule, so a bare `make` rebuilds the unmodified RTL and reproduces the
reds below whatever you fixed. Rerun against the tree you changed:

```sh
make -C hrm/ch5-audio-register-map test NANOMIG=/path/to/your/NanoMig
make -C hrm/ch5-audio-volume-scale  test NANOMIG=/path/to/your/NanoMig
```

Leave `NANOMIG` off only to reproduce the before-state.

| Package | Before any change (pinned submodule) | After a correct fix |
|---|---|---|
| `ch5-audio-register-map` | `8 passed, 1 failed`: `VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH` | `9 passed, 0 failed` |
| `ch5-audio-volume-scale` | `69 passed, 5 failed`: this finding's `MULTIPLY-PATH-COLLAPSES-VOLUME-64-ONTO-63`, `TABLE-5-9-VOLUME-TO-DECIBEL vol0=64`, `TABLE-5-9-HAS-65-VOLUME-LEVELS`; and **not** this finding's, `CH1-CH2-ROUTE-LEFT` and `CH0-CH3-ROUTE-RIGHT` | `72 passed, 2 failed`: the two routing reds |

So `ch5-audio-volume-scale` staying red is not a failed fix: its two remaining reds
belong to [NM-HRM-002](NM-HRM-002.md), the stereo routing, which nothing suggested
here touches. Read the assertion label before concluding anything.
