# The shortest package that is still a real one

It builds, it grades one assertion, and `make mutate` turns it red.

```
cd docs/example-package   # from the repository root; on Windows, inside WSL
make test      # PASS: DDRA is 0x00 after reset, so port A is all inputs
make mutate    # FAIL: the same line -- proving the assertion can go red
```

Why it exists: the complete worked example,
[`hrm/ch3-beam-counters-and-blanking`](../../hrm/ch3-beam-counters-and-blanking), runs to
some two thousand lines; this one is the shape, in about a hundred lines. The rules every
package follows are the pipeline's build rules,
[`docs/playbook/build.md`](https://github.com/darius600/hrm-testpipeline/blob/main/docs/playbook/build.md).

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

It is not in `hrm/`, so it is not in the suite. The repository-root
`./run_all.sh` prints one line for each package in `hrm/` and `aa/`, one for
each check of the test harness and the smoke test in `integration/`, one per
CPU core for each graded `integration/` package, and one `SPEC` line for each
`integration/` package that has a specification and no test yet; this example
is in none of them.
