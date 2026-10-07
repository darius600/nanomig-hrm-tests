# NM-HRM-015 — BRDNTRAN is declared and never connected

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | pp. 296, 297 and 307 place BRDNTRAN among the genlock features and Commodore's Denise specification ties bit 4 to the `ZD` pin, which NanoMig's Denise has no port for; `make -C hrm/ch3-horizontal-display-window test` records the seven claims as absent at this boundary |
| **What is asked of a reader** | Nothing. BRDNTRAN keeps the border from being replaced by a genlock's video; the manual gives it no effect on the border's colour, and NanoMig's Denise has no genlock output for it to act on. |
| **Withdrawal reason** | No printed page gives BPLCON3 bit 4 an effect on the colour the border is drawn in: pp. 296, 297 and 307 name it among the genlock features, whose signal is Denise's `ZD*` pin |
| **Error class** | **a name read as a requirement** |
| **Replacement** | Green control: `G11-BRDRBLNK-WORKS` in [`hrm/ch3-horizontal-display-window`](../../../hrm/ch3-horizontal-display-window), bit 5 of the same register blanking the border on the same fixture; `D-BRDNTRAN-HAS-NO-EFFECT` in the same package is now a `record()` |
| **Module** | `denise.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v); inherited (MiSTer has the same code, both lines commented out; TC64 differs: both lines are live, and `brdntran` appears nowhere else in its file) |
| **Test** | `make -C hrm/ch3-horizontal-display-window test` — `13 passed, 0 failed`; `D-BRDNTRAN-HAS-NO-EFFECT` is a `record()` that declines its seven claims as absent at this boundary, because BRDNTRAN acts on the `ZD*` background indicator (p. 380), which the `denise` fixture does not have. Related packages: `make -C hrm/ch7-bplcon2-live-bits-and-ecs test`, `make -C aa/aa-bplcon-bits-at-denise test` |
| **Chapter** | 3 — Playfield; printed pp. 46, 89, 260, 296, 297, 307 and Appendix J p. 380 · *Amiga Hardware Reference Manual*, 3rd ed., 1991. `D-BRDNTRAN-HAS-NO-EFFECT`: `opus:C-gen.c09` p. 296, `sol:C-genlock.c4` p. 296, `opus:C-gen.c12` p. 297, `sol:C-genlock-ext.c16` p. 307, `opus:C-ecsbit.b04` p. 307, `sol:C-genlock-ext.c3` p. 307, `opus:C-gen.c15` p. 307. `G11-BRDRBLNK-WORKS`: `opus:C-gen.c08` p. 296, `sol:C-genlock.c3` p. 296, `opus:C-ecsbit.b05` p. 307, `sol:C-genlock-ext.c4` p. 307 |
| **Assertions** | `D-BRDNTRAN-HAS-NO-EFFECT`: was an `expect()` requiring the border's colour with BPLCON3 = `$0010` to differ from the colour with `$0000`, red; now a `record()` with no pass/fail state. `G11-BRDRBLNK-WORKS`: green |
| **Verdict of the four** | **none changes the border's colour**: WinUAE and Copperline implement `BRDNTRAN` as genlock keying only, Copperline with a test that asserts the colour unchanged; vAmiga and Denise model no genlock and do not read the bit. Hardware: not measured |

---

## Why it was withdrawn

The assertion required BPLCON3 bit 4 to change the colour the border is drawn
in. The manual does not say that. Every page that names or describes the bit
places it among the genlock features: p. 296 lists BorderNotTransparent as one
of "four new genlock features", p. 297 gives "Borders may be explicitly
transparent or opaque" as part of the expanded genlock control, and p. 307
prints the bit in the section *Genlock Extensions*. Transparent and opaque are
the genlock's terms: with a genlock the background colour, which is also the
border's, is replaced by the external video (pp. 46, 89), and Denise's pin list
has an output for that background, `ZD*` (p. 380). An opaque border is one the
external video does not replace. Its colour is still COLOR00, with or without
the bit.

NanoMig's Denise drives `red`, `green` and `blue`, the bus and two mode bits,
and has no `ZD` pin, so at its outputs the manual predicts no change when bit 4
is set. Wiring `brdntran` into the border composition the way `brdrblnk` reaches
`t_blank` would change the border's colour, which no page, no Commodore
document and no implementation does.

**Condition for filing again.** A printed page or a Commodore document that
gives BRDNTRAN an effect on the colour the border is drawn in. A claim about the
other genlock keying controls p. 307 prints (BPLCON2's `ZDBPSEL2`-`ZDBPSEL0`,
`ZDBPEN`, `ZDCTEN`) meets the same absence: NanoMig decodes none of them, nor
BPLCON3's `BRDNTRAN`, because it has no genlock output for them to drive. `hrm/ch7-bplcon2-live-bits-and-ecs` records the BPLCON2 ones on ECS
without asserting them, and `aa/aa-bplcon-bits-at-denise` declines its claims
about the `ZD` pin and other chip pins as absent at this boundary. The genlock
attaches at the 23-pin connector, and the register does not take a claim about
that connector as a finding, because the board has no counterpart to it
([`findings.md`](../../findings.md#two-layers-are-under-test-and-a-finding-must-say-which)).

---

## What the HRM says

**Printed p. 296**, the new features of the ECS:

> "The enhanced Denise chip provides the following four new genlock features:
> Chromakey BitPlaneKey BorderBlank BorderNotTransparent"

> "BorderBlank creates a transparent "frame" surrounding the active area.
> BorderNotTransparent makes an opaque "frame" surrounding the active area."

**Printed p. 297**:

> "Genlock control has been expanded for more flexibility. Borders may be
> explicitly transparent or opaque, color registers other than zero can control
> video overlay and a bitplane mask may be used for special-purpose video
> masking concurrently with the other genlock features."

**Printed p. 307**, section *Genlock Extensions*:

> "The border areas surrounding the active display window may also be set to be
> opaque or transparent."

```
   05                                   BRDRBLNK   Border blank
   04                                   BRDNTRAN   Border opaque
