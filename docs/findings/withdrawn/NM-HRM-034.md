# NM-HRM-034 — VPOSR reports the video standard in bit 12 of an OCS configuration [withdrawn]

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | NanoMig reads `$10` in VPOSR bits 14-8 at `ecs=0`, `ntsc=1` (full word `0x1001`), the value Appendix C p. 299 prints for the 8361 and 8370 NTSC Agnus, and `VPOSR-IDENTIFICATION-FIELD-BITS-14-7` is green on that input |
| **What is asked of a reader** | Nothing. The manual gives the value NanoMig reads: Appendix C p. 299 lists `10` in the identification field for the 8361 and 8370 NTSC Agnus, and Appendix A p. 264 says unused bits of a read-only register have no value to rely on. |
| **Withdrawal reason** | No page requires VPOSR bits 14-1 to read zero at `ecs=0`, `ntsc=1`: p. 216 and p. 285 call them unused, p. 264 says an unused bit has no value to rely on, and p. 299 prints `10` there for the NTSC part |
| **Error class** | **a silence read as a requirement** |
| **Replacement** | Green: `VPOSR-IDENTIFICATION-FIELD-BITS-14-7` in [`hrm/ch3-beam-counters-and-blanking`](../../../hrm/ch3-beam-counters-and-blanking) (bits 13-12 read `$00`/`$10`/`$20`/`$30` across the four `ecs`, `ntsc` combinations; `$10` at `ecs=0`, `ntsc=1`) |
| **Module** | `agnus_beamcounter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L108-L110) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L114-L116); inherited (MiSTer has the same bit 12 and differs only in bits 2-0, TC64's lines 131 and 132 are byte-identical to lines 109 and 110) |
| **Test** | [`hrm/ch7-beam-position-word-layout`](../../../hrm/ch7-beam-position-word-layout): `make -C hrm/ch7-beam-position-word-layout test`, `11 passed, 0 failed`; [`hrm/ch3-beam-counters-and-blanking`](../../../hrm/ch3-beam-counters-and-blanking): `make -C hrm/ch3-beam-counters-and-blanking test`, `28 passed, 5 failed` (the five reds are other assertions, and an expected red exits nonzero) |
| **Chapter** | 7 — System Control Hardware, printed p. 216 (Table 7-5) and Appendix A p. 285 (VPOSR row); Chapter 8 p. 238; Appendix A p. 264; Chapter 1 p. 14 (programming guidelines); Appendix C p. 299 · *Amiga Hardware Reference Manual*, 3rd ed., 1991. Claim ids: <details><summary>claim ids by assertion (18)</summary> `VPOSR-OCS-BITS-14-1-UNUSED`: `sol:7-beam.c7`, `opus:7-bp.c7` (p. 216), `sol:7-appA-vpos.c5`, `sol:7-appA-vpos.c11`, `opus:7-appA-beam.c10` (p. 285); the two `VPOSR-OCS-BITS-14-1-UNUSED-MEASURED` notes carry no claim id. `VPOSR-IDENTIFICATION-FIELD-BITS-14-7` (p. 299): `sol:C-vposr-bits.c2`, `sol:C-vposr-bits.c3`, `sol:C-vposr-bits.c4`, `sol:C-vposr-bits.c5`, `sol:C-vposr-bits.c6`, `sol:C-vposr-bits.c7`, `sol:C-vposr-bits.c8`, `sol:C-vposr-bits.c9`, `opus:C-vposr.c01`, `opus:C-vposr.c03`, `opus:C-vposr.c06`, `sol:C-chipid.c1`, `sol:C-chipid.c5`</details> |
| **Assertions** | none graded in the ch7 package: `VPOSR-OCS-BITS-14-1-UNUSED` is a `record()` that declines the five ids above, because p. 264 and p. 299 give two readings of the band and neither decides; the band is printed as measured, `0x0000` at `ntsc=0` and `0x0800` at `ntsc=1`. `VPOSR-IDENTIFICATION-FIELD-BITS-14-7` green in the ch3 package |
| **Verdict of the four** | **2–2**: WinUAE and Denise report `$10` in bits 14-8 of an OCS NTSC read, p. 299's value for the 8361 and 8370, exactly as NanoMig does; vAmiga, which models only the PAL parts 8367 and 8371, and Copperline report `$00`. All four build bits 14-8 as a chip identification; none treats them as unused. Hardware: not measured; the 8371 Agnus specification, the PAL part, leaves bits 14-1 without a value, and the 8370 is not described there |

---

## Why it was withdrawn

The withdrawn claim is that bits 14-1 of a VPOSR read must be zero with
`ecs=0`, at `ntsc=0` and at `ntsc=1`. It rests on **printed p. 216**, Table 7-5:

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

**The suite grades p. 299's reading, and it is green.** `VPOSR-IDENTIFICATION-FIELD-BITS-14-7`
in the ch3 package asserts it on the same module: bits 13-12 read `$00`/`$10`/`$20`/`$30`
across the four (`ecs`, `ntsc`) combinations, `$10` at `ecs=0`, `ntsc=1`. For that input the
`ntsc=1` leg here requires bit 12 to be 0. No implementation passes both, and none that
does what p. 299 says can pass this leg.

**Condition for filing again.** The claim would be filed again only if a printed
page gave the VPOSR bits 14-1 of an OCS NTSC Agnus the value zero.

---

## What NanoMig does

The read mux drives bit 12 from the `ntsc` input and bit 13 from `ecs`
([`agnus_beamcounter.v#L108-L110`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_beamcounter.v#L108-L110)).
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

