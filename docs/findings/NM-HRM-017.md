# NM-HRM-017 — SuperHires bitplane colour encoding is not implemented

| | |
|---|---|
| **Status** | open · red |
| **Module** | `denise_colortable.v`, `denise.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v) |
| **Upstream** | `denise_colortable.v`: same as `NM-HRM-016` — **MiSTer** (`rtl/denise_colortable.v:32-35`) and **TC64** (`rtl/minimig/denise_colortable.v:32-35`) both keep the interface mode-blind, and their 11-line diffs from NanoMig (a RAM primitive's word width) are identical to each other and unrelated to colour mode. `denise.v`: **MiSTer** `rtl/denise.v:339` — `assign bpldata[3] = window_ena && (l_bpu > 2) ? bpldata_out[3] : 1'b0;`, the same `l_bpu > 2` depth force as NanoMig's `:369`, with an added `window_ena &&` term NanoMig's line lacks. **TC64** `rtl/minimig/denise.v:354` — the identical line, `window_ena` term included. The depth truncation this finding rests on is unchanged in both lines; the `window_ena` difference is a separate, narrower divergence this finding does not measure |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's, and a fix there reaches NanoMig |
| **Test** | [`hrm/ch3-shres-bitplane-colour-encoding`](../../hrm/ch3-shres-bitplane-colour-encoding) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **two against two**, the same split as `NM-HRM-016` |
| **See also** | `NM-HRM-016`, the sprite half of the same mechanism |

---

## What the HRM says

**Printed pp. 297 and 301** give the SuperHires bitplane colour scheme: the same
pair encoding as the sprite case, reading each colour register as two slots so
that colour can be carried at the doubled pixel rate.

---

## What NanoMig does

Same root cause as `NM-HRM-016`, reached down a different path:
`denise_colortable.v` has **no `shres` port**, so no colour lookup anywhere in
the design can depend on the mode.

### Measured

With `COLOR01 = 0x0C39` — every nibble's top two bits differing from its bottom
two, so a rearrangement would be visible in all three channels:

| | |
|---|---|
| `ocs` | `0xCC3399` |
| `shres` | `0xCC3399` |

**Identical.** Reproduced from a clean build.

---

## Why the measurement looks the way it does — a correction worth reading

An independent validator objected that the original assertion **could not have
gone green even against a perfect implementation**, and was right.

It wrote `COLOR01 = 0x0FFF` — all ones in every nibble. The claimed SuperHires
encoding is a **rearrangement** of bits, and rearranging all ones yields all
ones. So the red was not evidence of anything: it is the mirror image of a
circular oracle, an assertion that cannot pass for a reason unrelated to the
design.

The stimulus was changed to `0x0C39` and the finding **restored as test-backed**
— now on evidence that could have gone the other way.

**Two further corrections came out of the same pass:**

- The sweep had been running at `bpu = 4`, a depth p. 300 does not define for
  SuperHires: *"The maximum plane depth for a SuperHires viewport is 2
  bitplanes"*, and the register row reads `13  BPU1  } Depth of SuperHires mode
  (1 or 2)`. At a compliant depth `denise.v:369` forces `bpldata[3]` to zero, so
  registers 4–15 are not merely un-reinterpreted but **unaddressable**. They are
  now declined on that ground rather than asserted.
- `OCS-BASELINE-0RGB-FORMAT` had baked `denise_colortable.v`'s nibble-to-byte
  duplication into its expected value — an implementation detail as an oracle. It
  now checks only the top nibble of each component, which follows from "COLORxx
  stores 0x0RGB" alone. Still green.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | — | reinterprets, ECS Denise only |
| **vAmiga** | — | reinterprets, ECS Denise only |
| **Denise** | [`emulation/libami/video/denise.cpp:200`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L200) | <pre>colors[pos] = value;</pre> one table, no mode |
| **Copperline** | [`src/video/bitplane/tests.rs:7563`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L7563) | *"each 35 ns half resolves palette[1], never a pair-encoded entry"* |

---

## Suggested change

As with `NM-HRM-016`: `denise_colortable` needs a mode input it does not have.
The two findings are one mechanism and should be closed together or not at all —
they are kept as separate entries because they rest on separate measurements at
separate boundaries, not because they are separate problems.

---

## Why this entry is worth more than it was

The validator's objection was correct and the correction answers it. The finding
now rests on a stimulus that would have exposed a working implementation, at a
plane depth the manual defines, with an expected value that follows from the
manual rather than from the colour table's internals.

Three circularities were removed from one package in one pass. That is the reason
to keep the objection visible here rather than quietly fixing the test.
