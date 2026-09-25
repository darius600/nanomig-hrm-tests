# Specification — `ch1-agnus-what-it-contains`

Package: `hrm/ch1-agnus-what-it-contains`
Fixture: `agnus` (top module, plus every submodule it instantiates)
Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered to
`classify.package == "ch1-agnus-what-it-contains"` — 67 claims, printed pages
1–6, 8, 9, 12–14.
Subject: what is inside Agnus's boundary — the Copper, the blitter, the DMA
control logic, the beam counters, and which memory Agnus can address — and
whether the printed sentence's structural proposition (a thing is inside
Agnus, or a signal crosses its boundary) holds. This package does **not**
grade whether the Copper's timing is cycle-accurate, whether the blitter's
minterm logic implements all 256 functions correctly, or whether a sprite
DMA channel's reuse sequencing is correct — those are chapter 2's, chapter
6's and chapter 4's questions respectively, and (where an existing accepted
package already grades the same printed proposition) this file declines
rather than re-grades.

## Order of work, disclosed rather than pretended away

The build brief requires this file to be written from the 67 claims'
`quote`/`restatement` fields alone, committed, and only then the RTL opened.
That order did not fully hold, for two reasons already present before this
build started, exactly as the brief itself warns can happen:

1. Every one of the 67 claims' own `classify.evidence` field already carries
   concrete `file:line` citations (`agnus.v:70`, `agnus_copper.v:58-75`,
   `agnus_blitter_minterm.v:7-13`, and so on) — reading the claims file to
   filter and group them by page/proposition necessarily exposed these.
