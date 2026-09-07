# Test inventory and review backlog

The suite grew in two phases, and a green result means different things in each.
This file says which test belongs to which, so nobody reads more into a pass than
it carries.

## Three layers

| Layer | Tests | What is established |
|---|---:|---|
| **A — through the pipeline** | 12 | class declared, oracle named, page cited, and **stimulus mutations run**: each assertion was shown to bind to the behaviour it claims |
| **B — hand-written, declared** | 23 | class declared, oracle named, explicit non-goals — but no mutation evidence and no completeness claim |
| **C — no header in the source** | 8 | neither test class nor primary oracle in the `.cpp`; layer C is exactly the eight blitter tests |

Layer B is not sloppy work. Those tests fix the expected behaviour from the
manual before looking at the RTL, cite their pages, and state what they do *not*
prove; `agnus_blitter_fill` walks all 262,144 input combinations. What they lack
is the pipeline's evidence that each assertion would go red if the behaviour
changed, and any claim that the manual's statements about that module are
covered.

### Layer A — chapter 2, through the pipeline

```
agnus_copper_bus_behaviour   agnus_copper_control      agnus_copper_cycle_times
agnus_copper_cycles          agnus_copper_list         agnus_copper_locreg_width
agnus_copper_move_range      agnus_copper_move_src     agnus_copper_move_sweep
agnus_copper_rejected_move   agnus_copper_selfmod      agnus_copper_waitskip_range
```

### Layer C — no header in the source

```
agnus_blitter_barrelshifter  agnus_blitter_fill    agnus_blitter_line_octant
agnus_blitter_mask           agnus_blitter_minterm agnus_blitter_modulo
agnus_blitter_ptr            agnus_blitter_zero
```

Layer C turns out to be exactly the blitter set, and it is less bad than "no
header" suggests: three of the eight carry a `README.md` beside the source that
names the manual pages — `agnus_blitter_barrelshifter` (179, 182, 267),
`agnus_blitter_fill` (184, 185, 267), `agnus_blitter_minterm` (174, 175, 177,
178). The other five state their oracle nowhere at all:
`agnus_blitter_line_octant`, `_mask`, `_modulo`, `_ptr`, `_zero`.

An earlier revision of this file listed the DMA and Denise tests here instead.
That list was inferred from a group count rather than measured, and it was wrong:
`agnus_top_register_bus`, named among them, declares `INTEGRATION_SMOKE` in line
two of its source. The lists above are produced by the script in the method note.

## The review backlog

Nothing here is known to be wrong. It is work that has not been done, written
down so that it is not mistaken for work that has.

### 1. Four Copper tests predate the pipeline — three reviewed 07.09.2026

`agnus_copper`, `agnus_copper_move`, `agnus_copper_skip`, `agnus_copper_wait`
were written on 18.08.2026, before the pipeline existed. Three of them have now
been measured case by case against the layer-A tests rather than judged by
reading.

**The measurement found a real gap.** Of the six cases in `agnus_copper_skip` and
`agnus_copper_wait`, five were already covered by
`agnus_copper_waitskip_range` and covered more strictly — but one was not covered
at all: **a SKIP with BFD=0 while the blitter is busy.** BFD lives in IR2, which
WAIT and SKIP share, so the blitter term belongs to both conditions; the pipeline
test asserted it only for WAIT (B1–B3). It is now asserted for SKIP as **B2s**,
with a stimulus mutation, bringing that file to 44 assertions.

Five of six is exactly what an impression would have called "redundant".

| Test | Outcome |
|---|---|
| `agnus_copper_wait` | superseded in all three cases, each more strictly |
| `agnus_copper_skip` | superseded once B2s closed the BFD gap |
| `agnus_copper` | superseded as conformance; reclassified `INTEGRATION_SMOKE`, since it is the only test that runs WAIT → MOVE → SKIP as one continuous list |
| `agnus_copper_move` | **not reviewed.** Its oracle is the frozen FPGA-014/017 specification, not the manual — a regression test against the implementation. It must never count as conformance coverage. |

All four are kept. They are independent second implementations of checks that now
exist in stricter form; none of them is counted as coverage, and each says so in
its own header.

### 2. Eight blitter tests are prior art for chapter 6 — read 07.09.2026

Chapter 6 is in the pipeline now. The eight `agnus_blitter_*` tests already cover
parts of it from the manual. They are claims to reconcile with — the pipeline
either confirms each one or shows what it leaves out. They are not to be replaced
unread, and they are not counted as coverage in advance.

All eight have now been read file by file, before the pipeline's package list
exists, so the list meets something measured rather than remembered. The audit is
[blitter-prior-art.md](blitter-prior-art.md). It found three defects:

1. `line_octant`, `mask` and `zero` compile a **local copy** of Verilog cut from
   `agnus_blitter.v`, not the design. All three copies are still verbatim as of
   07.09.2026, but a change to NanoMig cannot make those tests go red.
2. Four assertions in `line_octant` **cannot fail** — they pick a shift value that
   makes the outputs they check structurally impossible. Shown by mutation: the
   `!sign_del` guard was removed from `lineinc` and the test stayed green.
3. `fill`'s four hardcoded "HRM examples" are subsumed by its own exhaustive
   sweep and cite no page.

The fixes belong to the chapter-6 run, not before it: repairing `line_octant` now
would mean re-deriving the octant encoding from the RTL, which is how the defect
got there.

### 3. Layer B, chapter 2 — checked 07.09.2026, one contested

Layer B rests on single citations, which is how FPGA-050 and FPGA-051 came to be
recorded as RTL defects when the manual contradicts itself on both. The same
check was applied to the layer-B tests that touch chapter 2. They were found
mechanically — Copper mentioned in the source, `agnus_copper.v` pulled into the
build, or chapter 2 / pages 19–36 / 273–276 cited in the header — not by name.

| Test | Chapter-2 claim | Result |
|---|---|---|
| `agnus_beamcounter` | PAL is 312 lines | **contested** — Table 7-5 sanctions 313 (see findings.md) |
| `paula_intcontroller` | INTREQ bit 4 is the Copper interrupt; VERTB outranks it | **holds** — both editions give "Bit 4, COPER, level 3"; the priority table puts COPER at 5 and VERTB at 6 on the same hardware level |
| `agnus_copper_move` | none | oracle is a frozen specification, not the manual |
| `agnus_top_register_bus` | none | `INTEGRATION_SMOKE`, oracle is the bus contract and a frozen spec |
| `agnus_top_dmacon` | none | `INTEGRATION_SMOKE`; cites p.222 only for DMACON bit meaning |

So one further check was needed and it survived. The remaining eighteen layer-B
tests make no chapter-2 claim and are left for their own chapters.

### 4. Layer C is the blitter set — same item as 2

Layer C and the chapter-6 prior art are the same eight files, so there is one
piece of work here, not two. Five of them state their oracle nowhere; giving them
headers is worth doing as part of the chapter-6 run rather than before it, since
the pipeline will have to establish what each one measures against anyway. Two of
the five assert something specific enough for the pipeline to find the page —
`modulo` reads the modulo register as a signed byte offset applied to a word
address, and `zero` claims `BZERO` is never restored once cleared. Neither says
where the manual states it.

Nothing outside the blitter set is missing a header.

## Method note

Layer membership is derived mechanically, not by opinion: layer A is "contains
`// MUTATION` records", layer B is "declares both `Test class:` and
`Primary oracle:`", layer C is neither. Re-deriving it after changes means
grepping for those three markers **and printing the names** — an earlier revision
of this file took the counts from a script and then wrote the name lists from
memory, which put a test into layer C that declares its class in line two.
