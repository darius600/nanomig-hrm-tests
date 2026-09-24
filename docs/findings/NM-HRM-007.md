# NM-HRM-007 — the hires fetch width cannot reach the manual's maximum of 49 words

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_bitplanedma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_bitplanedma.v#L339) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L339) |
| **Report to** | **nowhere yet** — NanoMig and both emulators read fetch 50, the manual's formula window uncut; the manual says the last word at $D8 is not fetched. See *For the specialists* |
| **Test** | [`hrm/ch3-ddf-fetch-window`](../../hrm/ch3-ddf-fetch-window), [`hrm/ch3-bitplane-memory-layout`](../../hrm/ch3-bitplane-memory-layout) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | WinUAE and vAmiga fetch 50, as NanoMig does; Copperline's own test requires an even count; the manual says 49 |

---

## What the HRM says

**Printed p. 80**:

> "Horizontally, the situation is similar. Strictly speaking, the hardware sets
> a rightmost limit to DDFSTOP of ($D8) and a leftmost limit to DDFSTRT of
> ($18). This gives a maximum of 25 words fetched in low resolution mode. In
> high resolution mode the maximum here is 49 words, because the rightmost limit
> remains ($D8) and only one word is fetched at this limit."

and the accompanying table gives `max words fetched   25   49`. The page gives
its own reason: at the $D8 limit the last hires fetch is **one word**.

**Printed p. 60** gives the general relation:

> DDFSTRT = DDFSTOP - (4 * (word count - 2)) for high resolution

At $18/$D8 that is 192/4 + 2 = **50** words -- the full window. The 49 is that
window with its last word cut at the limit, which is what p. 80 says.

---

## What NanoMig does

Both DDF comparisons force the low bit of the compared value to zero — the
trailing `1'b0`:

[`src/minimig-aga/agnus_bitplanedma.v#L339`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_bitplanedma.v#L339)

```verilog
      if (hpos[8:1]=={ddfstrt[8:3], ddfstrt[2] & ecs, 1'b0})
```

Start and stop therefore both land on **even** `hpos[8:1]` positions, so the
hires word count between them is structurally even, and nothing cuts the last
fetch at the $D8 limit. 25 is reachable in lores; 49 is not reachable in hires
by any register pair.

### Measured

| | words |
|---|---:|
| manual, hires maximum | 49 |
| p. 60's formula at $18/$D8, nothing cut | 50 |
| NanoMig, measured at $18/$D8 | **50** |
| WinUAE and vAmiga at the same limits | **50** |

Nobody produces 49. NanoMig and the two emulators fetch the formula's full
window; none cuts the last word at $D8.

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

  Same figure and same reason as the 3rd edition, which prints the same sentence with 'mode' and a comma added. The 49 comes from the $D8 stop limit cutting the last hires fetch to one word. Two pages earlier (p056), the hires relation 'DDFSTART = DDFSTOP - (4*(word count - 2)' gives 50 at $18/$D8, the emulators' figure. The page's own formula and its stated maximum therefore differ by one word. The 3rd edition prints the same formula.

Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS); Commodore-Amiga, *Amiga Hardware Manual* errata, October 1985; US patent 4,874,164, Commodore-Amiga.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

---

## Suggested change

**None until a specialist confirms the cut.** What p. 80 describes is a last
hires fetch at $D8 that delivers one word instead of two. No implementation read
here models it: NanoMig, WinUAE and vAmiga fetch 50, and Copperline asserts an
even count. A change that followed the page would stop the hires fetch one word
early when DDFSTOP is at the $D8 limit -- a behaviour nobody else has, on the
strength of one sentence.

---

## For the specialists

Whether real Agnus cuts the last hires fetch at the $D8 limit to one word, as
p. 80 says, decides which side is wrong. The implementations agree with each
other and not with the page: all three that fetch at the limits fetch 50, the
p. 60 formula's window with nothing cut.

The third-witness reading offers a mechanism
([B4](../third-witness-ch3-ch6.md#b4--hires-fetches-50-words-at-both-hardware-limits-not-49-2-assertions)):
$D8 − $18 = 192 is an exact multiple of the 8-CCK fetch block, so the stop lands
on a block boundary and one whole further block completes; "only one word is
fetched at this limit" would describe a stop that lands mid-block, which this
register pair never produces.
