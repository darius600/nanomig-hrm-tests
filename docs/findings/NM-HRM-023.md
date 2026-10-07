# NM-HRM-023 — the ATTACH bit is honoured in the even sprite, not only the odd [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `denise_sprites.v` stop letting bit 7 of the even sprite's SPR0CTL attach a pair when `aga = 0`, as p. 128 prints "valid only for odd-numbered" sprites, where setting it alone changes NanoMig's sprite output? Closes when the `(!aga && attach0)` term at `denise_sprites.v:474` is dropped and `ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE` is green, or when an Amiga specialist records that the original Denise attaches a pair from the even sprite's bit 7, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | NanoMig's `denise_sprites.v` with `aga = 0` (the OCS/ECS mode the package drives), SPR0CTL bit 7 set alone with SPR1CTL bit 7 clear, one position (`0x40`) and one pair of sprite images; `aga = 1` is not graded (there the `!aga` term is zero). Chip revision of the original Denise: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 4 — Sprite Hardware, printed pp. 122 and 128; Appendix A, printed p. 284 (register table, "(odd sprites)") · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE` carries `CH4-p128.c8`, `CH4-sctl.c4`, p. 128, and `CH4-attach.c27`, p. 122; `ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT` carries `CH4-attach.c25`, `CH4-attach.c26`, `CH4-attach.c27`, p. 122; `CODE-0000-YIELDS-TRANSPARENT-PIXEL` carries `CH4-tab45.c17` (Table 4-5, p. 122); `FIXTURE-LIVE-DATA-PATH` carries no claim id; the assertion messages cite no id for p. 284 |
| **Module** | `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L474) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v); inherited (MiSTer and TC64 have the same line) |
| **Test** | [`hrm/ch4-attached-sprites`](../../hrm/ch4-attached-sprites): `make -C hrm/ch4-attached-sprites test`; the expected red exits nonzero |
| **Assertions** | 1 red of 4 graded (3 passed, 1 failed): `ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE` — expected: SPR0CTL bit 7 set alone leaves the sprdata trace identical to the neither-bit-set baseline; observed: the trace differs. Green on the same fixture: `ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT`, `FIXTURE-LIVE-DATA-PATH`, `CODE-0000-YIELDS-TRANSPARENT-PIXEL` |
| **Verdict of the four** | **three of four take the bit from the odd sprite only**, as the manual states; outside the emulators, the 8373R4 dissection stores bit 7 for the odd sprites only — see *What other Commodore documents say*. Hardware: not measured |
| **Effort** | S — `denise_sprites.v` line 474 only — drop the `(!aga && attach0)` term; a narrower condition in its place, if the specialist finds the term was written for the original Denise, is the same line |

---

## Source requirement

The manual says it **twice, on two pages** of chapter 4, and neither sentence admits an
exception; Appendix A's register table, printed p. 284, marks the bit "(odd
sprites)" as well.

**Printed p. 128:**

> "Bit 7 is the attach bit. This bit is valid only for odd-numbered" sprites

**Printed p. 122:**

> "Attachment is in effect only when the ATTACH bit, bit 7 in sprite control word
> 2, is set to 1 in the data structure for the odd-numbered" sprite

The odd sprite's control word carries the attachment, and its data words supply the
high two bits of the combined colour.

## RTL and measurement

[`src/minimig-aga/denise_sprites.v#L474`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L474):

```verilog
if (attach1 || (!aga && attach0)) // sprites are attached -> 15 colors + transparant
```

`attach0` is the **even** sprite's bit, and `!aga` is the only thing gating it.

With `aga = 0`, setting bit 7 of **SPR0CTL** — the even sprite of the pair — changes
the measured sprdata trace on its own, which `ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE`
grades red; the manual requires SPR1CTL. With `aga = 1` the `!aga` term is zero, so the even sprite's bit cannot open the path; the package grades `aga = 0` only. The positive case `ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT`
is green on the same fixture: SPR1CTL bit 7 does attach the pair. `FIXTURE-LIVE-DATA-PATH`
shows the sprite path delivers distinct known values in order before anything reads one.

## Risks and unknowns

- **The `!aga` gate looks deliberate.** Someone wrote a chipset condition around the
  even-sprite term, and it covers the original and the enhanced chip set alike: the
  module takes `aga` and no ECS strap
  ([`denise_sprites.v#L39`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L39),
  `input aga,`). The gate is MiSTer's and TC64's too, not something NanoMig added.
