# Findings — one page each

A reference for developers, not a narrative.

**Sixty-five of these pages are filed against the Hardware Reference
Manual and three against the AA specification.** The AA trio (NM-AA-001,
NM-AA-002 and NM-AA-003) is counted separately in [`../findings.md`](../findings.md), and
the tests that hold them up live in `aa/` rather than `hrm/`. The header
table is the same on all sixty-eight.

**What every page has, in the same place: the header table.** These seven fields
are on all 68 pages, and the first four answer *what am I being asked to do*
before any prose is read:

| field | values |
|---|---|
| **Lifecycle** | `open` or `withdrawn` |
| **Filed against** | `the RTL`, `the printed page`, `the classification`, or `—` when withdrawn |
| **Evidence** | a graded red (plain, contested or inverted), a graded green with an inverse control, a source observation, or none |
| **What is asked of a reader** | one sentence. If it begins *No RTL change* or *Nothing*, the rest of the page is context |
| **Module** | the RTL under discussion, permalinked at the pinned revision |
| **Test** | the package that measures it, or `none yet` |
| **Chapter** | which part of the manual judges it |

**Eight further fields appear on some pages and not others**, and which ones a
page carries says something about it:

| field | on | what its absence means |
|---|---|---|
| **Verdict of the four** | 63 of 68 | the other implementations were not consulted, or the finding is not about behaviour they share |
| **Report to** | 61 of 68 | nothing is being asked of anyone upstream — usually a withdrawn finding |
| **Assertions** | 36 of 68 | the finding has no graded assertions of its own; the `Test` row still names the package |
| **See also** | 9 | no sibling finding bears on it |
| **Error class** | 5 | it is not one of the recurring shapes |
| **Boundary** | 2 | no edge case was isolated |
| **Provenance**, **Origin** | 1 each | one-offs, kept because the page needed them |

A header table that is shorter than the one on the page before it is therefore
not an incomplete page. Counted 20.09.2026 across all 65.

**What follows the header varies, and deliberately.** Most pages run *What the
HRM says* → *What NanoMig does* → *What the others say* → *Suggested change* →
*What holds the finding up*, some adding *For the specialists*. A withdrawn
finding instead explains the withdrawal, and a finding filed against the printed
page has no suggested change to make. Measured over the 65 pages there are 50
distinct heading sequences; the page is written for its own subject once the
header has told you whether to read it at all.

Where implementations are compared, six are quoted rather than cited: WinUAE,
vAmiga, Denise and Copperline, then the two maintained Minimig lines, **MiSTer**
and **TC64**. The last two are marked as descendants rather than witnesses, for
the reason set out below; they sit in the same table because a reader comparing
implementations wants them side by side.

**[`../findings.md`](../findings.md) is the register** — every finding, its
status, its package and its third witness, in one table. It is the list to go to;
this directory holds what it links to.

## Pinned revisions

Every link in these pages is a permalink at the revision that was read, and
every quotation was verified in a local checkout at that revision.

