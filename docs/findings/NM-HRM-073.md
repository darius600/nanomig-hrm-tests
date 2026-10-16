# NM-HRM-073 — a COPJMP2 written before Copper DMA is turned on starts the Copper at COP1LC, where pp. 25, 26, 30 and 276 start it at COP2LC [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Question: when COPJMP2 (p. 26, "Restart Copper from address contained in COP2LC") is written after the start of a frame with Copper DMA off, and Copper DMA is then turned on in the same frame, must the Copper's first fetch be at COP2LC, so that NanoMig has to stop the frame start's standing COP1LC request from outranking the later strobe, where NanoMig fetches from COP1LC? Closes when the change is made and `P4`, `P4-Run2`, `P4-Run3-reorder` and `P5-LOCATION-R2` are green on both cores, or when an Amiga specialist records that the standing restart outranks the later strobe, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | NanoMig's Copper (`agnus_copper.v`) on fx68k and on TG68K; Copper DMA off since the start of the frame, COPJMP2 written on a line from $40 to $C0 of a PAL frame, Copper DMA turned on in the same frame, no Copper DMA cycle between the frame start and the strobe; chip revision unknown, hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 2 — Coprocessor Hardware, printed pp. 25, 26, 30, 32; 3 — Playfield Hardware, printed pp. 65 and 88 (examples); Appendix A, printed pp. 263 and 276; 7 — System Control Hardware, printed p. 219 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claim ids by assertion: `P4`: `ch2-starting-the-copper-after-reset.c1.s2`, `ch2-starting-the-copper-after-reset.c2.s2` (p. 30); `P5-LOCATION-R2`: `ch2-location-registers.c16`, `ch2-location-registers.c17` (p. 25), `ch2-jump-address-strobes.c1`, `ch2-jump-address-strobes.c7` (p. 26), `ch2-appa-copins.c142` (p. 276); `P4-Run2` [pp. 25, 26, 30, 276], `P4-Run3` [p. 25], `P4-Run3-reorder` [p. 25] and `P4-Run3-PAIR` [p. 25] carry no claim id, only their page tags |
| **Module** | `agnus_copper.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L196-L199) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L196-L199); inherited (MiSTer has the same code; TC64's lines are identical) |
| **Test** | [`integration/ch2-copper-control`](../../integration/ch2-copper-control): `make -k -C integration/ch2-copper-control test` (or `test-fx68k` and `test-tg68k`); an expected red exits nonzero. [`integration/ch2-copper-move`](../../integration/ch2-copper-move): `make -k -C integration/ch2-copper-move test` (or `test-fx68k` and `test-tg68k`); an expected red exits nonzero |
| **Assertions** | In `integration/ch2-copper-control`, 5 red on each core (123 passed, 5 failed; these five are the only reds of that package), each observed at a first fetch at A, $001000: `P4` (expected A for Run 1 and B for Run 2), `P4-Run2` (B), `P4-Run3` (B), `P4-Run3-reorder` (C) and `P4-Run3-PAIR` (B and C). `P4-CTRL` and `P4-CTRL-C` are green, fetching $001200 and $001400, and so are the preconditions `P4a`, `P4b` and `P4c`. In `integration/ch2-copper-move`, 1 red of 2 on each core (77 passed, 2 failed on fx68k; 85 passed, 2 failed on TG68K; the other red is `P8-RESTART`, [NM-HRM-084](NM-HRM-084.md)'s): `P5-LOCATION-R2`, expected first fetch $25A5C after the COPJMP2 strobe, observed $20004, in ECS and in OCS; its restarts at $20004 are as expected. `P5-LOCATION-R2-CTRL` and `P5-LOCATION` are green |
| **Verdict of the four** | **unanimous against NanoMig on the package's runs**: WinUAE, vAmiga, Denise and Copperline all start Run 1 at A and Run 2 at B, where NanoMig starts Run 2 at A. Their mechanisms differ in a way the package's addresses do not separate: WinUAE, vAmiga and Copperline start at COP2LC whatever vertical blank came before the COPJMP2; Denise, like NanoMig, keeps the vertical blank's COP1LC strobe pending while Copper DMA is off, but merges it with the COPJMP2 and starts at COP1LC \| COP2LC, which for the package's lists, $001000 and $001200, is B's address. Hardware: not measured |
| **Effort** | S — two extra clear conditions on the `strobe1` and `strobe2` latches in `agnus_copper.v` lines 250-266, not run |

---

## Source requirement

**Printed p. 30**, *Starting the Copper After Reset*, prescribes the case measured, with either register:

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

The one automatic use of COP1LC is the restart in the box on **printed p. 25**:

> About Copper restart. At the start of each vertical blanking interval, COP1LC is automatically used to start the program counter. That is, no matter what the Copper is doing, when the end of vertical blanking occurs, the Copper is automatically forced to restart its operations at the address contained in COP1LC.

The box names both the start and the end of vertical blanking; p. 30 (*"When the screen display ends and vertical blanking starts"*), p. 32 (*"at the start of the vertical blanking period"*) and p. 276 (*"at the beginning of each vertical blank time"*) name the start. **Printed p. 219** places both ends:

> The minimum time of vertical blanking is 20 horizontal scan lines for an NTSC system and 25 horizontal scan lines for a PAL system. The range starts at line 0 and ends at line 20 for NTSC or line 25 for PAL.

**Read with the rest of the manual, no page lets a vertical blank that came before the strobe decide where the Copper starts.** Whichever edge of blanking the restart is read at, in a frame where the strobe is written on a line after 25 and Copper DMA is turned on before the frame ends, the restart lies before the strobe, and p. 25 makes the strobe the later event: the Copper *"jump[s] to a new location, from which its next instruction will be fetched"*, and continues *"until the Copper is interrupted by another jump address strobe"*. No page makes a strobe's effect wait for Copper DMA, and p. 30 and p. 276 ask for the strobe precisely while Copper DMA is off. p. 30's *"Usually, COP1LC is used"* gives a reason to prefer COP1LC, the register the restart reuses every frame, not a condition on COP2LC, which the same sentence's first half offers as the alternative. The examples on printed pp. 65 and 88 start the Copper with COP1LC and COPJMP1 only. p. 263 asks that a strobe be written with MOVE.W, not CLR.W; the package's strobe is `move.w #0,COPJMP2(a0)`, which assembles to one write.

