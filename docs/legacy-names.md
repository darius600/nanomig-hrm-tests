# Names a reader meets, and what they resolve to

Findings once carried `FPGA-nnn` keys borrowed from the private workspace where
this work is planned. That workspace is not published, so no reader outside it
could resolve such a reference — a failing assertion printed a key that led
nowhere — and the borrowed numbers carried the gaps of a task sequence
(`050, 051, 060, 062, 069`) that has nothing to do with findings. They were
renumbered `NM-HRM-nnn`.

This table is here so an older commit message or artefact can still be read. It
is not needed to use the register and does not belong in it.

| now | was | | now | was | | now | was |
|---|---|---|---|---|---|---|---|
| NM-HRM-001 | FPGA-050 | | NM-HRM-009 | FPGA-091-NM-HRM-009 | | NM-HRM-016 | FPGA-091-NM-HRM-016 |
| NM-HRM-002 | FPGA-051 | | NM-HRM-010 | FPGA-091-NM-HRM-010 | | NM-HRM-017 | FPGA-091-NM-HRM-017 |
| NM-HRM-003 | FPGA-060 | | NM-HRM-011 | FPGA-091-NM-HRM-011 | | NM-HRM-018 | FPGA-091-NM-HRM-018 |
| NM-HRM-004 | FPGA-062 | | NM-HRM-012 | FPGA-091-NM-HRM-012 | | NM-HRM-019 | FPGA-091-NM-HRM-019 |
| NM-HRM-005 | FPGA-069 | | NM-HRM-013 | FPGA-091-NM-HRM-013 | | NM-HRM-020 | *newly issued* |
| NM-HRM-006 | FPGA-091-NM-HRM-006 | | NM-HRM-014 | FPGA-091-NM-HRM-014 | | NM-HRM-021 | *newly issued* |
| NM-HRM-007 | FPGA-091-NM-HRM-007 | | NM-HRM-015 | FPGA-091-NM-HRM-015 | | NM-HRM-022 | *newly issued* |
| NM-HRM-008 | FPGA-091-NM-HRM-008 | | NM-HRM-037 | *newly issued* | | NM-HRM-039 | *newly issued* |
| | | | NM-HRM-038 | *newly issued* | | NM-HRM-040 | *newly issued* |

## Package names an older comment may still cite

