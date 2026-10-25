# NM-AA-013 — a Copper write to REFPTR moves no refresh address: every refresh slot drives address 0, where AA p. 27 makes REFPTR the refresh address generator [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `agnus.v` put a refresh pointer, loaded by a write to REFPTR ($028), on the chip address bus in the refresh slots of each line, as AA PDF p. 27 makes REFPTR "a Dynamic RAM refresh address generator", where NanoMig drives address 0 in every refresh slot, so that a Copper MOVE of $5555 and one of $AAAA to $028 with COPCON bit 1 set leave the same refresh addresses? Closes when the refresh slot takes its address from a pointer that a write to $028 loads, and `P1` and `PR-2a` are green on both cores, or when an Amiga specialist rejects the pointer; if the rejection means p. 27 is wrong for the AA chip set, the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | AA chip set selected (`chipset_config` = $10); REFPTR written once by a Copper MOVE after a WAIT for line $40, with COPCON bit 1 set, in run RA with $5555 and in run RB with $AAAA, the two runs differing in that word only; the refresh accesses are those the `refresh` output of `nanomig.v` marks at its RAM port, from the Copper's fetch of the data word to the end of the run; on fx68k and TG68K. Not measured: a write by the processor, which p. 27 rules out; the bit layout of a written word, which no page gives; the MOVE with COPCON bit 1 clear (`P2` and `P1P2-JOINT` print NOT GRADED). Chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software; on NanoMig: the change alters what the RAM port of `nanomig.v` shows in the refresh slots and not what the memory does, because the SDRAM auto-refresh command carries no address |
| **Chapter** | Commodore, *Functional Specification for the Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY · PDF p. 27 (the REFPTR entry), p. 6 (row 028 and its legend), p. 29 (the refresh slots). The document prints no page numbers: p. N is the PDF page. *Amiga Hardware Reference Manual*, 3rd ed., 1991, printed p. 282 (Appendix A), as context · `P1`: `opus:AA-refptr.c1`, `opus:AA-refptr.c2`, `sol:AA-refptr.c1`, `sol:AA-refptr.c2`, p. 6; `PR-2a`, `P2`, `P1P2-JOINT`, `P1-RE`, `P1-RN`, `R2-XCHK-REF`, `PR-1a`, `PR-1b` and `PR-1c` carry no claim id; `opus:AA-refptr.c4`, `opus:AA-refptr.c5`, `sol:AA-refptr.c4` and `sol:AA-refptr.c5`, the `&` and `~` marks of row 028, print DECLINED on RECORDED lines |
| **Module** | `agnus.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L183-L194) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L148-L158); inherited (`agnus_refresh.v` is the same file in NanoMig, MiSTer and TC64 once line endings are normalised, and all three drive address 0 in the refresh slot) |
| **Test** | [`integration/aa-integ-s7-rest`](../../integration/aa-integ-s7-rest): `make -k -C integration/aa-integ-s7-rest test`, or `test-fx68k` and `test-tg68k` of the same package; an expected red exits nonzero. The whole design at `nanomig.v`'s ports, once with each CPU core |
| **Assertions** | 2 red on fx68k and 2 on TG68K, in a package run of 36 passed, 21 failed, 11 NOT GRADED on each core: `P1` expects the word addresses of the refresh accesses after the MOVE to differ between RA ($5555) and RB ($AAAA) in at least one position, and observes 129 refresh accesses in each run, the two sequences equal in every position and both starting at $000000; `PR-2a`, the precondition "P1 is green" of the two protection lines, fails with it, and `P2` and `P1P2-JOINT`, the MOVE with COPCON bit 1 clear, print NOT GRADED behind it. Green on the same run: `R2-XCHK-REF`, `PR-1a`, `PR-1b`, `PR-1c`, `PR-2b`, and `P1-RE` and `P1-RN`, which predict no difference. The run's other nineteen reds are not this finding's: fourteen UHRES lines ([NM-AA-010](NM-AA-010.md)) and five HHPOSW and HHPOSR lines ([NM-AA-008](NM-AA-008.md)) |
| **Verdict of the four** | **split, two and two**: WinUAE and Denise load a refresh pointer from a write to 028, put it on the bus in every refresh slot and advance it each time — WinUAE with a bit mapping of its own for AA, Denise with its OCS/ECS mapping and a `// todo AGA`; vAmiga ignores the write and refreshes at address 0, and Copperline decodes no 028 and records its refresh slots at address 0. WinUAE's and Denise's comments both say AA memory refreshes from its own row counter, not from the address. Hardware: not measured |
| **Effort** | none; if the pointer is accepted, S: a 20-bit pointer register in `agnus.v` that a write to $028 loads, that the refresh slot drives in place of 0 at line 190, and that advances by one after each slot |
| **See also** | [NM-AA-010](NM-AA-010.md) and [NM-AA-008](NM-AA-008.md): the other reds of the same package |