2. The build brief itself requires, before any assertion is written,
   grepping `hrm/` for an existing assertion on the same printed proposition
   and declining onto a `record()` line if one is found. Doing that
   responsibly meant reading sibling packages' headers and (for close calls)
   their assertion labels — for example confirming that
   `hrm/ch4-sprite-dma-sequence`'s `A5-CHANNEL-REUSED-MULTIPLE-TIMES-
   WITHIN-ONE-FIELD` assertion already grades this chapter's sprite-reuse
   sentences, and that `hrm/ch3-beam-counters-and-blanking`'s
   `PAL-FIELD-312-NTSC-FIELD-262-LINES` assertion already grades the VTOTAL
   fact this chapter's NTSC/PAL display-geometry sentences depend on. That
   check cannot be done blind to file contents.

Despite having seen this, every proposition below is worded from the manual
quotations, and no expected value anywhere in the test source was set by
running the RTL and copying its answer — each is a value the printed
sentence itself commits to (a bit width, an address, a channel existing,
a signal crossing a boundary), decided before the fixture was run. Recorded
here and in the final report rather than glossed over, per the brief's own
instruction.

**What was written before the RTL was opened, and what was added after,
stated plainly rather than left to be inferred from the commit log:**

- **Before** (commit `6c293f5`, the first commit of this file, made before
  `test_agnus.cpp` existed at all): the whole 34-proposition structure below
  — every quotation, the claim inventory table, which propositions are
  graded versus declined and why, and the wording of what each graded
  proposition would measure. This is the content the order-of-work rule is
  actually about, and it held: no proposition's GRADE/DECLINE disposition or
  measurement description changed after the fixture existed.
- **After** (commit `7341b0a`, once the fixture was built, run, and its
  baseline read): five sections' *mechanism prose* was corrected to match
  what the built fixture actually does, without changing any proposition's
  disposition, expected value, or citation — P3 now says the check keys on
  `COPINS` rather than a bare `dbr` (refresh DMA, which fires unconditionally,
  was satisfying the original bare check regardless of the Copper's state);
  P17 and P31 note that `agnus_audiodma.v` needs `audio_dmas` alongside
  `audio_dmal` to load a written pointer at all; P18 notes the refresh-only
  baseline comparison its threshold rests on; P29b notes the assertion
  compares against a fixed expected value, not the (possibly mutated)
  stimulus address. Every one of these is a description of an RTL mechanic
  discovered while making the measurement work, not a change to what page 1–14
  was asked to prove. A later pass (uncommitted at time of writing, gate-
  compliance only) added `CONTROL-FIXTURE-RESET`'s own mutation record and
  fixed four `expect()` calls whose labels a tooling regex could not parse —
  neither touches this file, since neither changes a proposition's content.

## Grep-for-existing-assertion, disclosed

`hrm/` was grepped for prior assertions on this chapter's propositions before
any `expect()` below was written, per the brief. Matches that changed a
proposition's disposition are cited at that proposition below; the notable
ones, gathered in one place:

- **`hrm/ch3-beam-counters-and-blanking`'s `PAL-FIELD-312-NTSC-FIELD-262-LINES`**
  (uid `3-video.c11`, shared by an opus and a sol record) already grades, with
  mutation-tested exactness, the NTSC-262/PAL-312 total field length that this
  chapter's page-3 and page-5 NTSC/PAL sentences (P6, P7, P24 below) rest on.
- **`hrm/ch4-sprite-dma-sequence`'s `A5-CHANNEL-REUSED-MULTIPLE-TIMES-WITHIN-
  ONE-FIELD`** (232-claim package, ids including `CH4-reuse.c1`, `CH4-reuse.c4`,
  `CH4-reuse.c7` etc.) already grades sprite-channel reuse with a minimum
  line separation and beam-position gating — this chapter's page-4 sprite-reuse
  sentences (P12, P13 below).
- **`hrm/ch6-blitter-minterm`** (182 testable claims) already grades the
  blitter's 256-combination minterm function exhaustively (all 2048/256/768
  cases swept) — the "combine the data in one of 256 different possible ways"
  clause (P15 below) is its question, not this package's.
- **`hrm/ch6-blitter-line`** and **`hrm/ch6-blitter-fill`** already grade
  the special line-draw mode and the area-fill logic as mechanisms — the
  page-4 sentence naming them (P16 below) is graded there, not here.
- **`hrm/ch2-copper-waitskip-range`** (`COP-04`, `COP-08`) already grades
  WAIT/SKIP triggering many times per list, mid-line and at specific beam
  positions with mutation-tested exactness — page 3's "trigger many times per
  screen... mid-line... during blanking" sentence (P10 below) is graded
  there.
- **Page 6's four DMA-pointer sentences (P25–P28) are already covered,
  engine by engine, by five existing packages**, discovered only after
  attempting to design a fixture-boundary measurement for them: bitplane
  pointer range/writability by `hrm/ch3-bitplane-pointers-and-modulo`
  ("the bitplane pointer pair... pointer writes (BPLxPTH/BPLxPTL)"), sprite
  pointer range/writability by `hrm/ch4-sprite-pointer-registers` ("SPRxPTH/
  SPRxPTL... eight independent pairs exist"), audio pointer range/writability
  by `hrm/ch5-audio-location-pointer` ("the audio DMA location/pointer
  registers (AUDxLCH/AUDxLCL)", 176 claims), the Copper's own location
  registers by `hrm/ch2-copper-locreg-width` ("the width of the Copper
  location registers (COP1LCH/COP1LCL, COP2LCH/COP2LCL)... by walking single
  bits through the register-bus write path"), and the blitter's own pointer
  bank by `hrm/ch6-blitter-adrgen` ("four independent BLTxPT pointer
  registers... written as a byte address... the pointer's value after a
  simulated blit row"). Every engine page 6 names already has a package
  whose whole purpose is that engine's pointer register — this chapter's
  four sentences add no proposition none of them already grades. Bitplane
  and sprite DMA are also, as a consequence, never triggered anywhere in
  this fixture (see BOUNDARY below): reaching them would have meant
  reproducing the display-window/fetch-window and sprite-list machinery
  those two packages already own, for no new content.
- No existing package builds at the `agnus` **top-level** boundary while also
  grading an HRM-conformance claim about the Copper crossing into another
  chip's register space, the CPU-side double-trigger effect of a bare read,
  or the shared chip-address bus multiple engines multiplex onto — every
  `agnus_copper_*` and `agnus_blitter_*` sibling package builds its own
  submodule alone (`agnus_copper_list`'s own non-goals note: "Instruction
  cycle counts... are arbitration-level and need the Agnus top level" — this
  package is the first to build that level for these questions). That is the
  content this package adds.

## Figure 1-1

`docs/references/amiga-hrm-3rd/ch1/figures/p8.png` was opened and read. Three
claims in this package's inventory cite it (P29a, P29b below); their content
is described where they are graded.

## Claim inventory — 67 claims grouped into 34 propositions

Two readers (`sol`, `opus`) independently extracted this chapter; on 32 of
these 34 propositions both readers cite the identical quotation (a twin
pair, two uids); on one (P29a) only `sol` produced a claim; on one further
split (the Figure 1-1 REG ADR BUS, P29b) both readers cite it under
differently-numbered bare ids (`opus:1-blockdiag.c10` / `sol:1-blockdiag.c21`)
— a reminder that, per the build brief, **the `(reader, id)` pair is the uid;
the bare id alone is not**, and this chapter's own bare-id numbering drifts
by one between readers from page 3 onward (`sol:...c47` twins
`opus:...c46`, not `opus:...c47`).

| # | page | uids | disposition | label below |
|---|---|---|---|---|
| P1 | 1 | `sol/opus:1-components.c9` | GRADE | DMA-CHANNEL-TAKES-BUS-FROM-CPU |
| P2 | 1 | `sol/opus:1-components.c11` | GRADE | ADDRESS-OUTPUT-CEILING-2MB |
| P3 | 2 | `sol/opus:1-components.c34` | GRADE | SPECIAL-PURPOSE-HARDWARE-TAKES-BUS (copper) |
| P4 | 2 | `sol/opus:1-components.c37` | GRADE | DMA-WRITE-WITHOUT-CPU (disk) |
| P5 | 3 | `sol/opus:1-components.c42` | GRADE | ECS-ADDRESS-CEILING-2MB |
| P6 | 3 | `sol:...c47`/`opus:...c46` | DECLINE | → ch3-beam-counters-and-blanking |
| P7 | 3 | `sol:...c49`/`opus:...c48` | DECLINE | → ch3-beam-counters-and-blanking |
| P8 | 3 | `sol:...c56`/`opus:...c55` | GRADE | COPPER-GATED-BY-BEAM-POSITION |
| P9 | 3 | `sol:...c57`/`opus:...c56` | GRADE | COPPER-REACHES-OUTSIDE-AGNUS |
| P10 | 3 | `sol:...c58`/`opus:...c57` | DECLINE | → agnus_copper_waitskip_range |
| P11 | 3 | `sol:...c59`/`opus:...c58` | DECLINE | → P9 (this package) |
| P12 | 4 | `sol:...c65`/`opus:...c64` | DECLINE | → ch4-sprite-dma-sequence |
| P13 | 4 | `sol:...c66`/`opus:...c65` | DECLINE | → ch4-sprite-dma-sequence |
| P14 | 4 | `sol:...c71`/`opus:...c70` | GRADE | BLITTER-MOVES-DATA-ACROSS-BOUNDARY |
| P15 | 4 | `sol:...c72`/`opus:...c71` | DECLINE | → agnus_blitter_minterm |
| P16 | 4 | `sol:...c73`/`opus:...c72` | DECLINE | → agnus_blitter_line, agnus_blitter_fill |
| P17 | 4 | `sol:...c81`/`opus:...c80` | GRADE | DMA-CHORES-INDEPENDENT-OF-CPU |
| P18 | 4 | `sol:...c83`/`opus:...c82` | GRADE | CHIP-MEMORY-INTERLEAVED-BASIS |
| P19 | 4 | `sol:...c84`/`opus:...c83` | GRADE | CPU-GETS-EVERY-SLOT-WHEN-DMA-IDLE (control) |
| P20 | 4 | `sol:...c85`/`opus:...c84` | DECLINE | → P18/P19 (this package) |
| P21 | 4 | `sol:...c86`/`opus:...c85` | GRADE | DMA-REDUCES-CPU-SLOT-COUNT |
| P22 | 4 | `sol:...c87`/`opus:...c86` | DECLINE | → ch3-ddf-fetch-window |
| P23 | 4 | `sol:...c88`/`opus:...c87` | DECLINE | → P9/P14/P21 (this package) |
| P24 | 5 | `sol:...c103`/`opus:...c102` | DECLINE | → ch3-beam-counters-and-blanking |
| P25 | 6 | `sol/opus:1-components.c111` | DECLINE | → 5 sibling pointer packages |
| P26 | 6 | `sol/opus:1-components.c112` | DECLINE | → 5 sibling pointer packages |
| P27 | 6 | `sol/opus:1-components.c113` | DECLINE | → 5 sibling pointer packages |
| P28 | 6 | `sol/opus:1-components.c114` | DECLINE | → agnus_blitter_adrgen |
| P29a | 8 | `sol:1-blockdiag.c12` | GRADE | AGNUS-DMA-ARBITER-PRESENT (figure) |
| P29b | 8 | `opus:1-blockdiag.c10`/`sol:1-blockdiag.c21` | GRADE | REG-ADR-BUS-CROSSES-BOUNDARY (figure) |
| P30 | 9 | `sol:1-examples.c24`/`opus:1-examples.c23` | GRADE | COPPER-USES-OFFSET-NOT-LOCATION-REG |
| P31 | 12 | `sol/opus:1-guidelines.c28` | GRADE | HETEROGENEOUS-DMA-SHARES-ONE-BUS |
| P32 | 13 | `sol/opus:1-guidelines.c61` | GRADE | READ-ALONE-RETRIGGERS-COPJMP |
| P33 | 14 | `sol/opus:1-guidelines.c86` | GRADE | NO-REGISTER-IS-BOTH-READABLE-AND-WRITABLE |

18 propositions graded with an `expect()`; 16 declined on a `record()` line.
All 67 claims (34 propositions) are accounted for below — none left on no
line at all.

## BOUNDARY

Fixture `agnus` (`nanomig/src/minimig-aga/agnus.v`), Verilating that file plus
every submodule it instantiates (`agnus_refresh`, `agnus_diskdma`,
`agnus_audiodma`, `agnus_bitplanedma`, `agnus_spritedma`, `agnus_copper`,
`agnus_blitter` and the three modules `agnus_blitter` itself instantiates:
`agnus_blitter_adrgen`, `agnus_blitter_barrelshifter`, `agnus_blitter_fill`,
`agnus_blitter_minterm`, and `agnus_beamcounter`) — the same 13-file set
`agnus.v`'s own module-instantiation list (lines 319–490) names, nothing more,
nothing less. Ports: `clk`, `clk7_en`, `cck` (colour-clock enable — driven by
a `ClkGen` reproducing `amiga_clk.v`'s own division, the same scheme several
sibling `agnus_beamcounter`/`agnus`-boundary packages use), `reset`, `aen`,
`rd`, `hwr`, `lwr`, `data_in[15:0]`, `data_out[15:0]` (out), `address_in[8:1]`,
`address_out[20:1]` (out), `reg_address_out[8:1]` (out), `cpu_custom` (out),
`dbr` (out), `dbwe` (out), `refresh` (out), sync/blanking/beam-strobe outputs,
`htotal[8:0]` (out), `harddis`/`varbeamen` (out), `int3` (out),
`audio_dmal[3:0]`/`audio_dmas[3:0]` (in — normally driven by Paula, which is
outside this boundary and so is driven directly as stimulus here),
`disk_dmal`/`disk_dmas` (in, same reasoning), `bls` (in), `ntsc`/`a1k`/`ecs`/
`aga`/`floppy_speed` (in).

Register bus: unlike the full `minimig` top level's 68000 bus (which needs
`_cpu_as`/`_cpu_uds`/`_cpu_lds`/`_cpu_dtack`), `agnus`'s own register
interface is the simpler internal one `agnus.v:114` shows directly —
`reg_address_cpu = (aen&(rd|hwr|lwr)) ? address_in : 8'hFF` — a write
(`hwr`/`lwr`) or a read (`rd`) is presented on `address_in`/`data_in` for one
`clk7_en`-active tick and withdrawn; there is no dtack to wait for at this
boundary. Register addresses used below (byte addresses, matching each
submodule's own `parameter`/`localparam` table, read and cited by file:line
in the assertions themselves): `DMACON`=$096, `DMACONR`=$002
(`agnus.v:103-104`), `COP1LCH`=$080/`COP1LCL`=$082/`COPJMP1`=$088
(`agnus_copper.v:78-88`), `AUD0LCH`=$0A0/`AUD0LCL`=$0A2
(`agnus_audiodma.v:53-89`), `DSKPTH`=$020/`DSKPTL`=$022
(`agnus_diskdma.v:21-22`), `BLTCON0`=$040, `BLTCON1`=$042, `BLTSIZE`=$058,
`BLTDPTH`=$054/`BLTDPTL`=$056 (`agnus_blitter.v:106-119`,
`agnus_blitter_adrgen.v:27-34`).

