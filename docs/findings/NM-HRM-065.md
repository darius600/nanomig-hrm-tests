# NM-HRM-065 — register 48 does not configure a Zorro III board, and the implementations side with NanoMig

| | |
|---|---|
| **Status** | open · **against the PAGE**, not against the RTL |
| **Module** | `src/minimig-aga/cpu_wrapper.v:629-644`, the AUTOCONFIG write path — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/cpu_wrapper.v#L629-L644) at the pinned revision |
| **Report to** | **the Minimig family** if it is reported at all — the behaviour is **inherited**, identical in MiSTer and reached independently by TC64. No RTL change is proposed; the question is whether printed page 436 describes what Commodore's own system software did |
| **Test** | [`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address) · two graded assertions — **both red** |
| **Chapter** | Appendix K — Zorro Expansion Bus · printed pp. 435 and 436 |
| **Assertions** | 2 red, with an inverse control that turns green for a Zorro II board on the identical write |
| **Verdict of the four** | **two of four do exactly what NanoMig does; the other two have no Zorro III board to disagree with** |

---

## What the HRM says

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
configuration block at `$E8xxxx`, the write to **48** configures, whatever
kind of board it is; the rule that **44** configures belongs to the Zorro III
configuration block at `$FF000000`.

## What NanoMig does

**NanoMig has only the Zorro II configuration block.**

```verilog
cpu_wrapper.v:601  assign sel_autoconfig = fastramcfg_d && chip_addr[23:16] == 8'b11101000 && autocfg_card; //$E80000 - $E8FFFF
```

The Zorro III block cannot be reached at all: `cpu_wrapper.v:34` defines
`TG68K_A24` and `:60` declares `output reg [23:1] chip_addr`, so `$FF000000`
is not representable on this bus. Every AUTOCONFIG access NanoMig ever sees is
in the block page 436 assigns to register 48.

**And inside that block it applies the other rule.** The whole write path is
gated by `sel_autoconfig`, and then:

```verilog
cpu_wrapper.v:629  if (~cfg_z3) begin
cpu_wrapper.v:630      if (chip_addr[6:1] == 6'b100100) begin   // Register 0x48 - config, ZII RAM
cpu_wrapper.v:634  end else if (chip_addr[6:1] == 6'b100010) begin  // Register 0x44, assign base address to ZIII RAM.
```

With `cfg_z3` set, a write to register 48 matches neither arm and nothing
happens.

## What holds the finding up

`hrm/appK-autoconfig-base-address`, two assertions, both red. The fixture
drives a real 68000 program through `cpu_wrapper`'s own bus — no internal
signal is touched — writes register 48 in the `$E8` block, and reads the board
back:

    under Zorro III the board keeps responding after the write, raw nibble 0xA

An inverse control in the same package writes register 48 with `cfg_z3` clear,
and configuration **does** complete. So the fixture can observe configuration;
under Zorro III there is nothing to observe after a write to 48.

## What the others say

| | register 48 in the `$E8` block, Zorro III board | |
|---|---|---|
| **WinUAE** | `expansion.cpp:873-906` — the `$E8` handler `expamem_bput` is `case 0x48: if (expamem_type() == zorroII) { …configure… } else { expamem_lo = value & 0xff; }`, and `case 0x44: if (expamem_type() == zorroIII) { expamemz3_map(); …configure… }` | **latches only; configures on 44 — same as NanoMig** |
| **Copperline** | `src/zorro.rs:1020-1047` — at `AUTOCONFIG_BASE`, `EC_Z3_BASEADDRESS_PHYS` (`0x44`) configures when `board.spec.version == ZorroVersion::III`, and `EC_BASEADDRESS_PHYS` (`0x48`) configures only when the version is `ZorroVersion::II`. Its own comment: "Zorro III boards take a 16-bit write of the base address high word here, which both assigns and configures" | **configures on 44 — same as NanoMig** |
| **vAmiga** | `Core/Components/Zorro/ZorroBoard.cpp:132` — `case 0x48:` sets `state = BoardState::ACTIVE` unconditionally. There is **no `0x44` case and no Zorro III board type** in `Core/Components/Zorro/` | **cannot speak** |
| **Denise** | `emulation/libami/expansionPort/expansionPort.cpp:17` — `case 0x48:` sets `boardState = BoardState::Configured` unconditionally. **No `0x44` case**, no Zorro III board type | **cannot speak** |
| **MiSTer** | descendant — **structurally identical.** `rtl/cpu_wrapper.v:522-527` gives its Zorro II memory card `6'b100100` (0x48) and nothing else; `:540-552` gives the Zorro III card `6'b100010` (0x44). A Zorro III memory card there does not configure on a write to 48 either | **inherited** |
| **TC64** | descendant — its own module reaches the same place and **writes the doubt into a comment**. `rtl/minimig/minimig_autoconfig.v:92` is `9'h048: // Zorro II configures at 48`; `:117` is `9'h044: // Zorro III configures at 44 if in ZIII space, should be 48 in ZII space but seems to configure twice?` | **inherited, with a question mark** |

**Not one of the four implements the printed sentence for a Zorro III board in
the Zorro II block.** The two that have a Zorro III board type both configure
on register 44, exactly as NanoMig does. The two that configure on 48 do so
because every board they own is a Zorro II board — vAmiga's `DiagBoard`,
`HdController` and `RamExpansion`, Denise's `FastMemExpansion` — so their 48
is the page's Zorro II rule, not a contradiction of NanoMig's Zorro III one.

## Why this is filed against the page

WinUAE and Copperline are calibrated against real Kickstart images: what their
`$E8` handlers accept is what Commodore's own `expansion.library` actually
writes. Both configure a Zorro III board on register 44 **inside the Zorro II
block**, which is the behaviour page 436 assigns only to the Zorro III block.
That is two independent implementations reading the hardware the same way and
the printed sentence standing alone.

**And a fifth reading, from inside the Minimig family, wrote the doubt down.**
`MinimigAGA-MiST-TC64`'s autoconfig module is not derived from the same file as
NanoMig's, and it arrives at the same place with a comment nobody wrote for this
project:

```verilog
minimig_autoconfig.v:117
9'h044 : begin // Zorro III configures at 44 if in ZIII space,
               // should be 48 in ZII space but seems to configure twice?
```

An implementer who had the page in front of them, read the same sentence, built
the other behaviour, and left a question mark. That is not proof the page is
wrong. It is evidence that the sentence is hard to satisfy against real
software, which is exactly what this finding claims.

This is the pattern the finding key exists for: the key names the pair that
disagrees, and does not prejudge which side is wrong.

## Suggested change

Not an RTL change. Somebody who can read `expansion.library`'s Zorro III path,
or who has put a real Zorro III card in a Zorro II machine, saying which write
the system actually configures on. If the page is right, NanoMig and two
emulators are wrong together; if the page is wrong, it is wrong in the one
sentence that tells a board designer when to stop answering.

## For the specialists

That NanoMig is at fault, and that anything breaks. NanoMig's Zorro III fast
RAM is configured in practice, which is itself evidence about what the system
writes. What is measured is that the design does not do what one printed
sentence says, and that the implementations that can speak do not either.
