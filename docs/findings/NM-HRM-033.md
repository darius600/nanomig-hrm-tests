# NM-HRM-033 — LOLDIS does not disable the long-line toggle on its own [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should VPOSR's long-line bit (bit 7) stop toggling under NTSC when BEAMCON0 bit 11 (LOLDIS) alone is set, as pp. 305 and 309 print, rather than only when VARBEAMEN (BEAMCON0 bit 7) is set as well? Closes when an Amiga specialist either confirms that LOLDIS alone freezes the toggle on ECS Agnus (the RTL then drops `&& varbeamen`, see *Decision*), or names a configuration in which LOLDIS is meant to be inert. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer: the same condition stands there (see the Module row), and a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Scope** | `agnus_beamcounter` at the pinned NanoMig revision; NTSC (`pal=0`), `ecs=1`, VPOSR bit 7 sampled over four consecutive lines, one fresh model per leg. Chip revision: unknown. PAL and other modes: not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 7 — System Control, Appendix C, printed p. 305 (BEAMCON0 bit table, bit 11) and p. 309 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · `P6-BEAMCON0-LOLDIS-BIT11-DISABLES-LONG-LINE-TOGGLE` and the note `P6-COMBINATION-MEASURED` carry `sol:7-appC-beamcon0.c6` / `opus:7-appC-beamcon0.c6` (p. 305); `sol:7-appC-loldis.c1` / `opus:7-appC-beamcon0.c21` (p. 309) are carried by a recorded line of the package, not by an assertion |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L295-L297) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L282-L284); inherited (MiSTer and TC64 have the same code) |
| **Test** | [`hrm/ch7-beamcon0-ecs-control-bits`](../../hrm/ch7-beamcon0-ecs-control-bits): `make -C hrm/ch7-beamcon0-ecs-control-bits test`; the expected red exits nonzero |
| **Assertions** | `P6-BEAMCON0-LOLDIS-BIT11-DISABLES-LONG-LINE-TOGGLE` — red: expected the toggle to stop with LOLDIS alone, observed it still toggles. The package totals 8 passed, 4 failed; this page grades only the P6 assertion. The two-bit measurement beside it is `P6-COMBINATION-MEASURED`, a `note()` (control toggles; LOLDIS + VARBEAMEN frozen) |
| **Verdict of the four** | **unanimous that LOLDIS alone stops the toggle** (WinUAE, vAmiga, Denise, Copperline); NanoMig is the only independent implementation that also requires VARBEAMEN. Hardware status: not measured |
| **Effort** | S: one condition in `agnus_beamcounter.v` (the `if`/`else if` at L295 and L297), drop `&& varbeamen` |

---

## Source requirement

**Appendix C, printed p. 305**, in the `BEAMCON0` bit table:

> `11    LOLDIS     Disable long line/short line toggle`

**Appendix C, printed p. 309**, on the same bit:

> "The Strobe Long Line register (STRLONG) can be disabled if the Disable Long
> Line (LOLDIS) bit is set in the BEAMCON0 register."

Neither sentence names a second condition. Setting bit 11 disables the toggle.

## RTL and measurement

The freeze is gated on two bits, LOLDIS and VARBEAMEN — `if (pal || (loldis && varbeamen)) long_line <= 1'b0; else if (!(loldis && varbeamen)) long_line <= ~long_line;`
([`agnus_beamcounter.v#L292-L300`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L292-L300)).

Measured, VPOSR's LOL bit over four consecutive lines:

| BEAMCON0 | LOL bit |
|---|---|
| `0` — the control | toggles line by line |
| LOLDIS alone | **still toggles** |
| VARBEAMEN alone | still toggles |
| LOLDIS + VARBEAMEN | frozen |

## Risks and unknowns

- **Hardware was not measured.** Whether `&& varbeamen` was deliberate is open: on real ECS Agnus, is there a configuration in which LOLDIS is meant to be inert? If not, this is a plain transcription defect that has been inherited unchanged.
- **Copperline reads VARBEAMEN as a second way to stop the toggle**, a programmed line length having no long/short alternation to make. Denise takes the line length from HTOTAL under VARBEAMEN and still adds the toggled long-line cycle to it ([`events.cpp:479-480`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L479-L480)), and WinUAE's per-line toggle does not look at VARBEAMEN. This page does not rest on that reading.
- Only NTSC with `ecs=1` was measured.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:10970-10975`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10970-L10975) | toggles `lol` per line only if PAL and LOLDIS are both clear; `lol` is what VPOSR returns at bit 7 |
| **vAmiga** | [`Core/Components/Agnus/AgnusRegs.cpp:566-567`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L566-L567) | under NTSC, `lolToggle = !loldis` |
| **Denise** | [`emulation/libami/agnus/register.cpp:565`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L565) | `lolToggle = !(value & LOLDIS) && !(value & BEAM_PAL)` |
| **Copperline** | [`src/chipset/agnus.rs:855-863`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L855-L863) | toggles only under NTSC, with no programmable line length and `!loldis_active()` |
| **MiSTer** | [`rtl/agnus_beamcounter.v:282-285`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L282-L285) | descendant — identical to NanoMig: the freeze condition is `pal \|\| (loldis && varbeamen)` |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:343-346`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L343-L346) | descendant — the same AND, apart from the renamed `pal` signal (`displaypal`): `if (displaypal \|\| (loldis && varbeamen))` |

WinUAE, vAmiga and Denise combine LOLDIS with the video standard and nothing
else. Both Minimig lines carry the extra term unchanged, which is expected of
descendants and says nothing about the manual.

## Decision

Drop `varbeamen` from the condition:

```verilog
if (pal || loldis)
	long_line <= 1'b0;
else
	long_line <= ~long_line;
```

Copperline's reading suggests the fuller form may be `pal || loldis || varbeamen`,
but that is a second proposition and this finding does not rest on it. The
decision belongs to an Amiga specialist, not to this repository. The finding
closes as the *What is asked of a reader* row says; the red to re-run is
`P6-BEAMCON0-LOLDIS-BIT11-DISABLES-LONG-LINE-TOGGLE` with the command in the
Test row, and the package's other three reds grade other bits.
