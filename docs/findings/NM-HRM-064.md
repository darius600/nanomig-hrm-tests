# NM-HRM-064 — the board advertises that it can be shut up, and register 4C has no handler [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Question: must a Zorro II board whose register 08 bit 6 reads clear (p.433, "can be shut up") go silent after a write to register 4C (p.436), so that NanoMig has to set that bit or implement 4C, when today a 4C write leaves register 00 reading raw `0xE`? Closes when the change under *Decision* is made, with the six assertions `sol:K-0740`, `sol:K-0849`, `sol:K-0850`, `opus:K-0889`, `sol:K-0848` and `opus:K-0888` and `GATE-G-PRECOND`, which today require bit 6 clear, regraded for a board that declares it cannot be shut up; or when a 4C handler is made and those six reds and `GATE-G` turn green; or when an Amiga specialist rejects both, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | Zorro II memory board in NanoMig's AUTOCONFIG path (register 08 bit 6, write to register 4C), on TG68K and fx68k; the Zorro III arm sets bit 6 and is not in scope; chip revision unknown, hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix K — Zorro Expansion Bus · *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed pp. 431 (the shut-up address), 433 (register 08 bit 6) and 436 (register 4C) · claim ids by assertion: `sol:K-0740` (p. 431, any write to the shut-up address shuts up a supporting board); `sol:K-0849`, `sol:K-0850`, `opus:K-0889` (p. 436, a write to 4C shuts a supporting board up); `sol:K-0848`, `opus:K-0888` (4C is the write-only shut-up register); `sol:K-0785` (p.433, bit 6 clear, green in `hrm/appK-autoconfig-register-map`). `GATE-G`, `GATE-G-PRECOND` and `P3-IMG2` carry no claim id, only the page tags `[p433,p436]`, `[p433]` and `[p435,p431]` |
| **Module** | `src/minimig-aga/cpu_wrapper.v:629-665`, the AUTOCONFIG write path — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L629-L665) · [MiSTer](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cpu_wrapper.v#L518-L527) memory-card arm; TC64 differs (its own responder in `minimig_autoconfig.v`, shut-up output unconnected); inherited |
| **Test** | [`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address): `make -C hrm/appK-autoconfig-base-address test`; the same source on fx68k, [`hrm/appK-autoconfig-base-address-fx68k`](../../hrm/appK-autoconfig-base-address-fx68k): `make -C hrm/appK-autoconfig-base-address-fx68k test`; [`integration/appK-autoconfig`](../../integration/appK-autoconfig): `make -k -C integration/appK-autoconfig test`; an expected red exits nonzero. Bit 6 clear is graded green by [`hrm/appK-autoconfig-register-map`](../../hrm/appK-autoconfig-register-map): `make -C hrm/appK-autoconfig-register-map test` |
| **Assertions** | 6 red, expected register 00 raw `0x0` after a write to 4C, observed `0xE`: `sol:K-0740`, `sol:K-0849`, `sol:K-0850`, `opus:K-0889`, `sol:K-0848`, `opus:K-0888`, the same six in the TG68K and fx68k packages (TG68K: `5 passed, 8 failed`, the other two reds [NM-HRM-065](NM-HRM-065.md)'s); the inverse control, which writes register 48 in place of 4C, is green. At the system boundary `GATE-G` is red on fx68k and on TG68K, expected a differing register image or no answer after the write to 4C, observed an identical image (25 passed, 3 failed on each core, the other two reds [NM-HRM-085](NM-HRM-085.md)'s); `GATE-G-PRECOND` and `P3-IMG2` are green on both cores |
| **Verdict of the four** | **three of four implement it; vAmiga does not**. Hardware: not measured |
| **Effort** | S — the one-line option: one case entry in the Zorro II register list of `cpu_wrapper.v:568-590`; the 4C handler is not sized |

---

## Source requirement

**Printed page 433**, defining register 08 bit 6:

> 6 This bit is set to indicate that the board can't be shut up by software,
> cleared to indicate that the board can be shut up.

**Printed page 436**, defining register 4C:

> Shut up register, write only. Anything written to 4C will cause a board that
> supports shut-up to completely disappear until the next reset.

The two make one contract: a board that clears bit 6 promises to disappear when 4C is written.

## RTL and measurement

NanoMig clears bit 6 in its Zorro II configuration. The register-map package grades it and the run prints the nibble:

```
register08 (chip_addr index 4): Zorro-III logical nibble=0xF (raw=0x0, complemented),
                                Zorro-II  logical nibble=0x0
PASS: sol:K-0785 register08 bit6 clear -> can be shut up by software (Zorro II default)
```

The Zorro III arm at `cpu_wrapper.v:598` sets the bit (the RTL's own comment reads *"Memory card, not silenceable"*); the Zorro II path falls through to the `:547` default of `4'b1111`, and the complement rule clears it. Register 4C is decoded nowhere:

```
$ grep -c "100110" cpu_wrapper.v      # 6'b100110 == register 0x4C
0
```

The write path at `:629-665` decodes `6'b100100` (register 48) and `6'b100010` (register 44) and no third address.

The promise is inherited. `chip_addr[6:1]` counts 16-bit words, so register 08 is index `6'b000100`, and neither Zorro II case list maps it:

| | indices mapped | register 08 | reads as | bit 6 |
|---|---|---|---|---|
| **MiSTer** `cpu_wrapper.v:410-426` | 0, 1, 2, 3, 8, 9, 10, 11, 19 | absent, `:395` default `4'b1111` | `0000` | clear, "can be shut up" |
| **NanoMig** `cpu_wrapper.v:568-590` | 0, 1, 8, 9, 10, 11, 19 | absent, `:547` default `4'b1111` | `0000` | clear, "can be shut up" |

MiSTer's comment at `:459` states the inversion: *"nibbles are inverted, so `4'b1111` reads as 0"*. Its extra entries at `:418-419` are register 04, which NanoMig leaves unset.

[`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address) runs a 68000 program through the module's bus, writes 4C and reads register 00 back:

    MEASURED: Group Shutup: reg00 raw before=0xE after writing register 4C=0xE

A bit-6-clear board should go silent (`0x0`). Six assertions are red, of the package's `5 passed, 8 failed`. The inverse control writes register **48** and the board does go silent, so the fixture can observe silencing. The same source built with fx68k, [`hrm/appK-autoconfig-base-address-fx68k`](../../hrm/appK-autoconfig-base-address-fx68k), gives the same six reds and the same control.

[`integration/appK-autoconfig`](../../integration/appK-autoconfig) measures it at the system boundary: the whole design at `nanomig.v`'s ports, once per CPU core, `fastram_config` = 1, one Zorro II memory board. Run R_G (`prog_rg.s`) reads the board's register image, the 32 words from $E80000; run R_G' (`prog_rg_shutup.s`) writes $00 to $E8004C and reads the same 32 words. `GATE-G-PRECOND [p433]` (`test_appK_autoconfig.cpp:781-782`) takes register 08 from R_G: physical nybbles $F and $F, logical $00, so bit 6 is 0 and the line is green on both cores. `GATE-G [p433,p436]` (`:789-791`) passes on a differing image or no answer; R_G' finishes and its image is R_G's, all 32 register nybbles identical, register 00 reading physical $E6 in both, so it is red on fx68k and on TG68K. The control on the same comparator is `P3-IMG2 [p435,p431]` (`:840-842`): after the byte write to register 48, run R3 finishes and reads an image that differs from the one before, all 32 nybbles 0, green on both cores. The comparator can see a board leave $E8xxxx; after the write to 4C the board does not.

## Risks and unknowns

- Nothing measures how often AmigaOS shuts up a fast-RAM board on a real system, or whether any Kickstart writes 4C to one; what is measured is that the design states a capability and does not implement it.
- The TRM calls the 4C address "Optional"; the register 08/0A flag is how a board declares whether it has one, so whether a RAM board is exempt is for Amiga specialists.
- The integration nybbles were read from the package's own programs, run once more on each core with the same harness and settings and printed rather than graded; they are the same on both cores.
- Real-hardware behaviour is not measured, and the chip revision is unknown.
- The witnesses split: vAmiga makes the promise and does not keep it.

## Independent evidence

| | 0x4C handled | |
|---|---|---|
| **WinUAE** | [`expansion.cpp:833`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/expansion.cpp#L833) and `:917` advance the AUTOCONFIG chain on a 4C write, and device backends call it explicitly — `a2091.cpp:2646` is `if (addr == 0x4c && !wd->configured) expamem_shutup(&wd->bank);` | **yes** |
| **vAmiga** | `Core/Components/Zorro/ZorroBoard.cpp:130-149` — `pokeACF8` switches on `0x48` and `0x4A` and has **no `0x4C` case**. `ZorroBoard::shutup()` exists at `:166-170` and nothing in `Core/` calls it | **no** |
| **Denise** | **keeps both halves of the contract.** `expansionPort/fastMem.cpp:43` answers register 08 with `encNibble(0)` under its own comment *"any space ok and can be shut up"*, and `expansionPort/expansionPort.cpp:27-30` honours it: `case 0x4c: if (canBeShutUp()) boardState = BoardState::ShutUp;`. (`canBeShutUp()` at `expansionPort.h:51` is a virtual returning `true` with no override anywhere in `emulation/`, so it is the shape of the gate rather than a per-board capability.) | **yes** |
| **Copperline** | `src/zorro.rs:1054-1060` — the `EC_SHUTUP_PHYS` (`0x4C`) arm of `config_write` sets `BoardState::ShutUp` unless `board.spec.no_shutup`, which its Zorro II `fast_ram` board leaves `false` (`:212`). The CDTV DMAC has its own: `src/cdtv.rs:406-407` — `0x4C => { self.shut_up = true; }` | **yes** |
| **MiSTer** | descendant — `rtl/cpu_wrapper.v:518-520` decodes 4C (`6'b100110`) and ends the card, **but only in its CDTV arm**; the memory-card arm at `:522-527` has 0x48 and nothing else, like NanoMig, and its register 08 reads bit 6 clear as NanoMig's does | **for CDTV, not for RAM** |
| **TC64** | descendant — `rtl/minimig/minimig_autoconfig.v:154` decodes `9'h04c`, commented *"Zorro II / III shut up register"*, for the Toccata sound card and the control board, **not for a RAM board**; `rtl/minimig/minimig.v:1250` leaves `.board_shutup()` unconnected | **for two cards, not for RAM** |

Denise is the closest comparison: it makes NanoMig's exact promise and honours a write to 4C. WinUAE's Zorro II fast RAM answers register 08 with `flags` (`expansion.cpp:1748`), which only its Zorro III branch sets (`:1706`), so it reads `0x00` as NanoMig does, and the byte write to 4C at `:917` leaves the card unmapped; Copperline's `fast_ram` board clears bit 6 as well (`zorro.rs:1091-1095`). Same advertisement, opposite outcome. vAmiga's omission is the same omission: its boards are a fixed internal set (`DiagBoard`, `HdController`, `RamExpansion`) and each answers register 08 with bit 6 clear, `flags()` being `0x80` (`RamExpansion.h:86`) and `0x00` for the other two (`DiagBoard.h:94`, `HdController.h:171`). No Minimig line implements shut-up for a RAM board.

### What other Commodore documents say

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 31 — agrees with the manual:

  > "(4C/4E) X X X X X X X X Optional “shut up’ address, a write to this ad- dress will cause the board to pass its config out and then never again respond to any address."
  <!-- corpus: a500-a2000-trm-1987/p031.txt -->

  The expansion chapter's register table makes 4C a shut-up address: a write there makes the board pass config out and stop answering until RESET. Register 08/0A carries the matching flag, 'O means this board can be shut up' (p030). The example test-RAM PAL (p046) latches SHUTUP on a write decoded at 4C and gates its outputs with it.

## Decision

**Suggested change: add `6'b000100: autocfg_data = 4'b1011;` to the Zorro II arm**: register 08's upper nybble, complemented as every register but 00 is, logical `0100`, bit 6 set, the other bits as they read now. That stops the board announcing a capability the Minimig family lacks, at one case entry; MiSTer's memory card needs the same line.

The other direction, a handler for `6'b100110` that clears `autocfg_card` and stops the board answering, would be the first RAM-board shut-up in the family.
