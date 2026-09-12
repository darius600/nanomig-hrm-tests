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

### Minimig-AGA is not a fifth witness

It is the **ancestor**. NanoMig's audio files carry the same names and, where
checked so far, the same content. So for every finding there is a question the
four emulators cannot answer and this one can:

> **Is this NanoMig's, or did it come down from Minimig-AGA?**

That decides where a fix belongs, and whether fixing it in NanoMig means
diverging from upstream. `NM-HRM-025` is the first entry to carry the answer:
inherited, byte-identical.

An emulator agreeing with the manual is one more reading of the manual. Upstream
agreeing with NanoMig is a different fact entirely — it is the same decision,
made once, further back.

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
| [NM-HRM-025](NM-HRM-025.md) | volume 64 and volume 63 are the same gain on the multiply path | open · red |
