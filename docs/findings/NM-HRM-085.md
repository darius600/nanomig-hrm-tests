# NM-HRM-085 — a configured Zorro II memory board answers across the whole 8 MB Zorro II space, not only at the base software wrote to it [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `src/minimig-aga/cpu_wrapper.v:117`, the Zorro II RAM select, and `:639-642`, the register-48 write — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L117) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/memory_router.v#L35) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, **minimig-dev/MinimigAGA-MiST-TC64** and NanoMig, if the manual's reading is accepted. The select is **inherited**: NanoMig's is MiSTer's, and TC64 decodes the same range in its own CPU interface, so no line has a fix to port, and a fix made upstream must be ported to NanoMig deliberately, as there is no automatic merge; see *What the others say* |
| **Test** | [`integration/appK-autoconfig`](../../integration/appK-autoconfig) — two assertions, run on both CPU cores: `P3-NOTOTHER` and `P3-BASE`, red on fx68k and on TG68K. Neither carries a claim id |
| **Chapter** | Appendix K — Zorro Expansion Bus, printed pp. 384, 386, 388, 391, 395, 433–436 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red on fx68k and 2 on TG68K (`P3-NOTOTHER`, `P3-BASE`; 14 passed, 3 failed on each core). The third red on each core, `GATE-G`, is [NM-HRM-064](NM-HRM-064.md)'s. In the same run `P3-CONTENT`, green on both cores, reads back what was written at the base the board was given, and `P3-IMG2`, green, shows the board leaving $E8xxxx on the register-48 write |
| **Verdict of the four** | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline take the base from the byte written to register 48 and map a Zorro II memory board from that base, for its own size and nowhere else. MiSTer and TC64, descendants, select $200000–$9FFFFF once the board is configured, whatever was written, as NanoMig does |
| **See also** | [NM-HRM-064](NM-HRM-064.md), the shut-up register, red in the same package as `GATE-G`; [NM-HRM-065](NM-HRM-065.md), register 48 and a Zorro III board |

---

## What the HRM says

**Printed p. 391**, on AUTOCONFIG:

> On system powerup, the system software interrogates each board to
> determine what kind of board is installed and how much memory space it
> needs on the bus. The software then tells each board where to reside in
> memory. The bus provides hardware lines to allow the boards to be
> configured in a daisy chained fashion regardless of which slots they
> occupy and to prevent damage to boards if accidently configured to reside
> at the same memory location.

**Printed p. 384** says it of the bus from the start:

> Addressing jumpers were eliminated, the card's address instead being
> assigned by software

**Printed p. 435** names the register the address is written to, and the bits
it carries:

> 44 46 144 7-0 High order base address register, write only.
> 48 4A 148 7-0 Low order base address register, write only.
>
> The high order register takes bits 31-24 of the board's configured address,
> the low order register takes bits 23-16.

and **printed p. 436** says when that write takes effect:

> Note that writing to register 48 actually configures the board for both
> Zorro II and Zorro III boards in the Zorro II configuration block.

**Printed p. 433** gives the board's size in register 00, bits 2-0:

> These bits indicate the configuration size of the PIC.

The table under the sentence gives code `110` as 2 megabytes in the
Unextended column, the one a Zorro II board uses. Register 08's sub-size
paragraph on the same page places a memory board's memory at the base:

> A memory card, with memory starting at the base address, can be
> automatically sized by the Operating System.

The same paragraph, about how much logic a Zorro III design saves, says what a
board uses to recognise its base, from p. 433:

> since 16-megabyte cards and up can be designed with a single latch and

onto **printed p. 434**:

> comparator for base address matching, while 8 megabyte and smaller PICs
> require large latch/comparator circuits not available in standard TTL
> packages.

**Printed p. 388** gives the space the base is chosen from:

> Memory space is the standard 8 megabyte space from $00200000-$009FFFFF.

That is the space all Zorro II memory boards share. **Printed p. 395** says
what a board drives when it answers, and what happens when two answer for one
address:

> Whenever a Zorro II PIC is responding to an address on the bus, it must
> assert its /SLAVE output within 35ns of /AS asserted.

> If more than one /SLAVE output occurs for the same address, or if a PIC
> asserts its /SLAVE output for an address reserved by the local bus, a
> collision is registered and results in /BERR being asserted.

Read together: **a Zorro II memory board answers from the base software writes
to register 48, for the size register 00 declares, and nowhere else in the
Zorro II space.** A 2 MB board configured at $200000 owns $200000–$3FFFFF;
$400000–$9FFFFF is space software may give to another board.

### Nothing on these pages predicts an answer outside the base

