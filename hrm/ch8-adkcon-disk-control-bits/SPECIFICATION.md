# ch8-adkcon-disk-control-bits — SPECIFICATION

The package grades the quotations in
`work/ch8-s2-build/ch8-adkcon-disk-control-bits.jsonl` (the thirteen claims
assigned to this package), and the two further WORDSYNC claims in the
extension at the end.

## Source claims (verbatim quotations)

| uid | page | quote | restatement |
|---|---|---|---|
| `opus:8-appA-adkcon.c4` | 265 | "ADKCON     09E      W       P      Audio, disk, control write" | ADKCON's function is audio, disk and control write. |
| `sol:8-appa-adk.c5` | 265 | "ADKCON     09E      W       P      Audio, disk, control write" | Writing ADKCON controls disk behavior. (twin of `opus:8-appA-adkcon.c4`) |
| `opus:8-appA-adkcon.c8` | 265 | "ADKCONR    010      R       P      Audio, disk, control read" | ADKCONR's function is audio, disk and control read. |
| `sol:8-appa-adkr.c5` | 265 | "ADKCONR    010      R       P      Audio, disk, control read" | Reading ADKCONR reports disk control state. (twin of `opus:8-appA-adkcon.c8`) |
| `opus:8-appA-adkcon.c11` | 265 | "            14-13  PRECOMP 1-0" | Bits 14-13 are named PRECOMP 1-0. |
| `sol:8-appa-adkbit.c3` | 265 | "            14-13  PRECOMP 1-0" | Bit 14 is PRECOMP1, the high bit of the field. (individual half of `opus:8-appA-adkcon.c11`) |
| `sol:8-appa-adkbit.c33` | 265 | "            14-13  PRECOMP 1-0" | Bit 13 is PRECOMP0, the low bit of the field. (individual half of `opus:8-appA-adkcon.c11`) |
| `opus:8-appA-adkcon.c20` | 265 | "            10     WORDSYNC       Enables disk read synchronizing on a word\nequal to DISK SYNC CODE, located in address (3F)*2." | Bit 10 is named WORDSYNC and enables disk read synchronizing on a word. |
| `sol:8-appa-adkbit.c11` | 265 | same WORDSYNC quote as above | WORDSYNC enables disk-read synchronization to a word equal to the disk sync code. (twin of `opus:8-appA-adkcon.c20`) |
| `opus:8-appB-adkconr.c5` | 289 | "ADKCONR      *010   R           P    Audio, disk control register read" | ADKCONR's function is the audio, disk control register read. |
| `sol:8-appb-adkconr.c6` | 289 | same appendix ADKCONR row | ADKCONR reads disk-control state. (disk half of `opus:8-appB-adkconr.c5`) |
| `opus:8-appB-adkcon.c5` | 290 | "    ADKCON         09E    W         P     Audio, disk, UART control" | ADKCON's function is audio, disk and UART control. |
| `sol:8-appb-adkcon.c5` | 290 | same appendix ADKCON row | ADKCON controls disk behavior. (disk half of `opus:8-appB-adkcon.c5`) |

All thirteen carry `disposition: new` and name this package
(`ch8-adkcon-disk-control-bits`, fixture `paula`). Each cross-read `ok`.

## What this package grades, and what it hands to siblings instead

The package purpose recorded on every one of these thirteen claims:
"ADKCON/ADKCONR's disk half: PRECOMP 1-0, WORDSYNC's name and position, and
the 'controls disk' half of every function-column row." Two adjacent
propositions are named explicitly as **not** this package's to grade, and
none of the thirteen quotations above touches either:

- **The audio half of the same function-row cells and the ADKCON bits 0-7
  attach/modulation claims.** `hrm/ch5-audio-dmacon-adkcon` already carries
  that red. None of the thirteen quotations mentions audio, channels, or
  bits 0-7, so there is nothing here to duplicate.
