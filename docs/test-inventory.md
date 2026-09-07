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

### 2. Eight blitter tests are prior art for chapter 6

Chapter 6 is the next chapter due through the pipeline. The eight
`agnus_blitter_*` tests already cover parts of it from the manual. When the
pipeline runs, they are claims to reconcile with — the pipeline either confirms
each one or shows what it leaves out. They are not to be replaced unread, and
they are not to be counted as coverage in advance.

### 3. Layer C is the blitter set — same item as 2

Layer C and the chapter-6 prior art are the same eight files, so there is one
piece of work here, not two. Five of them state their oracle nowhere; giving them
headers is worth doing as part of the chapter-6 run rather than before it, since
the pipeline will have to establish what each one measures against anyway.

Nothing outside the blitter set is missing a header.

## Method note

Layer membership is derived mechanically, not by opinion: layer A is "contains
`// MUTATION` records", layer B is "declares both `Test class:` and
`Primary oracle:`", layer C is neither. Re-deriving it after changes means
grepping for those three markers **and printing the names** — an earlier revision
of this file took the counts from a script and then wrote the name lists from
memory, which put a test into layer C that declares its class in line two.
