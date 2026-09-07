# Test inventory and review backlog

The suite grew in two phases, and a green result means different things in each.
This file says which test belongs to which, so nobody reads more into a pass than
it carries.

## Three layers

| Layer | Tests | What is established |
|---|---:|---|
| **A — through the pipeline** | 12 | class declared, oracle named, page cited, and **stimulus mutations run**: each assertion was shown to bind to the behaviour it claims |
| **B — hand-written, declared** | 26 | class declared, oracle named, explicit non-goals — but no mutation evidence and no completeness claim |
| **C — hand-written, undeclared** | 8 | neither test class nor primary oracle in the source header |

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

### Layer C — no declared class or oracle

```
agnus_audiodma   agnus_bitplanedma   agnus_diskdma   agnus_refresh
agnus_spritedma  agnus_top_register_bus
denise_bitplane_shifter                denise_colortable
```

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

### 2. Eight blitter tests are prior art for chapter 6

Chapter 6 is the next chapter due through the pipeline. The eight
`agnus_blitter_*` tests already cover parts of it from the manual. When the
pipeline runs, they are claims to reconcile with — the pipeline either confirms
each one or shows what it leaves out. They are not to be replaced unread, and
they are not to be counted as coverage in advance.

### 3. Layer C needs headers

The eight tests without a declared class or oracle cannot be judged from their
source at all. Deciding what each one actually tests against is the smallest of
these three items and the prerequisite for the other two.

## Method note

Layer membership here is derived mechanically, not by opinion: layer A is
"contains `// MUTATION` records", layer B is "declares `Test class:` and
`Primary oracle:`", layer C is neither. Re-deriving it after changes is a matter
of grepping for those three markers.