- **The UART third of the three-part appendix rows** (`opus:8-appB-adkcon.c5`
  and `sol:8-appb-adkcon.c5` each explicitly split their cell into
  audio/disk/UART thirds and name only the disk third as this package's).
  Bit 11 (UARTBRK) belongs to a package named
  `ADKCON-BIT11-UARTBRK` in that split note — not this one, and none of the
  thirteen quotations asserts anything about bit 11.

## C8: ADKCON bits that store and drive nothing, and where this package's claims sit

C8 is the finding that ADKCON has exactly two consumers in all of
`src/minimig-aga` — bit 11 (UARTBRK, out of scope here per above) and bit
10 (WORDSYNC) (paula.v:211, :269). Bits 0-14 all store and read back
individually, bit 15 never stores. **PRECOMP (14-13), MFMPREC (12), MSBSYNC
(9) and FAST (8) are storage with no consumer.**

None of the thirteen claims asserts an *effect* for PRECOMP, and none names
MFMPREC, MSBSYNC or FAST at all:

- `opus:8-appA-adkcon.c11`'s own `split` field says it outright: "The
  observable half is the field's position and width — both bits are
  individually written and read at `data_out`. The mnemonic PRECOMP 1-0 is a
  label, not a signal. **What the field DOES is graded separately on c12-c15
  and is not asserted.**" `sol:8-appa-adkbit.c3` and `.c33` carry the
  identical split, one bit each.
- The two generic function-row propositions (`ADKCON`/`ADKCONR` "Audio, disk,
  control") are graded here as "does writing/reading this register actually
  reach disk-side logic at all" — a claim the two real fan-outs make true
  regardless of which specific bit does the work — not as "does every
  disk-labelled bit have an effect."
- The one claim in this package that *does* assert a mechanism —
  `opus:8-appA-adkcon.c20`/`sol:8-appa-adkbit.c11`, WORDSYNC — names bit 10,
  which is the one bit C8 itself says is a real consumer, not one of the
  storage-only four.

**So every claim in this specific package is expected GREEN.** The red half
of C8 (PRECOMP/MFMPREC/MSBSYNC/FAST *doing* nothing) is real, but no claim
assigned to this package asserts it as a testable proposition — it is
surfaced as a `note()` (MEASURED, graded by nothing) rather than invented as
an `expect()` with no citation behind it. Grading an effect nobody claimed
would be the overreach this suite's rules exist to prevent.

## Fixture boundary: `paula`, not `paula_floppy`

ADKCON ($09E) and ADKCONR ($010) are decoded inside `paula` (paula.v:126-127,
the register-write block at :182-195, the read mux at :198), and
`paula_floppy` has no `adkcon` port at all — only a one-bit input named
`wordsync` crosses that boundary (paula.v:269).

`hrm/ch5-audio-dmacon-adkcon` also uses `paula` as its top module; its
Makefile shows which submodule files a `paula` build needs, and nothing is
cited from it as an oracle.

## Rig hazard

`paula_floppy_ebr.v` declares a second `module paula_floppy`, so the build
compiles `paula_floppy.v` + `paula_floppy_fifo.v` and leaves the EBR file
out entirely. `paula` instantiates `paula_floppy` internally, so this
applies to this package's build.

## Reduction to six testable propositions, plus fixture liveness

- **A1 — ADKCON-DISK-WRITE** (`opus:8-appA-adkcon.c4`, `sol:8-appa-adk.c5`,
  p.265; `opus:8-appB-adkcon.c5`, `sol:8-appb-adkcon.c5`, p.290). ADKCON's
  write function includes real control of disk-side behaviour — graded by
  showing a write that changes the disk-relevant bit produces an observable
  difference at `paula`'s boundary. **Expected green** (via the WORDSYNC
  fan-out).
- **A2 — ADKCONR-DISK-READ** (`opus:8-appA-adkcon.c8`, `sol:8-appa-adkr.c5`,
  p.265; `opus:8-appB-adkconr.c5`, `sol:8-appb-adkconr.c6`, p.289).
  ADKCONR's read function reports real disk-control state — graded by
  writing the disk-relevant bit and confirming ADKCONR reads it back.
  **Expected green.**
- **A3 — PRECOMP-FIELD-POSITION** (`opus:8-appA-adkcon.c11`, p.265). Bits
  14-13 are the named PRECOMP field — graded as position/width only (per the
  claim's own split): write a pattern to bits 14:13 together, confirm
  exactly those two bits read back at $010, nothing else disturbed.
  **Expected green.**
- **A4 — PRECOMP1-BIT14-STORES** (`sol:8-appa-adkbit.c3`, p.265). Bit 14
  individually stores and reads back. **Expected green.**
- **A5 — PRECOMP0-BIT13-STORES** (`sol:8-appa-adkbit.c33`, p.265). Bit 13
  individually stores and reads back. **Expected green.**
- **A6 — WORDSYNC-BIT10-EFFECT** (`opus:8-appA-adkcon.c20`,
  `sol:8-appa-adkbit.c11`, p.265). Bit 10, WORDSYNC, has a real effect
  reaching disk-side logic (unlike PRECOMP/MFMPREC/MSBSYNC/FAST) — the
  observable is the first word DSKDATR delivers (the test source's header
  gives the mechanism read from `paula_floppy.v`); the proposition graded is "toggling bit
  10 changes something reachable at `paula`'s boundary that toggling a
  storage-only bit does not," which is the load-bearing half of the page's
  "enables disk read synchronizing" sentence at this module boundary.
  **Expected green.**

Plus, ungraded by any citation but required by this suite's own rules:

- **LIVENESS (fixture liveness)** — the ADKCON/ADKCONR data path can deliver
  two distinct, recognisable words and read them back correctly before any
  of A1-A6 is trusted to have measured one. A `note()`, MEASURED and not
  graded: it carries no claim uid, so it is a fixture control, not
  conformance evidence.

All six citation-bearing propositions are expected **green**. No claim in
this package's thirteen is expected red — the deliberate non-effect scoping
of A3/A4/A5, and the real-consumer scoping of A1/A2/A6, is what keeps every
one of them inside what the RTL actually does. The red half of C8 is
reported via `note()` lines for MFMPREC (bit 12), MSBSYNC (bit 9) and FAST
(bit 8) — each measured as storing and reading back individually while
reaching no consumer (paula.v's instantiations, read in full) — since no
claim in the thirteen names those three bits and inventing an assertion for
a proposition nobody in this package's claim file made would be the
overreach this suite's process exists to prevent.

## Every claim accounted for

All thirteen claims map onto the six citation-bearing assertions above; none
is declined. (`opus:8-appA-adkcon.c11` is cited on A3 alone, not also
repeated on A4/A5, to avoid citing one uid on three separate assertions for
what its own text treats as one field-position proposition; `sol:8-appa-adkbit.c3`
and `.c33` carry the individual-bit half of the same fact as their own,
separately-authored claims.)

## Mutation plan

Each of A1-A7 gets its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what changed>`
record in the C++ source, `<label>` being the exact leading token of the
string literal passed to `expect()`. Kind for all seven: **stimulus**,
changing what the rig asks the module to do (which bit is written, or to
which address) while leaving the expected value alone, confirming the
targeted assertion alone goes red, then reverting. Since every assertion is
green at baseline, `stimulus red` is the shape for all seven.

## Extension: two further claims on WORDSYNC (bit 10)

| uid | page | quote | restatement |
|---|---|---|---|
| `sol:5-appA-p265.c19` | 265 | "            10     WORDSYNC       Enables disk read synchronizing on a word\n                              equal to DISK SYNC CODE, located in\n                              address (3F)*2." | ADKCON bit 10 enables disk-read synchronization to the disk sync code at address (3F)*2. |
| `opus:A-adkcon.b10` | 265 | "            10     WORDSYNC       Enables disk read synchronizing on a word\n                              equal to DISK SYNC CODE, located in\n                              address (3F)*2." | ADKCON bit 10 is WORDSYNC and enables disk-read synchronisation on the word held at chip address (3F)*2, i.e. $07E (DSKSYNC). |

Both quote the same appendix-A WORDSYNC sentence A6 already cites
(`opus:8-appA-adkcon.c20`/`sol:8-appa-adkbit.c11`, p.265) and restate the
same proposition: bit 10 is WORDSYNC and enables disk-read synchronisation
to the word at address (3F)*2. Neither carries a clause A6 does not already
establish — opus's "$07E (DSKSYNC)" is arithmetic on the same cited address
((3F)*2 = $07E), not an additional manual statement. On that basis both ids
are cited on A6's existing label rather than on a new assertion, and on A7,
which grades the sentence's other half — that the word WORDSYNC waits for is
the content of the register at "address (3F)*2" (see the test source).
