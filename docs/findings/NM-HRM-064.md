# NM-HRM-064 — the board advertises that it can be shut up, and register 4C has no handler [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `src/minimig-aga/cpu_wrapper.v:629-665`, the AUTOCONFIG write path — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L629-L665) at the pinned revision |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** and NanoMig — the responder is **inherited**, and MiSTer's Zorro II memory card makes the same promise; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge. See *Where the responder comes from* below |
| **Test** | [`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address) · six graded assertions — **all red** |
| **Chapter** | Appendix K — Zorro Expansion Bus · *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed pp. 433 and 436 |
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

Bit 6 clear is "can be shut up". The Zorro III arm at `cpu_wrapper.v:598` sets it
— the RTL's own comment there reads *"Memory card, not silenceable"* — but the
Zorro II path falls through to the `:547` default of `4'b1111`, and the
complement rule clears the bit.

**And register 4C is decoded nowhere.** A grep for its address across the whole
module returns nothing:

```
$ grep -c "100110" cpu_wrapper.v      # 6'b100110 == register 0x4C
0
```

The write path at `:629-665` decodes `6'b100100` (register 48) and `6'b100010`
(register 44) and no third address. Writing 4C leaves the board responding.

### Measured

[`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address)
writes register `4C` and reads register 00 back: raw stays `0xE`, where a
bit-6-clear board should go silent (`0x0`) — six assertions red, of the
package's `5 passed, 8 failed`. See *What holds the finding up* below for the
six assertions and the inverse control in full.

## What the others say

| | 0x4C handled | |
|---|---|---|
| **WinUAE** | [`expansion.cpp:833`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/expansion.cpp#L833) and `:917` advance the AUTOCONFIG chain on a 4C write, and device backends call it explicitly — `a2091.cpp:2646` is `if (addr == 0x4c && !wd->configured) expamem_shutup(&wd->bank);` | **yes** |
| **vAmiga** | `Core/Components/Zorro/ZorroBoard.cpp:130-149` — `pokeACF8` switches on `0x48` and `0x4A` and has **no `0x4C` case**. `ZorroBoard::shutup()` exists at `:166-170` and nothing in `Core/` calls it | **no** |
| **Denise** | **keeps both halves of the contract.** `expansionPort/fastMem.cpp:43` answers register 08 with `encNibble(0)` under its own comment *"any space ok and can be shut up"* — bit 6 clear, the promise — and `expansionPort/expansionPort.cpp:27-30` honours it: `case 0x4c: if (canBeShutUp()) boardState = BoardState::ShutUp;`. (`canBeShutUp()` at `expansionPort.h:51` is a virtual returning `true` with no override anywhere in `emulation/`, so it is the shape of the gate rather than a per-board capability. Copperline has the real per-board form, a `no_shutup` flag on each board, in its row below.) | **yes** |
| **Copperline** | `src/zorro.rs:1054-1060` — the `EC_SHUTUP_PHYS` (`0x4C`) arm of `config_write` sets `BoardState::ShutUp` unless `board.spec.no_shutup`, which its Zorro II `fast_ram` board leaves `false` (`:212`). The CDTV DMAC, first in the chain when present, has its own: `src/cdtv.rs:406-407` — `0x4C => { self.shut_up = true; }`, and `:348` makes an un-configured, un-shut-up board the only one that answers | **yes** |
| **MiSTer** | descendant — **has the same responder in the same file.** `rtl/cpu_wrapper.v:518-520` decodes 4C (`6'b100110`) and ends the card, **but only in its CDTV arm**; the memory-card arm at `:522-527` has 0x48 and nothing else, exactly like NanoMig, and its register 08 reads bit 6 clear as NanoMig's does | **for CDTV, not for RAM** |
| **TC64** | descendant — `rtl/minimig/minimig_autoconfig.v:154` decodes `9'h04c`, commented *"Zorro II / III shut up register"*, for the Toccata sound card and the control board. **Not for a RAM board**, and `rtl/minimig/minimig.v:1250` leaves `.board_shutup()` unconnected | **for two cards, not for RAM** |

**Three of the four independent implementations handle the register, and vAmiga
does not.**

**Denise is the closest comparison, because it makes NanoMig's exact promise
and says so.** Its fast-RAM board answers register 08 with bit 6 clear — the
comment beside it reads *"any space ok and can be shut up"* — and it then
honours a write to 4C. It is not alone: WinUAE's Zorro II fast RAM answers
register 08 with `flags` (`expansion.cpp:1748`), which only its Zorro III
branch sets (`:1706`), so it reads `0x00` as NanoMig's does, and the byte write
to 4C at `:917` leaves the card unmapped; Copperline's `fast_ram` board clears
bit 6 as well (`zorro.rs:1091-1095`). **Same advertisement, opposite outcome.**
That is the pair the finding turns on: not that NanoMig lacks a 4C handler,
which every Minimig RAM board lacks, but that it promises one -- as the MiSTer
memory card it descends from does too.

**vAmiga's omission is the same omission.** Its boards are a fixed internal
set — `DiagBoard`, `HdController`, `RamExpansion` — and each answers register
08 with bit 6 clear: `flags()` is `0x80` (`RamExpansion.h:86`),
the other two `0x00` (`DiagBoard.h:94`, `HdController.h:171`). vAmiga makes
NanoMig's promise and, with no `0x4C` case, does not keep it.

## Where the responder comes from

`MiSTer-devel/Minimig-AGA_MiSTer` carries the same responder in the same file,
`rtl/cpu_wrapper.v`, which is where NanoMig's comes from; `minimig-dev/MinimigAGA-MiST-TC64` has its own in
`rtl/minimig/minimig_autoconfig.v`, driven by a `sel_autoconfig` decode at
`rtl/minimig/gary.v:174`.

**What no design in the family does is implement shut-up for a RAM board.**
MiSTer honours 4C for its CDTV card and not for memory; TC64 honours it for the
Toccata card and the control board and not for memory, and does not even wire
the result out. NanoMig does not honour it at all. On that, NanoMig is in
company.

**And the promise is inherited with it.** `chip_addr[6:1]` counts 16-bit words,
so register 08 is index `6'b000100` (register 04, the product number, is
`6'b000010`). Neither Zorro II case list maps it:

| | Zorro II case list, indices mapped | register 08 (`6'b000100`) | reads at the host as | bit 6 |
|---|---|---|---|---|
| **MiSTer** `cpu_wrapper.v:410-426` | 0, 1, 2, 3, 8, 9, 10, 11, 19 | absent, so the `:395` default `4'b1111` stands | `0000` | **clear — "can be shut up"** |
| **NanoMig** `cpu_wrapper.v:568-590` | 0, 1, 8, 9, 10, 11, 19 | absent, so the `:547` default `4'b1111` stands | `0000` | **clear — "can be shut up"** |

The inversion is not an assumption: MiSTer's own comment at `:459` states it —
*"nibbles are inverted, so `4'b1111` reads as 0"*. MiSTer's extra entries at
`:418-419` are register 04, which NanoMig leaves unset.

So upstream makes the same omission and the same promise. **The finding is the
family's, and the fix is one line either case list is missing.**

## Suggested change

**Add `6'b000100: autocfg_data = 4'b1011;` to the Zorro II arm** — register
08's upper nybble, complemented as every register but 00 is: logical `0100`,
bit 6 set, the other bits as they read now. That stops the board announcing a
capability the whole Minimig family lacks, and it costs one case entry;
MiSTer's memory card needs the same line.

The other direction — a handler for `6'b100110` that clears `autocfg_card` and
stops the board answering — makes the design complete instead of merely honest,
and would be the first RAM-board shut-up in the family. Whether that is wanted
is a question for Amiga specialists, and so is whether any Kickstart writes 4C
to a RAM board at all.

## What holds the finding up

`hrm/appK-autoconfig-base-address`, six assertions, all red. The fixture drives a
real 68000 program through `cpu_wrapper`'s own bus — no internal signal is
touched — writes 4C, and reads the board back:

    board still answers after a write to 4C, raw nibble stays 0xE

An inverse control in the same package writes register **48** instead, and the
board does go silent. So the fixture can observe silencing; there is nothing to
observe after 4C.

The package builds `cpu_wrapper` with TG68K. The same test source built with
fx68k, [`hrm/appK-autoconfig-base-address-fx68k`](../../hrm/appK-autoconfig-base-address-fx68k),
gives the same six reds and the same control, so the finding does not rest on
the CPU core or on the Verilator-only TG68K instance.

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

Silent on this point: Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

