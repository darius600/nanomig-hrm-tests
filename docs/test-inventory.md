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

### 1. Four Copper tests predate the pipeline

`agnus_copper`, `agnus_copper_move`, `agnus_copper_skip`, `agnus_copper_wait`
were written on 18.08.2026, before the pipeline existed. They sit in chapter 2's
subject area but not in layer A, which makes "chapter 2 is covered" easy to
misread.

They are table-driven and report a single collective PASS at the end rather than
one result per proposition. Their subject matter overlaps
`agnus_copper_waitskip_range` (43 assertions), `agnus_copper_move_range`,
`agnus_copper_move_sweep` and `agnus_copper_locreg_width` — but *overlaps* is an
impression from reading the headers, not a measurement. Each needs checking
proposition by proposition before it is retired or kept.

`agnus_copper_move` additionally declares its oracle as the **frozen FPGA-014/017
specification, not the manual**. It is a regression test against the
implementation and says so; it should not be counted as conformance coverage
under any circumstances.

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
