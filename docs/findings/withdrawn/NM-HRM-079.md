# NM-HRM-079 — a write to the TOD counter's middle byte leaves the clock running; p. 344 stops it on a write to "the register" and does not say which

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | p. 344's stop sentence names no register and its next sentence restarts the clock after a write to $8, and `TOD-WRITE-STOPS-CLOCK` measures NanoMig stopped after a write to $A and counting after a lone write to $8 or to $9 |
| **What is asked of a reader** | Nothing. p. 344 names no register for the stop, and its next sentence makes a lone write to $8 end with the clock counting. |
| **Withdrawal reason** | p. 344 stops the clock on a write to "the register" and names none, so no page requires a write to $9 to stop it, and the next sentence rules out the claim's requirement that a lone write to $8 leave it stopped |
| **Error class** | **a silence read as a requirement** |
| **Replacement** | Green, in [`hrm/appF-cia-tod-clock-and-alarm`](../../../hrm/appF-cia-tod-clock-and-alarm): `TOD-WRITE-STOPS-CLOCK` (a write to $A stops a clock started by a write to $8 and seen to count; a lone write to $8 leaves it counting; a write to $9 is measured and not graded, because p. 344 names no register for the stop), `TOD-WRITE-STARTS-LSB-ONLY` and `TOD-WRITE-STARTS-ATOMIC-VALUE` |
| **Module** | `cia_timerd.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerd.v#L68-L80) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timerd.v#L81-L96); inherited (MiSTer has the same block; TC64 has the same stop and differs in the restart and the reset, see *Independent evidence*) |
| **Test** | `make -C hrm/appF-cia-tod-clock-and-alarm test`; `TOD-WRITE-STOPS-CLOCK` is at `test_ciaa.cpp:792-869`; the package reads `44 passed, 0 failed`, its fixture-liveness check included, so nothing exits nonzero |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 344 (*Time of Day Clock*, and the table *BIT NAMES for WRITE TIME/ALARM or READ TIME*), 346, 349 (Control Register B), 350, 354 · *Amiga Hardware Reference Manual*, 3rd ed., 1991. Claim ids: `TOD-WRITE-STOPS-CLOCK`: `opus:F-tod.c9`, `sol:F-tod.c10`, p. 344; `TOD-WRITE-STARTS-LSB-ONLY`: `opus:F-tod.c10`, `sol:F-tod.c11`, p. 344; `TOD-WRITE-STARTS-ATOMIC-VALUE`: `opus:F-tod.c11`, `sol:F-tod.c12`, p. 344. All three assertions carry a claim id |
| **Assertions** | all green. `TOD-WRITE-STOPS-CLOCK` starts three fresh fixtures, writes $000000 with $8 last and gives one tick, so each clock reads $000001, then writes $00 to one register and gives one more tick: after the write to $A the count stayed at $000001, after the lone write to $8 it counted on from $000000 to $000001, and after the write to $9, recorded and not graded, it counted on to $000002. `TOD-WRITE-STARTS-LSB-ONLY` reads a write to $9 after one to $A leaving the clock stopped and the following write to $8 restarting it; `TOD-WRITE-STARTS-ATOMIC-VALUE` lands the sequence $A, $9, $8 on the value written with ticks between the writes |
| **Verdict of the four** | **unanimous with NanoMig**: WinUAE, vAmiga, Denise and Copperline all stop the TOD clock on a write to $A, restart it on a write to $8, and leave it running across a write to $9. vAmiga's and Copperline's comments state that rule; WinUAE and Denise, in their 6526 models, put the stop on the 6526's hours register, the top one of its four. Hardware: not measured; the 6502.org dissection of one 8520R4 draws the stop on a write to $A and the restart on a write to $8, and no strobe of $9 reaches the count enable |

---

## Why it was withdrawn

The withdrawn claim is that a lone write to any one of $A, $9 and $8 stops a
running clock. Its check writes $00 to one TOD register of a clock that
NanoMig's reset leaves running, gives one tick and requires the count not to
move, once each for $A, $9 and $8. NanoMig stops on the write to $A and counts
on after the writes to $9 and $8.

**p. 344 does not name the register whose write stops the clock.** *"TOD is
automatically stopped whenever a write to the register occurs"* is singular,
and no earlier sentence of its paragraph names a register for it to refer to.
That each of the three stops the clock is the claims' restatement, *"A write to
a TOD register automatically stops TOD"* (`sol:F-tod.c10`) and *"Writing a TOD
register stops the clock automatically"* (`opus:F-tod.c9`), not the page's
sentence. The page leaves a write to $9 on its own undecided.

**The register NanoMig stops on is the one the 8520R4 has.** In the 6502.org
dissection the latch that enables TOD's count is cleared by RES and by a write
strobe formed from the register-$A select and CRB7#, and set by the LSB's write
strobe; register 9's write strobe never reaches that latch. NanoMig clears and
sets `count_ena` on the same two writes, both with CRB7 clear, and all four
implementations and both Minimig lines do the same. The 6526 specification's
version of the paragraph names the register: the hours register, the top of the
6526's four.

**The $8 half cannot be met by an implementation of the page.** The next
sentence, *"The clock will not start again until after a write to the LSB event
register"*, makes a write to $8 the one after which the clock counts, so under
either reading of *the register* a lone write to $8 ends with the clock
counting. The claim requires it stopped.

**What the difference does.** A replay with the package's harness and bus
timing, run outside the package (`make test` does not print it), so a reading of the code and not a package result, writes to a
counting clock:

| from a running clock | NanoMig reads | if a write to $9 stopped the clock |
|---|---|---|
| at $000003, $05 to $9, three ticks | $000506 | $000503 |
| at $0000FE, $05 to $9, two ticks, $10 to $8 | **$000610** | $000510 |
| the same, with $00 to $A first | $000510 | $000510 |
| at $000000, $20 to $8, one tick | $000021 | $000021 |

A program that sets the counter by writing $9 and then $8, without $A first,
lands one higher in the middle byte in NanoMig when a carry out of the low byte
falls between the two writes. Writing $A first, NanoMig lands on the value
written.

---

## What the HRM says

**Printed p. 344**, *Time of Day Clock*:

> TOD consists of a 24-bit binary counter. Positive edge transitions on this pin cause the binary
> counter to increment.

and the paragraph the assertion grades, whole:

> A specific sequence of events must be followed for proper setting and reading of TOD. TOD is
> automatically stopped whenever a write to the register occurs. The clock will not start again until
> after a write to the LSB event register. This assures that TOD will always start at the desired
> time.

The paragraph after it, on reading:

> Since a carry from one stage to the next can occur at any time with respect to a read operation, a
> latching function is included to keep all TOD information constant during a read sequence. All
> TOD registers latch on a read of MSB event and remain latched until after a read of LSB event.

The table under them names register 8 `LSB Event` (E7–E0), register 9 `Event 8-15`
(E15–E8) and register A `MSB Event` (E23–E16). **Printed p. 349**, Control
Register B, gives the write its target:

> 7 ALARM 1 = writing to TOD registers sets Alarm
> 0 = writing to TOD registers sets TOD clock.

**The sentence that stops the clock names no register.** Every other sentence
of the section that picks out a particular TOD register names it: the LSB
event register that restarts the clock, and *MSB event* and *LSB event* for
the read latch. Where the pages mean every one of the three they say so: *any
read of a TOD address*, *the corresponding TOD registers*, *All TOD registers
latch*, p. 349's *writing to TOD registers*. The paragraph has no earlier
singular register for *the register* to refer to, so at the point measured
the page reads two ways:

- *the register* is TOD's register as a whole, and a write to any of $8, $9
  and $A stops the clock — the reading both claims the assertion grades take;
- *the register* is one register the sentence leaves unnamed, and nothing on
  the page says a write to $9 stops the clock.

A write to $A with CRB7 clear (p. 349) stops the clock under both readings.
**Under either reading a write to $8 leaves the clock running**: the next
sentence makes it the write after which the clock starts again, so a write to
$8 on its own, whether or not it stops the clock first, ends with the clock
counting.

Beyond writing the LSB event register last, the page prints no order for the
writes. No other page speaks to the stop. p. 349 routes a TOD write by CRB bit 7 and p. 350's bitmap repeats it;
p. 346 lists the TOD alarm among the interrupt sources; the reset paragraph on
p. 354 says nothing of the clock running or stopped.

---

## What NanoMig does

**It stops the clock on a write to $A and on nothing else, and restarts it on
a write to $8.**
[`src/minimig-aga/cia_timerd.v#L68-L80`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerd.v#L68-L80):

