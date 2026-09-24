# NM-AA-001 — the AA specification gives CLXCON2 two different addresses, and one of them belongs to BPLCON4

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a graded green with an inverse control |
| **What is asked of a reader** | Nothing in the RTL. NanoMig already follows the address the shipping header confirms. What is asked is whether this document is fit to be cited as an oracle for CLXCON2 at all, and if so which of its two statements a claim may rest on. |
| **Module** | `src/minimig-aga/denise_collision.v:20` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_collision.v#L20) at the pinned revision |
| **Report to** | **nowhere in code.** The defect is in a 1991 Commodore document, and the company is gone. It is filed so that no AA claim cites the wrong half of it |
| **Test** | [`aa/aa-clxcon2-address`](../../aa/aa-clxcon2-address) · five graded assertions — all green. Run it with `hrm/run_all.sh --in ../aa` |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF pp. 4, 8, 16, 18 |
| **Assertions** | 5 green, each with its own mutation; `make mutate` reports 0 passed, 5 failed |
| **Verdict of the four** | **unanimous for $10E, and it leaves the document's other half with nobody behind it.** Three of the four decode CLXCON2, and all three decode it at `$10E` with BPLCON4 at `$10C`. The fourth implements neither register and still names both at those addresses in its own register table. Not one of them puts CLXCON2 at `$10C`. See [What the others say](#what-the-others-say) |

## What the AA specification says

It says two things. The address-ordered register list, PDF p. 8:

```
CLXCON2    p 10E W   D Extended collision control register
```

The alphabetical section, PDF p. 18:

```
CLXCON2 P  10C  W    D    Extended Collision Control
```

`$10C` is not a free address. The same document gives it to BPLCON4, twice —
in the list on p. 8 and in the BPLCON bit table on p. 16:

```
BPLCON4    p 10C W   D Bit plane control reg (bitplane and sprite masks)
BPLCON4 P 10C  W     D       Bit Plane control reg.(display masks)
```

So one of the two CLXCON2 statements puts it on top of another register.

## What settles it

Commodore's own `custom.h` 39.1 (18.9.92, "Includes Release 40.15") — the
struct the OS and every Amiga compiler used, from the *shipping* AGA era, a
year after this PRELIMINARY text. Addresses there are struct offsets rather
than written constants, so they are derived rather than quoted, and the
derivation agrees with the page on eleven of eleven registers spot-checked,
`FMODE $1FC` included:

```
CLXCON2   $10E
BPLCON4   $10C
```

The address list is right. The alphabetical entry has taken its neighbour's
address.

## What NanoMig does

```verilog
parameter CLXCON2 = 9'h10e;
```

and the write is gated on AGA:

```verilog
else if (aga && (reg_address_in[8:1] == CLXCON2[8:1]))
  clxcon2 <= #1 data_in;
```

Measured at the `denise_collision` boundary, three assertions, all green: a
write to `$10E` changes the bitplane match; a write to `$10C` does not; and a
write to `CLXCON` clears CLXCON2, which is what p. 4 states in words —
*"CLXCON2 is reset by a write to CLXCON, so that old game programs will be able
to correctly detect collisions."*

CLXCON2 has no read path at this boundary, so it is reached through the one
effect that makes it observable: `clxcon2[7:6]` enable bitplanes 7 and 8 in the
collision match and `clxcon2[1:0]` carry their match values. With the register
zero, bit 7 of `bpldata` cannot affect a collision; with `$00C3` written, it
can. That difference is the measurement.

### Measured

[`aa/aa-clxcon2-address`](../../aa/aa-clxcon2-address) drives `bpldata=0x7F` at
the `denise_collision` boundary and reads whether it collides. Unwritten, it
collides; after `$00C3` is written to `$10E`, it does not — `CLXCON2-AT-10E`,
green. A write to `$10C` does not change the match — `CLXCON2-NOT-AT-10C`,
green. Armed, it does not collide; after a `CLXCON` write it collides again —
`CLXCON2-CLEARED-BY-CLXCON`, green. All five of the package's assertions are
green: `aa-clxcon2-address: 5 passed, 0 failed`.

## What the others say

The address is not a matter of opinion for an implementation: each of these
decodes a write to `$10E` or it does not, and each has to put BPLCON4
somewhere.

