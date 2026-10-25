# Findings — one page each

A reference for developers, not a narrative.

**Seventy-five of these pages are filed against the Hardware Reference
Manual, 3rd edition (1991), and thirteen against the AA specification (1991).** The AA group (NM-AA-001 to
NM-AA-013) is counted separately in [`../findings.md`](../findings.md), and
the tests that hold them up live in `aa/` rather than `hrm/`. The fourteen
withdrawn findings are kept apart, with their own list, in
[`withdrawn/`](withdrawn/README.md).

This page is the contract every finding page follows: one header table, five
body sections, and a few rules about what goes where.

## The header of an open page

**One table, the same rows in the same order on every open page.** The first
four rows say *what am I being asked to do* before any prose is read. An absent
value is written `none`, `unknown` or `not measured`, except that row 13 takes only the forms below; a row is never left out.
After the thirteen rows only the optional ones follow.

| # | row | what it holds |
|---|---|---|
| 1 | **Lifecycle** | `open` |
| 2 | **Filed against** | `the RTL`, `the printed page` or `the classification` |
| 3 | **Evidence** | what holds the finding up, in plain words: *a test fails against the page* (with *the implementations split* when the other implementations disagree among themselves, or *because NanoMig lacks a documented bug* when the page describes a defect of the original chips); *a test passes; the page's other reading fails* (the page says two things, and a control shows the reading NanoMig does not follow fails); *seen in the RTL source; no test yet*, or *seen in the RTL source and measured inside the module; no assertion at the ports fails* (a red on an internal signal the manual documents, which no pin shows); or *none* |
| 4 | **What is asked of a reader** | the exact decision and its boundary first, as one bounded yes/no question, then the event that closes it. If it begins *No RTL change* or *Nothing*, the rest of the page is context |
| 5 | **Report to** | the routing value in backticks, then ` — `, then the repository or role that acts: `upstream first` (the code is inherited from MiSTer's Minimig-AGA; report there first and NanoMig takes the fix), `NanoMig direct` (only NanoMig's own code, or code that a Minimig line has fixed and NanoMig has not: the action is to port that fix, and the Module row then ends `fixed in ...`) or `no RTL action` (the printed page or the classification is wrong, not the RTL; the role is then `HRM documentation` or `AA documentation`, followed by `: an issue on this repository whose title begins with ` and the page's own key, because that is where the decision is recorded). A page whose Module row ends `fixed in TC64` routes `NanoMig direct` to harbaum/NanoMig and MiSTer-devel/Minimig-AGA_MiSTer (port TC64's fix; MiSTer carries the same code); `fixed in MiSTer`, to harbaum/NanoMig and minimig-dev/MinimigAGA-MiST-TC64; `fixed in MiSTer and TC64`, to harbaum/NanoMig. Never `nobody`, never a person's name. Nothing follows the repository or role: a reason belongs in the Module, MiSTer or TC64 row |
| 6 | **Scope** | chip, revision and mode the finding holds for, and the edge case that was isolated, or `unknown`; and, where it decides whether NanoMig can show the difference, the core's own limit (chipset mode, video output, connector, disk image format) |
| 7 | **Software impact** | the named software case and its result, or `unknown; no named affected software`; either may end `; on NanoMig: <effect>` where the page knows the effect on this core |
| 8 | **Chapter** | the oracle: document, edition, page, table or figure, and claim ids. A cell with more than 12 claim ids holds them in one collapsed `<details><summary>claim ids by assertion (N)</summary> ... </details>` on one line; at 12 or fewer they stand in the cell |
| 9 | **Module** | the RTL under discussion, at most two links pinned at the revision below, each link text naming its commit (``[NanoMig `4315acb`](…)``), and then, as the last words of the cell, exactly one of: `inherited` (MiSTer has the same code; a clause may say how TC64 differs), `fixed in MiSTer`, `fixed in TC64` or `fixed in MiSTer and TC64` (that Minimig line has corrected the behaviour; NanoMig has not; row 5 then routes `NanoMig direct`), or `NanoMig's own` (the cited code has no counterpart in either Minimig line). A parenthetical may follow, and may say that the cited code declares the behaviour not implemented, with the file:line of that declaration. When the change the page asks for touches code that is NanoMig's own, the parenthetical names it with its file:line. A bare `fixed` is not a value |
| 10 | **Test** | every package the page names, each with its exact command, or the statement that no package observes the claim. For an integration package: `make -k -C integration/<pkg> test` (or both `test-fx68k` and `test-tg68k`), and that an expected red exits nonzero. The first command is the package whose red row 11 quotes first |
| 11 | **Assertions** | the red assertions by id with one expected → observed result and the package's count, or `none` and what the package records instead. An id may name the observed deviation (`DSKBYT-NOT-CLEARED-ON-READ`), the requirement (`DSKBYT-IS-CLEARED-BY-THE-READ`), a short code (`P1`, `LF-FRAME`, `PROP-1`) or a claim id (`sol:K-0740`); a FAIL always means that the page's requirement is not met |
| 12 | **Verdict of the four** | what WinUAE, vAmiga, Denise and Copperline do: split, unanimous or no vote, and, last, `Hardware:` and the hardware status |
| 13 | **Effort** | the size of the RTL change the page asks for, in exactly one of three forms: `S`, `M` or `L`, then ` — ` and what changes; `none — ` and one clause, for example no RTL change; or `none; if <condition>, S|M|L: <what changes>`. `unknown` is not a value of this row |

