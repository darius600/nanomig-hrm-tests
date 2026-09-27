# NM-HRM-034 — VPOSR reports the video standard in bit 12 of an OCS configuration

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. The manual gives the value NanoMig reads: Appendix C p. 299 lists `10` in the identification field for the 8361 and 8370 NTSC Agnus, and Appendix A p. 264 says unused bits of a read-only register have no value to rely on. |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L108-L110) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L114-L116) |
| **Test** | [`hrm/ch7-beam-position-word-layout`](../../hrm/ch7-beam-position-word-layout) |
| **Chapter** | 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | `VPOSR-OCS-BITS-14-1-UNUSED (ntsc=1)` — red. `VPOSR-OCS-BITS-14-1-UNUSED (ntsc=0)` is the green control, same label stem |
| **Error class** | **a silence read as a requirement** |
| **Verdict of the four** | **2–2**: WinUAE and Denise report `$10` in bits 14-8 of an OCS NTSC read, p. 299's value for the 8361 and 8370, exactly as NanoMig does; vAmiga, which models only the PAL parts 8367 and 8371, and Copperline report `$00`. All four build bits 14-8 as a chip identification; none treats them as unused |

---

## Why it was withdrawn

`VPOSR-OCS-BITS-14-1-UNUSED` requires bits 14-1 of a VPOSR read to be zero with
`ecs=0`, at `ntsc=0` and at `ntsc=1`. It grades **printed p. 216**, Table 7-5:

> "Bits 14-1 Unused"

and **Appendix A p. 285**, whose bit row for VPOSR has dashes between LOF at
bit 15 and V8 at bit 0, under a register marked `A(E)`, changed in ECS. Neither
page names a chipset, and neither gives the band a value.

**The manual says what "unused" means in a read-only register**, and it is not
zero. Appendix A's own preamble, **p. 264**:

> "Similarly, do not rely on the values of unused bits when reading from a read only register."

and the programming guidelines, **p. 14**:

> "Never write data to, or interpret data from the unused bits or addresses in the custom chip space. To be software-compatible with future chip revisions, all undefined bits must be set to zeros on writes, and must be masked out on reads before interpreting the contents of the register."

VPOSR is read-only (p. 216: "VPOSR Read-only").

**The manual also says what this band holds, on two further pages.** Chapter 8,
**printed p. 238**, on the same register:

> "Bits 14-1 Chip ID code. Do not depend on value!"

and **Appendix C p. 299**, bit by bit and part by part:

> "Use LOF I6 I5 I4 I3 I2 I1 I0 LOL -- -- -- -- v10 v9 V8"

> "I0-I6 (bits 8-14) provide the chip identification. At present there are four possible settings."
>
> "A value of 20 or 30 indicates that the enhanced HighRes Agnus is present."

> "8361 (regular NTSC) or 8370 (fat NTSC) = 10 for NTSC Agnus"
>
> "8367 (regular PAL) or 8371 (fat PAL) = 00 for PAL Agnus"

The sentence on `20` and `30` puts `10` and `00` outside the enhanced parts, so
the 8361, 8370, 8367 and 8371 are the OCS Agnus. Read as hexadecimal, as all
four implementations read the enhanced row (`20` at bit 13: `0x2000` in WinUAE,
[`custom.cpp:2401`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2401),
and vAmiga, [`Agnus.cpp:270`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L270);
`1 << 13` in Denise, [`agnus.cpp:745`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L745);
`0x20` in Copperline, [`agnus.rs:95`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L95)),
`10` is I4, bit 12.
**p. 299 therefore predicts `$10` in bits 14-8 of an OCS NTSC Agnus and `$00`
in those of an OCS PAL Agnus**, which is what NanoMig reads: `0x1001` and
`0x0001`, V8 set in both.
The zero the `ntsc=1` leg requires is printed nowhere; it is p. 216's "Unused"
read as a value.