| | where | what it does |
|---|---|---|
| **WinUAE** | [`custom.cpp:7412`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7412) | `case 0x10e: CLXCON2(value); break;` — with `case 0x10c: BPLCON4(value); break;` at [`:7364`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7364). Its state-file register table carries the same pair in address order, `/* 10C BPLCON4 */` then `/* 10E CLXCON2* */` ([`:7778`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7778-L7779)) |
| **vAmiga** | [`Memory.cpp:2894`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2894) | `case 0x10E >> 1: // CLXCON2 (AGA)`, in the slot immediately after `case 0x10C >> 1: // BPLCON4 (AGA)` — two adjacent arms of one address-decoded switch, so the pair cannot collide |
| **Denise** | [`debuggerSnapshot.h:456`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/debuggerSnapshot.h#L456-L457) | **implements neither register** — its write decode steps straight from `case 0x10a` to `case 0x110` ([`register.cpp:479`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L479-L483)) — and its own register-name table still reads `{0x210c, "BPLCON4"}, {0x210e, "CLXCON2"}`, the chip base `$200` plus the two addresses |
| **Copperline** | [`custom_regs.rs:248`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L247-L248) | `0x10E if self.denise_is_lisa() => Some(self.denise.clxcon2)` for the read, `0x10C` for BPLCON4 in the line above it, and `0x10E => self.clxcon2 = value & 0x0FFF` for the write ([`bus.rs:11710`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L11710)). Its register-mask table gives `$10E` the writable bits `0x00C3` ([`regcheck.rs:331`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/regcheck.rs#L331)) — the two enables and the two match values, which is the field this finding's test drives |
| **MiSTer** | [`rtl/denise_collision.v:20`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_collision.v#L20) | descendant — `parameter CLXCON2 = 9'h10e;`, the identical line |
| **TC64** | [`rtl/minimig/denise_collision.v:20`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_collision.v#L20) | descendant — the identical line |

**This does not repair the document and it does narrow what a reader may cite.**
The p. 8 address list and Commodore's own `custom.h` already agreed on `$10E`;
four independent implementations now agree with both, and the alphabetical
entry's `$10C` is held by no implementation at all — every one of them has
BPLCON4 there instead, which is the collision the page creates.

**Denise's abstention is the informative one.** A register it does not implement
still appears in its debugger table at `$10E`, which means the address reached
it from somewhere other than this document's alphabetical section.

## Suggested change

None to the RTL.

For the corpus: a claim about CLXCON2's address cites **both** pages and says
which it follows and why. The reading brief already requires a conflict to be
recorded rather than resolved; this is the worked example of one where an
outside source settles it.

For the pipeline: `tools/aa/cross_page_addresses.py` now reads every register
row in the corpus, groups by name, and reports any register stated at more than
one address, saying which one the shipped header agrees with. Over 349 register
rows and 257 names, 92 of them stated on more than one page, **CLXCON2 is the
only one.**

It read 342 rows until 21.09.2026. Five register rows carry TWO marks --
`SPRHDAT   &h 078`, and the same for SPRHPTH, SPRHPTL, BPLHPTH and BPLHPTL, one
mark from the usage legend and one from the revision legend. The pattern allowed
exactly one, so those lines matched nothing at all and the rows were never read.
All five turn out to disagree with their own alphabetical entries (`+h` or `&h`
against a bare `H`), which a reader had reported from the page while the tool
could not see it. The address conflict is unaffected: CLXCON2 is still the only
one, over more rows than before.

## What holds this up

The package grew two assertions on 21.09.2026 that are not about CLXCON2 at
all: CLXDAT is decoded at $00E and silent at $010, and CLXCON is decoded at
$098 and not at $09A. They are here rather than in a package of their own
because this fixture already drove both addresses without grading either, and a
second fixture at the same boundary would measure the same thing twice.

The assertions and their mutations — one each, so that `make mutate`
turns every one of them red rather than only the first; the earlier version
guarded the address assertion alone and reported "2 passed, 1 failed", which
means two of the three had never been shown capable of failing — the two printed pages quoted verbatim
from the corpus, and `custom.h` 39.1 as an independent later source. What it
does not rest on is any reading of a single page: every line quoted here is
`[3/3]` in the corpus — all three readings agree the pages say these things.

## What is left for specialists

Whether a PRELIMINARY specification that misplaces a register onto another
register's address should be used as a conformance oracle at all, and with what
caveat. That is a judgement about the document, not a measurement, and it
belongs to someone who knows what else this text got wrong before silicon
shipped.
