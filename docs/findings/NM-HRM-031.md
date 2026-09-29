# NM-HRM-031 — INTREQ bit 14 is a settable request bit where the appendix says enable-only

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test passes; the page's other reading fails |
| **What is asked of a reader** | No RTL change follows from this. p. 218's warning and p. 280's parenthesis say bit 14 creates no interrupt request; Figure 7-4 on p. 221 gives INTEN hardware priority 6. NanoMig makes bit 14 a request that a write sets and level 6 serves, and so do all four independent implementations. The printed parenthesis is the likely defect, and the entry exists so the next reader does not take the measured bit for an implementation fault. |
| **Module** | `paula_intcontroller.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L132-L134) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_intcontroller.v#L132-L134) |
| **Report to** | **nobody, as an RTL defect.** The question is what p. 218's warning and p. 280's parenthesis mean; the RTL's bit 14 is the one Figure 7-4 places at level 6, inherited unchanged by both Minimig lines |
| **Test** | [`hrm/ch5-audio-interrupt`](../../hrm/ch5-audio-interrupt) · `MASTER-BIT14-NO-REQUEST`, measured and recorded, not graded; [`hrm/ch7-interrupt-priority-encoder`](../../hrm/ch7-interrupt-priority-encoder) · `P3-INTREQ-FULLY-WRITABLE`, graded and green, and `P19-BIT14-INTEN`, measured, not graded |
| **Chapter** | Appendix A, printed p. 280; 7 — System Control, printed pp. 218, 219 and 221 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | none red. `MASTER-BIT14-NO-REQUEST` and `P19-BIT14-INTEN` are measured and not graded; `P3-INTREQ-FULLY-WRITABLE` is graded from p. 219 and green |
| **Verdict of the four** | **unanimous with NanoMig and with Figure 7-4**: WinUAE, vAmiga, Denise and Copperline all latch a written bit 14 in INTREQ, return it through INTREQR, and serve it at level 6 when INTENA's bit 14 is set. None implements p. 280's parenthesis |

---

## What the HRM says

**Appendix A, printed p. 280**, in the `INTREQ` / `INTENA` bit table:

> `14  INTEN  Master interrupt (enable only, no request)`

**Chapter 7, printed p. 218**, says the same in prose, and adds a warning:

> Bit 14 of the interrupt registers (INTEN) is for interrupt enable. This is the
> master interrupt enable bit. If this bit is a 0, it disables all other
> interrupts. You may wish to clear this bit to temporarily disable all
> interrupts to do some critical processing task.
>
> Warning: This bit is used for enable/disable only. It creates no interrupt request.

The same page describes the register a write sets, and which bits a write
selects:

> Interrupt request (status) - write only. Used by the processor to force a
> certain kind of interrupt to be processed (software interrupt). Also used to
> clear interrupt request flags once the interrupt process is completed.
>
> Positions 14-0 are bit selectors. You write a 1 to any one or more bits to
> select that bit.

**Printed p. 219**, of the Copper's bit in the same register:

> The Copper can change the content of any of the bits of this register, as it
> can write any value into most of the machine registers.

**Figure 7-4, printed p. 221**, *Interrupt Priorities*, puts INTEN in a hardware
priority bracket. Its last three rows, read from the figure, not quoted:

| hardware priority | Exec software priority | description | |
|---|---|---|---|
| 6 | 14 | external INT6 & CIAB | EXTER |
| 6 | 15 | special (master enable) | INTEN |
| 7 | -- | non-maskable interrupt | NMI |

**The manual says two things about bit 14.** p. 280 and p. 218's warning say it
creates no request. Figure 7-4 gives INTEN the hardware priority of EXTER beside
it, 6, and an Exec software priority of its own: a place in the order in which
pending requests are served. p. 218 calls positions 14-0 bit selectors and
p. 219 says the Copper can change any bit of the request register, and neither
excepts bit 14. Nothing on these pages says which is meant: "no request" can be
read as *there is no request bit*, or as *no hardware raises it*.

---

## What NanoMig does

It latches a request bit at 14 like any other, and the module says so in its own
comment:

[`src/minimig-aga/paula_intcontroller.v#L130-L134`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L130-L134)