- **Register 08 bit 7**, printed p. 433: "This was originally an indicator to
  place the card in the 8 megabyte Zorro II space, when set, or anywhere it'll
  fit, if cleared." It tells software where to put the board and does not
  widen the board's decode. NanoMig's board reads it 0 anyway: neither half of
  register 08 is in the Zorro II case list at `cpu_wrapper.v:568-590`, so both
  read logical 0 through the default at `:547`.
- **Memory wraps**, printed p. 434: "The memory is sized in 512K increments,
  and grows from the base address upward. Memory wraps are detected". That is
  the operating system sizing a board whose sub-size is 0001, inside the size
  register 00 declares. NanoMig's sub-size, register 08's low nybble, reads
  0000, "Logical size matches physical size", by the same default.
- **Footnote 6**, printed p. 435, says the configuration registers "absolutely
  must move out of the $00E8xxxx space" once a board is configured. NanoMig's
  board does (`P3-IMG2`, green); the footnote says nothing about where the
  board then answers.

---

## What NanoMig does

**The write to register 48 ends configuration and keeps nothing of the value
written** —
[`src/minimig-aga/cpu_wrapper.v#L639-L642`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L639-L642):

```verilog
if (~cfg_z3) begin
	if (chip_addr[6:1] == 6'b100100) begin // Register 0x48 - config, ZII RAM
		z2ram_ena <= 1;
		autocfg_card <= 2'b00;
```

The Zorro II arm reads no `cpu_dout`, and register 4A (`6'b100101`) is not
decoded anywhere in the module. The two other arms of the same write path do
keep the value: the Zorro III board stores `z3ram_base1 <= cpu_dout[15:12];`
([`#L655`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L655))
and compares it, `wire sel_z3ram1 = (cpu_addr[31:28] == z3ram_base1) && z3ram_ena1;`
([`#L116`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L116));
the Toccata sound card, built only with `ENABLE_TOCCATA`, stores
`toccata_base <= cpu_dout[7:0];`
([`#L634`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L634))
and `gary.v` compares it,
`assign sel_toccata = toccata_ena && cpu_address_in[23:16]==toccata_base;`
([`src/minimig-aga/gary.v#L197`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L197)).

**The select then takes the whole Zorro II space** —
[`#L117`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L117):

```verilog
wire sel_z2ram  = (cpu_addr[31:24] == 8'h00) && (cpu_addr[23] ^ |cpu_addr[22:21]) && z2ram_ena; // addr[23:21] = 1..4
```

