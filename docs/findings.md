# Findings

> **Deciding what to do about these:** see [`rtl-decisions.md`](rtl-decisions.md),
> which sets each open finding beside WinUAE's and vAmiga's implementation of the
> same question, with line citations verified against the commits
> `third-witness.md` records.

Every red test in this suite points at a place where the RTL does something the
*Amiga Hardware Reference Manual* does not describe. This file says which, so
that a red result is readable without access to anything outside this
repository.

**These are not broken tests.** Adjusting an assertion to make one pass deletes
the finding.

**There is exactly one exception, and the 11.09.2026 validation found four cases
of it:** an assertion that misstates the source must be corrected, not because
the implementation fails it, but because it never said what the manual says. The
test is the thing being fixed there, not the finding. Telling the two apart is
the whole discipline — the question is never "does this pass?" but "does the
printed page require what this asserts?".

## Validation, 11.09.2026 — read this before any finding below

An independent validator (Codex Sol, which wrote none of these tests) audited the
built packages against the printed manual. **Seven of the thirteen findings did not
survive in the form they were written.** Every disposition was re-checked against
the manual page by the lead before being accepted; none was taken on the
validator's word, and each affected finding carries its own note below.

| | findings |
|---|---|
| **Hold as test-backed findings** | F01, F02, F03, F06, F10 |
| **Hold with a correction** | F11 (evidential count) |
| **Source observations, not test-backed** | F05, F09 |
| **Downgraded, then restored once the witness was fixed** | F12 |
| **Withdrawn** | F04, F07, F08, F13 |
| **Established while correcting one of them** | **F14** |

**F02's evidence was sharpened on 11.09.2026.** Page 80 does not merely give the
number 49; it gives the reason — *"because the rightmost limit remains ($D8) and
only one word is fetched at this limit"*. The package measured only the total (50,
not 49). A second assertion now measures the mechanism the sentence names, and it
is red for a sharper reason: the hires word count is structurally even, so 49 is
not individually reachable at all — 48 and 50 are, 49 is not. The finding is the
same; the evidence now matches the clause the manual actually argues from.

Every withdrawal was also carried back into its package the same day, so no test
is left asserting something the manual does not say. Three of those corrections
answered the question the withdrawal raised: with an expectation the page
actually supports, **F04 and F07 turn green** — the implementation was right and
the oracle was not — and **F05's claim became a recorded decline**, because no
faithful assertion could be written without importing an RTL constant.

**F08 is the instructive one.** It asserted that SPRxCTL bits 3 *and* 4 are live.
Bit 3 the manual itself calls `(unimplemented)`; bit 4 it specifies. Split apart,
bit 3 is declined and bit 4 stays red on a claim that holds — recorded as F14.
The original finding pointed at something real and described it incorrectly, and
only reading the printed page again separated the two. That is the whole case for
this step.

Four of the withdrawals share one cause: **the assertion, not the RTL, was wrong.**
Each read a permission or a silence in the manual as a requirement — "not used" as
"reads zero", a bare mnemonic as a mandated effect, "ECS-only" as "must be live",
in one case even where the manual prints the word *unimplemented*. A red result is
a finding only where the manual actually says otherwise.

The two source-only downgrades share a different cause, and it is the one this
pipeline exists to prevent: **an expected value that came from the implementation.**
F05 computes `227.5 CCK × 2 hpos-ticks/CCK`, taking the factor 2 from the RTL's
internal representation. F12 is its mirror image — an assertion that could not have
passed against a perfect implementation, because it tested a bit *rearrangement*
using a value made of identical bits. **F12 is the one that came back:** given a
witness whose nibbles differ top-half from bottom-half, it is still red, and the
finding now rests on a measurement that could have gone the other way.

## About the keys

The `FPGA-nnn` keys come from the private workspace where this work is planned,
and they are already embedded in the test sources and in commit messages. They
are kept rather than renumbered: a second numbering would break the trail from a
failing assertion back to the reasoning that produced it, and would have to be
maintained in two places forever.

What the workspace holds is the *status* of each item — who decided what, when,
and what happens next. What this file holds is *what the finding is*, which is
the part a reader of this repository needs. Where a key appears in a test source
without further explanation, look it up here.

## Where each finding comes from, and whether it holds

Five findings, but not of equal standing. Three came out of the pipeline and are
witnessed more than once. Two came out of the earlier hand-written run, rest on a
single citation each — and on 07.09.2026 both of those citations turned out to be
contradicted elsewhere in the same manual.

| Finding | Origin | Standing |
|---|---|---|
| FPGA-060 | pipeline | corroborated — p273 and p309 give two different widths |
| FPGA-062 | pipeline | corroborated three ways |
| FPGA-069 | pipeline | corroborated — p20 and p275, units settled by FPGA-084 |
| FPGA-051 | hand-written, Aug | **stands** — the manual contradicts itself; two emulators settle it against the RTL |
| FPGA-050 | hand-written, Aug | **withdrawn** — the manual sanctions the measured value |

Both August findings rested on a manual that says two different things, and
neither could be decided by reading the manual harder. They were settled by a
third witness — see [`third-witness.md`](third-witness.md) — and they went
opposite ways: one survived, one did not.

## FPGA-050 — withdrawn on 07.09.2026

The assertion was removed rather than left red. A red test is a claim that the
RTL is wrong, and this one could not support that claim.

The *file* was not deleted: `agnus_beamcounter` still asserts that a PAL field
terminates with EOF and that the horizontal counter spans 0..453. The second one
matters beyond this file — `agnus_copper_waitskip_range`'s W4b rests on the line
being 454 units long.

It was not replaced with a corrected assertion either. Doing that properly means
driving the long-frame bit deliberately and checking both cases, which is work
nobody has done, and an assertion guessing at it would be worse than none.

The reasoning:

### FPGA-050 — PAL frame has one line too many

| | |
|---|---|
| Test asserts | normal PAL non-interlace is 312 positions, `vpos` 0..311 |
| Measured | `max_vpos=312`, 313 end-of-line pulses |
| Test | `hrm/agnus_beamcounter` |

