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

## Open findings

### FPGA-050 — PAL frame has one line too many

| | |
|---|---|
| Manual | normal PAL non-interlace is 312 lines, `vpos` 0..311 |
| Measured | `max_vpos=312`, 313 end-of-line pulses before end of frame |
| Test | `hrm/agnus_beamcounter` |

Before changing the RTL, the interlace and long-frame side effects have to be
checked — a beam counter is not a local decision.

### FPGA-051 — audio channels land on the wrong stereo side

| | |
|---|---|
| Manual | channels 0 and 3 route right, channels 1 and 2 route left |
| Measured | exactly inverted on all four channels (`expected L=0 R=17, got L=17 R=0`) |
| Test | `hrm/paula_audio_mixer` |

Open question is *where* to correct it: the swap may sit in the top-level or
board wiring rather than in `paula_audio_mixer` itself. Fixing it in the wrong
place would swap it twice.

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