**Bitplane DMA and sprite DMA are never triggered anywhere in this
fixture.** Both need real setup this fixture deliberately does not build:
bitplane DMA needs `agnus_bitplanedma.v`'s `vdiwena` (a two-**line** latch
of the best `DIWSTRT`/`DIWSTOP` window seen on the *previous* frame,
`agnus_bitplanedma.v:150-186`) and a valid `DDFSTRT`/`DDFSTOP` fetch window;
sprite DMA needs a programmed VSTART/VSTOP control pair per sprite. Building
either is exactly the display-window and sprite-list machinery
`hrm/ch3-vertical-display-window`, `hrm/ch3-ddf-fetch-window` and
`hrm/ch4-sprite-dma-sequence` already own — reproducing it here would be
this chapter's introductory sentences inflated into a mechanism test the
page never asked for. Every assertion below that needs "a DMA channel"
uses disk DMA (`disk_dmal`, unconditional on any register once
asserted — `agnus_diskdma.v:34-44`'s slot table needs only `hpos`, which
free-runs from reset), audio DMA (`audio_dmal`; `agnus_audiodma.v:108`,
`assign dma = dmal`, unconditional), or the Copper/blitter (already driven
by register writes alone once `DMACON` enables them).

**Refresh DMA fires unconditionally, with no enable of any kind**
(`agnus_refresh.v`: `dma=1` at `hpos` = $09, $0D, $11, $15, every line,
regardless of any register or input). Measured directly by this fixture
before it is relied on (see CONTROL-DMA-IDLE-BASELINE below) rather than
assumed away — a control case that claimed "the CPU gets every slot when
every channel is idle" without this would have been quietly wrong by four
slots per line.