**The manual sanctions 313.** Table 7-5, VPOSR bit 0: *"High bit of the vertical
position (V8). Allows PAL line counts (313) to appear in PAL versions of the
Amiga."* Confirmed in both the printed edition and the Developer-CD edition of
the same manual.

Chapter 3's "the beam produces 312 lines" is about the normal *display* field,
and chapter 2 describes the alternation itself: *"in PAL, 312, then 313 lines"*.
The test turned one of those sentences into a flat rule.

Both emulators do the same — the count follows the long-frame bit, and so does
`agnus_beamcounter.v`, which sets a long frame at reset and toggles it only in
interlace. The fixture ran non-interlaced from reset, so 313 is the right answer
for what it drove. Details and permalinks in
[`third-witness.md`](third-witness.md).

## Open findings

### FPGA-051 — audio channels on the wrong stereo side

| | |
|---|---|
| Manual, chapter 5 p.137 | channels 1+2 feed left, channels 0+3 feed right |
| Manual, appendix pin list | the opposite: *"AUDO — Audio output from left (channels 0, 3) port"* |
| RTL does | `ldatasum <= msample0 + msample3; rdatasum <= msample1 + msample2` — the pin list |
| Test | `hrm/paula_audio_mixer` |

**The manual says both**, in both editions, so it cannot decide this on its own.
Two independent emulators can, and both agree with chapter 5: WinUAE hands
channel 0 to `put_sound_word_right`, and vAmiga's default panning puts channels 0
and 3 at `pan = 0.85` on an axis where 1 is fully right. The appendix pin list is
the outlier.

**The NanoMig stereo channels are swapped.** Permalinks into both emulators are
in [`third-witness.md`](third-witness.md).

Where to correct it is still open: the swap may belong in the top level or the
board wiring rather than in `paula_audio_mixer` itself, and fixing it in two
places would swap it back.

### FPGA-060 — OCS Copper address uses five high bits instead of three

| | |
|---|---|
| Manual | p273, p309: OCS has three high address bits, ECS five; the location registers are 18-bit pointers (p276) |
| Measured | five high bits in both chipsets; 20-bit pointer (15 low + 5 high); `cop1lch[20:16] <= data_in[4:0]` latches unconditionally |
| Tests | `hrm/agnus_copper_control` (T4), `hrm/agnus_copper_locreg_width` (LW1, LW3, LW4) |

LW3 is the assertion that matters: the appendix gives *two different* widths for
the two chipsets, so a module reporting the same width in both contradicts the
manual whatever that width is.

### FPGA-062 — ECS danger bit admits too small a range

| | |
|---|---|
| Manual | Appendix C p310: with ECS and CDANG=0 the writable range equals the OCS CDANG=1 range, `$20`–`$FF` |
| Measured | `$40`–`$FF`; 32 addresses `$20`–`$3F` are falsely rejected, none falsely accepted |
| Tests | `hrm/agnus_copper_move_range` (T9), `hrm/agnus_copper_move_sweep` |

Corroborated three ways: the exhaustive address sweep, the appendix text, and the
appendix B marker column, which lists 25 registers `$040`–`$07E` as CDANG-gated
and 32 registers `$000`–`$03E` as never Copper-writable.

### FPGA-069 — WAIT and SKIP take longer than the manual states

| | |
|---|---|
| Manual | p20, p275: a SKIP spans four memory cycle times, a satisfied WAIT six, a WAIT two more than a MOVE |
| Measured | SKIP 8, satisfied WAIT 8, WAIT-minus-MOVE 4 |
| Tests | `hrm/agnus_copper_cycle_times` (P3–P6, AC5, AC6), `hrm/agnus_copper_cycles` (C5, C6) |

The unit question this depended on is settled (FPGA-064): appendix A p275 defines
bus cycle against memory cycle time and gives the factor two, so the readings are
comparable and the deviation is real. AC6 is expository, not independent — given
AC4 and AC5 it cannot fail on its own.

## Chapter 6 — two of these are now red tests

These came out of the chapter-6 classification stage on 08.09.2026, while it was
looking for a fixture boundary, and were observations rather than findings
because nothing asserted them.

**Two of them became red tests on 09.09.2026**, in `hrm/agnus_blitter_ecs`
(PKG-BLT-ECS), and both are required to fail:

| Assertion | Measured |
|---|---|
| `BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS` | a BLTSIZV write with `ecs=0` and with `ecs=1` give the identical height, r1=7 and r2=7 |
| `DOFF-NO-OBSERVABLE-EFFECT` | the same D write appears with BLTCON1 bit 7 clear and set |

Each says so in its own message and names every claim it covers, so the red line
is readable without this file. The remaining three rows of the table below still
have no test: the pointer width is asserted in `hrm/agnus_blitter_adrgen` as the
behaviour the design actually implements rather than as a defect, and BLTCON0L
and BLTSIZH are gated correctly and pass.

They carry no `FPGA-nnn` key. The keys are issued in the private workspace, and
inventing one here would collide with that register. When the chapter-6 tests are
built, each of these either gets a key and an assertion or is dropped with a
reason.

Every statement below was checked by reading the RTL directly, not taken from the
classifier's report.

### Appendix C's five ECS blitter changes are implemented five different ways

Appendix C, printed page 309, lists what ECS changed in the blitter. NanoMig's
`agnus_blitter` takes an `ecs` input. It uses it for two of the five:

| Appendix C, p309 | `agnus_blitter.v` | Gated by `ecs`? |
|---|---|---|
| `BLTCON0L` $05A new — control 0, lower 8 bits | line 259 | **yes** |
| `BLTSIZH` $05E new — H size and start | lines 500, 523, 557 | **yes** |
| `BLTSIZV` $05C new — V size, 15-bit | line 548 | **no** — and the comment on that line calls it ECS |
| `BLTxPTH` $050 chg — "high 5 bits, was 3 bits" | `agnus_blitter_adrgen.v:45`, `reg [20:16] bltpth [3:0]` | **no** — and now measured at the parent, see below |
| `DOFF`, BLTCON1 bit 7 — disable D output in line mode | nowhere | **not implemented** |

