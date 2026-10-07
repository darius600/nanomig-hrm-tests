# NM-HRM-030 — Paula does not decode DMACONR, though the appendix gives it part ownership

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this; the red is against the printed page. Question: does the `P` in the chip column of DMACONR (`$002`), printed on p. 278 and p. 289, mean that Paula answers a read of `$002`, or only that Paula holds a copy of DMACON, so that the Agnus-only answer NanoMig gives is correct? Closes when an Amiga specialist records the `P` as "holds the register" (a read answered by Agnus alone is correct), or confirms that Paula answers the read, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Appendices A and B of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | Paula's read of `$002` in `paula.v`; the one dissected Paula is the 8364R7; no other revision and no hardware read measured here |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix A, printed p. 278, and Appendix B, printed p. 289 (key on p. 287), DMACONR chip column; test chapter 5 · *Amiga Hardware Reference Manual*, 3rd ed., 1991. `DMACONR-NOT-DECODED-BY-PAULA`: `5-appA-p278.c7`, `5-appB-p289.c3`, `A-dmaconr.chip`, pp. 278 and 289; no other assertion of the package is part of this finding |
| **Module** | `paula.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v); inherited |
| **Test** | [`hrm/ch5-audio-dmacon-adkcon`](../../hrm/ch5-audio-dmacon-adkcon): `make -C hrm/ch5-audio-dmacon-adkcon test`; an expected red exits nonzero |
| **Assertions** | 1 red, `DMACONR-NOT-DECODED-BY-PAULA`: expected the `$002` read through `paula.v` to change after `DMACON = 0x8203`, observed `0x0`, equal to the idle read `0x0`; the package reports `19 passed, 2 failed`, and the other red is not this finding's |
| **Verdict of the four** | **three of four with NanoMig, and the fourth cannot answer**: vAmiga (`Memory.cpp:2411-2412`), Denise (`agnus/register.cpp:64-65`) and Copperline (`custom_regs.rs:12-27`) keep Paula apart from Agnus and read `$002` from Agnus's DMACON alone, while routing `ADKCONR` at `$010` to Paula; WinUAE has one `DMACONR()` for the whole chip set (`custom.cpp:7048`) and no separate Paula to ask. Hardware: the dissected 8364R7 gives Paula's DMACON copy no read path |
| **Effort** | none — no RTL change; a documentation note on pp. 278 and 289 only |

---

## Source requirement

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

## RTL and measurement

`paula.v` decodes `DMACON $096`, `ADKCON $09E` and `ADKCONR $010`; there is no
`DMACONR` decode. `data_out` is the OR of the UART, interrupt, disk and `ADKCONR`
sources only
([`paula.v:152`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L152)).
Agnus answers the read, from its own copy of DMACON and the blitter's status
([`agnus.v:280-283`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L280-L283)):

```verilog
always @(*) begin
	if (reg_address[8:1]==DMACONR[8:1]) dmaconr[15:0] = {1'b0, blit_busy, blit_zero, dmacon[12:0]};
	else                                dmaconr = 0;
end
```

| | `$002` read through `paula.v` |
|---|---|
| before a `DMACON` write | `0x0` |
| after `DMACON = 0x8203` sets DMAEN, AUD0EN, AUD1EN | `0x0` |

The value never changes: Paula is not driving that address.
`DMACONR-NOT-DECODED-BY-PAULA` grades it and is red.

## Risks and unknowns

- Two readings of the `P` compete. As **which silicon drives which read**, it is
  contradicted by the dissected 8364R7 and by every implementation that
  separates the chips. As **which chips hold the register**, it is right for
  NanoMig too: `paula.v` keeps its own copy of DMACON, written at `$096`
  ([`paula.v:164`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L164)),
  as the dissected die's section 6ab does.
- The dissection covers one revision, the 8364R7; no other revision and no
  hardware read of `$002` is measured.
- WinUAE cannot answer: it has no separate Paula. MiSTer and TC64 are
  descendants, and their agreement is not evidence about the manual.
- The test measures Paula's side only; the `A` half of `A P` is not measured
  there.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:7048`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7048) | `case 0x002: v = DMACONR(); break;` — one read dispatch and one `DMACONR()` for the whole chip set. There is no separate Paula to ask, so it cannot answer which chip drives `$002` |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:2411-2412`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2411-L2412)<br>[`Core/Components/Memory/Memory.cpp:2425-2426`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2425-L2426) | <pre>case 0x002 &gt;&gt; 1: // DMACONR<br>&nbsp;&nbsp;&nbsp;&nbsp;result = agnus.peekDMACONR(); break;</pre> **Agnus alone.** The central dispatch hands `$002` to the Agnus object and `ADKCONR` to Paula's, `result = paula.peekADKCONR(); break;` |
| **Denise** | [`emulation/libami/agnus/register.cpp:64-65`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L64-L65)<br>[`emulation/libami/agnus/register.cpp:82-83`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L82-L83)<br>[`emulation/libami/paula/paula.cpp:101-102`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L101-L102) | <pre>case 2:<br>&nbsp;&nbsp;&nbsp;&nbsp;return dmaCon \| (blitter.busy &lt;&lt; 14) \| (blitter.zero &lt;&lt; 13);</pre> **Agnus alone.** The read returns Agnus's `dmaCon`; `ADKCONR` goes to Paula, `return paula.getAdkCon();`. Paula keeps a copy of DMACON through `auto Paula::dmaCon(uint16_t value) -> void {`, a write path with no read |
| **Copperline** | [`src/bus/custom_regs.rs:12-27`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L12-L27)<br>[`src/bus/custom_regs.rs:41`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L41) | <pre>0x002 =&gt; {<br>&nbsp;&nbsp;&nbsp;&nbsp;...<br>&nbsp;&nbsp;&nbsp;&nbsp;let mut r = self.agnus.dmacon & 0x07FF;</pre> **Agnus alone**, with the blitter's two flags; `ADKCONR` reads `self.paula.adkcon` |
| **MiSTer** | [`rtl/paula.v:147`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v#L147) | descendant — carries `assign data_out = uartdata_out \| intdata_out \| diskdata_out \| adkconr;`, no `DMACONR` term, and the comment at `:153`: `//NOTE: this register is also present in the Agnus module, //there DMACONR (read) is implemented` |
| **TC64** | [`rtl/minimig/paula.v:166`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L166), [`:172`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L172) | descendant — the identical assignment and the identical comment. `DMACONR` is decoded in Agnus in all three trees; Paula answers it in none of them |

### What other Commodore documents say

**6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — **agrees with NanoMig**:

> "Note, that the CPU can't read the DMACON register."
<!-- corpus: paula-8364r7-dissection/6_6_txt.txt -->

Section 6ab is Paula's own DMACON copy (bits 0-4 and 9, gating its DMA requests); on the 8364R7 it has no read path, so this Paula does not answer a read of $002: NanoMig's partitioning, against the P in the appendix's chip column. ADKCON, by contrast: 'The CPU can read the ADKCON register.'

## Decision

No RTL change. Agnus decodes `DMACONR` in NanoMig, so a read of `$002` returns the
DMA enables and the blitter's status, which is what every implementation returns.
What fails is the appendix's statement, on the reading the test takes, that Paula
is *one of* the chips answering it.