## Risks and unknowns

- **Two printed readings of the band.** p. 216 and p. 285 call bits 14-1 unused;
  p. 238 and p. 299 call them the chip identification. The ch7 package declines to grade
  either reading, and the measured band is printed, not adjusted.
- **No hardware reading.** The `10` is p. 299's printed value, read as
  hexadecimal the way the four read the enhanced row; nothing on this page was measured on a chip.
- **WinUAE** models this on its default path; an explicitly configured Agnus revision
  (`if (currprefs.cs_agnusrev >= 0)`, [`custom.cpp:2395-2408`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2395-L2408)) replaces both identification bits with that revision's id.
- **vAmiga** models only the PAL OCS parts; `idBits()` ([`Agnus.cpp:266-276`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L266-L276)) switches on the revision alone, so an NTSC
  ECS Agnus also reads `0x2000`, where p. 299 gives `30`.
- **Copperline**'s OCS arm ([`agnus.rs:89`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L89)) carries no comment giving a reason for `0x00`.
- **The 8371 specification** describes the PAL part only; the 8370 that the `ntsc=1` leg models is not described there.

---

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:2395-2408`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2395-L2408) | `$10`, as NanoMig: gates the ECS id bit (`0x2000`) on `ecs_agnus` and the NTSC bit (`0x1000`) on nothing, so an OCS NTSC machine reads `0x1000`, p. 299's `10` |
| **vAmiga** | [`Core/Components/Agnus/Agnus.cpp:266-276`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L266-L276), [`AgnusTypes.h:39-40`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusTypes.h#L39-L40) | `$00`: `idBits()` returns `0x0000` for both OCS revisions, `OCS_OLD` and `OCS`, commented as the MOS 8367 and 8371, the PAL parts p. 299 gives `00`; its ids do not change with the video standard |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:747`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L747) | `$10`, as NanoMig: the explicit OCS return is `(ntsc << 12)` in its OR of LOF and V8 |
| **Copperline** | [`src/chipset/agnus.rs:89`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L89), [`:95-99`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L95-L99) | `$00`: `(Self::Ocs, _) => 0x00`, zero for OCS whatever the video standard, while its ECS and AGA arms split PAL and NTSC, `0x20`/`0x30` and `0x23`/`0x33` |
| **MiSTer** | [`rtl/agnus_beamcounter.v:116`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L116) | descendant — differs from NanoMig only in bits 2-0, which read `vpos_rb`, a copy of `vpos` registered two clock enables later; bits 15-3, bit 12 included, are identical |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:132`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L132) | descendant — byte-identical to NanoMig, `vpos` and all; `ntsc` is ungated by `ecs` |

**Two of the four read an OCS NTSC Agnus as p. 299 does, exactly as NanoMig does.**
vAmiga's `$00` is p. 299's value for the only OCS parts it models, the PAL 8367 and
8371. Both maintained Minimig lines do what NanoMig does, which is expected of
descendants and adds nothing to that count.

**None treats bits 14-1 as unused.** All four drive some bit of the band: bits 14-8
are built as an identification, and bit 7, which p. 299's bit row names LOL, splits
them: vAmiga (`if (pos.lol) result |= 0x0080;`) and Copperline (`u16::from(lol) << 7`)
drive it in OCS as NanoMig does; WinUAE (`if (!ecs_agnus) vp &= 1;`) and Denise
(the OCS return omits it) do not.

### What other Commodore documents say

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF page 30 — agrees with the manual:

  > "USE LOF--~-------------------------------------- V8"
  <!-- corpus: agnus-8371-spec/p030.txt -->

  VPOSR's bit row on the 8371 has LOF at bit 15, V8 at bit 0 and nothing between: p. 285's layout, which gives bits 14-1 no value. The 8371 is the PAL part, for which p. 299 gives the identification 00, so a blank band is also p. 299's value for it, and NanoMig's ntsc=0 leg reads it.
