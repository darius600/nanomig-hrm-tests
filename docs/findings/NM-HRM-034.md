# NM-HRM-034 — VPOSR reports the video standard in bit 12 of an OCS configuration

| | |
|---|---|
| **Status** | open · red — **contested**, see *What the others say* |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L108-L110) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L114-L116) |
| **Upstream** | **MiSTer** `rtl/agnus_beamcounter.v:115` — differs from NanoMig only in the readback signal's name (`vpos_rb` for `vpos`); the expression and bit positions are identical: `data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos_rb[10:8]};`. **TC64** `rtl/minimig/agnus_beamcounter.v:132` — byte-identical to NanoMig, `vpos` and all: `data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};`. `ntsc` is ungated by `ecs` in both. If the printed page is followed, a fix diverges from all three equally |
| **Report to** | **nobody yet** — two of the four implementations do the same thing; see *For the specialists* |
| **Test** | [`hrm/ch7-beam-position-word-layout`](../../hrm/ch7-beam-position-word-layout) |
| **Chapter** | 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | `VPOSR-OCS-BITS-14-1-UNUSED (ntsc=1)` — red. `VPOSR-OCS-BITS-14-1-UNUSED (ntsc=0)` is the green control, same label stem |
| **Verdict of the four** | **2–2**: WinUAE and Denise expose the video standard in an OCS read exactly as NanoMig does; vAmiga and Copperline do not |

---

## What the HRM says

**Printed p. 216**, and **Appendix A p. 285**, on `VPOSR`: in an OCS machine the
register carries the long-frame bit at 15 and V8 at 0, and **bits 14 through 1
are unused**.

---

## What NanoMig does

The read mux drives bit 12 from the `ntsc` input, and bit 13 from `ecs` — but
only bit 13 is gated by the chipset:

[`src/minimig-aga/agnus_beamcounter.v#L108-L110`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L108-L110)

```verilog
	if (reg_address_in[8:1]==VPOSR[8:1] || reg_address_in[8:1]==VPOSW[8:1])
		data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
```

`ntsc` is a module input, not a function of `ecs`, so with `ecs=0` and `ntsc=1`
bit 12 reads back as 1.

### Measured

`ecs=0`, `aga=0`, the band being bits 14-1 of the readback:

| | band | full word |
|---|---|---|
| `ntsc=0` | `0x0000` — passes | |
| `ntsc=1` | **`0x0800`** — fails | `0x1001` |

The `ntsc=0` leg is the control: same harness, same register, same extraction.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a local checkout
at the pinned revision named in the link.

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:2399-2408`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2399-L2408) | <pre>csbit \|= ecs_agnus ? 0x2000 : 0;<br>...<br>if (currprefs.ntscmode) {<br>&nbsp;&nbsp;&nbsp;&nbsp;csbit \|= 0x1000;<br>}</pre> the ECS id bit is gated on `ecs_agnus`; the NTSC bit, in the next lines, on nothing |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:747`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L747) | the explicit OCS return: <pre>return ((v >> 8) & 1) \| (_lof << 15) \| (ntsc << 12);</pre> |
| **vAmiga** | [`Core/Components/Agnus/Agnus.cpp:266-276`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L266-L276) | `idBits()` returns `0x0000` for both OCS revisions, so bit 12 stays clear |
| **Copperline** | [`src/chipset/agnus.rs:89`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L89) | <pre>(Self::Ocs, _) => 0x00,</pre> the chipset id is zero for OCS whatever the video standard |

**Two of the four do exactly what NanoMig does.** WinUAE's is the sharpest
statement: it gates the ECS identification bit on `ecs_agnus` and the NTSC bit on
nothing, in adjacent lines — which is a choice, not an oversight.

### And nobody treats bits 14-1 as unused

The long-line bit at 7 splits the other way:

| | drives LOL at bit 7 in OCS? |
|---|---|
| NanoMig | yes |
| vAmiga | yes — `if (pos.lol) result \|= 0x0080;`, ungated |
| Copperline | yes — `u16::from(lol) << 7` |
| WinUAE | **no** — `if (!ecs_agnus) vp &= 1;` |
| Denise | **no** — the OCS return omits it |

So the band the manual calls unused is driven by **every** implementation read,
and they disagree about which bits. That is the honest shape of this finding.

---

## Suggested change

If the printed page is to be followed, gate the NTSC bit the way the ECS bit is
already gated:

```verilog
		data_out[15:0] = {long_frame,1'b0,ecs,ecs & ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
```

**This is the least confident suggestion in the chapter.** Two independent
implementations expose the video standard in an OCS read, one of them
deliberately, and software that identifies the machine by reading VPOSR may
depend on it. A change here could break more than it fixes.

---

## What holds the finding up

- **The control passes on the same basis**: the `ntsc=0` leg of the identical
  harness reads a zero band.
- **The measurement is reproduced, not adopted**: the package re-measured the
  band rather than taking the classification's word for it.
- **The mutation is recorded honestly.** This assertion is red at baseline, so
  its record is the `none impossible` form with a stated reason, and the
  baseline colour was checked rather than assumed.

---

## For the specialists

This is the chapter's clearest case of *the manual against the field*. p.216 says
the band is unused in OCS; WinUAE and Denise put the video standard there anyway,
and the other two do not. Nobody leaves the whole band at zero.

The question worth settling on hardware: what does an OCS Agnus actually return
in VPOSR bits 14-1 — zero, the video standard, floating bus, or something that
depends on the part? Until that is known, **this should not be reported to the
RTL's authors as a defect**, which is why the *Report to* row above names nobody.
