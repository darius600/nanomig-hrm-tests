# NM-HRM-035 — any write to CIA-A clears the kickstart overlay, not only a write to the OVL bit [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should a byte write to CIA-A timer A low (`$BFE401`) leave `ovl` set in the OCS-A500 mode, as p. 223's "The OVL bit in one of the 8520 Chips disables the overlay" reads, where the RTL clears it on any CIA-A write? Closes when the RTL (`minimig.v:871-872`) takes `ovl` from the OVL bit and `OTHER-CIA-A-REGISTER-ALSO-CLEARS` turns green, or when an Amiga specialist rejects the change, in which case, if the rejection means p. 223 is wrong, this page is refiled against the printed page. |
| **Report to** | `NanoMig direct` — harbaum/NanoMig and MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | the release of `ovl` by a CIA-A write; measured in the OCS-A500 mode (`chipset_config` 0) with a byte write to timer A low. The same two lines serve the OCS-A1000, ECS and AGA modes, which were not measured; chip revision unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 7 — System Control Hardware, printed p. 223; Appendix E, printed p. 337; Appendix F, printed pp. 339, 341, 354; Chapter 1, the model list on printed p. 1 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `OVL-BIT-WRITE-CLEARS-OVERLAY` and `OTHER-CIA-A-REGISTER-ALSO-CLEARS` carry `sol:7-reset.c8` / `opus:7-reset.c4` (p. 223); `RESET-LINE-RESTORES-OVERLAY` carries `sol:7-reset.c12` / `opus:7-reset.c9` (p. 223); `OVL-AT-COLD-START` (`sol:7-reset.c3`, p. 223) is measured, not graded; `CONTROL-RESET-RELEASES`, `CIAB-WRITE-DOES-NOT-CLEAR` and `CHIPRAM-WRITE-DOES-NOT-CLEAR` carry no claim id |
| **Module** | `minimig.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L871-L872) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L933-L934); MiSTer has the same lines; TC64 takes `ovl` from CIA-A's port-A output (see *Independent evidence*): fixed in TC64 |
| **Test** | [`hrm/ch7-overlay-source`](../../hrm/ch7-overlay-source): `make -C hrm/ch7-overlay-source test`; an expected red exits nonzero |
| **Assertions** | `OTHER-CIA-A-REGISTER-ALSO-CLEARS`, red: expected `ovl` 1 after the timer A low write, observed 0; the package reports 10 passed, 1 failed. Its precondition line shares the label stem and is green; `CIAB-WRITE-DOES-NOT-CLEAR` is the control |
| **Verdict of the four** | **unanimous against NanoMig on every model the manual covers**: vAmiga, Denise and Copperline tie the overlay to the value of the OVL bit on every machine they model, vAmiga's and Copperline's A1200 included, and only a port-A register write can move it; **WinUAE splits by gate array**: every model without Gayle, the AGA A4000 included, does the same, while its Gayle-based A600 and A1200, its CD32 and its CDTV-CR release the overlay on the first write to any CIA-A register, as NanoMig does in every one of its modes. Hardware: not measured |
| **Effort** | S — `minimig.v:871-872` only, one assignment taken from CIA-A's existing port-A output; an A1200-mode release, if wanted, is a separate term |

---

## Source requirement

**Printed p. 223**, on leaving the post-reset memory map:

> "The OVL bit in one of the 8520 Chips disables the overlay (See Appendix F for the bit location)."

The sentence names **one bit**: OVL, which Appendix F (printed p. 339) places at
CIA-A port A bit 0, `$BFE001`. It names the chip only as "one of the 8520 Chips"
and does not name its address range.

**It is stated once, for every model the manual covers.** Printed p. 1:

> "There are four basic models that make up the Amiga computer family: the A500, A1000, A2000, and A3000."

The p. 223 paragraph that carries the OVL sentence separates the models where
they differ, and does so for the RAM, not for the OVL bit:

> "On some Amiga models, portions of the RAM still respond. On other models, no RAM responds."

Appendix E's port table (printed p. 337, as the page image prints it) does the
same: its PA1 row, "led light (0=bright) / audio filter control (A500 & A2000)",
carries a model qualifier, and its PA0 row, "OVL ROM/RAM overlay bit", carries
none. The A600, A1200, CD32 and CDTV-CR are not among the models the manual
covers.

Printed p. 341, on how the bit reaches the pin:

> "If a DDR bit is set to a 0, the corresponding PR bit is defined as an input."

Printed p. 354, on the reset input:

> "The port pins are set as inputs and port registers to zero (although a read of the ports will return all highs because of passive pull-ups)."

So after reset the OVL line reads high, only a write to PRA or DDRA can move it,
and a PRA write moves it only while DDRA bit 0 selects output. The address map
on p. 339 gives DDRA as "set to 0x03": PA0 and PA1 driven.

## RTL and measurement

NanoMig clears the overlay on **any** write to CIA-A, whatever register is
addressed and whatever data is written
([`minimig.v#L871-L872`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L871-L872)):