## Propositions graded

### P1 — DMA-CHANNEL-TAKES-BUS-FROM-CPU (page 1)

> Custom graphics and audio chips with DMA capability.
— `sol:1-components.c9`, `opus:1-components.c9`

Graded structurally: enabling one DMA channel (disk, via `disk_dmal` —
`agnus_diskdma.v`'s own slot table needs no register configuration beyond a
pointer write, unlike bitplane/sprite DMA, see BOUNDARY above) makes
`agnus.dbr` rise and `agnus.address_out` present a chip address, while
`cpu_custom` is driven low for that cycle (`agnus.v:167-266`'s priority mux).
This is also this package's control-pass baseline: the fixture must be shown
able to produce a *known* address (the pointer value written to `DSKPTH`/
`DSKPTL`) before any later assertion trusts `address_out`'s content.

### P2 — ADDRESS-OUTPUT-CEILING-2MB (page 1)

> The custom chips can access up to 2MB of memory directly without using the
> 68000 CPU.
— `sol:1-components.c11`, `opus:1-components.c11`

`address_out` is declared `[20:1]` (`agnus.v:67`) — 20 word-address bits, a
2MB byte ceiling and no more. Graded by writing the disk DMA pointer
(`DSKPTH`/`DSKPTL`) to the top of that range and reading it back on
`address_out` with `cpu_custom==0` during a granted disk-DMA cycle.

### P3 — SPECIAL-PURPOSE-HARDWARE-TAKES-BUS-FROM-PROCESSOR (page 2)

> In addition to the 680x0, all Amiga models contain special purpose
> hardware known as the custom chips that greatly enhance system
> performance.
— `sol:1-components.c34`, `opus:1-components.c34`

"All Amiga models" and "greatly enhance system performance" set no
threshold and are not graded (as both claims' own `classify.evidence`
already say). What is graded, using a *different* channel than P1 so this is
a distinct measurement rather than a copy: enabling the **Copper** channel
(`DMACON` bit 7+9) takes the bus from the processor (`cpu_custom` low,
`dbr` high) exactly as P1 showed for disk DMA. Checked specifically via
`reg_address_out` reading `COPINS` ($08C, `agnus_copper.v`'s own
instruction-fetch marker) during that cycle, not a bare `dbr` check --
refresh DMA (`agnus_refresh.v`) fires unconditionally, with no enable of
its own, and would otherwise satisfy a bare `dbr` check even with the
Copper left disabled.

### P4 — DMA-WRITE-WITHOUT-CPU (page 2)

> Because the custom chips have DMA capability, they can access memory
> without using the 680x0 CPU - this frees the CPU for other types of
> operations.
— `sol:1-components.c37`, `opus:1-components.c37`

Graded with disk DMA (`disk_dmal` asserted, the request path Paula would
normally drive), which is this fixture's only channel that can produce a
**write** cycle (`dbwe`, `agnus_diskdma.v:51`, `wr = ~dmas`) — distinct
content from P1/P3 (a memory *write*, not only a request), while
`cpu_custom` stays low throughout, and the fixture confirms `cpu_custom`
returns to 1 (available to the CPU again) once the request is withdrawn.

### P5 — ECS-ADDRESS-CEILING-2MB (page 3)

> The latest version of the custom chips, known as the Enhanced Chip Set or
> ECS) can handle up to 2 MB of memory and has other advanced features.
— `sol:1-components.c42`, `opus:1-components.c42`

"Other advanced features" names nothing specific and is not graded. The 2MB
figure is graded exactly as P2, but with `ecs=1` driven throughout (P2 runs
with `ecs=0`) — a distinct configuration, same ceiling.

### P6, P7 — declined (page 3)

> On NTSC systems, the Amiga typically produces a 320 by 200 non-interlaced
> or 320 by 400 interlaced display in 32 colors.
— `sol:1-components.c47`, `opus:1-components.c46`

> On PAL systems, the Amiga typically produces a 320 by 256 non-interlaced
> or 320 by 512 interlaced display in 32 colors.
— `sol:1-components.c49`, `opus:1-components.c48`

Declined. Two separate reasons, both recorded at the `record()` call:

1. The hardware-decoded fact these numbers rest on — the NTSC/PAL total
   field length the `ntsc` input selects between (`agnus_beamcounter.v:87-90`,
   `VTOTAL_PAL_VAL`=312, `VTOTAL_NTSC_VAL`=262) — is already graded, with
   mutation-tested exactness, by `hrm/ch3-beam-counters-and-blanking`'s
   `PAL-FIELD-312-NTSC-FIELD-262-LINES` assertion (uid `3-video.c11`).
   Re-grading it here risks exactly the divergence the build brief warns
   about.
2. The literal "200"/"256" **visible**-line figures these sentences print
   are not the same number as the 262/312 total (blanking is wider than
   that difference alone accounts for) — the manual's own word "typically"
   signals a software convention, not a hard decode, and the precise figure
   depends on `DIWSTRT`/`DIWSTOP` (the vertical display window), which
   `ch3-beam-counters-and-blanking` and `ch3-field-geometry` both disclose
   is **outside** `agnus_beamcounter`'s own boundary. Reproducing that
   derivation here would be exactly the "introductory sentence inflated
   into a mechanism test the page never asked for" trap the build brief
   names, for a question chapter 3's own display-window packages
   (`ch3-vertical-display-window`) already own.
