# What is in here

Five documents. Four of them exist because a red test is not self-explanatory:
someone reading `8 failed` needs to know what the manual says, what the RTL does,
whether anyone else agrees, and who decides. One is a record of tests that no
longer exist.

Read them in this order if you have just seen the suite go red.

| | |
|---|---|
| [`findings.md`](findings.md) | **Start here.** Every finding, with the measurement that produced it and the manual page it contradicts. Written so a red result is readable without access to the private workspace where the `FPGA-nnn` numbers are assigned. |
| [`rtl-decisions.md`](rtl-decisions.md) | The nine open findings stated as the manual's words, what NanoMig measurably does, the assertion that measures it, and what WinUAE and vAmiga do about the same question — with verified file-and-line citations into both emulators. This is the document written for a developer who has to decide something. |
| [`third-witness.md`](third-witness.md) | When an emulator may be consulted at all, and when it may not. The manual is the oracle; an emulator is admitted only to decide between two sentences of the manual that disagree with each other. FPGA-050 was withdrawn this way and FPGA-051 restored. |
| [`test-inventory.md`](test-inventory.md) | Which of the 50 tests came through the pipeline and which are hand-written, by name, and the review backlog. A green means different things in the two layers, and this is where that difference is written down. |

## Record

| | |
|---|---|
| [`blitter-prior-art.md`](blitter-prior-art.md) | The eight `agnus_blitter_*` tests that existed before chapter 6 went through the pipeline, read one by one to decide what each was worth. Three were extended into pipeline packages and five were superseded and deleted, each only after its replacement was green from a clean build. The tests are gone; the reasoning is why the deletions were safe. |

## Where the rest of it is

The method that produces the pipeline tests — the claim extraction, the gates,
the mutation campaign, and an honest account of what all that does and does not
guarantee — is in the [`hrm-testpipeline`](https://github.com/darius600/hrm-testpipeline)
repository, not here. This repository holds the tests and the findings.
