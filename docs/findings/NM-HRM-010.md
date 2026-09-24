# NM-HRM-010 — the horizontal line is a whole number of colour clocks, not 227.5

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a source observation, not test-backed |
| **What is asked of a reader** | Nothing yet. This was read from the source rather than measured by a graded assertion, and needs a test before it can ask for anything. |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L86) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L86) |
| **Report to** | **nowhere** — a known, deliberate simplification, stated in the RTL |
| **Test** | none. The claim is a **recorded decline**, not an assertion |
| **Chapter** | 3 — Playfield · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | both that were read model the half clock as alternating whole lines |

---

## Why this is a source observation and not a test result

The manual's 227.5 colour clocks and the RTL's own admission that half-clocks are
unsupported are both real. **The test was not evidence for it**, and was
withdrawn.

The assertion computed `want_227p5_in_ticks = 455` — that is 227.5 CCK × 2
hpos-ticks per CCK — and **the factor 2 is the RTL's internal representation, not
the manual's**. The expected value therefore came from the implementation: the
same circularity as the `kHiresPixelTicks = 2` defect caught during the build,
found a second time.

No way was found to state the disagreement without an RTL-supplied conversion.
The manual never names `htotal_out`, and the factor 2 lives in
`agnus_beamcounter.v:206`'s write-doubling. So the claim is a **recorded decline**
— the honest outcome.

Its neighbour was rebuilt as a pure relation instead: `HTOTAL-H8-H1-BIT-MAPPING`
asserts each bit's reading is exactly double the one below, which is what p. 304's
`h8..h1` Use-row states, and it **passes** with no unit conversion anywhere in it.

---

## What the HRM says

The horizontal line is **227.5 colour clocks** in NTSC.

---

## What NanoMig does

[`src/minimig-aga/agnus_beamcounter.v#L86`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L86)

```verilog
parameter HTOTAL_VAL = 8'd227 - 8'd1;   // line length of 227 CCKs in PAL mode
                                        // (NTSC line length of 227.5 CCKs is not supported)
```

and line 105 adds *"long lines are not supported yet"*.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | — | |
| **vAmiga** | — | |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:1030`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L1030) | <pre>fps = (double)frequency() / (227.5 * 263.0); // start with lol toggling</pre> NTSC `227.5 × 263`, PAL `227.0 × 313`, with long/short lines toggling |
| **Copperline** | — | an integer `COLORCLOCKS_PER_LINE` with a separate `NTSC_LONG_COLORCLOCKS_PER_LINE` |
| **MiSTer** | [`rtl/agnus_beamcounter.v:86`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L86) | descendant — identical to NanoMig: `parameter HTOTAL_VAL = 8'd227 - 8'd1;` with the same "NTSC line length of 227.5 CCKs is not supported" comment |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:102`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L102) | descendant — the same simplification and the same comment, the constant only wider (`9'd227 - 9'd1` versus `8'd227 - 8'd1`) inside a file whose port list and internals are otherwise substantially rewritten (643 of 461 lines differ). Both forks decline the half clock exactly as NanoMig does; there is no line to diverge from |

**Both Denise and Copperline model the half clock as alternating whole lines**,
not as a fractional one — which is what this observation says the manual's 227.5
must mean in practice. Denise's comment even names the mechanism: *lol toggling*,
long-line toggling. MiSTer and TC64 decline the half clock exactly as NanoMig
does — as descendants that is not a third reading, only the same simplification
inherited unchanged.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **US patent 4,874,164, Commodore-Amiga**, PDF page 25 — agrees with the manual:

  > "During scan of a horizon tal line, there are typically 227.5 memory access cycles, each of which is approximately 280 nanoseconds in duration."
  <!-- corpus: patent-us4874164-agnus-denise/p025.txt -->

  Same figure as the manual, per horizontal line. It says 'typically' and names no video standard. The same sentence is on patent-us5103499-copper p025.

- **Commodore, *Denise* specification, part 252126-01 (ECS)**, PDF page 11 — agrees with the manual:

  > "strobe shown above is used during the second refresh time slot of every other line,  to identify lines with long counts (228)."
  <!-- corpus: denise-spec-252126/p011.txt -->

  The ECS Denise specification says every other line is a long line of 228 counts. This is the alternation NanoMig does not generate.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

---

## Suggested change

**None.** This is a known, deliberate simplification that the RTL documents in
two comments. The half-clock is what produces the long-line/short-line
alternation the interlace machinery depends on, so implementing it is a feature
with consequences, not a bug fix.

The open question is narrower and worth stating: **does anything in chapter 3's
material depend on the half clock observably**, or is the simplification
invisible above this boundary? Nothing found so far says it is visible.
