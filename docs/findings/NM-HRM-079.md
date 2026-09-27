# NM-HRM-079 — a write to the TOD counter's middle byte leaves the clock running; p. 344 stops it on a write to "the register" and does not say which

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. p. 344 stops the TOD clock "whenever a write to the register occurs" and does not say which register; NanoMig, both Minimig lines and all four implementations stop it on a write to the MSB event register ($A) alone and restart it on a write to the LSB event register ($8). Whether a write to $9 stops it is the question, and the page does not settle it. The other half of the red is not a question: the assertion also requires a lone write to $8 to leave the clock stopped, which p. 344's next sentence rules out under either reading, so that half is the test's to correct. |
| **Module** | `cia_timerd.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerd.v#L68-L80) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timerd.v#L81-L96) |
| **Report to** | **the manual, not the implementation**, and this suite's test for its $8 half. NanoMig, both Minimig lines and all four implementations stop the clock on a write to $A alone |
| **Test** | [`hrm/appF-cia-tod-clock-and-alarm`](../../hrm/appF-cia-tod-clock-and-alarm) · `TOD-WRITE-STOPS-CLOCK` (`test_ciaa.cpp:781-818`), claims `sol:F-tod.c10`, `opus:F-tod.c9` (p. 344), red |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 344, 349 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red; the package reads `43 passed, 1 failed`, its fixture-liveness check included. `TOD-WRITE-STOPS-CLOCK` writes $00 to one TOD register of a running clock, gives one tick and requires the count not to move, for each of the three registers: the write to $A stopped the clock, the writes to $9 and $8 did not. In the same run `TOD-WRITE-STARTS-LSB-ONLY`, green, reads a write to $9 after one to $A leaving the clock stopped and the following write to $8 restarting it, and `TOD-WRITE-STARTS-ATOMIC-VALUE`, green, lands the sequence $A, $9, $8 on the value written with ticks between the writes |
| **Verdict of the four** | **unanimous with NanoMig**: WinUAE, vAmiga, Denise and Copperline all stop the TOD clock on a write to $A, restart it on a write to $8, and leave it running across a write to $9. vAmiga's and Copperline's comments state that rule; WinUAE and Denise, in their 6526 models, put the stop on the 6526's hours register, the top one of its four |

---

## What the HRM says

**Printed p. 344**, *Time of Day Clock*:

> TOD consists of a 24-bit binary counter. Positive edge transitions on this pin cause the binary
> counter to increment.

and the paragraph the red grades, whole:

> A specific sequence of events must be followed for proper setting and reading of TOD. TOD is
> automatically stopped whenever a write to the register occurs. The clock will not start again until
> after a write to the LSB event register. This assures that TOD will always start at the desired
> time.

The paragraph after it, on reading:

> Since a carry from one stage to the next can occur at any time with respect to a read operation, a
> latching function is included to keep all TOD information constant during a read sequence. All
> TOD registers latch on a read of MSB event and remain latched until after a read of LSB event.

The table under them, *BIT NAMES for WRITE TIME/ALARM or READ TIME*, names
register 8 `LSB Event` (E7–E0), register 9 `Event 8-15` (E15–E8) and register
A `MSB Event` (E23–E16). **Printed p. 349**, Control Register B, gives the
write its target:

> 7 ALARM 1 = writing to TOD registers sets Alarm
> 0 = writing to TOD registers sets TOD clock.

**The sentence that stops the clock names no register.** Every other sentence
of the section that picks out a particular TOD register names it: the LSB
event register that restarts the clock, and *MSB event* and *LSB event* for
the read latch. Where the pages mean all three they use the plural: *the corresponding TOD
registers*, *All TOD registers latch*, p. 349's *writing to TOD registers*. The
paragraph has no earlier singular register for *the register* to refer to, so
at the point measured the page reads two ways:

- *the register* is TOD's register as a whole, and a write to any of $8, $9
  and $A stops the clock — the reading both claims the assertion grades take,
  *"A write to a TOD register automatically stops TOD"* (`sol:F-tod.c10`);
- *the register* is one register the sentence leaves unnamed, and nothing on
  the page says a write to $9 stops the clock.

**Under either reading a write to $8 leaves the clock running.** The next
sentence makes it the write after which the clock starts again, so a write to
$8 on its own, whether or not it stops the clock first, ends with the clock
counting.

Beyond writing the LSB event register last, the page prints no order for the
writes. No other page speaks to the stop. p. 349 routes a TOD write by CRB bit 7
and p. 350's bitmap repeats it; p. 346 lists the TOD alarm among the interrupt
sources; the reset paragraph on p. 354 says nothing of the clock running or
stopped.

---

## What NanoMig does

**It stops the clock on a write to $A and on nothing else, and restarts it on
a write to $8.**
[`src/minimig-aga/cia_timerd.v#L68-L80`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerd.v#L68-L80):