`bltcon1[7]` does not appear in `agnus_blitter.v`. The bits it reads are 15-12,
4, 3, 2, 1 and 0.

#### BLTxPTH, measured at the boundary that has the `ecs` port — 10.09.2026

The row above used to read "no — that module has no `ecs` port at all", which was
true and too weak. `agnus_blitter_adrgen` having no `ecs` input explains why the
gating cannot happen *there*; it leaves open whether the parent, which does take
`ecs`, gates it before handing the write down. It does not, and that is now
measured rather than inferred.

`hrm/agnus_blitter_ecs`'s `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` writes `BLTAPTH` with
`$001F` — all five low bits — twice on the same fixture, once with `ecs` low and
once with `ecs` high, and reads `agnus_blitter`'s own `address_out`:

    ecs=0 -> 0xF8000
    ecs=1 -> 0xF8000

Identical. All five bits are taken in both chip sets, so appendix A p.270's
"high 3 bits, 5 bits if ECS" is not implemented in either direction: an OCS
machine gets ECS width, and `ecs` changes nothing about the pointer.

The path is unbroken in the source. `agnus_blitter.v:588-592` instantiates the
address generator passing `data_in`, `reg_address_in` and `address_out` and **no
`ecs`**; the name does not occur anywhere in `agnus_blitter_adrgen.v`; and
`agnus_blitter_adrgen.v:62` is
`assign bltptr_in[20:1] = enaptr ? newptr[20:1] : {data_in[4:0], data_in[15:1]}`
— five bits, unconditionally.

**The control matters here.** A red that depends on toggling `ecs` is worthless if
the fixture's `ecs` toggle does nothing, so the assertion names
`BLTCON0L-GATED-BY-ECS` — green in the same file, on the same harness — as proof
that this fixture's `ecs` does change behaviour elsewhere. And a mutation confirms
the observable is live rather than stuck: driving `$0015` instead of `$001F` moves
both readings to `0xA8000`, still identical.

This finding exists because Prompt 8's independent validation rejected the earlier
handling. The three claims behind it — `6-appa-registers.c110`,
`ch6-appa-bltxpt.c2`, `ch6-appa-bltxpt.c6` — had been routed to `PKG-BLT-ADRGEN`,
one fixture below the `ecs` port, where the builder correctly declined them as not
constructible. The validator ruled that they needed real assertions at the parent
rather than relocated citations. They did, and the assertion came out red.

#### The same deviation, in both Agnus engines

Noticed 10.09.2026, when the blitter measurement above landed next to FPGA-060 in
this document. FPGA-060 is the copper's version of exactly this finding: "OCS uses
five high address bits; the manual documents three." The blitter now shows the
same thing — and not merely something similar. The two pointers are built the same
way, from the same bits, with no chipset condition on either:

    agnus_copper.v:144    cop1lch[20:16] <= data_in[4:0];
    agnus_copper.v:197    address_out[20:1] <= {cop1lch[20:16], cop1lcl[15:1]};

    agnus_blitter_adrgen.v:62
        assign bltptr_in[20:1] = enaptr ? newptr[20:1]
                                        : {data_in[4:0], data_in[15:1]};

Five high bits from `data_in[4:0]`, fifteen low from `data_in[15:1]`, a 20-bit
word address, unconditional. Both engines take the ECS width in both chipsets, and
in both the manual documents three high bits for OCS and five for ECS.

That reframes what the two findings are. Separately they read as two omissions.
Together they look like one design decision applied consistently across Agnus:
this implementation does not model the OCS pointer narrowing anywhere. Whether
that is a deliberate simplification or an oversight is not something these tests
can answer — but it is a question about the whole chip, not about a register, and
neither the copper chapter nor the blitter chapter could have raised it alone.

Recorded here rather than acted on. The copper half is already witnessed more than
once and carries an FPGA number; the blitter half is new, has no number yet, and
none is invented here — numbering lives in the project workspace, not in this
repository.

### The classified inventory says otherwise, in 109 records

Worth recording because it is the one place a wrong statement could have made
these findings disappear. **109 of the chapter-6 records carry an `evidence`
field asserting that `ecs` gates BLTSIZV** — wording like "`ecs` ... gates the
BLTCON0L, BLTSIZH and BLTSIZV decodes `agnus_blitter.v:500,523,557`". Those three
line numbers are BLTSIZH's and BLTCON0L's, and they are correct for those two.
BLTSIZV's decode is at line 548 and has no `&& ecs`:

```verilog
    else if (reg_address_in[8:1]==BLTSIZV[8:1]) // ECS BLTSIZV register
        height[14:0] <= data_in[14:0];
```

The comment on that line calls it ECS; the code does not gate it. The
classifier's evidence text generalised from the two registers that are gated to
the third that is not, and repeated it 109 times.

The **claims** are unaffected — appendix C does mark BLTSIZV "new" for ECS, and
the classification as testable is right. It is the stated reason that
misdescribes the RTL. A builder who had trusted it would have written an
assertion expecting a difference, seen none, and had no way to tell a defect from
its own mistake. PKG-BLT-ECS's builder read line 548 instead and sided with the
RTL, which is what the build brief's "if the brief and the document disagree, the
document wins — and if the RTL and both disagree, say so" rule is for.

The pointer row is the sibling of FPGA-060, one appendix over: the same register
family, the same "three bits in OCS, five in ECS" sentence, and the same
unconditional five bits in the RTL. The third-witness work already recorded that
neither vAmiga nor WinUAE narrows a pointer by chipset at all, which is worth
knowing before anyone calls the blitter case a defect on its own.

What this is *not* is a claim that any of the five is wrong. A design that is
always ECS/AGA has a defensible reason to widen a pointer unconditionally. What
it is: the same appendix, the same five lines, handled four different ways, with
no comment anywhere saying why the two ungated ones are ungated.

### `agnus_top_dmacon` asserts nothing

Found in the same pass and recorded where it belongs, in
[`test-inventory.md`](test-inventory.md), backlog item 5. It is the only test in
the suite that cannot fail.

