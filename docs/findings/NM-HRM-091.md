# NM-HRM-091 — with BLTPRI set, the 68000 still gets the chip-bus cycles a blit does not use, where p. 278 prevents it from stealing any bus cycles while blitter DMA is running [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should Agnus, with DMACON bit 10 (BLTPRI) set and a blit running, keep the 68000 off the chip bus in the cycles the blit does not use — the `-` between the destination writes of a D-only blit, `D0 - D1 - D2` in Table 6-2 on p. 188, and, in an A-to-D blit, the `-` after the first source read and the `-` before the last destination write, `A0 - A1 D0 A2 D1 - D2` in that table's row for USE code 9 — as p. 278's "preventing micro from stealing any bus cycles while blitter DMA is running" says, where NanoMig grants the 68000 those cycles? The question stops at the unused cycles: in a blit of USE code E, which leaves none, NanoMig keeps the 68000 out (`P7-ON-E`, green). Closes when Agnus withholds the unused cycles from the 68000 while BLTPRI is set and `P7-ON` and `P7` of `integration/ch5-audio-registers-dma` and `A4` of `integration/ch6-blitter-cpu-sharing` are green on both cores, or when an Amiga specialist rejects this reading of pp. 278 and 198, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | ECS Agnus, PAL, 2 MB Chip memory, both CPU cores (fx68k, TG68K) in 68000 mode, the CPU looping in Chip memory: a blit of USE code 1 (D only, BLTCON0 `$01FF`, BLTSIZE `$2020`, 4096 words) with BLTPRI set, counted from its first to its last destination write; and a blit of USE code 9 (A to D, BLTCON0 `$09F0`, BLTSIZE `$7D01`, 500 words) with BLTPRI set, the CPU in the program's code after the BLTSIZE write, counted from its first source read to its last destination write (`A4`); OCS is not run; chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 6 — Blitter, printed pp. 186, 188 (Table 6-2), 189, 198; 7 — System Control, printed p. 222 (Table 7-6); Appendix A, printed p. 278 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims by assertion: `P7`: `5-appA-p278.c15`, `A-dmacon.b10`, p. 278. `P7-ON`, `P7-OFF`, `P7-ON-E` and `P7-OFF-E` carry no claim id; `A4` of `integration/ch6-blitter-cpu-sharing` (p. 198) carries none either |
| **Module** | `agnus.v`, `agnus_blitter.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L429-L433) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L402-L406); inherited (MiSTer has the same code; TC64 the same, its counter also counting in turbo mode) |
| **Test** | [`integration/ch5-audio-registers-dma`](../../integration/ch5-audio-registers-dma): `make -k -C integration/ch5-audio-registers-dma test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. It carries the finding. [`integration/ch6-blitter-cpu-sharing`](../../integration/ch6-blitter-cpu-sharing): `make -k -C integration/ch6-blitter-cpu-sharing test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. It carries `A4` |
| **Assertions** | In `integration/ch5-audio-registers-dma`, `P7-ON` and `P7` are red on both cores, the package's only reds (87 passed, 2 failed each): expected zero CPU accesses between the first and last destination write of the D-only blit with BLTPRI set; observed 3429 on fx68k and 4095 on TG68K. With BLTPRI clear the same blit measures 3434 and 4095 (`P7-OFF`, green); a blit of USE code E, which leaves no cycle unused, measures 0 on both cores with BLTPRI set (`P7-ON-E`, green) and 2975 and 3106 with it clear (`P7-OFF-E`, green). In `integration/ch6-blitter-cpu-sharing`, `A4` is red on both cores (37 passed, 3 failed each; `A8` and `PARITY`, the other reds, are filed on [NM-HRM-069](NM-HRM-069.md)): expected no CPU Chip-memory access from the first source read to the last destination write of an A-to-D blit with BLTPRI set; observed 2 on each core in a window of 8136 `clk_sys`, a fetch of `$8044A` at +12 (before the first destination write, which is at +24) and a fetch of `$8044C` at +8132, no counter write; `A4PRE` and `A4-CTRL` are green |
| **Verdict of the four** | **unanimous with NanoMig, against p. 278's *any***: WinUAE in its cycle-exact mode, vAmiga, Denise and Copperline give a waiting 68000 the cycle a blit does not use, the `-` of `D0 - D1 - D2`, whatever BLTPRI holds; WinUAE and Copperline say so in their comments. Copperline alone keeps the 68000 out of a BLTPRI blit's start-up, before its first destination write. Hardware: not measured |
| **Effort** | S — one Agnus output, high while BLTPRI is set and the blitter has not yet written its last word (`blit_busy` is already low in the empty cycle before that write; see *Decision*), wired through `minimig.v` into the 68000 bridge's acknowledge (`minimig_m68k_bridge.v` line 131) beside `dbr`; not run here |
| **See also** | [NM-HRM-069](NM-HRM-069.md) |

---

## Source requirement

**Appendix A, printed p. 278**, DMACON:

> 10 BLTPRI Blitter DMA priority (over CPU micro) (also called "blitter nasty") (disables /BLS pin, preventing micro from stealing any bus cycles while blitter DMA is running).

**Printed p. 198**, on the blitter and the 68000:

> Depending on the setting of the blitter DMA mode bit, commonly referred to as the "blitter-nasty" bit, the processor may be blocked from bus access. This bit is called DMAF_BLITHOG and is in register DMACON.

> If DMAF_BLITHOG is a 1, the blitter will keep the bus for every available Chip memory cycle. This could potentially be every cycle (ROM and Fast memory are not typically Chip memory cycles).

**Chapter 7, printed p. 222**, Table 7-6, bit 10:

> Blitter priority. Also known as "blitter-nasty." When this is a 1, the blitter has full (instead of partial) priority over the 680x0.

Table 6-2 on p. 188, *read from the table, not quoted*, gives each USE code
its cycle sequence: USE code 1, D alone, `D0 - D1 - D2`, a cycle without a
blitter access between every two destination writes; USE code E, A, B and C,
`A0 B0 C0 A1 B1 C1 A2 B2 C2`, a blitter access in every cycle.

### What the pages say about a cycle the blitter does not use

**Printed p. 189**, below Table 6-2:

> This Table is only meant to be an illustration of the typical order of blitter cycles on the bus. Bus cycles are dynamically allocated based on blitter operating mode; competing bus activity from processor, bitplanes, and other DMA channels; and other factors.

**Printed p. 186**, on starting a blit:

> When the BLTSIZE register is written the blit is started. The processor does not stop while the blitter is working, though; they can both work concurrently, and this provides much of the speed evident in the Amiga.

Neither sentence names BLTPRI or DMAF_BLITHOG. None of pp. 186 to 198 says
who has a cycle the blitter does not use while DMAF_BLITHOG is 1.

## RTL and measurement

**NanoMig's blitter asks for the bus only in the cycles it accesses memory,
and a cycle no DMA channel asks for goes to the 68000, whatever BLTPRI
holds.** In a blit with channel A off, the state `BLT_A` is an empty cycle,
`dma_req = usea; // empty cycle if channel A is not enabled`
([`src/minimig-aga/agnus_blitter.v#L668-L689`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_blitter.v#L668-L689)),
and a D-only blit alternates it with `BLT_D`, which asks for the bus when a
destination word is waiting, `dma_req = used & pipeline_full;`
([`#L735-L757`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_blitter.v#L735-L757)).
With no request, Agnus's priority chain falls through to its last branch,
"bus not allocated by agnus", `dbr = 0;`
([`src/minimig-aga/agnus.v#L243-L266`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L243-L266)),
and the 68000 bridge acknowledges the CPU,
`if (!l_as && cck && ((!vpa && !(dbr && dbs)) || (vpa && vma && eclk[8])) && !nrdy) _ta_n <= 0;`
([`src/minimig-aga/minimig_m68k_bridge.v#L131`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig_m68k_bridge.v#L131)).

**BLTPRI reaches one line.** `wire bltpri = dmacon[10];`
([`agnus.v#L298`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L298))
is read only by the blitter slowdown counter, `if (!bls || bltpri) bls_cnt <= 2'b00;`
([`#L429-L433`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L429-L433)),
which decides whether a cycle the blitter does request goes to it. The
module's header comment, *"Blitter nasty mode activates the buspri signal to
indicate to gary to stop access to the chipram/chipregisters."*
([`#L48-L50`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L48-L50)),
is the only place `buspri` occurs in NanoMig's sources.

### Measured

[`integration/ch5-audio-registers-dma`](../../integration/ch5-audio-registers-dma)
writes DMACON = `$8640` (SET, BLTPRI, DMAEN, BLTEN), starts a D-only blit of
4096 words, and runs `MOVE.W D1,(A4)+`, `ADDQ.W #1,D1`, `DBRA D2,L` from Chip
memory. At the RAM port it counts the CPU's accesses, program fetches and
writes to its counter block, strictly between the blit's first and last
destination write (`test_ch5_audio_registers_dma.cpp:1120-1124`, `:1140-1166`).
The contrasts clear BLTPRI (`$8240`) or run a blit of USE code E (BLTCON0
`$0EFF`), read between its first and last source read. On both cores the
program finished, DMACONR read back as written, every word of the blit's
blocks was accessed, and a CPU write followed the blit's end (`P7-G1`, `P7-G2`,
`P7-G3`, `P7-G(CPU-still-wanted-the-bus)`, green).

| run | CPU accesses inside the blit, fx68k | TG68K | graded by |
|---|---:|---:|---|
| D only, BLTPRI set | **3429** | **4095** | `P7-ON`, `P7`, red |
| D only, BLTPRI clear | 3434 | 4095 | `P7-OFF`, green |
| USE code E, BLTPRI set | 0 | 0 | `P7-ON-E`, green |
| USE code E, BLTPRI clear | 2975 | 3106 | `P7-OFF-E`, green |

On fx68k:

```text
MEASURED: P7 D-only blit, BLTPRI set: T_first=2274801 T_last=2341505 CPU accesses in between=3429
MEASURED: P7 D-only blit, BLTPRI clear: T_first=2274801 T_last=2341601 CPU accesses in between=3434
FAIL: P7-ON [p.278, p.198]: with BLTPRI set, zero CPU accesses in (T_first,T_last), the first and last destination write of a blit that uses D only (USE code 1, p.188: D0 - D1 - D2)
```

On TG68K both D-only runs print `T_first=2274721 T_last=2341425` and 4095
accesses: BLTPRI changes neither the count nor the blit's end there. `P7-ON`
grades the zero count (`test_ch5_audio_registers_dma.cpp:1278-1282`);
`P7` joins it to `P7-OFF` (`:1287-1292`).

### The unused cycles of an A-to-D blit

**The two `-` of `A0 - A1 D0 A2 D1 - D2` in row 9 of Table 6-2 are cycles NanoMig's blitter does
not request, and the 68000 gets both.** An A-to-D blit asks for the bus in
`BLT_A` with `dma_req = usea;` and in `BLT_D` with `dma_req = used & pipeline_full;`.
`pipeline_full` is cleared in `BLT_INIT` and set at the end of the first
`BLT_D` ([`agnus_blitter.v#L564-L571`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_blitter.v#L564-L571),
[`#L889-L892`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_blitter.v#L889-L892)),
so the first `BLT_D`, between the first `BLT_A` and the second, requests
nothing: that is the `-` after `A0`. Every later cycle up to the last word is
requested. After the last `BLT_D` the state machine enters `BLT_E`, `dma_req = 1'b0;`
([`#L759-L775`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_blitter.v#L759-L775)),
the `-` before `D2`, and then `BLT_F`, which requests the last destination write
([`#L777-L794`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_blitter.v#L777-L794)).
In both cycles no DMA channel requests, and the priority chain falls
through to `dbr = 0;` as above, whatever BLTPRI holds.

[`integration/ch6-blitter-cpu-sharing`](../../integration/ch6-blitter-cpu-sharing)
writes DMACON = `$8640` (SET, BLTPRI, DMAEN, BLTEN), BLTCON0 `$09F0` (A and D)
and BLTSIZE `$7D01` (`prog_sharing.s:56-72`), then runs `moveq #0,d0` and
`lea SHARE_SCRATCH,a1` (`:74-75`) and jumps to the counting loop (`:89`). `A4`
counts the CPU's program fetches and its writes to the counter block from the
blit's first source read, a read of the A channel's words, which only the blitter
reads, to its last destination write (`test_blitter_cpu_sharing.cpp:993-1033`,
counting in `cpu_hits_in`, `:173-183`). `A4PRE` requires a source read before
the first destination write and a run of more than 2000 `clk_sys`. The program
reads no register, and the capture is its own run with its own blit. The run is 8136
`clk_sys` on both cores, its first destination write is at +24, and it holds
two CPU fetches, both in the `lea`: `$8044A`, its first address word, at +12,
and `$8044C`, its last word, at +8132, 4 `clk_sys` before the last destination
write. A slot is 8 `clk_sys` (`SLOT-UNIT`): the fetch at +12 falls in the slot
after `A0`'s and the fetch at +8132 in the slot of `BLT_E`.

```text
MEASURED: A4: blit DMA run 2274353..2282489 (first source read to last destination write; first destination write at +24), 2 CPU Chip-memory access(es) inside it, 1 of them before the first destination write: fetch $8044A at +12 clk_sys, fetch $8044C at +8132 clk_sys (offsets from the run's first source read)
FAIL: A4 [p198]: with DMAF_BLITHOG (BLTPRI) set the blitter keeps the bus for every available Chip memory cycle -- the CPU makes no Chip-memory access at all (program fetch or counter write) from the blit's first source read to its last destination write (2 accesses, 0 of them counter writes)
```

TG68K prints the run `2274313..2282449` and the same two fetches at +12 and +8132.

`A4-CTRL` runs `A4`'s comparator over the 768 fetch windows of a
four-high-resolution-plane display (`test_blitter_cpu_sharing.cpp:1035-1077`),
each holding 160 bitplane fetches and no free slot, with the CPU making Chip-memory
accesses outside them (41771 on fx68k, 45984 on TG68K; `A4-CTRL-PRE`, green).
It counts 0 CPU accesses in them on both cores (`A4-CTRL`, green).

## Risks and unknowns

- **What weakens it: nobody implements the page.** All four implementations
  and both Minimig lines give the 68000 the cycles a blit leaves unused
  between its destination writes, with BLTPRI set as with it clear. The
  finding rests on pp. 278 and 198, and no hardware was measured.
- **Conflicting readings.** p. 198's *"every available Chip memory cycle"*
  can be read as the cycles the blitter has use for; p. 189 calls Table 6-2,
  whose `-` is the unused cycle, an illustration of an order that competing
  processor activity changes; p. 186 lets the processor work while a blit
  runs. p. 278's *"any bus cycles while blitter DMA is running"* and p. 222's
  *"full (instead of partial) priority"* carry no such limit.
- **Copperline is not independent here.** Its rule for the unused cycle cites
  the HRM and Minimig's `dma_req`, and its narrowing of the BLTPRI fence cites
  two programs and FS-UAE's and vAmiga's figures for one of its timing rows.
- **`A4` is placed by the state sequence.** The test prints the fetches' offsets, not the blitter's state; that the fetch at +12 lies in the first `BLT_D` and the fetch at +8132 in `BLT_E` is read from the RTL and the slot length. `A4` cites p. 198 alone, the sentence the conflicting reading above applies to.
- **Not graded.** The /BLS pin p. 278 names is inside the chips; the start-up
  of the D-only blit of `P7`, before its first destination write, lies outside
  `P7`'s interval; OCS is not run.
- **The suggested change has not been run.**

## Independent evidence

### What the others say

The question put to each: with BLTPRI set, who has the cycle a running blit
does not use, and where is BLTPRI read?

| | code | |
|---|---|---|
| **WinUAE** | [`blitter.cpp:144-153`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L144-L153), [`:1264-1272`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L1264-L1272), [`:1897-1907`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L1897-L1907), [`custom.cpp:12151-12155`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L12151-L12155), [`blitter.cpp:1755-1756`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L1755-L1756), [`newcpu.cpp:4797`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L4797) | **gives the 68000 the cycle a blit does not use, whatever BLTPRI holds.** In a blit without A, B and C, a cycle in which D is not written selects no channel, `return 0;`, its register stays `0x1fe`, and the slot is marked free, `if (reg == 0x1fe) {` / `idlecycle = true;` and `rga->alloc = -1;`. A cycle-exact CPU access waits only while the slot is allocated, `if (r->alloc <= 0) {` / `break;`. The source's own comment: *"Blitter idle cycle is not "used" by blitter, CPU can still use it normally if it needs the bus."* BLTPRI enters the cycle-exact blitter only in its count of missed cycles, `bool bstreq = blt_info.nasty_cnt >= BLIT_NASTY_CPU_STEAL_CYCLE_COUNT && !pri;`; for a 68000 the nasty flag's CPU delay runs only outside cycle-exact blitting, `((currprefs.waiting_blits && currprefs.cpu_model >= 68020) \|\| !blitter_cycle_exact)` |
| **vAmiga** | [`Core/Components/Agnus/Blitter/SlowBlitter.cpp:27-29`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Blitter/SlowBlitter.cpp#L27-L29), [`:163-168`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Blitter/SlowBlitter.cpp#L163-L168), [`:1101-1105`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Blitter/SlowBlitter.cpp#L1101-L1105), [`Core/Components/Agnus/Agnus.cpp:497-519`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L497-L519), [`Core/Components/Agnus/AgnusDma.cpp:44-53`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L44-L53), [`Core/Infrastructure/Defaults.cpp:177`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Defaults.cpp#L177), [`Core/Components/Agnus/Blitter/Blitter.cpp:584`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Blitter/Blitter.cpp#L584) | **leaves the unused cycle to the 68000, whatever BLTPRI holds.** The D-only micro-program, `// 1:  -- D0 -- D1 \| -- D2`, runs `&Blitter::exec <HOLD_D \| BUSIDLE>,` and then `&Blitter::exec <WRITE_D \| HOLD_A \| REPEAT>,`. A `BUSIDLE` cycle *"Waits for the bus to be free"*, where `BUS` *"Waits for the bus to be free and allocates it"*: `if (busidle && !agnus.busIsFree<BusOwner::BLITTER>()) return;` takes nothing. The CPU waits only `while (busOwner[pos.h] != BusOwner::NONE);` and then takes the cycle, `busOwner[pos.h] = BusOwner::CPU;`. BLTPRI is read where a raised BLS refuses the blitter, `if (bls && !bltpri()) return false;`. The slow blitter is the default, `registerDefault(Opt::BLITTER_ACCURACY,           2);` and `case 2: beginSlowCopyBlit(); break;` |
| **Denise** | [`emulation/libami/agnus/blitterSequencer.cpp:100-101`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blitterSequencer.cpp#L100-L101), [`:128-129`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blitterSequencer.cpp#L128-L129), [`emulation/libami/agnus/blitterCycle.cpp:48-51`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/blitterCycle.cpp#L48-L51), [`emulation/libami/agnus/dma.cpp:495-501`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L495-L501), [`:442-453`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L442-L453), [`emulation/libami/agnus/agnus.cpp:380-398`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L380-L398) | **leaves the unused cycle to the 68000, whatever BLTPRI holds.** Each word of a D-only blit runs `blockMode<BLT_NONE, 2>(); break;` and then writes D, `blockMode<BLT_SHIFT_A \| BLT_WRITE_D \| BLT_NEXT>(); break;`. The first needs a free bus and takes nothing, `if (!agnus.canBlitterUseBusExt())` / `return;`, where a write takes the bus, `busUsage = BUS_USAGE_BLITTER_D;`. The CPU waits only `while (busUsage != BUS_FREE) {` and then takes the cycle, `busUsage = BUS_USAGE_CPU;`. BLTPRI is read in the blitter's count of the CPU's wait cycles, `if (!blitterNasty() && (countWaitCycles > 2))` |
| **Copperline** | [`src/chipset/blitter.rs:1761-1766`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/blitter.rs#L1761-L1766), [`src/bus/dma_slots.rs:782-814`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L782-L814), [`:723-728`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L723-L728), [`src/chipset/blitter.rs:1830-1854`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/blitter.rs#L1830-L1854), [`src/bus/dma_slots.rs:714-716`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L714-L716) | **leaves the unused cycle to the 68000 once the blit is under way, whatever BLTPRI holds.** The A phase of a blit without A needs no bus, `NormalBlitPhase::A => self.use_a,`, and such a cycle goes to nobody, `if !self.blitter.current_slot_needs_bus() {` / `return ChipBusOwner::Idle;`, which the CPU may use, `ChipBusOwner::Cpu \| ChipBusOwner::Idle`. Its comment: *"Idle blit pipeline cycles (the "-" slots in the HRM cycle diagrams, ...) never claim the bus: per the HRM they are available to the other DMA channels or the 68000, and MiniMig only asserts the blitter's dma_req on channel-access states."* With BLTPRI set it keeps the 68000 out of the start-up only, the ladder and the body cycles *"until the first D slot has primed the hold register"*, `\| NormalBlitPhase::FillIdle => self.use_d && !self.pipeline_full,`; after that, its comment says, *"the CPU uses them even under BLTPRI -- line-heavy demo loops (Rampage's vector parts) rely on that CPU time, and FS-UAE and vAmiga agree"*. Its slowdown counter runs only with BLTPRI clear, `self.blitter.busy && self.blitter_dma_enabled() && self.agnus.dmacon & DMACON_BLTPRI == 0` |
| **MiSTer** | [`rtl/agnus_blitter.v:708`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L708), [`rtl/agnus.v:214-217`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L214-L217), [`:251`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L251), [`:402-406`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L402-L406), [`:48`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L48) | descendant — identical to NanoMig: the same empty cycle, `dma_req = usea; // empty cycle if channel A is not enabled`, the same fall-through to the CPU, `dbr = 0;`, BLTPRI read only by the counter, `if (!bls \|\| bltpri) bls_cnt <= 2'b00;`, and the same header comment naming `buspri` |
| **TC64** | [`rtl/minimig/agnus_blitter.v:678`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter.v#L678), [`rtl/minimig/agnus.v:265-268`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L265-L268), [`:304`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L304), [`:431-437`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L431-L437), [Changelog line 188](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/Changelog#L188), [line 370](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/Changelog#L370) | descendant — the same empty cycle, `dma_req = usea; // empty cycle if channel A is not enabled`, the same fall-through, `dbr = 0;`, and BLTPRI read only by the counter, `if (!bls \|\| bltpri)`, which also counts in turbo mode, `if (!cck \|\| turbo)`. Its changelog records `removed signal 'buspri'` and, for Agnus, `changed blitter nasty mode altogether, it is now not according to the HRM, but at least this solution seems to work for most games/demos`. Nothing in either line keeps the 68000 out of a cycle the blitter does not use |

**All four give the 68000 the cycles a blit leaves unused between its
destination writes, with BLTPRI set as with it clear.** WinUAE and Copperline
say why in their own comments: the blitter's idle cycle is *"not "used" by
blitter"*, and the `-` slots are *"available to the other DMA channels or the
68000"*. Copperline alone keeps the 68000 out of a BLTPRI blit's start-up,
before its first destination write, which `P7-ON` does not grade and `A4` does (the fetch at +12). On this
point the four side with NanoMig and against p. 278's *any*. MiSTer and TC64,
descendants, carry NanoMig's code, so the finding is inherited and fixed in
neither line.

### What practitioners report

Reports from the forum thread [EAB 19676](https://eab.abime.net/showthread.php?t=19676): witnesses, never an oracle ([practitioner reports](../evidence/practitioner-reports.md)).

**EXPLAINS, inferred.** [EAB post 579567](https://eab.abime.net/showpost.php?p=579567), Toni Wilen, 2009-08-02. Chip set: not stated. Known from: not stated.

> "All blitter cycles require free bus cycle (=not used by any other Agnus DMA channel), including blitter idle cycles."

Idle cycles are arbitrated like the others, so a cycle the blit does not use is free for the CPU unless something claims it. The post names no BLTPRI, which is the page's condition, so it does not say whether the 68000 gets those cycles with BLTPRI set.

## Decision

**One option: build the signal `agnus.v`'s header comment describes.** An
Agnus output, high while BLTPRI is set and the blitter has not written its last word
(`bltpri` is in `agnus.v`; `blit_busy` is cleared by `done` in the last
`BLT_D` ([`agnus_blitter.v#L905`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_blitter.v#L905)), before `BLT_E` and `BLT_F`, so the
output is taken from the blitter's state, not from `blit_busy`), wired through `minimig.v` to the 68000 bridge
and joined there to `dbr`, so that `_ta_n` is not asserted for a chip-bus
access (`dbs`) while it is high. What it would change:

- the 68000 would wait from the start of the blit to its end, the change
  aimed at turning `P7-ON`, `P7` and `A4` green; `P7-OFF` and `P7-OFF-E` run with
  BLTPRI clear, where the output stays low, and `P7-ON-E` already counts 0;
- `dbs` also selects the custom registers and slow memory, so a 68000 polling
  DMACONR during a BLTPRI blit would wait for the blit's end too;
- NanoMig would move away from all four implementations and both Minimig
  lines; Copperline's comments name two programs that bear on it, not run
  here: line-drawing loops in Rampage's vector parts, which it says rely on the
  CPU time of a BLTPRI line blit's bus-free cycles, and Jim Power's
  trackloader, which it says relies on the 68000 being kept out of a BLTPRI
  blit's start-up.