```verilog
	  	intreq[12] <= tmp[12] | syncint;
	  	//external interrupt
	  	intreq[13] <= tmp[13] | int6;
	  	//undocumented interrupt
	  	intreq[14] <= tmp[14];
```

`INTREQR` returns it with the other fourteen:

[`src/minimig-aga/paula_intcontroller.v#L74-L75`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L74-L75)

```verilog
	if (reg_address_in[8:1]==INTREQR[8:1])
		intreqr[15:0] = {1'b0,intreq[14:0]};
```

and once `INTENA`'s bit 14 opens the gate, the priority encoder serves it first,
with EXTER, at level 6 (`_ipl` is active low, so `1` is level 6):

[`src/minimig-aga/paula_intcontroller.v#L143-L144`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L143-L144),
[`#L152-L154`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L152-L154)

```verilog
	if (intena[14])
		intreqena[14:0] = intreq[14:0] & intena[14:0];
...
	  casez (intreqena[14:0])
	  	15'b1?????????????? : _ipl <= 1;
	  	15'b01????????????? : _ipl <= 1;
```

That is Figure 7-4's placement: bit 14 in the level-6 bracket, beside EXTER.

### Measured

| | |
|---|---|
| a `SET` write to `INTREQ` naming only bit 14 | bit 14 reads back **set** in `INTREQR`, `0x4000` |
| the control — bit 14 read back when never written | returns `0x0000`, so the fixture is live and the readback is not stuck high |
| bit 14 set in both `INTENA` and `INTREQ`, nothing else pending | `INTREQR` reads `0x4000` and the module drives 68000 level 6 |
| each of the 32768 `INTREQ` patterns written with every enable on, bit 14 among them | every one reads back unchanged, 0 mismatches |

The first two rows are `hrm/ch5-audio-interrupt`'s `MASTER-BIT14-NO-REQUEST`,
reported and not graded; the package records why in
`MASTER-BIT14-NO-REQUEST-NOT-GRADED` and reads `9 passed, 0 failed`. The third
is `P19-BIT14-INTEN` in `hrm/ch7-interrupt-priority-encoder`, also measured and
not graded, because the manual contradicts itself on this bit. The fourth is
`P3-INTREQ-FULLY-WRITABLE` in the same package, graded from p. 219 and
**green**; that package reads `22 passed, 0 failed`.

