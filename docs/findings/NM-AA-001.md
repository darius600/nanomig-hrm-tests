# NM-AA-001 — the AA specification gives CLXCON2 two different addresses, and one of them belongs to BPLCON4

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test passes; the page's other reading fails |
| **What is asked of a reader** | Should a claim about CLXCON2's address rest on the AA specification's p. 8 address-list entry (`$10E`) and not on its p. 18 alphabetical entry (`$10C`, which pp. 8 and 16 give to BPLCON4)? Nothing in the RTL changes: NanoMig already decodes CLXCON2 at `$10E`. Closes when an Amiga specialist records the p. 18 entry's `$10C` as an erratum to the AA specification, or confirms it as intended, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — AA documentation: an issue on this repository whose title begins with NM-AA-001 |
| **Scope** | CLXCON2 and BPLCON4 addresses in the AA chip set, as the PRELIMINARY AA specification of 06/07/91 prints them; chip revision unknown; isolated to the CLXCON2 address, with CLXDAT at `$00E` and CLXCON at `$098` measured alongside |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF pp. 4, 6, 7, 8, 16, 18. Claim ids by assertion: `CLXCON2-AT-10E`: `opus:AA-clxcon2.c1`, `sol:AA-clxcon2.c1`, address list p. 8; `CLXCON2-NOT-AT-10C`: `opus:AA-clxcon2.c2`, p. 18; `CLXCON2-CLEARED-BY-CLXCON`: `opus:AA-clxcon2.c3`, p. 4; `CLXDAT-AT-00E`: `opus:AA-clxdat.c1`, `sol:AA-clxdat.c1`, p. 6; `CLXCON-AT-098`: `opus:AA-clxcon.c1`, `sol:AA-clxcon.c1`, p. 7. Every assertion carries a claim id |
| **Module** | `src/minimig-aga/denise_collision.v:20` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/denise_collision.v#L20) at the pinned revision; inherited (MiSTer and TC64 carry the identical line) |
| **Test** | [`aa/aa-clxcon2-address`](../../aa/aa-clxcon2-address) · `make -C aa/aa-clxcon2-address test`, or `hrm/run_all.sh --in ../aa` (that one runs the whole `aa/` tree) |
| **Assertions** | none red. Five graded, all green: `CLXCON2-AT-10E` (unwritten, `bpldata=0x7F` collides; after `$00C3` is written to `$10E` it does not), `CLXCON2-NOT-AT-10C`, the inverse control (a write to `$10C` does not change the match, so the p. 18 reading of `$10C` fails), `CLXCON2-CLEARED-BY-CLXCON` (armed, it does not collide; after a CLXCON write it does), `CLXDAT-AT-00E` (a read at `$00E` answers with bit 15 set and `$010` is silent), `CLXCON-AT-098` (a write at `$098` clears CLXCON2 and a write at `$09A` does not). `aa-clxcon2-address: 5 passed, 0 failed` |
| **Verdict of the four** | **unanimous for $10E, and it leaves the document's other half with nobody behind it.** Three of the four decode CLXCON2, and all three decode it at `$10E` with BPLCON4 at `$10C`. The fourth implements neither register and still names both at those addresses in its own register table. Not one of them puts CLXCON2 at `$10C`. Hardware: not measured |
| **Effort** | none — no RTL change, nothing in `denise_collision.v` is touched |

---

## Source requirement

The address-ordered register list, PDF p. 8:

```
CLXCON2    p 10E W   D Extended collision control register
```

The alphabetical section, PDF p. 18:

```
CLXCON2 P  10C  W    D    Extended Collision Control
```

`$10C` is not a free address. The same document gives it to BPLCON4, in the list on p. 8 and in the BPLCON bit table on p. 16:

```
BPLCON4    p 10C W   D Bit plane control reg (bitplane and sprite masks)
BPLCON4 P 10C  W     D       Bit Plane control reg.(display masks)
```

So one of the two CLXCON2 statements puts it on top of another register. PDF p. 4 adds: *"CLXCON2 is reset by a write to CLXCON, so that old game programs will be able to correctly detect collisions."*

## RTL and measurement

```verilog
parameter CLXCON2 = 9'h10e;
else if (aga && (reg_address_in[8:1] == CLXCON2[8:1]))
  clxcon2 <= #1 data_in;
```