Optional rows, after the thirteen and in this order: **See also** (at most
three links to sibling findings), **Origin** (the document or run the finding
was first read from) and **Provenance** (where material that is not in this
repository comes from).

## The header of a withdrawn page

A withdrawn page is kept so the claim is not raised a second time. Its rows, in
this order:

| # | row | what it holds |
|---|---|---|
| 1 | **Lifecycle** | `withdrawn` |
| 2 | **Filed against** | `—` |
| 3 | **Evidence** | the withdrawal evidence: the measurement or passage that disproves the claim, in one sentence. It is never `none`, and never one of the open-page phrases (*a test fails against the page*, *seen in the RTL source; no test yet*, *seen in the RTL source and measured inside the module; ...*), which describe a live finding |
| 4 | **What is asked of a reader** | `Nothing.` and the one sentence that stops the claim being raised again |
| 5 | **Withdrawal reason** | why the claim does not hold, in one sentence; the full account is the page's *Why it was withdrawn* |
| 6 | **Error class** | which recurring shape the mistake had, or `none` |
| 7 | **Replacement** | the green results and the package links that stand in for the finding, or `none` |
| 8 | **Module** | as on an open page |
| 9 | **Test** | as on an open page |
| 10 | **Chapter** | as on an open page |
| 11 | **Assertions** | the assertions that were graded, with their verdicts |
| 12 | **Verdict of the four** | what the four do, or `none` |

Optional rows after the twelve: **See also**, **Origin**, **Provenance**, as above.
A withdrawn page has no `Report to`, `Scope`, `Software impact` or `Effort`.
Its rows 8 to 10 are Module, Test, Chapter; on an open page rows 8 to 10 are
Chapter, Module, Test. The two orders are deliberate.
Neither header carries a date or the name of whoever changed it.

**A title that ends in [RTL review]** marks a page whose header says three
things: `open`, filed against `the RTL`, and a test that fails against the page. That page asks an
Amiga specialist to decide whether the RTL should change;
[`../rtl-review.md`](../rtl-review.md) lists every such page by the module it
is about. The mark is derived from the header, never set by hand, so it cannot
say something the header does not.

**A title that ends in [withdrawn]** marks a withdrawn page: its title ends with
` [withdrawn]`, as an open page's may end with ` [RTL review]`, so that a search by
register name does not read a disproven claim as a defect.

## The body of an open page

Five `##` sections, in this order, with these exact headings. A `###` below
them is free, and no other `##` heading appears.

1. **Source requirement** — what the manual prints, quoted with its printed page.
   *What the HRM says* and *Which page this finding is against* belong here.
2. **RTL and measurement** — which RTL line does it, what the test measures and which
   assertion grades it. *What NanoMig does* and *Measured* belong here.
3. **Risks and unknowns** — every limitation of the finding. *What weakens it* and
   *For the specialists* belong here.
4. **Independent evidence** — the witness matrix and what the other documents say.
   *What the others say* belong here, and *What other Commodore documents say* only
   where that source speaks.
5. **Decision** — what is asked, the suggested change if there is one, and what
   closes it. *Suggested change* belongs here.

*What holds the finding up* has no section: a closing recap repeats the page.

A withdrawn page is short. It says what the claim was and why it does not hold
(*Why it was withdrawn*, first), shows the measurement or passage that disproves it,
and ends with the condition under which the claim would be filed again. Its other
headings are free.

## Rules for every page

- **A closure is an event on the page, not a person's decision.** Row 4 names the change
  and the reds that turn green, or the rejection that closes the page; no sentence says
  that a decision belongs to a specialist or to someone else. A command stands in the
  Test row only. The phrases `decision belongs to`, `Re-run:`, `Hardware status`,
  `entry is kept` and `entry exists` appear on no page.
