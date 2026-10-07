# NM-HRM-075 — the CIAs answer only at $BF0000–$BFFFFF, where p. 353 selects them on the upper three address bits [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `gary.v` line 190, `sel_cia = cpu_address_in[23:16]==8'hBF`, compare only A23–A21 against `101`, as p. 353 prints, so that $A00000–$BEFFFF reaches the CIAs as on the A1000, A500 and A2000? The difference shows only at addresses p. 314 marks "Reserved. Do not use." Closes when `gary.v` compares the three bits and `CIA-DECODE-DONTCARE` is green, or when an Amiga specialist rejects p. 353's three-bit decode, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | `gary`'s CIA select and the chip selects below it: A23–A16 for the pair, A13 and A12 for the chips. The decode has no mode term, so it is the same in NanoMig's four modes, the OCS-A500 and OCS-A1000 modes among them; measured with `gary` compiled alone at $A00000 and $BE0000 against $BFE001. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 350, 351 and 353; Appendix E, printed p. 337; Appendix D, printed pp. 314–316; Appendix K, printed pp. 385 and 388 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `CIA-DECODE-DONTCARE`: `sol:F-signals.c56`, `sol:F-signals.c63`, `opus:F-conn.c1`, `opus:F-conn.c6` (p. 353); `CIA-AB-TWOBIT-RULE`: `sol:F-signals.c57`, `sol:F-signals.c58`, `opus:F-conn.c2` (p. 353); both assertions carry claim ids |
| **Module** | `gary.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L190-L192) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L193-L195); inherited (MiSTer has the same code; TC64 differs, comparing four bits) |
| **Test** | [`hrm/appF-cia-address-decode`](../../hrm/appF-cia-address-decode): `make -C hrm/appF-cia-address-decode test`; an expected red exits nonzero |
| **Assertions** | 1 red of 5 graded: `CIA-DECODE-DONTCARE` — expected `sel_cia`=1 at $A00000 and $BE0000, observed 0 at both; the package reads 4 passed, 1 failed. `CIA-AB-TWOBIT-RULE`, green in the same run, grades p. 353's two chip selects at $BFE001 and $BFD000 and, at $BFC100, where p. 353 is silent, p. 351's one select line per chip: both chips are selected there. It is not part of this finding |
| **Verdict of the four** | **3–1 for p. 353 on the machines it describes, and no named model decodes NanoMig's window**: WinUAE's models with neither Gayle nor Fat Gary, vAmiga and Denise select the CIAs anywhere in $A00000–$BFFFFF, as p. 353's three bits do; Copperline answers only in $BFD000–$BFDFFF and $BFE000–$BFEFFF, narrower than NanoMig, and so do WinUAE's Gayle and Fat Gary models. Only a WinUAE configuration with the AGA chipset and neither gate array has NanoMig's $BF0000–$BFFFFF. At $BFC100 WinUAE's models without Gayle or Fat Gary, vAmiga and Denise select both CIAs, as NanoMig does and p. 351 predicts. Hardware: not measured |
| **Effort** | S: `gary.v` line 190 only, comparing `cpu_address_in[23:21]` in place of `[23:16]`; upstream the ROM mirrors at $A80000–$B7FFFF in MiSTer's `gary.v` must be left to them |
| **See also** | [NM-HRM-062](NM-HRM-062.md) — the same paragraph of p. 353 prints its two binary patterns interchanged |

---

## Source requirement

**Printed p. 353**, *Hardware Connection Details*:

> The system hardware selects the CIAs when the upper three address bits are 101. Furthermore, CIAA is selected when A12 is low, A13 high; CIAB is selected when A12 is high, A13 low.

and, two sentences on:

> Address bits A11, A10, A9, and A8 are used to specify which of the 16 internal registers you want to access. This is indicated by "r" in the address. All other bits are don't cares.

The page fixes three bits for the pair (A23–A21 = `101`), A13/A12 to choose the chip and A11–A8 for the register, and calls every other bit a don't-care, A20 to A14 among them. Read that way, every address from $A00000 to $BFFFFF with the right A13/A12 reaches a CIA. The next paragraph is a rule for software, not a narrowing of that decode:

> With future expansion in mind, we have decided on the following addresses: CIAA = BFEr01; CIAB = BFDr00. Software must use byte accesses to these address, and no other.

**Appendix D, printed p. 314**, *A1000, A500 and A2000 Memory Map*, as the page image prints it:

| Address Range | Description |
|---|---|
| `A0 0000 - BE FFFF` | Reserved. Do not use. |
| `BF D000 - BF DF00` | 8520-B (access at even-byte addresses only) |
| `BF E001 - BF EF01` | 8520-A (access at odd-byte addresses only) |

The row tells software to stay out of $A00000–$BEFFFF; it does not say the CIAs do not answer there.

**The A3000 gave part of the range away.** Appendix K, **printed p. 385**:

> The memory map in Figure K-1 shows how these three spaces are mapped in the A3000 system.

and **printed p. 388**, on that machine's Zorro II mapping:

> The I/O address space is mapped at $00E80000-$00EFFFFF, and a new 1.5 megabyte section (previously reserved for motherboard devices) from $00A00000-$00B7FFFF.

Appendix D's *A3000 Memory Map*, printed p. 315 (read from the page image; the text layer's rows are shifted against their descriptions), and the drawing on printed p. 316, give that machine's CIAs one 64K bank, NanoMig's window exactly:

| Address Range | Description |
|---|---|
| `$00A0 0000 - $00B7 FFFF` | Zorro II I/O Expansion Space |
| `$00B8 0000 - $00BE FFFF` | Reserved |
| `$00BF 0000 - $00BF FFFF` | CIA Ports & Timers |

So the manual describes two decodes, one per generation, and no page gives the narrower window to the A1000, A500 or A2000.

**Where A13 and A12 are both low** ($BFC000), p. 353's sentence says when each chip is selected and not what happens when A12 and A13 are both low. Printed **p. 351**, under *Port Signal Assignments*, which printed p. 350 introduces as the part that "specifies how various signals relate to the available ports of the 8520", marks one address line per chip with the asterisk of an active-low line:

> CIA-A Address BFEr01 data bits 7-0 (A12*) (INT2)

> CIA-B Address BFDr00 data bits 15-8 (A13*) (INT6)

CIA-A is selected while A12 is low, CIA-B while A13 is low; at $BFC100 both are, and both chips are selected. Appendix E's port table, printed p. 337, repeats both headings, with `x` where p. 351 prints `r`. p. 353's printed binary patterns fix both bits for each chip, but they are the patterns [NM-HRM-062](NM-HRM-062.md) shows interchanged, and they give "the following binary address" by which a chip "is selected", not the addresses at which it is not.

## RTL and measurement

**NanoMig compares all eight of A23–A16 with $BF, and one bit per chip below that** — [`src/minimig-aga/gary.v#L190-L192`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L190-L192):