NanoMig decodes CLXCON2 at `$10E`, gated on AGA; the gate is measured from the RTL, not graded against the page. CLXCON2 has no read path at `denise_collision`, so [`aa/aa-clxcon2-address`](../../aa/aa-clxcon2-address) reads it through the collision match: `clxcon2[7:6]` enable bitplanes 7 and 8 and `clxcon2[1:0]` carry their match values, so with the register zero bit 7 of `bpldata` cannot affect a collision and with `$00C3` written it can. Driving `bpldata=0x7F`, a write to `$10E` changes the match (`CLXCON2-AT-10E`), a write to `$10C` does not (`CLXCON2-NOT-AT-10C`), and a write to CLXCON clears it again (`CLXCON2-CLEARED-BY-CLXCON`). The other two assertions decode CLXDAT at `$00E` and CLXCON at `$098` through the same fixture.

## Risks and unknowns

- **Whether this PRELIMINARY specification can serve as a conformance oracle at all, and with what caveat, is unsettled.** It is a judgement about the document, not a measurement.
- **The tie-break is a later header, derived.** `custom.h` 39.1 gives addresses as struct offsets, not written constants, so they are derived rather than quoted; the derivation agrees with the specification's list on eleven of eleven registers spot-checked.
- **Not measured on hardware**; the `$10E` reading rests on the header and three implementations.
- **Quotation reliability.** All three readings agree on every quoted line except the second line of the p. 4 sentence, which carries no identifier to match on; the OCR and the `AGA.guide` retyping both carry its words.
- **CLXCON2 is only observed indirectly**, through the collision match.

## Independent evidence

Commodore's own `custom.h` 39.1 (18.9.92, "Includes Release 40.15"), the struct the OS and every Amiga compiler used, a year after this text and from the shipping AGA era, gives `CLXCON2 $10E` and `BPLCON4 $10C`. The address list is right; the alphabetical entry has taken its neighbour's address.

| | where | what it does |
|---|---|---|
| **WinUAE** | [`custom.cpp:7412`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7412) | `case 0x10e: CLXCON2(value); break;`, with `case 0x10c: BPLCON4(value); break;` at [`:7364`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7364) |
| **vAmiga** | [`Memory.cpp:2894`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2894) | `case 0x10E >> 1: // CLXCON2 (AGA)`, immediately after `case 0x10C >> 1: // BPLCON4 (AGA)` in one address-decoded switch |
| **Denise** | [`debuggerSnapshot.h:456`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/debuggerSnapshot.h#L456-L457) | **implements neither register**: its write decode steps from `case 0x10a` to `case 0x110` ([`register.cpp:479`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L479-L483)), yet its register-name table reads `{0x210c, "BPLCON4"}, {0x210e, "CLXCON2"}`: the `$2000` tag the table gives every AGA-only register (`{0x21fc, "FMODE"}`, [`:551`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/debuggerSnapshot.h#L551)), masked off with `& 0xfff` ([`dmaDebugger.cpp:276`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/program/debugger/dmaDebugger.cpp#L276)) |
| **Copperline** | [`custom_regs.rs:1154`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L1154-L1157) | `0x10E => { if self.denise_is_lisa() { self.denise.clxcon2 = val & 0x0FFF; } ...`, with `0x10C =>` for BPLCON4 in the arm before it ([`:1138`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L1138)); its register-access validator gives `$10E` the defined bits `0x00C3` ([`regcheck.rs:331`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/regcheck.rs#L331)), the field the test drives |
| **MiSTer** | [`rtl/denise_collision.v:20`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L20) | descendant: `parameter CLXCON2 = 9'h10e;`, the identical line |
| **TC64** | [`rtl/minimig/denise_collision.v:20`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_collision.v#L20) | descendant: the identical line |

Denise's abstention is the informative one: a register it does not implement still appears in its debugger table at `$10E`, so the address reached it from somewhere other than this document's alphabetical section.

## Decision

Nothing changes in the RTL. A claim about CLXCON2's address cites both pages and says which it follows and why; the p. 8 list and `custom.h` agree on `$10E`, and the alphabetical entry's `$10C` is held by no implementation. This does not repair the document, it narrows what may be cited. Of the specification's 257 register names, 92 stated on more than one page, CLXCON2 is the only one given two addresses.
