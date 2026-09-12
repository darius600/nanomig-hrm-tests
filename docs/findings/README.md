# Findings — one page each

A reference for developers, not a narrative. Every page has the same six parts,
in the same order, so a page can be read by jumping to the part you need:

1. **What the HRM says** — printed page and verbatim quotation.
2. **What NanoMig does** — the code, quoted, with a permalink to the exact line
   at the pinned revision.
3. **What the others say** — WinUAE, vAmiga, Denise, Copperline, each with the
   line quoted rather than merely cited.
4. **Suggested change** — where one can be stated. It is a suggestion; what to
   do about the RTL is for Amiga specialists.
5. **What holds the finding up** — the control case, the mutation, the reason a
   red is not a test defect.
6. **For the specialists** — the residue that needs judgement rather than
   measurement.

The register table with statuses and packages stays in
[`../findings.md`](../findings.md); these pages are what it links to.

## Pinned revisions

Every link in these pages is a permalink at the revision that was read, and
every quotation was verified in a local checkout at that revision.

| what | repository | revision |
|---|---|---|
| **NanoMig** — the design under test | [harbaum/NanoMig](https://github.com/harbaum/NanoMig) | [`d6f7df9`](https://github.com/harbaum/NanoMig/tree/d6f7df957fad655bc07f6a381e5094a9edff0be0) · v0.14-beta-33 |
| **Minimig-AGA** — what NanoMig descends from | [MiSTer-devel/Minimig-AGA_MiSTer](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer) | [`da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/tree/da7632e44498840850e78966947f8770166e6e77) |
| WinUAE | [tonioni/WinUAE](https://github.com/tonioni/WinUAE) | [`ccc407e`](https://github.com/tonioni/WinUAE/tree/ccc407e9c636668b99b5f19d6e9aa03ea04b3052) |
| vAmiga | [dirkwhoffmann/vAmiga](https://github.com/dirkwhoffmann/vAmiga) | [`35de004`](https://github.com/dirkwhoffmann/vAmiga/tree/35de004a2f1502c2811b9a8ebcde4d0979a98937) |
| Denise | [piciji/denise](https://github.com/piciji/denise) | [`1cb7d45`](https://github.com/piciji/denise/tree/1cb7d45117893c76025d32260edb6008001344f9) |
| Copperline | [CopperlineHQ/Copperline](https://github.com/CopperlineHQ/Copperline) | [`726bed0`](https://github.com/CopperlineHQ/Copperline/tree/726bed00c407727c1f5bd32c7193587986638941) |

All six are checked out locally under `_emulators/` in the workspace, outside
either repository, so a citation can be re-checked rather than trusted.

### Minimig-AGA is the ancestor, not a fifth witness

NanoMig is a port of Minimig-AGA and almost everything comes from there, which is
neither news nor a problem. So the `Upstream` row is one line and nothing more —
unless upstream has **changed** the code a finding is about and NanoMig has not
picked it up. That is the only case with something to act on, and it gets a
section. The full comparison is in
[`../upstream-minimig-aga.md`](../upstream-minimig-aga.md).

**Checking it is not optional, though.** Every page carries the answer, because
"has this already been fixed upstream?" is the question that decides whether
there is work to do at all.

### These pages are written to be filed upstream

Where a finding is inherited — which so far is all of chapter 5 — it is the
**ancestor's** finding, and a fix in Minimig-AGA reaches NanoMig by the usual
route. Reporting it there is likely to get it reviewed by people who know that
code better than anyone here does.

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

## Index

Pages are added here as they are written. `findings.md` remains complete in the
meantime.

| finding | subject | status |
|---|---|---|
| [NM-HRM-001](NM-HRM-001.md) | PAL frame has one line too many | WITHDRAWN — the manual sanctions the measured value |
| [NM-HRM-002](NM-HRM-002.md) | audio channels on the wrong stereo side | open · red |
| [NM-HRM-003](NM-HRM-003.md) | OCS Copper address uses five high bits instead of three | open · red |
| [NM-HRM-004](NM-HRM-004.md) | ECS danger bit admits too small a range | open · red |
| [NM-HRM-005](NM-HRM-005.md) | WAIT and SKIP take longer than the manual states | open · red |
| [NM-HRM-006](NM-HRM-006.md) | DDFSTRT/DDFSTOP bit H3 is gated by the chipset, not by resolution | open · red |
| [NM-HRM-007](NM-HRM-007.md) | the hires fetch width cannot reach the manual's maximum of 49 words | open · red |
| [NM-HRM-008](NM-HRM-008.md) | playfield-1 collision depends on playfield 2, by design, undocumented | open · red — but see the first section |
| [NM-HRM-009](NM-HRM-009.md) | CLXDAT bit 15 reads as 1, not 0 | WITHDRAWN — the expected value was invented by the assertion |
| [NM-HRM-010](NM-HRM-010.md) | the horizontal line is a whole number of colour clocks, not 227.5 | source observation, not test-backed |
| [NM-HRM-011](NM-HRM-011.md) | PAL vertical blanking is 26 lines, the manual says 29 | open · red |
| [NM-HRM-012](NM-HRM-012.md) | SPRxCTL bit 0 (SH0) is honoured at every resolution, not only SuperHires | WITHDRAWN — the assertion imported a restriction the manual does not state |
| [NM-HRM-013](NM-HRM-013.md) | SPRxCTL bits 3 and 4 are read nowhere | WITHDRAWN, split into [NM-HRM-019](NM-HRM-019.md) |
| [NM-HRM-014](NM-HRM-014.md) | the manual applies its own ECS pointer extension inconsistently | source observation, not test-backed |
| [NM-HRM-015](NM-HRM-015.md) | BRDNTRAN is declared and never connected | open · red |
| [NM-HRM-016](NM-HRM-016.md) | SuperHires sprite colour encoding is not implemented | open · red — 16 evidential reds, the largest single red result in chapter 3 |
| [NM-HRM-017](NM-HRM-017.md) | SuperHires bitplane colour encoding is not implemented | open · red |
| [NM-HRM-018](NM-HRM-018.md) | ECS BPLCON0 bit 4 (SPRHWRM) has no effect | WITHDRAWN as a conformance finding — a source observation remains |
| [NM-HRM-019](NM-HRM-019.md) | SPRxCTL bit 4 (SHSH1) is not implemented | open · red |
| [NM-HRM-020](NM-HRM-020.md) | BLTSIZV is not gated on ECS, while its two siblings are | open · red |
| [NM-HRM-021](NM-HRM-021.md) | DOFF (BLTCON1 bit 7) has no observable effect | open · red |
| [NM-HRM-022](NM-HRM-022.md) | BLTxPTH carries five high bits in both chipsets | open · red |
| [NM-HRM-023](NM-HRM-023.md) | the ATTACH bit is honoured in the even sprite, not only the odd | open · red |
| [NM-HRM-024](NM-HRM-024.md) | the audio DMA pointer takes five high bits in both chipsets | open · red |
| [NM-HRM-025](NM-HRM-025.md) | volume 64 and volume 63 are the same gain on the multiply path | open · red |
| [NM-HRM-026](NM-HRM-026.md) | the audio state codes are not the ones Figure 5-8 prints | source observation, not a conformance failure |
| [NM-HRM-027](NM-HRM-027.md) | in direct mode the interrupt is raised before a word is output | open · red |
| [NM-HRM-028](NM-HRM-028.md) | ADKCON's audio attach bits are stored and reach nothing | open · red |
| [NM-HRM-029](NM-HRM-029.md) | Figure 5-8's `dmasen` note does not hold outside its own exception | WITHDRAWN — the assertion was logically false |
| [NM-HRM-030](NM-HRM-030.md) | Paula does not decode DMACONR, though the appendix gives it part ownership | open · red |
| [NM-HRM-031](NM-HRM-031.md) | INTREQ bit 14 is a settable request bit where the appendix says enable-only | open · red |
