# NM-HRM-069 — the 68000 is given chip-bus slots of both parities, where p. 196 allows it only the even ones [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should the 68000 bridge or Agnus acknowledge a CPU chip-bus access only in colour clocks of the even parity, as p. 196's "only the even-numbered memory access cycles" says, where NanoMig acknowledges it in any colour clock no DMA channel claims? The question stops at p. 196's parity; which count p. 198 means is outside it. Closes when the change is made and `PARITY` and `A8` are green on both cores, or when an Amiga specialist rejects p. 196's reading, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | the 68000's chip-bus accesses on both CPU cores (fx68k, TG68K), measured with a counting loop with no DMA but refresh (red on fx68k only) and beside a share-mode A-to-D blit (red on both), and, beside that blit, the 68000's grants measured against the first free even-numbered slot (`A8`, red on both); chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 6 — Blitter, printed pp. 194–198 (Figure 6-9 on p. 195, Figures 6-10 and 6-11 on pp. 196 and 197); 2 — Coprocessor, printed p. 20; Appendix A, printed p. 275 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims by assertion: `PARITY`: `6-system-dma.c50`, `ch6-sysdma.c25`, `6-system-dma.c60`, p. 196; `A8`, no claim id: p. 196, p. 198; `A8-BOUNDARY`, recorded and not graded: `6-system-dma.c54`, `6-system-dma.c55`, `ch6-sysdma.c30`, p. 196; `SYSDMA-C29-BOUNDARY`, recorded and not graded: `ch6-sysdma.c29`, p. 196; `A5-WITHDRAWN`: `6-blitter-priority.c13`, `ch6-sysdma.c62`, p. 198; `A2`, green and not evidence here: `ch6-sysdma.c53`, p. 198. `PARITY-REF` (Figure 6-9) carries no claim id |
| **Module** | `minimig_m68k_bridge.v`, `agnus.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig_m68k_bridge.v#L131) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig_m68k_bridge.v#L151); inherited (MiSTer has the same code; TC64 the same acknowledge, its counter also counting in turbo mode) |
| **Test** | [`integration/ch6-blitter-cpu-sharing`](../../integration/ch6-blitter-cpu-sharing): `make -k -C integration/ch6-blitter-cpu-sharing test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. It carries the finding |
| **Assertions** | `PARITY` and `A8` are red on both cores (37 passed, 3 failed each; the third red, `A4`, is filed on [NM-HRM-091](NM-HRM-091.md)). `PARITY`: expected no CPU chip-bus access on an odd slot, both with no DMA and inside a share-mode blit's active window; observed 20006 of 41007 and 125 of 243 on odd slots on fx68k, 0 of 45209 and 124 of 254 on TG68K, so on fx68k both halves are red and on TG68K the share-mode half alone. `A8`: expected every 68000 access whose ready even-numbered slot a DMA access took to be granted exactly at the first free even-numbered slot, neither before it (an odd-numbered grant, the event `PARITY` counts) nor after it; observed 122 granted before it of 124 missed even cycles on fx68k and 120 of 125 on TG68K, none after a free even-numbered slot was left unused. `PARITY-REF` and `A8PRE` pass on both cores |
| **Verdict of the four** | **unanimous with NanoMig, against p. 196.** WinUAE, vAmiga, Denise and Copperline all make a CPU access wait only while DMA owns the colour clock, and none of them consults the colour clock's parity; WinUAE, vAmiga and Denise charge BRA's two internal clocks as one colour clock, as fx68k does. All four let a non-nasty blitter stand aside after a count of missed cycles, not at a parity, and Copperline's own test grants the CPU colour clock $27 there. Hardware: not measured |
| **Effort** | S — one parity term in the last branch of `agnus.v`'s priority chain (lines 255–266) or one new input to the bridge's acknowledge; not run here |

---

## Source requirement

**Printed p. 196**, on the 68000's share of the bus:

