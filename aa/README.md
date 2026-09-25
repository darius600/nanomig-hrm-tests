# AA conformance packages

The second oracle: Commodore's *Functional Specification for the Advanced Amiga
Chip Set (AA)*, 06/07/91, PRELIMINARY. 31 pages.

These packages sit apart from `hrm/`, which is the Hardware Reference Manual's
suite, because each oracle's figures are counted on their own: `run_all.sh`
prints one summary line for `hrm/` and one for `aa/`. The root
[`README.md`](../README.md#where-the-suite-stands) carries the current package
and assertion counts for both.

Run one package from the repository root:

```sh
make -C aa/aa-clxcon2-address test      # the graded assertions
make -C aa/aa-clxcon2-address mutate    # the address assertion goes red
```

The AA findings have a register of their own,
[The AA register](../docs/findings.md#the-aa-register), and one page each
under [`docs/findings/`](../docs/findings/) (`NM-AA-nnn`).

## Where the claims come from

`hrm-testpipeline/docs/references/amiga-aa-1991/` holds the corpus — 31 pages
merged from three readings, 1134 load-bearing lines of which 1130 are
corroborated by two or more. `hrm-testpipeline/docs/aa-text-extraction.md` says
how it was built and what is wrong with it.

The specification is **PRELIMINARY and contradicts itself**, which is why some
packages grade a contradiction rather than a behaviour:
`aa-clxcon2-address` does, and [NM-AA-001](../docs/findings/NM-AA-001.md) is
filed against the printed page. The specification is never adjusted to make a
test pass.