## What the emulators say about the three Copper findings

Read on 07.09.2026, out of interest rather than necessity — these three are not
contested, so an emulator cannot cancel them. The result is worth knowing anyway:

**On all three, WinUAE and vAmiga do what NanoMig does and not what the manual
says.**

| Finding | Manual | Both emulators, and NanoMig |
|---|---|---|
| FPGA-060 | OCS keeps three high pointer bits | neither emulator narrows the pointer by chipset at all |
| FPGA-062 | ECS with CDANG clear reaches $03E–$07E | CDANG clear blocks below $80 in both, chipset-independent |
| FPGA-069 | MOVE 2, SKIP 4, satisfied WAIT 6 | 4 / 8 / 8 in vAmiga, the same three numbers NanoMig measures |

The citations are in [`third-witness.md`](third-witness.md).

This does not close any of them, and none of the assertions was touched. It
changes the question: not "is the RTL wrong" but "is the manual wrong about the
hardware". Two independent implementations agreeing against a manual sentence is
strong evidence, but choosing what NanoMig should follow — the manual it is
tested against, or the hardware it emulates — is a decision, not a measurement.

FPGA-062 is the sharpest case. Appendix C read literally says that *clearing* the
danger bit grants the range that *setting* it grants, which would leave the bit
with nothing to do.

## How a finding is meant to end

A red test is closed by a decision about the RTL, never by relaxing the
assertion. If a deviation turns out to be intentional, the assertion stays and
the reason is written down beside it — a test whose expectation was quietly moved
to match the implementation stops being evidence about anything.


## FPGA-091-F01 — DDFSTRT/DDFSTOP bit H3 is gated by the chipset, not by resolution

**Package:** `hrm/ch3-ddf-fetch-window`, assertion
`DDFSTRT-H3-BIT-RESPECTED-ONLY-IN-HIRES-MODE`. **Red.**

The manual, printed page 91 (and again on 277):

> Bits 7-2 - pixel position H8-H3 (**bit H3 only respected in Hires Mode.**)

NanoMig, `agnus_bitplanedma.v:339` and `:349`:

```verilog
if (hpos[8:1] == {ddfstrt[8:3], ddfstrt[2] & ecs, 1'b0})
if (hpos[8:1] == {ddfstop[8:3], ddfstop[2] & ecs, 1'b0})
```

`ddfstrt[2]` is H3, and it is ANDed with **`ecs`** — the chipset strap, declared
on line 35 as *"ddfstrt/ddfstop ECS bits enable"*. Resolution never enters the
expression. So H3 is honoured on an ECS machine in **lores** as well, and ignored
on an OCS machine in **hires**, which is the opposite of both.

**For the specialists:** the RTL author clearly intended ECS gating, and it is
possible that real ECS Agnus behaves this way and the manual's sentence is an
OCS-era statement never revised. That is the question — not whether the test is
right. The test asserts what page 91 says.

## FPGA-091-F02 — the hires fetch width cannot reach the manual's maximum of 49 words

**Package:** `hrm/ch3-ddf-fetch-window`, assertion
`MAX-WORDS-49-HIRES-AT-HW-LIMITS`. **Red.**

The manual, printed page 80:

> DDFSTRT of ($18). This gives a maximum of 25 words fetched in low resolution
> mode. In high resolution mode the maximum here is **49 words**

and its table gives `max words fetched   25   49`.

Both DDF comparisons in `agnus_bitplanedma.v` force the low bit of the compared
value to zero — the trailing `1'b0` above. Start and stop therefore both land on
even `hpos[8:1]` positions, and the hires word count between them is structurally
**even**. 25 is reachable in lores; 49 is not reachable in hires by any register
pair. The measured maximum is 48.

**For the specialists:** 25 and 49 are both odd because the manual counts the
first word inclusively. Whether real Agnus can produce an odd hires count, or
whether the manual is counting differently from the hardware, decides which side
is wrong.

---

*Both found by the FPGA-091 chapter-3 build, 10.09.2026. Both confirmed against
the RTL by the lead before recording. Neither is decided here: the manual is this
project's oracle, so the test stays red until an Amiga specialist rules.*


## FPGA-091-F03 — playfield-1 collision depends on playfield 2, by design, undocumented

**Package:** `hrm/ch3-collision-registers`, the four `BITMAP-PAIR` cases involving
playfield 1. **Red.**

Appendix A's CLXDAT bit table, printed page 273, gives each collision pair its own
bit and treats them independently. NanoMig, `denise_collision.v:62-63`:

```verilog
assign oddmatch  = bm[6] & bm[4] & bm[2] & bm[0] & (dblpf | evenmatch);
assign evenmatch = bm[7] & bm[5] & bm[3] & bm[1];
```

With `dblpf = 0` — single playfield — `oddmatch` additionally requires
`evenmatch`. Playfield 1's presence is made to depend on playfield 2's.

**This is deliberate, and the RTL says so itself** in the comment directly above:

> this is the implementation of an undocumented function in the real Denise chip,
> developed by Yaqube. trigger was the game Rotor. mentioned in WinUAE sources to
> be the only known game needing this feature. it also fixes the Spaceport
> instantly helicopter crash at takeoff and Archon-1 'sticky effect' of player
> sprite at the battlefield.

**So this is very likely a case where the RTL is more correct than the manual.**
The manual documents the documented behaviour; the chip has more. The test is red
because the manual is this project's oracle, and that is the right outcome — but
the finding to carry forward is that appendix A's independence is incomplete, not
that NanoMig is wrong.

*For the specialists:* worth checking against WinUAE and vAmiga, which the comment
already points at. If they agree with NanoMig, the manual's table needs a footnote
rather than the RTL needing a change.

## FPGA-091-F04 — CLXDAT bit 15 reads as 1, not 0

> **WITHDRAWN** — validation 11.09.2026.
>
> Page 273 lists bit 15 as `not used` under the heading **COLLISIONS REGISTERED** -- it says no collision is registered there. It does not say the bit reads zero. NanoMig wires it high, which contradicts nothing printed. The expected zero was invented by the assertion, so the red measured the oracle, not the RTL.
>
> **Corrected the same day, and it settles the question.** The assertion now requires bit 15 to hold the *same* value before and after maximal collision stimulus, without claiming which value — which is what the page supports. It passes; 31/5 became 32/4. The implementation was right all along.

