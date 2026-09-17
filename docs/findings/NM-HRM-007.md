# NM-HRM-007 — the hires fetch width cannot reach the manual's maximum of 49 words

| | |
|---|---|
| **Status** | open · red |
| **Module** | `agnus_bitplanedma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_bitplanedma.v#L339) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L339) |
| **Upstream** | **MiSTer** `rtl/agnus_bitplanedma.v:339,349` — identical to NanoMig: both DDF comparisons end in the trailing `1'b0` that forces start and stop onto even `hpos[8:1]` positions. **TC64** `rtl/minimig/agnus_bitplanedma.v:313,323` — the same two expressions, byte for byte, despite 88 lines of unrelated `hde`-generation rewrite elsewhere in the file. Both forks share NanoMig's structurally-even fetch window; a fix diverges from neither |
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
| **Copperline** | [`src/bus/ddf_sequencer.rs:662`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/ddf_sequencer.rs#L662) | <pre>assert_eq!(plane0 % 2, 0);</pre> two words per plane per 8-cck hires unit |

**Copperline states the even word count as an invariant in its own test** — which
is NanoMig's structural argument, arrived at independently and written down as an
assertion rather than as a consequence.

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