**The suite already grades p. 299's reading, and it is green.**
[`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking)
asserts `VPOSR-IDENTIFICATION-FIELD-BITS-14-7` on the same module: bits 13-12
read `$00`/`$10`/`$20`/`$30` across the four (`ecs`, `ntsc`) combinations, `$10`
at `ecs=0`, `ntsc=1`. For that input the `ntsc=1` leg here requires bit 12 to
be 0. No implementation passes both, and none that does what p. 299 says can
pass this leg.

---

## What NanoMig does

The read mux drives bit 12 from the `ntsc` input and bit 13 from `ecs`:

[`src/minimig-aga/agnus_beamcounter.v#L108-L110`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L108-L110)

```verilog
	if (reg_address_in[8:1]==VPOSR[8:1] || reg_address_in[8:1]==VPOSW[8:1])
		data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};
```

`ntsc` is a module input, not a function of `ecs`, so with `ecs=0` and `ntsc=1`
bit 12 reads back as 1. Bits 14-8 then read `$10`, the identification p. 299
gives for the 8361 and 8370; with `ntsc=0` they read `$00`, its value for the
8367 and 8371.

### Measured

`ecs=0`, `aga=0`, the band being bits 14-1 of the readback:

| | band | full word |
|---|---|---|
| `ntsc=0` | `0x0000` — passes | `0x0001` |
| `ntsc=1` | **`0x0800`** — fails | `0x1001` |

The `ntsc=0` leg is the control: same harness, same register, same extraction.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a local checkout
at the pinned revision named in the link.

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:2395-2408`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2395-L2408) | <pre>csbit \|= ecs_agnus ? 0x2000 : 0;<br>...<br>if (currprefs.ntscmode) {<br>&nbsp;&nbsp;&nbsp;&nbsp;csbit \|= 0x1000;<br>}</pre> the ECS id bit is gated on `ecs_agnus`; the NTSC bit, in the next lines, on nothing. This is the default path; an explicitly configured Agnus revision (`if (currprefs.cs_agnusrev >= 0)`) replaces both with that revision's id |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:747`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L747) | the explicit OCS return: <pre>return ((v >> 8) & 1) \| (_lof << 15) \| (ntsc << 12);</pre> |
| **vAmiga** | [`Core/Components/Agnus/Agnus.cpp:266-276`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L266-L276), [`AgnusTypes.h:39-40`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusTypes.h#L39-L40) | `idBits()` returns `0x0000` for both OCS revisions, so bit 12 stays clear. The two revisions, `OCS_OLD` and `OCS`, are commented as the MOS 8367 and the MOS 8371, the PAL parts p. 299 gives `00`; `idBits()` switches on the revision alone, so an NTSC ECS Agnus also reads `0x2000`, where p. 299 gives `30` |
| **Copperline** | [`src/chipset/agnus.rs:89`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L89), [`:95-99`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L95-L99) | <pre>(Self::Ocs, _) => 0x00,</pre> the chipset id is zero for OCS whatever the video standard, while the ECS and AGA arms below it split PAL and NTSC, `0x20`/`0x30` and `0x23`/`0x33`. No comment gives a reason for the OCS arm |
| **MiSTer** | [`rtl/agnus_beamcounter.v:116`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L116) | descendant — differs from NanoMig only in bits 2-0, which read `vpos_rb`, a copy of `vpos` registered two clock enables later, where NanoMig reads `vpos` itself; bits 15-3, bit 12 included, are identical: `data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos_rb[10:8]};` |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:132`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L132) | descendant — byte-identical to NanoMig, `vpos` and all: `data_out[15:0] = {long_frame,1'b0,ecs,ntsc,2'b00,{2{aga}},long_line,4'b0000,vpos[10:8]};` — closer than MiSTer, whose only difference is the delayed `vpos_rb` in bits 2-0. `ntsc` is ungated by `ecs` in both |

**Two of the four independent implementations read an OCS NTSC Agnus as p. 299
does, exactly as NanoMig does.** WinUAE's is the sharpest statement: it gates the
ECS identification bit on `ecs_agnus` and the NTSC bit on nothing, five lines
apart, so an OCS NTSC machine reads `0x1000`, p. 299's `10`. vAmiga's `$00` is
p. 299's value for the only OCS parts it models, the PAL 8367 and 8371, and it
keeps its ids when switched to NTSC. Copperline gives `$00` to an OCS Agnus in
either standard, while its ECS arms follow p. 299's `20`/`30`. Both maintained
Minimig lines do what NanoMig does, which is expected of descendants and adds
nothing to that count — TC64 byte-identical, MiSTer differing only in the
vertical-position bits 2-0.

### And nobody treats bits 14-1 as unused

The long-line bit at 7 splits the other way:

| | drives LOL at bit 7 in OCS? |
|---|---|
| NanoMig | yes |
| vAmiga | yes — `if (pos.lol) result \|= 0x0080;`, ungated |
| Copperline | yes — `u16::from(lol) << 7` |
| WinUAE | **no** — `if (!ecs_agnus) vp &= 1;` |
| Denise | **no** — the OCS return omits it |

So **every** implementation read drives some bit of the band p. 216 calls
unused. They disagree about bit 7, which p. 299's bit row names LOL; bits 14-8
all four build as an identification, and split only on its OCS NTSC value.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 30 — agrees with the manual:

  > "USE LOF--~-------------------------------------- V8"
  <!-- corpus: agnus-8371-spec/p030.txt -->

  VPOSR's bit row on the 8371 has LOF at bit 15, V8 at bit 0 and nothing between: p. 285's layout, which gives bits 14-1 no value. The 8371 is the PAL part, for which p. 299 gives the identification 00, so a blank band is also p. 299's value for it, and NanoMig's ntsc=0 leg reads it. The NTSC part the ntsc=1 leg models, the 8370, is not described here.

Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989.

---

## Why this entry is kept

p. 216 and p. 285 are where a reader looks VPOSR up, and both leave bits 14-1
blank. A test that reads that blank as zero is red on this core in every
configuration but OCS PAL. The values are on p. 299, in the
appendix on ECS, and the name of the band on p. 238, in chapter 8's light-pen
section; what "unused" means is on p. 264 and p. 14.

**"Unused" is not a value.**