```verilog
else if (wr && !crb7) // crb7==0 enables writing to TOD counter
begin
  if (thi/* || tme*/) // stop counting
    count_ena <= 1'd0;
  else if (tlo) // write to LSB starts counting again
    count_ena <= 1'd1;
end
```

`count_ena` gates the count,
[`#L98-L99`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerd.v#L98-L99).
The middle byte's term is there and commented out, `/* || tme*/`, so a write to
$9 sets `tod[15:8]` and nothing else,
[`#L93-L94`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerd.v#L93-L94).
The three selects are the address decoder's registers 8, 9 and A,
[`src/minimig-aga/ciaa.v#L144-L146`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L144-L146),
wired to the module as `.tlo(tdlo)`, `.tme(tdme)`, `.thi(tdhi)`
([`ciaa.v#L376-L378`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L376-L378));
CIA-B wires the same module the same way
([`ciab.v#L240-L242`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L240-L242)).

**Measured.** [`hrm/appF-cia-tod-clock-and-alarm`](../../../hrm/appF-cia-tod-clock-and-alarm)
builds `ciaa` alone and drives its register bus and its TOD pin; the
`TOD-WRITE-STOPS-CLOCK` procedure is in the *Assertions* row.

| write to a running clock, then one tick | p. 344, *the register* any of the three | p. 344, *the register* the MSB event register | NanoMig | 8520R4, as sheet 14b draws it | the withdrawn claim requires | `TOD-WRITE-STOPS-CLOCK` grades |
|---|---|---|---|---|---|---|
| $A, MSB event | stopped | stopped | stopped | stopped | stopped | stopped |
| $9, event 8–15 | stopped | running | **running** | running | stopped | not graded |
| $8, LSB event | running — the next sentence restarts it | running | running | running | **stopped** | running |

The assertion grades the $A and $8 rows and is green. Of the two rows the
withdrawn claim adds, only the $9 row departs from a reading of the page, the
first one. The dissection's column is NanoMig's on all three rows.

---

## Risks and unknowns

**Condition for filing again.** The $9 half would be filed again only if a
printed page names a register other than $A for the stop, or says that a write
to $9 stops the clock. The $8 half cannot be: p. 344's next sentence rules it
out under either reading of *the register*.

- **The claim's starting state is NanoMig's, not the page's.** The check starts
  from a clock NanoMig's reset leaves running. p. 354 resets *"all internal
  registers"* and says nothing of the TOD clock running or stopped. In the
  dissection RES clears the count-enable latch, so a chip wired as sheet 14b
  draws it holds TOD stopped from a reset until the first write to $8; there the
  check's $A and $9 cases would read a count that never moved, whatever the write
  did. A running clock for the replacement is one started by a write to $8 and
  seen to count before the write under test.
- **The page reads two ways** on a write to $9 on its own; nothing grades it.
- **The dissection is of one revision**, the 8520R4, and was read from the
  drawing.
- **With the ALARM bit set** a high-byte write goes to the alarm without
  stopping the clock in the dissection; p. 344 does not address it.
- **The 6526 specification is a 6526 document** and does not itself speak for the
  8520; its OCR interleaves the page's two columns.

---

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1805-1834`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1805-L1834), [`:1835-1845`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1835-L1845), [`:1127-1137`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1127-L1137), [`cfgfile.cpp:9223-9236`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9223-L9236) | a TOD write with the ALARM bit clear **starts the clock on register 8 (`c->todon = 1;`) and stops it on register 10 (`c->todon = 0;`)**; register 9 only sets the middle byte, `setciatod(&c->tod, (getciatod(c->tod) & ~0xff00) \| (val << 8));`. The count is skipped while stopped, `if (!c->todon) {` / `return;`. With its 6526 option the stop moves to register 11, the 6526's hours register; only the Velvet configuration, an A1000 chip set, sets it, `p->cs_cia6526 = 1;` |
| **vAmiga** | [`Core/Components/CIA/CIARegs.cpp:325-352`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L325-L352), [`Core/Components/CIA/TOD.h:71-75`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/TOD.h#L71-L75), [`Core/Components/CIA/TOD.cpp:170-172`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/TOD.cpp#L170-L172) | **stops on the high byte (`tod.stop();`), continues on the low byte (`tod.cont();`), and the middle byte only stores**, under the comment *"The CIA chip stops the TOD clock when the counter's high byte (bits 16 - 23) is written and restarts it, when the low byte (bits 0 - 7) is written."* The count is skipped while stopped, `if (stopped) return;` |
| **Denise** | [`emulation/cia/new/register.cpp:319-334`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L319-L334), [`emulation/cia/new/tod.cpp:5-8`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/tod.cpp#L5-L8), [`emulation/libami/agnus/agnus.h:104-105`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.h#L104-L105) | the Amiga's two CIAs are its 8520 model, `Cia<MOS_8520>& cia1;`: **register 8 starts the clock (`todActive = true;`), register A stops it (`todActive = false;`)**, and register 9 changes only the count. Its 6526 model stops on register B, the hours register, `} else if (pos == 0xb)` / `todActive = false;`. A TOD edge does nothing while stopped, `if (!todActive)` / `return;` |
| **Copperline** | [`src/chipset/cia.rs:596-627`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L596-L627), [`:271-274`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L271-L274) | **stops on TODHI (`self.tod_stopped = true;`), restarts on TODLO (`self.tod_stopped = false;`)** under *"Writing TODLO restarts the counter after a TODHI write stopped it."*, and TODMID only sets the middle byte, `self.tod_count = (self.tod_count & 0xFF00FF) \| ((val as u32) << 8);`. A tick does nothing while stopped, `if self.tod_stopped {` / `return false;` |
| **MiSTer** | [`rtl/cia_timerd.v:81-96`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timerd.v#L81-L96) | descendant — the same block under a comment that states it, `// Writing to high byte stops TOD counting` / `// Writing to low byte starts TOD counting again`, with the same commented-out term, `if (thi/* \|\| tme*/) // Stop counting during update`, and the same restart, `else if (tlo)       // write to LSB starts counting again` |
| **TC64** | [`rtl/minimig/cia_timerd.v:70-81`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timerd.v#L70-L81) | descendant — the same stop, `if (thi && !crb7 /* \|\| tme*/) // stop counting`. It restarts more widely, `else if (tlo \|\| (tcr && !data_in[7]))`, on any write to $8 or a CRB write with bit 7 clear, and resets with the clock stopped, `count_ena <= 1'd0;`; a write to $9 stops nothing in either line |

**All four stop the clock on the MSB event register alone.** Each restarts it
on a write to $8, and none lets a write to $9 stop it. WinUAE and Denise keep a
6526 model beside the 8520 one, and in both the stop sits on the 6526's top
register, the hours register, where the 8520's is on $A. Both Minimig lines
carry NanoMig's stop, the commented-out middle-byte term included.

### What other Commodore documents say

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 219 — prints the manual's sentence, register unnamed:

  > "TOD is automatically stopped whenever a write to the register occurs. The clock will not start again until after a write to the LSB Event Register."
  <!-- corpus: a500-a2000-trm-1987/p219.txt -->

- **Commodore, 6526 CIA specification, 1981**, PDF page 6 — names the register: the hours register, the top of the 6526's four:

  > "stopped whenever a write to the hours"
  <!-- corpus: cia-6526-spec/p006.txt -->

  > "register occurs. The clock will not start again until after"
  <!-- corpus: cia-6526-spec/p006.txt -->

  > "a write to the 10ths of seconds register."
  <!-- corpus: cia-6526-spec/p006.txt -->

  It is p. 344's paragraph with the 6526's registers named: the stop on a write to the hours register, the restart on a write to the tenths register. The 6526's TOD is a BCD clock in four registers, tenths of seconds, seconds, minutes and hours.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, schematic sheets 14b and 6f — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 14b, drawn transistor by transistor, keeps TOD's count enable in a latch that RES or a write strobe of register $A, the MSB, clears, and that the LSB's write strobe W_TOD_LSB sets through a PHI2 and a PHI1 stage; the counter advances (TOD_CEN#) only while it is set, so from a reset it stands until the first write to $8. Register 9's write strobe, W_TOD_8_15, does not reach that latch: on the same sheet it pulls TOD_Cin12# high and TOD_CNT_8..11 low, the middle byte's own carry and count lines, while it lasts. Both drawings of the latches on 14b form the register-$A strobe, RW_TOD_MSB#, from SEL_$A and CRB7#. Sheet 6f, the address decoder, forms W_TOD_LSB and W_TOD_8_15 only with CRB7 clear.
  <!-- drawing: cia-8520r4-dissection/sheet_020.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_009.txt -->

  A high-byte write stops the clock until the next low-byte write: p. 344's sequence, the register whose write stops the clock being the MSB. A write to register 9 neither stops nor starts it. The stop acts only with CRB7 clear.