`cpu_addr[23:21]` of 1 to 4 is $200000–$9FFFFF. `sel_z2ram` feeds `ramsel`
([`#L167`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L167)),
the request that reaches `nanomig.v`'s `fastram_sel` port: directly in the
fx68k build,
`assign fastram_sel = ram_sel && (fastram_ready == fastram_ready_d);`
([`src/nanomig.v#L259`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/nanomig.v#L259)),
and through the CPU cache in the TG68K build. The select names no base and
no size, although register 00 declares one: with `fastram_config` = 1 it reads
size code `110`,
`5'b01001: autocfg_data = 4'b0110; // 2MB`
([`#L572`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L572)).
The module's own comment above the case list says where the design expects
the board, `// Zorro II RAM (Up to 8 meg at 0x200000)`
([`#L567`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L567)).

**Where the address lands.** In a build without `ENABLE_RAM32`, which is both
of the package's, the RAM address is
`{1'b0, cpu_addr[22], cpu_addr[21] | cpu_addr[23]};`
([`#L216-L217`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L216-L217)),
so $800000–$9FFFFF reach the same RAM addresses as $200000–$3FFFFF, and
$400000–$7FFFFF reach RAM addresses of their own.

### Measured

[`integration/appK-autoconfig`](../../integration/appK-autoconfig) runs the
whole design at `nanomig.v`'s ports, once with each CPU core, with
`fastram_config` = 1: one Zorro II memory board, register 00 size code `110`,
2 MB. Its programs write a nybble of $0 to 4A and then a byte to 48, which
carries A23-A16 (p. 435):

| run | base written to 48 | after the write | read back | assertion | fx68k | TG68K |
|---|---|---|---|---|---|---|
| R3, R3n | $20, so $200000 | P0 = $A55A at $200000 and P1 = $3CC3 at $200002, a $0000 write to a chip-RAM word, both read | P0, P1 | `P3-CONTENT` | green | green |
| R3n | $20, so $200000 | then P2 = $5AA5 at $400000, a $0000 write to a chip-RAM word, $400000 read | **P2** | `P3-NOTOTHER` | **red** | **red** |
| R3' | $40, so $400000 | P0 and P1 at $400000 and $400002, a $0000 write to a chip-RAM word, both read | P0, P1 | `P3-BASE` | **red**, through R3n' | **red**, through R3n' |
| R3n' | $40, so $400000 | the same, then P2 at $200000, a $0000 write to a chip-RAM word, $200000 read | **P2** | | | |

`P3-NOTOTHER` passes when the run never finishes or the word read differs
from P2, so its red means the run finished and read P2 back at a base the
board was not given. `P3-BASE` takes R3' and R3n' together and is red through
R3n' alone. The words in the *read back* column were read from the package's
own programs, run once more on each core with the same harness and settings
and printed rather than graded: they are the same on both cores.

**The select, measured for this page.** A probe program with the package's
environment, harness and settings — a measurement, not a graded assertion —
configures the board, writes a distinct word at nine addresses and reads each
back, logging `nanomig.v`'s `fastram_sel` and `fastram_addr`:

| CPU address | select, 48 ← $20 | select, 48 ← $40 | address at the port | read back, both bases | select, 48 never written |
|---|---|---|---|---|---|
| $200000 | yes | yes | $200000 | the word written at $800000 | no |
| $3FFFFE | yes | yes | $3FFFFE | the word written at $9FFFFE | no |
| $400000 | yes | yes | $400000 | its own word | no |
| $5FFFFE | yes | yes | $5FFFFE | its own word | no |
| $600000 | yes | yes | $600000 | its own word | no |
| $7FFFFE | yes | yes | $7FFFFE | its own word | no |
| $800000 | yes | yes | $200000 | its own word | no |
| $9FFFFE | yes | yes | $3FFFFE | its own word | no |
| $A00000 | no | no | — | $0000 | no |

Both cores give the same table. The *address at the port* is the write's;
the TG68K build reads through its cache, so its reads reach the port as line
fills at the eight-byte-aligned address, $3FFFF8 for $3FFFFE. The two bases
give identical logs. Without the write to 48 no access reaches
the port, and the reads return $FFFF at $200000 and $3FFFFE and $0000 at the
other seven. So:

- **the base written is not used**: told $400000, the board answers at
  $200000 exactly as it does when told $200000;
- **the size declared is not used**: a 2 MB board answers for 8 MB;
- **the answer stops at the Zorro II space**: $A00000, Zorro II I/O space in
  Figure K-1, reaches no select.

What a read returns outside $200000–$3FFFFF is partly the harness's: its
fast-RAM model is a flat memory of 4M words
([`nanomig_harness.h`](../../integration/harness/nanomig_harness.h),
`kFastWordCount = 1u << 22`) that stores whatever reaches the port. The
RTL-side fact is the select, and the alias of $800000–$9FFFFF onto
$200000–$3FFFFF is the RTL's address mapping.

### Why the unit package does not see it

[`hrm/appK-autoconfig-base-address`](../../hrm/appK-autoconfig-base-address)
drives `cpu_wrapper` alone, with `ramready` and `ramdout` held at 0: nothing
answers behind `ramsel`. It sees the configuration registers at $E8xxxx before
and after a write, so it grades that a write to 48 configures a Zorro II board
(`sol:K-0835`, `opus:K-0867`, green). It cannot see at which addresses the
configured board then answers, and it records `sol:K-0837` and `opus:K-0869`,
p. 435's "the low order register takes bits 23-16", as not graded,
`unreachable-observable`. The integration package has the harness's fast-RAM
model behind `nanomig.v`'s `fastram_*` port, and there the board's addresses
are observable.

---

## What the others say

The question put to each: once a Zorro II memory board is configured, which
addresses does it answer?

| | where the base comes from, and what is compared | answers at |
|---|---|---|
| **WinUAE** | [`expansion.cpp:873-878`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/expansion.cpp#L873-L878): a byte write to 48 for a Zorro II board sets `expamem_hi = value & 0xff;` and `expamem_board_pointer = (expamem_hi \| (expamem_lo >> 4)) << 16;`, then calls the board's map function. For fast RAM that is [`expamem_map_fastcard`, `:1617-1631`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/expansion.cpp#L1617-L1631): `ab->start = start;` and `if (map_banks_z2(ab, ab->start >> 16, size >> 16)) {`. [`memory.cpp:3887-3893`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L3887-L3893) maps that many 64K banks from the base, `map_banks (bank, start, size, 0);`, and its check before it, [`:3876-3881`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L3876-L3881), refuses to map a Z2 board over a bank already mapped at its base, `attempting to override existing memory bank` | **the base written, for the board's size** |
| **vAmiga** | [`Core/Components/Zorro/ZorroBoard.cpp:132-147`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Zorro/ZorroBoard.cpp#L132-L147): `case 0x48:` `baseAddr \|= (value & 0xF0) << 16;`, `state = BoardState::ACTIVE;`, `mem.updateMemSrcTables();`, and 4A adds A19-A16, `baseAddr \|= (value & 0xF0) << 12;`. [`RamExpansion.cpp:45-56`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Zorro/RamExpansion.cpp#L45-L56) marks only the board's pages, `isize firstPage = baseAddr / 0x10000;`, `for (isize i = firstPage; i < firstPage + numPages; i++) {`, `mem.cpuMemSrc[i] = MemSrc::FAST;`, in a map that [`Memory.cpp:863-865`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L863-L865) starts with every page `MemSrc::NONE`; the data offset is base-relative, [`Memory.h:24`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.h#L24) `#define FAST_RAM_STRT ramExpansion.getBaseAddr()` | **the base written, for the board's size** |
| **Denise** | [`emulation/libami/expansionPort/expansionPort.cpp:17-24`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/expansionPort/expansionPort.cpp#L17-L24): `case 0x48: {` `baseAdr \|= (data & 0xf0) << 16;`, `boardState = BoardState::Configured;`, `add();`, and 4A `baseAdr \|= (data & 0xf0) << 12;`. `add()`, [`:50-60`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/expansionPort/expansionPort.cpp#L50-L60), maps the board's pages only, `for (int i = _firstPage; i < (_firstPage + _pages); i++) {` `agnus.mapper[i] = memoryExpansion ? Agnus::FAST_MEM : Agnus::EXPANSION;`, over a Zorro II space [`agnus/memory.cpp:703-704`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L703-L704) leaves empty, `for(int i = 0x20; i <= 0x9f; i++)` `mapper[i] = Unmapped; // auto config (e.g. fast ram)` | **the base written, for the board's size** |
| **Copperline** | [`src/zorro.rs:1035-1046`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/zorro.rs#L1035-L1046): `EC_BASEADDRESS_PHYS => {` for a Zorro II board, `let base = u32::from(data_byte) << 16;` and `board.state = BoardState::Configured { base };`. [`:885-903`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/zorro.rs#L885-L903) records the window, `self.regions.push((base, board.ram.len() as u32, idx));`, and [`:909-917`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/zorro.rs#L909-L917) is the comparator, `let off = addr.wrapping_sub(base);` `if u64::from(off) + size as u64 <= u64::from(len) {`, which the CPU's memory path asks, [`src/cpu.rs:3594`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L3594) `if let Some((board, off)) = self.bus.mem.zorro.region_at(addr, size) {` | **the base written, for the board's size** |
| **MiSTer** | descendant — **the same select, in its own file.** [`rtl/memory_router.v:35`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/memory_router.v#L35) is `assign sel_z2ram    = !cpu_addr[31:24] && (cpu_addr[23] ^ \|cpu_addr[22:21]) && z2ram_ena; // addr[23:21] = 1..4`, and the memory card's write to 48, [`rtl/cpu_wrapper.v:522-526`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cpu_wrapper.v#L522-L526), is `z2ram_ena <= 1;` with no base. Its I/O cards keep one: `toccata_base <= cpu_dout[7:0];` and `a2065_base <= cpu_dout[7:0];` ([`:530-536`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cpu_wrapper.v#L530-L536)), compared in [`rtl/gary.v:199-200`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L199-L200), `assign sel_toccata = toccata_ena && cpu_address_in[23:16]==toccata_base;` | **$200000–$9FFFFF, as NanoMig** |
| **TC64** | descendant — **its own module, the same range.** [`rtl/minimig/minimig_autoconfig.v:92-100`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig_autoconfig.v#L92-L100): `9'h048 : begin // Zorro II configures at 48`, and for the RAM board `board_configured[0] <= 1'b1;` with no base, where the Toccata card stores `toccata_base_addr <= data_in[7:0];` ([`:106`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig_autoconfig.v#L106)). [rtl/soc/TG68K.vhd, lines 303-307](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/soc/TG68K.vhd#L303-L307) selects `cpuaddr(23 downto 21)` of `"001"` to `"100"`, and [`:318-319`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/soc/TG68K.vhd#L318-L319) enables it with `(sel_z2ram='1' and z2ram_ena='1')`, whatever size [`minimig_autoconfig.v:72`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig_autoconfig.v#L72) declares, `2'b01 : ramsize <= 4'b0110; // 2 Meg` | **$200000–$9FFFFF, as NanoMig** |

**All four independent implementations take the base from the write to 48 and
answer only from it, for the board's own size.** WinUAE and Copperline take
the whole byte; vAmiga and Denise take its upper nybble and add A19-A16 from
4A. None of them has a Zorro II board that answers for the whole space.

**Both Minimig lines select $200000–$9FFFFF.** MiSTer's line is NanoMig's, in
a module of its own; TC64 writes the decode in VHDL in its CPU interface and
reaches the same range. Both lines, and NanoMig with `ENABLE_TOCCATA`, keep a
base for their I/O cards and compare it; only the memory board has none.

---

## Suggested change

**One option: give the Zorro II memory board a base register and compare
against it.** In the Zorro II arm of the write path, latch the byte written
to 48, as the Zorro III arm at `:655` and the Toccata arm at `:634` already
latch theirs, and replace the fixed range of `sel_z2ram` with a compare of
the address bits the declared size leaves: `cpu_addr[23:21]` for a 2 MB
board, `cpu_addr[23:22]` for a 4 MB board. The RAM address at `:216-217` would
then be formed from the offset into the board rather than from the CPU
address. This has not been run here. What it would change:

- with the board placed at $200000, as the RTL's comment expects, a 2 MB board
  would stop answering at $400000–$9FFFFF;
- a board placed anywhere else would answer where it was placed;
- with `fastram_config` = 3 the module presents three Zorro II boards,
  4 MB, 1 MB and 512K (`:577-580`), each configured by its own write to 48,
  and the address map (`:199-205`, `:216`) assigns their memory by CPU
  address. Each would need its own base. This is read from the source and not
  measured here.

---

## What holds the finding up

- **The pages are plain on the base.** p. 391 has software tell each board
  where to reside, p. 435 gives register 48 bits 23-16 of that address, and
  p. 433 gives the board its size and its memory "starting at the base
  address". p. 434 says an 8 megabyte or smaller board needs a latch and
  comparator for its base, and p. 395 makes two boards answering one address
  a collision. No page lets a board answer outside its window (see *Nothing on
  these pages predicts an answer outside the base*).
- **The comparator reads green on a live input.** In the same run
  `P3-CONTENT` reads back P0 and P1 at the base the board was given, in both
  runs. In the probe above a word written at $A00000 reads back $0000, and
  with the board unconfigured a word written at $400000 reads back $0000:
  both would pass `P3-NOTOTHER`'s comparator.
- **It is the select, not the harness.** `fastram_sel` fires at both ends of
  each 2 MB block from $200000 to $9FFFFE, and at none of them before the
  write to 48, on both cores, whichever base was written; the code at
  `cpu_wrapper.v:117` selects the whole range.
- **All four implementations follow the page**, each with its base taken from
  the write and its size from the board.

---

## For the specialists

1. **Does anything break?** NanoMig's comment expects the board at $200000.
   Placed there, a 2 MB board also answers at $400000–$9FFFFF, memory nobody
   was given; a program that probes the Zorro II space for memory, or a second
   board placed there, would find it. Nothing here measures where system
   software places the board or what it does with the rest of the space.
2. **Is the whole-space select a deliberate economy?** Both Minimig lines have
   it for the memory board and not for their I/O cards, and NanoMig's address
   map assigns RAM by CPU address. Whether any build needs its Zorro II memory
   anywhere but $200000 is for the maintainers of that map to say.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF pages 28, 29 and 31 — agrees with the manual:

  > "Under normal conditions, the processor determines how much address space the PIC requires and then loads the PIC's address latch with an appropriate base address. This permanently relocates the PIC at its new address (until Reset)"
  <!-- corpus: a500-a2000-trm-1987/p028.txt -->

  > "All PICs should be designed to be based on boundaries that match their space requirements; for exam- ple, one megabyte PICs should be designed to reside on one mega- byte boundaries (match circuit matches A23-A20)."
  <!-- corpus: a500-a2000-trm-1987/p028.txt -->

  > "The address map is subject to change. A PIC should assume that it may be placed anywhere in the address space."
  <!-- corpus: a500-a2000-trm-1987/p029.txt -->

  > "(48/4A) 7654 321 0 Base address register, write only a bits are compared with AZ3 through A16 (or fewer) to determine the base address of this board."
  <!-- corpus: a500-a2000-trm-1987/p031.txt -->

  The expansion chapter's design guidelines give a board an address latch that software loads with its base, a match circuit on the high address bits for its size, and the 48/4A register as the value those bits are compared with; its notes tell a board to assume it may be placed anywhere. That is the manual's contract; NanoMig's Zorro II memory board keeps no base and matches a fixed range. The quotations carry the page's OCR as it is, "AZ3" for A23 included.
