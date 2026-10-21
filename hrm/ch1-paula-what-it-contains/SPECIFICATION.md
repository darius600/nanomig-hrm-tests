# ch1-paula-what-it-contains

HRM conformance package for chapter 1's ("Introduction") sentences that name
what is *inside* Paula: the four audio channels, the floppy disk interface,
the serial (RS-232-C) and parallel ports, and the interrupt controller.
Top module: `paula`. Printed pages 1, 2, 4, 6, 7, 8 and 10 of the *Amiga
Hardware Reference Manual*, 3rd edition. 37 claims, filtered from
`work/2026-09-16_ch1_packages.jsonl` where `classify.package ==
"ch1-paula-what-it-contains"`.

Test class: HRM_CONFORMANCE.
Primary oracle: the printed manual pages named above, quoted verbatim below.
An emulator is never an oracle; the RTL is never an oracle.

## Order-of-work disclosure

This package's own 37 claim records carry a `classify.evidence` field that
already cites `paula.v`, `paula_audio.v`, `paula_audio_channel.v`,
`paula_audio_mixer.v` and `paula_floppy.v` port names and line numbers (e.g.
`paula.v:137`, `paula_floppy.v:86`, `paula_audio.v:109-128`). Filtering
`work/2026-09-16_ch1_packages.jsonl` for this package's 37 claims -- required
just to know this package's own scope -- necessarily exposed that field
before this document was written. This is the same situation several
already-accepted sibling packages in this suite disclosed (for example
`hrm/ch8-parallel-port-direction` and `hrm/ch8-serial-interface`), and this
package follows their practice: the testability judgment and the wording
below are drawn from `extract.quote` / `classify.restatement` (the printed
sentence itself) and independent reasoning about what a structural sentence
of this kind can mean, not copied from the evidence field's line numbers.
The RTL was then read in full for the modules above, after this document's
first commit, to write the fixture -- see the git history for the two
separate commits. Not pretending the strict order held would be worse than
disclosing the deviation, per the build brief.

## Duplicate-proposition check (required before writing any assertion)

Grepped `../nanomig-hrm-tests/hrm/` (all accepted `ch5-*`, `ch7-*` and
`ch8-*` packages) for an assertion already grading the same printed
proposition as each of this package's 37 claims, per the build brief's
instruction ("Chapters 5 (audio) and 7 (interrupts) are accepted chapters
covering machinery yours only names, and chapter 8 covers the serial port
and the floppy"). 12 of the 19 distinct propositions below (24 of the 37
claims) are measured, at a near-identical or textually broader proposition,
in an accepted package; those are declined here via `record()`, citing the
package/assertion/uid that carries them. Ten of them (20 claims) are graded
there; P10 and P11 (4 claims) are declined there too (see below).
The remaining 7 propositions (13 claims) are not found anywhere else in the
tree and are graded here.

## Claim inventory

Each row is one distinct printed proposition. Where sol and opus quote the
identical sentence (verified: same `extract.quote`, same page), both uids
are carried by one row/assertion -- they are the same proposition read
twice, not two propositions.