**Package:** `hrm/ch3-collision-registers`, assertion `BIT15-UNUSED`. **Red.**

Printed page 273 lists CLXDAT bit 15 as not used. NanoMig,
`denise_collision.v:117`:

```verilog
assign data_out = reg_address_in[8:1]==CLXDAT[8:1] ? {1'b1,clxdat[14:0]} : 16'd0;
```

Bit 15 is hard-wired high on every successful read and can never be 0.

*For the specialists:* the question is what "not used" means in this table — reads
as zero, or undefined. Every other assertion in the package masks the bit out with
`& 0x7FFF`, which scopes the measurement without weakening any oracle.

---

*F03 and F04 found by the FPGA-091 chapter-3 build, 10.09.2026, both confirmed
against the RTL by the lead before recording.*

## FPGA-091-F05 — the horizontal line is a whole number of colour clocks, not 227.5

> **SOURCE OBSERVATION, not test-backed** — validation 11.09.2026.
>
> The manual's 227.5 CCKs and the RTL's own admission that half-clocks are unsupported are both real. The test is not evidence for it: `want_227p5_in_ticks = 455` is `227.5 CCK * 2 hpos-ticks/CCK`, and that factor 2 is the RTL's internal representation, not the manual's. The expected value therefore comes from the implementation -- the same circularity as the `kHiresPixelTicks = 2` defect caught during the build, found a second time.
>
> **Resolved the same day by declining it.** No way was found to state the 227.5-colour-clock disagreement without an RTL-supplied conversion: the manual never names `htotal_out`, and the factor 2 lives in `agnus_beamcounter.v:206`'s write-doubling. The claim is now a recorded decline rather than an assertion — the honest outcome, and the one the correction brief explicitly permitted. The neighbouring `HTOTAL-H8-H1-BIT-MAPPING` was rebuilt as a pure relation instead — each bit's reading exactly double the one below, which is what page 304's `h8..h1` Use-row states — and it **passes**, with no unit conversion anywhere in it.

**Package:** `hrm/ch3-beam-counters-and-blanking`, assertion
`HTOTAL-FIXED-NOT-FRACTIONAL-227POINT5`, both PAL and NTSC. **Red.**

Printed page 304 gives the scan line as 227.5 colour clocks. `htotal_out` reads
**452** — a whole 226 doubled — under both standards.

NanoMig says so itself, `agnus_beamcounter.v:86`:

```verilog
parameter HTOTAL_VAL = 8'd227 - 8'd1;   // line length of 227 CCKs in PAL mode
                                        // (NTSC line length of 227.5 CCKs is not supported)
```

and line 105 adds *"long lines are not supported yet"*.

*For the specialists:* this is a known, deliberate simplification rather than a
mistake, and the half-clock is what produces the long-line/short-line alternation
the interlace machinery depends on. The question is whether anything in the
chapter-3 material depends on it observably, or whether the simplification is
invisible above this boundary.

## FPGA-091-F06 — PAL vertical blanking is 26 lines, the manual says 29

**Package:** `hrm/ch3-beam-counters-and-blanking`, assertion
`VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL`. **Red.** Its NTSC twin passes and is
the control.

Table 3-13, printed page 79:

    Vertical Blank Stop     $15 (21)    $1D (29)
                              NTSC         PAL

NanoMig, `agnus_beamcounter.v:89-90`:

```verilog
parameter VBSTOP_PAL_VAL  = 9'd25;   // vertical blanking end (PAL 26 lines, NTSC vblank 21 lines)
parameter VBSTOP_NTSC_VAL = 9'd20;
```

NTSC matches exactly at 21. PAL is **26 against the manual's 29** — a three-line
gap, and the RTL's own comment states 26 as the intent.

*For the specialists:* the NTSC twin passing on the same measurement basis is what
makes this survive the units-error objection. Either the manual's PAL figure is
wrong or NanoMig loses three lines of blanking; both are checkable against a real
machine.

---

*F05 and F06 found by the FPGA-091 chapter-3 build, 11.09.2026, both confirmed
against the RTL by the lead before recording. In both cases the RTL's own
comments acknowledge the divergence.*

## FPGA-091-F07 — SPRxCTL bit 0 (SH0) is honoured at every resolution, not only SuperHires

> **WITHDRAWN** — validation 11.09.2026.
>
> Page 303 reads `00  SH0  Start horiz. value Low bit 140 ns increment`. SH0 is the 140-ns low bit; the SuperHires positions are bits 4 (70 ns) and 3 (35 ns). The assertion imported a SuperHires-only restriction the manual does not state.
>
> **Corrected the same day, and it settles the question.** The assertion now requires only what the row states — toggling SPRxCTL bit 0 moves the start by exactly one raw unit, with no mode gate. It is **green**. The implementation was right; the oracle was not.

**Package:** `hrm/ch3-sprite-registers`, assertion
`SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES`. **Red.** Nine claim ids.

Printed pages 297 and 303 make SH0 the SuperHires half-pixel bit. NanoMig loads
it unconditionally and compares on it unconditionally,
`denise_sprites_shifter.v:141` and `:119`:

```verilog
{attach,hstart[0]} <= {data_in[7],data_in[0]};
load <= armed && (hpos[7:0] == hstart[7:0]) && (fmode[15] || (hpos[8] == hstart[8]));
```

`spres` appears nowhere in the comparator — its only use in the module is the
`case` at line 161, which sets how fast the already-loaded shift register drains.
So the sprite's start position responds to SH0 at all four `spres` encodings.

*For the specialists:* the same shape as F01, where DDFSTRT's H3 is gated by the
chipset strap rather than by resolution. Both are "a low-order position bit the
manual scopes to one mode and the RTL honours always". Worth deciding together.

## FPGA-091-F08 — SPRxCTL bits 3 and 4 are read nowhere in the module

