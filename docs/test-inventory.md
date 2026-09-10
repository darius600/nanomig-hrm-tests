# Test inventory and review backlog

The suite grew in two phases, and a green result means different things in each.
This file says which test belongs to which, so nobody reads more into a pass than
it carries.

## Three layers

Re-measured 10.09.2026, after chapter 6 closed and the five superseded tests were deleted.

| Layer | Tests | What is established |
|---|---:|---|
| **A — through the pipeline** | 28 | class declared, oracle named, page cited, and **stimulus mutations run**: each assertion was shown to bind to the behaviour it claims |
| **B — hand-written, declared** | 22 | class declared, oracle named, explicit non-goals — but no mutation evidence and no completeness claim. **One of them asserts nothing at all** — see backlog item 5 |
| **C — no header in the source** | **0** | neither test class nor primary oracle in the `.cpp` |

Layer B is not sloppy work. Those tests fix the expected behaviour from the
manual before looking at the RTL, cite their pages, and state what they do *not*
prove. What they lack is the pipeline's evidence that each assertion would go red
if the behaviour changed, and any claim that the manual's statements about that
module are covered.

### Layer A — 28

Chapter 2's twelve Copper tests, and sixteen from chapter 6's build stage:

```
agnus_copper_bus_behaviour   agnus_copper_control      agnus_copper_cycle_times
agnus_copper_cycles          agnus_copper_list         agnus_copper_locreg_width
agnus_copper_move_range      agnus_copper_move_src     agnus_copper_move_sweep
agnus_copper_rejected_move   agnus_copper_selfmod      agnus_copper_waitskip_range

agnus_blitter_adrgen         agnus_blitter_barrelshifter  agnus_blitter_bltcon
agnus_blitter_cycles         agnus_blitter_desc           agnus_blitter_ecs
agnus_blitter_fill           agnus_blitter_line           agnus_blitter_masks
agnus_blitter_minterm        agnus_blitter_regdecode      agnus_blitter_shift_chain
agnus_blitter_size           agnus_blitter_status         agnus_slots
agnus_top_dmacon
```

Three of the chapter-6 sixteen are the *prior-art* files raised into layer A
rather than new directories — `barrelshifter`, `fill` and `minterm` kept their
exhaustive sweeps and gained a header, page citations and mutation records. The
other thirteen are new fixtures that compile the design.

`agnus_top_dmacon` is in this list for a different reason than the rest: its
directory existed before chapter 6, but as a stub whose testbench asserted
nothing. It was replaced rather than extended.

### Layer C — empty

```
(none)
```

**This layer is gone, and its emptying was predicted rather than arranged.** The
previous revision of this file said "when the last of them goes, layer C is
empty", listing the five that remained: `line_octant`, `mask`, `modulo`, `ptr`
and `zero`. All five were deleted on 10.09.2026 (`b0cfb1e` and `19b2354`), each
only after its replacement had been re-run green from a clean build.

That coincidence was not luck. Layer C was defined as "no header", and the reason
those five had none is the reason they were superseded: three compiled a local
copy of RTL rather than the design, and two asserted uncited expectations. The
measurement and the supersession were finding the same thing from two directions
— and the set emptied exactly when the supersessions completed, without anyone
deleting a file to make a count come out.

**No local `.v` copy remains anywhere under `hrm/`** — checked, not assumed. Every
test in the suite now compiles the design.

An earlier revision of this file listed the DMA and Denise tests in layer C
instead. That list was inferred from a group count rather than measured, and it
was wrong: `agnus_top_register_bus`, named among them, declares
`INTEGRATION_SMOKE` in line two of its source. The lists above are produced by the
method note's own rule, with the names printed, which is what that note demands.

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
| `agnus_top_dmacon` | none | `INTEGRATION_SMOKE`; cites p.222 only for DMACON bit meaning — **and asserts nothing, see item 5** |

So one further check was needed and it survived. The remaining eighteen layer-B
tests make no chapter-2 claim and are left for their own chapters.

### 2b. What the chapter-6 build has settled so far — 09.09.2026

