# NM-HRM-033 — LOLDIS does not disable the long-line toggle on its own

| | |
|---|---|
| **Status** | open · red |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L295-L297) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L282-L284) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's, and a fix there reaches NanoMig |
| **Test** | [`hrm/ch7-beamcon0-ecs-control-bits`](../../hrm/ch7-beamcon0-ecs-control-bits) |
| **Chapter** | 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | `P6-BEAMCON0-LOLDIS-BIT11-DISABLES-LONG-LINE-TOGGLE` — red. The two-bit measurement beside it is `P6-COMBINATION-MEASURED`, a `note()` |
| **Verdict of the four** | **all four implement LOLDIS on its own**; NanoMig is alone in also requiring VARBEAMEN |

---

## What the HRM says

**Appendix C, printed p. 305**, in the `BEAMCON0` bit table:

> `11    LOLDIS     Disable long line/short line toggle`

**Appendix C, printed p. 309**, on the same bit:

> "The Strobe Long Line register (STRLONG) can be disabled if the Disable Long
> Line (LOLDIS) bit is set in the BEAMCON0 register."

Neither sentence names a second condition. Setting bit 11 disables the toggle.

---

## What NanoMig does

The freeze is gated on **two** bits, LOLDIS **and** VARBEAMEN:

[`src/minimig-aga/agnus_beamcounter.v#L292-L300`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L292-L300)

```verilog
			if (pal || (loldis && varbeamen))
				long_line <= 1'b0;
			else if (!(loldis && varbeamen))
				long_line <= ~long_line;
```

### Measured

NTSC (`pal=0`), `ecs=1`, VPOSR's LOL bit sampled over four consecutive lines, one
fresh model per leg:

| BEAMCON0 | LOL bit |
|---|---|
| `0` — the control | toggles line by line |
| LOLDIS alone | **still toggles** |
| VARBEAMEN alone | still toggles |
| LOLDIS + VARBEAMEN | frozen |

---

## How this was nearly missed

The package originally asserted the **two-bit** case — LOLDIS together with
VARBEAMEN freezes the toggle — and that assertion passed. The single-bit case was
measured all along and printed as a `note()`, graded by nothing.

That is an expected value taken from the implementation rather than the page, and
this package's own `SPECIFICATION.md` had already said the right thing: *"setting
bit 11 (LOLDIS) stops the line-length toggling"*. The specification is committed
before the RTL is opened precisely so this cannot happen; it happened anyway, at
the point where the assertion was written.

It was found by the independent validation at stage 8, which read the printed
page and the assertion side by side and asked which condition the page states.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a local checkout
at the pinned revision named in the link.

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:2054`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2054) | <pre>linetoggle = !(new_beamcon0 & BEAMCON0_PAL) && !(new_beamcon0 & BEAMCON0_LOLDIS);</pre> |
| **vAmiga** | [`Core/Components/Agnus/AgnusRegs.cpp:566-567`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L566-L567) | <pre>bool loldis = GET_BIT(value, 11);<br>if (pos.type == TV::NTSC) pos.lolToggle = !loldis;</pre> |
| **Denise** | [`emulation/libami/agnus/register.cpp:565`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L565) | <pre>lolToggle = !(value & LOLDIS) && !(value & BEAM_PAL);</pre> |
| **Copperline** | [`src/chipset/agnus.rs:855-863`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L855-L863) | <pre>matches!(self.video_standard, VideoStandard::Ntsc)<br>&nbsp;&nbsp;&nbsp;&nbsp;&& self.programmable_line_cck().is_none()<br>&nbsp;&nbsp;&nbsp;&nbsp;&& !self.loldis_active()</pre> |
| **MiSTer** | [`rtl/agnus_beamcounter.v:282-285`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L282-L285) | descendant — identical to NanoMig: the freeze condition is `pal || (loldis && varbeamen)`, the same AND |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:343-346`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L343-L346) | descendant — the same AND, byte for byte apart from the renamed `pal` signal (`displaypal` there, matching TC64's own naming convention, not a behavioural change): `if (displaypal || (loldis && varbeamen))`. All three require both bits; a fix that drops `varbeamen` diverges from both lines equally |

**All four independent implementations AND LOLDIS with the video standard and
nothing else.** NanoMig is the only *independent* implementation read that
requires VARBEAMEN as well — both maintained Minimig lines inherit the same
extra term unchanged, which is expected of descendants and says nothing about
the manual.

Copperline is worth reading closely, because it inverts NanoMig's relationship:
VARBEAMEN **also** stops the toggle there — a programmable line length has no
long/short alternation to make. So where NanoMig needs both bits, Copperline
treats either as sufficient. That is the opposite of an AND, from the one
implementation that models VARBEAMEN's effect on line length in detail.

---

## Suggested change

Drop `varbeamen` from the condition:

```verilog
			if (pal || loldis)
				long_line <= 1'b0;
			else
				long_line <= ~long_line;
```

Copperline's reading suggests the fuller form may be `pal || loldis ||
varbeamen` — a programmed line length has no toggle to perform — but that is a
second proposition and this finding does not rest on it.

---

## What holds the finding up

- **The control passes on the same measurement basis**: with `BEAMCON0 = 0` the
  LOL bit toggles line by line, measured through the same VPOSR readback, on a
  fresh model.
- **The mutation moves the assertion.** Adding VARBEAMEN to the stimulus — the
  expected value untouched — turns the assertion green, so it responds to what
  the module is asked to do rather than being stuck red.
- **The neighbouring leg rules out the obvious alternative reading**: VARBEAMEN
  alone also leaves the toggle running, so the RTL is not treating VARBEAMEN as
  the real control with LOLDIS as a modifier.

---

## For the specialists

There is no ambiguity in the printed page here and no disagreement among the
implementations, which is unusual for this chapter. The question worth asking is
whether the `&& varbeamen` was deliberate — on real ECS Agnus, is there a
configuration in which LOLDIS is meant to be inert? If not, this is a plain
transcription defect that has been inherited unchanged.
