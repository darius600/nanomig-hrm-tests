# NM-HRM-085 — a configured Zorro II memory board answers across the whole 8 MB Zorro II space, not only at the base software wrote to it [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether a Zorro II memory board configured by a write to register 48 should answer only from the base byte written there, for the 2 MB that register 00 declares, as pp. 391 and 435 print, instead of at all of $200000–$9FFFFF as NanoMig's select does. The question stops at the board's own decode: where system software places the board is outside it. It closes when `cpu_wrapper.v:117` compares the base written and `P3-NOTOTHER` and `P3-BASE` are green on both cores, or when an Amiga specialist rejects the reading; if that means pp. 391 and 435 are wrong, the page is refiled against the printed page |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer and minimig-dev/MinimigAGA-MiST-TC64 |
| **Scope** | one Zorro II memory board (register 00 size code `110`, 2 MB) in the whole design at `nanomig.v`'s ports, `fastram_config` = 1, on both CPU cores: the base byte written to register 48 ($20 and $40), the reads at $200000–$9FFFFE, and two controls, $A00000 with the board configured and $400000 with the board never configured. `fastram_config` = 3 (three boards) is read from the source only. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix K — Zorro Expansion Bus, printed pp. 384, 386, 388, 391, 395, 431, 433, 434, 435, 436 · *Amiga Hardware Reference Manual*, 3rd ed., 1991. Claim ids by assertion: `P3-NOTOTHER` (pp. 391, 433–435) and `P3-BASE` (pp. 388, 391, 433, 435) each carry `sol:K-0837` and `opus:K-0869` (both p. 435). No claim id: `P3-NOTOTHER-ASSIGNED` and `P3-BASE-ASSIGNED` (pp. 391, 435), `P3-CONTENT` (p. 433), `P3-IMG2` (pp. 431, 435), `P3-CONTROL-IO` (p. 386), `P3-CONTROL-UNCONF` (p. 431) |
| **Module** | `src/minimig-aga/cpu_wrapper.v:117`, the Zorro II RAM select, and `:639-642`, the register-48 write — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L117) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/memory_router.v#L35); inherited (MiSTer has the same select; TC64 decodes the same range in its own file) |
| **Test** | [`integration/appK-autoconfig`](../../integration/appK-autoconfig): `make -k -C integration/appK-autoconfig test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero |
| **Assertions** | `P3-NOTOTHER` (`test_appK_autoconfig.cpp:878-882`): the board configured at $200000 should not also answer at $400000; the word written there, P2 = $5AA5, reads back as $5AA5. `P3-BASE` (`:901-905`): with $40 written to register 48 (base $400000) the board should answer at $400000 and not at $200000; P2 reads back at $200000. Red on fx68k and on TG68K, 25 passed, 3 failed on each core; the third red, `GATE-G`, is [NM-HRM-064](NM-HRM-064.md)'s. Green on both cores in the same run, the controls on the same comparator: `P3-CONTROL-IO` and `P3-CONTROL-UNCONF` read $0000 where the reds read P2 |
| **Verdict of the four** | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline take the base from the byte written to register 48 and map a Zorro II memory board from that base, for its own size and nowhere else. MiSTer and TC64, descendants, select $200000–$9FFFFF once the board is configured, whatever was written, as NanoMig does. Hardware: not measured |
| **Effort** | M: `cpu_wrapper.v` only — latch the byte written to 48 (`:639-642`), compare it in `sel_z2ram` (`:117`) for the size register 00 declares, and form the RAM address (`:216-217`) from the offset into the board; with `fastram_config` = 3 each of the three boards needs its own base |
| **See also** | [NM-HRM-064](NM-HRM-064.md), the shut-up register, red in the same package as `GATE-G`; [NM-HRM-065](NM-HRM-065.md), register 48 and a Zorro III board |

---

## Source requirement

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

Three passages on these pages could be read as predicting an answer outside
the base; none states one (see *Risks and unknowns*). Register 08 bit 7,
printed p. 433:

> This was originally an indicator to place the card in the 8 megabyte Zorro II
> space, when set, or anywhere it'll fit, if cleared.

Memory wraps, printed p. 434:

> The memory is sized in 512K increments, and grows from the base address
> upward. Memory wraps are detected, but the design must insure that its data
> bus doesn't float when the sizing routine addresses memory locations that
> aren't physically present on the board; data bus pullups or pulldowns are
> recommended.

Footnote 6, printed p. 435:

> Support for this is impossible, since the configuration registers generally
> disappear when a board is configured, and absolutely must move out of the
> $00E8xxxx space.

## RTL and measurement

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
decoded anywhere in the module. The Zorro III arm stores
`z3ram_base1 <= cpu_dout[15:12];`
([`#L655`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L655))
and compares it
([`#L116`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L116));
the Toccata card, built only with `ENABLE_TOCCATA`, stores
`toccata_base <= cpu_dout[7:0];`
([`#L634`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L634))
and `gary.v` compares it
([`src/minimig-aga/gary.v#L197`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L197)).

