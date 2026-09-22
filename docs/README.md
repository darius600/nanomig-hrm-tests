# What is in here, and what each document is for

Thirteen documents besides this map. A red test is not self-explanatory: someone
reading `FAIL` needs to know what the manual says, what the RTL does, whether
anyone else agrees, and who decides. Nine of these answer that; three are
records, and one binds anyone adding a test.

Each entry below says what the document *is*, so an instruction is not mistaken
for a record:

- **reference** — look things up in it. It describes what is true now.
- **decision support** — written for someone who has to choose what to do.
- **method** — the rule for when something is allowed.
- **binding** — follow it. A reviewer will check your work against it.
- **record** — what was true on the date in its title. Not an instruction, and
  not to be updated to match today; that is how a repository loses the ability to
  say when something changed.

## If you have just watched the suite go red

Read them in this order.

| | | |
|---|---|---|
| [`findings.md`](findings.md) | reference | **Start here.** The register tables of all **68** findings — key, subject, status, package, third witness. 65 are against the Hardware Reference Manual; the 3 against the AA specification sit in a section of their own so neither count has to be read twice. The maintained per-finding pages are in [`findings/`](findings/), one file each, written to be readable without access to anything outside this repository; the sections below the table in `findings.md` are the original text and are kept deliberately. Written so a red result is readable without access to anything outside this repository. |
| [`rtl-decisions.md`](rtl-decisions.md) | decision support | Six findings stated as the manual's words, what NanoMig measurably does, the assertion that measures it, and what WinUAE and vAmiga do about the same question — with verified file-and-line citations into both emulators. The document for a developer who has to decide something. |
| [`third-witness.md`](third-witness.md) | method | When an emulator may be consulted at all, and when it may not. The manual is the oracle; an emulator is admitted only to decide between two sentences of the manual that disagree with each other. NM-HRM-001 was withdrawn this way and NM-HRM-002 restored — and two further implementations have since split NM-HRM-002 two against two. |
| [`third-witness-ch3-ch6.md`](third-witness-ch3-ch6.md) | decision support | The same comparison carried out for chapters 3 and 6, ten findings at a time, sorted by which side the two emulators fall on. Section **A** is where both emulators agree with the manual and NanoMig is the outlier — the strongest case for looking at the RTL. Section **B** is where both agree with NanoMig and the printed sentence is the outlier. Section **C** is where the two emulators disagree with each other and settle nothing. |
| [`fourth-witness-denise-copperline.md`](fourth-witness-denise-copperline.md) | decision support | Two further independent implementations — **Denise** (C++) and **Copperline** (Rust) — read against the findings, with a file and line for each. Neither is an oracle. It corrected NM-HRM-002 from *two emulators settle it* to a two-against-two split and did the same to NM-HRM-016 and NM-HRM-017; found the first implementation that narrows a DMA pointer by chipset exactly as the manual describes; and made NM-HRM-020 the best-corroborated finding in the register. Three findings now stand against **all four** implementations — BLTSIZV's ECS gating, the DDF mask being chipset- and not resolution-gated, and playfield 1's collision depending on playfield 2. |
| [`finding-analyses.md`](finding-analyses.md) | reference | Work that produced or tested a **group** of findings and belongs to none of them alone: where the five oldest came from and whether they hold, appendix C's five ECS blitter changes implemented five different ways, and what the emulators say about the three Copper findings. Split out of `findings.md`, which a reader could not use as a register while `NM-HRM-005` appeared in it five times. |
| [`legacy-names.md`](legacy-names.md) | reference | Resolves every identifier a reader meets and cannot place: the `sol:`/`opus:` claim-id prefixes, `FPGA-nnn` task numbers, `work/…` claim files, and the pipeline's role words. It also resolves two kinds of stale name: the `FPGA-nnn` finding keys that older commit messages cite, and the twenty pre-pipeline package directories that `//` comments in live tests still point at. Needed only for that. |
| [`fifth-witness-ch8-controller.md`](fifth-witness-ch8-controller.md) | decision support | The same four-implementation comparison for chapter 8's findings — WinUAE, vAmiga, Denise and Copperline read against the controller and interface-hardware reds, file and line each. None of them is an oracle; `third-witness.md`'s rules apply here unchanged. |
| [`upstream-minimig-aga.md`](upstream-minimig-aga.md) | decision support | What NanoMig inherited from MiSTer's Minimig-AGA and what the two maintained lines have changed since. It answers the one question the four emulators structurally cannot: whether a finding is NanoMig's own or came down from upstream — which decides where a fix should be reported. |
| [`adding-a-test.md`](adding-a-test.md) | binding, for contributors | What a finished package has and what a reviewer checks: the manual before the RTL, one assertion per proposition with its page, the claim id on the graded line, a control on the same measurement basis, a stimulus mutation per assertion, a truthful non-zero exit, a clean rebuild — and the four things not to do. `hrm/ch3-beam-counters-and-blanking` is the worked example. |

## Record

| | | |
|---|---|---|
| [`record-2026-09-11-validation.md`](record-2026-09-11-validation.md) | record | An independent validator's audit of chapter 3's thirteen findings: seven did not survive in the form they were written, four were withdrawn, two became source observations, one was corrected, and one new finding was established while correcting another. Every finding it revised carries its own note in `findings.md`; this is the round's own account of why. |
| [`record-2026-09-12-ch5-validation.md`](record-2026-09-12-ch5-validation.md) | record | An independent validator's audit of chapter 5, which found seven high-severity defects and said plainly that the chapter's own coverage claim was unreliable. Eight of the eighteen reds survived; one was logically false and is withdrawn, eight reported an internal encoding as a conformance failure and became a source observation, four greens proved a Verilator storage cell rather than a state machine, and two known failures were being credited to passing assertions. Every one was repaired and the repairs re-checked. The round's own account of why. |
| [`blitter-prior-art.md`](blitter-prior-art.md) | record | The eight `agnus_blitter_*` tests that existed before chapter 6 went through the pipeline, read one by one to decide what each was worth. Three were extended into pipeline packages and five were superseded and deleted, each only after its replacement was green from a clean build. The tests are gone; the reasoning is why the deletions were safe. |

## Where the rest of it is

The method that produces the pipeline tests — the claim extraction, the gates,
the mutation campaign, and an honest account of what all that does and does not
guarantee — is in the
[`hrm-testpipeline`](https://github.com/darius600/hrm-testpipeline) repository,
not here. This repository holds the tests and the findings.
