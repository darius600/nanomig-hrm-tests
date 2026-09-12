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

**There is exactly one exception:** an assertion that misstates the source must be corrected, not because
the implementation fails it, but because it never said what the manual says. The
test is the thing being fixed there, not the finding. Telling the two apart is
the whole discipline — the question is never "does this pass?" but "does the
printed page require what this asserts?".

> **An independent validation revised seven of chapter 3's findings.** Every
> affected entry below carries its own note; the round's own account is in
> [`record-2026-09-11-validation.md`](record-2026-09-11-validation.md).

## The finding keys

Findings are numbered **`NM-HRM-nnn`** -- `NM` for the implementation under test,
`HRM` for the *Amiga Hardware Reference Manual* that judges it. The key names the
**pair that disagrees**, and it does so deliberately, because it must not
prejudge which side is wrong: on six of the findings below, two independent
emulators do what NanoMig does and the printed sentence is the outlier.

A number is issued once. It is never reused, never reordered, and a withdrawn
finding keeps both its number and its entry -- the reasoning that withdrew it is
the result, not a deletion.

**Why the keys were renumbered.** Findings previously carried
`FPGA-nnn` keys borrowed from the private workspace where this work is planned.
That workspace is not published. No reader outside it can resolve such a
reference, so a failing assertion printed a key that led nowhere -- and the
borrowed numbers carried the gaps of a task sequence (`050, 051, 060, 062, 069`)
that has nothing to do with findings. Where an older commit message or artefact
cites an `FPGA` number, this resolves it:

| now | was | | now | was | | now | was |
|---|---|---|---|---|---|---|---|
| NM-HRM-001 | FPGA-050 | | NM-HRM-009 | FPGA-091-NM-HRM-009 | | NM-HRM-016 | FPGA-091-NM-HRM-016 |
| NM-HRM-002 | FPGA-051 | | NM-HRM-010 | FPGA-091-NM-HRM-010 | | NM-HRM-017 | FPGA-091-NM-HRM-017 |
| NM-HRM-003 | FPGA-060 | | NM-HRM-011 | FPGA-091-NM-HRM-011 | | NM-HRM-018 | FPGA-091-NM-HRM-018 |
| NM-HRM-004 | FPGA-062 | | NM-HRM-012 | FPGA-091-NM-HRM-012 | | NM-HRM-019 | FPGA-091-NM-HRM-019 |
| NM-HRM-005 | FPGA-069 | | NM-HRM-013 | FPGA-091-NM-HRM-013 | | NM-HRM-020 | *newly issued* |
| NM-HRM-006 | FPGA-091-NM-HRM-006 | | NM-HRM-014 | FPGA-091-NM-HRM-014 | | NM-HRM-021 | *newly issued* |
| NM-HRM-007 | FPGA-091-NM-HRM-007 | | NM-HRM-015 | FPGA-091-NM-HRM-015 | | NM-HRM-022 | *newly issued* |
| NM-HRM-008 | FPGA-091-NM-HRM-008 | | | | | | |

`FPGA-nnn` still appears in test sources in a second, unrelated role, and those
references are kept: `// Primary oracle: Frozen FPGA-019 specification` marks a
package whose oracle is a frozen module specification rather than the manual, and
therefore **not** an HRM-conformance test. That distinction is worth more than
the resolvability of the number.

## The register

Every finding, with the package that measures it and what the third witness says.
**The emulators are not an oracle and never decide a finding** -- they are read
only for the report to the hardware developers
([`third-witness-ch3-ch6.md`](third-witness-ch3-ch6.md), whose section letters
this column cites). The manual decides; a red result stands until an Amiga
specialist rules on it.