```verilog
assign sel_cia   = cpu_address_in[23:16]==8'hBF; // $BFxxxx
assign sel_cia_a = sel_cia & ~cpu_address_in[12];
assign sel_cia_b = sel_cia & ~cpu_address_in[13];
```

Three of the eight compared bits are the page's; A20–A16 are the five it calls don't-cares. The chip selects are the two CIAs' enables ([`minimig.v#L612`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L612) `.aen(sel_cia_a),`, [`#L637`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L637) `.aen(sel_cia_b),`) and the pair's select is the 68000 bridge's peripheral-cycle input, [`minimig.v#L663`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L663) `.vpa(sel_cia),`. Line 190 reads nothing but the address, so the decode is the same in all four of NanoMig's modes, "chipset OCS-A500(0), OCS-A1000(1), ECS(2) or AGA(3)" ([`src/misc/sysctrl.v#L228`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/misc/sysctrl.v#L228)): in the first two NanoMig is a machine p. 314 names, and none of its modes is an A3000. In $A00000–$BEFFFF no other select of the module answers, apart from `sel_rtg` at $B8 ([`gary.v#L193`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L193)), whose output is left unconnected ([`minimig.v#L816`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L816) `.sel_rtg(),`).

### Measured

`hrm/appF-cia-address-decode` drives `gary`'s address input alone and reads its three CIA selects.

| address | A23–A16 | A13, A12 | `sel_cia` | `sel_cia_a` | `sel_cia_b` | graded by |
|---|---|---|---:|---:|---:|---|
| $BFE001 | $BF | 1, 0 | **1** | 1 | 0 | `CIA-DECODE-DONTCARE`, baseline; `CIA-AB-TWOBIT-RULE` |
| **$A00000** | $A0 | 0, 0 | **0** | | | `CIA-DECODE-DONTCARE`, red |
| **$BE0000** | $BE | 0, 0 | **0** | | | `CIA-DECODE-DONTCARE`, red |
| $BFEFFF | $BF | 1, 0 | | 1 | | `CIA-DECODE-DONTCARE`, control: A11–A1 set |
| $BFD000 | $BF | 0, 1 | | 0 | 1 | `CIA-AB-TWOBIT-RULE` |
| $BFC100 | $BF | 0, 0 | | **1** | **1** | `CIA-AB-TWOBIT-RULE`, as p. 351 predicts |

Both $A00000 and $BE0000 carry `101` in A23–A21; $BE0000 differs from $BF in A16 alone. The two probes differ from the baseline only in the page's don't-cares and in A13, which chooses the chip and does not enter the pair's select. Neither reaches a CIA. The control on the same signal holds: setting A11–A1, which `gary.v` does not read, leaves CIA-A selected, so the fixture answers live. (`gary.v` has no A0: its address port is `[23:1]`.)

`CIA-AB-TWOBIT-RULE` is green: p. 353's second sentence gives a condition under which each chip is selected and is silent on A12 and A13 both low, and where the manual does speak to $BFC100, on p. 351, it predicts what the two one-bit chip selects above do. So $BFC100 is not part of this finding.

## Risks and unknowns

- The manual tells software not to use the range (p. 314, p. 353), its A3000 map (p. 315) gives exactly NanoMig's $BF0000–$BFFFFF, and Copperline and WinUAE's Gayle and Fat Gary models decode narrower still. The finding rests on p. 353 for the machines p. 314 names, which NanoMig's OCS-A500 and OCS-A1000 modes are.
- The AGA mode is outside the manual. WinUAE gives its Gayle machines, the A1200 among them, $BFD000–$BFDFFF and $BFE000–$BFEFFF alone; that is a witness, not the page.
- Whether any real title reaches a CIA through a mirror at $A00000–$BEFFFF decides whether the difference matters; p. 353 says the hardware answers there and p. 314 says software must not rely on it.
- The suggested change has not been run here. It would apply in every mode, the AGA mode included; keeping $BF alone there would need a mode input `gary.v` does not have (its mode inputs are `a1k` and `ecs`).
- The TRM p. 26 quotation below is the OCR's reading, `Al2` included.

## Independent evidence

The question put to each: which address bits select the CIAs as a pair, and what does an access at $BFC100, with A13 and A12 both low, reach?

| | code | |
|---|---|---|
| **WinUAE** | [`memory.cpp:3192`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L3192), [`:3211-3222`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L3211-L3222), [`cia.cpp:2602-2647`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2602-L2647), [`:2677-2685`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2677-L2685), [`:2859-2868`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2859-L2868), [`cfgfile.cpp:9883-9897`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9883-L9897) | **splits by gate array.** The CIA handler is mapped over $A00000–$BFFFFF, `map_banks (&cia_bank, 0xA0, 32, 0);`, and "nothing" is mapped up to $A00000 or, on a Gayle or Fat Gary machine, up to $BF0000: `bnk_end = currprefs.cs_cd32cd ? 0xBE : (gayleorfatgary ? 0xBF : 0xA0);`, with `gayleorfatgary = ((currprefs.chipset_mask & CSMASK_AGA) \|\| currprefs.cs_pcmcia \|\| currprefs.cs_ide > 0 \|\| currprefs.cs_mbdmac) && !currprefs.cs_cd32cd;`. So a model with neither — the A500, A1000 and A2000 among them — reaches the CIAs anywhere in $A00000–$BFFFFF, as p. 353 says. The chip is chosen on A13:A12, `int cs = (addr >> 12) & 3;`, and a comment beside the selection logic gives the rule for both kinds of machine: `// Gayle or Fat Gary does not enable CIA /CS lines if both CIAs are selected`, `// Old Gary based Amigas enable both CIAs in this situation`. On the older machines a read at A13:A12 = 00 takes CIA-A or CIA-B by A0, `v = (addr & 1) ? ReadCIAA(r, &flags) : ReadCIAB(r, &flags);`, and a write reaches both, `if ((cs & 2) == 0)` `WriteCIAB(r, value, &flags);` `if ((cs & 1) == 0)` `WriteCIAA(r, value, &flags);`. With Gayle or Fat Gary only addresses whose A15–A12 read $D or $E reach a CIA, `return mask == 0xe000 \|\| mask == 0xd000;`, under `// gayle CIA region is only 4096 bytes at 0xbfd000 and 0xbfe000`. The two rules are set apart: the AGA chipset alone moves the region to $BF, while the $D/$E rule needs Gayle or Fat Gary, `return currprefs.cs_ide \|\| currprefs.cs_pcmcia;`, so a configuration with AGA and neither — the generic preset's `// very A500-like` branch with AGA chosen — has NanoMig's combination, a machine the manual does not describe |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:878-882`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L878-L882), [`:1470-1471`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1470-L1471), [`:2242-2245`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2242-L2245), [`:2338-2346`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2338-L2346) | **follows p. 353 and p. 351 on every model.** `for (isize i = 0xA0; i <= 0xBE; i++) {` `cpuMemSrc[i] = MemSrc::CIA_MIRROR;` and `cpuMemSrc[0xBF] = MemSrc::CIA;`, with no model term, and both sources reach the same CIA access, `case MemSrc::CIA_MIRROR:    return peek8 <Accessor::CPU, MemSrc::CIA>      (addr);`. Each chip is selected on one bit, `u32 selA = (addr & 0x1000) == 0;` `u32 selB = (addr & 0x2000) == 0;`, so a write at $BFC100 reaches both, `if (selA) ciaa.poke(reg, value);` `if (selB) ciab.poke(reg, value);`, and a byte read there takes one of the two by A0, `return a0 ? ciaa.peek(reg) : ciab.peek(reg);` |
| **Denise** | [`emulation/libami/agnus/memory.cpp:706-709`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L706-L709), [`:254-258`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L254-L258), [`:352-358`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L352-L358) | **follows p. 353 and p. 351.** `for(int i = 0xa0; i <= 0xbe; i++)` `mapper[i] = MMIO_CIA; // CIA mirror or overmap for Zorro 2 IO expansion` and `mapper[0xbf] = MMIO_CIA;`. Its own comment gives the reason in hardware terms: `// Gary assert VPA but don't see A12/A13. It only sees the upper address bits and knows when in general CIA area.` A byte write at $BFC100 reaches both chips, `if ((adr & 0x1000) == 0)` `cia1.write( reg, value );` `if ((adr & 0x2000) == 0)` `cia2.write( reg, value );` — `cia1` being CIA-A |
| **Copperline** | [`src/cpu.rs:18-21`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L18-L21), [`:4154-4161`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L4154-L4161), [`:4490`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L4490), [`:4513`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L4513) | **narrower than NanoMig on both counts.** Two 4K windows, `pub const CIA_A_BASE: u32 = 0x00BF_E000;` `pub const CIA_A_SIZE: u32 = 0x0000_1000;` `pub const CIA_B_BASE: u32 = 0x00BF_D000;`, matched by range on reads, `if range_contains(CIA_A_BASE, CIA_A_SIZE, addr) {`, and on writes alike, with no model term. $A00000, $BE0000 and $BFC100 reach neither chip |
| **MiSTer** | [`rtl/gary.v:193-195`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L193-L195), [`:135-136`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L135-L136) | descendant — identical to NanoMig: `assign sel_cia   = cpu_address_in[23:16]==8'hBF; // $BFxxxx`, `assign sel_cia_a = sel_cia & ~cpu_address_in[12];`, `assign sel_cia_b = sel_cia & ~cpu_address_in[13];`. Its `gary.v` also uses part of the range p. 353 gives the CIAs, ROM mirrors at $A80000–$AFFFFF and $B00000–$B7FFFF, `wire kick_mirror_a8 = cpu_address_in[23:19] == 5'b1010_1;` and `wire kick_mirror_b0 = cpu_address_in[23:19] == 5'b1011_0;` |
| **TC64** | [`rtl/minimig/gary.v:186-192`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/gary.v#L186-L192) | descendant — compares four bits, $B00000–$BFFFFF: `assign sel_cia = cpu_address_in[23:20]==4'b1011 ? 1'b1 : 1'b0;`, one bit more than the page, so $BE0000 reaches the CIAs and $A00000 does not; the chip selects are NanoMig's, `assign sel_cia_a = sel_cia & ~cpu_address_in[12];`, `assign sel_cia_b = sel_cia & ~cpu_address_in[13];` |

### What other Commodore documents say

- **Commodore, *A500/A2000 Gary Specification*, undated**, PDF page 1 — agrees with the manual:

  > "Address lines A17-A23 are connected to the processor address bus and are used to generate the various device decodes."
  <!-- corpus: gary-spec/p001.txt -->

  > "A00000-BFFFFF BANK5"
  <!-- corpus: gary-spec/p001.txt -->

  Gary has no input below A17, so it cannot tell $BF0000 from $BE0000, which differ in A16 alone, and its decode table puts $A00000–$BFFFFF in one bank. The table does not say which output that bank drives; the pin list describes NVPA as the output that "indicates that the CPU is accessing an 6800 type peripheral (such as the 8520)".

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 228 — agrees with the manual:

  > "IF (OVR) /VPA = /AS*A23*/A22*A21 ; PERIPHERAL ACCESS ; $A00000-BFFFFF"
  <!-- corpus: a500-a2000-trm-1987/p228.txt -->

  The A2000 board's PAL decodes the 68000 peripheral cycle, the one the CIAs answer, on A23–A21 alone: p. 353's three bits.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 26 — agrees with the manual's p. 351:

  > "If your slave uses the VPA VMA protocol to be synchronous with the 68000's E clock, you must only use addresses in which Al2 and A13 are high. This is because we have synchronous ports on board which are activated by (A12* AND VMA), also (A13* AND VMA)."
  <!-- corpus: a500-a2000-trm-1987/p026.txt -->

  One address line per on-board port, as p. 351 marks them, so at A12 and A13 both low both are activated.

## Decision

**One option: compare the three bits p. 353 prints**, in place of line 190:

```verilog
assign sel_cia   = cpu_address_in[23:21]==3'b101; // $A00000-$BFFFFF, p.353
```

- An access anywhere in $A00000–$BEFFFF would reach the CIAs as a peripheral cycle, since the bridge's `vpa` is `sel_cia`. Software that keeps to p. 353's addresses sees no difference.
- In NanoMig nothing else answers in that range. Upstream, `gary.v` uses $A80000–$B7FFFF as ROM mirrors (MiSTer row), so a fix there has to leave those ranges to them.
- NanoMig would join WinUAE's models without Gayle or Fat Gary, vAmiga and Denise, and move further from Copperline.
- `CIA-DECODE-DONTCARE` is the test of the change: it expects `sel_cia`=1 at $A00000 and $BE0000. Re-run with `make -C hrm/appF-cia-address-decode test`.