```verilog
// timer D count enable control
always @(posedge clk)
  if (clk7_en) begin
    if (reset)
      count_ena <= 1'd1;
    else if (wr && !crb7) // crb7==0 enables writing to TOD counter
    begin
      if (thi/* || tme*/) // stop counting
        count_ena <= 1'd0;
      else if (tlo) // write to LSB starts counting again
        count_ena <= 1'd1;
    end
  end
```

`count_ena` gates the count,
[`#L98-L99`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerd.v#L98-L99),
`else if (count_ena && count)` / `tod[23:0] <= tod[23:0] + 24'd1;`. The middle
byte's term is there and commented out, `/* || tme*/`, so a write to $9 sets
`tod[15:8]` and nothing else,
[`#L93-L94`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerd.v#L93-L94),
`if (tme)` / `tod[15:8] <= data_in[7:0];`. The three selects are the address
decoder's registers 8, 9 and A,
[`src/minimig-aga/ciaa.v#L144-L146`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L144-L146),
`assign tdlo = enable && rs==4'h8;`, wired to the module as `.tlo(tdlo)`,
`.tme(tdme)`, `.thi(tdhi)`
([`ciaa.v#L376-L378`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L376-L378));
CIA-B wires the same module the same way
([`ciab.v#L240-L242`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L240-L242)).

The read latch is keyed to the same two registers the page names for it: a
read of $A freezes the output and a read of $8 releases it,
[`#L37-L40`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerd.v#L37-L40),
`if (thi && !crb7)` / `latch_ena <= 1'd0;` / `else if (tlo)` /
`latch_ena <= 1'd1;`. The package's four read-latch assertions are green.

### Measured

[`hrm/appF-cia-tod-clock-and-alarm`](../../hrm/appF-cia-tod-clock-and-alarm)
builds `ciaa` alone and drives its register bus and its TOD pin.
`TOD-WRITE-STOPS-CLOCK` starts three fresh fixtures with the clock running
from $000000, writes $00 to one register in each, gives one tick and reads
the count back:

| write to a running clock, then one tick | p. 344, *the register* any of the three | p. 344, *the register* the MSB event register | NanoMig | the assertion requires |
|---|---|---|---|---|
| $A, MSB event | stopped | stopped | stopped | stopped |
| $9, event 8–15 | stopped | running | **running** | stopped |
| $8, LSB event | running — the next sentence restarts it | running | running | **stopped** |

The $A row is the same comparator reading a stop, so the comparator is live.
The red is the two rows below it, and of those only the $9 row departs from a
reading of the page, the first one.

**What the difference does.** A replay with the package's own harness and bus
timing — a measurement for this page, not a graded assertion — writes to a
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

## What the others say

The question put to each: which TOD register writes stop the clock, and which
restart it?

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:1805-1834`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1805-L1834), [`:1835-1845`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1835-L1845), [`:1127-1137`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1127-L1137), [`cfgfile.cpp:9223-9236`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cfgfile.cpp#L9223-L9236) | <pre>case 8:<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;c-&gt;todon = 1;<br>...<br>case 10:<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;if (!currprefs.cs_cia6526) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;c-&gt;todon = 0;</pre> a TOD write with the ALARM bit clear **starts the clock on register 8 and stops it on register 10**; register 9 only sets the middle byte, `setciatod(&c->tod, (getciatod(c->tod) & ~0xff00) \| (val << 8));`. The count is skipped while stopped, `if (!c->todon) {` / `return;`. With its 6526 option the stop moves to register 11, the 6526's hours register, `c->todon = 0;`; only the Velvet configuration, an A1000 chip set, sets it, `p->cs_cia6526 = 1;` |
| **vAmiga** | [`Core/Components/CIA/CIARegs.cpp:325-352`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L325-L352), [`Core/Components/CIA/TOD.h:71-75`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/TOD.h#L71-L75), [`Core/Components/CIA/TOD.cpp:170-172`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/TOD.cpp#L170-L172) | <pre>tod.setCounterLo(value);<br>tod.cont();<br>...<br>tod.setCounterMid(value);<br>...<br>tod.setCounterHi(value);<br>tod.stop();</pre> **stops on the high byte, continues on the low byte, and the middle byte only stores**, under the comment *"The CIA chip stops the TOD clock when the counter's high byte (bits 16 - 23) is written and restarts it, when the low byte (bits 0 - 7) is written."* The count is skipped while stopped, `if (stopped) return;` |
| **Denise** | [`emulation/cia/new/register.cpp:319-334`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L319-L334), [`emulation/cia/new/tod.cpp:5-8`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/tod.cpp#L5-L8), [`emulation/libami/agnus/agnus.h:104-105`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.h#L104-L105) | <pre>if (pos == 8) {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;todActive = true;<br><br>} else if (model == MOS_8520 &amp;&amp; pos == 0xa) {<br>&nbsp;&nbsp;&nbsp;&nbsp;todActive = false;</pre> the Amiga's two CIAs are its 8520 model, `Cia<MOS_8520>& cia1;`: **register 8 starts the clock, register A stops it**, and register 9 changes only the count. Its 6526 model stops on register B, the hours register, `} else if (pos == 0xb)` / `todActive = false;`. A TOD edge does nothing while stopped, `if (!todActive)` / `return;` |
| **Copperline** | [`src/chipset/cia.rs:596-627`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L596-L627), [`:271-274`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L271-L274) | <pre>REG_TODHI =&gt; {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;// Writing TODHI stops the counter until TODLO<br>&nbsp;&nbsp;&nbsp;&nbsp;// is written.<br>&nbsp;&nbsp;&nbsp;&nbsp;self.tod_stopped = true;</pre> **stops on TODHI, restarts on TODLO**, `self.tod_stopped = false;` under *"Writing TODLO restarts the counter after a TODHI write stopped it."*, and TODMID only sets the middle byte, `self.tod_count = (self.tod_count & 0xFF00FF) \| ((val as u32) << 8);`. A tick does nothing while stopped, `if self.tod_stopped {` / `return false;` |
| **MiSTer** | [`rtl/cia_timerd.v:81-96`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timerd.v#L81-L96) | descendant — the same block under a comment that states it, `// Writing to high byte stops TOD counting` / `// Writing to low byte starts TOD counting again`, with the same commented-out term, `if (thi/* \|\| tme*/) // Stop counting during update`, and the same restart, `else if (tlo)       // write to LSB starts counting again` |
| **TC64** | [`rtl/minimig/cia_timerd.v:70-81`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timerd.v#L70-L81) | descendant — the same stop, `if (thi && !crb7 /* \|\| tme*/) // stop counting`. It restarts more widely, `else if (tlo \|\| (tcr && !data_in[7]))`, on any write to $8 or a CRB write with bit 7 clear, and resets with the clock stopped, `count_ena <= 1'd0;`; a write to $9 stops nothing in either line |

**All four stop the clock on the MSB event register alone.** Each restarts it
on a write to $8, and none lets a write to $9 stop it. vAmiga and Copperline
say so in their comments. WinUAE and Denise keep a 6526 model beside the 8520
one, and in both the stop sits on the 6526's top register, the hours register,
where the 8520's is on $A. Both Minimig lines carry NanoMig's block, the
commented-out middle-byte term included.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 219 — prints the manual's sentence, register unnamed:

  > "TOD is automatically stopped whenever a write to the register occurs. The clock will not start again until after a write to the LSB Event Register."
  <!-- corpus: a500-a2000-trm-1987/p219.txt -->

  Its 8520 section gives p. 344's paragraph with the same unnamed register, and on PDF page 220 the read latch on a read of MSB event, as p. 344 does.

- **Commodore, 6526 CIA specification, 1981**, PDF pages 6 and 7 — names the register: the hours register, the top of the 6526's four:

  > "stopped whenever a write to the hours"
  <!-- corpus: cia-6526-spec/p006.txt -->

  > "register occurs. The clock will not start again until after"
  <!-- corpus: cia-6526-spec/p006.txt -->

  > "a write to the 10ths of seconds register."
  <!-- corpus: cia-6526-spec/p006.txt -->

  > "on a read of Hours and remain latched until after a"
  <!-- corpus: cia-6526-spec/p007.txt -->

  The OCR interleaves the page's two columns, so the sentence is quoted line by line. It is p. 344's paragraph with the 6526's registers named: the stop on a write to the hours register, the restart on a write to the tenths register, and the read latch on a read of hours where p. 344 has MSB event. The 6526's TOD is a BCD clock in four registers, tenths of seconds, seconds, minutes and hours; it is a 6526 document and does not itself speak for the 8520.

---

## Why this is filed against the page

**The red is two halves, and neither is NanoMig departing from a clear page.**

- **The $8 half grades the opposite of the page.** p. 344 starts the clock
  after a write to the LSB event register; the assertion requires that write,
  alone, to leave it stopped. An implementation of either reading of the page
  fails that half, and so do all four implementations and both Minimig lines.
  It is the test's to correct.
- **The $9 half grades one reading of an unnamed register.** p. 344 says *the
  register* where every other sentence of the section names the register it
  means, and the pages use the plural where they mean all three. NanoMig reads
  it as the MSB event register, and so does every implementation read here;
  the 6526 specification, whose paragraph is p. 344's word for word but for
  the register names, names the hours register there.

The finding key names the pair that disagrees, NanoMig and p. 344, and does
not prejudge which side is wrong.

---

## Suggested change

**Not an RTL change.** NanoMig already does what all four implementations and
both Minimig lines do.

For the test: the $8 sub-check of `TOD-WRITE-STOPS-CLOCK` should read the clock
running after a lone write to $8, as p. 344's next sentence makes it, or be
dropped. The $9 sub-check grades the reading in which any TOD register stops
the clock; whether that is the 8520's behaviour is the question below, and
until it is answered the sub-check measures the page's ambiguity, not a defect.

---

## What holds the finding up

- **The measurement is live.** The $A sub-check reads a stop on the same
  comparator, and `TOD-WRITE-STARTS-LSB-ONLY` and
  `TOD-WRITE-STARTS-ATOMIC-VALUE` are green on the same writes and ticks.
- **The page names every other register it means**, and names the MSB event
  register for the read latch that mirrors the stop.
- **Every implementation read here stops the clock on $A alone**, the four
  independent ones and both Minimig lines; two of the four say so in comments,
  and the two with a 6526 model put the stop on that chip's top register.
- **The 6526 specification names the register** in a paragraph that is
  p. 344's word for word but for the register names, and the *A500/A2000
  Technical Reference Manual* repeats p. 344's unnamed one.

## What weakens it

- **Read alone, the any-register reading is the one under which the page is
  complete.** The only order p. 344 prints is the LSB event register last. Under
  the MSB reading the page never says to write $A first, and a program that
  writes $9 and then $8, as the first reading allows, lands one higher in the
  middle byte in NanoMig when a carry falls between the writes (the replay
  above).
- **The second document is another chip's.** The 6526 counts BCD time in four
  registers; its sentence says what the paragraph meant there, not what the
  8520's stop is.

---

## For the specialists

1. **Does a write to register $9 stop a real 8520's TOD clock?** With the clock
   counting, write $9 alone and read the count after the next TOD edge. All
   four implementations and both Minimig lines say it does not, and p. 344 does
   not say.
2. **Does software write $A first?** A program that writes $A, then $9, then
   $8 gets the same result under both readings; only one that leaves $A out
   can see the difference.
