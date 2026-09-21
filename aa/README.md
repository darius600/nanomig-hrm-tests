# AA conformance packages

The second oracle: Commodore's *Functional Specification for the Advanced Amiga
Chip Set (AA)*, 06/07/91, PRELIMINARY. 31 pages.

These are not in `hrm/` and not in the 208. `hrm/` is the Hardware Reference
Manual's suite, `run_all.sh` and `check_published_counts.py` both count the
directories there, and a second oracle's packages would silently change a
published figure that means something else.

```
wsl
cd /mnt/c/code4nanomig/code/nanomig-hrm-tests/aa/aa-clxcon2-address
make test      # 3 passed, 0 failed
make mutate    # the address assertion goes red
```

| package | what it grades | finding |
|---|---|---|
| `aa-clxcon2-address` | where CLXCON2 is decoded, and that a CLXCON write clears it | [NM-AA-001](../docs/findings/NM-AA-001.md) |

## Where the claims come from

`hrm-testpipeline/docs/references/amiga-aa-1991/` holds the corpus — 31 pages
merged from three readings, 1134 load-bearing lines of which 1130 are
corroborated by two or more. `hrm-testpipeline/docs/aa-text-extraction.md` says
how it was built and what is wrong with it.

The specification is **PRELIMINARY and contradicts itself**, which is why the
first package grades a contradiction rather than a behaviour. It is never
adjusted to make a test pass.
