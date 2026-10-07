# NM-HRM-059 — ADKCONR's bit 15 always reads 0, and so does everyone's

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault. The red is against p.265, not against the RTL. Question: does p.265's ADKCON/ADKCONR bit-15 row describe a write-only SET/CLR control, so that a read of 0 is correct? Closed when the printed-page note below is settled one way or the other. |
| **Report to** | `no RTL action` — documentation role: the maintainer of the printed-page notes in this repository's findings register. Closure: p.265's bit-15 row is annotated as write-only, or a specialist confirms it reads back and this page is refiled against the RTL |
| **Scope** | Paula's ADKCONR read of bit 15; chip revision unknown; hardware read not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix A, printed p. 265, ADKCON/ADKCONR bit 15; test chapter 8 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Module** | `paula.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L132); inherited from Minimig-AGA |
| **Test** | [`hrm/ch8-adkcon-and-interrupt-relations`](../../hrm/ch8-adkcon-and-interrupt-relations): `make -C hrm/ch8-adkcon-and-interrupt-relations test`; an expected red exits nonzero |
| **Assertions** | 1 red, `ADKCONR-BIT15-SETCLR-NEVER-READS-BACK`: expected bit 15 of ADKCONR to follow the bit written, observed `0x7FFF`, bit 15 clear; the package reports `17 passed, 2 failed` |
| **Verdict of the four** | **unanimous with NanoMig.** All four read bit 15 as 0; only WinUAE guarantees it the same way |
| **Effort** | none — no RTL change; a documentation note on p.265 only |

---

## Source requirement

### What the HRM says

**Appendix A, printed p. 265** lists ADKCON (`09E`, write) and ADKCONR (`010`,
read) as the two registers over one shared BIT#/USE table
(`appx5-source/p265.txt`, scan reading; `appA-audio/p265.txt`, Developer CD
text). Its bit-15 row reads:

> SET/CLR Set/clear control bit. Determines if bits written with a 1 get set or
> cleared. Bits written with a zero are always unchanged.

The table is printed once, under both registers, and bit 15 has only a write
meaning. Every other row of that table reads back.

Two readings compete. This finding takes the first: ADKCONR is the read
companion of the same table, so bit 15 should read back. The second: bit 15 is a
write-only SET/CLR control, and a read of 0 is correct.

## RTL and measurement

    reg [14:0] adkcon;                                         // paula.v:132
    assign adkconr = ... ? {1'b0, adkcon[14:0]} : 16'h0000;    // paula.v:198

Fifteen bits stored, a literal zero in bit 15 on every read. The register cannot
hold the bit, so no stimulus can change the answer.

[`hrm/ch8-adkcon-and-interrupt-relations`](../../hrm/ch8-adkcon-and-interrupt-relations)
writes ADKCON bit 15 and reads ADKCONR back: `0x7FFF`, bit 15 clear regardless of
what was written, `ADKCONR-BIT15-SETCLR-NEVER-READS-BACK`, red.

## Risks and unknowns

- The decisive words are the shared bit table; p.265 never says in words whether
  ADKCONR returns bit 15, so the second reading above is not excluded by the page.
- Real Paula's read of bit 15 is not measured; no named software depends on it.
- All four store sixteen bits and none stores fifteen, so NanoMig's narrower
  register is its own, but the observable result is the same in all of them.

## Independent evidence

| | | |
|---|---|---|
| **WinUAE** | bit 15 always 0 | masked **twice** — in `setclr` (`custom.cpp:835`, called at `:3321`) and again at the read, `return adkcon & 0x7fff;` (`:2270`) |
| **vAmiga** | bit 15 always 0 in practice | read unmasked (`Core/Components/Paula/PaulaRegs.cpp:22`), write masks with `value & 0x7FFF` (`:47`) |
| **Denise** | bit 15 always 0 in practice | read unmasked (`emulation/libami/paula/paula.cpp:80`), write masks with `value & 0x7fff` (`:87`) |
| **Copperline** | bit 15 always 0 in practice | read unmasked (`src/bus/custom_regs.rs:41`), write masks (`src/chipset/paula.rs:1182-1184`) |
| **MiSTer** | bit 15 always 0 | same observable, inherited (descendant) |
| **TC64** | bit 15 always 0 | same observable, inherited (descendant) |

## Decision

No RTL change. A documentation reader decides which reading of p.265's bit-15
row stands. If it is write-only, p.265 gets a note and the red is recorded as
against the page; if it reads back, this page is refiled against the RTL and
NanoMig's fifteen-bit `adkcon` becomes the question. Re-run:
`make -C hrm/ch8-adkcon-and-interrupt-relations test`.
