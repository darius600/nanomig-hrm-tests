# NM-HRM-001 — PAL frame has one line too many [withdrawn]

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | p. 216 prints "Allows PAL line counts (313) to appear in PAL versions of the Amiga", and NanoMig counts 313 end-of-line pulses from reset to the first end-of-frame with the long-frame bit at its reset value 1 |
| **What is asked of a reader** | Nothing. With the long-frame bit set, as NanoMig sets it at reset, a PAL field is 313 lines, the count p. 216 prints; a test that grades 312 must clear the bit first. |
| **Withdrawal reason** | The test graded 312 positions with the long-frame bit at its reset value 1; NanoMig counts 313 then, which p. 216 prints, and 312 with the bit cleared |
| **Error class** | none |
| **Replacement** | Green: `PAL-FIELD-312-NTSC-FIELD-262-LINES` (PAL) in [`hrm/ch3-beam-counters-and-blanking`](../../../hrm/ch3-beam-counters-and-blanking), two fields of exactly 312 lines with the bit cleared first; `CONTROL-PAL-DEFAULT-FIELD-LENGTH-313-LINES` in [`hrm/ch3-field-geometry`](../../../hrm/ch3-field-geometry), 313 end-of-line pulses from reset to the first end-of-frame |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4495f86` `:87`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L87), [NanoMig `4495f86` `:344`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L344); MiSTer and TC64 have the same `VTOTAL_PAL_VAL` and the same reset default: inherited |
| **Test** | `make -C hrm/ch3-field-geometry test` (`10 passed, 0 failed`); `make -C hrm/ch3-beam-counters-and-blanking test` (`28 passed, 5 failed`; the five reds are other assertions, and an expected red exits nonzero) |
| **Chapter** | 3 — Playfield, printed p. 40, and 7 — System Control Hardware, printed p. 216 (Table 7-5) · *Amiga Hardware Reference Manual*, 3rd ed., 1991. `PAL-FIELD-312-NTSC-FIELD-262-LINES`: `opus:3-video.c04`, `opus:3-video.c06`, `opus:3-video.c08`, `opus:3-video.c11`, `sol:3-video.c11`, `sol:3-video.c14`, `sol:3-video.c7`, `sol:3-video.c9`, p. 40. `CONTROL-PAL-DEFAULT-FIELD-LENGTH-313-LINES` and `PRECONDITION-LONG-FRAME-DEFAULT-AT-RESET` carry no claim id. The p. 216 sentence below is `sol:7-beam.c8` and `opus:7-bp.c9`, which no assertion of the two packages carries (recorded-only in the coverage ledger) |
| **Assertions** | `PAL-FIELD-312-NTSC-FIELD-262-LINES` (PAL) green; `CONTROL-PAL-DEFAULT-FIELD-LENGTH-313-LINES` green; `PRECONDITION-LONG-FRAME-DEFAULT-AT-RESET` green |
| **Verdict of the four** | **all four** run a 313-line PAL field with the long-frame bit set; vAmiga and Denise start with it set, as NanoMig does, Copperline ignores it outside interlace, and WinUAE clears it at hard reset. Hardware: unknown |

---

## Why it was withdrawn

The test asserted that a normal PAL non-interlace field is 312 positions, `vpos`
0..311. NanoMig measures `max_vpos = 312` and 313 end-of-line pulses.

**Table 7-5 sanctions 313.** Printed p. 216, VPOSR bit 0:

> Bit 0 High bit of the vertical position (V8). Allows PAL line counts (313) to appear in PAL versions of the Amiga.

**p. 40 prints 312:**

> With a PAL system, the beam produces 312 lines, of which 256 are normally visible. Each complete set of lines (262/NTSC or 312/PAL) is called a display field.

NanoMig gives both: the long-frame bit is set at reset
([`:344`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L344)),
which makes 313 lines, and with it cleared by a VPOSW write
`PAL-FIELD-312-NTSC-FIELD-262-LINES` measures 312.

**Risks and unknowns.** Neither p. 40 nor p. 216 says which count a PAL field has
at reset. The claim would be filed again if a printed page required 312 lines for
a PAL field with the long-frame bit set; neither page does. No measurement on
real hardware is cited.

---

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:10926`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10926) | `if ((vpos == maxvpos + lof_store) \|\| (vpos == maxvpos_long)) {` follows the long-frame bit: 313 lines with it set, 312 with it clear, and a hard reset clears it (`lof_store = lof_display = 0;`, [`:6739`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6739)) |
| **vAmiga** | [`Core/Components/Agnus/Beam.h:86`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.h#L86) | `isize vCntPal() const { return lof ? 313 : 312; }` follows the long-frame bit |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:178`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L178) | `lines = ntsc ? 261 : 311;` frame restarts at `vPos == lines + lof` |
| **Copperline** | [`src/chipset/agnus.rs:7`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L7) | `pub const PAL_LINES: u32 = 313;` non-interlaced fields are all long frames |
| **MiSTer** | [`rtl/agnus_beamcounter.v:87`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L87), [`:330`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L330) | descendant — the same `VTOTAL_PAL_VAL = 11'd312 - 11'd1` and the same `long_frame <= 1'b1` reset default NanoMig has; runs a 313-line PAL field for the same reason |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:103`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L103), [`:386`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L386) | descendant — the same `VTOTAL_PAL_VAL` and the same `long_frame_r <= 1'b1` reset default; runs a 313-line PAL field for the same reason |