- **A decision is recorded as an issue.** A decision an Amiga specialist records is
  an issue on this repository whose title begins with the finding key, and the page
  then links it in row 4; a maintainer closes a page filed against the RTL by the
  RTL change its row 4 names.
- **A red for a finding filed against the printed page** does not stay a `FAIL`
  line once the page closes because the misprint is recorded. Its assertions are
  not changed to pass; their claims are recorded as resting on a recorded misprint
  (a `RECORDED` line naming the finding key), so the suite stops printing `FAIL`
  for them.
- **Each fact once.** The header summarizes and the body proves; a page has no closing recap.
- **Sizes are defaults, nothing more.** The gate does not measure them, and a page
  that needs more writes more. Overflow never moves to a method document.

  | part | default |
  |---|---|
  | Source requirement | 120 words, two quotations, 50 quoted words |
  | RTL and measurement | 220 words, at most 8 code lines, a table of at most 6 rows |
  | Risks and unknowns | 80 words |
  | Independent evidence | 140 words |
  | Decision | 140 words |
  | the visible path of one page | 1,000 words open, 400 withdrawn |

- **One witness matrix**, in the order WinUAE, vAmiga, Denise, Copperline, MiSTer, TC64.
  MiSTer and TC64 are marked *descendant* and never counted among the four. A source
  nobody read is written *not read*.
- **`[n/3]`, *corpus* and *Developer CD reading*.** The *corpus* is this project's verbatim text of the AA specification, one page per file, merged from three readings of the page: Tesseract OCR, the 1993 `AGA.guide` typing and a model reading of the rendered page. `[3/3]` marks a line all three produced, `[1/3]` a line only one produced, with nothing to corroborate it. The *Developer CD reading* is the text of a printed-manual page as the Amiga Developer CD's edition of the manual gives it, as against the scan's text layer.
- **"Identical" means the cited lines.** A claim about a whole file says *file*.
- **No method prose on a page.** The oracle statement, the mutation and gate
  explanation, the private-copy narrative, the revision ledger and the second-documents
  paragraph are stated once, here and in *Second documents* below.
- **No list of silent sources**, and no package that does not observe the claim.
- **External observables before internal signals.** An internal confirmation keeps one
  sentence and its link.
- **Every limitation sits in *Risks and unknowns***: unmeasured hardware, untested
  modes, conflicting passages, witness splits and revision limits. For a withdrawn page
  that is the condition for filing again.
- **Every page gives a reproducible next step**: the exact commands, the expected red
  and green ids, the owner, the effort and the closure. A page about source that no
  package observes says so.

Where implementations are compared, six are quoted rather than cited: WinUAE,
vAmiga, Denise and Copperline, then the two maintained Minimig lines, **MiSTer**
and **TC64**. The last two are marked as descendants rather than witnesses, for
the reason set out below; they sit in the same table because a reader comparing
implementations wants them side by side.

**[`../findings.md`](../findings.md) is the register** — every open finding,
its status, its package and the verdict of the four, in one table. It is the list to go to;
this directory holds what it links to.

## Pinned revisions

Every link in these pages is a permalink at the revision that was read, and
every quotation was verified in a local checkout at that revision. A page that
names another finding links to it, as [NM-HRM-019](NM-HRM-019.md) does here, so
a reader can follow the thread without going back to the register.