> The 68000 uses only the even-numbered memory access cycles. The 68000
> spends about half of a complete processor instruction time doing internal
> operations and the other half accessing memory. Therefore, the allocation
> of alternate memory cycles to the 68000 makes it appear to the 68000 that
> it has the memory all of the time, and it will run at full speed.

> Some 68000 instructions do not match perfectly with the allocation of even
> cycles and cause cycles to be missed. If cycles are missed, the 68000 must
> wait until its next available memory slot before continuing. However, most
> instructions do not cause cycles to be missed, so the 68000 runs at full
> speed most of the time if there is no blitter DMA interference.

**Figure 6-10 on the same page**, *read from the drawing, not quoted*, splits
an average 68000 cycle into two halves and labels them in the figure: the
memory access portion *"even cycle, available to the 68000"*, the internal
operation portion *"odd cycle, assigned to other devices"*. So *available*, the
word p. 196 uses for the slot a 68000 waits for, is the word the figure uses
for the even cycle.

**Printed p. 194** says what a slot is:

> During a horizontal scan line (about 63 microseconds), there are 227.5
> "color clocks", or memory access cycles. A memory cycle is approximately
> 280 ns in duration.

Figure 6-9 on p. 195, *read from the drawing, not quoted*, numbers the slots
of one line from $0 and draws refresh, disk, audio and sprite DMA on the odd
slots, the last refresh slot at $05, with the slots between them marked by
its legend as *"Slots available for Blitter, Copper and 68000"*. In the
display region it puts plane 1 of a four-plane lores display at $3F, an odd
slot, and Figure 6-11 on p. 197 puts planes 1 to 4 on the odd slots of each
eight-slot group and planes 5 and 6 on two of the even ones.

**Printed p. 198**, on sharing with the blitter when the nasty bit is clear:

> If DMAF_BLITHOG is a 0, the DMA manager will monitor the 68000 cycle
> requests. If the 68000 is unsatisfied for three consecutive memory cycles,
> the blitter will release the bus for one cycle.

The sentence counts cycles and does not say on which parity the released
cycle falls.

**Printed p. 194**, the one sentence that could give the 68000 an odd slot:

> If a device does not request one of its allocated time slots, the slot is
> open for other uses.

## RTL and measurement

