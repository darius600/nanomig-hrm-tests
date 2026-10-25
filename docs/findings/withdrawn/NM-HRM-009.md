# NM-HRM-009 — CLXDAT bit 15 reads as 1, not 0 [withdrawn]

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | p. 273 lists bit 15 as `not used` under the heading COLLISIONS REGISTERED and gives it no read value; `BIT15-UNUSED`, which asks only that bit 15 read the same before and after maximal collision stimulus, is green |
| **What is asked of a reader** | Nothing. A column heading is not a specification of the bits underneath it: p. 273 gives bit 15 no value to read. |
| **Withdrawal reason** | The expected 0 came from the assertion, not from the page; NanoMig wires bit 15 high, which contradicts nothing printed |
| **Error class** | **a silence read as a requirement** |
| **Replacement** | Green: `BIT15-UNUSED` in [`hrm/ch3-collision-registers`](../../../hrm/ch3-collision-registers) (bit 15 unchanged between an idle read and a read after all six entities collide) |
| **Module** | `denise_collision.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_collision.v#L117); line 117 is identical in MiSTer and TC64; inherited |
| **Test** | `make -C hrm/ch3-collision-registers test`; the package prints `32 passed, 4 failed` — the four reds are other assertions, and an expected red exits nonzero |
| **Chapter** | Appendix A, printed p. 273 (CLXDAT bit table) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · `BIT15-UNUSED` carries `opus:A-clxdat.b15`, `sol:A-clxdat.c6`, `opus:7-appA-clxdat.c5` and `sol:7-appA-clxdat.c8`, p. 273, and `opus:7-cd.c8` and `sol:7-collision.c10`, p. 214 (the assertion message prints the first two without their prefix) |
| **Assertions** | `BIT15-UNUSED` green (its earlier expectation, bit 15 reads 0, was the withdrawn claim) |
| **Verdict of the four** | **unanimous with NanoMig**: WinUAE, vAmiga, Denise and Copperline all OR `0x8000` into the value a CLXDAT read returns. Hardware: not measured |
| **See also** | [NM-HRM-008](../NM-HRM-008.md) |

---

## Why it was withdrawn

The claim was that CLXDAT bit 15 must read 0. Printed p. 273 prints, as its
column heading and its bit-15 row:

> COLLISIONS REGISTERED

> 15 not used

The heading scopes the row: no collision is registered at bit 15. It does not
say the bit reads zero. NanoMig returns bit 15 as 1 on every CLXDAT read
([`denise_collision.v#L117`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_collision.v#L117)),
which contradicts nothing printed, so the red measured the oracle, not the RTL.
The assertion now requires bit 15 to hold the same value before and after
maximal collision stimulus, without claiming which value, and it passes.

**Condition for filing again.** The claim would be filed again if a printed page
gave bit 15 a value to read; p. 273 does not.

---

## Independent evidence

| | |
|---|---|
| **WinUAE** | **as NanoMig.** [`custom.cpp:4016`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L4016) returns `clxdat \| 0x8000` |
| **vAmiga** | **as NanoMig.** [`DeniseRegs.cpp:319`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/DeniseRegs.cpp#L319) `peekCLXDAT` returns `clxdat \| 0x8000` |
| **Denise** | **as NanoMig.** [`denise.cpp:313`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L313) `getClxDat` returns `clxDat \| 0x8000` |
| **Copperline** | **as NanoMig.** [`denise.rs:538`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/denise.rs#L538) `read_clxdat` returns the register `\| 0x8000` |
| **MiSTer** | descendant — [`rtl/denise_collision.v:117`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L117) is identical to NanoMig's |
| **TC64** | descendant — [`rtl/minimig/denise_collision.v:117`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_collision.v#L117) is identical to NanoMig's |

### What other Commodore documents say

- **US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989**, PDF page 29 — agrees with the manual:

  > "Fifteen bits in the collision storage register are utilized to indicate whether any of several types of overlaps has occurred"
  <!-- corpus: patent-us4874164-agnus-denise/p029.txt -->

  Fifteen of sixteen bits carry collisions, the manual's 'not used' for the remaining bit in other words; like the manual it gives that bit no value (p020.txt calls the input 'A 16-bit output from the collision detection logic'). Same passage in patent-us5103499-copper/p029.txt.

- **Commodore, *Denise* specification, part 252126-01 (ECS), 1988**, PDF page 7 — agrees with the manual:

  > "15 Not Used"
  <!-- corpus: denise-spec-252126/p007.txt -->

  The ECS Denise specification repeats the manual's row under the same heading 'COLLISIONS REGISTERED' and also gives no read value for bit 15.
