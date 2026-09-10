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

**Package:** `hrm/ch3-register-map-agnus`, recorded as a note rather than an
assertion. This is the resolution of the pointer-width conflict that
`source_conflicts.md` lists first.

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