**NanoMig completes a CPU chip-bus access in any colour clock that no DMA channel
claims, of either parity.** The 68000 bridge acknowledges the CPU on the
colour-clock phase whenever Agnus is not requesting the bus —
[`src/minimig-aga/minimig_m68k_bridge.v#L131`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig_m68k_bridge.v#L131):

```verilog
if (!l_as && cck && ((!vpa && !(dbr && dbs)) || (vpa && vma && eclk[8])) && !nrdy) _ta_n <= 0;
```

`cck` is high once in every colour clock, not every other one
([`src/minimig-aga/amiga_clk.v#L34-L35`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/amiga_clk.v#L34-L35)),
and `dbr` is low exactly when Agnus's priority chain falls through to its last
branch, "bus not allocated by agnus"
([`src/minimig-aga/agnus.v#L255-L258`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L255-L258)).
Neither module reads the colour clock's parity on the CPU's behalf, so a
68000 bus cycle that starts one colour clock late — after an internal
operation of two clocks, for instance — is served in the next free colour
clock, which is the other parity.

**With the nasty bit clear, the blitter stands aside by a count.** The bridge
raises `bls` while the CPU waits
([`#L145`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig_m68k_bridge.v#L145)),
Agnus counts the colour clocks it stays raised
([`src/minimig-aga/agnus.v#L429-L433`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L429-L433))
up to `parameter BLS_CNT_MAX = 3;`
([`#L109`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L109)),
and refuses the blitter the bus once the count is reached
([`#L243`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L243)).
The released colour clock is wherever the count ends, of either parity.

### Measured

[`integration/ch6-blitter-cpu-sharing`](../../integration/ch6-blitter-cpu-sharing)
runs a counting loop — `addq.w #1,d0`, `move.w d0,(a1)`, `bra.s` — once with no
DMA but memory refresh (`prog_sharing.s`) and once beside a share-mode A-to-D blit
(`prog_sharing_share.s`), on fx68k and on TG68K, and logs every access at the
chip RAM port. It numbers the slots of each line from that line's own refresh
group, its last refresh read being Figure 6-9's $05, and checks the numbering
against the figure: in a four-plane lores build every one of the 15360 plane-1
fetches lands on an odd slot (`PARITY-REF`, green on both cores), and the first
of each line's fetches is at $3F in all 768 lines, where Figure 6-9 draws
plane 1.

| run | what is counted | fx68k | TG68K | graded by |
|---|---|---:|---:|---|
| no DMA | CPU accesses on an odd slot | **20006 of 41007** | 0 of 45209 | `PARITY` |
| share-mode blit, from its first to its last destination write | CPU accesses on an odd slot | **125 of 243** | **124 of 254** | `PARITY` |
| share-mode blit, missed even cycles (a DMA access took the slot the 68000 was ready for) | granted before the first free even slot | **122 of 124** | **120 of 125** | `A8` |

`PARITY` grades both halves in one line, so it is red on both cores; `A8` grades the share-mode run alone. A mutation build, not run by `make test` ([how to run it](../../integration/README.md#mutation-builds); `NMH_MUTATE=PARITY`), feeds the unchanged comparator the CPU accesses inside the four-plane lores fetch windows, where bitplane DMA holds the odd slots, and reads green: 0 odd of 50688 accesses on fx68k and 0 of 61440 on TG68K. `NMH_MUTATE=A8`, also a mutation build, not run by `make test`, feeds `A8`'s unchanged comparator the six-lores-plane display against the whole-4-clock-aligned counting loop and reads green on both cores; the test header records that this input cannot show a grant before the first free even-numbered slot, so it shows the comparator live and does not reach the share-mode release (`test_blitter_cpu_sharing.cpp:31`).

- **No DMA, fx68k.** The loop's `bra.s` has an internal operation of two clock
  periods, which WinUAE, vAmiga and Denise spend as one colour clock; the next
  fetch then lands on the other parity, and nothing in the bridge or Agnus makes
  it wait for the even one.
- **Share-mode blit, both cores.** A 5-slot spacing between consecutive CPU
  accesses alternates the parity, and the first missed pair on both cores is
  5 slots apart (see *The share-mode window and p. 198* below).

## Risks and unknowns

- **What weakens it: nobody implements the page.** All four implementations and
  both Minimig lines let the CPU take any colour clock DMA leaves free. The
  finding rests on p. 196 and Figure 6-10, not on any implementation, and no
  hardware was measured: a BRA.S loop timed with no DMA on a real Amiga would
  settle which parity its fetches land on.
- **The two cores differ, and neither difference is a verdict.** TG68K's loop
  has no two-clock step, so it is green without DMA because its bus timing never
  offers the chip an odd request, not because anything waits; beside the blit it
  takes odd slots as fx68k does, and its red rests on the share-mode half alone.
- **Conflicting passages.** p. 194's open-slot sentence (quoted under *Source
  requirement*) could be read to let the 68000 in; it is about the four DMA devices the paragraph names — disk,
  audio, display and sprites — and does not name the 68000, where p. 196 does,
  and says *only*. Printed p. 20 and Appendix A's COPINS entry on printed p. 275
  label the parities the other way round: in p. 20's words the Copper "requests
  the bus only during odd-numbered memory cycles", while audio, disk, refresh,
  sprites and most low resolution display DMA "use only the even-numbered memory
  cycles"; p. 275 says the same in shorter words. Neither page says which cycles
  the 68000 uses. Whichever labels are used, every page that does gives it one
  parity, the one opposite the fixed DMA, and the reds are CPU accesses on both.
- **Copperline's release count is not independent of NanoMig.** Its limit of 3
  is, by its own comment, *"Grounded in the Minimig RTL (agnus.v)"*
  ([`src/bus.rs:40-50`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L40-L50)).
- **Not graded: the missed-cycle wait.** The claims of `A8-BOUNDARY` and
  `SYSDMA-C29-BOUNDARY` are conditional on a missed cycle: p. 196 says that *if*
  an instruction misses its even slot the 68000 waits for its next available
  slot. The package records both and does not grade them. `A8-BOUNDARY` records
  that the page sets an instruction's missed cycles apart from blitter
  interference, and that `A8` instead grades the cycles a share-mode blit took
  from a ready 68000, whose release p. 198 governs. `SYSDMA-C29-BOUNDARY`
  records that on an implementation which keeps the 68000 on even-numbered slots
  an instruction's own miss is not found, and the package prints its counts as
  measurements: on the four-lores-plane BRA.S loop 11097 missed odd cycles in
  92965 access pairs on fx68k and 0 in 107423 on TG68K. The RAM port shows
  grants, not the 68000's readiness: the package infers the ready slot, of either
  parity, from the address pair's largest unimpeded gap, and counts a cycle as
  missed where a DMA access took that slot (`A8PRE`'s witness). `A8` is a separate red line without
  a claim id; for the share-mode run its measurement line shows 124 missed even
  cycles on fx68k, 122 of them granted before the first free even slot and 2
  exactly on it, and 125 on TG68K, 120 and 5; the first missed pair is a fetch at
  $80500 followed by one at $80502, 5 slots apart, on both cores. Those grants
  before the free even slot are odd-numbered grants, the behaviour `PARITY`
  grades.
- **The suggested change has not been run.**

### The share-mode window and p. 198

TG68K's red rests on the share-mode half alone, so it depends on where the
blitter hands the bus back, and p. 198 is the page that says so. p. 194 makes a
memory cycle one colour clock. What p. 198 does not fix is where the count of
three *unsatisfied* cycles starts and in which unit it runs, and `A5-WITHDRAWN`
records two questions: the count starts either at the cycle the request is
raised, which the RAM port does not show, or at the first even-numbered slot the
68000 was ready for; and it runs either in memory cycles, so that the released
slot is the fourth and an odd-numbered one, or in the 68000's own alternate
cycles, so that it is the seventh and an even-numbered one, which p. 196 would
require. The package prints the share-mode pattern as a measurement: of 124
missed even cycles on fx68k 121 were granted on the third slot after the slot
the 68000 was ready for (the fourth counting that slot), that slot and the two
after it taken by DMA, and on TG68K 120 of 125. **NanoMig's first missed pair is 5 slots apart on both cores.** The
memory-cycle reading puts the release on an odd-numbered slot, which p. 196 does
not allow; the alternate-cycle reading puts it on an even-numbered one and keeps
p. 196, and p. 198 says nothing about the released cycle's parity. The
share-mode odd grants are therefore filed here, against p. 196, on both cores —
they are the same acknowledge that ignores parity with no DMA. Which count
p. 198 means is a separate question, and the package withdraws `A5` rather than
choose.

### For the specialists

1. **Which count does p. 198 mean?** See the two questions above: memory
   cycles, which release on the fourth slot, an odd-numbered one, or the 68000's
   own alternate cycles, which release on the seventh, an even-numbered one.
   NanoMig and, by their code, the four implementations count every denied
   colour clock; only the reading that counts the 68000's own cycles is written
   in the terms of p. 196.

## Independent evidence

### What the others say

The question put to each: does a CPU chip access ever wait for the parity of
the colour clock, and where does a non-nasty blitter hand the bus back?

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:12140-12161`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L12140-L12161), [`cpuemu_13.cpp:53063`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cpuemu_13.cpp#L53063), [`include/cpu_prefetch.h:376-380`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/cpu_prefetch.h#L376-L380), [`newcpu.cpp:2176`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L2176), [`:1055`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L1055), [`custom.cpp:12410-12417`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L12410-L12417), [`blitter.cpp:1756-1758`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/blitter.cpp#L1756-L1758) | <pre>while (currprefs.cpu_memory_cycle_exact &amp;&amp; quit_program &lt;= 0) {<br>&nbsp;&nbsp;&nbsp;&nbsp;struct rgabuf *r = read_rga_out();<br>&nbsp;&nbsp;&nbsp;&nbsp;if (r-&gt;alloc &lt;= 0) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;break;<br>&nbsp;&nbsp;&nbsp;&nbsp;}<br>&nbsp;&nbsp;&nbsp;&nbsp;blt_info.nasty_cnt++;</pre> a CPU access in cycle-exact mode waits one colour clock at a time **while a DMA channel holds it, and for nothing else**. BRA.S spends `do_cycles_ce000_internal(2);`, which is `x_do_cycles (clocks * cpucycleunit);` with `cpucycleunit = CYCLE_UNIT / 2;`, and the cycle-exact 68000 sets `x_do_cycles = do_cycles_ce;`, which runs one `do_cck(true);` per `CYCLE_UNIT` — so the internal operation is one colour clock, and its next fetch lands one colour clock later, on the other parity, as fx68k's does. The blitter yields on a count, `bool bstreq = blt_info.nasty_cnt >= BLIT_NASTY_CPU_STEAL_CYCLE_COUNT && !pri;`, under the comment *"blitter not in nasty mode = CPU gets one cycle if it has been waiting at least 4 cycles (all DMA cycles count, not just blitter cycles, even blitter idle cycles do count!)"* |
| **vAmiga** | [`Core/Components/Agnus/Agnus.cpp:489-519`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L489-L519), [`Core/Components/Agnus/AgnusDma.cpp:49-50`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L49-L50), [`Core/Components/CPU/CPU.cpp:39`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/CPU.cpp#L39), [`Core/Components/CPU/Moira/MoiraExec_cpp.h:841-848`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/Moira/MoiraExec_cpp.h#L841-L848) | <pre>if (busOwner[pos.h] != BusOwner::NONE) {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;if (++delay == 2) bls = true;<br><br>&nbsp;&nbsp;&nbsp;&nbsp;} while (busOwner[pos.h] != BusOwner::NONE);</pre> `executeUntilBusIsFree()` runs Agnus a DMA cycle at a time **until the cycle has no owner, and assigns it to the CPU whatever its parity**, `busOwner[pos.h] = BusOwner::CPU;`. The CPU clock drives Agnus at one DMA cycle per two CPU cycles, `agnus.execute(CPU_AS_DMA_CYCLES(cycles));`, and BRA begins with `SYNC(2);`, one DMA cycle. On the CPU's second denied cycle BLS goes up and the blitter is refused, `if (bls && !bltpri()) return false;` — a count, no parity term |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:380-398`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L380-L398), [`:771-778`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L771-L778), [`emulation/libami/agnus/dma.cpp:449-450`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L449-L450), [`emulation/libami/cpu/m68000/instruction.cpp:463-464`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/cpu/m68000/instruction.cpp#L463-L464) | <pre>while (busUsage != BUS_FREE) {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;dmaCycle();<br>&nbsp;&nbsp;&nbsp;&nbsp;countWaitCycles++;</pre> the CPU waits **only while the bus is in use**, then takes it, `busUsage = BUS_USAGE_CPU;`. Agnus advances one DMA cycle per two CPU cycles, `cycles -= 2;`, and `opBcc` begins with `SYNC(2);`, one DMA cycle. The blitter is refused on a count, `if (!blitterNasty() && (countWaitCycles > 2))`, under *"in non nasty mode, CPU gets BUS after 3 wait cycles."* — no parity term |
| **Copperline** | [`src/bus.rs:6925-6927`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L6925-L6927), [`src/bus/dma_slots.rs:723-728`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L723-L728), [`:815-826`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L815-L826), [`:1103-1106`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L1103-L1106), [`src/bus.rs:40-50`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L40-L50), test [`src/bus/tests.rs:10965-11020`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L10965-L11020) | <pre>matches!(<br>&nbsp;&nbsp;&nbsp;&nbsp;self.scheduled_dma_owner(true),<br>&nbsp;&nbsp;&nbsp;&nbsp;ChipBusOwner::Cpu \| ChipBusOwner::Idle<br>)</pre> the CPU waits `while !self.cpu_can_use_current_slot()`, which asks **only who owns the colour clock**. A non-nasty blitter yields after `BLITTER_SLOWDOWN_CPU_MISS_LIMIT` = 3 missed cycles; the comment says it does so *"not on every even slot-pair"* and that *"The old even/odd grant starved the blitter so big fills overran the frame and flickered."* Its own test `blithog_clear_busy_blitter_yields_to_cpu_only_after_starvation` starts a CPU read of DMACONR at $24 beside such a blit and asserts it ends at `0x24 + u32::from(BLITTER_SLOWDOWN_CPU_MISS_LIMIT) + 2`: three missed colour clocks, the grant at $27, a bus-free tail — and $27 is odd on its grid, which puts the fixed DMA on odd colour clocks, `if !(0x015..=0x033).contains(&hpos) \|\| hpos & 1 == 0 {`. |
| **MiSTer** | [`rtl/minimig_m68k_bridge.v:151`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig_m68k_bridge.v#L151), [`rtl/agnus.v:400-405`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L400-L405) | descendant — identical to NanoMig: the same acknowledge, `if (!l_as && cck && ((!vpa && !(dbr && dbs)) \|\| (vpa && vma && eclk[8])) && !nrdy) _ta_n <= 0;`, and the same counter, `if (!bls \|\| bltpri) bls_cnt <= 2'b00;` |
| **TC64** | [`rtl/minimig/minimig_m68k_bridge.v:206`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig_m68k_bridge.v#L206), [`rtl/minimig/agnus.v:430-437`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L430-L437) | descendant — the same acknowledge, `if (!l_as && cck && ((!vpa && !(dbr && dbs)) \|\| (vpa && vma && eclk[8])) && !nrdy)`, and the same counter, which also counts in turbo mode, `if (!cck \|\| turbo)`. Nothing in either line holds the CPU to a parity |

**None of the four holds the 68000 to one parity.** Each makes a CPU access
wait while DMA owns the colour clock and for no other reason, and WinUAE,
vAmiga and Denise spend BRA's internal operation as one colour
clock and then fetch in the next free one, as fx68k does with no
DMA. **Beside a non-nasty blitter all four release the bus by counting missed
cycles** — three in WinUAE's constant, Denise's comment and Copperline's
constant, two before vAmiga raises BLS — and none of them moves the released
cycle to an even slot. On this point the four
side with NanoMig and against p. 196's *only*.

## Decision

**One option: withhold the CPU's acknowledge in colour clocks of the fixed-DMA
parity.** Agnus has the beam position; a term on it in the last branch of the
priority chain in `agnus.v` (lines 255–266), or a new input to the bridge's
acknowledge, would leave such a free colour clock to the blitter or to nobody
instead of to the CPU. NanoMig's own counter puts refresh at `hpos[8:1]` = $04,
$06, $08 and $0A (`agnus_refresh.v`) — even numbers in its counter, the slots
Figure 6-9 calls odd — so the CPU would be held to the colour clocks whose
`hpos[8:1]` is odd. What it would change:

- the fx68k's no-DMA loop would no longer take odd slots, as TG68K's loop
  does not;
- beside a non-nasty blit, a release that lands on an odd slot would reach the
  CPU one slot later, which interacts with the `A5` question above;
- NanoMig would move away from all four implementations and both Minimig
  lines, which is the other half of the decision.