```verilog
	if(~_cpu_reset | ~_cpu_reset_in)       ovl <= 1;
	else if(sel_cia_a & (cpu_lwr|cpu_hwr)) ovl <= 0;
```

`cpu_address_out[11:8]`, which carries the CIA register number, is never
consulted, and neither is the data.

**The rule has no mode term.** NanoMig's four machine modes,
"OCS-A500(0), OCS-A1000(1), ECS(2) or AGA(3)"
([`src/misc/sysctrl.v:263-264`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/misc/sysctrl.v#L263-L264)),
reach `minimig.v` as `chipset_config`
([`:487-489`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L487-L489)),
and lines 861-862 read none of it: the any-write release is the same in the
OCS-A500, OCS-A1000 and ECS modes, machines the manual covers, as in the AGA
mode, the A1200, which it does not.

**NanoMig's own CIA-A computes the line the manual describes, and nothing uses
it.** [`ciaa.v:262`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/ciaa.v#L262)
drives `porta_out[3:0]` so that bit 0 is high while DDRA bit 0 selects input and
follows PRA bit 0 otherwise, as pp. 341 and 354 say; `minimig.v` keeps only bits
3-1 of it ([`:614-615`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L614-L615)).

### Measured

| | |
|---|---|
| configuration — `chipset_config` driven with 0 (`hrm/ch7-overlay-source/test_minimig.cpp:217`) | OCS, the **A500** mode |
| precondition — `ovl` before the write | **1** |
| after a byte write (lower data strobe) to CIA-A **timer A low** (`$BFE401`) | `ovl` reads **0** |
| the same test built without the write (mutation build `MUT_2`, `make -C hrm/ch7-overlay-source MUT=MUT_2 test`; a plain `make test` does not build it) | `ovl` stays **1** |
| a write to **CIA-B** (the control) | `ovl` stays **1** |

Timer A low has no relation to port A. The stimulus is not DDRA (`$BFE201`),
because DDRA sets the direction of the OVL pin itself and the manual says
nothing about the overlay's fate when that direction changes.

## Risks and unknowns

- **Only the A500 mode is measured.** The OCS-A1000, ECS and AGA modes share the
  code but were not run.
- **The A1200 mode is not decided by the manual.** There the four split: WinUAE
  releases the overlay on any CIA-A write, vAmiga and Copperline follow PA0, and
  Denise models no A1200. Whether the mode should keep the any-write release is a
  question about Gayle for someone who knows it; it is no reason to keep the
  release in the modes the manual covers.
- **The suggested change moves behaviour.** The overlay then follows the line in
  both directions, as in vAmiga, Denise and WinUAE's models without Gayle, where
  today it is only ever released. Boot code that writes some other CIA-A register
  before PRA would keep the overlay longer than it does today; whether any real
  software depends on the current behaviour, for instance a title that pokes a
  CIA-A timer while the overlay is still up, is unknown.
- **The two Commodore documents are indirect.** Gary's text says "typically" and
  does not name CIA-A PA0; the TRM drawing gives the overlay line no source other
  than port A bit 0, but the TRM's text does not say what drives or clears it.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1464-1477`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1464-L1477) | follows the effective pin (`pra` with `~dra`), and only on a change, when `cs_ciaoverlay` is set. That is the default ([`cfgfile.cpp:9864`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9864)); it is cleared for four models only: the CDTV-CR ([`:9920`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9920)), the CD32 ([`:9928`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9928)), the A600 ([`:9951`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9951)) and the A1200 ([`cfgfile.cpp:9975-9979`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9975-L9979)); the A4000, an AGA machine, keeps it ([`:10010-10021`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L10010-L10021)). The changelog gives the reason: "Pre-Gayle models use CIA overlay io pin, newer models have overlay logic inside Gayle" ([`od-win32/winuaechangelog.txt:14789-14790`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/winuaechangelog.txt#L14789-L14790)). With it cleared, `WriteCIAA` releases the overlay before it decodes the register: [`cia.cpp:2118-2121`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2118-L2121) |
| **vAmiga** | [`Core/Components/CIA/CIA.cpp:852-855`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L852-L855) | updates the memory map when PA bit 0 changes, on every configuration it offers, its A1200 scheme included ([`Core/Components/AmigaTypes.h:121`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/AmigaTypes.h#L121)): the overlay is `ciaa.getPA() & 1` ([`Core/Components/Memory/Memory.cpp:860`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L860)), with no configuration term |
| **Denise** | [`emulation/libami/system/system.cpp:70-72`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L70-L72) | sets the overlay from port A bit 0 when it changes, the line computed as `lines.pra \| ~lines.ddra` ([`cia/new/register.cpp:194`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L194)); it models no Gayle machine (`todo: gayle (A600, A1200)`, [`emulation/libami/agnus/memory.cpp:714`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L714)) |
| **Copperline** | [`src/chipset/cia.rs:689-695`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L689-L695) | only a write to `REG_PRA` or `REG_DDRA` can move the line; the condition names no machine model, and Copperline models an A1200 ([`src/config/mod.rs:2269`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/config/mod.rs#L2269)) |
| **MiSTer** | [`rtl/minimig.v:933-934`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L933-L934) | descendant — byte-identical, the same any-write clear |
| **TC64** | [`rtl/minimig/minimig.v`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig.v#L343) | descendant — no such lines: `ovl` is a plain `wire` ([`:343`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig.v#L343)), tied to CIA-A's own port-A output bit, `.porta_out({_fire1_dat,_fire0_dat,_led,ovl})` at [`:922`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/minimig.v#L922), which [`ciaa.v:238`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa.v#L238) computes from `regporta` gated by `ddrporta`: the effective-pin formula of the emulators above, with no mode term |

### What other Commodore documents say

- **Commodore, *A500/A2000 Gary Specification*, undated**, PDF page 1 — agrees with the manual:

  > "OVL (OVerLay) input causes ROM to be accessed at memory location 0. This signal is typically driven by a port line so that ROM is mapped to the memory 0 at RESET time."
  <!-- corpus: gary-spec/p001.txt -->

  Gary takes the overlay as a level input pin (pin table: 'OVL IN HI Overlay (ROM to address 0)') driven by a port line. None of Gary's inputs carries a CIA register select or a CIA write strobe, and the spec describes no latch, so the overlay follows the level of that port line. That sides with the manual's 'the OVL bit', not with a clear on any CIA-A write.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 301 — agrees with the manual:

  *Read from the drawing, not quoted:* the A500 main board's CIA-A (U7, an 8520) drives the net OVL from pin 2, PA0; beside it PA1 drives _LED and PA2-PA7 carry _CHNG, _WPROT, _TRK0, _RDY, _FIRE0 and _FIRE1.
  <!-- drawing: a500-a2000-trm-1987/p301.txt -->

  The overlay line is port A's bit 0 as an output, so it follows that bit's output level; the drawing gives it no other source.

## Decision

The question and its closure are in the header. Suggested change: take `ovl`
from CIA-A's own port-A output, which `ciaa.v:262` already computes and
`minimig.v:615` leaves unconnected, as TC64 does:

```verilog
	// p.223 names THE OVL bit -- CIA-A PA0 as the pin drives it (pp.339, 341, 354)
	always @(posedge clk) ovl <= porta_out[0];
```

CIA-A's reset input is `sys_reset | ~_cpu_reset_in`
([`minimig.v:427`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/minimig.v#L427)),
and reset clears DDRA, which drives the line high, so the overlay returns on
reset without a term of its own, as p. 354 has it.