**The select then takes the whole Zorro II space** —
[`#L117`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L117):

```verilog
wire sel_z2ram  = (cpu_addr[31:24] == 8'h00) && (cpu_addr[23] ^ |cpu_addr[22:21]) && z2ram_ena; // addr[23:21] = 1..4
```

`cpu_addr[23:21]` of 1 to 4 is $200000–$9FFFFF. `sel_z2ram` feeds `ramsel`
([`#L167`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L167)),
the request that reaches `nanomig.v`'s `fastram_sel` port
([`src/nanomig.v#L259`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/nanomig.v#L259)
in the fx68k build; through the CPU cache in the TG68K build). The select
names no base and no size, although register 00 declares one: with
`fastram_config` = 1 it reads size code `110`
([`#L572`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L572)),
and the module's own comment says where the design expects the board,
`// Zorro II RAM (Up to 8 meg at 0x200000)`
([`#L567`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L567)).
In a build without `ENABLE_RAM32`, which is both of the package's, the RAM
address is `{1'b0, cpu_addr[22], cpu_addr[21] | cpu_addr[23]};`
([`#L216-L217`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_wrapper.v#L216-L217)),
so $800000–$9FFFFF reach the same RAM addresses as $200000–$3FFFFF, and
$400000–$7FFFFF reach RAM addresses of their own.

### Measured

[`integration/appK-autoconfig`](../../integration/appK-autoconfig) runs the
whole design at `nanomig.v`'s ports, once with each CPU core, with
`fastram_config` = 1. Its programs write a nybble of $0 to 4A and then a byte
to 48, which carries A23-A16 (p. 435):

| run | base written to 48 | after the write | read back | assertion | fx68k | TG68K |
|---|---|---|---|---|---|---|
| R3, R3n | $20, so $200000 | P0 = $A55A at $200000 and P1 = $3CC3 at $200002, a $0000 write to a chip-RAM word, both read | P0, P1 | `P3-CONTENT` (R3, R3n), `P3-NOTOTHER-ASSIGNED` (R3n) | green | green |
| R3n | $20, so $200000 | then P2 = $5AA5 at $400000, a $0000 write to a chip-RAM word, $400000 read | **P2** | `P3-NOTOTHER` | **red** | **red** |
| R3' | $40, so $400000 | P0 and P1 at $400000 and $400002, a $0000 write to a chip-RAM word, both read | P0, P1 | `P3-BASE-ASSIGNED` | green | green |
| R3n' | $40, so $400000 | the same, then P2 at $200000, a $0000 write to a chip-RAM word, $200000 read | **P2** | `P3-BASE` | **red** | **red** |
| R3n at $A00000 (`prog_r3_io.s`) | $20, so $200000 | as R3n, with P2 written at $A00000 and $A00000 read | $0000 | `P3-CONTROL-IO` | green | green |
| R3n unconfigured (`prog_r3_unconf_n.s`) | not written; 4A only | P2 at $400000, a $0000 write to a chip-RAM word, $400000 read | $0000 | `P3-CONTROL-UNCONF` | green | green |

The red lines print their claim ids and pages as
`P3-NOTOTHER (sol:K-0837, opus:K-0869) [p391,p433-435]` and
`P3-BASE (sol:K-0837, opus:K-0869) [p388,p391,p433,p435]`, and both quote
p. 391's and p. 435's sentences. Each is an aggregate: `P3-NOTOTHER` passes
when the board answers at the base it was given (graded alone by
`P3-NOTOTHER-ASSIGNED`, `:875-877`, green on both cores) and the run never
finishes or the word read at the other base differs from P2, so the red is that
R3n finishes and reads P2 back at a base the board was not given. The
preconditions, register 00's memory flag and the $0000 chip-RAM write between
the write of P2 and its read (`P3-NOTOTHER-PRECOND`, `:870-872`), hold on both
cores. `P3-BASE` is the same aggregate over R3' and R3n'
(`P3-BASE-ASSIGNED`, `:898-900`, grades P0 and P1 at $400000). The two
controls run the red lines' own comparator, `!done || R2 != P2`, where the board
is not to answer: `P3-CONTROL-IO` (`:919-922`) at $A00000, Zorro II I/O
expansion space in Figure K-1 (p. 386), with the board configured at $200000;
`P3-CONTROL-UNCONF` (`:934-937`) at $400000 with the board never configured.
Both finish and read $0000, not P2, so the comparator goes green on a live
input. The log prints R2 for each of the four runs, as
`MEASURED: P3-NOTOTHER run R3n: completion marker written, R2 = $5AA5`, the
same for R3n', and `R2 = $0000` for the two control runs, the same on both
cores.

**The select, probed.** A probe program with the package's environment,
harness and settings — a run outside the package that `make test` does not print, a measurement, not a graded assertion — configures the
board, writes a distinct word at nine addresses and reads each back, logging
`nanomig.v`'s `fastram_sel` and `fastram_addr`:

| CPU address | select, 48 ← $20 and 48 ← $40 | address at the port | read back, both bases | select, 48 never written |
|---|---|---|---|---|
| $200000, $3FFFFE | yes | $200000, $3FFFFE | the word written at $800000, at $9FFFFE | no |
| $400000, $5FFFFE, $600000, $7FFFFE | yes | the same | each its own word | no |
| $800000, $9FFFFE | yes | $200000, $3FFFFE | each its own word | no |
| $A00000 | no | — | $0000 | no |

Both cores and both bases give the same table; the two bases give identical
logs. The *address at the port* is the write's: the TG68K build reads through
its cache, so its reads reach the port as line fills at the eight-byte-aligned
address, $3FFFF8 for $3FFFFE. Without the write to 48 no access reaches the
port, and the reads return $FFFF at $200000 and $3FFFFE and $0000 at the other
seven. So the base written is not used (told $400000, the board answers at
$200000 exactly as when told $200000), the size declared is not used (a 2 MB
board answers for 8 MB), and the answer stops at $A00000, Zorro II I/O space in
Figure K-1, which reaches no select.

## Risks and unknowns

- **Not measured:** real hardware, and software: nothing here measures where
  system software places the board or what it does with the rest of the space.
  A 2 MB board placed at $200000, as NanoMig's comment expects, also answers at
  $400000–$9FFFFF, memory nobody was given; a program that probes the Zorro II
  space for memory, or a second board placed there, would find it.
- **The probe's reads outside $200000–$3FFFFF are partly the harness's:** its
  fast-RAM model is a flat memory of 4M words
  ([`nanomig_harness.h`](../../integration/harness/nanomig_harness.h),
  `kFastWordCount = 1u << 22`) that stores whatever reaches the port. The
  RTL-side fact is the select, and the alias of $800000–$9FFFFF onto
  $200000–$3FFFFF is the RTL's address mapping. Builds with `ENABLE_RAM32` are
  not measured.
- **Conflicting passages:** register 08 bit 7 tells software where to put the
  board and does not widen the board's decode; NanoMig's board reads it 0
  anyway, since neither half of register 08 is in the Zorro II case list at
  `cpu_wrapper.v:568-590` and both read logical 0 through the default at `:547`.
  Memory wraps are the operating system sizing a board whose sub-size is 0001,
  inside the size register 00 declares; NanoMig's sub-size, register 08's low
  nybble, reads 0000, "Logical size matches physical size", by the same
  default. Footnote 6 is about the configuration registers leaving $00E8xxxx,
  which NanoMig's board does (`P3-IMG2`, green); it says nothing about where
  the board then answers.
- **Is the whole-space select a deliberate economy?** Both Minimig lines have
  it for the memory board and not for their I/O cards, and NanoMig's address
  map (`:199-205`, `:216`) assigns RAM by CPU address. Whether any build needs
  its Zorro II memory anywhere but $200000 is for the maintainers of that map
  to say.
- **The suggested change has not been run.** Its effect with
  `fastram_config` = 3, three Zorro II boards of 4 MB, 1 MB and 512K
  (`:577-580`), each configured by its own write to 48, is read from the source
  and not measured.
- The 1987 quotations carry the page's OCR as it is, "AZ3" for A23 included.

## Independent evidence

The question put to each: once a Zorro II memory board is configured, which
addresses does it answer?

| | where the base comes from, and what is compared | answers at |
|---|---|---|
| **WinUAE** | [`expansion.cpp:873-878`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/expansion.cpp#L873-L878): a byte write to 48 sets `expamem_board_pointer = (expamem_hi \| (expamem_lo >> 4)) << 16;` and calls the board's map function, [`expamem_map_fastcard`, `:1617-1631`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/expansion.cpp#L1617-L1631); [`memory.cpp:3887-3893`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L3887-L3893) maps that many 64K banks from the base, and [`:3876-3881`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L3876-L3881) refuses to map a Z2 board over a bank already mapped at its base | **the base written, for the board's size** |
| **vAmiga** | [`Core/Components/Zorro/ZorroBoard.cpp:132-147`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Zorro/ZorroBoard.cpp#L132-L147): `case 0x48:` takes `baseAddr \|= (value & 0xF0) << 16;`, and 4A adds A19-A16; [`RamExpansion.cpp:45-56`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Zorro/RamExpansion.cpp#L45-L56) marks only the board's pages `MemSrc::FAST` in a map that [`Memory.cpp:863-865`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L863-L865) starts with every page `MemSrc::NONE`; the data offset is base-relative, [`Memory.h:24`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.h#L24) | **the base written, for the board's size** |
| **Denise** | [`emulation/libami/expansionPort/expansionPort.cpp:17-24`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/expansionPort/expansionPort.cpp#L17-L24): `case 0x48:` takes `baseAdr \|= (data & 0xf0) << 16;`, and 4A adds A19-A16; `add()`, [`:50-60`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/expansionPort/expansionPort.cpp#L50-L60), maps the board's pages only, over a Zorro II space [`agnus/memory.cpp:703-704`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L703-L704) leaves `Unmapped` | **the base written, for the board's size** |
| **Copperline** | [`src/zorro.rs:1035-1046`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/zorro.rs#L1035-L1046): for a Zorro II board `let base = u32::from(data_byte) << 16;`; [`:885-903`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/zorro.rs#L885-L903) records the window, and [`:909-917`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/zorro.rs#L909-L917) is the comparator, which the CPU's memory path asks, [`src/cpu.rs:3594`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L3594) | **the base written, for the board's size** |
| **MiSTer** | descendant — the same select in its own file: [`rtl/memory_router.v:35`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/memory_router.v#L35); the memory card's write to 48, [`rtl/cpu_wrapper.v:522-526`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cpu_wrapper.v#L522-L526), is `z2ram_ena <= 1;` with no base, while its I/O cards keep one, `toccata_base <= cpu_dout[7:0];` and `a2065_base <= cpu_dout[7:0];` ([`:530-536`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cpu_wrapper.v#L530-L536)), compared in [`rtl/gary.v:199-200`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L199-L200) | **$200000–$9FFFFF, as NanoMig** |
| **TC64** | descendant — its own module, the same range: [`rtl/minimig/minimig_autoconfig.v:92-100`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig_autoconfig.v#L92-L100) sets `board_configured[0] <= 1'b1;` for the RAM board with no base, where the Toccata card stores `toccata_base_addr <= data_in[7:0];` ([`:106`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig_autoconfig.v#L106)); [rtl/soc/TG68K.vhd, lines 303-307](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/soc/TG68K.vhd#L303-L307) selects `cpuaddr(23 downto 21)` of `"001"` to `"100"` and [`:318-319`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/soc/TG68K.vhd#L318-L319) enables it with `(sel_z2ram='1' and z2ram_ena='1')`, whatever size [`minimig_autoconfig.v:72`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig_autoconfig.v#L72) declares | **$200000–$9FFFFF, as NanoMig** |

None of the four has a Zorro II board that answers for the whole space.
WinUAE and Copperline take the whole byte; vAmiga and Denise take its upper
nybble and add A19-A16 from 4A.

### What other Commodore documents say

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF pages 28, 29 and 31 — agrees with the manual:

  > "Under normal conditions, the processor determines how much address space the PIC requires and then loads the PIC's address latch with an appropriate base address. This permanently relocates the PIC at its new address (until Reset)"
  <!-- corpus: a500-a2000-trm-1987/p028.txt -->

  > "All PICs should be designed to be based on boundaries that match their space requirements; for exam- ple, one megabyte PICs should be designed to reside on one mega- byte boundaries (match circuit matches A23-A20)."
  <!-- corpus: a500-a2000-trm-1987/p028.txt -->

  > "The address map is subject to change. A PIC should assume that it may be placed anywhere in the address space."
  <!-- corpus: a500-a2000-trm-1987/p029.txt -->

  > "(48/4A) 7654 321 0 Base address register, write only a bits are compared with AZ3 through A16 (or fewer) to determine the base address of this board."
  <!-- corpus: a500-a2000-trm-1987/p031.txt -->

  The expansion chapter's design guidelines give a board an address latch that software loads with its base, a match circuit on the high address bits for its size, and the 48/4A register as the value those bits are compared with; its notes tell a board to assume it may be placed anywhere. NanoMig's Zorro II memory board keeps no base and matches a fixed range.

## Decision

**One option: give the Zorro II memory board a base register and compare
against it.** In the Zorro II arm of the write path, latch the byte written
to 48, as the Zorro III arm at `:655` and the Toccata arm at `:634` already
latch theirs, and replace the fixed range of `sel_z2ram` with a compare of
the address bits the declared size leaves: `cpu_addr[23:21]` for a 2 MB
board, `cpu_addr[23:22]` for a 4 MB board. The RAM address at `:216-217` would
then be formed from the offset into the board rather than from the CPU
address. What it would change:

- with the board placed at $200000, as the RTL's comment expects, a 2 MB board
  would stop answering at $400000–$9FFFFF;
- a board placed anywhere else would answer where it was placed;
- with `fastram_config` = 3 the module presents three Zorro II boards, 4 MB,
  1 MB and 512K (`:577-580`), each configured by its own write to 48, and the
  address map (`:199-205`, `:216`) assigns their memory by CPU address; each
  would need its own base.
