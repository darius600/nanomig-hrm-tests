# NM-HRM-023 — the ATTACH bit is honoured in the even sprite, not only the odd [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. The 8373R4 silicon stores bit 7 for the odd sprites only; *For the specialists* says what that settles for the `!aga` term and what it leaves open. |
| **Module** | `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L474) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch4-attached-sprites`](../../hrm/ch4-attached-sprites) |
| **Chapter** | 4 — Sprites · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | **three of four take the bit from the odd sprite only**, as the manual states; outside the emulators, the 8373R4 dissection stores bit 7 for the odd sprites only — see *What other Commodore documents say* |

---

## What the HRM says

It says it **twice, on two pages** of chapter 4, and neither sentence admits an
exception; Appendix A's register table, printed p. 284, marks the bit "(odd
sprites)" as well.

**Printed p. 128:**

> "Bit 7 is the attach bit. This bit is valid only for odd-numbered" sprites

**Printed p. 122:**

> "Attachment is in effect only when the ATTACH bit, bit 7 in sprite control word
> 2, is set to 1 in the data structure for the odd-numbered" sprite

The chapter's own design depends on it: the odd sprite's control word is what
carries the attachment, and its data words supply the high two bits of the
combined colour.

---

## What NanoMig does

[`src/minimig-aga/denise_sprites.v#L474`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L474)

```verilog
if (attach1 || (!aga && attach0)) // sprites are attached -> 15 colors + transparant
```

`attach0` is the **even** sprite's bit, and `!aga` is the only thing gating it.

### Measured

