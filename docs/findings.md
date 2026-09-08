# Findings

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

## Chapter 6 — read from the RTL, no test yet

These came out of the chapter-6 classification stage on 08.09.2026, while it was
looking for a fixture boundary. They are **not red tests**: nothing asserts them
yet, and until something does they are observations, not findings in the sense
this file otherwise uses.

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
| `BLTxPTH` $050 chg — "high 5 bits, was 3 bits" | `agnus_blitter_adrgen.v:45`, `reg [20:16] bltpth [3:0]` | **no** — that module has no `ecs` port at all |
| `DOFF`, BLTCON1 bit 7 — disable D output in line mode | nowhere | **not implemented** |

`bltcon1[7]` does not appear in `agnus_blitter.v`. The bits it reads are 15-12,
4, 3, 2, 1 and 0.

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
