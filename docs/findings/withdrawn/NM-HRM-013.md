# NM-HRM-013 — SPRxCTL bits 3 and 4 are read nowhere [withdrawn]

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | p. 303 prints bit 3 as `(unimplemented)`, and with SPR0CTL bit 3 set against clear the `sprdata` trace is identical at each of the four `spres` values |
| **What is asked of a reader** | Nothing. p. 303 marks bit 3 `(unimplemented)`. The surviving half, bit 4, is [NM-HRM-019](../NM-HRM-019.md). |
| **Withdrawal reason** | The note says which chips have the bits, not that both alter output, and p. 303 itself prints bit 3 as `(unimplemented)`, so no page requires bit 3 to be a live field |
| **Error class** | **a scope note read as a requirement** |
| **Replacement** | Green: `SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES` in [`hrm/ch3-sprite-registers`](../../../hrm/ch3-sprite-registers) (bit 3 alone leaves the `sprdata` trace identical at each of the four `spres` values). Bit 4 is a behaviour and is re-asserted on its own as [NM-HRM-019](../NM-HRM-019.md) |
| **Module** | `denise_sprites.v`; SPRxCTL is written in `denise_sprites_shifter.v`, which keeps bits 7 and 0 only — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_sprites_shifter.v#L75); MiSTer (`rtl/denise_sprites_shifter.v:95`) and TC64 (`rtl/minimig/denise_sprites_shifter.v:91`) have the same statement: inherited |
| **Test** | `make -C hrm/ch3-sprite-registers test`; the package reads `10 passed, 1 failed`: the green assertion above is among the 10 and the one red is NM-HRM-019's, so the command exits nonzero |
| **Chapter** | Appendix C, printed p. 303 (SPRxCTL bit 3 and bit 4 rows, and the ECS note) · *Amiga Hardware Reference Manual*, 3rd ed., 1991. Claim ids: `SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES`: `opus:APPC-pos.c11`, `sol:CH4-appC-position.c19`, p. 303. `SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED` is declined, so it is not an assertion and carries `sol:C-shres-sprite.c13` as a declined claim, p. 303 |
| **Assertions** | `SPRXCTL-BIT3-SHSH0-INERT-AT-EVERY-SPRES` green; `SPRXCTL-BIT3-SHSH0-UNIMPLEMENTED` declined, quoting the manual's own word, with no red or green state |
| **Verdict of the four** | none |
| **See also** | [NM-HRM-019](../NM-HRM-019.md) |

---

## Why it was withdrawn

The finding rested on the note:

> "bits 3 and 4 are in the ECS chips only"

That is a statement about **which chips have the bits**, not a promise that both
alter output. Requiring bit 3 to be a live field contradicts the oracle, because
the same page settles bit 3 in the opposite direction:

> 03      SHSH0     Start horizontal (SHR mode) 35ns (unimplemented)

**The manual itself calls bit 3 unimplemented.** Bit 4 prints a behaviour and is re-asserted as [NM-HRM-019](../NM-HRM-019.md):

> 04      SHSH1     Start horizontal (SHR mode) 70ns increment

---

## Risks and unknowns

- **Condition for filing again.** The claim for bit 3 would be filed again only if a printed page gave SHSH0 a behaviour in place of `(unimplemented)`; p. 303 as printed does not.
- **Reach of the green assertion.** It sets SPR0CTL bit 3 alone, with SH0 and the other bits clear, at SPR0POS `$40`, in the `denise_sprites` fixture without AGA, and compares the `sprdata` trace over hpos 0 to 511. Other positions, bit 3 together with other bits, and a chip are not measured.
- **The second document may be a carry-over.** The ECS Denise specification below has the shape of an OCS-era table and lacks even the EV8 row.

---

## Independent evidence

### What other Commodore documents say

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 10 — **agrees with NanoMig**:

  > "06-04 X Not used."
  <!-- corpus: denise-spec-252126/p010.txt -->

  This ECS Denise specification (it reports 8373 revisions via DENISEID) prints SPRxCTL bits 06-04 as not used and has no row for bit 03: on bit 4 it sides with NanoMig reading neither bit, against the 3rd edition's SHSH1 70ns increment; bit 3 the manual itself calls unimplemented. The table has the shape of an OCS-era table and lacks even the EV8 row, so it may be an un-updated carry-over. It bears equally on NM-HRM-019, the surviving half.
