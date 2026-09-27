# NM-HRM-035 — any write to CIA-A clears the kickstart overlay, not only a write to the OVL bit [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `minimig.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L861-L862) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L933-L934) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch7-overlay-source`](../../hrm/ch7-overlay-source) |
| **Chapter** | 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | `OTHER-CIA-A-REGISTER-ALSO-CLEARS` — red. Its precondition line shares the label stem and is green; `CIAB-WRITE-DOES-NOT-CLEAR` is the control |
| **Verdict of the four** | **unanimous against NanoMig on every model the manual covers**: vAmiga, Denise and Copperline tie the overlay to the value of the OVL bit on every machine they model, vAmiga's and Copperline's A1200 included, and only a port-A register write can move it; **WinUAE splits by gate array**: every model without Gayle, the AGA A4000 included, does the same, while its Gayle-based A600 and A1200, its CD32 and its CDTV-CR release the overlay on the first write to any CIA-A register, as NanoMig does in every one of its modes |

---

## What the HRM says

**Printed p. 223**, on leaving the post-reset memory map:

> "The OVL bit in one of the 8520 Chips disables the overlay (See Appendix F for the bit location)."

The sentence names **one bit**: OVL, which Appendix F (printed p. 339) places at
CIA-A port A bit 0, `$BFE001`. It names the chip only as "one of the 8520 Chips",
and it does not name its address range.

**The sentence is stated once, for every model the manual covers.** Printed p. 1
names them:

> "There are four basic models that make up the Amiga computer family: the A500, A1000, A2000, and A3000."

The p. 223 paragraph that carries the OVL sentence separates the models where
they differ, and does so for the RAM, not for the OVL bit:

> "On some Amiga models, portions of the RAM still respond. On other models, no RAM responds."

Appendix E's port table (printed p. 337, as the page image prints it) does the
same: its PA1 row, "led light (0=bright) / audio filter control (A500 & A2000)",
carries a model qualifier, and its PA0 row, "OVL ROM/RAM overlay bit", carries
none. The A600, A1200, CD32 and CDTV-CR are not among the models the manual
covers, and it says nothing about them.

Appendix F says how the bit reaches the pin. Printed p. 341:

> "If a DDR bit is set to a 0, the corresponding PR bit is defined as an input."

Printed p. 354, on the reset input:

> "The port pins are set as inputs and port registers to zero (although a read of the ports will return all highs because of passive pull-ups)."

So after reset the OVL line reads high, only a write to PRA or DDRA can move it,
and a PRA write moves it only while DDRA bit 0 selects output. The address map
on p. 339 gives DDRA as "set to 0x03": PA0 and PA1 driven.

---

## What NanoMig does

It clears the overlay on **any** write to CIA-A, whatever register is addressed
and whatever data is written:

[`src/minimig-aga/minimig.v#L861-L862`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L861-L862)

```verilog
	if(~_cpu_reset | ~_cpu_reset_in)       ovl <= 1;
	else if(sel_cia_a & (cpu_lwr|cpu_hwr)) ovl <= 0;
```

`sel_cia_a` is the chip select. `cpu_address_out[11:8]`, which carries the CIA
register number, is never consulted, and neither is the data.

**The rule has no mode term.** NanoMig offers four machine modes, "OCS-A500(0),
OCS-A1000(1), ECS(2) or AGA(3)" ([`src/misc/sysctrl.v:228-229`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/misc/sysctrl.v#L228-L229)),
which reach `minimig.v` as `chipset_config`:
`.a1k(chipset_config[2])`, `.ecs(|chipset_config[4:3])`, `.aga(chipset_config[4])` ([`:482-484`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L482-L484)).
Lines 861-862 read none of it, so the any-write release is the same in the
OCS-A500, OCS-A1000 and ECS modes, machines the manual covers, as in the AGA
mode, the A1200, which it does not.

**NanoMig's own CIA-A computes the line the manual describes, and nothing uses
it.** [`src/minimig-aga/ciaa.v:262`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L262) drives `porta_out[3:0] = {(~ddrporta[7:6] | regporta[3:2]), (~ddrporta[1:0] | regporta[1:0])}`,
so its bit 0 is high while DDRA bit 0 selects input and follows PRA bit 0 otherwise,
as pp. 341 and 354 say; `minimig.v` keeps only bits 3-1 of it:
[`:604-605`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L604-L605) `assign {_fire1_dat,_fire0_dat,_led} = porta_out[3:1];`.

### Measured