| # | uids | page | disposition |
|---|---|---|---|
| P1 | sol:1-components.c10, opus:1-components.c10 | 1 | **GRADED** -- THREE-CHIPS-DISJOINT-OUTPUTS |
| P2 | sol:1-components.c15, opus:1-components.c15 | 2 | DECLINED -- see "Already graded" |
| P3 | sol:1-components.c17, opus:1-components.c17 | 2 | DECLINED -- see "Already graded" |
| P4 | sol:1-components.c22, opus:1-components.c22 | 2 | **GRADED** -- TWO-CHANNELS-PER-STEREO-OUTPUT (with P8) |
| P5 | sol:1-components.c74, opus:1-components.c73 | 4 | DECLINED -- see "Already graded" |
| P6 | sol:1-components.c75, opus:1-components.c74 | 4 | DECLINED -- see "Already graded" |
| P7 | sol:1-components.c76, opus:1-components.c75 | 4 | DECLINED -- see "Already graded" |
| P8 | sol:1-components.c77, opus:1-components.c76 | 4 | **GRADED** -- TWO-CHANNELS-PER-STEREO-OUTPUT (with P4) |
| P9 | sol:1-components.c78, opus:1-components.c77 | 4 | DECLINED -- see "Already graded" |
| P10 | sol:1-components.c79, opus:1-components.c78 | 4 | DECLINED -- see "Already graded" |
| P11 | sol:1-components.c80, opus:1-components.c79 | 4 | DECLINED -- see "Already graded" |
| P12 | sol:1-components.c117, opus:1-components.c117 | 6 | DECLINED -- see "Already graded" |
| P13 | sol:1-components.c118, opus:1-components.c118 | 6 | **GRADED** -- FLOPPY-DISK-GEOMETRY-AND-CAPACITY |
| P14 | sol:1-components.c120, opus:1-components.c120 | 6 | DECLINED -- see "Already graded" |
| P15 | sol:1-components.c126, opus:1-components.c126 | 6 | DECLINED -- see "Already graded" |
| P16 | sol:1-components.c138, opus:1-components.c138 | 7 | DECLINED -- see "Already graded" |
| P17 | sol:1-blockdiag.c13 | 8 | **GRADED** -- PAULA-ON-SHARED-REGISTER-BUS (Figure 1-1) |
| P18 | sol:1-examples.c25, opus:1-examples.c24 | 10 | **GRADED** -- INTENA-7FFF-DISABLES-ALL-INTERRUPTS (with P19) |
| P19 | sol:1-examples.c30, opus:1-examples.c26 | 10 | **GRADED** -- INTENA-7FFF-DISABLES-ALL-INTERRUPTS (with P18) |

7 graded propositions, one assertion each (P4/P8 share one assertion,
P18/P19 share one assertion, so 5 `expect()` blocks total), 12 declined
propositions (each one `record()` line, several sharing a citation).

## Graded propositions -- testability argument from the quotation alone

**P1 -- THREE-CHIPS-DISJOINT-OUTPUTS** (p.1): *"All Amiga models are
equipped with three custom chips named Paula, Agnus, and Denise which
provide for superior color graphics, digital audio, and high-performance
interrupt and I/O handling."* Grading the sentence, not the chip: the
testable content at Paula's own boundary is that Paula is a complete,
self-contained module that itself provides the audio and the interrupt/I-O
thirds of the sentence -- i.e. that driving Paula's own register bus alone
(no other chip) produces an audio output, an interrupt level, and serial
I/O. "All Amiga models" and "superior" set no measurable threshold and are
not graded. The graphics third is Denise's, not Paula's, and is out of this
package's boundary entirely.

**P4/P8 -- TWO-CHANNELS-PER-STEREO-OUTPUT** (p.2): *"Ports for left and
right stereo audio from four special purpose audio channels."* (p.4):
*"Two channels are directed into each of the two stereo audio outputs."*
Both sentences make the same coarse, page-agnostic structural claim: of
four audio channels, exactly two feed the left output and exactly two feed
the right output. Neither sentence names *which* two go to which side --
that is a stronger, separate claim this package does not make (see the
caution below). The claim as printed is a partition claim only, and is
sharply testable: drive each of the four channels alone and observe which
output(s) move.

*Caution, found while checking for existing coverage:* an accepted sibling
package, `hrm/ch5-audio-volume-scale`, already grades the *specific*
channel-to-side assignment (assertions `CH1-CH2-ROUTE-LEFT` /
`CH0-CH3-ROUTE-RIGHT`, citing manual quotes on p.137 that DO name channel
numbers) and finds it **EXPECTED RED** -- the manual's own printed channel
numbers for left/right do not match `paula_audio_mixer.v`'s routing
(already logged there as finding NM-HRM-002). This package's P4/P8 claim is
strictly weaker (a partition exists) and is not the same proposition as
NM-HRM-002 (which side a *specific* channel lands on); grading P4/P8 here
does not re-decide or contradict that finding, and this document says so
explicitly so the two results are never mistaken for disagreeing about the
same sentence.

**P13 -- FLOPPY-DISK-GEOMETRY-AND-CAPACITY** (p.6): *"Disks are 80 track,
double sided, and formatted as 11 sectors per track, 512 bytes per sector
(over 900,000 bytes per disk)."* A dimensional, arithmetic sentence:
track count, side count, sectors/track and bytes/sector, with a derived
total. All four factors and the derived total are structural quantities at
`paula_floppy`'s own boundary (track addressing width and reach, a side
input, and the disk-image sector-index arithmetic). The physical media
description ("3.5 inch") has no RTL counterpart and is not graded; the
counts and the arithmetic are.