> **WITHDRAWN** — validation 11.09.2026.
>
> Page 303 reads `03  SHSH0  Start horizontal (SHR mode) 35ns (unimplemented)` -- the manual itself calls bit 3 unimplemented. The note `bits 3 and 4 are in the ECS chips only` is a scope statement, not a promise that both alter output. Requiring bit 3 to be a live field contradicts the oracle.
>
> **Split the same day, and one half survives as F14.** Bit 3 is now declined, quoting the manual's own `(unimplemented)`. Bit 4 — which the manual *does* specify — was re-asserted on its own and is red for a reason that holds. The original finding was right that something is missing, and wrong about what.

**Package:** `hrm/ch3-sprite-registers`, assertion
`SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS`. **Red.**

The SPRxCTL write path captures exactly two bits — bit 7 (attach) and bit 0
(SH0), in the line above. Bits 3 and 4 are not referenced anywhere in
`denise_sprites.v` or `denise_sprites_shifter.v`.

That is a stronger statement than "ECS only": they are unimplemented in every
chip mode this module offers, including with `aga` asserted.

---

*F07 and F08 found by the FPGA-091 chapter-3 build, 11.09.2026, both confirmed
against the RTL by the lead before recording.*

## FPGA-091-F09 — the manual applies its own ECS pointer extension inconsistently

> **SOURCE OBSERVATION, not test-backed (as this entry already stated)** — validation 11.09.2026.
>
> Recorded in the package as `checks.note(...)`, whose own text says `Not folded into any PASS/FAIL above`. The manual/RTL disagreement is real; no assertion, red result or control stands behind it. Listed here for completeness of the audit.

**Package:** `hrm/ch3-register-map-agnus`, recorded as a note rather than an
assertion. This is the resolution of the pointer-width conflict that
the chapter's source-conflict register lists first
(`work/2026-09-10_fpga-091_chapter3_source_conflicts.md` in the pipeline repo;
it is not a file in this one).

Three sources, three widths:

| source | says |
|---|---|
| chapter 3, printed p.54 | a **19-bit** address |
| appendix A p.272 and appendix B p.291 | an **18-bit** pointer — "(high 3 bits)" plus low 15 |
| NanoMig, `agnus_bitplanedma.v:82` | `reg [20:16] bplpth` — **5** high bits, so **20** |

The deciding observation is inside appendix B itself. The manual **knows** about
the ECS extension and writes it out — `DSKPTH` on printed page 289 reads

    DSKPTH  + *020  W  A (E)  Disk pointer (high 3 bits, 5 bits if ECS)

and two further pointer rows on page 290 carry the same note. But **all fourteen
bitplane-pointer rows on page 291 say only "(high 3 bits)"**, with no ECS
qualifier at all.

So the likeliest reading is not that anybody is wrong about the hardware: it is
that the appendix's ECS annotation was applied to the disk and coprocessor
pointers and **not** carried through to the bitplane pointers. NanoMig implements
the ECS width the manual documents three pages earlier.

**What this means for the tests.** The pointer *addresses* are undisputed and are
asserted — all sixteen of them, green. The bit-width descriptor is not asserted;
it is recorded here. A package that asserted "high 3 bits" would go red against an
ECS-capable implementation for a reason that is a gap in the manual's own
editing.

*For the specialists:* the check is cheap — does ECS Agnus extend BPLxPTH to five
high bits the way it extends DSKPTH? If yes, page 291 needs the same qualifier the
manual already uses elsewhere, and chapter 3's "19-bit" is a third, separate slip.

---

*F09 found by the FPGA-091 chapter-3 build, 11.09.2026, confirmed against the RTL
and against appendix B's own DSKPTH row by the lead before recording.*

## FPGA-091-F10 — BRDNTRAN is declared and never connected

**Package:** `hrm/ch3-horizontal-display-window`, assertion
`D-BRDNTRAN-HAS-NO-EFFECT`. **Red.** Pages 296, 297, 307.

Appendix C gives BPLCON3 bit 4 as **BRDNTRAN**, "border opaque" — one of the
four new ECS genlock features. In NanoMig it is commented out twice, at its
declaration and at its assignment:

```verilog
wire          brdrblnk;   // border blank enable; disabled when ECSENA is low
//wire        brdntran;   // border area is non-transparent; disabled when ECSENA is low

assign brdrblnk = bplcon3[5] & ecsena;
//assign brdntran = bplcon3[4] & ecsena;
```

Writing bit 4 therefore changes nothing observable at this boundary.

**What makes this finding solid is the neighbour.** BRDRBLNK, bit 5 of the same
register, gated by the same `ecsena`, *is* wired — line 253 — and reaches
`t_blank` at line 515. The package asserts it (`G11-BRDRBLNK-WORKS`, green) on
the same measurement basis. So the fixture demonstrably can observe a BPLCON3
border control; bit 4 simply is not one.

*For the specialists:* this reads as unimplemented rather than wrong — the
commented-out lines look like work left for later, not a decision. Worth
confirming that nothing downstream silently depends on the border being opaque.

---

*F10 found by the FPGA-091 chapter-3 build, 11.09.2026, confirmed against the RTL
by the lead before recording.*

## FPGA-091-F11 — SuperHires sprite colour encoding is not implemented

> **HOLDS, with the count corrected** — validation 11.09.2026.
>
> The finding stands: fifteen live registers and the general mode comparison all measure identical colour sets at both `shres` settings, using deliberately asymmetric slot-A/slot-B values so a rearrangement would be visible. But **`REG-0x10` is an unconditional `expect(false)`**, so it is a declaration, not a measurement: the evidential count is sixteen reds, not seventeen. The per-register assertions also test only that *some* mode difference exists, not the exact symbol-to-bit mapping their claim ids cite.

**Package:** `hrm/ch3-shres-sprite-colour-encoding`. **17 red, 2 green.**
Pages 301, 302. The largest single red result in chapter 3.