## RTL and measurement

**The start of every frame raises the same latch a COPJMP1 write raises, and nothing lowers it until the Copper gets a DMA cycle.** `sof` sets `copjmp1` as a write to COPJMP1 does ([`agnus_copper.v#L236-L237`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L236-L237)), and `copjmp1` sets `strobe1`, cleared only in the Copper's reload cycle, `else if (copper_state==RESET && dma_ack)` ([`#L253-L256`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L253-L256)); `strobe2` is the same latch for COPJMP2 ([`#L262-L265`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L262-L265)). `sof` is the beam counter's end of frame, `assign end_of_frame = vpos_inc & last_line;` ([`agnus_beamcounter.v#L374-L377`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_beamcounter.v#L374-L377)), wired as `.eof(sof),` ([`agnus.v#L485`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L485)). `dma_ack` needs Copper DMA: `assign dma_ack = ackdma & enable;` ([`agnus_copper.v#L363`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L363)), and Agnus raises the acknowledge only in the branch `dma_cop` selects ([`agnus.v#L231-L235`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L231-L235)), with `wire dma_cop = req_cop & copen;` ([`#L126`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L126)) and `assign copen  = dmacon[7] & dmacon[9];` ([`#L300`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus.v#L300)). While Copper DMA is off, every start of frame leaves `strobe1` set.

**In the reload cycle `strobe1` is tested first** ([`agnus_copper.v#L196-L199`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L196-L199)):

```verilog
if (dma_ack && strobe1 && copper_state==RESET)//load pointer with location register 1
	address_out[20:1] <= {cop1lch[20:16],cop1lcl[15:1]};
else if (dma_ack && strobe2 && copper_state==RESET)//load pointer with location register 2
	address_out[20:1] <= {cop2lch[20:16],cop2lcl[15:1]};
```

So a COPJMP2 written after a vertical blank, with Copper DMA off since then, sets `strobe2` beside a `strobe1` still standing from the start of the frame, and when Copper DMA comes on the pointer is loaded from COP1LC. The location register is read in that cycle, not when the strobe is written; the module says so itself at [`#L392`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L392), `//after reset or strobe write an allocated DMA cycle is required to reload instruction pointer from location registers`.

### Measured

[`integration/ch2-copper-control`](../../integration/ch2-copper-control) places lists A, B and C at $001000, $001200 and $001400, each a MOVE to COLOR00 and a WAIT that never ends. Its P4 program reads DMACONR first, leaves Copper DMA off, samples the beam until a record shows a line between $40 and $C0, writes a marker MS, the location registers and the strobe, a marker M1, then DMACON = $8280, and logs every access at the chip RAM port. The first fetch in A, B or C after M1, identical on fx68k and TG68K:

| run | written after MS, in order | first fetch after DMA-on | the pages |
|---|---|---|---|
| Run 1 | COP2LC = B, COP1LC = A, COPJMP1 | A | A |
| Run 2 | COP1LC = A, COP2LC = B, COPJMP2 | **A** | B |
| Run 3 | COP1LC = A, COP2LC = B, COPJMP2, COP2LC = C | **A** | B |
| Run 3-reorder | COP1LC = A, COP2LC = B, COP2LC = C, COPJMP2 | **A** | C |
| `P4-CTRL` | COP2LC = A, COP1LC = B, COPJMP1 | B | B |
| `P4-CTRL-C` | COP1LC = C, COPJMP1 | C | C |

**The five reds record one behaviour.** In Runs 2, 3 and 3-reorder the first fetch is $001000, the first word of COP1LC's list. `P4` grades Run 1 and Run 2 together and `P4-Run2` grades Run 2 alone, both against B, the list p. 30's strobe selects, and both are red through Run 2. `P4-Run3` grades Run 3 against B, the value COP2LC holds when COPJMP2 is written (p. 25), and is red at A. `P4-Run3-reorder` grades Run 3-reorder against C, where p. 25 gives C, and is red at A; `P4-Run3-PAIR` grades Run 3 and Run 3-reorder together and is red through both starting at A. `P4-CTRL`, with COP2LC = A as a decoy, reads B, so A in the red runs is not a comparator stuck on list A. `P4a` holds (the first stored DMACONR has bit 7 clear), `P4b` holds (no fetch in A, B or C before M1), and both are graded on each of the six captures, as is `P4c`.

The assertions are graded in [`test_ch2_copper_control.cpp`](../../integration/ch2-copper-control/test_ch2_copper_control.cpp) at these lines:

| assertion | grading call and label |
|---|---|
| **P4** | `test_ch2_copper_control.cpp:1159-1160` |
| **P4-Run2** | `test_ch2_copper_control.cpp:1176-1177` |
| **P4-Run3** | `test_ch2_copper_control.cpp:1181-1182` |
| **P4-Run3-reorder** | `test_ch2_copper_control.cpp:1198-1199` |
| **P4-Run3-PAIR** | `test_ch2_copper_control.cpp:1215-1216` |
| **P4-CTRL** | `test_ch2_copper_control.cpp:1228-1229` |
| **P4-CTRL-C** | `test_ch2_copper_control.cpp:1231-1232` |

**No vertical blank lies between the strobe and the fetch.** `P4c`, the package's precondition (c), holds on all six captures on both cores: the last record stored before MS and the record following the first record stored after the first fetch lie in the same frame and both read V > 25 (p. 219). Each capture's first fetch is at the same address on fx68k and on TG68K, so the Copper, not the CPU, decides the address.

**The same sequence in [`integration/ch2-copper-move`](../../integration/ch2-copper-move), run R2.** `BASELINE` writes DMACON = $7FFF, so Copper DMA is off from the start. The program waits for the beam to leave line $80 (`prog_p5_r2.s:24`), writes COP1LC = $00020004 and COP2LC = $00025A5C by long-word writes (`:25-26`), writes marker 1, then `move.w #0,CUSTOM+COPJMP2` (`:31`, assembled as `33FC000000DFF08A`, one write), marker 2 and DMACON = $8280 (`:37-38`). The only custom-register read in the listing is `move.b CUSTOM+VHPOSR,d0` in the line wait. The precondition `P5-PRE-A` holds, no Copper access between marker 0 and marker 2 (`test_ch2_copper_move.cpp:1025`). `P5-LOCATION-R2` is graded at `:1109-1113`; its comparator takes the first Copper access after marker 2 against COP2LC's $25A5C and every later restart against COP1LC's $20004 (`:1063-1064`). On fx68k and TG68K, in ECS and in OCS:

```text
MEASURED: P5 [R2, ECS] first $20004; restart accesses: $20004 $20004 $20004 $20004 $20004
FAIL: P5-LOCATION-R2: ... the Copper's first fetch after the COPJMP2 strobe, written with Copper DMA off, is at $25A5C, and every restart is at $20004
```

The restarts equal $20004 as graded; the red is the first fetch, the case of lines 196–199 above: the start of frame's `strobe1` stands while Copper DMA is off, and the reload reads COP1LC. `P5-LOCATION-R2-CTRL`, run R2c with COP1LC and COP2LC both $25A5C at the strobe, is green with first fetch $25A5C; the package prints 77 passed, 2 failed on fx68k and 85 passed, 2 failed on TG68K, the other red being `P8-RESTART` ([NM-HRM-084](NM-HRM-084.md)).

**Where the start address comes from.** The same three programs, each with one change before MS (COP1LC pointed at a fourth list D, $001600, a WAIT that never ends; COPJMP1; Copper DMA on for three beam records and off again, DMACON = $0080), a measurement that `make test` does not print and no assertion grades:

| scratch variant | first fetch after DMA-on, fx68k and TG68K | the pages |
|---|---|---|
| Run 2, the Copper given DMA once earlier in the frame | **B** | B |
| Run 3, the same | **C** | B |
| Run 3-reorder, the same | **C** | C |

Before MS the Copper reads D's two words at $001600 and $001602 and stops. After DMA-on its first read is at $001604, the word after D's WAIT, where its pointer stood (the reload cycle of lines 196–199), and the next is B's (Run 2) or C's (Run 3) first word. Once a DMA cycle has cleared `strobe1` in this frame, a COPJMP2 written with Copper DMA off starts the Copper at COP2LC; the standing `strobe1` is what starts Runs 2, 3 and 3-reorder at A.

## Risks and unknowns

- **Hardware is not measured.** pp. 25, 26, 30 and 276 say the strobe loads the program counter; NanoMig and both Minimig lines keep the vertical blank's COP1LC request standing and prefer it; Denise keeps it and ORs it with the later one. On real hardware, COP2LC and COP1LC set to lists whose addresses OR to a third, COPJMP2 written after line 25 with Copper DMA off, then Copper DMA turned on in the same frame, would settle all three readings at once.
- **The package's addresses do not separate the mechanisms.** WinUAE ORs two strobes queued in the same step and Denise ORs the standing and the later strobe; lists whose addresses OR to a third address would start Denise at neither list. The package's $001000 and $001200 OR to B's address, and `integration/ch2-copper-move`'s lists, $20004 and $25A5C, OR to $25A5C, COP2LC's address: by [`copper.cpp:54-55`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L54-L55), read and not run, Denise starts there too.
- **Run 3 is a separate question, not filed here.** Run 3 writes COP2LC = C after COPJMP2 and before Copper DMA is turned on. p. 25 says the location register *"is loaded into the Copper program counter"* when the strobe is written, so Run 3 reads B. All four implementations read C, and so does NanoMig once the pending strobe is out of the way (the scratch variant), because the location register is read when Copper DMA starts or a later write moves a Copper that has had none. That is a question about p. 25's sentence, not about the standing COP1LC request, so `P4-Run3` and `P4-Run3-PAIR` would stay red with the latch out of the way. `P4-Run3-reorder` does not reach it: COP2LC holds C both at the strobe and at DMA-on, p. 25 and all four give C, and its red, A, is this page's. The claims rest on Runs 1 and 2: `P4`, which carries them, grades only those two runs, and all four implementations give its answer, A and B.
- **When is the location register read?** p. 25 reads it at the strobe (Run 3: B). WinUAE and Denise read it at the first Copper DMA cycle, vAmiga and Copperline let a location-register write move a Copper that has not run since the vertical blank (Run 3: C), and NanoMig without the standing latch reads it at its reload cycle (C); Copperline attributes its rule to captures of a real Agnus.
- The suggested change has not been run. A COPJMP2 written in the same `clk_ena` step as the start of frame would still set both latches, and line 196 would still prefer COP1LC.

## Independent evidence

The question put to each: after a vertical blank with Copper DMA off, where does a COPJMP2 written before Copper DMA is turned on start the Copper, and when is the location register read?

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:7301-7302`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7301-L7302), [`:10934`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L10934), [`:2936-2946`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2936-L2946), [`:9177-9194`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9177-L9194), [`:9067-9071`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9067-L9071), [`:9019-9027`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9019-L9027), [`:2925-2934`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2925-L2934) | **a later strobe replaces a pending one, and the register is read in the Copper's first DMA cycle.** A write to COPJMP2 calls `COPJMP(2, false);` and the start of each frame `COPJMP(1, true);`; each is queued in `startstrobe` and, a few cycles later, assigned to `cop_state.strobe` (`if (cop_state.startstrobe & 15) {` … `cop_state.strobe = cop_state.startstrobe;`), an assignment, not an OR, so a COPJMP2 written after the vertical blank replaces the vertical blank's strobe. The DMA request that consumes it needs Copper DMA, `bool ena_odd = odd_cycle && dma && check_rga_free_slot_in();`, and only in that cycle does `cop_state.ip = getstrobecopip();` read the register, `return cop2lc;` for strobe 2. Run 2 starts at B and Run 3-reorder at C; Run 3 starts at C, because COP2LC holds C when Copper DMA comes on |
| **vAmiga** | [`Core/Components/Agnus/Copper/CopperRegs.cpp:47-64`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperRegs.cpp#L47-L64), [`Core/Components/Agnus/Copper/Copper.cpp:46-54`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L46-L54), [`Core/Components/Agnus/AgnusDma.cpp:27-30`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L27-L30), [`Core/Components/Agnus/Copper/CopperEvents.cpp:282-290`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperEvents.cpp#L282-L290), [`Core/Components/Agnus/Copper/CopperRegs.cpp:122-137`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperRegs.cpp#L122-L137), [`Core/Components/Agnus/Copper/Copper.h:75-79`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.h#L75-L79), [`Core/Components/Agnus/AgnusRegs.cpp:118-124`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L118-L124) | **the strobe loads the program counter when it is written.** `pokeCOPJMP2` calls `switchToCopperList(2);`, which sets the PC from COP2LC at once (`copList = nr;` `setPC(nr == 1 ? cop1lc : cop2lc);`) and requests a fetch; the fetch waits while Copper DMA is off, `if (!copdma()) return false;`. The start of each frame runs `switchToCopperList(1);` and records `activeInThisFrame = agnus.copdma();`, which turning Copper DMA on sets, `if (value) copper.activeInThisFrame = true;`. While it is false, a write to the current list's location register moves the PC as well, `if (!activeInThisFrame && copList == 2) {` … `setPC(cop2lc);`, under the comment *"a write to the location registers will be pushed through the Copper's program counter"*. Run 2 starts at B, Run 3-reorder at C, and Run 3 at C: its COP2LC = C comes after the strobe, while the Copper has not been active in the frame |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:425`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L425), [`emulation/libami/agnus/copper.cpp:362-386`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L362-L386), [`:92-97`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L92-L97), [`:53-59`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L53-L59), [`:77`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L77), [`emulation/libami/agnus/dma.cpp:393-406`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L393-L406) | **the vertical blank's strobe stays pending while Copper DMA is off, as in NanoMig, and a later strobe is merged with it rather than ranked below it.** The start of each frame calls `copper.strobeCOPJMP(1, Trigger_Vsync);`, and every strobe, the CPU's included, adds its bit, `strobeCop \|= pos;`. The bits are cleared only where the pointer is loaded, `strobeCop = 0;`, in the Copper's first DMA cycle, `if (agnus.fetchCopperDma<1>(copPtr, ir2)) {` … `assignCopPtr();`, which cannot happen while Copper DMA is off, `if (!useCopperDMAForQueue())` … `return false;`. With both bits set the pointer is `if (strobeCop == 3)` `copPtr = cop1lc \| cop2lc;`, commented `// fix Quarterline-TrRickDangerous`, read when Copper DMA comes on: $001000 \| $001200 = $001200, B's address, in Run 2; $001000 \| $001400 = $001400, C's, in Run 3 and its reorder |
| **Copperline** | [`src/bus/custom_regs.rs:162-172`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L162-L172), [`:549-566`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L549-L566), [`src/bus/dma_slots.rs:1455-1459`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L1455-L1459), [`:1577-1594`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L1577-L1594), [`src/bus.rs:10506-10516`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L10506-L10516), [`src/bus/dma_slots.rs:1596-1625`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L1596-L1625), test [`src/bus/tests.rs:3146-3181`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L3146-L3181) | **the strobe loads the program counter when it is written**: `copjmp_strobe` sets `self.pending_copper_frame_start = None;` and `self.copper.jump(...)`, under the comment *"reload the Copper program counter from COP1LC/COP2LC now"*; the Copper steps only with Copper DMA on, `&& self.copper_dma_enabled()`. The restart at line 0 points it at COP1LC and records whether Copper DMA is on, `self.copper_active_in_frame = self.copper_dma_enabled();`; while it is not, a write to the current list's location register moves the PC as well, `self.copper.jump(lc);`, a rule the comment calls *"real Agnus behaviour, photographed by the vAmigaTS Copper/lc family"*. Its own test `copper_lc_write_retargets_dormant_copper_pc` strobes COPJMP1 with Copper DMA off, rewrites COP1LCL and asserts *"the redirected list must run without a COPJMP strobe"*. Run 2 starts at B, Run 3-reorder at C, and Run 3 at C |
| **MiSTer** | [`rtl/agnus_copper.v:196-199`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L196-L199), [`:236-237`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L236-L237), [`:253-256`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L253-L256) | descendant — the same latch and the same order, same lines: `if (reg_address_in[8:1]==COPJMP1[8:1] \|\| sof)` sets `copjmp1 = 1;`, `strobe1 = 0;` only `else if (copper_state==RESET && dma_ack)`, and `if (dma_ack && strobe1 && copper_state==RESET)` is tested before `strobe2`. The assignments are blocking where NanoMig's are non-blocking; the latch and its priority are the same |
| **TC64** | [`rtl/minimig/agnus_copper.v:196-199`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L196-L199), [`:236-237`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L236-L237), [`:253-256`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L253-L256) | descendant — identical to MiSTer's file in these lines: `if (reg_address_in[8:1]==COPJMP1[8:1] \|\| sof)`, `else if (copper_state==RESET && dma_ack)`, `if (dma_ack && strobe1 && copper_state==RESET)`. Neither line clears the vertical blank's strobe when COPJMP2 is written |

## Decision

**One option: let a jump strobe clear the other strobe's pending latch, so the later strobe decides.** In lines 250–266:

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

The scratch variant above, in which a DMA cycle has already cleared `strobe1`, shows what a cleared latch gives. Predicted:

- Run 2 would start at B and Run 3-reorder at C, and `P4` would read A and B, so `P4`, `P4-Run2` and `P4-Run3-reorder` would pass; `integration/ch2-copper-move`'s `P5-LOCATION-R2` has the stimulus of Run 2 and would read $25A5C for its first fetch, its restarts staying at COP1LC's $20004, so it would pass;
- Run 3 would start at C, as on each of the four, because lines 196–199 still read the location register in the reload cycle, and `P4-Run3` and `P4-Run3-PAIR` would stay red (see *Risks and unknowns*);
- with Copper DMA on, the reload follows a strobe within a few colour clocks, so the latch is rarely pending there; a CPU COPJMP2 in the colour clocks between the start of frame and the vertical blank's reload would start the Copper at COP2LC instead of COP1LC.
