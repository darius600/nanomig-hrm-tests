# NM-HRM-040 — CIAADDRA's direction mask corresponds one-to-one for two bit positions of eight

| | |
|---|---|
| **Status** | open · red · **contested** |
| **Module** | `ciaa.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/ciaa.v#L252-L262) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L298-L309) |
| **Upstream** | **MiSTer** `rtl/ciaa.v:300` — the same split, plus one added trailing comment: `pa_out[7:0] = {porta_in2[7:2],porta_out[1:0]};  // Mix inputs and outputs`. **TC64** `rtl/minimig/ciaa.v:230,238` — byte-identical to NanoMig, no comment added either: the same `pa_out` assignment and the same `porta_out[3:0]` pull-up assign. A fix diverges from both lines equally; neither has the eight-bit mask this page requires |
| **Report to** | **read the third-witness section first.** Two of four implementations gate this behaviour on a CIA part number |
| **Test** | [`hrm/ch8-ciaa-port-direction`](../../hrm/ch8-ciaa-port-direction) |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 2 red, each sweeping all eight positions. Two PA0-restricted controls pass on the identical comparator in the same run |
| **Verdict of the four** | **a three-way split.** Denise honours all eight; Copperline reproduces NanoMig's two-of-eight bit for bit; WinUAE and vAmiga each restore the full eight only under a chip-variant switch that is off by default |

---

## What the HRM says

**p.241**: *"CIAADDRA ($BFE201) contains a mask that corresponds one-to-one with
the data read register, CIAAPRA ($BFE001). Setting a 1 in the direction position
makes the corresponding bit an output."*

One-to-one, across the register. The page names no exception.

## What NanoMig does

```verilog
pa_out[7:0] = {porta_in2[7:2], porta_out[1:0]};                      // :254
assign porta_out[3:0] = {(~ddrporta[7:6] | regporta[3:2]),
                         (~ddrporta[1:0] | regporta[1:0])};          // :262
```

Two different reasons, which is why this is one finding rather than two:

- **PA2–PA5** — a CIAAPRA read takes bits 7:2 straight from the synchronised
  input. No direction bit can make them outputs at all.
- **PA6/PA7** — their written value *is* captured and *does* drive
  `porta_out[3:2]`, which leaves the module toward the fire-button wiring. But
  the same read mux never reads it back, so a CPU read of CIAAPRA never reflects
  a value it set.

### Measured

    PA0=PASS  PA1=PASS  PA2=FAIL  PA3=FAIL  PA4=FAIL  PA5=FAIL  PA6=FAIL  PA7=FAIL

The two PA0-restricted controls call the identical comparator functions and pass
in the same run, which is what stops this being a defect in the measurement.

## What the others say

| | | |
|---|---|---|
| **Denise** | [`cia/new/register.cpp:194`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L194) | `uint8_t ioa = lines.pra \| ~lines.ddra;` — all eight, and every external contribution passes through `~ddra`. The only one that implements the page as printed |
| **Copperline** | [`bus.rs:9010`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L9010) | its generic core is full width, and `cia_a_read` then discards it: `v = (v & !0x3C) \| self.floppy.cia_a_status_bits();`, no DDR term. **NanoMig's split, bit for bit** |
| **WinUAE** | [`cia.cpp:1917`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1917) | only this line implements the rule, and only for PA1/PA0. It will not even store a written value elsewhere: `c->pra = (c->pra & ~0xc3) \| (val & 0xc3);` |
| **vAmiga** | [`CIA.cpp:863-867`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L863-L867) | muxes all eight, then lets an attached control-port device override PA7/PA6 *after* the mux without consulting `ddra` |

**Copperline's split is demonstrably deliberate, not an oversight.** Eleven lines
after the one above, in the same function, its **port B** overlay masks with
`!port_b_ddr()` under the comment *"A pin the guest has switched to an output
stays CIA-driven"*, and CIA-B applies `!ddr` to all three of its overlays.
Somebody decided port A's status bits are different.

**Two of the four gate the printed behaviour on a chip part number.** WinUAE's
`391078-01` type ([`cia.cpp:1923-1927`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1923-L1927),
enabled for the A600 alone, default off) and vAmiga's `MOS_8520_PLCC`
([`CIA.cpp:869-871`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L869-L871),
default off) each restore strict one-to-one on all eight. WinUAE's own comment
on that branch reads *"391078-01 CIA: output mode bits always return PRA
contents"*.

**That is why this finding is marked contested.** The question behind p.241 may
not be "does the implementation obey the page" but "which CIA part is fitted",
and two independent emulator authors evidently concluded the answer differs by
part. That is a decision for Amiga specialists.

One corroboration worth passing on: vAmiga logs *"DDRA: Setting unusual value"*
for CIA-A whenever the write is not `0x03`
([`CIARegs.cpp:230-231`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L230-L231))
— its own code treats PA1/PA0-as-outputs as the only expected mask, which is
exactly the pair NanoMig implements.

## Suggested change

**Not without an answer to the part-number question.** If the fitted CIA returns
PRA contents for bits in output mode, `pa_out` should mux on `ddrporta` across
all eight positions, the way Denise does in one line. If it does not, the page is
describing a part this core does not model, and the finding is documentation
rather than a defect.

What should not happen is a change made because a test is red. The test is red
because the page is unambiguous; whether the page is right about the silicon is
a different question with a different owner.

## What holds the finding up

- The page states one-to-one and names no exception.
- Two controls on the identical comparator pass in the same run.
- Six of eight positions fail, for two distinct and independently readable RTL
  reasons.

## What weakens it

- Only one implementation of four does what the page says by default.
- Two of the four treat it as part-dependent, which is a reading the page does
  not offer and the manual may simply predate.
