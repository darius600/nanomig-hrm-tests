# The shortest package that is still a real one

It builds, it grades one assertion, and `make mutate` turns it red.

```
cd docs/example-package   # from the repository root; on Windows, inside WSL
make test      # PASS: DDRA is 0x00 after reset, so port A is all inputs
make mutate    # FAIL: the same line -- proving the assertion can go red
```

Why it exists: [`adding-a-test.md`](../adding-a-test.md) used to send a first-time
reader to a 1,749-line worked example. That file is still the reference for
every rule; this one is the shape, in about a hundred lines.

What to look at, in order:

1. **The header.** Test purpose, test class, primary oracle, non-goals. A
   package is judged on those four before anything else.
2. **The three verbs.** `expect()` grades and is the only thing that counts as
   an assertion; `note()` prints `MEASURED:` and records what the RTL did
   without judging it; `record()` prints `RECORDED:` and states something the
   test knows but does not measure.
3. **The mutation guard and its record.** Every `expect()` in a real package
   is written so that `MUTATE=1` can force it red, by changing what the module
   is asked to do and never the expected value, and the header records what
   that did as `// MUTATION <id> stimulus red: ...`. An assertion that cannot
   fail measures nothing, and the mutation gate exists to catch one.
4. **Driving every input.** A Verilated model starts undefined, not at zero.

It is not in `hrm/`, so it is not in the suite and not in the 208.

## The 210-versus-208 question, answered here because it comes up

`run_all.sh` prints 210 PASS/FAIL lines and the README says 208 packages. Both
are right: 208 directories under `hrm/`, plus two integration targets
(`test_minimig_system`, `test_copper`) that the run script builds from its own
second section. This example is a third thing again — it is not in either count.
