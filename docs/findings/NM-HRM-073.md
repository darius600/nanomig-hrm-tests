# NM-HRM-073 — a COPJMP2 written before Copper DMA is turned on starts the Copper at COP1LC, where pp. 25, 26, 30 and 276 start it at COP2LC [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_copper.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L196-L199) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L196-L199) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**. NanoMig has the strobe latches from there and TC64 carries the same two, so there is no fix to port; WinUAE shows a later strobe replacing a pending one — see *What the others say* |
| **Test** | [`integration/ch2-copper-control`](../../integration/ch2-copper-control) — `P4`, claims `ch2-starting-the-copper-after-reset.c1.s2`, `ch2-starting-the-copper-after-reset.c2.s2` (pp. 25, 26, 30, 276), which grades Run 1 (COP1LC, COPJMP1) and Run 2 (COP2LC, COPJMP2) together, run on both CPU cores and red on fx68k and on TG68K through Run 2. Three assertions without a claim id are red on both cores through the same start at COP1LC: `P4-Run2` (pp. 25, 26, 30, 276), Run 2 alone; `P4-Run3-reorder` (p. 25), where p. 25 gives C; and `P4-Run3-PAIR` (p. 25), Run 3 and Run 3-reorder graded together. `P4-Run3` is a precondition, the first fetch B or C, red at A, so Run 3's own clause (B, where all four implementations give C) is recorded as not reached — a separate question this page does not file, see *Run 3 is a separate question* |
| **Chapter** | 2 — Coprocessor Hardware, printed pp. 25, 26, 30, 32; Appendix A, printed p. 276; 7 — System Control Hardware, printed p. 219 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | `P4`, `P4-Run2`, `P4-Run3-reorder`, `P4-Run3-PAIR` and the precondition `P4-Run3` red on fx68k and on TG68K, each through a first fetch at A (96 passed, 5 failed on each core; these five are the only reds). In the same run `P4-CTRL` and `P4-CTRL-C`, COP1LC = B and COP1LC = C with COPJMP1, read B and C through the same comparator on both cores, and the preconditions `P4a`, `P4b` and `P4c` hold, `P4c` graded on the captures of Run 1, Run 2, Run 3 and Run 3-reorder each |
| **Verdict of the four** | **unanimous against NanoMig on the package's runs**: WinUAE, vAmiga, Denise and Copperline all start Run 1 at A and Run 2 at B, where NanoMig starts Run 2 at A. Their mechanisms differ in a way the package's addresses do not separate: WinUAE, vAmiga and Copperline start at COP2LC whatever vertical blank came before the COPJMP2; Denise, like NanoMig, keeps the vertical blank's COP1LC strobe pending while Copper DMA is off, but merges it with the COPJMP2 and starts at COP1LC \| COP2LC, which for the package's lists, $001000 and $001200, is B's address |

---

## What the HRM says

**Printed p. 30**, *Starting the Copper After Reset*, prescribes the case
measured, with either register:

> At power-on or reset time, you must initialize one of the Copper location registers (COP1LC or COP2LC) and write to its strobe address before Copper DMA is turned on. This ensures a known start address and known state. Usually, COP1LC is used because this particular register is reused during each vertical blanking time.

**Printed p. 25** says what the strobe does and when:

> The Copper location registers contain the two indirect jump addresses used by the Copper. The Copper fetches its instructions by using its program counter and increments the program counter after each fetch. When a jump address strobe is written, the corresponding location register is loaded into the Copper program counter. This causes the Copper to jump to a new location, from which its next instruction will be fetched. Instruction fetch continues sequentially until the Copper is interrupted by another jump address strobe.

**Printed p. 26** names each strobe's register:

> When you write to a Copper strobe address, the Copper reloads its program counter from the corresponding location register.

> COPJMP1 Restart Copper from address contained in COP1LC.
> COPJMP2 Restart Copper from address contained in COP2LC.

**Printed p. 276**, Appendix A's COPJMP entry, says both again:

> They are transferred to the program counter whenever strobe addresses COPJMP1 or COPJMP2 are written. In addition, COP1LC is automatically used at the beginning of each vertical blank time.
>
> It is important that one of the jump registers be initialized and its jump strobe address hit after power-up but before Copper DMA is initialized. This insures a determined startup address and state.

