# Specification — `ch1-chip-memory-banking`

Package: `hrm/ch1-chip-memory-banking`
Fixture: `minimig_bankmapper` (per the chapter 1 package list's top-module
assignment).
Claims file: `hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl`, filtered on
`classify.package == "ch1-chip-memory-banking"` (8 claims).
Subject: how much Chip memory the custom chips can reach, and how that ceiling
changes across the Agnus generations the manual names — the *banking*
behaviour, not the memory-map boundary (which is `ch1-memory-map-and-two-buses`,
top `gary`).

## Order-of-work disclosure

The build brief for this slice requires: write this file from the quotations
alone, commit it, and only then open the RTL — and says plainly that if the
task sheet handed the builder RTL facts ahead of that, the builder should say
so rather than pretend the order held. **It did not hold here.** The task
instructions that assigned this package already named, up front:
`minimig_bankmapper.v:31-36` (the four `memory_config` cases, 0.5M/1.0M/1.5M/2.0M
CHIP), that the top is a single small combinational module, and that
`gary.v:159` (a different claim, "the first 512K of memory is Chip memory")
belongs to a different package and must not be cited here. The claims file
itself also carries `classify.evidence` with `file:line` citations into
`minimig_bankmapper.v`, `gary.v` and `agnus.v` for every one of the 8 claims
— unavoidable, since filtering the claims file for this package is how this
builder was told to find its 8 claims at all. None of this was discovered by
this builder reading ahead on its own initiative; it was handed as part of the
package assignment. This file is nonetheless written from the
`extract.quote`/`classify.extract.restatement` content of the 8 claims below,
not from an independent RTL exploration — but the letter of "spec before RTL"
did not hold, and this paragraph says so rather than concealing it.
`minimig_bankmapper.v` was subsequently read in full (it is 38 lines) to
design the fixture; `gary.v` and `agnus.v` were not opened by this builder at
all — the evidence citations to them are carried as-is from the claims file
and are not re-verified or re-cited by this package's own test source.

## The 8 claims

Two blind readers (`sol`, `opus`) each quoted these three sentences
independently; `(reader, id)` is the uid, the bare id is not — sol and opus
used different bare ids for the same sentence (e.g. `sol:...c93` and
`opus:...c92` are the same A1000 sentence, read twice).

**Page 3, `sol:1-components.c40` / `opus:1-components.c40`:**
> The more Chip memory the Amiga has, the more graphics, audio, and I/O data
> it can operate on without the CPU being involved.

**Page 5, `sol:1-components.c93` / `opus:1-components.c92`:**
> The original custom chip set found in the A1000 could access the first 512K
> of RAM.

**Page 5, `sol:1-components.c101` / `opus:1-components.c100`:**
> ECS Fat Agnus (8372A) can access up to one megabyte of Chip memory.

**Page 5, `sol:1-components.c104` / `opus:1-components.c103`:**
> In the A3000, the Enhanced Chip Set can access up to two megabytes of Chip
> memory.

Table 1-1 (p.5, "Summary of Amiga Memory Configurations") corroborates all
three page-5 sentences numerically (Maximum Chip RAM column: 512K / 1 MB /
1 MB / 2 MB for the four listed configurations) but is not itself one of the
8 claims in this package's filtered inventory, so it is read as oracle context
only, not graded as its own assertion.

## What `minimig_bankmapper` is, and what it can answer