With `aga = 0`, setting bit 7 of **SPR0CTL** — the even sprite of the pair —
opens the combined-colour path on its own. The manual requires SPR1CTL.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2838`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2838) | <pre>dspr[num & ~1].attached = (dspr[num \| 1].ctl & 0x80) != 0;</pre> the **odd** sprite only |
| **vAmiga** | [`Core/Components/Denise/Denise.cpp:1130`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Denise.cpp#L1130) | <pre>bool attached = GET_BIT(sprctl[sprite2], 7);</pre> with `sprite2 = 2*pair + 1` — the odd one |
| **Denise** | [`emulation/libami/video/denise.cpp:300`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L300) | <pre>spr.attached = (nr & 1) && (value & 0x80);</pre> **the manual's rule exactly** |
| **Copperline** | [`src/video/bitplane/sprite.rs:1096-1110`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/sprite.rs#L1096-L1110) | <pre>even_lines<br>&nbsp;&nbsp;&nbsp;&nbsp;.iter()<br>&nbsp;&nbsp;&nbsp;&nbsp;.chain(odd_lines.iter())<br>&nbsp;&nbsp;&nbsp;&nbsp;.filter(\|line\| line.attached)</pre> reads the bit without the odd-sprite condition: a pair renders attached on any line where either sprite's own bit is set (`attached: ctl & 0x0080 != 0`, `:322`), with no chipset condition *(source reading, not run)* |
| **MiSTer** | [`rtl/denise_sprites.v:283`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v#L283) | descendant — byte-identical: `if (attach1 \|\| (!aga && attach0)) // sprites are attached -> 15 colors + transparant`, the same even-sprite term and the same `!aga` gate |
| **TC64** | [`rtl/minimig/denise_sprites.v:273`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_sprites.v#L273) | descendant — the identical line. A fix here diverges from both lines at once, and the `!aga` gate this finding's caution asks about is theirs too, not something NanoMig added |

**Three of the four independent implementations take the attach bit from the odd
sprite only.** Denise's line is the manual's sentence written as code. MiSTer and
TC64 have NanoMig's even-sprite term too, `!aga` gate included — that gate is
theirs, not something NanoMig added on its own.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — agrees with the manual:

  > "07 ATT Sprite attach control bit (odd Sprites)"
  <!-- corpus: denise-spec-252126/p010.txt -->

  The ECS Denise specification scopes the attach bit to odd sprites, as the manual does. It is a table annotation rather than an explicit statement that the even sprite's bit 7 is ignored, the same strength as the manual's own register table.

- **US patents 4,874,164 and 5,103,499, Commodore-Amiga, filed 1986, granted 1989 and 1992 — one shared specification, counted once**, PDF page 28 — agrees with the manual:

  > "For each of the odd numbered sprites, the associated control register 37 contains a bit that can be set in order to attach an odd and an even numbered sprite to pair the data for both sprites in color interpretation."
  <!-- corpus: patent-us4874164-agnus-denise/p028.txt -->

  Places the attach bit in the odd sprites' control registers only, as the manual does; like the manual it gives the even sprite's bit 7 no role.

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheet 6c and the second pass of section 6 — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 6c draws the control cells of sprites 0 and 1 transistor by transistor. SPR1CTL's write strobe, cSPR1CTL, latches D7in into S1C7#; SPR0CTL's, cSPR0CTL, latches D0in (SH0) there but not D7in, which passes sprite 0's cell unlatched. The second-pass sheet of section 6 draws a row of bit-7 latches under cSPR7CTL, cSPR5CTL, cSPR3CTL and cSPR1CTL, with the cells under the even sprites' strobes empty.
  <!-- drawing: denise-8373r4-dissection/sheet_027.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_039.txt -->

  The 8373R4 does not store bit 7 of an even sprite's control word, so nothing downstream can attach a pair from it: the manual's rule, not NanoMig's even-sprite term. To the question *For the specialists* asks of the `!aga` term, it answers that the term encodes no behaviour of this revision; the OCS Denise is not dissected. The sprite 0/1 cells are traced in the netlist; the other three pairs are read from the overview sheet. It speaks for the ECS Denise 8373R4 only, and the dissection is a work in progress.

Silent on this point: US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988.

---

## Suggested change

Drop the even-sprite term:

```verilog
if (attach1) // sprites are attached -> 15 colors + transparant
```

Denise's `(nr & 1) && (value & 0x80)` is the same rule stated at the register
write instead of at the compositor, which may be the tidier place for it.

**One caution, for the original chip set.** The `!aga` gate looks deliberate
rather than accidental — someone wrote a chipset condition around the
even-sprite term on purpose — and it covers the original and the enhanced chip
set alike: the module takes `aga` and no ECS strap
([`denise_sprites.v#L39`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_sprites.v#L39),
`input aga,`). For the enhanced chip set the silicon answers what it could be
for: the 8373R4 has no bit-7 latch for an even sprite, so there the even
sprite's bit attaches nothing and the term encodes no behaviour of that Denise.
The original Denise is not dissected. Before removing the term, find out
whether it was written for that one: the likeliest answer is a title that sets
the wrong control word and relies on the original Denise accepting it, which
would make this a case like [NM-HRM-008](NM-HRM-008.md), where NanoMig does
what the 8373R4 silicon does and the printed pages say otherwise.

---

## What holds the finding up

- **Scoped by its own mutation.** Setting `aga = 1` zeroes the `!aga` term and
  the implementation then matches the manual — so this is an OCS/ECS behaviour,
  exactly the chipset the package drives, and not an AGA one.
- **The positive case is green on the same fixture.**
  `ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT` passes: the odd sprite's bit does
  attach the pair, so the fixture demonstrably observes attachment.
- **The data path is proven live first.** A separate
  `[control] FIXTURE-LIVE-DATA-PATH` shows the sprite path delivers distinct
  known values in order before anything reads one.
- **The enhanced chip set's silicon agrees.** The 8373R4 latches bit 7 on the
  write strobes of SPR1CTL, SPR3CTL, SPR5CTL and SPR7CTL and on no even
  sprite's (*What other Commodore documents say*).

---

## For the specialists

The `!aga` term is the whole question, and the silicon answers half of it. On
the enhanced chip set it encodes no behaviour of the 8373R4, which stores
SPRxCTL bit 7 for the odd sprites only: for that Denise the manual's two
sentences hold and the term models nothing. On the original chip set nothing read here speaks to
it: the OCS Denise is not dissected, and none of the four implementations
conditions the attach bit on the chip set. If the term encodes real OCS Denise
behaviour, the manual's two sentences are incomplete for that chip, and the
term belongs behind an OCS condition rather than `!aga`, which admits the ECS
Denise too. If it is a workaround for one title, it belongs behind a narrower
condition than a chipset strap.
