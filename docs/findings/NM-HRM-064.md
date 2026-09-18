# NM-HRM-064 — the board advertises that it can be shut up, and register 4C has no handler

| | |
|---|---|
| **Status** | open · against the RTL |
| **Module** | `src/minimig-aga/cpu_wrapper.v:619-646` — the AUTOCONFIG write path |
| **Report to** | **NanoMig.** The AUTOCONFIG responder is NanoMig's own: neither Minimig line has one. See *Upstream* in the table below |
| **Test** | `hrm/appK-autoconfig-base-address` · six graded assertions — **all red** |
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

## The measurement

`hrm/appK-autoconfig-base-address`, six assertions, all red. The fixture drives a
real 68000 program through `cpu_wrapper`'s own bus — no internal signal is
touched — writes 4C, and reads the board back:

    board still answers after a write to 4C, raw nibble stays 0xE

An inverse control in the same package writes register **48** instead, and the
board does go silent. So the fixture can observe silencing; there is nothing to
observe after 4C.

## What the others do

| | 0x4C handled | |
|---|---|---|
| **WinUAE** | [`expansion.cpp:833`](https://github.com/tonioni/WinUAE) and `:917` advance the AUTOCONFIG chain on a 4C write, and device backends call it explicitly — `a2091.cpp:2646` is `if (addr == 0x4c && !wd->configured) expamem_shutup(&wd->bank);` | **yes** |
| **vAmiga** | `Core/Components/Zorro/ZorroBoard.cpp:130-149` — `pokeACF8` switches on `0x48` and `0x4A` and has **no `0x4C` case**. `ZorroBoard::shutup()` exists at `:166-170` and nothing in `Core/` calls it | **no** |
| **Denise** | `emulation/libami/expansionPort/expansionPort.cpp:27-30` — `case 0x4c: if (canBeShutUp()) boardState = BoardState::ShutUp;`, gated on the very capability bit the page defines (`expansionPort.h:51`) | **yes** |
| **Copperline** | `src/cdtv.rs:406-407` — `0x4C => { self.shut_up = true; }`, and `:348` makes an un-configured, un-shut-up board the only one that answers | **yes** |
| **MiSTer** | descendant — **no AUTOCONFIG responder at all.** `rtl/` has no `sel_autoconfig`, no `autocfg_data` and no `$E8` decode; the responder is NanoMig's own addition | **n/a** |
| **TC64** | descendant — **the same**: no AUTOCONFIG responder in `rtl/minimig/` | **n/a** |

**Three of the four independent implementations handle the register, and vAmiga
does not.** Denise is the closest comparison and the most pointed one: it
implements exactly the contract the two pages state, testing `canBeShutUp()`
before honouring the write.

**vAmiga's omission is not the same omission.** Its boards are a fixed internal
set — `DiagBoard`, `HdController`, `RamExpansion` — that the emulator itself
plugs and unplugs, and a board it controls has no need to be silenced by guest
software. NanoMig's responder answers a real guest.

## Why this is NanoMig's and not upstream's

Neither Minimig line has an AUTOCONFIG responder to diverge from. The
`sel_autoconfig` decode, the `autocfg_data` register file and the write path are
absent from both `MiSTer-devel/Minimig-AGA_MiSTer`'s `rtl/` and
`minimig-dev/MinimigAGA-MiST-TC64`'s `rtl/minimig/`. This is NanoMig's own code
and a fix has nowhere upstream to go.

## What would settle it

Either a handler for `6'b100110` that clears `autocfg_card` and stops the board
answering, or setting bit 6 in the Zorro II arm so the board stops promising a
capability it does not have. **The second is one constant and makes the design
honest; the first makes it complete.** Which is right is a question for Amiga
specialists and for whoever knows whether any Kickstart actually writes 4C to a
RAM board.

## What this finding does not claim

That anything breaks. AmigaOS shuts a board up when it has a reason to — a
driver refusing its own card, a memory board it declines to add — and nothing
here measures how often that happens to a fast-RAM board on a real system. What
is measured is that the design states a capability in its configuration and does
not implement it.