`minimig_bankmapper.v:10-23` declares the whole module: four Chip-memory
512KB-block selects in (`chip0`..`chip3`), three slow-memory block selects
(`slow0`..`slow2`), `kick`/`kick256kmirror`, a 2-bit `memory_config`, and one
8-bit `bank` output. It is purely combinational (`always @(*)`,
`minimig_bankmapper.v:28-36`) — no clock, no state. `bank[3:0]`
(`minimig_bankmapper.v:31-34`) is what this package measures: which physical
512KB bank line each logical Chip-memory 512KB block (`chip0..chip3`) is
routed onto, under each of the four `memory_config` values the RTL supports
(0 = "0.5M CHIP", 1 = "1.0M CHIP", 2 = "1.5M CHIP", 3 = "2.0M CHIP" — the
module's own comments, `:31-34`).

The claims data's own evidence already establishes the read: at
`memory_config = 0`, every one of `chip0..chip3` ORs onto the same single
bank bit — all four logical 512K blocks alias onto one physical 512K, which
is exactly "could access the first 512K" (`sol:...c93`/`opus:...c92`). At
`memory_config = 1`, `chip0`/`chip2` share one bank bit and `chip1`/`chip3`
share a second, distinct bank bit — two physical 512K banks, 1 MB, matching
the ECS Fat Agnus sentence (`sol:...c101`/`opus:...c100`). At
`memory_config = 3`, all four chip selects reach four distinct bank bits —
no aliasing at all, the full 2 MB, matching the A3000 sentence
(`sol:...c104`/`opus:...c103`). The page-3 sentence is the general form of
the same fact: as `memory_config` increases across these three points, the
number of *distinct* physical banks reachable strictly grows (1, then 2,
then 4) — that is what "the more Chip memory... the more ... it can operate
on" cashes out to at a structural boundary: growing addressable region, not
a throughput number the page never states.

`memory_config = 2` ("1.5M CHIP") appears in the RTL but is not the subject
of any of this package's 8 claims — no sentence on pages 3 or 5 names a
1.5 MB configuration — so it is not exercised by any assertion below. That
is a gap in the *claims*, not a gap this package fills by inventing a
requirement the pages do not state.

**Boundary discipline:** `slow0..slow2`, `kick` and `kick256kmirror` are held
at 0 throughout every assertion below. None of the 8 claims say anything
about slow/pseudo-fast memory or Kickstart mirroring — that is a different
sentence, on a different subject, most of it un-owned by this package's
filtered claim list — and driving them non-zero would be answering a
question these pages did not ask.

**What is not this package's fixture to build:** the classify evidence in
the claims file also cites `gary.v:147-150` (how `chip0..chip3` are actually
derived from `dma_address_in`, i.e. from a real DMA address) and
`agnus.v:67` (`address_out[20:1]`, the width of Agnus's own chip-address
port) as context for why the claims are observable at this boundary at all.
Neither `gary` nor `agnus` is this package's top; the package list fixes the
top at `minimig_bankmapper` alone, the same way the top for
`ch8-display-colour-count` was fixed at `denise_colortable` alone rather than
all of `denise`. This package drives `chip0..chip3` directly, one live at a
time, standing in for whatever DMA address gary's decode would have produced
for an access into that logical 512K block — it does not instantiate `gary`
or `agnus` to produce that stimulus itself. `gary.v:159` (the CPU-side
"first 512K of memory is Chip memory" address decode) is explicitly a
different claim assigned to a different package (`ch1-memory-map-and-two-buses`,
top `gary`) and is neither cited nor tested here.

## Existing coverage: grepped, none found

Per the build brief, `../nanomig-hrm-tests/hrm/` was grepped for `memory_config`,
`minimig_bankmapper`, `512K`, `Chip memory`, `Chip RAM`, `1 MB`/`1MB`, `2 MB`/`2MB`,
`A1000`, `Fat Agnus` and `ECS` across every existing package's test source and
Makefile before any assertion below was written. No existing package builds
`minimig_bankmapper` or varies `memory_config`. Two accepted packages do build
`gary` and touch the *same 512KB-granular vocabulary* from a different angle:

- `ch7-custom-address-space` (`test_gary.cpp`) sweeps `dma_address_in` across
  the 2 MB DMA window (one point per 512KB bank: `$000000`, `$080000`,
  `$100000`, `$1FFFFE`) and confirms every point decodes to `sel_chip`, never
  Kickstart — citing p.263's "custom chips can only access Chip memory"
  (Appendix A). That grades the *ceiling of the address port* (2 MB, fixed),
  not how many of that port's 512K blocks are *physically distinct* under a
  given `memory_config` — `memory_config` does not vary anywhere in that
  file. It does not grade any of this package's 8 claims.
- `ch7-reset-overlay` (`test_gary.cpp`) sweeps the same four 512KB points
  under reset/overlay, grading whether Chip memory is replaced by ROM at
  each — a different proposition (overlay, not banking) at a fixed,
  unstated `memory_config`.

Neither carries a `classify.note` on any of this package's 8 claims (the
claims file's own inventory of already-graded-elsewhere claims for this
package is empty), and neither varies the signal this package's claims are
about. All 8 claims are graded below, not declined.

## Assertions in this package (final list)

- **`FIXTURE-LIVE`** — with `memory_config = 3` (the configuration with the
  least aliasing), `chip0` alone and `chip1` alone must produce two distinct
  `bank[3:0]` readings, before anything else here is trusted. No claim uid;
  infrastructure. This is also this package's control case: same
  `probe_bank()` measurement basis as every graded assertion below, expected
  to (and does) PASS.
- **`sol:1-components.c40 opus:1-components.c40 MEMORY-CONFIG-MONOTONIC-REACH`**
  (p.3) — the number of pairwise-distinct `bank[3:0]` readings across
  `chip0..chip3`, one-hot, strictly increases across `memory_config` = 0, 1,
  3 (1, then 2, then 4 distinct physical banks) — the "more Chip memory...
  the more... it can operate on" claim read as the structural, monotonic
  growth of the addressable region the page states, not a throughput figure
  it does not.
- **`sol:1-components.c93 opus:1-components.c92 A1000-512K-MIRROR`** (p.5) —
  at `memory_config = 0`, all four one-hot `chip0..chip3` probes produce the
  identical `bank[3:0]` value (and that value is the "first" physical bank,
  bit 0 — the same bank `chip0` alone reaches at `memory_config = 3` below),
  i.e. the full 2 MB logical window collapses onto one physical 512K bank.
- **`sol:1-components.c101 opus:1-components.c100 ECS-FATAGNUS-1MB-TWO-BANKS`**
  (p.5) — at `memory_config = 1`, `chip0`/`chip2` produce one `bank[3:0]`
  value, `chip1`/`chip3` produce a second, distinct value — exactly two
  physical 512K banks, 1 MB.
- **`sol:1-components.c104 opus:1-components.c103 A3000-ECS-2MB-FOUR-BANKS`**
  (p.5) — at `memory_config = 3`, `chip0..chip3` each produce their own
  distinct `bank[3:0]` value (bit-for-bit: `chip0`→1, `chip1`→2, `chip2`→4,
  `chip3`→8) — four physical 512K banks with no aliasing at all, the full
  2 MB.

All 8 claims are graded with `expect()` under their own uids; none appears
on a `record()` line.
