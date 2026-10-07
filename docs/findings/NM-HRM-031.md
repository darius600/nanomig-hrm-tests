# NM-HRM-031 — INTREQ bit 14 is a settable request bit where the appendix says enable-only

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test passes; the page's other reading fails |
| **What is asked of a reader** | Does p. 280's `14 INTEN Master interrupt (enable only, no request)`, with p. 218's warning that bit 14 "creates no interrupt request", get a correction saying that a write to INTREQ sets bit 14, INTREQR returns it as `0x4000`, and level 6 serves it when INTENA's bit 14 is set, as Figure 7-4 on p. 221 places INTEN? Closes when an Amiga specialist records that correction, or confirms that real Paula takes no request on bit 14, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Chapter 7 and Appendix A of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | unknown: the pages name no chip revision; measured on NanoMig's `paula_intcontroller.v` at the pinned revision, with a `SET` write naming bit 14 of INTREQ, a readback through INTREQR, and INTENA's bit 14 set with nothing else pending |
| **Software impact** | unknown; the only named software is *Unreal*: `paula.v` carries the note [`implementation of intreq[14] (Unreal needs it)`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L53), and what Unreal does without the bit is not stated |
| **Chapter** | Appendix A, printed p. 280; 7 — System Control, printed pp. 218, 219 and 221 (Figure 7-4) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (13)</summary> claims: `MASTER-BIT14-NO-REQUEST` `5-appA-p280.c20`, `A-int.b14` (p. 280); `P3-INTREQ-FULLY-WRITABLE` carries no claim id, and the package's bus-master-identity record names it with `sol:7-interrupts.c40`, `opus:7-int.c38`, `opus:7-int.c39` (p. 219); `P19-BIT14-INTEN` `sol:7-fig7-4.c45`, `opus:7-fig7-4.c43`, `opus:7-int.c82` (p. 221); `P19-INTEN-SELF-REQUEST-DISPUTED` `sol:7-interrupts.c28`, `opus:7-int.c25`, `sol:7-appA-int.c10`, `opus:7-appA-int.c6`, `opus:7-fig7-4.c45` (pp. 218, 280, 221); `MASTER-ENABLE-BIT14-GATE` carries no claim id </details> |
| **Module** | `paula_intcontroller.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L132-L134) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_intcontroller.v#L132-L134); inherited |
| **Test** | [`hrm/ch5-audio-interrupt`](../../hrm/ch5-audio-interrupt): `make -C hrm/ch5-audio-interrupt test`; [`hrm/ch7-interrupt-priority-encoder`](../../hrm/ch7-interrupt-priority-encoder): `make -C hrm/ch7-interrupt-priority-encoder test`; [`hrm/ch7-interrupt-register-semantics`](../../hrm/ch7-interrupt-register-semantics): `make -C hrm/ch7-interrupt-register-semantics test`; no assertion is red, so there is no expected red |
| **Assertions** | none red. Measured, not graded: `MASTER-BIT14-NO-REQUEST` (expected under p. 280's parenthesis: bit 14 reads 0 after the write; observed `0x4000`), `P19-BIT14-INTEN` (observed `INTREQR` `0x4000` and level 6) and `P19-INTEN-SELF-REQUEST-DISPUTED`. Graded and green: `P3-INTREQ-FULLY-WRITABLE` (32768 patterns read back unchanged, 0 mismatches) and `MASTER-ENABLE-BIT14-GATE`. Package totals: `hrm/ch5-audio-interrupt` 9 passed, 0 failed; `hrm/ch7-interrupt-priority-encoder` 22 passed, 0 failed; `hrm/ch7-interrupt-register-semantics` 26 passed, 0 failed |
| **Verdict of the four** | **unanimous with NanoMig and with Figure 7-4**: WinUAE, vAmiga, Denise and Copperline all latch a written bit 14 in INTREQ, return it through INTREQR, and serve it at level 6 when INTENA's bit 14 is set. None implements p. 280's parenthesis. Hardware: not measured on a real machine; the dissected 8364R7's write path sets any of D0..D14, and it states neither the readback through INTREQR nor the level |
| **Effort** | none — no RTL change: the correction is wording in p. 280's row and p. 218's warning |

---

## Source requirement

**Appendix A, printed p. 280**, in the `INTREQ` / `INTENA` bit table:

> `14  INTEN  Master interrupt (enable only, no request)`

**Chapter 7, printed p. 218**, says the same in prose and adds a warning:

> Bit 14 of the interrupt registers (INTEN) is for interrupt enable. This is the
> master interrupt enable bit. If this bit is a 0, it disables all other
> interrupts. You may wish to clear this bit to temporarily disable all
> interrupts to do some critical processing task.
>
> Warning: This bit is used for enable/disable only. It creates no interrupt request.

The same page describes the request register a write sets:

> Interrupt request (status) - write only. Used by the processor to force a
> certain kind of interrupt to be processed (software interrupt). Also used to
> clear interrupt request flags once the interrupt process is completed.
>
> Positions 14-0 are bit selectors. You write a 1 to any one or more bits to
> select that bit.

**Printed p. 219**, of the Copper's access to the same register:

> The Copper can change the content of any of the bits of this register, as it
> can write any value into most of the machine registers.

**Figure 7-4, printed p. 221**, *Interrupt Priorities*, puts INTEN in a hardware
priority bracket. Its last three rows, read from the figure, not quoted:

| hardware priority | Exec software priority | description | |
|---|---|---|---|
| 6 | 14 | external INT6 & CIAB | EXTER |
| 6 | 15 | special (master enable) | INTEN |
| 7 | -- | non-maskable interrupt | NMI |

The manual says two things about bit 14. p. 280 and p. 218's warning say it
creates no request; Figure 7-4 gives INTEN EXTER's hardware priority, 6, and an
Exec software priority of its own, a place in the order in which pending
requests are served. p. 218's bit selectors and p. 219's "any of the bits" do
not except bit 14.

## RTL and measurement

NanoMig latches a request bit at 14 like any other, and the module's own comment
calls it undocumented
([`paula_intcontroller.v#L130-L134`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L130-L134)):