| what | repository | revision |
|---|---|---|
| **NanoMig** — the design under test | [harbaum/NanoMig](https://github.com/harbaum/NanoMig) | [`d6f7df9`](https://github.com/harbaum/NanoMig/tree/d6f7df957fad655bc07f6a381e5094a9edff0be0) · v0.14-beta-33 |
| **Minimig-AGA** — what NanoMig descends from | [MiSTer-devel/Minimig-AGA_MiSTer](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer) | [`da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/tree/da7632e44498840850e78966947f8770166e6e77) |
| **Minimig-AGA on MiST/TC64** — the second maintained line | [minimig-dev/MinimigAGA-MiST-TC64](https://github.com/minimig-dev/MinimigAGA-MiST-TC64) | [`9e8e308`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/tree/9e8e30844fe134dca54cea65f1bd38539dfc15ad) · 2025-05-03 |
| WinUAE | [tonioni/WinUAE](https://github.com/tonioni/WinUAE) | [`ccc407e`](https://github.com/tonioni/WinUAE/tree/ccc407e9c636668b99b5f19d6e9aa03ea04b3052) |
| vAmiga | [dirkwhoffmann/vAmiga](https://github.com/dirkwhoffmann/vAmiga) | [`35de004`](https://github.com/dirkwhoffmann/vAmiga/tree/35de004a2f1502c2811b9a8ebcde4d0979a98937) |
| Denise | [piciji/denise](https://github.com/piciji/denise) | [`1cb7d45`](https://github.com/piciji/denise/tree/1cb7d45117893c76025d32260edb6008001344f9) |
| Copperline | [CopperlineHQ/Copperline](https://github.com/CopperlineHQ/Copperline) | [`726bed0`](https://github.com/CopperlineHQ/Copperline/tree/726bed00c407727c1f5bd32c7193587986638941) |

All six are checked out locally under `_emulators/` in the workspace, outside
either repository, so a citation can be re-checked rather than trusted.

### The two Minimig lines are ancestors, not witnesses — and nothing merges them down

**NanoMig was forked from MiSTer's Minimig-AGA, and there is at present no
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
[`../upstream-minimig-aga.md`](../upstream-minimig-aga.md).

**Both lines are stated once, in the comparison table, and nowhere else.** Until
18.09.2026 every finding also carried a separate `Upstream` header row, so the
same two implementations were described twice on one page, in two wordings that
had to be kept in step by hand. The row is gone; whatever it said that the table
did not is now in the table.

**Two groups maintain that ancestry independently**, and the table names both:
MiSTer-devel's `Minimig-AGA_MiSTer` and minimig-dev's
`MinimigAGA-MiST-TC64`. Neither is a witness to what the manual requires — both
are descendants of the same code NanoMig descends from, so agreement between them
says nothing about the HRM. What it does say is **where a fix belongs**, and the
two lines have drifted apart far enough for that to matter.

Measured across the 28 modules the findings cite, against NanoMig at `d6f7df9`:

- **six are identical in all three** — `agnus_audiodma.v`,
  `agnus_blitter_adrgen.v`, `denise_collision.v`, `paula_audio_mixer.v`,
  `paula_intcontroller.v`, `paula_uart.v`. Line numbers match as well as content,
  so a citation carries across unchanged.
- **`cia_int.v` is identical in TC64 and 113 lines different in MiSTer.** For
  NM-HRM-063 the closer sibling is the one this project had not been looking at.
- **three are NanoMig's own** — `src/misc/amiga_keymap.v`, `hid.v`, `sysctrl.v` —
  and exist in neither line.
- **A module is only absent when the whole repository has no copy.** The two
  lines lay their sources out differently — TC64 spreads Verilog across
  `rtl/minimig`, `rtl/sdram`, `rtl/mist`, `rtl/audio` and more — so a comparison
  scoped to one directory reports absences that are not there. This one was
  scoped to `rtl/minimig` first and called `cpu_cache_new.v` missing.
- **`cpu_cache_new.v` sits in a different place in each line.** MiSTer keeps it
  at `rtl/cpu_cache_new.v`, 150 lines from NanoMig's; TC64 at
  `rtl/sdram/cpu_cache_new.v`, 1105 lines from it — and yet the statement
  NM-HRM-060 is about is identical in all three, commented-out guard included.
  A file-level line count is not an answer to a finding.
- Where they diverge they diverge in both directions: TC64 is closer on
  `agnus_blitter.v` (14 lines against 61) and `ciab.v` (6 against 218), and much
  further on `agnus.v` (741 against 115), `minimig.v` (1563 against 811) and
  `userio.v` (736 against 165).

Comparison is against line endings normalised: NanoMig's files are **LF** and both maintained lines are **CRLF**, so a naive
byte comparison reports every line of every file as changed and says nothing.

**Checking it is not optional, though.** Every page carries the answer, because
"has this already been fixed upstream?" is the question that decides whether
there is work to do at all.

### These pages are written to be filed upstream

Where a finding is inherited — which so far is all of chapter 5 and all of chapter 7, the latter checked line by line because two of its files differ substantially from upstream — it is the
**ancestor's** finding. A fix in Minimig-AGA does not reach NanoMig by itself --
see the paragraph above: somebody ports it deliberately. Reporting it there is likely to get it reviewed by people who know that
code better than anyone here does. Two of chapter 7's five name nobody in their `Report to` row all the same: the other implementations agree with NanoMig there, and the printed sentence is the more likely defect.

So each page is written to stand on its own for a reader who has never seen this
project: the manual quoted rather than referenced, the code quoted rather than
cited, the reasoning stated rather than assumed, and the `Report to` row naming
the repository the fix belongs in. A page should be usable as the body of an
issue with no editing beyond deleting this paragraph's worth of context.

Nothing is filed anywhere without the maintainer of this repository saying so.

## Withdrawn findings are kept, and here is why

Six entries are withdrawn. They are **not** relevant to the question "what should
be changed in NanoMig", and they are listed separately for that reason. They are
relevant to two other questions:

- **Would someone re-file this?** `NM-HRM-001` stood as a documented finding for
  weeks before Table 7-5 turned out to sanction the measured value. Without the
  entry, the next reader of that page files it again.
- **What kind of mistake was it?** Four of the six are one error wearing four
  hats: a permission or a silence in the manual read as a requirement. That is
  the most common way this suite produces a false finding, and the four entries
  together are the clearest statement of it in the project.

So they keep their pages, in the same structure, with the reason for the
withdrawal stated where the suggested change would otherwise be.

## The list of findings is one file

[`../findings.md`](../findings.md). This page used to repeat it as a table, which
stopped at NM-HRM-031 while the register grew to 65 — so the copy said the work
ended thirty-four findings before it did. One register, kept in one place.
