# Independent validation — HRM chapter 5, Audio Hardware (FPGA-092)

Date: 2026-09-12

Status: complete. Source-level validation finished; fresh WSL execution was blocked as recorded under Limitations.

## Overall verdict

The claimed “959 testable claims covered” result is not reliable. I found seven high-severity defects, three medium-severity defects, and one low-severity bookkeeping assertion. Most importantly, the suite contains false state-machine reds, a logically reversed Figure 5-8 assertion, vacuous greens, known failures deliberately left as measurements while their IDs are credited to passing checks, and broad claims mapped to predicates that exercise only a small clause. Of the 18 recorded reds, the eight outside `ch5-audio-channel-state` survive source/control review; the state package's ten reds should not be product failures under the current behavioral-conformance contract (eight are private encodings, one reverses “only when,” and one enforces an internal schematic equation).

## Scope and method

The audit covers the eleven `ch5-*` packages, their assigned claim files, the classified/union claim sets, the recorded coverage and attack artifacts, the cited HRM source pages (including Figure 5-8), and the relevant RTL. No test, claim file, or RTL is to be changed, and Git is not used.

Priority order follows the brief: false reds; vacuous greens; assertions where the manual is silent; dishonest claim-to-assertion mappings; classification errors. The three packages whose assertion syntax is not understood by the mutation gate are to be checked manually.

## Findings

### V-01 — HIGH — `TABLE-5-9-HAS-65-VOLUME-LEVELS` is a vacuous green

**Where:** `ch5-audio-volume-scale/test_paula_audio_mixer.cpp:519-545`, especially lines 527-538; assigned claims `5-vol.c3` and `5-vol.c4`.

**What is wrong:** the assertion does not observe the DUT. It fills a host-side `seen[65]` array by looping from 0 through 64 and then asserts that the loop visited 65 inputs. Its mutation merely skips one loop iteration. This proves the C++ loop, not “65 volume levels.” At this very boundary the DUT makes volume 64 equal volume 63, so there are only 64 distinct multiply-path outputs for the documented inputs 0..64. The vacuous green hides exactly the deviation that the neighboring red detects.

**How established:** source inspection of the condition (`count == 65 && all_present`) and comparison with the immediately preceding DUT sweep and the explicit 63/64 equality check at lines 327-347. A valid level-count assertion would collect and count distinct DUT outputs, not distinct stimulus indices.

### V-02 — HIGH — testable claims are counted as covered although the package explicitly declines them

**Where:** `ch5-audio-dmacon-adkcon/test_paula.cpp:29-33, 172-175, 188-189, 212-216, 1111-1135, 1223-1235, 1395-1427`.

**What is wrong:** nine assigned records are classified `testable`, but have no pass/fail assertion: `5-appA-p278.c7`, `5-appB-p289.c3`, `A-dmaconr.chip`; `5-appA-p265.c19`, `A-adkcon.b10`; `5-appA-p278.c21`, `A-dmacon.b04`; `5-p148.c4`, `5-join.c9`. Their IDs remain in comments/`checks.record()` text, so an ID-search coverage gate reports them as covered.

The first three are more than a bookkeeping defect: the package measures that `paula.v` has no DMACONR decode and says the unambiguous manual ownership claim “cannot hold,” but deliberately suppresses the resulting red. The other six need either the floppy fixture or the already-used channel/interrupt fixture; that may justify reassignment, not a 155/155 coverage claim.

**How established:** compared the 155-record assigned JSONL (all `testable`) to the source's own `DECLINED`/`RECORDED, not asserted` blocks. The package itself says those blocks carry no `expect()` and no colour.

### V-03 — HIGH — eight state-code reds report an internal encoding choice as an externally meaningful conformance failure

**Where:** `ch5-audio-channel-state/test_ch5_audio_channel_state.cpp:112-132` and the red checks cited there; RTL parameters `paula_audio_channel.v:242-246`.