```verilog
//undocumented interrupt
intreq[14] <= tmp[14];
```

`INTREQR` returns it with the other fourteen
([`#L74-L75`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L74-L75)),
and once `INTENA`'s bit 14 opens the gate
([`#L143-L144`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L143-L144)),
the priority encoder serves it first, with EXTER, at level 6; `_ipl` is active
low, so `1` is level 6
([`#L152-L154`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L152-L154)).
That is Figure 7-4's placement.

| measured | result |
|---|---|
| a `SET` write to `INTREQ` naming only bit 14 (`MASTER-BIT14-NO-REQUEST`, reported, not graded) | bit 14 reads back **set** in `INTREQR`, `0x4000` |
| the control, bit 14 read back when never written (same assertion) | `0x0000`, so the fixture is live and the readback is not stuck high |
| bit 14 set in both `INTENA` and `INTREQ`, nothing else pending (`P19-BIT14-INTEN`, reported, not graded) | `INTREQR` reads `0x4000` and the module drives 68000 level 6 |
| each of the 32768 `INTREQ` patterns written with every enable on, bit 14 among them (`P3-INTREQ-FULLY-WRITABLE`, graded from p. 219, **green**) | every one reads back unchanged, 0 mismatches |

The enable half of the same appendix line is **green**: bit 14 does gate the
other enables, as the manual says (`MASTER-ENABLE-BIT14-GATE`).

## Risks and unknowns

- **The manual contradicts itself, and nothing on these pages says which
  statement is meant.** "No request" can be read as *there is no request bit* or
  as *no hardware raises it*. Both readings fit if no hardware raises it:
  NanoMig has three such bits, SOFT, COPER and INTEN, each set only by a write
  ([`#L110`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L110),
  [`#L114`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L114),
  [`#L134`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L134)).
  p. 218's "enable/disable only" does not bend that far, which is why the page,
  and not the RTL, is what is in question.
- **Nothing in the suite asserts "no request".** The disputed half is recorded,
  not graded, because grading either half would turn one printed page into a
  false finding against the other: `MASTER-BIT14-NO-REQUEST-NOT-GRADED`
  in `hrm/ch5-audio-interrupt`, `P19-BIT14-INTEN` in
  `hrm/ch7-interrupt-priority-encoder` and `P19-INTEN-SELF-REQUEST-DISPUTED` in
  `hrm/ch7-interrupt-register-semantics`.
