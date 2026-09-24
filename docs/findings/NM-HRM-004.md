# NM-HRM-004 — ECS danger bit admits too small a range

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_copper.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_copper.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if the manual's reading is accepted — see *Suggested change* |
| **Test** | [`hrm/ch2-copper-move-range`](../../hrm/ch2-copper-move-range), [`hrm/ch2-copper-move-sweep`](../../hrm/ch2-copper-move-sweep) |
| **Chapter** | 2 — Coprocessor · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | **all four read the appendix the other way round** |

---

## What the HRM says

**Appendix C, printed p. 310:** with ECS and `CDANG` **clear**, the Copper reaches
`$DFF03E`–`$DFF07E`:

> "the same range as when the danger bit is set in the standard chip set"

---

## What NanoMig does

It rejects that range. With `CDANG` clear the accepted set is `$080`–`$1FE` at
`ecs = 0` and `ecs = 1` alike.

### Measured

| | |
|---|---|
| falsely rejected | 32 addresses, `$040`–`$07E` |
| falsely accepted | none |

Both a single-address probe and an exhaustive sweep agree.

*Addresses here are the manual's byte addresses. A Copper instruction encodes
DA8-1, so the module's register ports carry them halved and the sweep prints them
that way. An earlier version of this entry quoted the halved form without saying
so, which made it impossible to reconcile with the test it cited.*

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | — | blocks below `$80` |
| **vAmiga** | — | blocks below `$80` |
| **Denise** | [`emulation/libami/agnus/copper.cpp:342`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L342) | <pre>cdang = (value & 2) ? (agnus.ecsAndHigher() ? 0 : 0x40) : 0x80;</pre> floor `$80` when the danger bit is clear |
| **Copperline** | [`src/bus/dma_slots.rs:697-701`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L697-L701) | <pre>pub(super) fn copper_can_write_custom(&self, off: u16) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;let off = off & 0x01FE;<br>&nbsp;&nbsp;&nbsp;&nbsp;if off &lt;= 0x03E {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return !matches!(self.agnus.revision(), AgnusRevision::Ocs)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&& self.agnus.copper_danger_enabled();</pre> needs the bit **set** for `$040`–`$07E` |
| **MiSTer** | [`rtl/agnus_copper.v:225`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L225) | descendant — identical to NanoMig: `if (ir2[8:7]==2'b00 && !cdang \|\| ir2[8:6]==3'b000 && !ecs) // illegal access`, the same CDANG-clear range |
| **TC64** | [`rtl/minimig/agnus_copper.v:225`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L225) | descendant — the same line, byte for byte. Neither fork's only edit to this file (the blocking-assignment change at lines 234-274) reaches the CDANG decode. If the manual's reading is upheld, a fix diverges from neither line |

**All four independent implementations read appendix C p. 310 the other way
round** — that `CDANG` must be set, not clear, to reach the low range on ECS.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS); US patent 4,874,164, Commodore-Amiga.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

---

## Suggested change

**None, probably.** Four independent implementations read the sentence the
opposite way from the way this test reads it. When the design under test and all
four references agree against a single printed sentence, the sentence is the
thing to re-read.

If the manual's reading is upheld, the change is to make `CDANG` clear on ECS
admit `$040`–`$07E` — but that inverts the behaviour every reference
implementation has, and would be a change worth being very sure about.

The useful next step is a careful reading of appendix C p. 310 by someone who
knows what the ECS Copper actually does, not a patch.

---

## What holds the finding up

**Corroborated on both sides, though not three ways.**

- **The manual side carries in two editions.** The Developer-CD text and the OCR
  of the printed book give the sentence word for word.
- **The measured side** has the single-address probe and the exhaustive sweep
  agreeing.

An earlier version counted appendix B's marker column as a third corroboration.
**It is not one.** That column describes the standard chip set, where the RTL
already agrees with it, so it cannot disagree with the design here — and a record
that cannot disagree with the design is not evidence.

What it does contribute is the **referent** of appendix C's comparison: it fixes
which addresses "the danger bit set in the standard chip set" denotes, and it
does so exactly. All 32 word addresses in `$040`–`$07E` appear in the table and
all 32 carry the CDANG marker.