**What is wrong:** eight of the ten recorded reds are red only because the synthesizable RTL uses different internal state bit patterns (`011` for its discard state and `110` for its low-byte state) while preserving the state actions, topology, sample order, and timing. They are the eight failing CHECK groups enumerated at lines 120-130: unused-code transition, discard code, low-byte code/output, loop destination, low-byte self-loop, main-loop name, and the `001->101` destination. The fixture obtains `audio_state` through Verilator `--public-flat-rw`; it is not a module port or software-visible register. The HRM figure indeed labels the physical Paula flip-flops `sq2,1,0`, but a behavioral RTL implementation is free to re-encode an internal FSM unless exact gate/state encoding is an explicit project requirement. These should be recorded as a microarchitectural difference, not product reds.

**How established:** the package's own result summary says every affected assertion is otherwise green and only the bit-pattern naming fails. Figure 5-8 names the flip-flops, but none of the `paula_audio_channel` public ports exposes their code. Severity is high because false reds are the product's worst defect. If the project explicitly intends cycle-and-state-identical Paula reconstruction rather than observable HRM conformance, this finding becomes a scope decision that must be stated; the current test class does not state that stronger contract.

### V-04 — MEDIUM — the filter-bypass claims are only partially exercised

**Where:** `ch5-audio-filter-bypass/test_ch5_audio_filter_bypass.cpp:1-24, 45-51, 192-238`; claims `5-p157.c8` and `5-filt.c9`.

**What is wrong:** both assertions prove only that CIA-A PRA bit 1 can be written and read at the `ciaa` boundary. The cited quotation says this is the same output bit that controls the power LED (and context identifies it as the filter control). The package explicitly does not observe either downstream connection, and it does not compile the `minimig.v` wiring used by the classifier's evidence. A design with a perfectly writable but disconnected PRA bit passes both assertions.

**How established:** compared the two assigned quotations/restatements and classifier evidence with the fixture boundary and assertion conditions. The address/bit-register half is exercised; the identity/connection half is not. The IDs should not count as wholly covered without a wider-boundary assertion or a split classification.

### V-05 — MEDIUM — partial claims are systematically allowed to count as fully covered

**Where:** classified inventory and several package headers/assertion labels. A mechanical scan finds 30 `testable` rows whose own classifier evidence says `Split`, `half ... not observable`, or equivalent.

**What is wrong:** the coverage unit is the claim ID, yet the implementation routinely cites an ID after testing only one clause. Examples: `A-audxdat.c4` is counted by the byte-order assertion while its DAC/output-pin clause is explicitly not observed; the eight per-register Appendix-B pointer claims and `B-legend.plus` are counted after testing even alignment while their Chip-memory clause is explicitly outside `agnus_audiodma`; and the filter claims in V-04 stop before either advertised downstream connection. This is not merely conservative classification: the stage reports 959 testable claims covered even when source text says a clause is unmeasured.

**How established:** searched the 959 `testable` classifications for their own split/non-observable admissions, then checked representative assertion sites by hand. Claims must be atomized before coverage or carry an explicit partial-coverage status; an ID occurrence cannot make the untested clause disappear.

### V-06 — HIGH — the `dmasen` NOTE red reverses “only when” and is logically false

**Where:** `ch5-audio-channel-state/test_ch5_audio_channel_state.cpp:1161-1175`; claims `5-p166.c7` and `F58.note.dmasen`.

**What is wrong:** Figure 5-8 says, after exempting the `000->001` case, “dmasen is true only when LENFIN=1.” That is the implication `dmasen -> LENFIN`. The assertion instead searches for at least one later sample satisfying `dmasen && lenfin` and fails if none exists. Absence of later `dmasen` pulses satisfies the printed “only when” restriction; it does not violate it. The test has changed a necessary condition into an existence/biconditional requirement. Its mutation is recorded `impossible`, and there is no passing same-basis control.

**How established:** visually read the NOTE in Figure 5-8 and compared its one-way wording with the predicate at lines 1166-1175. This is a false red independently of the broader state-encoding judgment in V-03. The neighboring `AUDxDSR=AUDxDR*dmasen` red is a separate microarchitectural-equation question: it mixes the public `dmas` port with two `--public-flat-rw` implementation signals and should be informational under a behavioral interface contract, but would be a legitimate red if exact schematic-internal identity is explicitly required.

### V-07 — HIGH — the bit-14 “no request” failure is measured and then suppressed behind a passing claim citation

