# Analyses behind the findings

Work that produced or tested a **group** of findings and belongs to none of
them alone. It was in [`findings.md`](findings.md) and made that file hard to
read as a register: a reader scanning for a key met `NM-HRM-005` five times in
three different tables.

Nothing here is a status. The register is [`findings.md`](findings.md), and the
maintained statement of any single finding is its page in
[`findings/`](findings/).

## Where the five oldest findings come from, and whether they hold


Five findings, but not of equal standing. Three came out of the pipeline and are
witnessed more than once. Two came out of the earlier hand-written run, rest on a
single citation each, and both of those citations turn out to be contradicted
elsewhere in the same manual.

| Finding | Origin | Standing |
|---|---|---|
| [NM-HRM-003](findings/NM-HRM-003.md) | pipeline | corroborated — p273 and p309 give two different widths |
| [NM-HRM-004](findings/NM-HRM-004.md) | pipeline | corroborated — two editions carry the appendix C sentence; challenged and upheld |
| [NM-HRM-005](findings/NM-HRM-005.md) | pipeline | corroborated — p20 and p275, units settled on p275 itself |
| [NM-HRM-002](findings/NM-HRM-002.md) | hand-written, Aug | **stands** — the manual contradicts itself, and four implementations split two against two |
| [NM-HRM-001](findings/NM-HRM-001.md) | hand-written, Aug | **withdrawn** — the manual sanctions the measured value |

Both August findings rested on a manual that says two different things, and
neither could be decided by reading the manual harder. They were settled by a
third witness — see [`third-witness.md`](third-witness.md) — and they went
opposite ways: one survived, one did not.

## NM-HRM-001 is withdrawn — and why the entry stays

The assertion was removed rather than left red. A red test is a claim that the
RTL is wrong, and this one could not support that claim.

The *file* was not deleted: `agnus_beamcounter` still asserts that a PAL field
terminates with EOF and that the horizontal counter spans 0..453. The second one
matters beyond this file — `agnus_copper_waitskip_range`'s W4b rests on the line
being 454 units long.

It was not replaced with a corrected assertion either. Doing that properly means
driving the long-frame bit deliberately and checking both cases, which is work
nobody has done, and an assertion guessing at it would be worse than none.

The reasoning:

## Chapter 6 — the ECS blitter registers

Appendix C lists five ECS changes to the blitter. `agnus_blitter` takes an `ecs`
input and uses it for two of them. Three are findings and they all live in
`hrm/agnus_blitter_ecs`, each required to fail:

| Assertion | Measured |
|---|---|
| `BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS` | a BLTSIZV write with `ecs=0` and with `ecs=1` gives the identical height, r1=7 and r2=7 |
| `DOFF-NO-OBSERVABLE-EFFECT` | the same D write appears with BLTCON1 bit 7 clear and set |
| `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` | `BLTAPTH` written with `$001F` gives `address_out = 0xF8000` at both `ecs` settings |

Each names every claim it covers in its own message, so the red line is readable
without this file. The two remaining rows of the five-way table below are not
defects: BLTCON0L and BLTSIZH are gated correctly and pass.

### Appendix C's five ECS blitter changes are implemented five different ways

Appendix C, printed page 309, lists what ECS changed in the blitter. NanoMig's
`agnus_blitter` takes an `ecs` input. It uses it for two of the five:

| Appendix C | `agnus_blitter.v` | Gated by `ecs`? |
|---|---|---|
| `BLTCON0L` $05A new — control 0, lower 8 bits | line 259 | **yes** |
| `BLTSIZH` $05E new — H size and start | lines 500, 523, 557 | **yes** |
| `BLTSIZV` $05C new — V size, 15-bit | line 548 | **no** — and the comment on that line calls it ECS |
| `BLTxPTH` $050 chg — "high 5 bits, was 3 bits" | `agnus_blitter_adrgen.v:45`, `reg [20:16] bltpth [3:0]` | **no** — and now measured at the parent, see below |
| `DOFF`, BLTCON1 bit 7 — disable D output in line mode | nowhere | **not implemented** |

`bltcon1[7]` does not appear in `agnus_blitter.v`. The bits it reads are 15-12,
4, 3, 2, 1 and 0.

#### BLTxPTH, measured at the boundary that has the `ecs` port

The row above used to read "no — that module has no `ecs` port at all", which was
true and too weak. `agnus_blitter_adrgen` having no `ecs` input explains why the
gating cannot happen *there*; it leaves open whether the parent, which does take
`ecs`, gates it before handing the write down. It does not, and that is now
measured rather than inferred.

`hrm/agnus_blitter_ecs`'s `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` writes `BLTAPTH` with
`$001F` — all five low bits — twice on the same fixture, once with `ecs` low and
once with `ecs` high, and reads `agnus_blitter`'s own `address_out`:

    ecs=0 -> 0xF8000
    ecs=1 -> 0xF8000

Identical. All five bits are taken in both chip sets, so appendix A p.270's
"high 3 bits, 5 bits if ECS" is not implemented in either direction: an OCS
machine gets ECS width, and `ecs` changes nothing about the pointer.

The path is unbroken in the source. `agnus_blitter.v:588-592` instantiates the
address generator passing `data_in`, `reg_address_in` and `address_out` and **no
`ecs`**; the name does not occur anywhere in `agnus_blitter_adrgen.v`; and
`agnus_blitter_adrgen.v:62` is
`assign bltptr_in[20:1] = enaptr ? newptr[20:1] : {data_in[4:0], data_in[15:1]}`
— five bits, unconditionally.