- **The silicon answers half of it.** On the enhanced chip set the 8373R4 stores
  SPRxCTL bit 7 for the odd sprites only, so there the term encodes no behaviour. The
  OCS Denise is not dissected, and none of the four implementations conditions the
  attach bit on the chip set.
- **If the term encodes real OCS Denise behaviour**, the manual's two sentences are
  incomplete for that chip, and the term belongs behind an OCS condition rather than
  `!aga`, which admits the ECS Denise too. If it is a workaround for one title, it
  belongs behind a narrower condition than a chipset strap.
- **The ECS Denise specification's agreement is a table annotation.** Its "(odd Sprites)" is not an explicit statement that the even sprite's bit 7 is ignored, the same strength as the manual's own register table.
- **The dissection is one revision and unfinished.** It speaks for the ECS Denise 8373R4
  only and is a work in progress; the sprite 0/1 cells are traced in the netlist, the
  other three pairs are read from the overview sheet.
- **Copperline is a source reading, not run.**

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2838`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2838) | <pre>dspr[num & ~1].attached = (dspr[num \| 1].ctl & 0x80) != 0;</pre> the **odd** sprite only |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:1130`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1130) | <pre>bool attached = GET_BIT(sprctl[sprite2], 7);</pre> with `sprite2 = 2*pair + 1` — the odd one |
| **Denise** | [`emulation/libami/video/denise.cpp:300`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L300) | <pre>spr.attached = (nr & 1) && (value & 0x80);</pre> **the manual's rule exactly** |
| **Copperline** | [`src/video/bitplane/sprite.rs:1096-1110`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1096-L1110) | reads the bit without the odd-sprite condition: a pair renders attached on any line where either sprite's own bit is set (`attached: ctl & 0x0080 != 0`, `:322`), with no chipset condition |
| **MiSTer** | [`rtl/denise_sprites.v:283`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v#L283) | descendant — byte-identical: `if (attach1 \|\| (!aga && attach0)) // sprites are attached -> 15 colors + transparant`, the same even-sprite term and the same `!aga` gate |
| **TC64** | [`rtl/minimig/denise_sprites.v:273`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_sprites.v#L273) | descendant — the identical line; a fix here diverges from both lines at once |

Denise's line is the manual's sentence written as code.

### What other Commodore documents say

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — agrees with the manual:

  > "07 ATT Sprite attach control bit (odd Sprites)"
  <!-- corpus: denise-spec-252126/p010.txt -->

  The ECS Denise specification scopes the attach bit to odd sprites, as the manual does.

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 28 — agrees with the manual:

  > "For each of the odd numbered sprites, the associated control register 37 contains a bit that can be set in order to attach an odd and an even numbered sprite to pair the data for both sprites in color interpretation."
  <!-- corpus: patent-us4874164-agnus-denise/p028.txt -->

  Places the attach bit in the odd sprites' control registers only, as the manual does; like the manual it gives the even sprite's bit 7 no role.

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheet 6c and the second pass of section 6 — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 6c draws the control cells of sprites 0 and 1 transistor by transistor. SPR1CTL's write strobe, cSPR1CTL, latches D7in into S1C7#; SPR0CTL's, cSPR0CTL, latches D0in (SH0) there but not D7in, which passes sprite 0's cell unlatched. The second-pass sheet of section 6 draws a row of bit-7 latches under cSPR7CTL, cSPR5CTL, cSPR3CTL and cSPR1CTL, with the cells under the even sprites' strobes empty.
  <!-- drawing: denise-8373r4-dissection/sheet_027.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_039.txt -->

  The 8373R4 does not store bit 7 of an even sprite's control word, so nothing downstream can attach a pair from it: the manual's rule, not NanoMig's even-sprite term.

## Decision

Is the even-sprite term to go? If so, drop it:

```verilog
if (attach1) // sprites are attached -> 15 colors + transparant
```

Denise's `(nr & 1) && (value & 0x80)` is the same rule stated at the register
write instead of at the compositor, which may be the tidier place for it.

Before removing the term, find out whether it was written for the original Denise
(*Risks and unknowns*). It closes as stated in *What is asked of a reader*.
