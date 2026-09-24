# NM-HRM-007 — the hires fetch width cannot reach the manual's maximum of 49 words

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_bitplanedma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_bitplanedma.v#L339) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L339) |
| **Report to** | **nowhere yet** — the two emulators disagree with NanoMig by exactly one word. See *Suggested change* |
| **Test** | [`hrm/ch3-ddf-fetch-window`](../../hrm/ch3-ddf-fetch-window), [`hrm/ch3-bitplane-memory-layout`](../../hrm/ch3-bitplane-memory-layout) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | two say 50, NanoMig says 48, the manual says 49 |

---

## What the HRM says

> "DDFSTRT of ($18). This gives a maximum of 25 words fetched in low resolution
> mode. In high resolution mode the maximum here is **49 words**"

and the accompanying table gives `max words fetched   25   49`.

---

## What NanoMig does

Both DDF comparisons force the low bit of the compared value to zero — the
trailing `1'b0`:

[`src/minimig-aga/agnus_bitplanedma.v#L339`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_bitplanedma.v#L339)

```verilog
      if (hpos[8:1]=={ddfstrt[8:3], ddfstrt[2] & ecs, 1'b0})
```

Start and stop therefore both land on **even** `hpos[8:1]` positions, so the
hires word count between them is structurally even. 25 is reachable in lores; 49
is not reachable in hires by any register pair.

### Measured

| | words |
|---|---:|
| manual, hires maximum | 49 |
| NanoMig, measured maximum | **48** |
| WinUAE and vAmiga at the same limits | **50** |

Nobody produces 49. NanoMig lands one below, the two emulators one above.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | — | 50 words at the limits |
| **vAmiga** | — | 50 words at the limits |
| **Denise** | — | not read for this finding |
| **Copperline** | [`src/chipset/ddf_sequencer.rs:662`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/ddf_sequencer.rs#L662) | <pre>assert_eq!(plane0 % 2, 0);</pre> two words per plane per 8-cck hires unit |
| **MiSTer** | [`rtl/agnus_bitplanedma.v:339,349`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L339) | descendant — identical to NanoMig: both DDF comparisons end in the trailing `1'b0` that forces start and stop onto even `hpos[8:1]` positions |
| **TC64** | [`rtl/minimig/agnus_bitplanedma.v:313,323`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L313) | descendant — the same two expressions, byte for byte, despite 88 lines of unrelated `hde`-generation rewrite elsewhere in the file. Both forks share NanoMig's structurally-even fetch window; a fix diverges from neither |

**Copperline states the even word count as an invariant in its own test** — which
is NanoMig's structural argument, arrived at independently and written down as an
assertion rather than as a consequence.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore-Amiga, *Amiga Hardware Manual*, 1985 — the oracle's first edition**, PDF page 78 — agrees with the manual:

  > "In high resolution, the maximum here is 49 words because the rightmost limit remains ($D8) and only one word is fetched at this limit."
  <!-- corpus: hw-manual-1985/p078.txt -->

  Same figure and same reason as the 3rd edition, which prints this sentence unchanged. The 49 comes from the $D8 stop limit cutting the last hires fetch to one word. It does not come from counting inclusively, which is the explanation the finding proposes. Two pages earlier (p056), the hires relation 'DDFSTART = DDFSTOP - (4*(word count - 2)' gives 50 at $18/$D8, the emulators' figure. The page's own formula and its stated maximum therefore differ by one word. The 3rd edition prints the same formula.

Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS); Commodore-Amiga, *Amiga Hardware Manual* errata, October 1985; US patent 4,874,164, Commodore-Amiga.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

---

## Suggested change

**None until the counting question is settled.** Three positions exist and none
of them is 49:

- NanoMig: 48, even by construction;
- WinUAE and vAmiga: 50, also even;
- Copperline: asserts evenness as an invariant.

An odd maximum is what nobody produces. 25 and 49 are both odd because **the
manual counts the first word inclusively** — 25 = 24 + 1 and 49 = 48 + 1. That
makes NanoMig's 48 and the manual's 49 the same fetch window counted two ways,
and the emulators' 50 a different window.

So the open question is not "why can NanoMig not reach 49" but **which of the
three windows real Agnus fetches**, and how it counts. A patch before that is
guesswork.

---

## For the specialists

Whether real Agnus can produce an odd hires count, or whether the manual counts
differently from the hardware, decides which side is wrong. The inclusive-count
reading makes NanoMig and the manual agree and leaves the emulators as the
outlier, which is the opposite of how the numbers first look.