**The control matters here.** A red that depends on toggling `ecs` is worthless if
the fixture's `ecs` toggle does nothing, so the assertion names
`BLTCON0L-GATED-BY-ECS` — green in the same file, on the same harness — as proof
that this fixture's `ecs` does change behaviour elsewhere. And a mutation confirms
the observable is live rather than stuck: driving `$0015` instead of `$001F` moves
both readings to `0xA8000`, still identical.

This finding exists because Prompt 8's independent validation rejected the earlier
handling. The three claims behind it — `6-appa-registers.c110`,
`ch6-appa-bltxpt.c2`, `ch6-appa-bltxpt.c6` — had been routed to `PKG-BLT-ADRGEN`,
one fixture below the `ecs` port, where the builder correctly declined them as not
constructible. The validator ruled that they needed real assertions at the parent
rather than relocated citations. They did, and the assertion came out red.

#### The same deviation, in both Agnus engines

The blitter measurement above sits next to NM-HRM-003 in this document, and the
two are worth reading together. NM-HRM-003 is the copper's version of exactly this finding: "OCS uses
five high address bits; the manual documents three." The blitter now shows the
same thing — and not merely something similar. The two pointers are built the same
way, from the same bits, with no chipset condition on either:

    agnus_copper.v:144    cop1lch[20:16] <= data_in[4:0];
    agnus_copper.v:197    address_out[20:1] <= {cop1lch[20:16], cop1lcl[15:1]};

    agnus_blitter_adrgen.v:62
        assign bltptr_in[20:1] = enaptr ? newptr[20:1]
                                        : {data_in[4:0], data_in[15:1]};

Five high bits from `data_in[4:0]`, fifteen low from `data_in[15:1]`, a 20-bit
word address, unconditional. Both engines take the ECS width in both chipsets, and
in both the manual documents three high bits for OCS and five for ECS.

That reframes what the two findings are. Separately they read as two omissions.
Together they look like one design decision applied consistently across Agnus:
this implementation does not model the OCS pointer narrowing anywhere. Whether
that is a deliberate simplification or an oversight is not something these tests
can answer — but it is a question about the whole chip, not about a register, and
neither the copper chapter nor the blitter chapter could have raised it alone.

Recorded rather than acted on. The copper half is NM-HRM-003 and is witnessed
more than once; the blitter half is NM-HRM-022. Whether one
decision closes both is the question this cross-reference exists to raise.

### The classified inventory says otherwise, in 109 records

Worth recording because it is the one place a wrong statement could have made
these findings disappear. **109 of the chapter-6 records carry an `evidence`
field asserting that `ecs` gates BLTSIZV** — wording like "`ecs` ... gates the
BLTCON0L, BLTSIZH and BLTSIZV decodes `agnus_blitter.v:500,523,557`". Those three
line numbers are BLTSIZH's and BLTCON0L's, and they are correct for those two.
BLTSIZV's decode is at line 548 and has no `&& ecs`:

```verilog
    else if (reg_address_in[8:1]==BLTSIZV[8:1]) // ECS BLTSIZV register
        height[14:0] <= data_in[14:0];
```

The comment on that line calls it ECS; the code does not gate it. The
classifier's evidence text generalised from the two registers that are gated to
the third that is not, and repeated it 109 times.

The **claims** are unaffected — appendix C does mark BLTSIZV "new" for ECS, and
the classification as testable is right. It is the stated reason that
misdescribes the RTL. A builder who had trusted it would have written an
assertion expecting a difference, seen none, and had no way to tell a defect from
its own mistake. PKG-BLT-ECS's builder read line 548 instead and sided with the
RTL, which is what the build brief's "if the brief and the document disagree, the
document wins — and if the RTL and both disagree, say so" rule is for.

The pointer row is the sibling of NM-HRM-003, one appendix over: the same register
family, the same "three bits in OCS, five in ECS" sentence, and the same
unconditional five bits in the RTL. The third-witness work already recorded that
neither vAmiga nor WinUAE narrows a pointer by chipset at all, which is worth
knowing before anyone calls the blitter case a defect on its own.

What this is *not* is a claim that any of the five is wrong. A design that is
always ECS/AGA has a defensible reason to widen a pointer unconditionally. What
it is: the same appendix, the same five lines, handled four different ways, with
no comment anywhere saying why the two ungated ones are ungated.

### `agnus_top_dmacon` asserts nothing

Found in the same pass and recorded where it belongs, in
[`test-inventory.md`](test-inventory.md), backlog item 5. It is the only test in
the suite that cannot fail.

## What the emulators say about the three Copper findings

Read out of interest rather than necessity — these three are not
contested, so an emulator cannot cancel them. The result is worth knowing anyway:

**On all three, WinUAE and vAmiga do what NanoMig does and not what the manual
says.**

| Finding | Manual | Both emulators, and NanoMig |
|---|---|---|
| [NM-HRM-003](findings/NM-HRM-003.md) | OCS keeps three high pointer bits | neither emulator narrows the pointer by chipset at all |
| [NM-HRM-004](findings/NM-HRM-004.md) | ECS with CDANG clear reaches $03E–$07E | CDANG clear blocks below $80 in both, chipset-independent |
| [NM-HRM-005](findings/NM-HRM-005.md) | MOVE 2, SKIP 4, satisfied WAIT 6 | 4 / 8 / 8 in vAmiga, the same three numbers NanoMig measures |

The citations are in [`third-witness.md`](third-witness.md).

This does not close any of them, and none of the assertions was touched. It
changes the question: not "is the RTL wrong" but "is the manual wrong about the
hardware". Two independent implementations agreeing against a manual sentence is
strong evidence, but choosing what NanoMig should follow — the manual it is
tested against, or the hardware it emulates — is a decision, not a measurement.

NM-HRM-004 is the sharpest case. Appendix C read literally says that *clearing* the
danger bit grants the range that *setting* it grants, which would leave the bit
with nothing to do.