| what | repository | revision · commit date |
|---|---|---|
| **NanoMig** — the design under test | [harbaum/NanoMig](https://github.com/harbaum/NanoMig) | [`4315acb`](https://github.com/harbaum/NanoMig/tree/4315acba5e9f1e15103037627aa78b3460cae8af) · v0.17-beta-2-g4315acb · 2026-10-16 |
| **Minimig-AGA** — what NanoMig descends from | [MiSTer-devel/Minimig-AGA_MiSTer](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer) | [`da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/tree/da7632e44498840850e78966947f8770166e6e77) · 2026-09-11 |
| **Minimig-AGA on MiST/TC64** — the second maintained line | [minimig-dev/MinimigAGA-MiST-TC64](https://github.com/minimig-dev/MinimigAGA-MiST-TC64) | [`9e8e308`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/tree/9e8e30844fe134dca54cea65f1bd38539dfc15ad) · 2025-05-03 |
| WinUAE | [tonioni/WinUAE](https://github.com/tonioni/WinUAE) | [`ccc407e`](https://github.com/tonioni/WinUAE/tree/ccc407e9c636668b99b5f19d6e9aa03ea04b3052) · 2026-09-06 |
| vAmiga | [dirkwhoffmann/vAmiga](https://github.com/dirkwhoffmann/vAmiga) | [`35de004`](https://github.com/dirkwhoffmann/vAmiga/tree/35de004a2f1502c2811b9a8ebcde4d0979a98937) · 2026-09-06 |
| Denise | [piciji/denise](https://github.com/piciji/denise) | [`1cb7d45`](https://github.com/piciji/denise/tree/1cb7d45117893c76025d32260edb6008001344f9) · 2026-09-11 |
| Copperline | [CopperlineHQ/Copperline](https://github.com/CopperlineHQ/Copperline) | [`726bed0`](https://github.com/CopperlineHQ/Copperline/tree/726bed00c407727c1f5bd32c7193587986638941) · 2026-09-11 |

NanoMig has moved on since `4315acb`; open the permalinks, not a current checkout.

### The two Minimig lines are ancestors, not witnesses — and nothing merges them down

**NanoMig was forked from MiSTer's Minimig-AGA, and there is no
automatic merge of fixes from MiSTer into NanoMig.** A correction made upstream
does not arrive by itself; somebody ports it deliberately. That single fact is
why these two rows earn their place in the table and why they are marked
differently from the other four:

- **As evidence about the manual they are worth nothing.** All three trees
  descend from the same code. When MiSTer and NanoMig agree, that is one
  implementation counted twice, and the four independent ones are the only
  reading of the HRM the table contains.
- **As evidence about where a fix belongs they are worth more than the other
  four**, precisely because nothing flows downhill on its own. A finding whose
  code TC64 has already corrected is a finding with a patch to point at; a
  finding all three share is one nobody has fixed anywhere.

So the table's last two rows answer a different question from its first four,
and each row says which question it is answering.

### Minimig-AGA is the ancestor, not a fifth witness — and it has two lines

NanoMig is a port of Minimig-AGA and almost everything comes from there, which is
neither news nor a problem. So a Minimig row is usually one sentence and nothing
more — unless a maintained line has **changed** the code a finding is about and
NanoMig has not picked it up. That is the only case with something to act on, and
it gets a section. The full comparison is in
[`../evidence/upstream-minimig-aga.md`](../evidence/upstream-minimig-aga.md).

**Both lines are stated once, in the comparison table, and nowhere else.** A
separate header row would describe the same two implementations twice on one
page, in two wordings that would have to be kept in step by hand.

**Two groups maintain that ancestry independently**, and the table names both:
MiSTer-devel's `Minimig-AGA_MiSTer` and minimig-dev's
`MinimigAGA-MiST-TC64`. Neither is a witness to what the manual requires — both
are descendants of the same code NanoMig descends from, so agreement between them
says nothing about the HRM. What it does say is **where a fix belongs**, and the
two lines have drifted apart far enough for that to matter.

Measured across the 28 modules the findings cite, against NanoMig at `4495f86`,
with `diff --strip-trailing-cr -b` and counting the lines either side has that
the other lacks; the first two items are measured against NanoMig at `4315acb`
as well:

- **five are identical in all three** — `agnus_audiodma.v`,
  `agnus_blitter_adrgen.v`, `denise_collision.v`, `paula_audio_mixer.v`,
  `paula_intcontroller.v`, at `4495f86` and at `4315acb`. Line numbers match as
  well as content, so a citation carries across unchanged.
- **`paula_uart.v` is identical in MiSTer and TC64, and NanoMig's at `4315acb`
  differs from both by 18 lines**: the AT-modem bridge's `serper_o` output and
  `tx_hold` input under `ENABLE_AT_MODEM`, the constant `tx_hold` without it,
  and the two transmitter terms that test it. From the end of the port list on,
  NanoMig's line numbers run 5 to 14 higher than theirs; the transmitter and the
  receiver are 10 higher.
- **`cia_int.v` is identical in TC64 and 115 lines different in MiSTer.** For
  NM-HRM-063 the closer sibling is TC64.
- **three are NanoMig's own** — `src/misc/amiga_keymap.v`, `hid.v`, `sysctrl.v` —
  and exist in neither line.
- **A module is only absent when the whole repository has no copy.** The two
  lines lay their sources out differently — TC64 spreads Verilog across
  `rtl/minimig`, `rtl/sdram`, `rtl/mist`, `rtl/audio` and more — so a comparison
  scoped to `rtl/minimig` alone would call `cpu_cache_new.v` missing.
- **`cpu_cache_new.v` sits in a different place in each line.** MiSTer keeps it
  at `rtl/cpu_cache_new.v`, 142 lines from NanoMig's; TC64 at
  `rtl/sdram/cpu_cache_new.v`, 979 lines from it — and yet the statement
  NM-HRM-060 is about is the same in all three, commented-out guard included
  (TC64's carries a `#1` delay).
  A file-level line count is not an answer to a finding.
- Where they diverge they diverge in both directions: TC64 is closer on
  `agnus_blitter.v` (whitespace only, against MiSTer's 51 lines), `ciab.v`
  (6 against 196) and the three Denise files that hold sprite and bitplane
  data (`denise_bitplanes.v` 200 against 310, `denise_sprites.v` 201 against
  209, `denise_sprites_shifter.v` 167 against 193), and much further on
  `agnus.v` (235 against 113), `minimig.v` (1225 against 601) and `userio.v`
  (584 against 165).

Comparison is against line endings normalised: NanoMig's files are **LF** and both maintained lines are **CRLF**, so a naive
byte comparison reports every line of every file as changed and says nothing.

**Checking it is not optional, though.** Every page carries the answer, because
"has this already been fixed upstream?" is the question that decides whether
there is work to do at all.

### These pages are written to be filed upstream

Where a finding is inherited, it is the **ancestor's** finding. Reporting it there is likely to get it reviewed
by people who know that code better than anyone here does. That is the
`upstream first` routing value of `Report to`; a finding about the printed page
carries `no RTL action`.

So each page is written to stand on its own for a reader who has never seen this
project: the manual quoted rather than referenced, the code quoted rather than
cited, the reasoning stated rather than assumed, and the `Report to` row naming
the repository the fix belongs in. A page should be usable as the body of an
issue with no editing beyond deleting this paragraph's worth of context.

Nothing is reported upstream without the maintainer of this repository saying so; a decision on a finding is recorded as an issue here.

## Second documents

The manual is the oracle. Commodore's other documents are not, but they can say
whether the manual's sentence is Commodore's settled statement or an outlier. A
finding page that one of them speaks to has a section *What other Commodore
documents say*.

| kind | documents | counted as |
|---|---|---|
| Commodore specifications | A500/A2000 Gary specification, undated; Denise specification 252126-01 (ECS), 1988; 8371 Agnus specification 318071-01, 1987, revised 1988; 6526 CIA specification, 1981 | one each |
| Commodore technical manuals | *A500/A2000 Technical Reference Manual*, 1987 | one |
| Commodore-Amiga patents | US 4,777,621, filed 1985, granted 1988; US 4,874,164 and 5,103,499, filed 1986, granted 1989 and 1992; US 4,829,473, filed 1986, granted 1989; US 4,780,844, filed 1986, granted 1988 | one each, except 4,874,164 and 5,103,499, which carry one specification and count once |
| Commodore articles | *AmigaMail* vol. 1, 1989 | one each |
| silicon | 6502.org's dissection of Paula 8364R7, 2023; techtravels' analysis of Paula's floppy controller, 2026; 6502.org's dissection of Denise 8373R4, a work in progress, undated | witnesses, like the emulators: third-party reverse engineering of one chip revision, not a statement of Commodore's intent |

Each year is the document's own -- a copyright line, a revision block, a
patent's filing and grant dates -- and where none is printed the document is
called undated. 5,103,499 was filed in 1989 as a division of the 1986
application behind 4,874,164, which is why the two share a specification.

Quotations are taken from each PDF's text layer or, for the three scans that
have none -- the TRM and the 8371 and 6526 specifications -- from Tesseract
OCR, errors and line-end hyphens included, and every one is checked against the
page it names by `hrm-testpipeline/tools/verify_second_documents.py --pages`.
A schematic or a patent figure cannot be quoted: where a drawing speaks, the page says what it
shows, marked *read from the drawing, not quoted*, with its PDF page, and the
check requires the net labels it names on that page. The documents themselves
are in neither repository.

## The list of findings is one file

[`../findings.md`](../findings.md) lists every open finding, once. This page does not repeat it.

`tools/check_findings_shape.py` in `hrm-testpipeline`, a separate repository, enforces the header, the routing value and what follows it, the five headings and
the link limit, and reports each page by the row or heading it lacks. It also
checks that the Module row ends with one of the values above, that the Verdict of the four
ends with its `Hardware:` clause, that no retired phrase appears, and that a withdrawn
page's Evidence is a withdrawal evidence. It checks the route against the
Module value, the form of the Effort row, the claim-id count of the Chapter row and
the title of a withdrawn page.