32 colors (both sentences) needs Denise's colour table, which is not part
of this fixture at all — `agnus` alone cannot answer it, and it is not
re-declared as its own claim id since neither reader split it out from
these two sentences.

### P8 — COPPER-GATED-BY-BEAM-POSITION (page 3)

> A custom graphics coprocessor, called the Copper, that allows changes to
> most of the special purpose registers in synchronization with the
> position of the video beam.
— `sol:1-components.c56`, `opus:1-components.c55`

Graded at the `agnus` boundary — a level no `agnus_copper_*` sibling package
builds at (their own non-goals disclose this). A Copper list holding one
WAIT for a chosen `(vpos,hpos)` followed by one MOVE is loaded; the fixture
confirms the MOVE's effect (`reg_address_out` reaching the target address on
a copper-granted cycle) is observed only **at or after** the beam reaches
the WAIT position, never before — proof that the Copper's `vpos`/`hpos`
inputs (`agnus_copper.v:69-70`, wired from the same `agnus_beamcounter`
instance the rest of this fixture also uses) actually gate it, not only that
the ports are declared. This is an ordering fact, not a cycle-exact timing
claim (cycle-accuracy is `agnus_copper_waitskip_range`'s question).

### P9 — COPPER-REACHES-OUTSIDE-AGNUS (page 3)

> This allows such special effects as mid-screen changes to the color
> palette, splitting the screen into multiple horizontal slices each
> having different video resolutions and color depths, beam-synchronized
> interrupt generation for the 680x0, and more.
— `sol:1-components.c57`, `opus:1-components.c56`

Every named effect (colour palette, display-mode register, interrupt
register) lives in Denise or Paula, not Agnus. Graded at the boundary: a
Copper MOVE targets an address `agnus` itself never decodes (`data_out`
stays 0 for it — confirmed by the fixture, since `agnus.v:277`'s
`data_bmc | dmaconr | data_blt` OR only answers Agnus's own registers) and
the fixture confirms `reg_address_out` nonetheless carries that address
unfiltered during the Copper's granted cycle — the value crosses `agnus`'s
own boundary rather than being consumed or blocked inside it. That crossing,
not any particular chip's response to it, is what this sentence asserts is
possible.

### P10 — declined (page 3)

> The coprocessor can trigger many times per screen, in the middle of
> lines, and at the beginning or during the blanking interval.
— `sol:1-components.c58`, `opus:1-components.c57`

Declined. This is WAIT/SKIP triggering at arbitrary line/mid-line/blanking
beam positions, repeated across one list — exactly what
`hrm/ch2-copper-waitskip-range` (`COP-04`, `COP-08`) already grades with
mutation-tested exactness (its own header: "WAIT/SKIP comparison
semantics... BFD and counter rollover"). A weaker restatement here adds no
falsifiable content P8 does not already cover at this boundary (that the
gate genuinely depends on beam position, not that it can fire at every kind
of position).

### P11 — declined (page 3)

> The coprocessor itself can directly affect most of the registers in the
> other custom chips, freeing the 680x0 for general computing tasks.
— `sol:1-components.c59`, `opus:1-components.c58`

Declined, citing P9 in this same package: this sentence is the general
statement of the capability P9 already demonstrates directly (a Copper MOVE
reaching an address outside `agnus`'s own decode). "Most of the registers"
and "freeing the 680x0 for general computing tasks" add no separately
falsifiable threshold — P9's crossing, run with `cpu_custom` sampled
throughout (already low, since the Copper channel holds the DMA grant for
the cycle), is the whole of what is measurable here.

### P12, P13 — declined (page 4)

> After producing the last line of a sprite on the screen, a sprite DMA
> channel may be used to produce yet another sprite image elsewhere on
> screen (with at least one horizontal line between each reuse of a sprite
> processor).
— `sol:1-components.c65`, `opus:1-components.c64`

> Thus, many small sprites can be produced by simply reusing the sprite
> processors appropriately.
— `sol:1-components.c66`, `opus:1-components.c65`

Declined. `hrm/ch4-sprite-dma-sequence` (232 claims) already grades exactly
this — its own header: "the channel's reuse several times within one
display field and the minimum separation and beam-position gating that
reuse requires" — at its `A5-CHANNEL-REUSED-MULTIPLE-TIMES-WITHIN-ONE-FIELD`
assertion, citing (among others) `CH4-reuse.c1`, `CH4-reuse.c4`,
`CH4-reuse.c7`, `CH4-reuse.c8`, `CH4-reuse.c10`–`c14`.

### P14 — BLITTER-MOVES-DATA-ACROSS-BOUNDARY (page 4)

> Custom bit blitter used for high speed data movement, adaptable to
> bitplane animation.
— `sol:1-components.c71`, `opus:1-components.c70`

"Adaptable to bitplane animation" names a use, not a threshold, and is not
graded. Graded structurally, and deliberately **not** re-testing the
blitter's combining logic (chapter 6's question, P15/P16 below): a minimal
channel-D-only blit (`BLTCON0`=$01FF — `used`=1, `usea`=`useb`=`usec`=0,
`LF`=$FF, a constant-output minterm code that needs no source data) is
started via `BLTSIZE`; the fixture confirms `dbr`+`dbwe`+`address_out` show
a real DMA write cycle at the destination pointer address, and `int3`
(blit-finished interrupt) fires — the blitter, as a subsystem of `agnus`,
moves data across the chip-memory boundary. (`busy` itself is
`agnus_blitter`'s own internal port, not brought out at `agnus`'s top level
except through `DMACONR`'s bit 14 — which this measurement does not rely
on, since that readback is only meaningful on ticks the CPU register path,
not a DMA channel, currently holds the shared address bus.) The written
*value* is never asserted (that is chapter 6's minterm-correctness
question).

### P15, P16 — declined (page 4)

> The blitter has been designed to efficiently retrieve data from up to
> three sources, combine the data in one of 256 different possible ways,
> and optionally store the combined data in a destination area.
— `sol:1-components.c72`, `opus:1-components.c71`

> The bit blitter, in a special mode, draws patterned lines into
> rectangularly organized memory regions at a speed of about 1 million dots
> per second; and it can efficiently handle area fill.
— `sol:1-components.c73`, `opus:1-components.c72`

Declined, per the build brief's own instruction for this package: "the
blitter's minterm logic is chapter 6's question and is already graded
there." `hrm/ch6-blitter-minterm` (182 claims) exhaustively sweeps all
256 LF codes (2048/256/768 cases across three coverage groups) — the
"combine... in one of 256 different possible ways" clause. `hrm/
agnus_blitter_line` and `hrm/ch6-blitter-fill` grade the line-draw and
area-fill mechanisms by name. P14 above already grades this sentence's
structural half (the blitter moves data across the boundary); these two
sentences' remaining content is exactly the mechanism chapter 6 owns.

### P17 — DMA-CHORES-INDEPENDENT-OF-CPU (page 4)

> As mentioned previously, the custom chips have DMA access to RAM which
> allows them to perform graphics, audio, and I/O chores independently of
> the CPU.
— `sol:1-components.c81`, `opus:1-components.c80`

Graded by enabling each of the three named chore categories in turn —
graphics (the blitter, via the same minimal channel-D blit P14 uses — a
real DMA write, not bitplane fetch, which this fixture never triggers, see
BOUNDARY above), audio (`audio_dmal` **and** `audio_dmas`), I/O
(`disk_dmal`) — and confirming each leg's `address_out`, during a granted
cycle, matches that leg's own distinct, just-written pointer. Two measured
mechanics shape this, both worth recording rather than leaving implicit:
`audio_dmas` (restart) must accompany `audio_dmal` on the channel's own
slot, or `agnus_audiodma.v`'s pointer just increments its own reset-default
value instead of loading the written `AUD0LCH`/`AUD0LCL` content (`audpt
<= dmas ? audlcout : audptout+1`); and each leg is checked against its own
pointer value rather than a bare `dbr`, because refresh DMA
(`agnus_refresh.v`) fires unconditionally, with no enable of its own, and
drives `address_out` to exactly 0 during its own grant — a bare `dbr` check
would let refresh alone satisfy a leg even with that leg's channel left
disabled.

### P18 — CHIP-MEMORY-INTERLEAVED-BASIS (page 4)

> The custom chips and the 680x0 CPU share Chip memory on a fully
> interleaved basis.
— `sol:1-components.c83`, `opus:1-components.c82`

`agnus.v:38-41`'s own slot-order comment and `cck` (`agnus.v:58`, "active
whenever hpos[0] is high (odd dma slots used by chipset)") describe
alternation by slot parity. Graded by enabling one DMA channel continuously
and sampling `cpu_custom` across many consecutive `clk7_en` ticks: it must
be high on some ticks and low on others in the same run (interleaved),
never constantly one or the other. Because refresh DMA fires
unconditionally regardless of any channel's own enable (see P17's note
above), "low on some ticks" alone would stay true even with disk DMA fully
disabled; a refresh-only baseline is measured in the same run (a second,
DMA-idle harness) and the disk-active run is required to show meaningfully
more low ticks than that baseline, not merely a nonzero count.

### P19 — CPU-GETS-EVERY-SLOT-WHEN-DMA-IDLE (page 4, control case)

> Since the 680x0 only needs to access the Chip memory bus during each
> alternate clock cycle in order to run full speed, the rest of the time
> the Chip memory bus is free for other activities.
— `sol:1-components.c84`, `opus:1-components.c83`

This package's other required control case (alongside P1): with every
*software-visible* DMA channel disabled (`DMACON` DMAEN bit clear, no
`audio_dmal`/`disk_dmal` asserted), `cpu_custom` must read 1 on every
sampled tick **except** the four fixed refresh slots per line
(`agnus_refresh.v`, unconditional — see BOUNDARY above; this is measured by
the fixture itself, not assumed) — the processor is never made to wait for
a channel that is not using the bus, refresh aside. Graded on the same
measurement basis (`cpu_custom` sampling) as P18/P21, which is what makes
those reds (if any) meaningful rather than a units-error.

### P20 — declined (page 4)

> The custom chips use the memory bus during these free cycles, effectively
> allowing the CPU to run at full speed most of the time.
— `sol:1-components.c85`, `opus:1-components.c84`

Declined, citing P18 and P19 in this same package: this sentence is the
converse framing of what those two already show together (DMA takes only
the slots the CPU is not using — P18 — and the CPU gets every slot when no
DMA channel is active — P19). "Effectively allowing... full speed most of
the time" states no separate threshold.

### P21 — DMA-REDUCES-CPU-SLOT-COUNT (page 4)

> There are some occasions though when the custom chips steal memory cycles
> from the 680x0.
— `sol:1-components.c86`, `opus:1-components.c85`

Graded by comparing `cpu_custom`'s high-tick count over a fixed sample
window with DMA idle (P19's baseline) against the same window with one
channel enabled: the count must be strictly lower with DMA active — "some
occasions" the chips "steal" a cycle the CPU would otherwise have had.

### P22 — declined (page 4)

> In the higher resolution video modes, some or all of the cycles normally
> used for processor access are needed by the custom chips for video
> refresh.
— `sol:1-components.c87`, `opus:1-components.c86`

Declined. Unlike P17/P18/P19/P21 (disk/audio/blitter DMA, none of which need
any window setup), this sentence is specifically about bitplane-fetch volume
scaling with resolution/plane count — measuring it means actually running
bitplane DMA, which needs `agnus_bitplanedma.v`'s two-line `vdiwena` latch
and a programmed `DDFSTRT`/`DDFSTOP` fetch window (see BOUNDARY above,
"Bitplane DMA and sprite DMA are never triggered"). That is exactly
`hrm/ch3-ddf-fetch-window`'s own subject (DDFSTRT/DDFSTOP, word counts) —
reproducing it here for one comparison would be the mechanism-test trap the
build brief warns an introductory sentence invites, for a question that
package already owns.

### P23 — declined (page 4)

> In that case, the Copper and the blitter in the custom chips steal time
> from the 680x0 for jobs they can do better than the 680x0.
— `sol:1-components.c88`, `opus:1-components.c87`

Declined, citing P9, P14 and P21 in this same package: that the Copper
(P9, granted DMA cycles with `cpu_custom` low) and the blitter (P14, granted
DMA cycles with `cpu_custom` low) both take cycles the same way any other
channel does (P21) is already shown. "Jobs they can do better than the
680x0" is a judgement the manual itself does not attach a threshold to and
is not graded.

### P24 — declined (page 5)

> In addition, ECS Fat Agnus supports both the NTSC and PAL video standards
> on a single chip.
— `sol:1-components.c103`, `opus:1-components.c102`

Declined, same citation as P6/P7: `hrm/ch3-beam-counters-and-blanking`'s
`PAL-FIELD-312-NTSC-FIELD-262-LINES` (uid `3-video.c11`) already proves, on
one build with `ntsc` toggled at runtime (not two chip variants), that both
exact field lengths are produced — a strictly *stronger* fact than "the two
standards are merely different from each other", which is all a fresh
assertion here could add.

### P25, P26 — declined (page 6)

> Another primary feature of the Amiga hardware is the ability to
> dynamically control which part of the Chip memory is used for the
> background display, audio, and sprites.
— `sol:1-components.c111`, `opus:1-components.c111`

> The Amiga is not limited to a small, specific area of RAM for a frame
> buffer.
— `sol:1-components.c112`, `opus:1-components.c112`

Declined. Both are restatements — one general, one in negative form — of
the fact P27 discusses below; see P27 for the citation, which applies to
all four of P25/P26/P27/P28 alike.

### P27 — declined (page 6)

> Instead, the system allows display bitplanes, sprite processor control
> lists, coprocessor instruction lists, or audio channel control lists to
> be located anywhere within Chip memory.
— `sol:1-components.c113`, `opus:1-components.c113`

Declined. This sentence names four engines, and — found only while designing
this proposition's own fixture measurement, not before — every one of the
four already has a dedicated existing package whose whole purpose is that
engine's own pointer register's range and writability: bitplane
(`hrm/ch3-bitplane-pointers-and-modulo`), sprite
(`hrm/ch4-sprite-pointer-registers`), audio (`hrm/ch5-audio-location-pointer`,
176 claims), Copper (`hrm/ch2-copper-locreg-width`, which walks single
bits through the register-bus write path to `COP1LCH`/`COP1LCL`). A fresh
sweep at the `agnus` boundary would answer the identical question four
existing packages already answer in more depth, and — for bitplane and
sprite specifically — would require reproducing the display-window/
fetch-window and sprite-list machinery those packages own (see BOUNDARY
above). P25 and P26, the same paragraph's general and negative framings of
this same fact, are declined for the same reason.

### P28 — declined (page 6)

> This same region of memory can be accessed by the bit blitter.
— `sol:1-components.c114`, `opus:1-components.c114`

Declined, same reasoning as P27: `hrm/ch6-blitter-adrgen`'s own stated
purpose is "four independent BLTxPT pointer registers... written as a byte
address... the pointer's value after a simulated blit row" — exactly this
sentence's proposition (the blitter's pointer reaches the same chip-address
range the other engines do), already graded there.

### P29a — AGNUS-DMA-ARBITER-PRESENT (page 8, Figure 1-1)

> [Figure 1-1] — a double-outlined box labelled AGNUS sits at the top of the
> dashed CUSTOM CHIP SECTION, with an arrowhead entering from the chip
> address line on its left and another entering from the REG ADR BUS on its
> right.
— `sol:1-blockdiag.c12`

Graded: the DMA priority arbiter the figure's AGNUS box represents
(`agnus.v:167-266`) is exercised by enabling one channel and observing the
arbitration outcome on `address_out`/`dbr`/`cpu_custom`, precisely as P1.
Only `sol` produced a claim on this half of the figure (the AGNUS box
itself, distinct from the REG ADR BUS arrows graded at P29b); no `opus`
twin exists for it in this package's inventory.

### P29b — REG-ADR-BUS-CROSSES-BOUNDARY (page 8, Figure 1-1)

> [Figure 1-1] — a vertical label bar REG ADR BUS runs down the right edge
> of the dashed CUSTOM CHIP SECTION and sends one arrowhead into AGNUS, one
> into PAULA and one into DENISE.
— `opus:1-blockdiag.c10`, `sol:1-blockdiag.c21`

Graded via the **CPU** register-write path (distinct from P9's Copper-DMA
path, though the underlying question — does a value cross `agnus`'s
boundary on `reg_address_out` — is the same shape): a plain CPU register
write (`aen`+`hwr`+`lwr`) at an address `agnus` does not itself decode is
shown to still appear on `reg_address_out`, exactly the wire the figure
draws leaving AGNUS toward PAULA and DENISE (`agnus.v:136`,
`assign reg_address_out = reg_address`, unconditional — not gated by
whether `agnus` itself answers that address). The assertion compares
`reg_address_out` against the fixed expected value ($C0, COLOR00's own
word address) rather than re-deriving the expectation from whatever address
the stimulus used — comparing against the stimulus's own address would
stay trivially true under the mutation too, since the mutation changes what
is written, not what ought to be read back.

### P30 — COPPER-USES-OFFSET-NOT-LOCATION-REG (page 9)

> (One exception to this rule is the Copper which uses only the offset
> access the registers.)
— `sol:1-examples.c24`, `opus:1-examples.c23` (both readers disclose the
scan-damaged grammar; the dropped word is "to", not graded content)

Graded: a Copper MOVE's `reg_address_out` is driven purely from the MOVE
instruction word's own 9-bit field (`agnus_copper.v:216`,
`reg_address_out[8:1] = ir2[8:1]`) — changing `COP1LC` to point the list
fetch somewhere else changes *where the instruction is fetched from*, never
*which register it targets*. The fixture confirms this directly: the same
MOVE word, fetched via two different `COP1LC` values, produces the same
`reg_address_out`.

### P31 — HETEROGENEOUS-DMA-SHARES-ONE-BUS (page 12)

> This includes bitplanes, sound samples, trackdisk buffers, and images for
> sprites, bobs, pointers, and gadgets.
— `sol:1-guidelines.c28`, `opus:1-guidelines.c28`

"Bobs", "pointers" and "gadgets" are OS-level names for imagery, not for
engines — what this fixture can reach underneath them is the blitter, audio
and disk fetches (bitplane and sprite DMA are not triggered anywhere in this
fixture, see BOUNDARY above), disclosed as the honest limit here (matching
both readers' own evidence text, which itself names disk/audio/bitplane/
sprite/blitter as the underlying engines, not the OS-level names). Graded:
the blitter (trackdisk-buffer-style raw data, standing in for "images" —
its own minimal write from P14), audio DMA (sound samples, `audio_dmal`
**and** `audio_dmas` — see P17's note on why the restart bit is required
for `agnus_audiodma.v` to load the written pointer at all) and disk DMA
(trackdisk buffers) are each pointed, in turn, at the **same** target chip
address, and the fixture confirms all three heterogeneous engines cross the
identical `address_out` bus — one shared Chip memory, not per-kind separate
address spaces.

### P32 — READ-ALONE-RETRIGGERS-COPJMP (page 13)

> This can cause a hardware register to be triggered twice.
— `sol:1-guidelines.c61`, `opus:1-guidelines.c61`

Graded, following both claims' own evidence: `agnus.v:114`'s
`reg_address_cpu` is driven identically by a read (`rd`) as by a write
(`hwr`/`lwr`) — no qualifier distinguishes them — and
`agnus_copper.v:236`'s COPJMP1 strobe (`if (reg_address_in[8:1]==
COPJMP1[8:1] || sof) copjmp1 <= 1`) fires on the address alone. The fixture
loads `COP1LC` pointing at address X, lets the Copper's list fetch move its
own program counter away from X, then performs a bare **read** (no write)
of `COPJMP1`, and confirms the Copper's next granted-cycle `address_out`
returns to X — the read alone re-triggered the jump, exactly the
"triggered twice" mechanism a CPU CLR instruction's read-then-write produces.

### P33 — NO-REGISTER-IS-BOTH-READABLE-AND-WRITABLE (page 14)

> All custom chip registers are Read-only or Write-only.
— `sol:1-guidelines.c86`, `opus:1-guidelines.c86`

Graded exactly as both claims' own evidence lays out: `agnus.v:277`'s
`data_out = data_bmc | dmaconr | data_blt` is the only read path, and it
answers nonzero only at addresses the register file explicitly decodes for
reading. The fixture sweeps `agnus`'s own known register addresses,
recording which answer a read with a distinguishable value, then separately
confirms (for `DMACON`, the one register this fixture can drive both ways)
that the same address's write has an observable effect — and that no
address does both.

## Extension — one claim routed here independently of `classify.package`

Claim `sol:5-appA-p263.c6` (page 263, Appendix A) reads:

> The lowest bit of all pointers must be written as zero.

Restated: the least significant bit of every custom-chip pointer must be
written as zero. Its own classification names `ch5-audio-location-pointer`
as `classify.package`; the routing step that decides which package grades a
claim, not `classify.package`, placed it here instead — this package is the
one place more than one engine's own pointer register (disk, audio, Copper,
blitter destination) already sits in a single build, which a single-engine
sibling package is not. That is why the claim was routed here, not why it
grades: it does not.

| # | page | uids | disposition | label below |
|---|---|---|---|---|
| P34 | 263 | `sol:5-appA-p263.c6` | RECORD | POINTER-LOW-BIT-WRITTEN-ZERO |

### P34 — POINTER-LOW-BIT-WRITTEN-ZERO (page 263, Appendix A) — recorded, not graded

> The lowest bit of all pointers must be written as zero.
— `sol:5-appA-p263.c6`

In context — between "These registers may be written with one MOVE.L
instruction" and the Chip-memory warning that follows — this sentence tells
the programmer what to write; the page states no hardware consequence for
writing it as 1, and silence is not a requirement. The claim is also
universal over every custom-chip pointer (bitplane, sprite, Copper, blitter
destination, disk, audio); this fixture has only two of the six built
(disk, audio).

Even read as a hardware claim, it would be unfalsifiable at this boundary:
`agnus_diskdma.v:56` forms DSKPTL's ALU input as
`{data_in[4:0],data_in[15:1]}` and `agnus_audiodma.v:89` captures only
`data_in[15:1]` into `audlcl` — the pointer's own bit 0 drives no wire in
either module — and `agnus`'s own `address_out` is declared only `[20:1]`
(`agnus.v:67`), a word address with no bit-0 position to disagree in. No
stimulus varying that bit, on any implementation sharing this port width,
could ever move it.

The file keeps two assertions, `POINTER-LOW-BIT-WRITTEN-ZERO-DISK` and
`-AUDIO` — each written with its own low register's bit 0 forced to 1
instead of the clean, already-even values every proposition above uses,
confirming the DMA-fetched address it observes is identical to what the
same pointer produces with bit 0 held at 0 — as that
unfalsifiable-at-this-boundary observation, and each is paired with a
`-BIT1-CONTROL` companion measurement on the same register's own bit 1 (a
bit the manual does not exempt), which must show a *different* observed
address when bit 1 differs, proving the comparator can detect a real
difference and the bit-0 result is not a vacuous equality for every bit
position. Neither carries this claim id; `sol:5-appA-p263.c6` is declined
via `record()` instead.

## Non-goals

This package does not grade: Copper/blitter cycle-exact timing (chapter 2,
chapter 6); the blitter's minterm/line/fill mechanisms (chapter 6); sprite
DMA reuse sequencing (chapter 4); the precise NTSC/PAL visible-line pixel
counts or 32-colour depth (chapter 3, and Denise, which is outside this
fixture entirely); DIWSTRT/DIWSTOP display-window sizing (chapter 3);
absolute clock frequency (no board oscillator exists at either UUT root, per
the build brief's own disclosed finding).
