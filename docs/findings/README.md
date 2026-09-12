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
| [NM-HRM-020](NM-HRM-020.md) | BLTSIZV is not gated on ECS, while its two siblings are | open · red |
| [NM-HRM-021](NM-HRM-021.md) | DOFF (BLTCON1 bit 7) has no observable effect | open · red |
| [NM-HRM-022](NM-HRM-022.md) | BLTxPTH carries five high bits in both chipsets | open · red |
| [NM-HRM-024](NM-HRM-024.md) | the audio DMA pointer takes five high bits in both chipsets | open · red |
| [NM-HRM-025](NM-HRM-025.md) | volume 64 and volume 63 are the same gain on the multiply path | open · red |
| [NM-HRM-026](NM-HRM-026.md) | the audio state codes are not the ones Figure 5-8 prints | source observation |
| [NM-HRM-027](NM-HRM-027.md) | in direct mode the interrupt is raised before a word is output | open · red |
| [NM-HRM-028](NM-HRM-028.md) | ADKCON's audio attach bits are stored and reach nothing | open · red |
| [NM-HRM-030](NM-HRM-030.md) | Paula does not decode DMACONR, though the appendix gives it part ownership | open · red |
| [NM-HRM-031](NM-HRM-031.md) | INTREQ bit 14 is a settable request bit where the appendix says enable-only | open · red |
