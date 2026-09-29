# NM-HRM-030 — Paula does not decode DMACONR, though the appendix gives it part ownership

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault: the three independent implementations that model Paula apart from Agnus read `$002` from Agnus alone, as NanoMig does, and the one dissected Paula die has no read path for its copy of DMACON. |
| **Module** | `paula.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v) |
| **Report to** | **nobody.** NanoMig, the three independent implementations that separate the chips and the dissected 8364R7 all answer `$002` from Agnus alone |
| **Test** | [`hrm/ch5-audio-dmacon-adkcon`](../../hrm/ch5-audio-dmacon-adkcon) · `DMACONR-NOT-DECODED-BY-PAULA` |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red: `DMACONR-NOT-DECODED-BY-PAULA` (`19 passed, 2 failed`; the other red is not this finding's). Its mutation, `ADKCONR` in place of `$002`, turns it green |
| **Verdict of the four** | **three of four with NanoMig, and the fourth cannot answer**: vAmiga (`Memory.cpp:2411-2412`), Denise (`agnus/register.cpp:64-65`) and Copperline (`custom_regs.rs:12-27`) keep Paula apart from Agnus and read `$002` from Agnus's DMACON alone, while routing `ADKCONR` at `$010` to Paula; WinUAE has one `DMACONR()` for the whole chip set (`custom.cpp:7048`) and no separate Paula to ask |

---

## What the HRM says

**Appendix A, printed p. 278** gives `DMACONR` at `$002` the chip column:

> `A P`

— Agnus and Paula. **Appendix B, printed p. 289** repeats the row among the read
registers:

> DMACONR \*002 R A P DMA control (and blitter status) read

Appendix B's key, **printed p. 287**, says only what the letters stand for:

> A=Agnus chip, D=Denise chip, P=Paula chip.

No page says whether a letter on a read register means that the chip drives the
data bus for the read, or only that the chip holds the register. The claim under
test takes the first reading: that Paula is one of the chips answering a read of
`$002`.

---

## What NanoMig does

`paula.v` decodes `DMACON $096`, `ADKCON $09E` and `ADKCONR $010`. There is **no
`DMACONR` decode at all**; `data_out` is the OR of the UART, interrupt, disk and
`ADKCONR` sources only
([`paula.v:152`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L152)).
Agnus answers the read, from its own copy of DMACON and the blitter's status
([`agnus.v:280-283`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L280-L283)):

```verilog
always @(*) begin
	if (reg_address[8:1]==DMACONR[8:1]) dmaconr[15:0] = {1'b0, blit_busy, blit_zero, dmacon[12:0]};
	else                                dmaconr = 0;
end
```

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
| **WinUAE** | [`custom.cpp:7048`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7048) | `case 0x002: v = DMACONR(); break;` — one read dispatch and one `DMACONR()` for the whole chip set. There is no separate Paula to ask, so it cannot answer which chip drives `$002` |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:2411-2412`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2411-L2412)<br>[`Core/Components/Memory/Memory.cpp:2425-2426`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2425-L2426) | <pre>case 0x002 &gt;&gt; 1: // DMACONR<br>&nbsp;&nbsp;&nbsp;&nbsp;result = agnus.peekDMACONR(); break;</pre> **Agnus alone.** The central dispatch hands `$002` to the Agnus object and `ADKCONR` to Paula's, `result = paula.peekADKCONR(); break;` |
| **Denise** | [`emulation/libami/agnus/register.cpp:64-65`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L64-L65)<br>[`emulation/libami/agnus/register.cpp:82-83`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L82-L83)<br>[`emulation/libami/paula/paula.cpp:101-102`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L101-L102) | <pre>case 2:<br>&nbsp;&nbsp;&nbsp;&nbsp;return dmaCon \| (blitter.busy &lt;&lt; 14) \| (blitter.zero &lt;&lt; 13);</pre> **Agnus alone.** The read returns Agnus's `dmaCon`; `ADKCONR` goes to Paula, `return paula.getAdkCon();`. Paula keeps a copy of DMACON through `auto Paula::dmaCon(uint16_t value) -> void {`, a write path with no read |
| **Copperline** | [`src/bus/custom_regs.rs:12-27`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L12-L27)<br>[`src/bus/custom_regs.rs:41`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L41) | <pre>0x002 =&gt; {<br>&nbsp;&nbsp;&nbsp;&nbsp;...<br>&nbsp;&nbsp;&nbsp;&nbsp;let mut r = self.agnus.dmacon & 0x07FF;</pre> **Agnus alone**, with the blitter's two flags; `ADKCONR` reads `self.paula.adkcon` |
| **MiSTer** | [`rtl/paula.v:147`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v#L147) | descendant — carries `assign data_out = uartdata_out \| intdata_out \| diskdata_out \| adkconr;`, no `DMACONR` term, and the comment at `:153`: `//NOTE: this register is also present in the Agnus module, //there DMACONR (read) is implemented` |
| **TC64** | [`rtl/minimig/paula.v:166`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L166), [`:172`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L172) | descendant — the identical assignment and the identical comment. `DMACONR` is decoded in Agnus in all three trees; Paula answers it in none of them |

**Every independent implementation that models Paula as a component of its own
reads `$002` from Agnus alone, as NanoMig does**, and each of the three routes
`ADKCONR`, the neighbouring Paula read, to its Paula. WinUAE decodes the whole
register space in one function and has no per-chip answer to give.

**MiSTer and TC64 answer the same way NanoMig does.** Both are per-chip RTL, and
Paula's `data_out` carries no `DMACONR` term in either tree — Agnus decodes it in
all three. As evidence about the manual that is worth nothing, for the reason set
out in `docs/findings/README.md`: all three trees share the same partitioning, so
agreement among them is one reading counted three times.

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

**None.** Agnus decodes `DMACONR` in NanoMig, so a read of `$002` is answered at
the top level with the DMA enables and the blitter's status, which is what every
implementation returns. What fails is the appendix's statement, on the reading
the test takes, that Paula is *one of* the chips answering it.

Whether that reading is the right one is the question this page leaves:

- read as **which silicon drives which read**, the `P` is contradicted by the
  dissected 8364R7 and by every implementation that separates the chips;
- read as **which chips hold the register**, the `P` is right for NanoMig too:
  `paula.v` keeps its own copy of DMACON, written at `$096`
  ([`paula.v:164`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L164)),
  as the dissected die's section 6ab does.

---

## What holds the finding up

The assertion's mutation substitutes `ADKCONR` — an address Paula genuinely does
decode — and the assertion **flips green**. So the red is a property of `$002`
and not a dead fixture or an unreachable read path.