```

**What a genlock does with the background**, printed p. 46 and p. 89:

> "The background color shows in any area on the display where there is no
> other object present and is also displayed outside the defined display
> window, in the border area."

> "When you use genlock, the background color is replaced by the display from
> this external video source."

**Appendix J, printed p. 380**, the Denise pin list:

```
 33      ZD*                         Background indicator       0
```

**Printed p. 260**, where the genlock attaches:

> "All Amigas provide a 23-pin connector on the back. This jack contains
> video outputs and inputs for external genlock devices."

No page gives `BRDNTRAN` an effect on the colour the border is drawn in.

---

## What NanoMig does

`brdntran` is commented out at its declaration
([`src/minimig-aga/denise.v#L234-L235`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L234-L235))
and at its assignment, `//assign brdntran = bplcon3[4] & ecsena;`
([`src/minimig-aga/denise.v#L253-L254`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L253-L254)),
while its neighbour `brdrblnk` is live and paints the border black at the colour
outputs
([`src/minimig-aga/denise.v#L515-L518`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L515-L518)).

The module has no output a genlock could key on. Its outputs are the bus, the
colour, and two mode bits
([`src/minimig-aga/denise.v#L42-L51`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L42-L51)),
and BPLCON2's genlock bits 14-10 are read nowhere: the file uses bits 8 and 9
([`denise.v#L219-L226`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L219-L226))
and 6-0
([`denise.v#L383`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L383))
only.

`G11-BRDRBLNK-WORKS`, green in
[`hrm/ch3-horizontal-display-window`](../../../hrm/ch3-horizontal-display-window),
shows bit 5 of the same register blanking the border on the same fixture.

---

## What the others say

| | |
|---|---|
| **WinUAE** | **implements it**, as genlock keying only: with ECSENA set on ECS Denise, `bordertrans = (bplcon3_denise & 0x10) && ecsena;` ([`drawing.cpp:3117`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L3117)) switches its genlock emulation on, and the border's genlock pixel is made opaque, `// border color with BRDNTRAN bit set = not transparent` ([`drawing.cpp:1246-1255`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L1246-L1255)); the border's colour does not change |
| **vAmiga** | does not read the bit and models no genlock. Its only BPLCON3 border control is bit 5, `static bool brdrblnk(u16 v) { return !!GET_BIT(v, 5); }` ([`Core/Components/Denise/Denise.h:1068`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.h#L1068)), which draws the border black ([`Denise.cpp:1324-1325`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1324-L1325)) |
| **Denise** | does not read the bit and models no genlock. It decodes two bits of BPLCON3, `brdrblnk = ecsena() && (bplCon3 & 0x20);` and `extblanken = ecsena() && (bplCon3 & 1);` ([`emulation/libami/video/denise.cpp:414-415`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L414-L415)) |
| **Copperline** | **implements it**, with its own test, as genlock keying only: while it is set the border is never genlock-transparent ([`src/video/bitplane.rs:997`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L997)); the test ([`src/video/bitplane/tests.rs:1389-1415`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L1389-L1415)) expects the border's colour unchanged and only its alpha opaque |
| **MiSTer** | descendant — `rtl/denise.v:197-198,216-217` — same as NanoMig: `brdntran` is declared and assigned but both lines stay commented out |
| **TC64** | descendant — `rtl/minimig/denise.v:205,224` — both lines are live, `wire brdntran;` and `assign brdntran = bplcon3[4] & ecsena;`, and `brdntran` appears nowhere else in the file; its Denise has no genlock output either |

**No implementation changes the border's colour for this bit.** WinUAE and
Copperline implement `BRDNTRAN` as genlock keying: the border stops being
transparent to the overlay, and its colour stays COLOR00. All four paint the
border black for bit 5, `BRDRBLNK`, as NanoMig does: WinUAE
`if (borderblank) { bordercolor = fullblack;`
([`drawing.cpp:3018-3019`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L3018-L3019)),
Denise `color = (_hBlank || _brdrblnk) ? 0 : colors[0];`
([`denise.cpp:800`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L800)),
Copperline by taking colour latch 0 in the border
([`src/video/bitplane/output.rs:164-168`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/output.rs#L164-L168)),
and vAmiga as its row says.

### What other Commodore documents say

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 5 — agrees with the manual:

  > "BRDNTRAN= "border area" is non-transparent(ZD pin is low when border is displayed."
  <!-- corpus: denise-spec-252126/p005.txt -->

  > "BRDRBLNK= "border area" is blanked instead of color(0)."
  <!-- corpus: denise-spec-252126/p005.txt -->

  The specification defines the two border bits by two different outputs: bit
  5 by the border's colour, bit 4 by the `ZD` (background indicator) pin, which
  is genlock keying. It names no colour change for bit 4.
