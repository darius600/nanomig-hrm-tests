# NM-HRM-059 — ADKCONR's bit 15 always reads 0, and so does everyone's

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect; the red is against p.265, not against the RTL. Question: does p.265's ADKCON/ADKCONR bit-15 row describe a write-only SET/CLR control, so that a read of 0 is correct? Closes when an Amiga specialist records p.265's bit-15 row as write-only (a read of 0 is correct), or confirms that ADKCONR returns it, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Appendix A of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | Paula's ADKCONR read of bit 15; chip revision unknown; hardware read not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix A, printed p. 265, ADKCON/ADKCONR bit 15; test chapter 8 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claim ids: `ADKCONR-BIT15-SETCLR-NEVER-READS-BACK`: `opus:8-adkcon.c12`, p. 265 (no ch8 ledger; as the FAIL line prints it) |
| **Module** | `paula.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L132); inherited (MiSTer `rtl/paula.v:127`, `:193` and TC64 `rtl/minimig/paula.v:146`, `:212` carry the identical lines) |
| **Test** | [`hrm/ch8-adkcon-and-interrupt-relations`](../../hrm/ch8-adkcon-and-interrupt-relations): `make -C hrm/ch8-adkcon-and-interrupt-relations test`; an expected red exits nonzero |
| **Assertions** | 1 red, `ADKCONR-BIT15-SETCLR-NEVER-READS-BACK`: expected bit 15 of ADKCONR to follow the bit written, observed `0x7FFF`, bit 15 clear; the package reports `17 passed, 2 failed`; the other red, `SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT`, is [NM-HRM-057](NM-HRM-057.md)'s |
| **Verdict of the four** | **unanimous with NanoMig.** All four read bit 15 as 0; only WinUAE guarantees it the same way. Hardware: not measured |
| **Effort** | none — no RTL change, only a documentation note on p.265 |

---

## Source requirement

**Appendix A, printed p. 265** lists ADKCON (`09E`, write) and ADKCONR (`010`,
read) as the two registers over one shared BIT#/USE table. Its bit-15 row reads:

> SET/CLR Set/clear control bit. Determines if bits written with a 1 get set or
> cleared. Bits written with a zero are always unchanged.

The table is printed once, under both registers, and bit 15 has only a write
meaning. Every other row of that table reads back.

Two readings compete. The test takes the first: ADKCONR is the read
companion of the same table, so bit 15 should read back. The second: bit 15 is a
write-only SET/CLR control, and a read of 0 is correct.

## RTL and measurement

    reg [14:0] adkcon;                                         // paula.v:132
    assign adkconr = ... ? {1'b0, adkcon[14:0]} : 16'h0000;    // paula.v:198

Lines [`paula.v:132`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L132) and [`paula.v:198`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L198). Fifteen bits stored, a literal zero in bit 15 on every read. The register cannot
hold the bit, so no stimulus can change the answer.

[`hrm/ch8-adkcon-and-interrupt-relations`](../../hrm/ch8-adkcon-and-interrupt-relations)
writes ADKCON bit 15 and reads ADKCONR back: `0x7FFF`, bit 15 clear regardless of
what was written, `ADKCONR-BIT15-SETCLR-NEVER-READS-BACK`, red.

## Risks and unknowns

- The decisive words are the shared bit table; p.265 never says in words whether
  ADKCONR returns bit 15, so the second reading above is not excluded by the page.
- Real Paula's read of bit 15 is not measured; no named software depends on it.
- NanoMig's fifteen-bit register is inherited (MiSTer and TC64 carry the
  identical lines); a read of bit 15 returns 0 in all six implementations below.

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

No RTL change. If p.265's bit-15 row is write-only, the defect is p.265's shared table and the red stays
against the page; if it reads back, this page is refiled against the RTL and
NanoMig's fifteen-bit `adkcon` becomes the question.