**Where:** `ch5-audio-interrupt/test_paula_intcontroller.cpp:65-77, 172-184, 732-769`; claims `5-appA-p280.c20` and `A-int.b14`.

**What is wrong:** both claims say bit 14 is the master interrupt “enable only, no request.” The passing CHECK asserts only the enable-gate half, yet cites both IDs. Immediately afterward, the package writes INTREQ bit 14, reads it back set, and states that this contradicts “no request”; that result is printed as `MEASURED`, not asserted. Thus two testable claims whose full text is known to fail are reported covered by a green assertion. The RTL comment that the extra request bit is for compatibility explains the deviation; it does not make the HRM claim pass.

**How established:** compared the claim text and classifier evidence with `paula_intcontroller.v`'s `intreq[14] <= tmp[14]` behavior and the source's own recorded readback. This should be a red (or a formally waived extension), not 9/9 green.

### V-08 — HIGH — four “eight-state” greens are force/readback tautologies

**Where:** `ch5-audio-channel-state/test_ch5_audio_channel_state.cpp:500-521, 704-736`; claims in CHECK groups `5-p164.c2`/`5-sm.c2`, `5-sig.sq`, `F58.states`, and `F58.sqbits`.

**What is wrong:** `run_encoding()` assigns each requested integer directly into the private Verilated `audio_state` register, calls `eval()`, and reads that same register back without a clock transition. The four assertions then count the values the host forced or check their bits. This establishes only that the generated storage can be overwritten with the eight 3-bit patterns; it does not establish that the implemented state machine has eight reachable states, that its own state flip-flops independently assume both values, or that Figure 5-8's eight circles exist behaviorally. Their stimulus mutations merely omit host-written values, so they validate the host loop rather than the DUT FSM.

**How established:** traced the entire helper and its four predicates. A discriminating test must reach states through legal inputs/transitions (or, for unused-state recovery, force a state and then clock the actual next-state logic); direct unclocked write/read is not evidence of reachability or machine structure.

### V-09 — HIGH — one slot/address assertion is credited with DMA, RAM, sample, and DAC claims it never exercises

**Where:** `ch5-audio-location-pointer/test_agnus_audiodma.cpp:83-100, 483-533`, assertion `SLOT-PER-CHANNEL-PER-LINE`.

**What is wrong:** the predicate only sweeps `hpos`, supplies `audio_dmal`, and checks `dma` plus `reg_address_out==AUDxDAT`. It has no RAM input, does not deliver a word into Paula, never observes either sample byte, and has no DAC/output boundary. Nevertheless its 18 cited IDs include claims that each channel's DAC is DMA-driven (`5-p137.c2`, `5-hw.c6`), that a DMA word supplies two samples per line (`5-p137.c3`, `5-p141.c2`, `5-hw.c7`, `5-rate.c3`), and that memory data is automatically retrieved/loaded into AUDxDAT (`5-p137.c6`, `5-hw.c10`, `5-p148.c5`, `5-join.c10`, `5-appA-p265.c44`, `A-audxdat.c6`). The measured slot and register address are necessary for those behaviors, but not sufficient to prove them.

**How established:** compared the assertion's two booleans (`one_slot_each`, `right_register`) with each cited restatement and the `agnus_audiodma` fixture ports. At most the channel/slot/address subset is covered; the data-transfer and output claims need an integrated Agnus-to-Paula data path.

### V-10 — MEDIUM — `not-hardware` classification discards hardware-testable timing ratios and behaviors

**Where:** `2026-09-12_ch5_classified.jsonl`, among the 620 `not-asserted/not-hardware` rows.

**What is wrong:** the absence of a Hertz-valued port prevents an absolute frequency calibration, but it does not prevent dimensionless timing assertions. Concrete misclassifications include:

- `5-p141.c7`, `5-p141.c8`, `5-rate.c11`, `5-p142.c1`, `5-p142.c2`, and `5-per.c13`: `sample interval / clock interval == AUDxPER` is directly measurable in cck ticks. The existing period package already uses exactly that basis for the page-158 and page-159 formula claims.
- `5-p159.c8` and `5-scale.c25`: same period with sample byte count halved must halve block duration (double repetition rate). The existing `G_LEN_FACTOR2` test demonstrates that this ratio is constructible without knowing seconds or Hertz.
- `5-eff.c3`: a longer table reduces interrupt service rate. `AUDxLEN` and the channel's `intreq` output make this directly testable as an interrupt-count/cadence ratio; the classifier's claim that the module “never sees a table” ignores its length counter.
- `5-direct.c7`: volume and period registers work as usual in direct mode. Direct/DMA runs can compare `sample`, `sample_okk`, timing, and volume on the same module boundary.
- `5-noise.c1`: the accepted signed sample range `-128..127` is digitally observable through the eight-bit sample input/sign extension even though acoustic quantization noise is not.

The 360 Table-5-8 `not-hardware` row records (`5-p160.c1..c144`, `5-p161.c1..c96`, and `5-t58.s{256,128,64,32,16}.*.{ntsc,pal}`) mix an absolute-Hz/note assertion with a hardware-relative `SampleBytes*Period` timing assertion. They are not wholly assertable as written, but their relational component is testable and should be split rather than discarded wholesale. The analogous Table-5-7 actual-frequency families have the same partial structure.

**How established:** enumerated all 620 rows by page and inspected the timing-related restatements/evidence against `paula_audio_channel`'s `cck`, `AUDxPER`, `AUDxLEN`, `sample`, `dmas`, and `intreq` behavior. Absolute NTSC/PAL Hertz remains outside this boundary; relative cck-tick ratios do not.

### V-11 — LOW — `VOLUME-64-OUT-OF-SCOPE` is a host arithmetic assertion, not a DUT check

**Where:** `ch5-audio-volume-helper/test_ch5_audio_volume_helper.cpp:173-184`.

**What is wrong:** the assertion computes `64 & 0x3F` in C++ and checks that the result is zero; it never applies 64 to the Verilated module. Its mutation changes the host constant to 65. It is labeled a boundary demonstration and cites no claim, so it does not corrupt claim coverage, but it inflates the green assertion total and the statement that every mutation proves a non-vacuous hardware check.

**How established:** followed the value from declaration to `checks.expect()` and confirmed there is no `eval_case()`/DUT call in the block.

## Checks that currently survive attack

- **Figure 5-8, `011→000`:** independently inspected the repository's rendered printed page (`docs/references/amiga-hrm-3rd/ch5/figures/p166.png`) and a magnified crop. The diagonal label visibly has a long overbar across the parenthesized sum and a second shorter overbar over `AUDxIP`. The reading is therefore `perfin · NOT(AUDxON + NOT AUDxIP) = perfin · NOT AUDxON · AUDxIP`, matching `paula_audio_channel.v:431`.
- **Audio pointer width red:** the green ECS-width control and required-red OCS check use the same walking-one method and the same word-address output. The red assertion's explanatory wording correctly says the module implements the ECS five-bit form unconditionally; it does not claim that a register named `AUDxLCH` is universally five bits.
- **ADKCON audio red:** the assertion is limited to `paula.v`'s observable `ldata`/`rdata` outputs and asks only whether the modulating channels are silenced. It does not claim to observe period/volume words reaching another channel. Its zero-volume control uses the same output/peak basis.
- **Volume 64 boundary reconciliation:** `ch5-audio-channel-registers` measures the seven-bit `volume`/PWM `sample_okk` path and sees 64 distinct from 63; `ch5-audio-volume-scale` measures `paula_audio_mixer.ldatasum` and sees equality; `ch5-audio-register-map` observes both `paula_audio` PWM outputs (distinct) and multiply outputs (equal). Those results agree and name their ports. The adjacency control at the mixer (62 vs 63 on `ldatasum`) plus clocked-liveness control is adequate for the 63/64 red itself, but it does not rescue the separate vacuous level-count green in V-01.
- **Direct-output interrupt red:** printed page 157 really says the direct-mode interrupt occurs after one data word has been output. The test compares `AUDxIR` and both sample-byte timestamps in the same raw-tick trace; `intreq` is a direct assignment from `AUDxIR` in the RTL. Group H observes the same write/output transition and passes. The changed-word mutation remains red (a robustness check, not a flip), but the same-basis trace makes the baseline red credible.
- **Stereo-route reds:** the assertions drive distinct values into the named channel pairs and observe `ldatasum`/`rdatasum`; their channel-swapped controls flip each red green. They reproduce the already registered left/right naming disagreement rather than inventing a new oracle.
- **Period/length impossible mutations:** the `percount` decrement and `perfin==1` checks both require multiple positive DUT events and check every observed event, so their structural `impossible` stimulus mutations do not make the greens vacuous. `F54.even` becomes unmeasurable when the responder is starved rather than falsely passing; its baseline still counts a completed DUT block.

