# NM-HRM-023 — the ATTACH bit is honoured in the even sprite, not only the odd

| | |
|---|---|
| **Status** | open · red |
| **Module** | `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise_sprites.v#L484) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_sprites.v) |
| **Upstream** | **MiSTer** `rtl/denise_sprites.v:283` — byte-identical: `if (attach1 \|\| (!aga && attach0)) // sprites are attached -> 15 colors + transparant`, the same even-sprite term and the same `!aga` gate. **TC64** `rtl/minimig/denise_sprites.v:273` — the identical line. A fix here diverges from both lines at once, and the `!aga` gate this finding's caution asks about is theirs too, not something NanoMig added |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's, and a fix there reaches NanoMig |
| **Test** | [`hrm/ch4-attached-sprites`](../../hrm/ch4-attached-sprites) |
| **Chapter** | 4 — Sprites · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **three of four take the bit from the odd sprite only**, as the manual states |

---

## What the HRM says

It says it **twice, on two pages**, and neither sentence admits an exception.

**Printed p. 128:**

> "Bit 7 is the attach bit. This bit is valid only for odd-numbered" sprites

**Printed p. 122:**

> "Attachment is in effect only when the ATTACH bit, bit 7 in sprite control word
> 2, is set to 1 in the data structure for the odd-numbered" sprite

The chapter's own design depends on it: the odd sprite's control word is what
carries the attachment, which is why its data words supply the high two bits of
the combined colour.

---

## What NanoMig does

[`src/minimig-aga/denise_sprites.v#L484`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise_sprites.v#L484)

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
| **Copperline** | — | reads the bit without the odd-sprite condition *(source reading, not run)* |

**Three of four take the attach bit from the odd sprite only.** Denise's line is
the manual's sentence written as code.

---

## Suggested change

Drop the even-sprite term:

```verilog
if (attach1) // sprites are attached -> 15 colors + transparant
```

Denise's `(nr & 1) && (value & 0x80)` is the same rule stated at the register
write instead of at the compositor, which may be the tidier place for it.

**One caution.** The `!aga` gate looks deliberate rather than accidental — someone
wrote a chipset condition around the even-sprite term on purpose. Before removing
it, find out what it was for: the likeliest answer is a title that sets the wrong
control word and relies on OCS/ECS Denise accepting it, which would make this
another `NM-HRM-008`-shaped case where the RTL knows something the manual does
not.

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

### Found three times before it was built

Two classifiers working different lots of chapter 4 measured it independently,
neither having seen the other's work; the lead then verified it against the
source and both printed pages; and the builder reproduced it from the quotations
alone, having written and committed its specification before opening the RTL.

That is the strongest provenance any finding in this register has.

---

## For the specialists

The `!aga` term is the whole question. If it encodes real OCS/ECS Denise
behaviour, the manual's two sentences are incomplete and this becomes a source
observation. If it is a workaround for one title, it belongs behind a narrower
condition than a chipset strap.