Pages 301–302 give the ECS SuperHires sprite colour scheme: in SuperHires the
sixteen colour registers `$10`–`$1F` are read as *two* colour slots each — slot A
in the top bit of every 2-bit field (bits 11:10 R, 7:6 G, 3:2 B) and slot B in
the bottom (9:8, 5:4, 1:0) — so that a sprite can carry colour at the doubled
pixel rate. NanoMig reads them as ordinary colour registers in every mode.

**Root cause, verified in the RTL:** `denise_colortable.v` has no `shres` or
`spres` port at all. Its whole interface is mode-blind:

```verilog
module denise_colortable (
  clk, clk7_en, reg_address_in, data_in, rdram,
  select, bplxor, bank, loct, ehb_en, rgb );   // no shres, no spres
```

`spres` reaches the sprite path in exactly one place — `denise_sprites.v:158-166`,
where it sets `shift`, the shift-register drain rate. That changes how **wide** a
SuperHires sprite is, never what colour it is.

**What makes this measured rather than merely read.** The drain-rate effect is a
real confound: at `shres=1` a sprite is half as wide, so a raw sample trace
differs between modes for a reason that has nothing to do with colour. The
package therefore compares the *set of distinct colours* a sprite pixel can show,
which is width-invariant. Every live register returns the identical set:

```
MEASURED: REG-0x11: shres=0 distinct colours {R=2 G=1 B=3 (raw 0x9966CC)};
                    shres=1 distinct colours {R=2 G=1 B=3 (raw 0x9966CC)}.
```

— the same for `$11` through `$1F`. Reproduced by the lead from a deleted
`obj_dir`, not taken from the builder's run.

**Two controls hold the result up.** `FIXTURE-SPRITE-DATA-PATH-LIVE` and
`FIXTURE-SPR0-TRANSPARENT-AT-VALUE0` are green, so the fixture demonstrably
drives sprite pixels and observes their colour; the seventeen reds are an absent
feature, not a dark fixture. Both controls were mutated red and reverted.

**One claim group is architecturally unreachable, not undecided.** The 21 ids on
COLOR16 (`$10`) cannot be reached by live sprite display at all — combined sprite
index 0 is universally transparent. That is recorded `none impossible` and
confirmed by its own control rather than assumed.

*For the specialists:* this is a feature-completeness gap in an ECS mode, of the
same character as [F10] — not a wrong behaviour but an unbuilt one. Whether
SuperHires sprite colour is worth implementing is a scope question for NanoMig,
not a defect report.

---

*F11 found by the FPGA-091 chapter-3 build, 11.09.2026; RTL root cause and the
measurement independently reproduced by the lead before recording.*

## FPGA-091-F12 — SuperHires bitplane colour encoding is not implemented

> **SOURCE OBSERVATION on 11.09.2026 — RESTORED as test-backed the same day,
> once the witness was fixed.** The validator's objection was correct and the
> correction answers it: with `COLOR01 = 0x0C39` — every nibble's top two bits
> differing from its bottom two, so a rearrangement is visible in all three
> channels — the assertion still measures `ocs = shres = 0xCC3399`. Identical.
> Reproduced by the lead from a clean build. The finding stands, now on evidence
> that could have gone the other way.
>
> Two further corrections came out of the same pass. The sweep had been running
> at `bpu=4`, a depth page 300 does not define for SuperHires (*"The maximum
> plane depth for a SuperHires viewport is 2 bitplanes"*, and the register row
> `13  BPU1  } Depth of SuperHires mode (1 or 2)`); at a compliant depth
> `denise.v:369` forces `bpldata[3]` to zero, so registers 4–15 are not merely
> un-reinterpreted but unaddressable, and they are now declined on that ground.
> And `OCS-BASELINE-0RGB-FORMAT` had baked `denise_colortable.v`'s nibble-to-byte
> duplication into its expected value; it now checks only the top nibble of each
> component, which follows from "COLORxx stores 0x0RGB" alone. Still green.
>
> The original objection, preserved because it is the reason the finding is now
> worth more than it was:
>
> `denise_colortable.v` has no `shres` port -- that reading holds, and the lead confirmed it. The test does not establish it. `SHRES-REINTERPRETS-SAME-REGISTERS` writes `COLOR01 = 0x0FFF`, all ones in every nibble, and the claimed SuperHires encoding is a **rearrangement** of bits: rearranging all ones yields all ones. The assertion could not have gone green even against a perfect implementation, so its red is not evidence. This is the mirror of a circular oracle -- an assertion that cannot pass, for a reason unrelated to the design.

**Package:** `hrm/ch3-shres-bitplane-colour-encoding`. **19 red, 4 green.**
Pages 297, 301. **Same root cause as [F11], reached down a different path.**

Page 301 says SuperHires reinterprets the sixteen lower colour registers: the
stored 12-bit value is read as two-bit-per-channel data rather than as an
ordinary RGB triple. NanoMig reads every register identically in every mode.

**Verified in the RTL by the lead.** `denise_colortable.v` has no `shres` port —
the same absence that produces F11. Every occurrence of `shres` in the bitplane
path is shift-rate logic, and `denise_bitplanes.v:298` says so in its own words:

```verilog
// shifter enable: lowres shifts once every 4 cycles, hires every other, shres always
if (shres)      shift = 1'b1;
```

The remaining uses are all inside `denise_bitplane_shifter.v`. Nothing
mode-dependent reaches the colour table: `clut0` is instantiated with `select`,
`bplxor`, `bank`, `loct` and `ehb_en`, none of which carries the mode. So
`shres` changes how fast pixels are shifted out, never which colour-table
address is formed nor how a stored value is read back.

**The green that makes the reds meaningful.** `OCS-BASELINE-0RGB-FORMAT` passes:
the same fixture reads the same registers correctly in plain mode, on page
301's own 0RGB format. Three further controls pass
(`CONTROL-KNOWN-VALUES-ARRIVE`, `FIXTURE-DATA-PATH-LIVE`,
`SELECT-REGISTER-ADDRESSES-CORRECTLY`), each mutated red via its own build flag
and reverted. The nineteen reds are an absent feature, not a dark fixture.
Reproduced by the lead from a deleted `obj_dir`.

