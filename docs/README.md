# What is in here, and what each document is for

Four documents besides this map, and three folders. A red test is not
self-explanatory: someone reading `FAIL` needs to know what the printed page
says, what the RTL does, whether anyone else agrees, and who decides. The first
three rows below answer that.

## Start here

| | |
|---|---|
| [`findings.md`](findings.md) | **The register.** The register tables of all **71** open findings — key, subject, status, package, third witness. 66 are against the Hardware Reference Manual; the 5 against the AA specification sit in a section of their own, so neither count has to be read twice. |
| [`findings/`](findings/) | One page per finding, readable without anything outside this repository: what the printed page says, what NanoMig does and how it was measured, what the other implementations do, and what is asked of you. [`findings/README.md`](findings/README.md) says how a page is built and why NanoMig's two Minimig ancestors are listed but not counted as witnesses. Withdrawn findings are kept apart, with their own list, in [`findings/withdrawn/`](findings/withdrawn/). |
| [`rtl-review.md`](rtl-review.md) | Every finding that asks an Amiga specialist to review the RTL, grouped by the module it is about, so a reader who knows one part of the chip set can start with their own. |
| [`adding-a-test.md`](adding-a-test.md) | **Binding for contributors**: what a finished package has and what a reviewer checks. [`example-package/`](example-package/) is the shortest package that is still a real one. |

## Look things up

| | |
|---|---|
| [`legacy-names.md`](legacy-names.md) | Resolves identifiers you may meet and cannot place: the `sol:`/`opus:` claim-id prefixes, `FPGA-nnn` task numbers, `work/…` claim files, and old package names that `//` comments in tests still cite. |

## The evidence behind the findings

Eight documents of evidence, in [`evidence/`](evidence/). Every finding page
already states its conclusion and what the other implementations do; these hold
the comparisons and reasoning those statements rest on. None is needed to read a
finding.

| | |
|---|---|
| [`third-witness.md`](evidence/third-witness.md) | The rule: an emulator is consulted only to decide between two sentences of the manual that disagree with each other, never as an oracle. |
| [`third-witness-ch3-ch6.md`](evidence/third-witness-ch3-ch6.md) | WinUAE and vAmiga read against chapters 3 and 6's findings, sorted by which side the two fall on. |
| [`fourth-witness-denise-copperline.md`](evidence/fourth-witness-denise-copperline.md) | Denise (C++) and Copperline (Rust) read against the findings, file and line each. |
| [`fifth-witness-ch8-controller.md`](evidence/fifth-witness-ch8-controller.md) | The four implementations read against chapter 8's controller and interface findings. |
| [`rtl-decisions.md`](evidence/rtl-decisions.md) | Six Copper and blitter findings: the manual's words, NanoMig's measured behaviour, the assertion that measures it, and what WinUAE and vAmiga do. |
| [`upstream-minimig-aga.md`](evidence/upstream-minimig-aga.md) | What NanoMig inherited from Minimig-AGA, which decides whether a finding is NanoMig's own and where a fix should be reported. |
| [`finding-analyses.md`](evidence/finding-analyses.md) | Work that produced or tested a group of findings and belongs to none of them alone. |
| [`blitter-prior-art.md`](evidence/blitter-prior-art.md) | A record: the eight blitter tests that existed before chapter 6 went through the pipeline, and why the five that were deleted could be. Chapter 6's test sources cite it. |

## Elsewhere

How the tests are made — claim extraction, gates, mutation, validation, and the
records of the validation rounds — is in the
[`hrm-testpipeline`](https://github.com/darius600/hrm-testpipeline) repository.
This one holds the tests and the findings.
