# NM-HRM-015 — BRDNTRAN is declared and never connected [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `denise.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch3-horizontal-display-window`](../../hrm/ch3-horizontal-display-window) |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | two of four implement `BRDNTRAN`, both as genlock keying; none changes the border's colour |

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
//wire          brdntran;   // border area is non-transparent; disabled when ECSENA is low

assign brdrblnk = bplcon3[5] & ecsena;
//assign brdntran = bplcon3[4] & ecsena;
```

Writing bit 4 changes nothing observable at this boundary.

**This reads as unimplemented rather than wrong.** The commented-out lines look
like work left for later, not a decision to omit the feature.

### Measured

[`hrm/ch3-horizontal-display-window`](../../hrm/ch3-horizontal-display-window)
sets BPLCON3 bit 4 alone (ECSENA set, BRDRBLNK clear) and compares the
border's rendered colour against leaving it clear: identical —
`D-BRDNTRAN-HAS-NO-EFFECT`, red, one of the package's `13 passed, 1 failed`.
The same fixture's `G11-BRDRBLNK-WORKS`, green, shows bit 5 of the same
register does change the rendering — the control described in *What holds
the finding up* below.

---

## What the others say

| | |
|---|---|
| **WinUAE** | **implements it**, as genlock keying only: with ECSENA set on ECS Denise, `bordertrans = (bplcon3_denise & 0x10) && ecsena;` ([`drawing.cpp:3117`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L3117)) keeps the border from being genlock-transparent ([`drawing.cpp:1246-1255`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L1246-L1255)); the border's colour does not change |
| **vAmiga** | no border effect |
| **Denise** | the name is not present anywhere in its Amiga core |
| **Copperline** | **implements it**, with its own test, as genlock keying only: while it is set the border is never genlock-transparent ([`src/video/bitplane.rs:997`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane.rs#L997)); the test ([`src/video/bitplane/tests.rs:1389-1415`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L1389-L1415)) expects the border's colour unchanged and only its alpha opaque |
| **MiSTer** | descendant — `rtl/denise.v:197-198,216-217` — same as NanoMig: `brdntran` is declared and assigned but both lines stay commented out; 91 lines differ elsewhere in the file, `BRDNTRAN` in none of them. No border effect |
| **TC64** | descendant — `rtl/minimig/denise.v:205,224` — diverges here: both lines are live — `wire brdntran;` and `assign brdntran = bplcon3[4] & ecsena;` — computing the signal NanoMig and MiSTer leave commented out. TC64 goes no further, though: `brdntran` appears nowhere else in the file, so the computed value still reaches no consumer. A fix meets MiSTer where NanoMig already is; against TC64 it inherits a live signal but still has to write the border-composition wiring that no line has. The observable behaviour is unchanged from NanoMig's; only the shape underneath it differs |

Two of the four independent implementations, WinUAE and Copperline, implement
`BRDNTRAN`, and both as genlock keying: the border stops being transparent to
the overlay, and its colour does not change. vAmiga and Denise do not implement
it. MiSTer matches NanoMig exactly. TC64 is neither: it computes the
signal, unlike NanoMig and MiSTer, but wires it nowhere, so at the pins it still
behaves like the two that do nothing at all — live is not the same as
connected.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 5 — agrees with the manual:

  > "BRDNTRAN= "border area" is non-transparent(ZD pin is low when border is displayed."
  <!-- corpus: denise-spec-252126/p005.txt -->

  Defines BPLCON3 bit 4 as a live control next to BRDRBLNK at bit 5, and the same page says 'ENBPLCN3= When set enables all the new features in BPLCON3;'. Sides with the manual that the bit does something. The effect it names is on the ZD (background indicator) pin, i.e. genlock keying, not on the RGB colour of the border.

Silent on this point: US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989; US patent 5,103,499, Commodore-Amiga, filed 1989 as a division of the 1986 application, granted 1992.

---

## Suggested change

Uncomment the two lines and wire the signal where the border is composed. The
neighbour one line up is the model: `brdrblnk` is gated identically and reaches
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
