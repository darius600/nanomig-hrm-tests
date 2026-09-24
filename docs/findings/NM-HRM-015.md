# NM-HRM-015 — BRDNTRAN is declared and never connected

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `denise.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/denise.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch3-horizontal-display-window`](../../hrm/ch3-horizontal-display-window) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | one of four treats `BRDNTRAN` as real border behaviour |

---

## What the HRM says

**Appendix C, printed pp. 296, 297, 307** gives `BPLCON3` bit 4 as **BRDNTRAN**,
"border opaque" — one of the four new ECS genlock features.

---

## What NanoMig does

Nothing. The signal is **commented out twice**, at its declaration and at its
assignment, while its neighbour is live:

```verilog
wire          brdrblnk;   // border blank enable; disabled when ECSENA is low
//wire        brdntran;   // border area is non-transparent; disabled when ECSENA is low

assign brdrblnk = bplcon3[5] & ecsena;
//assign brdntran = bplcon3[4] & ecsena;
```

Writing bit 4 changes nothing observable at this boundary.

**This reads as unimplemented rather than wrong.** The commented-out lines look
like work left for later, not a decision to omit the feature.

---

## What the others say

| | |
|---|---|
| **WinUAE** | no border effect |
| **vAmiga** | no border effect |
| **Denise** | the name is not present anywhere in its Amiga core |
| **Copperline** | **implements it**, with its own test |
| **MiSTer** | descendant — `rtl/denise.v:197-198,216-217` — same as NanoMig: `brdntran` is declared and assigned but both lines stay commented out; 91 lines differ elsewhere in the file, `BRDNTRAN` in none of them. No border effect |
| **TC64** | descendant — `rtl/minimig/denise.v:205,224` — diverges here: both lines are live — `wire brdntran;` and `assign brdntran = bplcon3[4] & ecsena;` — computing the signal NanoMig and MiSTer leave commented out. TC64 goes no further, though: `brdntran` appears nowhere else in the file, so the computed value still reaches no consumer. A fix meets MiSTer where NanoMig already is; against TC64 it inherits a live signal but still has to write the border-composition wiring that no line has. The observable behaviour is unchanged from NanoMig's; only the shape underneath it differs |

One of the four independent implementations treats `BRDNTRAN` as real border
behaviour. Three of the four do not — which is worth weighing before calling
this urgent. MiSTer matches NanoMig exactly. TC64 is neither: it computes the
signal, unlike NanoMig and MiSTer, but wires it nowhere, so at the pins it still
behaves like the three that do nothing at all — live is not the same as
connected.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS)**, PDF page 5 — agrees with the manual:

  > "BRDNTRAN= "border area" is non-transparent(ZD pin is low when border is displayed."
  <!-- corpus: denise-spec-252126/p005.txt -->

  Defines BPLCON3 bit 4 as a live control next to BRDRBLNK at bit 5, and the same page says 'ENBPLCN3= When set enables all the new features in BPLCON3;'. Sides with the manual that the bit does something. The effect it names is on the ZD (background indicator) pin, i.e. genlock keying, not on the RGB colour of the border.

Silent on this point: US patent 4,777,621, Commodore-Amiga; US patent 4,874,164, Commodore-Amiga; US patent 5,103,499, Commodore-Amiga.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

---

## Suggested change

Uncomment the two lines and wire the signal where the border is composed. The
neighbour two lines up is the model: `brdrblnk` is gated identically and reaches
`t_blank`.

```verilog
wire          brdntran;   // border area is non-transparent; disabled when ECSENA is low
assign        brdntran = bplcon3[4] & ecsena;
```

The work is then in the border composition, not the decode.

**Worth confirming first** that nothing downstream silently depends on the border
being transparent — a signal that has never been connected has never had its
consumers written.

---

## What holds the finding up

**The neighbour is the control.** `BRDRBLNK`, bit 5 of the same register, gated
by the same `ecsena`, **is** wired — and the package asserts it as
`G11-BRDRBLNK-WORKS`, **green**, on the same fixture and the same measurement
basis.

So the fixture demonstrably can observe a `BPLCON3` border control. Bit 4 simply
is not one.
