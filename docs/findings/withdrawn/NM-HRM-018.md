# NM-HRM-018 — ECS BPLCON0 bit 4 (SPRHWRM) has no effect [withdrawn]

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | p. 300 prints bit 4 as the mnemonic `SPRHWRM` with no description column, and `hrm/ch3-bplcon0-display-modes` declines both claims on the bit instead of grading them |
| **What is asked of a reader** | Nothing. p. 300 prints only the mnemonic for BPLCON0 bit 4, so no printed behaviour exists for NanoMig to diverge from. |
| **Withdrawal reason** | The manual gives bit 4 a name and no behaviour, so the claim that the bit has no effect has nothing printed to fail against |
| **Error class** | **a name read as a requirement** |
| **Replacement** | none; the package declines both claims instead of grading them |
| **Module** | `denise.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L181); inherited (the same statement is at line 146 in MiSTer and line 153 in TC64) |
| **Test** | [`hrm/ch3-bplcon0-display-modes`](../../../hrm/ch3-bplcon0-display-modes), `make -C hrm/ch3-bplcon0-display-modes test`; `233 passed, 0 failed`, and the two claims on this bit are declined |
| **Chapter** | Appendix C — ECS Hardware and the Graphics Library, printed p. 300 (BPLCON0 bit table) · *Amiga Hardware Reference Manual*, 3rd ed., 1991; claim ids `opus:C-bplcon0.b04` and `sol:C-shres.c18`, both p. 300; the package grades no assertion on either |
| **Assertions** | none graded; `opus:C-bplcon0.b04` and `sol:C-shres.c18` print `[SKIP] ... not asserted in this package` |
| **Verdict of the four** | none. Hardware: not measured |

---

## Why it was withdrawn

The claim was that bit 4 of BPLCON0 has no effect in ECS.

**Printed p. 300's row reads, in full:**

> 04       SPRHWRM

A mnemonic with **no description column**, unlike the row two above it:

> 06       SHRES       SuperHires 35ns pixel enable bit

The fixture could not have observed it in any case: it drives bitplanes only,
while the mnemonic names a sprite-horizontal function.

The package declines both claims on this bit, with that reason printed in its
`[SKIP]` lines, and asserts neither true nor false.

---

## What remains true

A source observation: `denise.v:181` gives the bit its **AGA** meaning
(`bplcon0[4] & aga`) and no ECS meaning. This is not a conformance failure,
because the manual covers OCS and ECS and prints no ECS behaviour for the bit.

---

## Independent evidence

### What other Commodore documents say

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 5 — says something else:

  > "04 y PF2H0 PF2P1 BRDNTRAN"
  <!-- corpus: denise-spec-252126/p005.txt -->

  In the ECS Denise specification BPLCON0 bit 4 is 'y', and 'y= register bits contained in AGNUS, not defined here.': no SPRHWRM name and no Denise function. That is compatible with the withdrawal (no printed behaviour) and with denise.v giving the bit no ECS meaning; what it adds is that the bit belongs to Agnus, where a Denise-only bitplane fixture could not observe it.

---

## Risks and unknowns

**Condition for filing again.** The claim would be filed again only if a
printed page gave BPLCON0 bit 4 a described ECS behaviour. No hardware was
measured.
