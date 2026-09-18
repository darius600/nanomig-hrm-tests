# NM-HRM-016 — SuperHires sprite colour encoding is not implemented

| | |
|---|---|
| **Status** | open · red — 16 evidential reds, the largest single red result in chapter 3 |
| **Module** | `denise_colortable.v`, `denise_sprites.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's, and a fix there reaches NanoMig |
| **Test** | [`hrm/ch3-shres-sprite-colour-encoding`](../../hrm/ch3-shres-sprite-colour-encoding) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **two against two** — the second finding where the four split evenly |
| **See also** | `NM-HRM-017`, the bitplane half of the same mechanism |

---

## What the HRM says

**Printed pp. 301–302** give the ECS SuperHires sprite colour scheme. In
SuperHires the sixteen colour registers `$10`–`$1F` are read as **two colour
slots each**:

| slot | bits |
|---|---|
| A | the top bit of every 2-bit field — 11:10 R, 7:6 G, 3:2 B |
| B | the bottom — 9:8, 5:4, 1:0 |

so that a sprite can carry colour at the doubled pixel rate.

---

## What NanoMig does

It reads them as ordinary colour registers in every mode.

**Root cause, verified by reading the RTL:** `denise_colortable.v` has **no
`shres` or `spres` port at all**. Its whole interface is mode-blind:

```verilog
module denise_colortable (
  clk, clk7_en, reg_address_in, data_in, rdram,
  select, bplxor, bank, loct, ehb_en, rgb );   // no shres, no spres
```

`spres` reaches the sprite path in exactly one place —
[`denise_sprites.v:158-166`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise_sprites.v#L158-L166) — where it sets `shift`, the
shift-register drain rate. That changes how **wide** a SuperHires sprite is,
never what colour it is.

### Measured

The drain-rate effect is a real confound: at `shres = 1` a sprite is half as
wide, so a raw sample trace differs between modes for a reason that has nothing
to do with colour. The package therefore compares the **set of distinct colours**
a sprite pixel can show, which is width-invariant. Every live register returns
the identical set:

```
MEASURED: REG-0x11: shres=0 distinct colours {R=2 G=1 B=3 (raw 0x9966CC)};
                    shres=1 distinct colours {R=2 G=1 B=3 (raw 0x9966CC)}.
```

Deliberately asymmetric slot-A and slot-B values were used, so a rearrangement
would have been visible.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | — | reinterprets, ECS Denise only |
| **vAmiga** | — | reinterprets, ECS Denise only |
| **Denise** | [`emulation/libami/video/denise.cpp:200`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L200) | <pre>colors[pos] = value;</pre> one table, no mode |
| **Copperline** | [`src/video/bitplane/tests.rs:7563`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L7563) | *"each 35 ns half resolves palette[1], never a pair-encoded entry"* |
| **MiSTer** | [`rtl/denise_colortable.v:32-35`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v#L32-L35) | descendant — no `shres`/`spres` port, same as NanoMig; the 11-line diff there is a RAM primitive's word width growing from 12/24 bits to 32 with byte-enables, not a colour-mode change. `rtl/denise_sprites.v:110-115` has the identical `case (spres)` block, still only driving `shift` |
| **TC64** | [`rtl/minimig/denise_colortable.v:32-35`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_colortable.v#L32-L35) | descendant — the identical 11 lines; MiSTer's and TC64's diffs from NanoMig are themselves byte-identical to each other, same primitive, same edit. `denise_sprites.v` likewise: `rtl/minimig/denise_sprites.v:110-115` — the same three-line case. Neither line gives `denise_colortable` a mode input or makes `denise_sprites` pick a colour slot from `spres`; a fix is new work on both lines, in both places |

**Two against two among the four independent implementations**, and it is the
second finding after `NM-HRM-002` where the four split evenly. Denise stores one
colour per register in every mode; Copperline states the absence of pair
encoding **deliberately, in a test's own words** — which makes it a decision
rather than an omission. MiSTer and TC64 are mode-blind in the identical way
NanoMig is, which is not a third reading — a descendant with no `shres` port
inherited that absence, it did not arrive at it independently.

---

## Suggested change

The feature is absent rather than wrong, and adding it is not a one-line change:
`denise_colortable` would need a mode input it does not have, and the sprite path
would need to select slot A or slot B per 35 ns half.

**Two of four reference implementations do not do this either**, and one of them
says so in a test. So the first question is not how to implement it but whether
ECS Denise really does — and that is what WinUAE and vAmiga, which both
reinterpret for ECS Denise only, are evidence for.

---

## An honest correction to the count

`REG-0x10` is an unconditional `expect(false)` — a **declaration, not a
measurement**. The evidential count is therefore **sixteen** reds, not seventeen.

The per-register assertions also test only that *some* mode difference exists,
not the exact symbol-to-bit mapping their claim ids cite. The finding holds;
the precision of the individual assertions is less than the claim ids imply.