---

## Source requirement

**PDF p. 27** gives the register one sentence of function:

```
REFPTR      028  W   A       Refresh pointer
                    This register is used as a Dynamic RAM refresh address
                    generator. It is writeable for test purposes only, and
                    should never be written by the microprocesor.
```

**PDF p. 6**, the address list, marks the row with two symbols and defines them in its legend:

```
REFPTR    & ~028  W    A        Refresh pointer
```

```
&=Register used by DMA channel only.
~=Address not writable by the Coprocessor unless COPCON bit 1 is set true.
```

So REFPTR is the generator of the refresh addresses, a write to it is meant for testing, the processor should not make one, and the Coprocessor can make one with COPCON bit 1 set. **PDF p. 29** gives the refresh channel its slots, "There are 4 refresh time slots". No page gives REFPTR's bit layout or how a written value becomes the addresses that follow.

The *Hardware Reference Manual*, 3rd ed., Appendix A, printed p. 282, prints the same entry for the original chip set:

> REFPTR 028 W A Refresh pointer
>
> This register is used as a dynamic RAM refresh address generator. It is writeable for test purposes only, and should never be written by the microprocessor.

## RTL and measurement

**The refresh channel is four slots and no address.** `agnus_refresh.v` decodes four horizontal positions and nothing else ([`agnus_refresh.v#L11-L18`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_refresh.v#L11-L18)), and in the slot it requests Agnus drives address 0 ([`agnus.v#L183-L194`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus.v#L183-L194)):

```verilog
else if (dma_ref) begin
	// bus allocated to refresh dma engine
	...
	address_out = 0;
	reg_address = 8'hFF;
	...
	refresh = 1;
```

None of [NanoMig's minimig-aga sources](https://github.com/harbaum/NanoMig/tree/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga) decodes $028. The Copper may make the MOVE: Agnus's `ecs` input is `.ecs(|chipset_config[4:3]),` ([`minimig.v#L488`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L488)), set with the AA chip set, and with CDANG set the Copper refuses a register below $040 only while `ecs` is clear ([`agnus_copper.v#L224-L228`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_copper.v#L224-L228)). The word then reaches no register, and nothing it could hold reaches the refresh slot.

[`integration/aa-integ-s7-rest`](../../integration/aa-integ-s7-rest) has the Copper write REFPTR, as p. 27 keeps the processor away from it: a Copper list of a WAIT for line $40 and a MOVE of the value to $028, with COPCON bit 1 set, in run RA with $5555 and in run RB with $AAAA (`prog_ref.s`). It compares the word addresses of the refresh accesses, those in whose clock the `refresh` output is high, from the Copper's fetch of the MOVE's data word to the end of the run:

```
MEASURED: PR-1c: complete lines of UR between the fetch of L+6 and the end marker: RA 31 lines with 4 to 4 refresh accesses per line, RB 31 lines with 4 to 4 (fx68k)
MEASURED: P1: refresh accesses between the fetch of L+6 and the end marker: RA 129, RB 129; first RA address $000000 RB $000000 (fx68k)
FAIL: P1: a Copper MOVE to REFPTR (028) with COPCON bit 1 set changes the refresh addresses that follow: the sequence of refresh-access word addresses of RA (data $5555) differs from that of RB (data $AAAA) in at least one position (AA p.27: 'This register is used as a Dynamic RAM refresh address generator', AA p.6 row 028) opus:AA-refptr.c1 opus:AA-refptr.c2 sol:AA-refptr.c1 sol:AA-refptr.c2
FAIL: PR-2a: P1 is green (RA and RB differ), so that a REFPTR without any effect cannot make P2 green without testing the protection
```

The same lines on TG68K, with `(tg68k)`. The preconditions are green on both cores: the `refresh` output marks exactly the accesses that a run with no DMA channel enabled shows (`R2-XCHK-REF`); the Copper fetches the WAIT, the MOVE and its data word once each, with the run's value (`PR-1a`); RA and RB agree in every access up to that fetch but at the value word, and in every non-refresh access and every refresh slot after it (`PR-1b`); every complete line after it has refresh accesses, four in each of 31 lines (`PR-1c`). `P1` is graded at `s7r_p1.h:148-151` and `PR-2a` at `:176`.

## Risks and unknowns

- **Does Alice drive a refresh address at all?** If AA memory is refreshed CAS before RAS, as WinUAE's comment says and Denise's describes, the DRAMs count their own rows and REFPTR's address has nothing to reach; p. 27's sentence would then be the older chips' carried forward (the HRM prints it word for word for the original chip set), and the finding moves to the printed page. Hardware is not measured, and the document is marked PRELIMINARY.
- **Is the pointer visible to software?** Denise merges it into a bit-plane pointer where a bit-plane fetch meets a refresh slot. If real Agnus does that, REFPTR's value reaches the display; on NanoMig the refresh slot wins over bit-plane DMA (`agnus.v` line 183 comes before line 207), so a bit-plane fetch never sees the refresh address.
- **What layout does a written word have?** p. 27 gives none. WinUAE's OCS, ECS and AA mappings differ, and Denise's covers OCS and ECS only.
- **The protection of p. 6's `~` is not graded.** `P2` and `P1P2-JOINT` need a REFPTR that a write moves, and print NOT GRADED; the `~` claims are declined, since one MOVE at one beam position cannot establish a rule for every Copper write.
- **One reading of the port.** The package takes the RAM-port address of an access that the `refresh` output marks as the refresh address. The 8371 specification says the refresh controller "places RAS addresses on the memory address bus" in these slots, which is that reading.
- **Half of the implementations agree with NanoMig**, and the two that model REFPTR both say AA memory does not need it. The finding rests on p. 27, not on any implementation.
- **Green lines that say nothing here:** `P1-RE` and `P1-RN`, which grade a predicted difference of 0, are green, as they are on any design whose refresh address never moves.
- **The reading of the page.** The p. 27 and p. 6 rows, p. 27's first line of function and the `&` and `~` legend lines are `[3/3]` in the corpus. The continuation lines "generator. It is writeable for test purposes only, and" and "should never be written by the microprocesor." and p. 29's "There are 4 refresh time slots" carry no identifier and are `[1/3]`; the Tesseract reading has all three, and the 1993 `AGA.guide` retyping has them with "dynamic" and "only,and".

## Independent evidence

The question put to each: does a write to 028 set the address that the following refresh slots put on the memory bus?

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:7255`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7255), [`:2702-2767`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2702-L2767), [`:2681`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2681), [`:9747-9770`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9747-L9770), [`:11633-11636`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L11633-L11636), [`:6761-6765`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6761-L6765), [`:2809`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2809) | <pre>if (r-&gt;type &amp; (CYCLE_REFRESH \| CYCLE_STROBE)) {<br>&nbsp;&nbsp;&nbsp;&nbsp;*r-&gt;p += ref_ras_add;<br>&nbsp;&nbsp;&nbsp;&nbsp;*r-&gt;p &amp;= refmask;<br>&nbsp;&nbsp;&nbsp;&nbsp;refptr = *r-&gt;p;</pre> **REFPTR is the refresh address.** A write to 028 goes to `REFPTR(hpos, value)`, which maps the word into the hidden pointer bit by bit, with an AA mapping of its own (`if (aga_mode) {`, sixteen bit tests, then `refptr <<= 1;`). Every refresh slot is queued with the pointer as its address, `write_rga(RGA_SLOT_IN, CYCLE_REFRESH, 0x1fe, &refptr);`, and advances it when it is served. After reset the AA pointer is `refptr = 0x1ffffe;`. The Copper reaches 028 with COPCON bit 1 set on an ECS or AA Agnus, `if (addr < ((copcon & 2) ? (ecs_agnus ? 0 : 0x40) : 0x80)) {`. The mapping's own header reads `AGA (RAS/CAS 10/10, CBR refresh)` |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:2671-2672`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L2671-L2672), [`Core/Components/Agnus/AgnusEvents.cpp:710-728`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusEvents.cpp#L710-L728) | <pre>case 0x028 &gt;&gt; 1: // REFPTR<br>&nbsp;&nbsp;&nbsp;&nbsp;return;</pre> **no refresh address.** A write to 028 is decoded and dropped. The refresh slots take the bus with address 0, `busAddr[0x01] = 0;`, `busAddr[0x03] = 0;`, `busAddr[0x05] = 0;` and the fourth at $E2 or $E3 |
| **Denise** | [`emulation/libami/agnus/register.cpp:142-148`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L142-L148), [`emulation/libami/agnus/agnus.cpp:792-812`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L792-L812), [`:461-486`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L461-L486), [`emulation/libami/agnus/dma.cpp:155-164`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/dma.cpp#L155-L164), [`emulation/libami/agnus/copper.cpp:342`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L342) | <pre>addrBus = rDmaPtr;<br>rDmaPtr += rasAdder;<br>rDmaPtr &amp;= dmaChipMemMask;</pre> **REFPTR is the refresh address.** A write to 028 calls `setRefPtr(value);` except in the four clocks where the pointer is in use, and `setRefPtr` maps the word with its OCS/ECS layout under `// todo AGA`. Every refresh slot puts the pointer on the address bus and advances it. Where a bit-plane fetch and a refresh slot meet, the pointer merges into the bit-plane pointer, `bpl1pt \|= rDmaPtr;`. The Copper reaches 028 with COPCON bit 1 set, `cdang = (value & 2) ? (agnus.ecsAndHigher() ? 0 : 0x40) : 0x80;`. The refresh routine's comment says AA does not use the address: *"For AGA chipset, DRAM use internal refresh counter. AGA chipset only triggers Refresh but can't control the refresh position."* |
| **Copperline** | [`src/bus/custom_regs.rs:399-434`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L399-L434), [`:1362`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L1362), [`src/bus/dma_slots.rs:203-213`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L203-L213) | <pre>ChipBusOwner::Refresh =&gt; self.annotate_bus_slot(<br>&nbsp;&nbsp;&nbsp;&nbsp;self.agnus.vpos,<br>&nbsp;&nbsp;&nbsp;&nbsp;hpos,<br>&nbsp;&nbsp;&nbsp;&nbsp;BUS_RECORD_REFRESH,<br>&nbsp;&nbsp;&nbsp;&nbsp;0,<br>&nbsp;&nbsp;&nbsp;&nbsp;0x01FE,<br>&nbsp;&nbsp;&nbsp;&nbsp;0,</pre> **no refresh address.** The register write dispatch has cases for 020 to 026 and none for 028, so a write ends in the default, `_ => false,`. A refresh slot owns the bus and is recorded with register 1FE and address 0 |
| **MiSTer** | [`rtl/agnus_refresh.v`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_refresh.v), [`rtl/agnus.v:148-158`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus.v#L148-L158) | descendant — the same `agnus_refresh.v`, and `address_out = 0;` in the refresh slot. No register at 028 |
| **TC64** | [`rtl/minimig/agnus_refresh.v`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_refresh.v), [`rtl/minimig/agnus.v:205-214`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus.v#L205-L214) | descendant — the same `agnus_refresh.v`, and `address_out = 0;` in the refresh slot |

WinUAE and Denise load a pointer from 028 and walk it through the refresh slots, which is p. 27's sentence; vAmiga and Copperline refresh at address 0 and drop the write, which is what NanoMig does. The two that model it say in their own comments that AA memory refreshes from its own row counter: WinUAE in the header of its AA mapping, `CBR refresh`, CAS before RAS; Denise in so many words, *"AGA chipset only triggers Refresh but can't control the refresh position"*, and its mapping is marked `// todo AGA`. WinUAE still keeps and advances an AA pointer.

### What other Commodore documents say

- **Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988**, PDF pages 29 and 19 — agrees with the page:

  > "This register is used as a Dynamic RAM refresh address generator."
  <!-- corpus: agnus-8371-spec/p029.txt -->

  > "The refresh controller uses a single DMA channel with its own time slots."
  <!-- corpus: agnus-8371-spec/p019.txt -->

  > "It places RAS addresses on the memory address bus (MAs) during these slots, in order to refresh the dynamic RAM."
  <!-- corpus: agnus-8371-spec/p019.txt -->

  The register entry is p. 27's, word for word. The channel's description says where the address goes: on the memory address bus, as RAS addresses, in the refresh slots. AA p. 29 lists the part as `8371(fat-pal) (agnus-pal)`.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 208 — agrees with the page:

  > "This register is used as a Dynamic RAM refresh address generator."
  <!-- corpus: a500-a2000-trm-1987/p208.txt -->

  The technical reference's register summary carries the same entry.

## Decision

**One option: a refresh pointer in `agnus.v`.**

- **The register.** A 20-bit pointer that a write to 028 loads, reset to 0. The page gives no bit layout, so the layout is open.
- **The slot.** In the refresh slot, `address_out` (line 190) takes the pointer in place of 0, and the pointer advances by one after each slot.
- **The Copper.** Nothing to add: 028 lies below $080, and the Copper already needs COPCON bit 1 to write there, as p. 6's `~` says ([`agnus_copper.v#L224-L228`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_copper.v#L224-L228)).

With the pointer, RA's and RB's refresh addresses differ after the MOVE, so `P1` and `PR-2a` would turn green, and `P2` and `P1P2-JOINT` would be graded. The SDRAM controller in `src/misc/sdram.sv` serves a chipset refresh cycle on its refresh port ([`sdram.sv#L359-L363`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/misc/sdram.sv#L359-L363)) with an auto-refresh command that carries no address ([`#L387-L388`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/misc/sdram.sv#L387-L388)), so there the change would alter what the RAM port of `nanomig.v` shows in the refresh slots and not what the memory does. This is a reading of the code, not run by `make test`.