### The vertical-blank restart, and where it lies

The one automatic use of COP1LC is the restart in the box on **printed
p. 25**:

> About Copper restart. At the start of each vertical blanking interval, COP1LC is automatically used to start the program counter. That is, no matter what the Copper is doing, when the end of vertical blanking occurs, the Copper is automatically forced to restart its operations at the address contained in COP1LC.

The box's two sentences name the start and the end of vertical blanking; p. 30
(*"When the screen display ends and vertical blanking starts"*), p. 32 (*"at
the start of the vertical blanking period"*) and p. 276 (*"at the beginning of
each vertical blank time"*) name the start. **Printed p. 219** places both
ends:

> The minimum time of vertical blanking is 20 horizontal scan lines for an NTSC system and 25 horizontal scan lines for a PAL system. The range starts at line 0 and ends at line 20 for NTSC or line 25 for PAL.

**Read with the rest of the manual, no page lets a vertical blank that came
before the strobe decide where the Copper starts.** Whichever edge of blanking
the restart is read at, in a frame where the strobe is written on a line after
25 and Copper DMA is turned on before the frame ends, the restart lies before
the strobe, and p. 25 makes the strobe the later event: the Copper *"jump[s] to
a new location, from which its next instruction will be fetched"*, and
continues *"until the Copper is interrupted by another jump address strobe"*.
No page makes a strobe's effect wait for Copper DMA, and p. 30 and p. 276 ask
for the strobe precisely while Copper DMA is off. p. 30's *"Usually, COP1LC is
used"* gives a reason to prefer COP1LC — it is the register the restart reuses
every frame — not a condition on COP2LC, which the same sentence's first half
offers as the alternative. The examples on printed pp. 65 and 88 start the
Copper with COP1LC and COPJMP1 only. p. 263 asks that a strobe be written with
MOVE.W, not CLR.W; the package's strobe is `move.w #0,COPJMP2(a0)`, which
assembles to one write.

---

## What NanoMig does

**The start of every frame raises the same latch a COPJMP1 write raises, and
nothing lowers it until the Copper gets a DMA cycle.** `sof` sets `copjmp1`
exactly as a write to COPJMP1 does —
[`src/minimig-aga/agnus_copper.v#L236-L237`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L236-L237):

```verilog
if (reg_address_in[8:1]==COPJMP1[8:1] || sof)
	copjmp1 <= 1;
```

and `copjmp1` sets `strobe1`, which is cleared only in the Copper's reload
cycle —
[`#L253-L256`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L253-L256):

```verilog
if (copjmp1 && clk_ena)
	strobe1 <= 1;
else if (copper_state==RESET && dma_ack)
	strobe1 <= 0;
```

`strobe2` is the same latch for COPJMP2,
[`#L262-L265`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L262-L265).
`sof` is the beam counter's end of frame, the edge on which the vertical count
returns to line 0 —
[`src/minimig-aga/agnus_beamcounter.v#L374-L377`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L374-L377),
`assign end_of_frame = vpos_inc & last_line;` and `assign eof = end_of_frame;`,
wired to the Copper as `.eof(sof),`
([`src/minimig-aga/agnus.v#L485`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L485)).

**`dma_ack` needs Copper DMA.**
[`agnus_copper.v#L363`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L363),
`assign dma_ack = ackdma & enable;`, and Agnus raises the Copper's acknowledge
only in the branch of its priority chain that `dma_cop` selects —
[`src/minimig-aga/agnus.v#L231-L235`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L231-L235),
`ack_cop = 1;` — with `wire dma_cop = req_cop & copen;`
([`#L126`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L126))
and `assign copen  = dmacon[7] & dmacon[9];`
([`#L300`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L300)).
While Copper DMA is off, every start of frame leaves strobe1 set.

**In the reload cycle `strobe1` is tested first** —
[`agnus_copper.v#L196-L199`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L196-L199):

```verilog
if (dma_ack && strobe1 && copper_state==RESET)//load pointer with location register 1
	address_out[20:1] <= {cop1lch[20:16],cop1lcl[15:1]};
else if (dma_ack && strobe2 && copper_state==RESET)//load pointer with location register 2
	address_out[20:1] <= {cop2lch[20:16],cop2lcl[15:1]};
```

So a COPJMP2 written after a vertical blank, with Copper DMA off since then,
sets strobe2 beside a strobe1 still standing from the start of the frame,
and when Copper DMA comes on the pointer is loaded from COP1LC. The location
register is read in that cycle, not when the strobe is written; the module
says so itself at
[`#L392`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L392),
`//after reset or strobe write an allocated DMA cycle is required to reload instruction pointer from location registers`.

### Measured

[`integration/ch2-copper-control`](../../integration/ch2-copper-control)
places lists A, B and C at $001000, $001200 and $001400, each a MOVE to COLOR00
and a WAIT that never ends. Its P4 program reads DMACONR first, leaves Copper
DMA off, samples the beam until a record shows a line between $40 and $C0,
writes a marker MS, the location registers and the strobe, a marker M1, then
DMACON = $8280, and logs every access at the chip RAM port. The first fetch in
A, B or C after M1, identical on fx68k and TG68K:

| run | written after MS, in order | first fetch after DMA-on | the pages |
|---|---|---|---|
| Run 1 | COP2LC = B, COP1LC = A, COPJMP1 | A | A |
| Run 2 | COP1LC = A, COP2LC = B, COPJMP2 | **A** | B |
| Run 3 | COP1LC = A, COP2LC = B, COPJMP2, COP2LC = C | **A** | B |
| Run 3-reorder | COP1LC = A, COP2LC = B, COP2LC = C, COPJMP2 | **A** | C |
| `P4-CTRL` | COP2LC = A, COP1LC = B, COPJMP1 | B | B |
| `P4-CTRL-C` | COP1LC = C, COPJMP1 | C | C |

**This finding rests on Runs 1 and 2**, the two runs that carry the claims:
`P4` grades them together and `P4-Run2` grades Run 2 alone, and both are red
through Run 2. `P4-Run3-reorder` is red because Run 3-reorder starts at A
where p. 25 gives C, by the same mechanism, and `P4-Run3-PAIR`, which grades
Run 3 and Run 3-reorder together, is red through both starting at A. The
precondition `P4-Run3`, a first fetch at B or C, is red at A, so Run 3's own
clause is recorded as not reached; that clause is a separate question (see
below). `P4a` holds: the first stored DMACONR has bit 7 clear. `P4b`
holds: no fetch in A, B or C before M1. Both are graded on Run 1's capture.

**No vertical blank lies between the strobe and the fetch.** `P4c`, the
package's precondition (c) — no frame wrap and no record with V ≤ 25 between
MS and the first fetch — is graded on the captures of Run 1, Run 2, Run 3 and
Run 3-reorder, and holds on both cores; read the same way from `P4-CTRL`'s
capture, a measurement for this page and not a graded assertion, it holds
there too. In every run the last record stored before MS reads line 64, no
record is stored between MS and the first fetch, and the fetch follows MS by
420 clk_sys (Runs 1 and 2, `P4-CTRL`) or 516 (Run 3 and its reorder) on
fx68k, 428 or 524 on TG68K: 52½ to 65½ colour clocks of 8 clk_sys, in a PAL
line of 227. The strobe, DMA-on and the first fetch all lie on line 64 or at
the turn to line 65, and the last vertical blank before them is the one that
began this frame.

**Where the start address comes from.** The same three programs, each with one
change before MS — COP1LC pointed at a fourth list D ($001600, a WAIT that
never ends), COPJMP1, Copper DMA on for three beam records and off again
(DMACON = $0080) — a measurement for this page, not a graded assertion:

| scratch variant | first fetch after DMA-on, fx68k and TG68K | the pages |
|---|---|---|
| Run 2, the Copper given DMA once earlier in the frame | **B** | B |
| Run 3, the same | **C** | B |
| Run 3-reorder, the same | **C** | C |

Before MS the Copper reads D's two words at $001600 and $001602 and stops.
After DMA-on its first read is at $001604, the word after D's WAIT, where its
pointer stood — the reload cycle of lines 196–199 — and the next is B's (Run 2)
or C's (Run 3) first word. Once a DMA cycle has cleared `strobe1` in this
frame, a COPJMP2 written with Copper DMA off starts the Copper at COP2LC; the
standing `strobe1` is what starts Runs 2, 3 and 3-reorder at A. The variant's
C in Run 3 is the second property above: COP2LC is read when Copper DMA comes
on, after the program has rewritten it.

---

## What the others say

The question put to each: after a vertical blank with Copper DMA off, where
does a COPJMP2 written before Copper DMA is turned on start the Copper, and
when is the location register read?

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:7301-7302`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7301-L7302), [`:10934`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10934), [`:2936-2946`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2936-L2946), [`:9177-9194`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9177-L9194), [`:9067-9071`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9067-L9071), [`:9019-9027`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9019-L9027), [`:2925-2934`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2925-L2934) | <pre>if (cop_state.startstrobe &amp; 15) {<br>&nbsp;&nbsp;&nbsp;&nbsp;cop_state.strobe = cop_state.startstrobe;<br>}</pre> **a later strobe replaces a pending one, and the register is read in the Copper's first DMA cycle.** A write to COPJMP2 calls `COPJMP(2, false);` and the start of each frame `COPJMP(1, true);`; each is queued in `startstrobe` and, a few cycles later, assigned to `cop_state.strobe` — an assignment, not an OR, so a COPJMP2 written after the vertical blank replaces the vertical blank's strobe. The DMA request that consumes it needs Copper DMA, `bool ena_odd = odd_cycle && dma && check_rga_free_slot_in();`, and only in that cycle does `cop_state.ip = getstrobecopip();` read the register, `return cop2lc;` for strobe 2. Run 2 starts at B and Run 3-reorder at C; Run 3 starts at C, because COP2LC holds C when Copper DMA comes on. Two strobes queued in the same step give `return cop1lc \| cop2lc;`, a case the package does not reach |
| **vAmiga** | [`Core/Components/Agnus/Copper/CopperRegs.cpp:47-64`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperRegs.cpp#L47-L64), [`Core/Components/Agnus/Copper/Copper.cpp:46-54`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L46-L54), [`Core/Components/Agnus/AgnusDma.cpp:27-30`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L27-L30), [`Core/Components/Agnus/Copper/CopperEvents.cpp:282-290`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperEvents.cpp#L282-L290), [`Core/Components/Agnus/Copper/CopperRegs.cpp:122-137`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperRegs.cpp#L122-L137), [`Core/Components/Agnus/Copper/Copper.h:75-79`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.h#L75-L79), [`Core/Components/Agnus/AgnusRegs.cpp:118-124`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L118-L124) | <pre>copList = nr;<br>setPC(nr == 1 ? cop1lc : cop2lc);</pre> **the strobe loads the program counter when it is written.** `pokeCOPJMP2` calls `switchToCopperList(2);`, which sets the PC from COP2LC at once and requests a fetch; the fetch waits while Copper DMA is off, `if (!copdma()) return false;`. The start of each frame runs `switchToCopperList(1);` and records `activeInThisFrame = agnus.copdma();`, which turning Copper DMA on sets, `if (value) copper.activeInThisFrame = true;`. While it is false, a write to the current list's location register moves the PC as well, `if (!activeInThisFrame && copList == 2) {` … `setPC(cop2lc);`, under the comment *"a write to the location registers will be pushed through the Copper's program counter"*. Run 2 starts at B, Run 3-reorder at C, and Run 3 at C: its COP2LC = C comes after the strobe, while the Copper has not been active in the frame |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:425`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L425), [`emulation/libami/agnus/copper.cpp:362-386`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L362-L386), [`:92-97`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L92-L97), [`:53-59`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L53-L59), [`:77`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L77), [`emulation/libami/agnus/dma.cpp:393-406`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L393-L406) | <pre>if (strobeCop == 3)<br>&nbsp;&nbsp;&nbsp;&nbsp;copPtr = cop1lc \| cop2lc; // fix Quarterline-TrRickDangerous</pre> **the vertical blank's strobe stays pending while Copper DMA is off, as in NanoMig, and a later strobe is merged with it rather than ranked below it.** The start of each frame calls `copper.strobeCOPJMP(1, Trigger_Vsync);`, and every strobe, the CPU's included, adds its bit, `strobeCop \|= pos;`. The bits are cleared only where the pointer is loaded, `strobeCop = 0;`, in the Copper's first DMA cycle, `if (agnus.fetchCopperDma<1>(copPtr, ir2)) {` … `assignCopPtr();`, which cannot happen while Copper DMA is off, `if (!useCopperDMAForQueue())` … `return false;`. A COPJMP2 written after a vertical blank with Copper DMA off leaves both bits set, and the Copper starts at COP1LC \| COP2LC, read when Copper DMA comes on: $001000 \| $001200 = $001200, B's address, in Run 2; $001000 \| $001400 = $001400, C's, in Run 3 and its reorder. Lists whose addresses OR to a third address would start it at neither |
| **Copperline** | [`src/bus/custom_regs.rs:162-172`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L162-L172), [`:549-566`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L549-L566), [`src/bus/dma_slots.rs:1455-1459`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L1455-L1459), [`:1577-1594`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L1577-L1594), [`src/bus.rs:10506-10516`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L10506-L10516), [`src/bus/dma_slots.rs:1596-1625`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L1596-L1625), test [`src/bus/tests.rs:3146-3181`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L3146-L3181) | <pre>fn copjmp_strobe(&amp;mut self, list: u8) {<br>&nbsp;&nbsp;&nbsp;&nbsp;self.pending_copper_frame_start = None;<br>&nbsp;&nbsp;&nbsp;&nbsp;self.copper_current_list = list;<br>&nbsp;&nbsp;&nbsp;&nbsp;self.copper.jump(match list {</pre> **the strobe loads the program counter when it is written**, under the comment *"reload the Copper program counter from COP1LC/COP2LC now"*; the Copper steps only with Copper DMA on, `&& self.copper_dma_enabled()`. The restart at line 0 points it at COP1LC and records whether Copper DMA is on, `self.copper_active_in_frame = self.copper_dma_enabled();`; while it is not, a write to the current list's location register moves the PC as well, `self.copper.jump(lc);`, a rule the comment calls *"real Agnus behaviour, photographed by the vAmigaTS Copper/lc family"*. Its own test `copper_lc_write_retargets_dormant_copper_pc` strobes COPJMP1 with Copper DMA off, rewrites COP1LCL and asserts *"the redirected list must run without a COPJMP strobe"*. Run 2 starts at B, Run 3-reorder at C, and Run 3 at C |
| **MiSTer** | [`rtl/agnus_copper.v:196-199`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L196-L199), [`:236-237`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L236-L237), [`:253-256`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L253-L256) | descendant — the same latch and the same order, same lines: `if (reg_address_in[8:1]==COPJMP1[8:1] \|\| sof)` sets `copjmp1 = 1;`, `strobe1 = 0;` only `else if (copper_state==RESET && dma_ack)`, and `if (dma_ack && strobe1 && copper_state==RESET)` is tested before `strobe2`. The assignments are blocking where NanoMig's are non-blocking; the latch and its priority are the same |
| **TC64** | [`rtl/minimig/agnus_copper.v:196-199`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L196-L199), [`:236-237`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L236-L237), [`:253-256`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L253-L256) | descendant — identical to MiSTer's file in these lines: `if (reg_address_in[8:1]==COPJMP1[8:1] \|\| sof)`, `else if (copper_state==RESET && dma_ack)`, `if (dma_ack && strobe1 && copper_state==RESET)`. Neither line clears the vertical blank's strobe when COPJMP2 is written |

**On the package's runs the four are unanimous against NanoMig**: each starts
Run 1 at A and Run 2 at B, and none lets the vertical blank before the strobe
decide the start. Their mechanisms differ where the package's addresses do not
reach: WinUAE, vAmiga and Copperline start the Copper at COP2LC; Denise keeps
the vertical blank's COP1LC strobe pending as NanoMig does, but merges it with
the later strobe instead of preferring it, and $001000 \| $001200 is B's
address.

---

## Run 3 is a separate question

Run 3 writes COP2LC = C after COPJMP2 and before Copper DMA is turned on. p. 25
says the location register *"is loaded into the Copper program counter"* when
the strobe is written, so Run 3 reads B. **All four implementations read C
there**, and so does NanoMig once the pending strobe is out of the way (the
scratch variant above): WinUAE, Denise and NanoMig read the location register
when Copper DMA starts, and vAmiga and Copperline let a later write to the
strobed location register move a Copper that has had no Copper DMA since the
vertical blank. That is a question about p. 25's sentence, not about the
standing COP1LC request this page files, and NanoMig's A in Run 3 does not
reach it. Run 3's clause grades that sentence; on both cores it is not
reached, because the precondition `P4-Run3` is red at A through the standing
latch this page files, and with that latch out of the way Run 3 would read C
and the clause would be red, so this page does not file it.
`P4-Run3-reorder` does not reach the question: COP2LC holds C both when
COPJMP2 is written and when Copper DMA comes on, p. 25 and all four give C,
and its red, A, is this page's.

---

## Suggested change

**One option: let a jump strobe clear the other strobe's pending latch, so the
later strobe decides.** In lines 250–266:

```verilog
//strobe1 (also triggered by sof, start of frame)
always @(posedge clk)
  if (clk7_en) begin
  	if (copjmp1 && clk_ena)
  		strobe1 <= 1;
  	else if (copjmp2 && clk_ena || copper_state==RESET && dma_ack)
  		strobe1 <= 0;
  end

//strobe2
always @(posedge clk)
  if (clk7_en) begin
  	if (copjmp2 && clk_ena)
  		strobe2 <= 1;
  	else if (copjmp1 && clk_ena || copper_state==RESET && dma_ack)
  		strobe2 <= 0;
  end
```

A COPJMP2 written in the same `clk_ena` step as the start of frame would still
set both, and line 196 would still prefer COP1LC. This has not been run here;
the scratch variant above, in which a DMA cycle has already cleared `strobe1`,
shows what a cleared latch gives. What it would change:

- Run 2 would start at B and Run 3-reorder at C, and `P4` would read A and B,
  so `P4`, `P4-Run2` and `P4-Run3-reorder` would pass;
- Run 3 would start at C, as on each of the four, because lines 196–199 still
  read the location register in the reload cycle, and Run 3's clause and
  `P4-Run3-PAIR` would be red — the separate question above;
- with Copper DMA on, the reload follows a strobe within a few colour clocks,
  so the latch is rarely pending there; a CPU COPJMP2 in the colour clocks
  between the start of frame and the vertical blank's reload would start the
  Copper at COP2LC instead of COP1LC.

---

## What holds the finding up

- **The comparator reads whichever register was strobed.** `P4-CTRL`, COP1LC =
  B with COPJMP1 and COP2LC = A as a decoy, reads B on both cores, so A in Runs
  2, 3 and 3-reorder is not a comparator stuck on list A.
- **Copper DMA was off, and nothing was fetched early.** `P4a` and `P4b` hold
  on both cores, graded on Run 1's capture.
- **No vertical blank lies between the strobe and the fetch.** `P4c` holds on
  both cores, graded on the captures of Runs 1, 2, 3 and 3-reorder: MS, the
  strobe, DMA-on and the first fetch follow each other within 66 colour clocks
  on line 64, with no record between them; the restart that lies before them
  is the one at the start of that frame.
- **The mechanism shows at the ports.** Given one DMA cycle earlier in the
  frame, the same Run 2 starts at B on both cores.
- **Both cores agree**, and the Copper, not the CPU, decides the address: the
  two cores write the registers at different times and read the same list.
- **It rests on Runs 1 and 2 alone.** `P4`, which carries the claims, grades
  only those two runs, and all four implementations give its answer, A and B.
  Run 3, where the four and p. 25 part, is not part of it.

---

## For the specialists

1. **Does a vertical blank's restart outrank a later strobe while Copper DMA
   is off?** pp. 25, 26, 30 and 276 say the strobe loads the program counter;
   NanoMig and both Minimig lines keep the vertical blank's COP1LC request
   standing and prefer it; Denise keeps it and ORs it with the later one. On
   real hardware, COP2LC and COP1LC set to lists whose addresses OR to a third,
   COPJMP2 written after line 25 with Copper DMA off, then Copper DMA turned on
   in the same frame, would settle all three readings at once.
2. **When is the location register read?** Not filed here — see *Run 3 is a
   separate question*. p. 25 reads it at the strobe (Run 3: B). WinUAE and
   Denise read it at the first Copper DMA cycle, vAmiga and Copperline let a
   location-register write move a Copper that has not run since the vertical
   blank (Run 3: C), and NanoMig without the standing latch reads it at its
   reload cycle (C); Copperline attributes its rule to captures of a real
   Agnus.