**Why F11 and F12 are recorded separately.** They share a cause but not a
measurement: F11 drives the sprite colour path, F12 the bitplane colour path,
and neither result depends on the other's fixture. One absent port, demonstrated
twice from opposite directions, is a firmer result than either package alone.

*For the specialists:* together F11 and F12 say that ECS SuperHires colour is
unimplemented in NanoMig as a whole, not in one corner of it. Whether that is
worth building is a scope question for the project.

---

*F12 found by the FPGA-091 chapter-3 build, 11.09.2026; RTL path and measurement
independently reproduced by the lead before recording.*

## FPGA-091-F13 — ECS BPLCON0 bit 4 (SPRHWRM) has no effect

> **WITHDRAWN as a conformance finding** — validation 11.09.2026.
>
> Page 300's row reads, in full, `04       SPRHWRM` -- a mnemonic with no description column, unlike `06  SHRES  SuperHires 35ns pixel enable bit` three rows above. The manual gives bit 4 a name and no behaviour, so there is no printed behaviour for NanoMig to diverge from. The fixture also drives bitplanes only and could not observe a sprite-horizontal function in any case. What remains true is a source observation: `denise.v:181` gives the bit its AGA meaning (`bplcon0[4] & aga`) and no ECS meaning. Recorded by the lead earlier the same day; the overreach was the lead's.
>
> **Corrected the same day.** Both claims are now declined rather than asserted, with the reason printed: the manual states no behaviour for this bit, and the fixture drives bitplanes while the mnemonic names a sprite function. Neither is asserted true nor false — the source simply does not define anything to test. 225/2 became 225/0.

**Package:** `hrm/ch3-bplcon0-display-modes` (225 green, 2 red). Page 300.

Appendix C's BPLCON0 table gives bit 4 as **SPRHWRM**. Both readers took that row
independently and wrote the same thing — `C-bplcon0.b04` (opus) and `C-shres.c18`
(sol), each quoting `04  SPRHWRM` from print page 300. Independent agreement
between the two readings is the strongest claim evidence this pipeline produces.

NanoMig gives bit 4 the **AGA** meaning instead, as the top bit of the
bitplane-count field, and gates it on `aga` alone:

```verilog
assign bpu = {bplcon0[4] & aga, bplcon0[14:12]};   // denise.v:181
```

With `ecs=1, aga=0` the bit is therefore dead. The package toggles it against a
working baseline and measures no effect anywhere at the boundary.

**The gating pattern next to it is what makes this a finding rather than an
oversight in our reading.** `denise.v` gates other ECS functions on `ecs`
exactly as expected — `killehb = bplcon2[9] & ecs`, `brdrblnk = bplcon3[5] &
ecsena`, `brdsprt`, `shres & ecs`, and DIWHIGH's writes. ECS gating is present
and used throughout the file. Bit 4 is simply not part of it.

### The finding was hidden by the test's own exit code

`main()` returned `0` unconditionally while printing
`=== ch3-bplcon0-display-modes: 225/227 PASS, 2 FAIL ===`. `run_all.sh` reads the
exit status, so the suite listed this package as **passing** for as long as it
existed, with both reds visible only inside its own directory. Fixed to
`return g_fail ? 1 : 0;` — a harness correction, no oracle touched. The package
now reports red to the suite, as it always should have.

This is the third instance in one day of the same shape: **a green report from
something that was not measuring.** The other two were the mutation gate reading
one assertion of nineteen, and six control assertions never mutated because a
`[control] ` prefix hid them.

*For the specialists:* NanoMig implements the AGA meaning of this bit and not the
ECS one. Whether SPRHWRM is worth having, or whether the AGA reading should win
outright, is a scope decision for the project — we only record that the printed
ECS behaviour is absent.

---

*F13 found by the FPGA-091 chapter-3 build, 11.09.2026; RTL and both claim
quotations checked by the lead before recording.*

## FPGA-091-F14 — SPRxCTL bit 4 (SHSH1) is not implemented

**Package:** `hrm/ch3-sprite-registers`, assertion
`SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE`. **Red.** Page 303.

**This is the half of the withdrawn F08 that survives**, re-established on a
claim the manual actually makes. F08 asserted that SPRxCTL bits 3 *and* 4 are
live fields, resting on the note `bits 3 and 4 are in the ECS chips only` — a
statement about which chips have them, not about what they do. For bit 3 the
manual settles it in the opposite direction, printing `(unimplemented)`. For bit
4 it prints a behaviour:

```
04      SHSH1     Start horizontal (SHR mode) 70ns increment
```

scoped by the page's own Warning (`70ns sprite positions are only available in
SuperHires mode`) and by the ECS note. That is a requirement, and NanoMig does
not meet it.

**Verified in the RTL by the lead.** The sprite shifter's CTL write keeps two
bits and discards the rest:

```verilog
// CTL register                          denise_sprites_shifter.v:137-142
always @(posedge clk)
  if (clk7_en) begin
    if (aen && address==CTL)
      {attach,hstart[0]} <= {data_in[7],data_in[0]};
  end
```

Only `data_in[7]` (attach) and `data_in[0]` (SH0) are captured. `data_in[4]` and
`data_in[3]` appear **nowhere** in `denise_sprites_shifter.v` or
`denise_sprites.v`. Bit 4 is dead at this boundary under any stimulus, which is
why its mutation is honestly recorded `none impossible`.

**What holds this up.** `SPRXCTL-BIT0-SH0-IS-140NS-LOW-BIT` is green on the same
fixture and the same measurement basis — a neighbouring bit of the same register,
written by the same CTL access, demonstrably observable. The fixture can see a
SPRxCTL bit move the sprite; bit 4 simply is not one.

*For the specialists:* 70 ns SuperHires sprite positioning is unbuilt, of a piece
with F11 and F12 — the ECS SuperHires feature set is broadly absent rather than
wrong in detail.

---

*F14 established 11.09.2026 while correcting F08, and confirmed in the RTL by the
lead before recording. Its existence is the argument for the validation step: the
original finding pointed at something real and described it incorrectly, and only
reading the printed page again separated the two.*