The enable half of the same appendix line is **green**: bit 14 does gate the
other enables, exactly as the manual says (`MASTER-ENABLE-BIT14-GATE`). The
"no request" half does not hold, and nothing in the suite grades it.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a checkout at
the pinned revision and the quotation verified there.

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:7311`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L7311), [`:3295-3298`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3295-L3298), [`:832-839`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L832-L839), [`:2264-2267`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2264-L2267), [`:3213-3216`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3213-L3216), [`:3113-3117`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3113-L3117), [`newcpu.cpp:4655`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L4655) | <pre>if (val & 0x8000) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&#42;p \|= val & 0x7FFF;</pre> A write to `$09C` reaches `setclr(&intreq, v);` through `INTREQ_0`, and the mask strips bit 15 and nothing else — bit 14 is settable. `INTREQR` returns `return intreq & 0x7fff;`, bit 14 included. The delayed copy the CPU sees is set the same way, `setclr(&intreq2, v);`, and `intlev` serves bit 14 with EXTER, `if (imask & (0x4000 \| 0x2000))` / `return 6;`, commented `// 13 14`; `doint` hands that level to the CPU, `int ipl = intlev();`. **Bit 14 is settable, reads back, and interrupts at level 6** |
| **vAmiga** | [`Core/Components/Paula/PaulaRegs.cpp:92-93`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L92-L93), [`PaulaRegs.cpp:57-62`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L57-L62), [`:70-80`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L70-L80), [`Core/Components/Agnus/AgnusEvents.cpp:221`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusEvents.cpp#L221), [`Paula.cpp:163-169`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Paula.cpp#L163-L169) | <pre>if (setclr) {<br>&nbsp;&nbsp;&nbsp;&nbsp;intreq \|= value;</pre> the caller, [`Paula.h:263`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Paula.h#L263), splits off bit 15 with `value & 0x7FFF` and masks **only** that — bit 14 is settable. With a 68000 or 68010 the write is deferred and applied as `case Reg::INTREQ: paula.setINTREQ(change.value); break;`, through the same caller. `peekINTREQR` returns `intreq` unmasked, and `interruptLevel` serves bits 13 and 14 together behind the master bit, `if (intena & 0x4000) {` … `if (mask & 0b0110000000000000) return 6;`. **Bit 14 is settable, reads back, and interrupts at level 6** |
| **Denise** | [`emulation/libami/paula/irq.cpp:17-18`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/irq.cpp#L17-L18), [`:138-140`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/irq.cpp#L138-L140), [`paula/paula.cpp:75-76`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L75-L76), [`agnus/register.cpp:43-44`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L43-L44), [`agnus/events.cpp:335`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/events.cpp#L335) | <pre>if (value & 0x8000)<br>&nbsp;&nbsp;&nbsp;&nbsp;intreq \|= value & 0x7fff;</pre> masks **only** bit 15, on the path a write takes, `case INTREQ: paula.setIntreq(data); break;`. A read of `$01E` returns `return paula.getIntreq();`, which is `intreq` unmasked, and `prepareIpl` serves bit 14 with EXTER behind the master bit, `if (intMask && (intena & 0x4000)) {` / `if (intMask & (0x4000 \| 0x2000))` / `level = 6;`. **Bit 14 is settable, reads back, and interrupts at level 6** |
| **Copperline** | [`src/chipset/paula.rs:26-29`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L26-L29), [`:51`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L51), [`:1145-1150`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1145-L1150), [`src/bus/custom_regs.rs:881`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L881), [`:906`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L906), [`:83-85`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L83-L85), [`src/chipset/paula.rs:2448-2452`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2448-L2452), [`src/cpu.rs:3486-3490`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L3486-L3490), [`src/chipset/paula.rs:4563-4564`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L4563-L4564) | its own comment names the bit: `The same bit is also latchable in INTREQ as the undocumented INT14 source.` A write to `$09C` reaches `let asserted = self.paula.write_intreq(val);`, which masks with `const INTREQ_MASK: u16 = 0x7FFF;` and sets with `self.intreq \|= bits;`; a read of `$01E` returns `self.paula.intreq // INTREQR`; and behind `if bus.paula.intena & INT_MASTER == 0 {`, `pending_ipl` serves it with EXTER, `if pending & (INT_INT14 \| (1 << 13)) != 0 {` → `6`, which a unit test asserts: `assert_eq!(pending_ipl(INT_INT14), 6);`. `IRQ_SOURCE_BITS: u16 = 0x3FFF` ([`src/bus.rs:169-171`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L169-L171)) leaves bit 14 out of the recognition-latency delay only; `cpu_visible_intreq` ([`src/bus.rs:7434-7449`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L7434-L7449)) passes it through. **Bit 14 is settable, reads back, and interrupts at level 6** |
| **MiSTer** | [`rtl/paula_intcontroller.v:132-134`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_intcontroller.v#L132-L134) | descendant — byte-identical to NanoMig, same line numbers, comments included |
| **TC64** | [`rtl/minimig/paula_intcontroller.v:132-134`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_intcontroller.v#L132-L134) | descendant — the same, byte-identical; `paula_intcontroller.v` is identical across all three trees throughout, confirmed by a full-file comparison. The settable bit-14 request is inherited unchanged by both lines |

**All four let bit 14 be set as a request, read it back, and serve it at level
6**, where Figure 7-4 puts INTEN. Each write mask is a precise statement —
WinUAE's, vAmiga's and Copperline's `0x7FFF` and Denise's `0x7fff` strip the
SET/CLR bit and nothing else — and each priority table groups bit 14 with
EXTER, as NanoMig's encoder does. Copperline gives its reason in words, calling
the bit "the undocumented INT14 source"; NanoMig's own comment calls it
"undocumented interrupt". None of the four implements p. 280's parenthesis.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **6502.org dissection of Paula 8364R7, 2023 — third-party, the silicon of one revision**, an author's note in the archive — **agrees with NanoMig**:

  > "When writing the interrupt flag register, if a register Bit selected with D0..D14 is high, the register Bit is set or cleared according to D15."
  <!-- corpus: paula-8364r7-dissection/4_4_txt.txt -->

  The 8364R7's interrupt flag register has a bit 14: 'Note, that for Bit 2, Bit 4, and Bit 14 of the interrupt flag register the "interrupt set inputs" are tied to GND.' No hardware source, like SOFT and COPER, but the write path sets any of D0..D14, so bit 14 is a request bit a write can set, as in NanoMig. Readback through INTREQR is not stated in the note.

Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

---

## Why this is filed against the page

The manual contradicts itself, and the RTL takes the side of the figure. p. 280
and p. 218's warning say bit 14 creates no request; Figure 7-4 serves INTEN at
level 6, and p. 218's bit selectors and p. 219's "any of the bits" do not except
it. NanoMig builds the bit Figure 7-4 describes, and so does every independent
implementation read here, each with the same mask and the same level. The
8364R7 dissection finds a bit 14 in the flag register that a write sets. Nothing
read here implements "no request".

The two voices meet if "no request" means *no hardware raises it*. NanoMig has
three such bits, SOFT, COPER and INTEN, each set only by a write
([`#L110`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L110),
[`#L114`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L114),
[`#L134`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_intcontroller.v#L134)),
and the 8364R7 note ties the set inputs of the same three to ground. p. 218's
"enable/disable only" does not bend that far, which is why the page, and not
the RTL, is what is in question.

The finding key names the pair that disagrees, NanoMig and p. 280, and does not
prejudge which side is wrong.

---

## Suggested change

Not an RTL change. If real Paula latches a written bit 14 and serves it at
level 6, p. 218's warning and p. 280's parenthesis are the defect: what they
describe is a bit no hardware raises, not a bit that cannot be requested.

Following the parenthesis literally in the RTL would mean

```verilog
	  	intreq[14] <= 1'b0;
```

at line 134. Every one of the 16384 `INTREQ` patterns with bit 14 set would
then read back without it, so `P3-INTREQ-FULLY-WRITABLE` in
`hrm/ch7-interrupt-priority-encoder` would go red, and the change would remove
the bit that `paula.v`'s notes say *Unreal* needs:

[`src/minimig-aga/paula.v#L53`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L53) — `implementation of intreq[14] (Unreal needs it)`

---

## What holds the finding up

- **NanoMig's reading is graded, and green.** `P3-INTREQ-FULLY-WRITABLE` writes
  all 32768 `INTREQ` patterns, bit 14 set in half of them, and reads each one
  back unchanged. A bit 14 that took no request would fail it on every one of
  those.
- **The other reading is measured, with its own control on the same fixture**:
  bit 14 read back without ever being written returns `0x0000`, and after a
  `SET` write naming only bit 14 it returns `0x4000`, so a set readback is the
  write arriving and not a stuck bit.
- **The level is Figure 7-4's.** With bit 14 set in both `INTENA` and `INTREQ`
  and nothing else pending, the module drives level 6 (`P19-BIT14-INTEN`).
- **The other half of p. 280's line passes.** `MASTER-ENABLE-BIT14-GATE` is
  green, so the fixture reaches `INTENA`/`INTREQ`, and bit 14 is the master
  enable the manual describes.
- **Nothing in the suite asserts "no request".** The disputed half is recorded,
  not graded, in `hrm/ch5-audio-interrupt`
  (`MASTER-BIT14-NO-REQUEST-NOT-GRADED`) and in both chapter-7 interrupt
  packages (`P19-BIT14-INTEN` in `hrm/ch7-interrupt-priority-encoder`,
  `P19-INTEN-SELF-REQUEST-DISPUTED` in `hrm/ch7-interrupt-register-semantics`).

---

## For the specialists

The interesting possibility is that the manual is incomplete rather than NanoMig
wrong, and every implementation read here points that way: four independent
emulators latch a written bit 14, read it back and serve it at level 6. The
8364R7 dissection agrees that a write sets the bit, on one revision of the
silicon; it states neither the readback through `INTREQR` nor the level.
Settling it needs a real machine, not another reading: set bit 14 in `INTENA`
and in `INTREQ` with nothing else enabled, read `INTREQR`, and see whether the
68000 takes a level-6 interrupt.
