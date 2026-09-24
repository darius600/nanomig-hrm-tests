# NM-HRM-064 — the board advertises that it can be shut up, and register 4C has no handler

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `src/minimig-aga/cpu_wrapper.v:619-646`, the AUTOCONFIG write path — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/cpu_wrapper.v#L619-L646) at the pinned revision |
| **Report to** | **NanoMig**, and the fix is one line MiSTer already has. The AUTOCONFIG responder is **inherited**, not NanoMig's own — see *Why this is NanoMig's* below, which was wrong in this page's first version |
| **Test** | [`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address) · six graded assertions — **all red** |
| **Chapter** | Appendix K — Zorro Expansion Bus · printed pp. 433 and 436 |
| **Assertions** | 6 red, with an inverse control that turns green when register 48 is written in place of 4C |
| **Verdict of the four** | **three of four implement it; vAmiga does not** |

---

## What the HRM says

**Printed page 433**, defining register 08 bit 6:

> 6 This bit is set to indicate that the board can't be shut up by software,
> cleared to indicate that the board can be shut up.

**Printed page 436**, defining register 4C:

> Shut up register, write only. Anything written to 4C will cause a board that
> supports shut-up to completely disappear until the next reset.

The two sentences make one contract: **a board that clears bit 6 promises to
disappear when 4C is written.**

## What NanoMig does

**It clears bit 6 in its Zorro II configuration**, so it makes the promise.
Measured, not inferred — `hrm/appK-autoconfig-register-map` grades it and the run
prints the nibble:

```
register08 (chip_addr index 4): Zorro-III logical nibble=0xF (raw=0x0, complemented),
                                Zorro-II  logical nibble=0x0
PASS: sol:K-0785 register08 bit6 clear -> can be shut up by software (Zorro II default)
```

Bit 6 clear is "can be shut up". The Zorro III arm at `cpu_wrapper.v:588` sets it
— the RTL's own comment there reads *"Memory card, not silenceable"* — but the
Zorro II path falls through to the `:541` default of `4'b1111`, and the
complement rule clears the bit.

**And register 4C is decoded nowhere.** A grep for its address across the whole
module returns nothing:

```
$ grep -c "100110" cpu_wrapper.v      # 6'b100110 == register 0x4C
0
```

The write path at `:619-646` decodes `6'b100100` (register 48) and `6'b100010`
(register 44) and no third address. Writing 4C leaves the board responding.

### Measured

[`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address)
writes register `4C` and reads register 00 back: raw stays `0xE`, where a
bit-6-clear board should go silent (`0x0`) — six assertions red, of the
package's `5 passed, 8 failed`. See *What holds the finding up* below for the
six assertions and the inverse control in full.

## What holds the finding up

`hrm/appK-autoconfig-base-address`, six assertions, all red. The fixture drives a
real 68000 program through `cpu_wrapper`'s own bus — no internal signal is
touched — writes 4C, and reads the board back:

    board still answers after a write to 4C, raw nibble stays 0xE

An inverse control in the same package writes register **48** instead, and the
board does go silent. So the fixture can observe silencing; there is nothing to
observe after 4C.

## What the others say

| | 0x4C handled | |
|---|---|---|
| **WinUAE** | [`expansion.cpp:833`](https://github.com/tonioni/WinUAE) and `:917` advance the AUTOCONFIG chain on a 4C write, and device backends call it explicitly — `a2091.cpp:2646` is `if (addr == 0x4c && !wd->configured) expamem_shutup(&wd->bank);` | **yes** |
| **vAmiga** | `Core/Components/Zorro/ZorroBoard.cpp:130-149` — `pokeACF8` switches on `0x48` and `0x4A` and has **no `0x4C` case**. `ZorroBoard::shutup()` exists at `:166-170` and nothing in `Core/` calls it | **no** |
| **Denise** | **keeps both halves of the contract.** `expansionPort/fastMem.cpp:43` answers register 08 with `encNibble(0)` under its own comment *"any space ok and can be shut up"* — bit 6 clear, the promise — and `expansionPort/expansionPort.cpp:27-30` honours it: `case 0x4c: if (canBeShutUp()) boardState = BoardState::ShutUp;`. (`canBeShutUp()` at `expansionPort.h:51` is a virtual returning `true` with no override anywhere in `emulation/`, so it is the shape of the gate rather than a per-board capability. Copperline has the real per-board form: `zorro.rs` `if board.spec.no_shutup { return; }`) | **yes** |
| **Copperline** | `src/cdtv.rs:406-407` — `0x4C => { self.shut_up = true; }`, and `:348` makes an un-configured, un-shut-up board the only one that answers | **yes** |
| **MiSTer** | descendant — **has the same responder in the same file.** `rtl/cpu_wrapper.v:518-520` decodes 4C (`6'b100110`) and ends the card, **but only in its CDTV arm**; the memory-card arm at `:522-527` has 0x48 and nothing else, exactly like NanoMig | **for CDTV, not for RAM** |
| **TC64** | descendant — `rtl/minimig/minimig_autoconfig.v:154` decodes `9'h04c`, commented *"Zorro II / III shut up register"*, for the Toccata sound card and the control board. **Not for a RAM board**, and `rtl/minimig/minimig.v:1249` leaves `.board_shutup()` unconnected | **for two cards, not for RAM** |

**Three of the four independent implementations handle the register, and vAmiga
does not.**

**Denise is the closest comparison, because it is the one that makes NanoMig's
exact promise.** Its fast-RAM board answers register 08 with bit 6 clear — the
comment beside it reads *"any space ok and can be shut up"* — and it then
honours a write to 4C. **Same advertisement, opposite outcome.** That is the
pair the finding turns on: not that NanoMig lacks a 4C handler, which the whole
Minimig family lacks, but that it is the only one of the five that says it has
one.

**vAmiga's omission is not the same omission.** Its boards are a fixed internal
set — `DiagBoard`, `HdController`, `RamExpansion` — that the emulator itself
plugs and unplugs, and a board it controls has no need to be silenced by guest
software. NanoMig's responder answers a real guest.

## Why this is NanoMig's — corrected

**This page's first version said neither Minimig line has an AUTOCONFIG
responder to diverge from. That is false, and the correction makes the finding
sharper rather than weaker.** `MiSTer-devel/Minimig-AGA_MiSTer` carries the same
responder in the same file, `rtl/cpu_wrapper.v`, which is where NanoMig's comes
from; `minimig-dev/MinimigAGA-MiST-TC64` has its own in
`rtl/minimig/minimig_autoconfig.v`, driven by a `sel_autoconfig` decode at
`rtl/minimig/gary.v:174`.

**What no design in the family does is implement shut-up for a RAM board.**
MiSTer honours 4C for its CDTV card and not for memory; TC64 honours it for the
Toccata card and the control board and not for memory, and does not even wire
the result out. NanoMig does not honour it at all. On that, NanoMig is in
company.

**Where NanoMig stands alone is in advertising it.** Register 08 is index
`6'b000010`, and the two Zorro II case lists differ by exactly that entry:

| | Zorro II register 08 | reads at the host as | bit 6 |
|---|---|---|---|
| **MiSTer** `cpu_wrapper.v:418` | `6'b000010: autocfg_data = 4'b1010;` | `0101` | **set — "cannot be shut up"** |
| **NanoMig** `cpu_wrapper.v:562-580` | **the index is absent**, so the `:541` default `4'b1111` stands | `0000` | **clear — "can be shut up"** |

The inversion is not an assumption: MiSTer's own comment at `:460` states it —
*"nibbles are inverted, so `4'b1111` reads as 0"*.

So upstream makes the same omission and does not make the same promise. **A fix
has somewhere to go after all, and it is one line NanoMig's case list is
missing.**

## Suggested change

**Add `6'b000010: autocfg_data = 4'b1010;` to the Zorro II arm** — MiSTer's own
line, at MiSTer's own value. That stops the board announcing a capability the
whole Minimig family lacks, and it costs one case entry.

The other direction — a handler for `6'b100110` that clears `autocfg_card` and
stops the board answering — makes the design complete instead of merely honest,
and would be the first RAM-board shut-up in the family. Whether that is wanted
is a question for Amiga specialists, and so is whether any Kickstart writes 4C
to a RAM board at all.

## For the specialists

That anything breaks. AmigaOS shuts a board up when it has a reason to — a
driver refusing its own card, a memory board it declines to add — and nothing
here measures how often that happens to a fast-RAM board on a real system. What
is measured is that the design states a capability in its configuration and does
not implement it.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 31 — agrees with the manual:

  > "(4C/4E) X X X X X X X X Optional “shut up’ address, a write to this ad- dress will cause the board to pass its config out and then never again respond to any address."
  <!-- corpus: a500-a2000-trm-1987/p031.txt -->

  The expansion chapter's register table makes 4C a shut-up address: a write there makes the board pass config out and stop answering until RESET. Register 08/0A carries the matching flag, 'O means this board can be shut up' (p030). The example test-RAM PAL (p046) latches SHUTUP on a write decoded at 4C and gates its outputs with it. That is the manual's contract; NanoMig advertises the flag and has no 4C handler. The table calls the address 'Optional', and the 08/0A flag is how a board declares whether it has one.

Silent on this point: Commodore, *A500/A2000 Gary Specification*; US patent 4,829,473, Commodore-Amiga; 6502.org dissection of Paula 8364R7.

