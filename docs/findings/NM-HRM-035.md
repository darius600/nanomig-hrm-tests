# NM-HRM-035 — any write to CIA-A clears the kickstart overlay, not only a write to the OVL bit

| | |
|---|---|
| **Status** | open · red |
| **Module** | `minimig.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/minimig.v#L861-L862) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L933-L934) |
| **Upstream** | inherited — the two lines are byte-identical to upstream |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's, and a fix there reaches NanoMig |
| **Test** | [`hrm/ch7-overlay-source`](../../hrm/ch7-overlay-source) |
| **Chapter** | 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | `OTHER-CIA-A-REGISTER-ALSO-CLEARS` — red. Its precondition line shares the label stem and is green; `CIAB-WRITE-DOES-NOT-CLEAR` is the control |
| **Verdict of the four** | **all four tie the overlay to the value of the OVL bit**; three say in code that only port-A registers can move it |

---

## What the HRM says

**Printed p. 223**, on leaving the post-reset memory map:

> "THE OVL bit ... disables the overlay"

The sentence names **one bit**: OVL, which is CIA-A port A bit 0 at `$BFE001`. It
does not name the chip, and it does not name its address range.

---

## What NanoMig does

It clears the overlay on **any** write to CIA-A, whatever register is addressed
and whatever data is written:

[`src/minimig-aga/minimig.v#L861-L862`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/minimig.v#L861-L862)

```verilog
	if(~_cpu_reset | ~_cpu_reset_in)       ovl <= 1;
	else if(sel_cia_a & (cpu_lwr|cpu_hwr)) ovl <= 0;
```

`sel_cia_a` is the chip select. `cpu_address_out[11:8]`, which carries the CIA
register number, is never consulted, and neither is the data.

### Measured

| | |
|---|---|
| precondition — `ovl` before the write | **1** |
| after a word write to CIA-A **timer A low** (`$BFE401`) | `ovl` reads **0** |
| the mutation — the same run with no CIA-A access at all | `ovl` stays **1** |
| the control — a write to **CIA-B** | `ovl` stays **1** |

Timer A low is a register with no relation to port A whatsoever. On the manual's
wording the overlay must survive it.

---

## The stimulus this finding nearly rested on

The assertion originally wrote **CIA-A DDRA** (`$BFE201`), reasoning that DDRA is
"CIA-A, but not the OVL bit". The independent validation rejected that: DDRA is
the **direction** register for port A, including the OVL bit itself, so a write
to it changes what the OVL pin drives, and the manual says nothing about the
overlay's fate when a bit's direction changes.

Three of the four emulators say the same thing in their own code — WinUAE folds
`dra` into the effective pin, vAmiga computes `(internal & ddra) | (external &
~ddra)`, and Copperline lists `REG_DDRA` beside `REG_PRA` as the two registers
that may move the overlay line. The stimulus is now a timer register, and the red
survived the change.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a local checkout
at the pinned revision named in the link.

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1464-1477`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1464-L1477) | <pre>uae_u8 v = cia[0].pra;<br>v \|= ~cia[0].dra;<br>if (currprefs.cs_ciaoverlay && (v & 1) != oldovl) { ... map_overlay ... }</pre> the effective pin, and only on a change |
| **vAmiga** | [`Core/Components/CIA/CIA.cpp:852-855`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L852-L855) | <pre>// Check the OVL bit which controls the Kickstart ROM overlay<br>if ((oldpa ^ pa) & 0b00000001) mem.updateMemSrcTables();</pre> |
| **Denise** | [`emulation/libami/system/system.cpp:70-72`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L70-L72) | <pre>if (port == PORTA) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if ((lines-&gt;ioa ^ lines-&gt;ioaOld) & 1)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;agnus.setOVL(lines-&gt;ioa & 1);</pre> |
| **Copperline** | [`src/chipset/cia.rs:689-695`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L689-L695) | <pre>if self.which == Which::A && matches!(reg, REG_PRA \| REG_DDRA) {<br>&nbsp;&nbsp;&nbsp;&nbsp;let now_no_overlay = self.cia_a_no_overlay_line();<br>&nbsp;&nbsp;&nbsp;&nbsp;if !prev_no_overlay && now_no_overlay { effect.disable_overlay = true; }<br>}</pre> |

**All four follow the OVL bit's value.** None of them clears the overlay because
a write happened to land somewhere in the chip, and Copperline states the
restriction explicitly by naming the only two registers that may move the line.

---

## Suggested change

Qualify the clear with the register select and the data, the way the port-A
write is decoded elsewhere:

```verilog
	// p.223 names THE OVL bit -- CIA-A PRA bit 0 -- not the chip
	else if(sel_cia_a & (cpu_lwr|cpu_hwr) & (cpu_address_out[11:8] == 4'h0))
	                                       ovl <= cpu_data_out[0];
```

Two cautions. The condition above also changes the *polarity* semantics — today
any write clears, and the manual's bit says the overlay follows the bit — so the
two halves should be considered separately. And boot code that happens to write
some other CIA-A register before writing PRA would keep the overlay longer than
it does today; whether any real software depends on the current behaviour is a
question for someone who knows the boot sequence.

---

## What holds the finding up

- **The control is on the same fixture and the same measurement basis**: a write
  to CIA-B, a different chip, leaves `ovl` set. So the clear is not "any bus
  write at all".
- **The precondition is asserted, not assumed**: `ovl == 1` is checked before the
  write, so the red is a transition and not a stuck value.
- **The mutation moves it**: with no CIA-A access at all the assertion reads
  green, so the FAIL is attributable to the write happening.
- **The stimulus survived being attacked.** Its first form was rejected as not
  genuinely unrelated; the replacement is a timer register, and the result did
  not change.

---

## For the specialists

The likely reading is that this is a deliberate simplification: real boot code
writes PRA to release the overlay and nothing else needs the distinction, so
decoding the register number buys nothing on a machine that only ever boots one
way. That is an argument for leaving it alone, not an argument that it matches
the manual — and it is worth knowing whether any real title pokes a CIA-A timer
while the overlay is still up.
