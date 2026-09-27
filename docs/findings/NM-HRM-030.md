# NM-HRM-030 — Paula does not decode DMACONR, though the appendix gives it part ownership [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v) |
| **Report to** | **probably nowhere** — see *Suggested change* |
| **Test** | [`hrm/ch5-audio-dmacon-adkcon`](../../hrm/ch5-audio-dmacon-adkcon) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Verdict of the four** | unanswerable by an emulator — see below |

---

## What the HRM says

**Appendix A, printed p. 278** gives `DMACONR` at `$002` the chip column:

> `A P`

— Agnus and Paula. **Appendix B, printed p. 289** lists it among the read
registers.

The claim under test is narrow: that Paula is one of the chips answering a read
of `$002`.

---

## What NanoMig does

`paula.v` decodes `DMACON $096`, `ADKCON $09E` and `ADKCONR $010`. There is **no
`DMACONR` decode at all**; `data_out` is the OR of the UART, interrupt, disk and
`ADKCONR` sources only.

### Measured

| | `$002` read through `paula.v` |
|---|---|
| before a `DMACON` write | `0x0` |
| after `DMACON = 0x8203` sets DMAEN, AUD0EN, AUD1EN | `0x0` |

The value never changes. Paula is not driving that address.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | — | not read; no per-chip decode exists in this emulator to look for |
| **vAmiga** | — | not read; no per-chip decode exists in this emulator to look for |
| **Denise** | — | not read; no per-chip decode exists in this emulator to look for |
| **Copperline** | — | not read; no per-chip decode exists in this emulator to look for |
| **MiSTer** | [`rtl/paula.v:147`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v#L147) | descendant — carries `assign data_out = uartdata_out \| intdata_out \| diskdata_out \| adkconr;`, no `DMACONR` term, and the comment at `:153`: `//NOTE: this register is also present in the Agnus module, //there DMACONR (read) is implemented` |
| **TC64** | [`rtl/minimig/paula.v:166`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L166), [`:172`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L172) | descendant — the identical assignment and the identical comment. `DMACONR` is decoded in Agnus in all three trees; Paula answers it in none of them |

**Nobody has read any of the four independent implementations here, and for once
that is structural rather than an omission.** All four decode the custom register
space in one place; none of them models *which physical chip* answers a read. The
question this finding asks — does Paula drive `$002`? — is one only a per-chip
RTL can be asked. The manual's chip column is the only oracle available.

**MiSTer and TC64 can be asked, and they answer the same way NanoMig does.**
Both are per-chip RTL, and Paula's `data_out` carries no `DMACONR` term in either
tree — Agnus decodes it in all three. As evidence about the manual that is worth
nothing, for the reason set out in `docs/findings/README.md`: all three trees
share the same partitioning decision, so agreement among them is one reading
counted three times. What it does say is that nobody has ported a different
partitioning to either maintained line either — there is no patch anywhere to
point at.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — **agrees with NanoMig**:

  > "Note, that the CPU can't read the DMACON register."
  <!-- corpus: paula-8364r7-dissection/6_6_txt.txt -->

  Section 6ab is Paula's own DMACON copy (bits 0-4 and 9, gating its DMA requests); on the 8364R7 it has no read path, so this Paula does not answer a read of $002: NanoMig's partitioning, against the P in the appendix's chip column. ADKCON, by contrast: 'The CPU can read the ADKCON register.'

Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

---

## Suggested change

**Most likely none.** Agnus decodes `DMACONR` in NanoMig, so a read of `$002` is
answered at the top level and nothing is broken on a real machine. What fails is
the appendix's statement that Paula is *one of* the chips answering it.

Whether that matters depends on how the chip column is read:

- as a **specification** of which silicon drives which address — then Paula
  should contribute its DMA-enable bits to `$002`, and this is a partitioning
  defect inherited from Minimig-AGA;
- as **documentation of the original machine's wiring** — then consolidating the
  decode into Agnus is a legitimate implementation choice for an FPGA core that
  has no separate chips, and the finding is a note rather than a defect.

An independent reviewer called it "a boundary artifact of unit-level testing…
faithful to the appendix text, but reflecting architectural partitioning rather
than system functional failure". That reading is probably right.

---

## What holds the finding up

The assertion's mutation substitutes `ADKCONR` — an address Paula genuinely does
decode — and the assertion **flips green**. So the red is a property of `$002`
and not a dead fixture or an unreachable read path.

---

## A note on how this entry came to exist

The package measured this and then declined to assert it, recording the result as
a measurement instead. An independent validator found the suppression and named
it. The assertion exists because of that.

It is worth keeping in the record: a result that is recorded but not asserted
does not appear in any count, and that is indistinguishable from not having
looked.