| | |
|---|---|
| configuration — `chipset_config` driven with 0 ([`test_minimig.cpp:234`](../../hrm/ch7-overlay-source/test_minimig.cpp#L234)) | OCS, the **A500** mode |
| precondition — `ovl` before the write | **1** |
| after a byte write (lower data strobe) to CIA-A **timer A low** (`$BFE401`) | `ovl` reads **0** |
| the mutation — the same test built with that write left out, no CIA-A access at all | `ovl` stays **1** |
| the control — a write to **CIA-B** | `ovl` stays **1** |

Timer A low is a register with no relation to port A whatsoever. On the manual's
wording the overlay must survive it.

---

## Why the stimulus is a timer register

The stimulus is not **CIA-A DDRA** (`$BFE201`), although DDRA too is "CIA-A, but
not the OVL bit": DDRA is the **direction** register for port A, including the
OVL bit itself, so a write to it changes what the OVL pin drives, and the manual
says nothing about the overlay's fate when a bit's direction changes.

The emulators say the same thing in their own code — WinUAE folds
`dra` into the effective pin, vAmiga computes `(internal & ddra) | (external &
~ddra)`, Denise computes `lines.pra | ~lines.ddra`
([`cia/new/register.cpp:194`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L194)),
and Copperline lists `REG_DDRA` beside `REG_PRA` as the two registers that may
move the overlay line. Timer A low has no relation to port A.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a local checkout
at the pinned revision named in the link.

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1464-1477`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1464-L1477) | <pre>uae_u8 v = cia[0].pra;<br>v \|= ~cia[0].dra;<br>if (currprefs.cs_ciaoverlay && (v & 1) != oldovl) { ... map_overlay ... }</pre> the effective pin, and only on a change, when `cs_ciaoverlay` is set. It is set by default ([`cfgfile.cpp:9864`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9864)) and cleared for four models only: the CDTV-CR ([`:9920`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9920)), the CD32 ([`:9928`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9928)), the A600 ([`:9951`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9951)) and the A1200 ([`cfgfile.cpp:9975-9979`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9975-L9979)); the A4000, an AGA machine, keeps it ([`:10010-10021`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L10010-L10021)). WinUAE's changelog gives the reason: "Pre-Gayle models use CIA overlay io pin, newer models have overlay logic inside Gayle" ([`od-win32/winuaechangelog.txt:14789-14790`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/winuaechangelog.txt#L14789-L14790)). With it cleared, the top of `WriteCIAA`, before the register is decoded, releases the overlay on any CIA-A write: [`cia.cpp:2118-2121`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2118-L2121) <pre>if (!currprefs.cs_ciaoverlay && oldovl) {<br>&nbsp;&nbsp;&nbsp;&nbsp;map_overlay(1);<br>&nbsp;&nbsp;&nbsp;&nbsp;oldovl = 0;<br>}</pre> |
| **vAmiga** | [`Core/Components/CIA/CIA.cpp:852-855`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L852-L855) | <pre>// Check the OVL bit which controls the Kickstart ROM overlay<br>if ((oldpa ^ pa) & 0b00000001) mem.updateMemSrcTables();</pre> the same on every configuration it offers, its A1200 scheme included ([`Core/Components/AmigaTypes.h:121`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/AmigaTypes.h#L121)): the overlay is `ciaa.getPA() & 1` ([`Core/Components/Memory/Memory.cpp:860`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L860)), with no configuration term |
| **Denise** | [`emulation/libami/system/system.cpp:70-72`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L70-L72) | <pre>if (port == PORTA) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if ((lines-&gt;ioa ^ lines-&gt;ioaOld) & 1)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;agnus.setOVL(lines-&gt;ioa & 1);</pre> Denise models no Gayle machine: its memory map leaves Gayle as `todo: gayle (A600, A1200)` ([`emulation/libami/agnus/memory.cpp:714`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L714)) |
| **Copperline** | [`src/chipset/cia.rs:689-695`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L689-L695) | <pre>if self.which == Which::A && matches!(reg, REG_PRA \| REG_DDRA) {<br>&nbsp;&nbsp;&nbsp;&nbsp;let now_no_overlay = self.cia_a_no_overlay_line();<br>&nbsp;&nbsp;&nbsp;&nbsp;if !prev_no_overlay && now_no_overlay { effect.disable_overlay = true; }<br>}</pre> the condition names no machine model, and Copperline models an A1200 ([`src/config/mod.rs:2269`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/config/mod.rs#L2269)) |
| **MiSTer** | [`rtl/minimig.v:933-934`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L933-L934) | descendant — byte-identical: `if(~_cpu_reset \| ~_cpu_reset_in) ovl <= 1; else if(sel_cia_a & (cpu_lwr\|cpu_hwr)) ovl <= 0;`, the same any-write clear |
| **TC64** | `rtl/minimig/minimig.v` | descendant — `rtl/minimig/minimig.v` has no such lines at all — `ovl` is a plain `wire` (`:343`), tied directly to CIA-A's own port-A output bit: `.porta_out({_fire1_dat,_fire0_dat,_led,ovl})` at `:922`. That output is computed in `ciaa.v` from `regporta` gated by `ddrporta` — the effective-pin formula vAmiga, Denise, Copperline and WinUAE's models without Gayle use, not an any-write rule. TC64 is itself an AGA line, and the wire carries no mode term. A fix in `minimig.v` meets MiSTer where NanoMig already is; TC64 has already moved this decision out of `minimig.v` entirely, to a place where it tracks the OVL bit's value by construction |

**vAmiga, Denise and Copperline follow the OVL bit's value on every machine they
model, and so does WinUAE on every model without Gayle, the AGA A4000
included.** None of those clears the overlay because a write happened to land
somewhere in the chip, and Copperline states the restriction explicitly by
naming the only two registers that may move the line. WinUAE's Gayle-based A600
and A1200, its CD32 and its CDTV-CR do clear it that way: the first write to any
CIA-A register, whatever its value, releases the overlay, which is NanoMig's
behaviour; WinUAE's own changelog places the overlay logic of the Gayle models
in Gayle, not in the CIA. **So the four agree with the manual on every model it
covers, and split only on machines it does not cover**: on the A1200, WinUAE
releases the overlay on any CIA-A write, while vAmiga and Copperline follow PA0.
TC64 reaches the effective-pin formula by construction — its `ovl` is not a
register a write clears at all, but a wire computed the way vAmiga, Denise and
Copperline compute it — while MiSTer carries the same any-write clear NanoMig
has, unchanged.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Gary Specification*, undated**, PDF page 1 — agrees with the manual:

  > "OVL (OVerLay) input causes ROM to be accessed at memory location 0. This signal is typically driven by a port line so that ROM is mapped to the memory 0 at RESET time."
  <!-- corpus: gary-spec/p001.txt -->

  Gary takes the overlay as a level input pin (pin table: 'OVL IN HI Overlay (ROM to address 0)') driven by a port line. None of Gary's inputs carries a CIA register select or a CIA write strobe, and the spec describes no latch, so the overlay follows the level of that port line. That sides with the manual's 'the OVL bit', not with a clear on any CIA-A write. It does not name CIA-A PA0 itself, and it says 'typically'.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 301 — agrees with the manual:

  *Read from the drawing, not quoted:* the A500 main board's CIA-A (U7, an 8520) drives the net OVL from pin 2, PA0; beside it PA1 drives _LED and PA2-PA7 carry _CHNG, _WPROT, _TRK0, _RDY, _FIRE0 and _FIRE1.
  <!-- drawing: a500-a2000-trm-1987/p301.txt -->

  The overlay line is port A's bit 0 as an output, so it follows that bit's output level; the drawing gives it no other source. The TRM's own text does not say what drives or clears the line.

Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; Commodore, 6526 CIA specification, 1981; US patent 4,777,621, Commodore-Amiga, filed 1985, granted 1988.

---

## Suggested change

Take `ovl` from CIA-A's own port-A output, which `ciaa.v:262` already computes
and `minimig.v:605` leaves unconnected; TC64 takes its `ovl` from the same
output:

```verilog
	// p.223 names THE OVL bit -- CIA-A PA0 as the pin drives it (pp.339, 341, 354)
	always @(posedge clk) ovl <= porta_out[0];
```

CIA-A's reset input is `sys_reset | ~_cpu_reset_in` ([`minimig.v:422`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L422)),
and reset clears DDRA, which drives the line high, so the overlay returns on
reset without a term of its own, as p. 354 has it.

Four cautions. The overlay then follows the line in both directions, as it does
in vAmiga, Denise and WinUAE's models without Gayle, where today it is only ever
released. Boot code that writes some other CIA-A register before PRA would keep
the overlay longer than it does today; whether any real software depends on the
current behaviour is a question for someone who knows the boot sequence. If the
A1200 mode is to keep the release WinUAE gives Gayle machines, that is a separate
term gated on the mode (`chipset_config[4]`); nothing in the manual asks for it.
And two other assertions of the same package read red after this change,
`OVL-BIT-WRITE-CLEARS-OVERLAY` and the precondition of `RESET-LINE-RESTORES-OVERLAY`:
both write `$00` to PRA while DDRA still holds its reset value `$00`, a write
that p. 341 says leaves the OVL line an input. They need DDRA bit 0 set first.

---

## What holds the finding up

- **The control is on the same fixture and the same measurement basis**: a write
  to CIA-B, a different chip, leaves `ovl` set. So the clear is not "any bus
  write at all".
- **The precondition is asserted, not assumed**: `ovl == 1` is checked before the
  write, so the red is a transition and not a stuck value.
- **The mutation moves it**: with no CIA-A access at all the assertion reads
  green, so the FAIL is attributable to the write happening.
- **The stimulus is unrelated to port A**: a timer register, neither the OVL
  bit nor its direction.

---

## For the specialists

NanoMig's rule is the one WinUAE gives its Gayle machines, and NanoMig applies
it in every mode. In the OCS-A500, OCS-A1000 and ECS modes NanoMig is a machine
the manual describes, and there all four independent implementations follow the
OVL bit; the test measures the A500 mode. In the A1200 mode the manual does not
decide, and the four split: WinUAE releases the overlay on any CIA-A write,
vAmiga and Copperline follow PA0, and Denise models no A1200. Whether the A1200
mode should keep the any-write release is a question about Gayle for someone who
knows it; it is not a reason to keep the release in the modes the manual
covers. It is also worth knowing whether any real title pokes a CIA-A timer
while the overlay is still up.
