# NM-HRM-065 — register 48 does not configure a Zorro III board, and the implementations side with NanoMig

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect. Question: in the Zorro II configuration block (`$E8xxxx`), does a write to register 48 configure a Zorro III board, as printed p. 436 says, or does register 44 configure it, so that a board that keeps answering at register 00 (raw 0xA) after a write to 48 is correct? Closes when an Amiga specialist records that register 44 configures a Zorro III board in the Zorro II block, or confirms that register 48 does, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Appendix K of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | `cpu_wrapper`'s AUTOCONFIG write path with `cfg_z3` set, in the Zorro II configuration block `$E8xxxx`; the package builds with `ENABLE_RAM32` and drives `cpucfg` and `fastramcfg` at the module's ports; chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix K — Zorro Expansion Bus, printed pp. 435 and 436 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims `sol:K-0844` and `opus:K-0881` (p. 436, register 48 configures in the Zorro II block); both graded assertions carry a claim id, and the package's other six reds are register 4C and carry none of this page's |
| **Module** | `src/minimig-aga/cpu_wrapper.v:639-664`, the AUTOCONFIG write path — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L639-L664) at the pinned revision; inherited (MiSTer has the same code, and TC64's own module behaves the same) |
| **Test** | [`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address): `make -C hrm/appK-autoconfig-base-address test`, and its fx68k build `make -C hrm/appK-autoconfig-base-address-fx68k test`; an expected red exits nonzero |
| **Assertions** | 2 red, the same two in both builds: `sol:K-0844` and `opus:K-0881`, expected register 00 to read 0x0 (configured) after a write to register 48 with `cfg_z3` set, observed raw 0xA; the package reports `5 passed, 8 failed`, and an inverse control turns green for a Zorro II board on the identical write |
| **Verdict of the four** | **two of four do exactly what NanoMig does; the other two have no Zorro III board to disagree with**. Hardware: not measured |
| **Effort** | none — no RTL change; a documentation note on p. 436 only |

---

## Source requirement

**Printed page 435** names the two registers:

> 44 46 144 7-0 High order base address register, write only.
> 48 4A 148 7-0 Low order base address register, write only.
>
> The high order register takes bits 31-24 of the board's configured address,
> the low order register takes bits 23-16. For Zorro III boards configured in
> the Zorro II space, the configuration address is written both nybble and
> byte wide, with the ordering:
>
> | Reg | Nybble | Byte |
> |---|---|---|
> | 46 | A27-A24 | N/A |
> | 44 | A31-A28 | A31-A24 |
> | 4A | A19-A16 | N/A |
> | 48 | A23-A20 | A23-A16 |

**Register 48 is written last.** **Printed page 436** then says what that last
write does:

> Note that writing to register 48 actually configures the board for both
> Zorro II and Zorro III boards in the Zorro II configuration block. For
> Zorro III PICs in the Zorro III configuration block, the action is slightly
> different. […] The actual configuration takes place when register 44 is
> written, thus supporting any physical size of configuration register.

The two sentences draw a line between two **address spaces**: in the Zorro II
configuration block at `$E8xxxx`, the write to **48** configures, whatever kind
of board it is; the rule that **44** configures belongs to the Zorro III
configuration block at `$FF000000`.

## RTL and measurement

NanoMig has only the Zorro II configuration block. The Zorro III block cannot be
reached: `cpu_wrapper.v:34` defines `TG68K_A24` and `:60` declares
`output reg [23:1] chip_addr`, so `$FF000000` is not representable on this bus.
Inside the Zorro II block the write path applies the Zorro III rule:

```verilog
cpu_wrapper.v:611  assign sel_autoconfig = |fastramcfg_d && chip_addr[23:16] == 8'b11101000 && |autocfg_card; //$E80000 - $E8FFFF
cpu_wrapper.v:639  if (~cfg_z3) begin
cpu_wrapper.v:640      if (chip_addr[6:1] == 6'b100100) begin   // Register 0x48 - config, ZII RAM
cpu_wrapper.v:653  end else if (chip_addr[6:1] == 6'b100010) begin  // Register 0x44, assign base address to ZIII RAM.
```

With `cfg_z3` set, a write to register 48 matches neither arm and nothing
happens.

[`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address)
writes register 48 with `cfg_z3` set and reads register 00 back: raw stays
`0xA`, not the `0x0` a configured board would read — `sol:K-0844` and
`opus:K-0881`, both red. An inverse control in the same package writes register
48 with `cfg_z3` clear, and configuration does complete. The same test source
built with fx68k,
[`hrm/appK-autoconfig-base-address-fx68k`](../../hrm/appK-autoconfig-base-address-fx68k),
gives the same two reds and the same control.

## Risks and unknowns

- **No shipped build presents a Zorro III board.** `cfg_z3` can follow the
  configuration only where `ENABLE_RAM32` is defined, the icepi-zero,
  icesugar-pro and both MiSTle targets (elsewhere it is wired to 0,
  `cpu_wrapper.v:535-540`), and it needs `fastramcfg_d[2]`, whose source bit every board top ties low:
  `wire [2:0] fastram_config = { 1'b0, osd_fastmem };`
  ([`src/mistle/gw5a_25/top.sv:494`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/mistle/gw5a_25/top.sv#L494)
  and the same line in every other top). The package reaches the path at the
  module's own ports.
- That NanoMig is at fault, or that anything breaks, is not shown: nothing here
  says what a system writes to a Zorro III board. What is measured is that the
  design does not do what one printed sentence says and that the implementations
  that can speak do not either.
- The page is the likely defect, not proven wrong; the TC64 comment below shows
  the sentence is hard to satisfy against real software.
- vAmiga and Denise have no Zorro III board type, so they cannot speak.

## Independent evidence

| | register 48 in the `$E8` block, Zorro III board | |
|---|---|---|
| **WinUAE** | `expansion.cpp:873-906` — `expamem_bput` is `case 0x48: if (expamem_type() == zorroII) { … } else { expamem_lo = value & 0xff; }`, and `case 0x44: if (expamem_type() == zorroIII) { expamemz3_map(); … }`, where each elided branch configures the board | **latches only; configures on 44 — same as NanoMig** |
| **vAmiga** | `Core/Components/Zorro/ZorroBoard.cpp:132` — `case 0x48:` sets `state = BoardState::ACTIVE` unconditionally; no `0x44` case and no Zorro III board type in `Core/Components/Zorro/` | **cannot speak** |
| **Denise** | `emulation/libami/expansionPort/expansionPort.cpp:17` — `case 0x48:` sets `boardState = BoardState::Configured` unconditionally; no `0x44` case, no Zorro III board type | **cannot speak** |
| **Copperline** | `src/zorro.rs:1020-1047` — at `AUTOCONFIG_BASE`, `EC_Z3_BASEADDRESS_PHYS` (`0x44`) configures when `board.spec.version == ZorroVersion::III` and the write is 16 bits or wider (`size >= 2`), and `EC_BASEADDRESS_PHYS` (`0x48`) configures only when the version is `ZorroVersion::II`. Its own comment: "Zorro III boards take a 16-bit write of the base address high word here, which both assigns and configures" | **configures on 44 — same as NanoMig** |
| **MiSTer** | descendant — `rtl/cpu_wrapper.v:522-527` gives its Zorro II memory card `6'b100100` (0x48) and nothing else; `:540-552` gives the Zorro III card `6'b100010` (0x44), so a Zorro III card there does not configure on 48 either | **inherited** |
| **TC64** | descendant — `rtl/minimig/minimig_autoconfig.v:92` is `9'h048: // Zorro II configures at 48`; `:117` is `9'h044: // Zorro III configures at 44 if in ZIII space, should be 48 in ZII space but seems to configure twice?` | **inherited, with a question mark** |

Not one of the four implements the printed sentence for a Zorro III board in the
Zorro II block. The two that have a Zorro III board type both configure on
register 44, exactly as NanoMig does. The two that configure on 48 do so because
every board they own is a Zorro II board (vAmiga's `DiagBoard`, `HdController`
and `RamExpansion`, Denise's `FastMemExpansion` and `HDController`), so their 48
is the page's Zorro II rule, not a contradiction of NanoMig's Zorro III one.

WinUAE and Copperline both configure a Zorro III board on register 44 inside
the Zorro II block, which is the behaviour page 436 assigns only to the Zorro III block. TC64's
autoconfig module is not derived from the same file as NanoMig's and arrives at
the same place with a comment that wrote the doubt down.

## Decision

No RTL change. Somebody who can read `expansion.library`'s Zorro III path, or who
has put a real Zorro III card in a Zorro II machine, says which write the system
actually configures on. If the page is right, NanoMig and two emulators are wrong
together; if the page is wrong, it is wrong in the one sentence that tells a board
designer when to stop answering.