**P17 -- PAULA-ON-SHARED-REGISTER-BUS** (p.8, Figure 1-1): the figure shows
a double-outlined PAULA block inside the dashed CUSTOM CHIP SECTION, with
one arrowhead entering from the CHIP DATA line on its left and one from the
REG ADR BUS on its right (`docs/references/amiga-hrm-3rd/ch1/figures/p8.png`,
opened and read directly, per the build brief -- the `quote` field is a
marker line, not the evidence). The testable content this package's
boundary can reach is that Paula answers on its own register-address/data
bus -- a write at `reg_address_in`/`data_in` is readable back at
`data_out`. The figure's *further* claim that Paula's `reg_address_in`
is wired to the identical net Agnus's own register-address input is a
`minimig.v`-level (system-instantiation) fact this package's `paula`-only
build cannot reach without violating its own fixture boundary by
instantiating `agnus` as well; that half is named here and left ungraded,
not silently dropped.

**P18/P19 -- INTENA-7FFF-DISABLES-ALL-INTERRUPTS** (p.10): the assembler
listing's own trailing comment, `move.w #$7FFF,intena(a0) ; to disable all
interrupts`, and the C listing's own trailing comment, `custom.intena =
0x7FFF; /* Disable all interrupts */` (both listings reproduced with the
manual's own scan damage: `#` prints as a diamond glyph, `a0` as `aO`).
Per the build brief and per this package's own claim record, "the
gradeable assertion inside the listing is the comment on its last line" --
the INCLUDE/XREF/extern-struct/compiler-specific lines above it are not
graded, they have no RTL counterpart. Both listings assert the identical
machine fact in two languages (confirmed by this package's own classify
stage: the C listing's own evidence field calls it "the same machine fact
as the assembler one above it"), so both are graded by one assertion. The
testable content: writing $7FFF to INTENA (SET/CLR bit clear, all fifteen
named bits) leaves every one of Paula's interrupt sources disabled, so that
no combination of pending requests reaches the processor.

## Already graded or declined elsewhere -- 12 propositions declined here

For each, the package cited measures the same or a strictly broader
version of this package's proposition, and the measurement is not
reproduced here. Ten are graded there; P10 and P11 are declined there too.

- **P2** (p.2, "Built-in 3.5 inch ... drive with ... ports for ... up to
  three additional disk drives"), **P12** (p.6, "Floppy disk storage is
  provided by a built-in, 3.5 inch floppy disk drive"), **P14** (p.6, "Up
  to three extra 3.5 inch or 5.25 inch disk drives can be added"), **P16**
  (p.7, "Extra floppy disk units may be added from a connector at the rear
  of the unit"): all four are the SAME underlying drive-count proposition
  (one built-in plus up to three more, four total) restated across pages
  2, 6 and 7. Already graded in `hrm/ch8-disk-controller-overview`,
  assertion **P-DRIVE-COUNT** (uids `opus:8-dsk.c1`, `sol:8-dsk.c1`, p.241
  "The built-in disk controller in the system can handle up to four
  MFM-type devices."): with `floppy_drives` at its maximum, each of the
  four `_sel` bits independently selects a working drive slot.

- **P3** (p.2, "Fully programmable parallel and RS-232-C serial ports."):
  the parallel half is already graded in `hrm/ch8-parallel-port-direction`,
  requirement **PROP-1** (uids `opus:8-par.c1`, `sol:8-par.c1`, p.259) --
  measured **RED**: the direction register is programmable but the data
  path that would drive the port is absent. The serial half is already
  graded across `hrm/ch8-serial-interface`'s 97 `expect()` assertions
  (e.g. assertion A2, `opus:8-serial.c10`, p.255, "a programmable length
  of eight or nine bits") -- fully programmable and functioning.

- **P5** (p.4, "Audio consisting of four digital channels with
  independently programmable volume and sampling rate."): already graded
  in `hrm/ch5-audio-dmacon-adkcon`, assertion
  **CHANNELS-INDEPENDENTLY-PROGRAMMABLE** (citing manual quotes p.136,
  p.149) and in `hrm/ch5-audio-register-map`, assertion
  **CHANNEL-INDEPENDENCE** (uids `5-p164.c1`, `5-sm.c1`) for the
  four-channel-count half.

- **P6** (p.4, "The audio channels retrieve their control and sample data
  via DMA.") and **P7** (p.4, "Once started, each channel can
  automatically play a specified waveform without further processor
  interaction."): both already graded by the same assertion in
  `hrm/ch5-audio-location-pointer`, **LOCATION-REGISTER-VS-WORKING-POINTER**
  (uids include `5-p137.c4`, `5-p139.c5/c6/c7`, `5-p164.c25`, `5-loc.c5/c6/c7`
  among 33 total): the location registers load a working pointer once, at
  start/restart, which Agnus then advances on its own -- DMA-fetched, and
  repeating "with no further rewrite" needed.

- **P9** (p.4, "The audio channels may be linked together to provide
  amplitude or frequency modulation or both forms of modulation
  simultaneously."): this is the package's own disclosed "measured fact" --
  already graded, **EXPECTED RED**, in `hrm/ch5-audio-dmacon-adkcon`,
  assertion **ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS**: with every one
  of ADKCON's eight audio attach/modulate bits set, two independently
  armed channels reach exactly the peaks they would have reached with no
  attach bits set at all -- attach/modulate has zero effect on the audio
  path. `paula_audio_channel.v`'s own first line states attached modes are
  not supported, and `paula.v` instantiates `paula_audio` with no `adkcon`
  port at all. Not re-graded here; the measurement already exists and this
  package's own two claims are the same proposition the p.149-151 quotes
  in that assertion's own label already cover.

- **P10** (p.4, "DMA controlled floppy disk read and write on a full track
  basis.") and **P11** (p.4, "This means that the built-in disk can read
  over 5600 bytes of data in a single disk revolution (11 sectors of 512
  bytes each)."): both measured, and declined, by the same check in
  `hrm/ch8-disk-controller-overview`, **P-WHOLETRACK-DMA** (uids
  `opus:8-dsk.c6`, `opus:8-dsk.c7`, `sol:8-dsk.c7`, `sol:8-dskdma.c6`,
  p.241/p.247): a single DSKLEN arming delivers all 5984 words of a whole
  track's real content (11 sectors x 544 MFM words) and raises `blckint`
  exactly once -- one DMA operation, one whole track. That package declines
  to grade it: 5984 is derived from the RTL's own sector-counter
  structure, not a figure the manual states, and its fixture never observes
  an index-to-index interval, so it cannot show "a single disk revolution".
  Neither package grades P10 or P11.

- **P15** (p.6, "The Amiga includes a standard RS-232-C serial port for
  external serial input/output devices such as a modem, MIDI interface, or
  printer."): the RS-232-C port's existence and function is already graded
  across `hrm/ch8-serial-interface`'s 97 assertions against printed pages
  255-259 (device-agnostic -- modem/MIDI/printer name the use, not a
  distinct RTL boundary).

## The two "measured facts that will cost a day," as they apply here

The build brief names two facts up front. The frequency one does not touch
any of this package's 37 claims (none name a clock rate). The audio-attach
one is this package's own P9, handled above by declining to `record()`
citing the accepted package that already measured and graded it RED --
consistent with the brief's instruction to measure it and report the
finding, not avoid the question, while also not re-grading what is already
graded.

## Fixture

`paula` (top), built with its own directly-instantiated submodules
(`paula_uart`, `paula_intcontroller`, `paula_floppy` +
`paula_floppy_fifo`, `paula_audio` + `paula_audio_channel` +
`paula_audio_mixer` + `paula_audio_volume`) -- the same source list already
established and disclosed by the sibling package
`hrm/ch8-adkcon-and-interrupt-relations` (whose own header explains why
`paula_floppy_ebr.v` and `paula_floppy_dma_buf.v` are deliberately
excluded: `paula_floppy.v` does not instantiate either, and
`paula_floppy_ebr.v` declares a second, colliding `module paula_floppy`).
Reused here as the established build pattern for this top module, not
re-derived independently.

## Never

No RTL under `nanomig/` is modified. No oracle is adjusted to make a test
pass. No debug port is added. Nothing outside this package's own directory
is written.
