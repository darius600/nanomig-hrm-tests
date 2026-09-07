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
| FPGA-050 | hand-written, Aug | **contested** — the manual sanctions the measured value |
| FPGA-051 | hand-written, Aug | **contested** — the manual states both sides |

That is not a coincidence. Two independent readings, mechanical anchoring and an
attack on the discards exist precisely to catch a claim that rests on one page
while another page says otherwise.

## Withdrawn on 07.09.2026

Both contested assertions were removed rather than left red. A red test is a
claim that the RTL is wrong; neither of these could support that claim, and
leaving them red would have kept two false accusations in the suite.

Neither *file* was deleted — both carried checks unrelated to the contested
claim, and those still run:

- `agnus_beamcounter` still asserts that a PAL field terminates with EOF and that
  the horizontal counter spans 0..453. That second one matters beyond this file:
  `agnus_copper_waitskip_range`'s W4b rests on the line being 454 units long.
- `paula_audio_mixer` still guards the mixer's signed arithmetic, which is a
  separate question from its left/right labels. Its class is now
  `RTL_REGRESSION`, since the manual is no longer serving as its oracle.

Neither replaces the withdrawn assertion with a corrected one. Stating the right
value needs a reading that has not been done, and an assertion guessing at it
would be worse than none.

The reasoning that withdrew them:

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

This needs resolving as a reading of the manual before it is treated as an RTL
defect at all.

### FPGA-051 — audio channels on the wrong stereo side

| | |
|---|---|
| Test asserts | channels 1+2 feed left, channels 0+3 feed right (chapter 5, p.137) |
| RTL does | `ldatasum <= msample0 + msample3; rdatasum <= msample1 + msample2` |
| Test | `hrm/paula_audio_mixer` |

**The manual says both.**

- Chapter 5, p.137: *"Channels 1 and 2 are connected to the left-side stereo
  output jack. Channels 0 and 3 are connected to the right-side output jack."*
- Appendix pin list: *"AUDO — Audio output from **left (channels 0, 3)** port"*,
  *"AUDI — Audio input to **right (channels 1, 2)** port"*

The RTL implements the pin list. Both statements appear in both editions, so this
is a contradiction inside the oracle, not a deviation from it.

A third witness is needed — the same shape as the chapter-2 conflict between p.21
and p.26, which appendix B's marker column settled. Until then, changing the RTL
would mean picking a page.

## Corroborated findings

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

## How a finding is meant to end

A red test is closed by a decision about the RTL, never by relaxing the
assertion. If a deviation turns out to be intentional, the assertion stays and
the reason is written down beside it — a test whose expectation was quietly moved
to match the implementation stops being evidence about anything.