A first suite of 23 packages was written before the HRM pipeline existed. Their
oracle was a *frozen specification*: a short note describing what the module
already does, written by reading the Verilog. Three of them were later rewritten
as pipeline packages and kept their directory name; the other **20 were removed**,
because a description of the implementation cannot decide whether the
implementation is right. Every proposition they carried is asserted by a
pipeline package on the same module, with a page citation, except two numbers
that have no page behind them at all (`agnus_beamcounter`'s horizontal span of
0..453 and `agnus_refresh`'s slot positions 9/13/17/21).

Many live packages cite one of the removed names in a `//` comment, because
their harness pattern — clock phasing, register-write helper, reset discipline —
was read off one of them while it was being built. Those comments are accurate
about where the pattern came from; the directory is simply no longer there. The
sources are in the history.

| removed | what a live package covers instead |
|---|---|
| `agnus_audiodma` | `ch5-audio-location-pointer` |
| `agnus_beamcounter` | `ch3-field-geometry`, `ch3-beam-counters-and-blanking`, `ch3-register-map-agnus-beamcounter` |
| `agnus_bitplanedma` | `ch3-ddf-fetch-window` |
| `agnus_copper_move` | `agnus_copper_move_range`, `agnus_copper_move_src`, `agnus_copper_move_sweep` |
| `agnus_diskdma` | `ch8-disk-dma-data-channel`, `ch8-disk-dma-pointer` |
| `agnus_refresh` | `agnus_slots` (`REFRESH-FOUR-CYCLES-PER-LINE`, printed p.194) |
| `agnus_spritedma` | `ch4-sprite-dma-sequence`, `ch4-sprite-pointer-registers`, `ch4-sprite-vertical-position` |
| `agnus_top_register_bus` | `agnus_top_dmacon` |
| `denise` | `ch3-horizontal-display-window`, `ch3-register-map-denise`, `ch4-sprite-display-window` |
| `denise_bitplane_shifter` | `ch3-bplcon1-scroll-delay` |
| `denise_collision` | `ch3-collision-registers`, `ch4-sprite-collision`, `ch7-collision-model`, `ch7-collision-playfield-mode` |
| `denise_colortable` | `ch3-colour-register-format` |
| `denise_hamgenerator` | `ch3-ham-control-field` |
| `denise_playfields` | `ch3-dual-playfield` |
| `denise_spritepriority` | `ch3-sprite-playfield-priority`, `ch4-sprite-priority`, `ch7-playfield-priority-codes`, `ch7-sprite-priority-groups` |
| `denise_sprites` | `ch3-sprite-registers`, `ch4-attached-sprites`, `ch4-sprite-colour-registers` |
| `paula_audio_channel` | `ch5-audio-channel-registers`, `ch5-audio-channel-state`, `ch5-audio-direct-output`, `ch5-audio-period-and-length` |
| `paula_audio_volume` | `ch5-audio-volume-helper` |
| `paula_floppy` | `ch8-disk-interrupt-requests`, `ch8-disk-interrupt-path`, `ch8-dsksync-word-match`, `ch8-mfm-cell-encoding` |
| `paula_intcontroller` | `ch7-interrupt-register-semantics`, `ch7-interrupt-priority-encoder`, `ch5-audio-interrupt` |

## Four packages that were renamed, not removed

Three of the twenty-three kept their directory names when they were rewritten as
pipeline packages, and `agnus_slots` was built later without a prefix. All four
have been renamed to their unit, because a reader who has seen two `chN-`
packages expects the third to be one:

| was | is | unit |
|---|---|---|
| `agnus_top_dmacon` | `ch7-dmacon-register-bits` | chapter 7 / appendix A, 100 claim citations |
| `paula_uart` | `ch8-uart-frame-and-status` | chapter 8, 4 claim citations |
| `paula_audio_mixer` | `ch5-audio-stereo-routing` | chapter 5, carries NM-HRM-002 |
| `agnus_slots` | `ch6-refresh-dma-slots` | chapter 6, printed p.194 |

The Verilog modules keep their own names — `paula_uart.v`, `paula_audio_mixer.v`
and the rest are unchanged, and a `//` comment naming a *module* is not a stale
package reference.

Records in `hrm-testpipeline/work/` cite the old directory names 89 times. Those
are records and are not edited; this table resolves them.

One removal moved ledger content rather than dropping it. `paula_floppy` was the
only one of the twenty that printed claim ids: a `record()` declining three
chapter-7 uids (`opus:7-int.c54`, `opus:7-fig7-4.c6`, `opus:7-fig7-4.c39`) about
what DSKBLK and DSKSYNC *mean*. That decline now sits in
`ch8-disk-interrupt-requests`, beside the two local-half measurements it talks
about, and it says plainly that all three are still ungraded anywhere in the
suite.


## The vocabulary in a test source

A test prints identifiers that were never explained anywhere, and a reader who
cannot resolve one reads it as somebody else's process leaking into the file.
All of them are here.

| you meet | it is |
|---|---|
| `sol:7-int.c56`, `opus:8-appB.c31` | a **claim id**. The prefix names which of two models read the printed page: the pipeline has every chapter read twice, independently, before either reading is merged. The rest is that reader's own numbering — chapter or appendix, section, claim. An id names one reader's reading of one printed sentence, so the same sentence usually has two ids. |
| `FPGA-nnn` | a task number in the private lab that plans this work. It is **not** a finding key and resolves to nothing public. Findings are `NM-HRM-nnn`; the mapping is in the table above. |
| `work/ch8-s2-build/<package>.jsonl` | the package's claim file, in the sibling repository: [`hrm-testpipeline/work/`](https://github.com/darius600/hrm-testpipeline/tree/main/work). Every id a package cites is a row there. |
| a `work/…` path marked *private lab workspace* | a planning document that is not published. Five such citations exist, all in chapter-2 packages; the claim ids beside them are the public record of the same work. |
| `BOUNDARIES.md`, `CANDIDATES.md` | working files of a single build, in that build's directory under `work/`. |
| "the builder", "the lead", "the validator" | roles in the pipeline, not people: the model that wrote a package, the model that assigned the work, and the independent model that checked it. `hrm-testpipeline/docs/model-collaboration-guide.md` defines them. |

**Every package that prints claim ids says so in its own header**, in a
`Claim ids:` field beside `Test purpose`, `Test class`, `Primary oracle` and
`Non-goals` — 153 of the 208 packages. The other 55 grade propositions that
carry no id.