- **No real machine was measured.** The only silicon reading, the 8364R7
  dissection below, covers one revision and does not state the readback through
  `INTREQR` or the level.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:7311`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7311), [`:3295-3298`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3295-L3298), [`:832-839`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L832-L839), [`:2264-2267`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2264-L2267), [`:3213-3216`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3213-L3216), [`:3113-3117`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3113-L3117), [`newcpu.cpp:4655`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L4655) | a write to `$09C` (`:7311`) reaches `setclr` (`:3295-3298`), which strips bit 15 and nothing else (`:832-839`); `INTREQR` returns bit 14 (`:2264-2267`); the delayed copy is set the same way (`:3213-3216`), `intlev` serves bit 14 with EXTER at level 6 (`:3113-3117`) and `doint` hands it to the CPU (`newcpu.cpp:4655`). **Settable, reads back, level 6** |
| **vAmiga** | [`Core/Components/Paula/PaulaRegs.cpp:92-93`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L92-L93), [`PaulaRegs.cpp:57-62`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L57-L62), [`:70-80`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L70-L80), [`Core/Components/Agnus/AgnusEvents.cpp:221`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusEvents.cpp#L221), [`Paula.cpp:163-169`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Paula.cpp#L163-L169) | the set ORs the value into `intreq` (`PaulaRegs.cpp:92-93`), and the caller masks only bit 15 ([`Paula.h:263`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Paula.h#L263)); `peekINTREQR` returns `intreq` unmasked (`:57-62`); with a 68000 or 68010 the write is deferred (`:70-80`) and applied through the same caller (`AgnusEvents.cpp:221`); `interruptLevel` serves bits 13 and 14 together behind the master bit (`Paula.cpp:163-169`). **Settable, reads back, level 6** |
| **Denise** | [`emulation/libami/paula/irq.cpp:17-18`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/irq.cpp#L17-L18), [`:138-140`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/irq.cpp#L138-L140), [`paula/paula.cpp:75-76`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L75-L76), [`agnus/register.cpp:43-44`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L43-L44), [`agnus/events.cpp:335`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L335) | the write masks only bit 15 (`irq.cpp:17-18`) on the path a write takes (`events.cpp:335`); a read of `$01E` returns `intreq` unmasked (`register.cpp:43-44`, `paula.cpp:75-76`); `prepareIpl` serves bit 14 with EXTER behind the master bit (`irq.cpp:138-140`). **Settable, reads back, level 6** |
| **Copperline** | [`src/chipset/paula.rs:26-29`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L26-L29), [`:51`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L51), [`:1145-1150`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1145-L1150), [`src/bus/custom_regs.rs:881`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L881), [`:906`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L906), [`:83-85`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L83-L85), [`src/chipset/paula.rs:2448-2452`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2448-L2452), [`src/cpu.rs:3486-3490`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L3486-L3490), [`src/chipset/paula.rs:4563-4564`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L4563-L4564), [`src/bus.rs:169-171`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L169-L171), [`src/bus.rs:7434-7449`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L7434-L7449) | its comment names the bit, the undocumented INT14 source (`paula.rs:26-29`); a write to `$09C` (`custom_regs.rs:881`, `:906`) masks with `0x7FFF` (`paula.rs:51`) and sets (`paula.rs:1145-1150`); a read of `$01E` returns `intreq` (`custom_regs.rs:83-85`); behind the master bit (`cpu.rs:3486-3490`) `pending_ipl` serves it with EXTER (`paula.rs:2448-2452`), which a unit test asserts as level 6 (`paula.rs:4563-4564`). `IRQ_SOURCE_BITS` leaves bit 14 out of the recognition-latency delay only (`bus.rs:169-171`), and `cpu_visible_intreq` passes it through (`bus.rs:7434-7449`). **Settable, reads back, level 6** |
| **MiSTer** | [`rtl/paula_intcontroller.v:132-134`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_intcontroller.v#L132-L134) | descendant — byte-identical to NanoMig, same line numbers, comments included |
| **TC64** | [`rtl/minimig/paula_intcontroller.v:132-134`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_intcontroller.v#L132-L134) | descendant — the same; `paula_intcontroller.v` is identical across all three trees throughout, confirmed by a full-file comparison |

All four let bit 14 be set as a request, read it back and serve it at level 6,
where Figure 7-4 puts INTEN. Each write mask strips the SET/CLR bit and nothing
else, and each priority table groups bit 14 with EXTER, as NanoMig's encoder
does. None of the four implements p. 280's parenthesis.

### What other Commodore documents say

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — **agrees with NanoMig**:

  > "When writing the interrupt flag register, if a register Bit selected with D0..D14 is high, the register Bit is set or cleared according to D15."
  <!-- corpus: paula-8364r7-dissection/4_4_txt.txt -->

  The 8364R7's interrupt flag register has a bit 14: 'Note, that for Bit 2, Bit 4, and Bit 14 of the interrupt flag register the "interrupt set inputs" are tied to GND.' No hardware source, like SOFT and COPER, but the write path sets any of D0..D14, so bit 14 is a request bit a write can set, as in NanoMig.

## Decision

**No RTL change.** If real Paula latches a written bit 14 and serves it at
level 6, p. 218's warning and p. 280's parenthesis are the defect: what they
describe is a bit no hardware raises, not a bit that cannot be requested.

Following the parenthesis literally in the RTL would mean `intreq[14] <= 1'b0;`
at line 134. Every one of the 16384 `INTREQ` patterns with bit 14 set would then
read back without it, so `P3-INTREQ-FULLY-WRITABLE` would go red, and the
change would remove the bit that `paula.v`'s note says *Unreal* needs (see
*Software impact*).

To settle it, a real machine is needed, not another reading: set bit 14 in
`INTENA` and in `INTREQ` with nothing else enabled, read `INTREQR`, and see
whether the 68000 takes a level-6 interrupt.