| Key | Subject | Status | Package | Third witness |
|---|---|---|---|---|
| [NM-HRM-001](#nm-hrm-001--pal-frame-has-one-line-too-many) | PAL field terminates at 313 lines, not 312 | **withdrawn** | `hrm/agnus_beamcounter` | **all four** run a 313-line PAL field; Table 7-5 sanctions 313 |
| [NM-HRM-002](#nm-hrm-002--audio-channels-on-the-wrong-stereo-side) | audio channels on the wrong stereo side | **red** | `hrm/paula_audio_mixer` | **2–2**: WinUAE and vAmiga with chapter 5; Denise and Copperline with the pin list |
| [NM-HRM-003](#nm-hrm-003--ocs-copper-address-uses-five-high-bits-instead-of-three) | OCS Copper pointer uses five high bits, not three | **red** | `hrm/agnus_copper_control`, `hrm/agnus_copper_locreg_width` | **Copperline narrows to 19 bits on OCS**, matching the manual; the other three do not |
| [NM-HRM-004](#nm-hrm-004--ecs-danger-bit-admits-too-small-a-range) | ECS danger bit admits too small a range | **red** | `hrm/agnus_copper_move_range`, `hrm/agnus_copper_move_sweep` | both block below `$80` regardless of chipset |
| [NM-HRM-005](#nm-hrm-005--wait-and-skip-take-longer-than-the-manual-states) | WAIT and SKIP take longer than the manual states | **red** | `hrm/agnus_copper_cycle_times`, `hrm/agnus_copper_cycles` | vAmiga gives NanoMig's three numbers |
| [NM-HRM-006](#nm-hrm-006--ddfstrtddfstop-bit-h3-is-gated-by-the-chipset-not-by-resolution) | DDFSTRT bit H3 gated by chipset, not by resolution | **red** | `hrm/ch3-ddf-fetch-window` | **all four** gate DDF on the chipset; none on resolution |
| [NM-HRM-007](#nm-hrm-007--the-hires-fetch-width-cannot-reach-the-manuals-maximum-of-49-words) | hires fetch cannot reach the manual's 49 words | **red** | `hrm/ch3-ddf-fetch-window`, `hrm/ch3-bitplane-memory-layout` | **B4** for the first two; Copperline asserts an even hires word count in its own test |
| [NM-HRM-008](#nm-hrm-008--playfield-1-collision-depends-on-playfield-2-by-design-undocumented) | playfield-1 collision depends on playfield 2 | **red** | `hrm/ch3-collision-registers` | **all four** gate playfield 1 on playfield 2 in single-playfield mode |
| [NM-HRM-009](#nm-hrm-009--clxdat-bit-15-reads-as-1-not-0) | CLXDAT bit 15 reads as 1, not 0 | **withdrawn** | `hrm/ch3-collision-registers` | **all four** set bit 15 on every CLXDAT read |
| [NM-HRM-010](#nm-hrm-010--the-horizontal-line-is-a-whole-number-of-colour-clocks-not-2275) | the scan line is a whole number of colour clocks | source observation | `hrm/ch3-beam-counters-and-blanking` | Denise and Copperline both alternate whole line lengths |
| [NM-HRM-011](#nm-hrm-011--pal-vertical-blanking-is-26-lines-the-manual-says-29) | PAL vertical blanking is 26 lines, manual says 29 | **red** | `hrm/ch3-beam-counters-and-blanking` | **B6** for the first two; Denise's PAL-NTSC gap is 5 lines like NanoMig's, not the manual's 8 |
| [NM-HRM-012](#nm-hrm-012--sprxctl-bit-0-sh0-is-honoured-at-every-resolution-not-only-superhires) | SPRxCTL bit 0 (SH0) honoured at every resolution | **withdrawn** | `hrm/ch3-sprite-registers` | three of four take SH0 at every resolution, as NanoMig does |
| [NM-HRM-013](#nm-hrm-013--sprxctl-bits-3-and-4-are-read-nowhere-in-the-module) | SPRxCTL bits 3 and 4 are read nowhere | **withdrawn**, split into NM-HRM-019 | `hrm/ch3-sprite-registers` | bit 4 is real from ECS on, bit 3 only on AGA -- the split this was withdrawn into |
| [NM-HRM-014](#nm-hrm-014--the-manual-applies-its-own-ecs-pointer-extension-inconsistently) | the manual applies its own ECS pointer note inconsistently | source observation | `hrm/ch3-register-map-agnus` | one of four extends every DMA pointer by chipset; no implementation can settle a question about the manual's own text |
| [NM-HRM-015](#nm-hrm-015--brdntran-is-declared-and-never-connected) | BRDNTRAN is declared and never connected | **red** | `hrm/ch3-horizontal-display-window` | **B3** for the first two; **Copperline implements it, with a test** |
| [NM-HRM-016](#nm-hrm-016--superhires-sprite-colour-encoding-is-not-implemented) | SuperHires sprite colour encoding not implemented | **red** | `hrm/ch3-shres-sprite-colour-encoding` | **2-2**: WinUAE and vAmiga reinterpret; Denise and Copperline do not |
| [NM-HRM-017](#nm-hrm-017--superhires-bitplane-colour-encoding-is-not-implemented) | SuperHires bitplane colour encoding not implemented | **red** | `hrm/ch3-shres-bitplane-colour-encoding` | **2-2**: the same split as NM-HRM-016, from the same reading |
| [NM-HRM-018](#nm-hrm-018--ecs-bplcon0-bit-4-sprhwrm-has-no-effect) | ECS BPLCON0 bit 4 (SPRHWRM) has no effect | **withdrawn** | `hrm/ch3-bplcon0-display-modes` | neither Denise nor Copperline gives bit 4 an effect on ECS |
| [NM-HRM-019](#nm-hrm-019--sprxctl-bit-4-shsh1-is-not-implemented) | SPRxCTL bit 4 (SHSH1) is not implemented | **red** | `hrm/ch3-sprite-registers` | **C2** for the first two; Denise and Copperline both implement bit 4 |
| [NM-HRM-020](#nm-hrm-020--bltsizv-is-not-gated-on-ecs-while-its-two-siblings-are) | BLTSIZV is not gated on ECS, while its siblings are | **red** | `hrm/agnus_blitter_ecs` | **A2** — **all four** gate it; NanoMig alone does not |
| [NM-HRM-021](#nm-hrm-021--doff-bltcon1-bit-7-has-no-observable-effect) | DOFF (BLTCON1 bit 7) has no observable effect | **red** | `hrm/agnus_blitter_ecs` | **C1**; Denise implements it and ECS-gates it |
| [NM-HRM-022](#nm-hrm-022--bltxpth-carries-five-high-bits-in-both-chipsets) | BLTxPTH carries five high bits in both chipsets | **red** | `hrm/agnus_blitter_ecs` | **B2** — but Copperline narrows every DMA pointer by Agnus revision |
| [NM-HRM-023](#nm-hrm-023--the-attach-bit-is-honoured-in-the-even-sprite-not-only-the-odd) | the ATTACH bit is honoured in the even sprite, not only the odd | **red** | `hrm/ch4-attached-sprites` | **Denise applies the manual's rule exactly**: `(nr & 1) && (value & 0x80)` |
| [NM-HRM-024](#nm-hrm-024--the-audio-dma-pointer-takes-five-high-bits-in-both-chipsets) | the audio DMA pointer takes five high bits in both chipsets | **red** | `hrm/ch5-audio-location-pointer` | **B2**, as NM-HRM-003 and NM-HRM-022 — Copperline alone narrows by revision |
| [NM-HRM-025](#nm-hrm-025--volume-64-and-volume-63-are-the-same-gain-on-the-multiply-path) | volume 64 and volume 63 are the same gain on the multiply path | **red** | `hrm/ch5-audio-volume-scale`, `hrm/ch5-audio-register-map` | **all four** clamp at 64 and keep it distinct; two say why in their own comments |
| [NM-HRM-026](#nm-hrm-026--the-audio-state-codes-are-not-the-ones-figure-5-8-prints) | the audio state codes are not the ones Figure 5-8 prints | source observation | `hrm/ch5-audio-channel-state` | **all four** use the figure's numbering; Copperline names the five constants with it |
| [NM-HRM-027](#nm-hrm-027--in-direct-mode-the-interrupt-is-raised-before-a-word-is-output) | in direct mode the interrupt is raised before a word is output | **red** | `hrm/ch5-audio-direct-output` | not read — and the manual states the moment four different ways |
| [NM-HRM-028](#nm-hrm-028--adkcons-audio-attach-bits-are-stored-and-reach-nothing) | ADKCON's audio attach bits are stored and reach nothing | **red** | `hrm/ch5-audio-dmacon-adkcon` | **all four** implement attach and modulation; NanoMig's own first line says it does not |
| [NM-HRM-029](#nm-hrm-029--figure-5-8s-dmasen-note-does-not-hold-outside-its-own-exception) | Figure 5-8's `dmasen` note does not hold outside its own exception | **withdrawn** | `hrm/ch5-audio-channel-state` | not comparable — `dmasen` appears at no pin |

Chapters 1, 7 and 8 have not been extracted, so no finding below comes from
them. The register is a record of what has been looked at, not of what is there.

## Where the five oldest findings come from, and whether they hold


Five findings, but not of equal standing. Three came out of the pipeline and are
witnessed more than once. Two came out of the earlier hand-written run, rest on a
single citation each, and both of those citations turn out to be contradicted
elsewhere in the same manual.

| Finding | Origin | Standing |
|---|---|---|
| [NM-HRM-003](#nm-hrm-003--ocs-copper-address-uses-five-high-bits-instead-of-three) | pipeline | corroborated — p273 and p309 give two different widths |
| [NM-HRM-004](#nm-hrm-004--ecs-danger-bit-admits-too-small-a-range) | pipeline | corroborated — two editions carry the appendix C sentence; challenged and upheld |
| [NM-HRM-005](#nm-hrm-005--wait-and-skip-take-longer-than-the-manual-states) | pipeline | corroborated — p20 and p275, units settled on p275 itself |
| [NM-HRM-002](#nm-hrm-002--audio-channels-on-the-wrong-stereo-side) | hand-written, Aug | **stands** — the manual contradicts itself, and four implementations split two against two |
| [NM-HRM-001](#nm-hrm-001--pal-frame-has-one-line-too-many) | hand-written, Aug | **withdrawn** — the manual sanctions the measured value |

Both August findings rested on a manual that says two different things, and
neither could be decided by reading the manual harder. They were settled by a
third witness — see [`third-witness.md`](third-witness.md) — and they went
opposite ways: one survived, one did not.

## NM-HRM-001 is withdrawn — and why the entry stays

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

### NM-HRM-001 — PAL frame has one line too many

| | |
|---|---|
| Test asserts | normal PAL non-interlace is 312 positions, `vpos` 0..311 |
| Measured | `max_vpos=312`, 313 end-of-line pulses |
| Test | `hrm/agnus_beamcounter` |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| follows the long-frame bit | follows the long-frame bit | `agnus.cpp:178` `lines = ntsc ? 261 : 311`, frame restarts at `vPos == lines + lof` | `agnus.rs:7` `PAL_LINES = 313`; non-interlaced fields are all long frames |

**All four run a 313-line PAL field.** Table 7-5 sanctions 313.

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

### NM-HRM-002 — audio channels on the wrong stereo side

| | |
|---|---|
| Manual, chapter 5 p.137 | channels 1+2 feed left, channels 0+3 feed right |
| Manual, appendix pin list | the opposite: *"AUDO — Audio output from left (channels 0, 3) port"* |
| RTL does | `ldatasum <= msample0 + msample3; rdatasum <= msample1 + msample2` — the pin list |
| Test | `hrm/paula_audio_mixer` |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| channel 0 right — chapter 5 | channels 0,3 at pan 0.85 — chapter 5 | `paula.cpp:354` sums 0+3 into **left** — the pin list | `paula.rs:2180` puts 0+3 **left**, knowingly |

**Two against two.** Nothing outside the manual settles it.

**The manual says both**, in both editions, so it cannot decide this on its own.

**Four implementations split two against two.** WinUAE and
vAmiga agree with chapter 5: WinUAE hands channel 0 to `put_sound_word_right`,
and vAmiga's default panning puts channels 0 and 3 at `pan = 0.85` on an axis
where 1 is fully right. **Denise and Copperline agree with the appendix pin list**,
which is what NanoMig does — `paula/paula.cpp:354` sums channels 0 and 3 into the
left sample, and Copperline's own comment says it knows the HRM prose describes
the opposite side and keeps the pin-list routing anyway. See
[`fourth-witness-denise-copperline.md`](fourth-witness-denise-copperline.md).

This entry previously said two emulators settle it against the RTL. They do not.
The red stands — the manual contradicts itself and an implementation has to pick
a side — but nothing outside the manual settles which side is right.

**The NanoMig stereo channels are swapped.** Permalinks into both emulators are
in [`third-witness.md`](third-witness.md).

Where to correct it is still open: the swap may belong in the top level or the
board wiring rather than in `paula_audio_mixer` itself, and fixing it in two
places would swap it back.

### NM-HRM-003 — OCS Copper address uses five high bits instead of three

| | |
|---|---|
| Manual | p273, p309: OCS has three high address bits, ECS five; the location registers are 18-bit pointers (p276) |
| Measured | five high bits in both chipsets; 20-bit pointer (15 low + 5 high); `cop1lch[20:16] <= data_in[4:0]` latches unconditionally |
| Tests | `hrm/agnus_copper_control` (T4), `hrm/agnus_copper_locreg_width` (LW1, LW3, LW4) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| no chipset narrowing | no chipset narrowing | `copper.cpp:347` takes the whole write, no mask | **narrows to 19 bits on OCS** — the manual's width |

One of four implements what the manual describes.

LW3 is the assertion that matters: the appendix gives *two different* widths for
the two chipsets, so a module reporting the same width in both contradicts the
manual whatever that width is.

### NM-HRM-004 — ECS danger bit admits too small a range

| | |
|---|---|
| Manual | Appendix C p310: with ECS and CDANG clear the Copper reaches `$DFF03E`–`$DFF07E`, "the same range as when the danger bit is set in the standard chip set" |
| Measured | that range is rejected; with CDANG clear the accepted set is `$080`–`$1FE` at `ecs=0` and `ecs=1` alike, so 32 addresses `$040`–`$07E` are falsely rejected and none falsely accepted |
| Tests | `hrm/agnus_copper_move_range` (T9), `hrm/agnus_copper_move_sweep` |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| blocks below $80 | blocks below $80 | `copper.cpp:342` floor $80 when the danger bit is clear | `dma_slots.rs:697` needs the bit **set** for $040–$07E |

All four read appendix C p310 the other way round.

Addresses here are the manual's byte addresses. A Copper instruction encodes
DA8-1, so the module's register ports carry them halved and the sweep prints them
that way; an earlier version of this entry quoted the halved form without saying
so, which made it unreconcilable with the test it cites.

**Corroborated on both sides, but not three ways.** The manual side carries in two
editions: the Developer-CD text and the OCR of the printed book give the sentence
word for word. The measured side has the single-address probe and the exhaustive
sweep agreeing.

An earlier version counted appendix B's marker column as a third corroboration.
It is not one. That column describes the standard chip set, where the RTL already
agrees with it, so it cannot disagree with the design here — and a record that
cannot disagree with the design is not evidence. What it does contribute is the
*referent* of appendix C's comparison: it fixes which addresses "the danger bit
set in the standard chip set" denotes, and it does so exactly. All 32 word
addresses in `$040`–`$07E` appear in the table and **all 32 carry the CDANG
marker** — the 26 named registers and the six reserved gaps that carry a marker
and an address with no name beside them (`$068`, `$06A`, `$06C`, `$06E`, `$076`,
`$07A`). None is unmarked and none is missing. Below `$040` the 32 addresses are
marked `*`, not writable by the Copper at all.

The marked set and the measured set of falsely rejected addresses are therefore
the same 32 addresses, one for one.

#### The challenge, and why this entry stands

The chapter-2 appendix classification proposed that T9's oracle is wrong rather
than the RTL. It measured the same `$080`–`$1FE` at both chipsets and preferred
appendix B's legend to appendix C p310. Appendix B cannot govern this case, and
says so itself:

> "About the ECS registers. Registers denoted with an "(E)" in the chip column
> means that those registers have been changed in the Enhanced Chip Set (ECS).
> [...] See the register map in Appendix C for information on which ECS registers
> are new and which have been modified." — appendix B p288

COPCON is listed there as `*02E W A(E)`: marked ECS-changed, with the change
documented only in appendix C. The manual states its own order of precedence for
exactly this register, and the challenge inverts it. Neither appendix B's legend
nor the chapter body on p26 mentions ECS at all, so neither can settle the ECS
case. The oracle stands and T9's red is unchanged.

One imprecision inside appendix C is recorded rather than resolved: it gives the
lower bound as `$DFF03E`, which is STRLONG, a strobe that appendix B marks `& *`,
never Copper-writable. The sentence's stated purpose is to permit writes "to the
Blitter hardware", whose first register is BLTCON0 at `$040`. T9 targets `$040`
and holds on either reading of the bound.

### NM-HRM-005 — WAIT and SKIP take longer than the manual states

| | |
|---|---|
| Manual | p20, p275: a SKIP spans four memory cycle times, a satisfied WAIT six, a WAIT two more than a MOVE |
| Measured | SKIP 8, satisfied WAIT 8, WAIT-minus-MOVE 4 |
| Tests | `hrm/agnus_copper_cycle_times` (P3–P6, AC5, AC6), `hrm/agnus_copper_cycles` (C5, C6) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| not compared | 4 / 8 / 8 — NanoMig's three numbers | — | — |

vAmiga measures what NanoMig measures.

The unit question this depended on is settled: appendix A p275 defines
bus cycle against memory cycle time and gives the factor two, so the readings are
comparable and the deviation is real. AC6 is expository, not independent — given
AC4 and AC5 it cannot fail on its own.

## Chapter 6 — the ECS blitter registers

Appendix C lists five ECS changes to the blitter. `agnus_blitter` takes an `ecs`
input and uses it for two of them. Three are findings and they all live in
`hrm/agnus_blitter_ecs`, each required to fail:

| Assertion | Measured |
|---|---|
| `BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS` | a BLTSIZV write with `ecs=0` and with `ecs=1` gives the identical height, r1=7 and r2=7 |
| `DOFF-NO-OBSERVABLE-EFFECT` | the same D write appears with BLTCON1 bit 7 clear and set |
| `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` | `BLTAPTH` written with `$001F` gives `address_out = 0xF8000` at both `ecs` settings |

Each names every claim it covers in its own message, so the red line is readable
without this file. The two remaining rows of the five-way table below are not
defects: BLTCON0L and BLTSIZH are gated correctly and pass.

### NM-HRM-020 — BLTSIZV is not gated on ECS, while its two siblings are

| | |
|---|---|
| Manual | Appendix C's ECS register map, printed page 298, marks BLTSIZV `$05C` **new for ECS**, the same status it gives BLTSIZH `$05E` and BLTCON0L `$05A` |
| Measured | a BLTSIZV write with `ecs=0` and with `ecs=1` gives the identical height, r1=7 and r2=7 |
| Test | `hrm/agnus_blitter_ecs` (`BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS`) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| gates all three | gates all three | `register.cpp:220-230` gates all three | `custom_regs.rs:787` gates it |

**All four gate BLTSIZV on ECS. NanoMig alone does not.**

**The control is the point.** `BLTCON0L-GATED-BY-ECS` is green on the same
fixture, so this fixture's `ecs` demonstrably changes behaviour elsewhere. Three
registers given one status by one appendix page; two honour it, one does not.

`agnus_blitter.v:548` carries the contradiction in its own text -- the comment
calls the register ECS and the code does not gate it:

```verilog
    else if (reg_address_in[8:1]==BLTSIZV[8:1]) // ECS BLTSIZV register
        height[14:0] <= data_in[14:0];
```

against line 259, where the sibling is gated: `… BLTCON0L[8:1] && ecs`.

**Third witness: both emulators side with the manual** (section A2). This is one
of only two findings in chapters 3 and 6 where the manual, vAmiga and WinUAE all
stand on the same side and NanoMig alone is the outlier -- which makes it the
strongest of the three here.

### NM-HRM-021 — DOFF (BLTCON1 bit 7) has no observable effect

| | |
|---|---|
| Manual | Appendix C p.309 lists `DOFF` among the five ECS blitter changes: disable D output in line mode |
| Measured | the same D write appears with BLTCON1 bit 7 clear and with it set |
| Test | `hrm/agnus_blitter_ecs` (`DOFF-NO-OBSERVABLE-EFFECT`) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| implements DOFF | absent, a constant only | `blitter.cpp:297` implements **and** ECS-gates it | no decode found for BLTCON1 bit 7 |

Two of four implement it.

`bltcon1[7]` does not occur in `agnus_blitter.v` at all. The bits that module
reads are 15-12, 4, 3, 2, 1 and 0. This is an unbuilt feature rather than a wrong
one.

**Third witness: the two emulators disagree with each other** (section C1) --
the only place in this chapter where the third witness cannot break a tie. It
therefore says nothing about this finding in either direction, and the manual
decides alone, which it does clearly.

### NM-HRM-022 — BLTxPTH carries five high bits in both chipsets

| | |
|---|---|
| Manual | Appendix A p.270: "Blitter pointer to x (high 3 bits, 5 bits if ECS)" |
| Measured | writing `BLTAPTH` with `$001F` gives `address_out = 0xF8000` at `ecs=0` **and** at `ecs=1` -- identical |
| Test | `hrm/agnus_blitter_ecs` (`BLTXPTH-WIDTH-REGARDLESS-OF-ECS`) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| wide register | wide, narrowed on access by chipset | no mask on the high word | **narrows every DMA pointer by Agnus revision** |

The same question as NM-HRM-003, one engine over.

Measured at `agnus_blitter`, the parent that has the `ecs` port, after Prompt 8's
validation rejected handling it one fixture lower where the port does not exist.
The full measurement, its control and its mutation are recorded below.

**Same root cause as NM-HRM-003, and recorded separately on purpose.** The Copper
and the blitter build their pointers from the same bits with no chipset condition
on either, so the two findings very likely have one answer. They keep separate
numbers because they rest on separate measurements at separate boundaries --
the same rule that keeps NM-HRM-016 and NM-HRM-017 apart. Whether they should be
closed by one decision is a question for the specialists, and the cross-reference
below is what puts it to them.

**Third witness: both emulators side with NanoMig** (section B2) -- neither
narrows a pointer by chipset at all. Read with NM-HRM-003, where the same is
true, this points at the manual rather than at the RTL. It does not close either
one: the manual is this project's oracle, and an emulator never promotes to that.

Every statement below was checked by reading the RTL directly, not taken from the
classifier's report.


### NM-HRM-023 — the ATTACH bit is honoured in the even sprite, not only the odd

| | |
|---|---|
| Manual, p128 | "Bit 7 is the attach bit. This bit is valid only for odd-numbered" sprites |
| Manual, p122 | "Attachment is in effect only when the ATTACH bit, bit 7 in sprite control word 2, is set to 1 in the data structure for the odd-numbered" sprite |
| Measured | with `aga = 0`, setting bit 7 of **SPR0**CTL — the even sprite of the pair — opens the combined-colour path on its own |
| Test | `hrm/ch4-attached-sprites` (`ATTACH-BIT-VALID-ONLY-FOR-ODD-SPRITE`) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| `drawing.cpp:2838` — `dspr[num & ~1].attached = (dspr[num \| 1].ctl & 0x80)`, the **odd** sprite only | `Denise.cpp:1130` reads `sprctl[sprite2]`, and `sprite2 = 2*pair + 1` — the odd one | `denise.cpp:300` — `(nr & 1) && (value & 0x80)`, the manual's rule exactly | reads the bit without the odd-sprite condition *(source reading, not run)* |

**Three of four take the attach bit from the odd sprite only, as the manual states.**
Copperline appears to accept either — a source reading, not a measurement.

`denise_sprites.v:484`:

```verilog
if (attach1 || (!aga && attach0)) // sprites are attached -> 15 colors + transparant
```

`attach0` is the even sprite's bit, and `!aga` is the only thing gating it.

**The manual says it twice, on two pages**, and neither sentence admits an
exception. The chapter's own design depends on it: the odd sprite's control word
is what carries the attachment, which is why its data words supply the high two
bits of the 4-bit colour index.

**Scoped by its own mutation.** Setting `aga = 1` zeroes the `!aga` term and the
implementation then matches the manual — so this is an OCS/ECS behaviour, exactly
the chipset the package drives, and not an AGA one.

**What holds it up.** `ATTACH-BIT-ODD-SPRITE-ENABLES-ATTACHMENT` is green on the
same fixture and the same measurement basis: the odd sprite's bit does attach the
pair, so the fixture demonstrably observes attachment. A separate
`[control] FIXTURE-LIVE-DATA-PATH` proves the sprite data path delivers distinct
known values in order before anything reads one.

**Found three times before it was built.** Two classifiers working different lots
of chapter 4 measured it independently, neither having seen the other's work; the
lead then verified it against the source and both printed pages; and the builder
reproduced it from the quotations alone, having written and committed its
specification before opening the RTL.

*For the specialists:* the `!aga` term looks deliberate rather than accidental —
somebody wrote a chipset condition here on purpose. Whether real OCS Denise
honours the even sprite's attach bit is the question, and it is cheap to check
against a real machine.

### Appendix C's five ECS blitter changes are implemented five different ways

Appendix C, printed page 309, lists what ECS changed in the blitter. NanoMig's
`agnus_blitter` takes an `ecs` input. It uses it for two of the five:

| Appendix C | `agnus_blitter.v` | Gated by `ecs`? |
|---|---|---|
| `BLTCON0L` $05A new — control 0, lower 8 bits | line 259 | **yes** |
| `BLTSIZH` $05E new — H size and start | lines 500, 523, 557 | **yes** |
| `BLTSIZV` $05C new — V size, 15-bit | line 548 | **no** — and the comment on that line calls it ECS |
| `BLTxPTH` $050 chg — "high 5 bits, was 3 bits" | `agnus_blitter_adrgen.v:45`, `reg [20:16] bltpth [3:0]` | **no** — and now measured at the parent, see below |
| `DOFF`, BLTCON1 bit 7 — disable D output in line mode | nowhere | **not implemented** |

`bltcon1[7]` does not appear in `agnus_blitter.v`. The bits it reads are 15-12,
4, 3, 2, 1 and 0.

#### BLTxPTH, measured at the boundary that has the `ecs` port

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

The blitter measurement above sits next to NM-HRM-003 in this document, and the
two are worth reading together. NM-HRM-003 is the copper's version of exactly this finding: "OCS uses
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

Recorded rather than acted on. The copper half is NM-HRM-003 and is witnessed
more than once; the blitter half is NM-HRM-022. Whether one
decision closes both is the question this cross-reference exists to raise.

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

The pointer row is the sibling of NM-HRM-003, one appendix over: the same register
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

Read out of interest rather than necessity — these three are not
contested, so an emulator cannot cancel them. The result is worth knowing anyway:

**On all three, WinUAE and vAmiga do what NanoMig does and not what the manual
says.**

| Finding | Manual | Both emulators, and NanoMig |
|---|---|---|
| [NM-HRM-003](#nm-hrm-003--ocs-copper-address-uses-five-high-bits-instead-of-three) | OCS keeps three high pointer bits | neither emulator narrows the pointer by chipset at all |
| [NM-HRM-004](#nm-hrm-004--ecs-danger-bit-admits-too-small-a-range) | ECS with CDANG clear reaches $03E–$07E | CDANG clear blocks below $80 in both, chipset-independent |
| [NM-HRM-005](#nm-hrm-005--wait-and-skip-take-longer-than-the-manual-states) | MOVE 2, SKIP 4, satisfied WAIT 6 | 4 / 8 / 8 in vAmiga, the same three numbers NanoMig measures |

The citations are in [`third-witness.md`](third-witness.md).

This does not close any of them, and none of the assertions was touched. It
changes the question: not "is the RTL wrong" but "is the manual wrong about the
hardware". Two independent implementations agreeing against a manual sentence is
strong evidence, but choosing what NanoMig should follow — the manual it is
tested against, or the hardware it emulates — is a decision, not a measurement.

NM-HRM-004 is the sharpest case. Appendix C read literally says that *clearing* the
danger bit grants the range that *setting* it grants, which would leave the bit
with nothing to do.

## How a finding is meant to end

A red test is closed by a decision about the RTL, never by relaxing the
assertion. If a deviation turns out to be intentional, the assertion stays and
the reason is written down beside it — a test whose expectation was quietly moved
to match the implementation stops being evidence about anything.


### NM-HRM-006 — DDFSTRT/DDFSTOP bit H3 is gated by the chipset, not by resolution

**Package:** `hrm/ch3-ddf-fetch-window`, assertion
`DDFSTRT-H3-BIT-RESPECTED-ONLY-IN-HIRES-MODE`. **Red.**

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| `custom.cpp:1210` `ddf_mask = ecs_agnus ? 0xfe : 0xfc`, applied at `:3663`/`:3674` | respects H3 in lores too | `agnus/register.cpp:293`, `:306` `value & (ecsAndHigher() ? 0xfe : 0xfc)` | `agnus.rs:490` masks by revision; `:498` takes `bplcon0` and discards it |

**All four gate the DDF granularity on the chipset. None gates it on resolution** — Copperline's function takes the resolution register as a parameter and throws it away with `let _ = bplcon0;`. Page 91's *"only respected in Hires Mode"* is the outlier.

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

### NM-HRM-007 — the hires fetch width cannot reach the manual's maximum of 49 words

**Package:** `hrm/ch3-ddf-fetch-window`, assertion
`MAX-WORDS-49-HIRES-AT-HW-LIMITS`. **Red.**

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| 50 words at the limits | 50 words at the limits | — | `ddf_sequencer.rs:662` asserts `plane0 % 2 == 0` — two words per plane per 8-cck hires unit |

Copperline states the even word count as an invariant in its own test, which is NanoMig's structural argument arrived at independently. Denise not read for this.

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



### NM-HRM-008 — playfield-1 collision depends on playfield 2, by design, undocumented

**Package:** `hrm/ch3-collision-registers`, the four `BITMAP-PAIR` cases involving
playfield 1. **Red.**

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| PF1 gated by PF2 | PF1 gated by PF2 | `denise.cpp:478-486` sets the PF1 group only when `_doublePlayfield` | `bitplane.rs:6797` `odd_match_raw && (dual_playfield || even_match)` |

**All four gate playfield 1 on playfield 2 in single-playfield mode** — NanoMig's expression, term for term, and Copperline has a test named for it (`collision_pixel_single_playfield_odd_match_requires_even_match`). Appendix A's table of independent bits is incomplete.

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

### NM-HRM-009 — CLXDAT bit 15 reads as 1, not 0

> **WITHDRAWN.**
>
> Page 273 lists bit 15 as `not used` under the heading **COLLISIONS REGISTERED** -- it says no collision is registered there. It does not say the bit reads zero. NanoMig wires it high, which contradicts nothing printed. The expected zero was invented by the assertion, so the red measured the oracle, not the RTL.
>
> **The correction settles it.** The assertion now requires bit 15 to hold the *same* value before and after maximal collision stimulus, without claiming which value — which is what the page supports. It passes; 31/5 became 32/4. The implementation was right all along.

**Package:** `hrm/ch3-collision-registers`, assertion `BIT15-UNUSED`. **Red.**

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| `custom.cpp:4016` `clxdat | 0x8000` | `DeniseRegs.cpp:319` `clxdat | 0x8000` | `denise.cpp:313` `clxDat | 0x8000` | `denise.rs:538` `take(&mut self.clxdat) | 0x8000` |

**All four set bit 15 on every CLXDAT read.** The withdrawal is unanimous.

```verilog
assign data_out = reg_address_in[8:1]==CLXDAT[8:1] ? {1'b1,clxdat[14:0]} : 16'd0;
```

Bit 15 is hard-wired high on every successful read and can never be 0.

*For the specialists:* the question is what "not used" means in this table — reads
as zero, or undefined. Every other assertion in the package masks the bit out with
`& 0x7FFF`, which scopes the measurement without weakening any oracle.


### NM-HRM-010 — the horizontal line is a whole number of colour clocks, not 227.5

> **SOURCE OBSERVATION, not test-backed.**
>
> The manual's 227.5 CCKs and the RTL's own admission that half-clocks are unsupported are both real. The test is not evidence for it: `want_227p5_in_ticks = 455` is `227.5 CCK * 2 hpos-ticks/CCK`, and that factor 2 is the RTL's internal representation, not the manual's. The expected value therefore comes from the implementation -- the same circularity as the `kHiresPixelTicks = 2` defect caught during the build, found a second time.
>
> **Resolved by declining it.** No way was found to state the 227.5-colour-clock disagreement without an RTL-supplied conversion: the manual never names `htotal_out`, and the factor 2 lives in `agnus_beamcounter.v:206`'s write-doubling. The claim is now a recorded decline rather than an assertion — the honest outcome, and the one the correction brief explicitly permitted. The neighbouring `HTOTAL-H8-H1-BIT-MAPPING` was rebuilt as a pure relation instead — each bit's reading exactly double the one below, which is what page 304's `h8..h1` Use-row states — and it **passes**, with no unit conversion anywhere in it.

**Package:** `hrm/ch3-beam-counters-and-blanking`, assertion
`HTOTAL-FIXED-NOT-FRACTIONAL-227POINT5`, both PAL and NTSC. **Red.**

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| — | — | `agnus.cpp:1030-1032` — NTSC `227.5 * 263.0` with long/short lines toggling, PAL `227.0 * 313.0` | an integer `COLORCLOCKS_PER_LINE` with a separate `NTSC_LONG_COLORCLOCKS_PER_LINE` |

Both model the half clock as **alternating whole lines**, not as a fractional one — which is what the observation says the manual's 227.5 must mean.

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

### NM-HRM-011 — PAL vertical blanking is 26 lines, the manual says 29

**Package:** `hrm/ch3-beam-counters-and-blanking`, assertion
`VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL`. **Red.** Its NTSC twin passes and is
the control.

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| 26 lines | 26 lines | `agnus/agnus.cpp:448` — fixed blank clears at `vPos == (ntsc ? 19 : 24)` | programmable blank only (VBSTRT/VBSTOP under VARVBEN); no fixed PAL stop to compare |

Denise's PAL-minus-NTSC gap is **five lines, like NanoMig's**; the manual's is eight. Copperline models a programmable blank, which is a different quantity from Table 3-13's fixed figure and is not comparable without running it.

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


### NM-HRM-012 — SPRxCTL bit 0 (SH0) is honoured at every resolution, not only SuperHires

> **WITHDRAWN.**
>
> Page 303 reads `00  SH0  Start horiz. value Low bit 140 ns increment`. SH0 is the 140-ns low bit; the SuperHires positions are bits 4 (70 ns) and 3 (35 ns). The assertion imported a SuperHires-only restriction the manual does not state.
>
> **The correction settles it.** The assertion now requires only what the row states — toggling SPRxCTL bit 0 moves the start by exactly one raw unit, with no mode gate. It is **green**. The implementation was right; the oracle was not.

**Package:** `hrm/ch3-sprite-registers`, assertion
`SPRXCTL-BIT0-SH0-ONLY-IN-SUPERHIRES`. **Red.** Nine claim ids.

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| `custom.cpp:1203` keeps `0x01` in every chipset | — | `denise.cpp:298` `spr.x |= value & 1;` — unconditional | `sprite.rs:1854` `((pos & 0x00FF) << 1) | (ctl & 0x0001)` — unconditional |

**Three of four take SH0 at every resolution**, as NanoMig does. WinUAE's `sprite_sprctlmask` is computed at `:1199-1203` and **read nowhere in this checkout**, so it names an intent, not a measured behaviour.

```verilog
{attach,hstart[0]} <= {data_in[7],data_in[0]};
load <= armed && (hpos[7:0] == hstart[7:0]) && (fmode[15] || (hpos[8] == hstart[8]));
```

`spres` appears nowhere in the comparator — its only use in the module is the
`case` at line 161, which sets how fast the already-loaded shift register drains.
So the sprite's start position responds to SH0 at all four `spres` encodings.

*For the specialists:* the same shape as NM-HRM-006, where DDFSTRT's H3 is gated by the
chipset strap rather than by resolution. Both are "a low-order position bit the
manual scopes to one mode and the RTL honours always". Worth deciding together.

### NM-HRM-013 — SPRxCTL bits 3 and 4 are read nowhere in the module

> **WITHDRAWN.**
>
> Page 303 reads `03  SHSH0  Start horizontal (SHR mode) 35ns (unimplemented)` -- the manual itself calls bit 3 unimplemented. The note `bits 3 and 4 are in the ECS chips only` is a scope statement, not a promise that both alter output. Requiring bit 3 to be a live field contradicts the oracle.
>
> **Split the same day, and one half survives as NM-HRM-019.** Bit 3 is now declined, quoting the manual's own `(unimplemented)`. Bit 4 — which the manual *does* specify — was re-asserted on its own and is red for a reason that holds. The original finding was right that something is missing, and wrong about what.

**Package:** `hrm/ch3-sprite-registers`, assertion
`SPRXCTL-BITS-3-4-ARE-LIVE-FIELDS`. **Red.**

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| `custom.cpp:1199-1203` — bit 3 AGA only, bit 4 ECS and later | — | `denise.cpp:299` reads bit 4 into `shsh10`; bit 3 not read | `sprite.rs:1858` reads bit 4; bit 3 not read |

The split this finding was withdrawn into is the one the other implementations make: **bit 4 is real from ECS on, bit 3 only on AGA.** See [NM-HRM-019](#nm-hrm-019--sprxctl-bit-4-shsh1-is-not-implemented).

That is a stronger statement than "ECS only": they are unimplemented in every
chip mode this module offers, including with `aga` asserted.


### NM-HRM-014 — the manual applies its own ECS pointer extension inconsistently

> **SOURCE OBSERVATION, not test-backed (as this entry already stated).**
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

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| wide register | wide, narrowed on access by chipset | no mask on the high word at all | **narrows every DMA pointer by Agnus revision** — `agnus.rs:113` |

One of four extends the pointers by chipset everywhere. The manual's inconsistency is about which register entries carry the note, and no implementation settles a question about the manual's own text.

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


### NM-HRM-015 — BRDNTRAN is declared and never connected

**Package:** `hrm/ch3-horizontal-display-window`, assertion
`D-BRDNTRAN-HAS-NO-EFFECT`. **Red.** Pages 296, 297, 307.

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| no border effect | no border effect | name not present anywhere in its Amiga core | **implements it**, with its own test |

One of four treats BRDNTRAN as real border behaviour.

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


### NM-HRM-016 — SuperHires sprite colour encoding is not implemented

> **HOLDS, with the count corrected.**
>
> The finding stands: fifteen live registers and the general mode comparison all measure identical colour sets at both `shres` settings, using deliberately asymmetric slot-A/slot-B values so a rearrangement would be visible. But **`REG-0x10` is an unconditional `expect(false)`**, so it is a declaration, not a measurement: the evidential count is sixteen reds, not seventeen. The per-register assertions also test only that *some* mode difference exists, not the exact symbol-to-bit mapping their claim ids cite.

**Package:** `hrm/ch3-shres-sprite-colour-encoding`. **17 red, 2 green.**
Pages 301, 302. The largest single red result in chapter 3.

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| reinterprets, ECS Denise only | reinterprets, ECS Denise only | `denise.cpp:200` `colors[pos] = value;` — one table, no mode | `tests.rs:7563` — *"each 35 ns half resolves palette[1], never a pair-encoded entry"* |

**Two against two**, and it is the second finding after NM-HRM-002 where the four split evenly. Denise stores one colour per register in every mode; Copperline states the absence of pair encoding deliberately, in a test's own words.

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
same character as NM-HRM-015 — not a wrong behaviour but an unbuilt one. Whether
SuperHires sprite colour is worth implementing is a scope question for NanoMig,
not a defect report.


### NM-HRM-017 — SuperHires bitplane colour encoding is not implemented

> **SOURCE OBSERVATION, then RESTORED as test-backed once the witness was fixed.** The validator's objection was correct and the
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
Pages 297, 301. **Same root cause as NM-HRM-016, reached down a different path.**

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| reinterprets, ECS Denise only | reinterprets, ECS Denise only | `denise.cpp:200` `colors[pos] = value;` — one table, no mode | `tests.rs:7563` — *"each 35 ns half resolves palette[1], never a pair-encoded entry"* |

**Two against two**, the same split as NM-HRM-016 and from the same reading: neither Denise nor Copperline reinterprets a colour register by resolution.

Page 301 says SuperHires reinterprets the sixteen lower colour registers: the
stored 12-bit value is read as two-bit-per-channel data rather than as an
ordinary RGB triple. NanoMig reads every register identically in every mode.

**Verified in the RTL by the lead.** `denise_colortable.v` has no `shres` port —
the same absence that produces NM-HRM-016. Every occurrence of `shres` in the bitplane
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

**Why NM-HRM-016 and NM-HRM-017 are recorded separately.** They share a cause but not a
measurement: NM-HRM-016 drives the sprite colour path, NM-HRM-017 the bitplane colour path,
and neither result depends on the other's fixture. One absent port, demonstrated
twice from opposite directions, is a firmer result than either package alone.

*For the specialists:* together NM-HRM-016 and NM-HRM-017 say that ECS SuperHires colour is
unimplemented in NanoMig as a whole, not in one corner of it. Whether that is
worth building is a scope question for the project.


### NM-HRM-018 — ECS BPLCON0 bit 4 (SPRHWRM) has no effect

> **WITHDRAWN as a conformance finding.**
>
> Page 300's row reads, in full, `04       SPRHWRM` -- a mnemonic with no description column, unlike `06  SHRES  SuperHires 35ns pixel enable bit` three rows above. The manual gives bit 4 a name and no behaviour, so there is no printed behaviour for NanoMig to diverge from. The fixture also drives bitplanes only and could not observe a sprite-horizontal function in any case. What remains true is a source observation: `denise.v:181` gives the bit its AGA meaning (`bplcon0[4] & aga`) and no ECS meaning. Recorded by the lead earlier the same day; the overreach was the lead's.
>
> **Corrected the same day.** Both claims are now declined rather than asserted, with the reason printed: the manual states no behaviour for this bit, and the fixture drives bitplanes while the mnemonic names a sprite function. Neither is asserted true nor false — the source simply does not define anything to test. 225/2 became 225/0.

**Package:** `hrm/ch3-bplcon0-display-modes` (225 green, 2 red). Page 300.

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| — | — | `setBplCon0` (`denise.cpp:318-350`) never reads bit 4 | `denise.rs:345` uses bit 4 as AGA's BPU3, inside `if aga` |

**Neither gives BPLCON0 bit 4 an effect on ECS.** Copperline gives it one on AGA, where it selects eight bitplanes — which is a different register function, not the SPRHWRM the ECS appendix names.

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


### NM-HRM-019 — SPRxCTL bit 4 (SHSH1) is not implemented

**Package:** `hrm/ch3-sprite-registers`, assertion
`SPRXCTL-BIT4-SHSH1-70NS-SUPERHIRES-LIVE`. **Red.** Page 303.

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| implements bit 4 | does not | `denise.cpp:299` reads bit 4, compared only when `_shres` | `sprite.rs:1857` — `ctl & 0x0010` |

Three of four implement it, and Denise scopes it exactly as page 303's Warning
states.

**This is the half of the withdrawn NM-HRM-013 that survives**, re-established
on a claim the manual actually makes. NM-HRM-013 asserted that SPRxCTL bits 3 *and* 4 are
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
with NM-HRM-016 and NM-HRM-017 — the ECS SuperHires feature set is broadly absent rather than
wrong in detail.


### NM-HRM-024 — the audio DMA pointer takes five high bits in both chipsets

| | |
|---|---|
| Manual | Chapter 5 p.139 and p.145: the location registers "contain the high three bits". Appendix A p.266: "(high 3 bits, 5 bits if ECS)" and "18 bit starting address". Appendix C's ECS register map, printed p.298: `0A0 AUDxLCH chg W A Audio channel x location(high 5 bits was 3 bits)` |
| Measured | A walking one through all five bits of `AUDxLCH` lands at `address_out` bits 16–20 on every one of the four channels; all-ones gives word address `0xFFFFF`. Bit 3 alone gives `0x40000`, bit 4 alone `0x80000` — neither is zero |
| RTL | `agnus_audiodma.v:82`, `audlch[audlcsel] <= data_in[4:0]`. The module has **no `ecs` port, no chipset input and no mention of the chipset in its 157 lines** |
| Test | `hrm/ch5-audio-location-pointer`, `LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD` (red) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| no chipset narrowing | no chipset narrowing | no mask on the high word at all | **narrows every DMA pointer by Agnus revision** — `bus.rs:4990` applies the mask to Agnus, Denise, the blitter, Paula and the floppy alike |

The same question as NM-HRM-003 and NM-HRM-022, in a third engine.

**This is the third engine, and that is what makes it worth reading.** Copper
(`agnus_copper.v:144`), blitter (`BLTxPTH`) and audio all build their pointer as
`data_in[4:0]` for the high bits and `data_in[15:1]` for the low, unconditionally.
Separately they read as three omissions. Together they are one decision not to
model the OCS pointer narrowing anywhere in Agnus — and audio is the case where
the module has nothing to gate on even in principle.

**The assertion's wording matters and was chosen deliberately.** It asserts that
the OCS three-bit reading does **not** hold, not that "AUDxLCH is five bits".
NanoMig implements the ECS width unconditionally; the OCS alternative cannot be
selected for. Its control is `LCH-IMPLEMENTS-ECS-WIDTH-FIVE-BITS`, green, on the
identical walking-one methodology — so the red is the claim failing, not a dark
fixture.

*For the specialists:* if the intent is an ECS-only core, the manual's OCS
sentences are simply out of scope and the three findings are one scope decision.
If not, three engines need the same narrowing.


### NM-HRM-025 — volume 64 and volume 63 are the same gain on the multiply path

| | |
|---|---|
| Manual | Table 5-9, printed p.163, gives volume 63 as −0.1 dB and volume 64 as 0.0 dB. Appendix A p.266 describes "65 linear volume levels" with bit 6 forcing maximum |
| Measured | `ldatasum = 4032` for volumes 63, 64, 65, 96 **and** 127. On the PWM output of the same design, open-tick counts are 0, 4, 128, 252, 256 — exactly 4× each written value, all 65 distinct |
| RTL | `paula_audio_mixer.v:25-31` wires the helper's six-bit input as `{(vol[6] \| vol[5]), (vol[6] \| vol[4]), … (vol[6] \| vol[0])}`, so bit 6 sets every bit and the helper sees **63**. The PWM path keeps `audvol` seven bits (`paula_audio_channel.v:103`) and compares across widths, `volgate = (volcnt < audvol)` (`:149`) |
| Test | `hrm/ch5-audio-volume-scale`, `MULTIPLY-PATH-COLLAPSES-VOLUME-64-ONTO-63` and `TABLE-5-9-VOLUME-TO-DECIBEL vol0=64` (red); `hrm/ch5-audio-register-map`, `VOLUME-64-VERSUS-63-ON-THE-MULTIPLY-PATH` (red) with `VOLUME-65-LEVELS` green on the PWM output |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| `audio.cpp:1541-1543` — `v &= 127; if (v > 64) v = 64;` under the comment "7 bit register in Paula" | `StateMachineRegs.cpp:38` — `std::min(value & 0x7F, 64)`, with "All values greater than 64 are treated as 64 (max volume)" in the code | `paula/audio.cpp:153` — `if (value & 0x40) value = 0x40;` | `chipset/paula.rs:228` — `((word & 0x007F) as u8).min(64)` |

**All four clamp at 64 and keep it distinct from 63.** Two of them say why in
their own comments. NanoMig's multiply path is alone in making the two equal.

**NanoMig implements volume twice and the two implementations disagree.** This is
the finding, and it is why every assertion touching it names the port it
measured. Denise's line is worth putting next to NanoMig's: both implement
appendix A's "bit 6 forces maximum", but Denise forces the **value** to 64 while
`paula_audio_mixer` ORs bit 6 down into the low six bits and arrives at 63.

**What holds it up.** `MULTIPLY-PATH-DISTINGUISHES-ADJACENT-VOLUMES` (62 against
63) is green on the same fixture and the same measurement basis, so the collapse
is a property of 64 and not a dead sweep. The collapse assertion's own mutation
is recorded `impossible` — the OR is unconditional and no port gates it — and
that adjacency control is what stands in its place.

*For the specialists:* the whole of Table 5-9 runs +0.10 to +0.18 dB high on the
multiply path and reproduces to the printed decimal on the PWM path. Which path
reaches the DAC in a real build decides whether this is audible or bookkeeping.


### NM-HRM-026 — the audio state codes are not the ones Figure 5-8 prints

> **SOURCE OBSERVATION, not a conformance failure.** An independent validator
> objected that `audio_state` is not a port of `paula_audio_channel` and is
> reached only through Verilator's `--public-flat-rw`; a behavioural
> implementation is free to re-encode an internal state machine unless
> state-identical reconstruction is an explicit project requirement, and the
> test class does not state that contract. The objection is correct and the
> entry is reclassified.
>
> The measurement stands and is worth keeping: the codes differ, the actions,
> topology, sample order and timing do not, and all four independent
> implementations use the figure's own numbering. What changes is the claim made
> about it. This is a difference between NanoMig and the printed figure that
> nothing outside the chip can observe — so it is a note for whoever reads
> Figure 5-8 as a specification, not a defect at the pins.
>
> The same validator found that **four of this package's greens are
> force/readback tautologies**: `run_encoding()` writes `audio_state` directly
> and reads it back without a clock, which proves the generated storage accepts
> eight 3-bit patterns and not that eight states are reachable. Those four
> greens are not evidence and the package needs them rebuilt.


| | |
|---|---|
| Manual | Figure 5-8, printed p.166, labels its states `000`, `001`, `101`, `010`, `011`, and names `sq2,1,0` as "The name of the state flip-flops, MSB to LSB" — so the printed patterns are bit values, not just node names |
| Measured | An 80 000-tick sweep reaches `000`, `001`, `010`, `011`, `110` and **never** `100`, `101` or `111`. "In the 011 state the lower byte is output" measures `sample = 0x0` there; the `010→011→010` loop measures its destination as `6` |
| RTL | `paula_audio_channel.v:242-246` — `AUDIO_STATE_2 = 3'b011` where the figure has `101`, and `AUDIO_STATE_4 = 3'b110` where the figure has `011` |
| Test | `hrm/ch5-audio-channel-state`, six assertions including `5-p164.c9` and `5-p164.c10` (red) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| `audio.cpp` states 0, 1, 2, 3 | `StateMachine.cpp` — `0b000`, `0b001`, `0b010`, `0b101` | `paula/audio.cpp` — states 0, 1, 2, 3 and **5** | `chipset/paula.rs:107-111` names them: `AUD_IDLE 0b000`, `AUD_DMA_FIRST 0b001`, `AUD_DMA_SECOND 0b101`, `AUD_OUT_HI 0b010`, `AUD_OUT_LO 0b011` |

**All four use the figure's own numbering, including `101`.** Copperline writes
the five constants out with the manual's bit patterns and the manual's meanings.
That is what makes this more than an internal-encoding quibble: four independent
implementations treat the printed patterns as the specification.

**The behaviour is right and the encoding is not.** Every affected assertion is
otherwise green — correct actions, correct timing — and fails only on the bit
pattern. That distinction is the finding.

*For the specialists:* nothing outside the chip can observe `sq2,1,0`, so this
costs nothing at the pins. It matters if the figure is ever used as a
specification for a rewrite, and it is the only place chapter 5 prints a state
encoding at all.


### NM-HRM-027 — in direct mode the interrupt is raised before a word is output

| | |
|---|---|
| Manual | Printed p.157: in direct (non-DMA) audio output "the interrupt occurs after one data word has been output" |
| Measured | `AUDxIR` is set at the `AUDxDAT` write itself, with the state still `000` and `sample` still at its pre-write value |
| RTL | `paula_audio_channel.v:283-285` — the branch leaving the idle state for `AUDIO_STATE_3` sets `AUDxIR = 1'b1` at the write |
| Test | `hrm/ch5-audio-direct-output`, assertion C on p.157 (red) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| — | — | — | — |

Nobody has read any implementation for this finding.

**This sits inside a four-way conflict in the manual itself.** The audio
interrupt's moment is stated four different ways: printed 147 (after the location
and length are copied to the back-up registers), printed 157 (after a data
address has been read), printed 164 (as the last word starts output), and
appendix A printed 265 (when words-output equals length **and** the buffered word
has been used). The classification carries all four; the assertion tests page
157's, because page 157 is the direct-mode page and this is the direct-mode case.

**What holds it up.** Assertion H, "immediate transfer to the main loop" from the
same page and the same trace event, is green and is the control.

*For the specialists:* read this together with the four-way conflict before
deciding anything. A manual that gives four answers may not be settled by
choosing one of them.


### NM-HRM-028 — ADKCON's audio attach bits are stored and reach nothing

| | |
|---|---|
| Manual | Printed pp.149–151 describe modulating one channel with another: with an attach bit set the modulating channel's own output is silenced and its words are used as period or volume data for the next channel. Appendix A p.265 gives ADKCON bits 7–0 as ATPER3–0 and ATVOL3–0 |
| Measured | With **every** attach bit set, a channel armed for a peaked output still peaks `ldata`/`rdata` at 8001 — sample 127 × effective volume 63, the manual's own arithmetic for an unmodulated channel |
| RTL | `paula.v` instantiates `paula_audio ad1 (…)` with **no ADKCON port at all**. `adkcon[14:0]` is stored (`:186-192`) and read back through `adkconr` (`:198`); only bits 11 (`uartbrk`, `:211`) and 10 (`wordsync`, `:269`) reach anything, both disk and UART. `paula_audio_channel.v`'s first line: *"This module handles a single amiga audio channel. attached modes are not supported"* |
| Test | `hrm/ch5-audio-dmacon-adkcon`, `ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS` (red) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| `audio.cpp` — `audio_update_adkmasks()` builds a per-channel `adk_mask` from `adkcon \| (adkcon >> 4)` | `StateMachine.cpp:120,130` — `AUDxAV()` and `AUDxAP()`, with an attach-period path | `paula/audio.cpp:79-81` — `audav = adkcon & (0x01 << nr)`, `audap = adkcon & (0x10 << nr)`, `napnav` | `chipset/paula.rs:1190` — `channel_attached_as_modulator()`, documented as "AUDxAV: attach-volume (this channel's words modulate ch+1's volume)" |

**Four implementations implement attach and modulation. NanoMig alone does not**,
and its own source says so in its first line. This is the best-corroborated
finding chapter 5 produced.

**What is asserted and what is not.** Only the half whose stimulus and
consequence are both on real ports of `paula.v`: write ADKCON, observe that the
modulating channel is **not** silenced. The other half — a modulator word
reaching another channel's period or volume register — is not testable at any
boundary, because no audio module has an attach port, and it is recorded
`not-asserted` rather than asserted and failed.
`ADKCON-ATTACH-MEASUREMENT-BASIS-CONTROL` is green on the same basis.

*For the specialists:* this is a whole documented feature of chapter 5 that is
absent, of the same character as NM-HRM-015 and NM-HRM-021 — declared in a
register and never connected — but larger, and the RTL states the decision
openly rather than leaving it to be discovered.


### NM-HRM-029 — Figure 5-8's `dmasen` note does not hold outside its own exception

> **WITHDRAWN. The assertion is logically false.** Figure 5-8's NOTE says, after
> its exception, *"dmasen is true only when LENFIN=1"* — the implication
> `dmasen → LENFIN`. The assertion searched the trace for at least one later
> sample with `dmasen && lenfin` and went red when none was found:
>
> ```cpp
> if (s.lenfin && s.dmasen) dmasen_true_elsewhere_at_lenfin = true;
> ...
> CHECK("... the note's remaining claim requires 'yes'", dmasen_true_elsewhere_at_lenfin);
> ```
>
> **Absence of later `dmasen` pulses satisfies "only when". It does not violate
> it.** A necessary condition was tested as an existence requirement, so the red
> reports the opposite of what the figure says. Found by an independent
> validator and confirmed by reading the assertion.
>
> The entry is kept, as the register's withdrawn entries are, because the
> reasoning that withdrew it is the result — and because this one is a new shape
> in this register. The four earlier withdrawals read a permission or a silence
> as a requirement. This one read a **one-way implication as a biconditional**,
> which is the same error in the direction nobody was watching for.
>
> Its sibling assertion, the `AUDxDSR = AUDxDR · dmasen` equation, is a separate
> question and is also not a conformance failure: it compares the public `dmas`
> port against two internal signals reached through `--public-flat-rw`, so it
> asks whether NanoMig arranges two terms the way the figure draws them, not
> whether the chip behaves as the manual requires. `dmas <= dmasen | lenfin`
> computes the same output one register level further out.


| | |
|---|---|
| Manual | Figure 5-8's boxed NOTE, printed p.166: *"Except for this case, dmasen is true only when LENFIN=1. Also, AUDxDSR=AUDxDR · dmasen"*. The excepted case is the `000→001` transition, whose action bracket is `[lencntrld, AUDxDR, dmasen]` |
| Measured | `dmasen` never correlates with `lenfin` anywhere outside that stated exception. The note's own AND equation holds at the first restart and at neither later, `lenfin`-driven one |
| RTL | `dmasen` is raised only in the idle-state DMA branch (`paula_audio_channel.v:276`); the block-end restart comes from `:220`, `dmas <= dmasen \| lenfin`. So the implemented relation is `AUDxDSR = AUDxDR ∧ (dmasen ∨ lenfin)` — the `lenfin` term sits one register level further out than the figure puts it |
| Test | `hrm/ch5-audio-channel-state`, `F58.note.dmasen` and `F58.note.dsr` (red) |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| — | — | — | — |

`dmasen` and `AUDxDSR` are internal to Paula and appear at no pin, so an
implementation is free to place the term wherever it likes. Reading the four for
this finding would say what they chose, not what is required.

**This finding exists because the qualifier was honoured.** An earlier
restatement of the same note dropped "Except for this case" and stated the rule
unconditionally, which inverts what the figure says about the one transition the
note exists to exempt. The cross-read caught it and the restatement was
corrected before any test was written.

*For the specialists:* this is the weakest of chapter 5's six findings and is
listed as such. The figure describes one internal signal's relationship to
another; NanoMig computes the same output through a different arrangement of the
same two terms. Whether the printed arrangement is normative is a question about
how a state diagram should be read, not about what the chip does at its pins.


---
