# NM-HRM-011 — PAL vertical blanking is 26 lines, the manual says 29

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L89-L90) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L89-L90) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if the manual's figure is upheld — see *Suggested change* |
| **Test** | [`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking) · [`hrm/ch7-vertb-vertical-blank-timing`](../../hrm/ch7-vertb-vertical-blank-timing) |
| **Chapter** | 3 — Playfield, and 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | Denise's PAL−NTSC gap is five lines like NanoMig's; the manual's is eight |

---

## Chapter 7: the manual gives two numbers for this on one page

Chapter 7 measured the same quantity from its own page and found that **p. 219
contradicts itself**:

> "The minimum time of vertical blanking is 20 horizontal scan lines for an NTSC
> system and 25 horizontal scan lines for a PAL system." (printed p. 219)

> the range "starts at line 0 and ends at line 20" (printed p. 219)

Read as a count the first sentence gives 20; read inclusively the second gives
21. They are on the same page, about the same thing.

| | NanoMig | p.219 as a count | p.219 read inclusively | Table 3-13 |
|---|---|---|---|---|
| NTSC | 21 | 20 | 21 | 21 (stop `$15`) |
| PAL | **26** | 25 | 26 | **29** (stop `$1D`) |

`agnus_beamcounter.v` says which reading it implements, in its own comment on the
constant it uses:

```verilog
parameter VBSTOP_PAL_VAL  = 9'd25;           // vertical blanking end (PAL 26 lines, NTSC vblank 21 lines)
parameter VBSTOP_NTSC_VAL = 9'd20;           // vertical blanking end (PAL 26 lines, NTSC vblank 21 lines)
```

So NanoMig implements the inclusive reading deliberately, and for NTSC that
agrees with Table 3-13 as well. **For NTSC there is no deviation at all except
against one of the two sentences on p. 219.**

PAL is where this finding lives, and chapter 7 narrows it: NanoMig's 26 agrees
with p. 219 read inclusively and disagrees only with Table 3-13's 29. The gap
this finding reports is therefore between **two places in the manual**, with
NanoMig sitting on one of them.

`hrm/ch7-vertb-vertical-blank-timing` grades the count reading — because it is
printed and it is a requirement — and `record()`s the inclusive reading beside
it rather than choosing silently. Both of its assertions are red.

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

### Measured

[`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking)
drives `vbl` and counts consecutive blanked lines from `vpos=0`: 21 under
NTSC, matching Table 3-13 — `VBLANK-DURATION-NTSC-MATCHES-MANUAL`, green —
and 26 under PAL, three short of the table's 29 —
`VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL`, red.
[`hrm/ch7-vertb-vertical-blank-timing`](../../hrm/ch7-vertb-vertical-blank-timing)
measures the same counts against p. 219's plain reading: 21 lines where the
page's count sentence gives 20 — `NTSC-VBLANK-DURATION-20-LINES`, red — and 26
where it gives 25 — `PAL-VBLANK-DURATION-25-LINES`, red.

---

## What the others say

| | |
|---|---|
| **WinUAE** | — |
| **vAmiga** | — |
| **Denise** | PAL−NTSC gap is **five lines, like NanoMig's** |
| **Copperline** | models a programmable blank — a different quantity from Table 3-13's fixed figure, and not comparable without running it |
| **MiSTer** | descendant — `rtl/agnus_beamcounter.v:89-90` — identical to NanoMig: `VBSTOP_PAL_VAL = 9'd25`, `VBSTOP_NTSC_VAL = 9'd20` |
| **TC64** | descendant — `rtl/minimig/agnus_beamcounter.v:105-106` — the same two values (as `11'd25`, `11'd20`; the constant is wider, not the number), the same comment. If Table 3-13's 29 is upheld, a fix diverges from both lines equally |

The manual's gap is **eight** lines, 29 − 21. NanoMig's and Denise's are five.
That the gap agrees between two independent implementations, and disagrees with
the manual by the same amount, is the useful signal here.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988; Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989.

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
