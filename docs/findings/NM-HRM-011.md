# NM-HRM-011 — PAL vertical blanking is 26 lines, the manual says 29

| | |
|---|---|
| **Status** | open · red |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L89-L90) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L89-L90) |
| **Upstream** | inherited — the parameters are unchanged upstream |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if the manual's figure is upheld — see *Suggested change* |
| **Test** | [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | Denise's PAL−NTSC gap is five lines like NanoMig's; the manual's is eight |

---

## What the HRM says

**Table 3-13:**

```
Vertical Blank Stop     $15 (21)    $1D (29)
                          NTSC         PAL
```

---

## What NanoMig does

[`src/minimig-aga/agnus_beamcounter.v#L89-L90`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L89-L90)

```verilog
parameter VBSTOP_PAL_VAL  = 9'd25;   // vertical blanking end (PAL 26 lines, NTSC vblank 21 lines)
parameter VBSTOP_NTSC_VAL = 9'd20;
```

| | manual | NanoMig |
|---|---:|---:|
| NTSC | 21 | **21** — exact |
| PAL | 29 | **26** — three lines short |

The RTL's own comment states 26 as the intent, so this is a deliberate value
rather than an arithmetic slip.

---

## What the others say

| | |
|---|---|
| **WinUAE** | — |
| **vAmiga** | — |
| **Denise** | PAL−NTSC gap is **five lines, like NanoMig's** |
| **Copperline** | models a programmable blank — a different quantity from Table 3-13's fixed figure, and not comparable without running it |

The manual's gap is **eight** lines, 29 − 21. NanoMig's and Denise's are five.
That the gap agrees between two independent implementations, and disagrees with
the manual by the same amount, is the useful signal here.

---

## Suggested change

If the manual's figure is upheld, one parameter:

```verilog
parameter VBSTOP_PAL_VAL  = 9'd28;   // Table 3-13: PAL vertical blank stop $1D (29)
```

**But check Denise first.** Two implementations agreeing on a five-line gap
against the manual's eight is the kind of coincidence that usually means the
manual is being read differently — for instance counting the stop line
inclusively or exclusively — rather than two projects making the same mistake.

---

## What holds the finding up

**The NTSC twin passes on the same measurement basis.** `VBLANK-DURATION-NTSC`
matches Table 3-13's 21 exactly, on the same fixture, the same counter and the
same counting convention. That is what makes the PAL red survive the units-error
objection: if the test were counting lines wrongly, NTSC would be wrong too.

---

## For the specialists

Either the manual's PAL figure is wrong or NanoMig loses three lines of blanking.
Both are checkable against a real machine, and nothing short of that will settle
it.