## Recorded package inventory

Fresh execution was blocked as described below. These are the checked-in source summaries (for the two small packages without an explicit baseline sentence, the totals follow from their three/four CHECK sites and recorded one-at-a-time mutation statements):

| Package | Assigned claims | Recorded assertions | Recorded result |
|---|---:|---:|---:|
| `ch5-audio-channel-registers` | 94 | 18 | 18 pass, 0 fail |
| `ch5-audio-channel-state` | 79 | 44 | 34 pass, 10 fail |
| `ch5-audio-direct-output` | 19 | 12 | 11 pass, 1 fail |
| `ch5-audio-dmacon-adkcon` | 155 | 19 | 18 pass, 1 fail |
| `ch5-audio-filter-bypass` | 2 | 3 | 3 pass, 0 fail |
| `ch5-audio-interrupt` | 106 | 9 | 9 pass, 0 fail |
| `ch5-audio-location-pointer` | 176 | 10 | 9 pass, 1 fail |
| `ch5-audio-period-and-length` | 53 | 31 | 31 pass, 0 fail |
| `ch5-audio-register-map` | 116 | 9 | 8 pass, 1 fail |
| `ch5-audio-volume-helper` | 6 | 4 | 4 pass, 0 fail |
| `ch5-audio-volume-scale` | 153 | 74 | 70 pass, 4 fail |
| **Total** | **959** | **233** | **215 pass, 18 fail** |

The aggregate counts are not a valid completeness result because V-02, V-04, V-05, V-07, and V-09 show IDs credited without their full propositions being asserted, while V-01 and V-08 show passing predicates that do not establish the named behavior.

## Audit coverage ledger

- [x] Inventory all eleven packages and their results.
- [x] Validate every red against its cited source and a same-basis control.
- [x] Check green assertions for unreachable preconditions and impossible mutations.
- [x] Check that the unusual `AUDxLEN` 1/2/4 behavior and NTSC 3579545 remain unasserted. (`AUDxLEN` 2 and 4 are still asserted for separate manual-backed ordinary length/ratio claims; the undocumented anomaly itself is only `MEASURED`.)
- [x] Audit the 959 testable claim mappings for comment-only or non-exercising citations.
- [x] Attack the 620 `not-hardware` classifications, including ratio-testability.
- [x] Inspect `agnus_audiodma.v:82` assertion wording and boundary.
- [x] Reconcile volume-64 measurements at all three boundaries.
- [x] Inspect Figure 5-8 directly from the available rendered printed page at high resolution (direct local-PDF navigation was browser-policy blocked; the checked-in page render is source-derived).
- [x] Judge state-code differences as observable deviation or internal encoding.
- [x] Check ADKCON assertions at the `paula.v` boundary for overreach.
- [x] Manually inspect mutation evidence for channel-state, direct-output, and period-and-length.

## Limitations

- Fresh builds could not be run: every WSL invocation was refused with `Wsl/Service/CreateInstance/E_ACCESSDENIED`. Per the validation brief, WSL was not retried. Runtime conclusions therefore rely on sources, checked-in build artifacts, and recorded results; this limitation will be distinguished from source-level findings.
- The original PDF could not be opened through the browser because local `file:` navigation was rejected by browser policy, and no local Poppler/Python runtime is available outside blocked WSL. Figure 5-8 was nevertheless inspected directly from the repository's full-page PNG rendered from printed page 166, including a locally magnified crop; no OCR or prior prose reading was used for the overbar judgment.
- The mapping audit was exhaustive for explicit declined/record-only blocks and classifier-declared partial claims, plus manual review of all red-bearing and high-density assertions. It was not a semantic proof of each of the 959 individual English propositions; V-05 and V-09 demonstrate why the current ID-search gate cannot provide such a proof either.