Eight of the sixteen chapter-6 packages are built, and they answer most of item 2.
Each was reviewed by rebuilding it, deriving its claim coverage from the inventory
rather than adopting the builder's count, and mutating the **RTL** in a scratch
copy to confirm the test can fail:

| Package | Test | Claims | Mutation that turned it red |
|---|---|---:|---|
| PKG-BLT-STATUS | `agnus_blitter_status` | 28 | `zero` clear condition disabled → 5 of 17 red |
| PKG-BLT-FILL | `agnus_blitter_fill` | 51 | inclusive fill loses its OR → 4 of 13 red |
| PKG-BLT-ADRGEN | `agnus_blitter_adrgen` | 92 | pointer increments by 2 units → 3 of 16 red |
| PKG-BLT-MINTERM | `agnus_blitter_minterm` | 182 | `lf[2]`/`lf[4]` swapped → 25 of 56 red |
| PKG-BLT-SHIFT | `agnus_blitter_barrelshifter` | 24 | `desc` inverted → sweep fails on case 1 |
| PKG-BLT-SHIFT-CHAIN | `agnus_blitter_shift_chain` | 35 | `bltaold` cleared every word → 2 of 11 red |
| PKG-BLT-MASKS | `agnus_blitter_masks` | 74 | first word takes BLTALWM → 8 of 19 red |
| PKG-BLT-LINE | `agnus_blitter_line` | 225 | in progress |

Defect 1 from [blitter-prior-art.md](blitter-prior-art.md) is answered by the
supersessions: `mask`, `zero` and `line_octant` compiled a local copy of the RTL,
and their replacements compile the design. Defect 3 is answered — `fill`'s worked
examples now carry pp.184-185. Defect 2 is `agnus_blitter_line`'s to fix and is
the reason that package must carry **both** probes, `ash = 15` against `lineinc`
and `ash = 0` against `linedec`.

Three of the eight builders found a **vacuous mutation** in their own work — a
stimulus change that left the assertion green because the mutated value
coincidentally matched the expected one — and each recorded it rather than
quietly replacing it. That is the mutation records doing the job they exist for.

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

### 5. `agnus_top_dmacon` cannot fail — found 08.09.2026

It builds the whole of `agnus` under Verilator, releases reset, sets up an
address and a data word for a DMACON write, and then stops:

```cpp
    // Need selreg pulse
    // TODO: The testbench needs to hook into the bus appropriately

    std::cout << "PASS: Agnus Top-Level tests passed.\n";
```

It never pulses the register strobe, never reads an output, and prints PASS
unconditionally. `return 0;` is the only return statement in the file. Nothing
the RTL does can make it red, so its green has been worth nothing since it was
written — and it has been counted in every suite result.

It is the only one. Every other test in the suite has a failure path:
`return 1`, `return pass ? 0 : 1`, `return fail_count == 0 ? 0 : 1`, or
`return checks.failed == 0 ? 0 : 1`. That was checked by listing every return
statement in all 43 sources, not by reading a sample.

Two things follow, and they pull in opposite directions:

- **It is not evidence and must stop being counted as any.** Until it asserts
  something, the honest suite figure is one lower than the runner prints.
- **It is still useful.** It is proof that an `agnus`-level fixture builds and
  runs, which is exactly what the 82 discarded chapter-6 DMACON and DMACONR
  claims were discarded for lacking. Finishing this file is the cheapest way to
  find out whether that discard reason holds.

It was found by the chapter-6 classification stage while looking for a fixture
boundary — not by anything in this repository. Nothing here checks that a test
can fail; that is what the layer-A mutation records are for, and this test has
none because it is layer B.

## Method note

Layer membership is derived mechanically, not by opinion: layer A is "contains
`// MUTATION` records", layer B is "declares both `Test class:` and
`Primary oracle:`", layer C is neither. Re-deriving it after changes means
grepping for those three markers **and printing the names** — an earlier revision
of this file took the counts from a script and then wrote the name lists from
memory, which put a test into layer C that declares its class in line two.
