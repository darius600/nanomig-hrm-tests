# NM-HRM-053 — Appendix B's `*` mark has no ECS qualifier, and nothing honours it under ECS

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. Appendix C p. 310 gives the ECS Copper all of the chip registers once CDANG is set, and chapter 2, Appendix A and Appendix B's own `(E)` note send the reader to Appendix C for ECS; NanoMig does what p. 310 says. |
| **Module** | `agnus_copper.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L225) |
| **Test** | [`hrm/ch8-appb-copper-access-marks`](../../../hrm/ch8-appb-copper-access-marks) — `SERDAT-CDANG1`, `SERPER-CDANG1` and `SERDATR-CDANG1` still fail, because they require the block p. 310 lifts; their `-CDANG0` twins and `CTRL-BLTCON0` green |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 3 red of 8, with a control that separates the two printed marks |
| **Error class** | **a legend graded without the appendix that extends it** |
| **Verdict of the four** | **unanimous with NanoMig and with p. 310**: WinUAE, vAmiga, Denise and Copperline all keep a Copper MOVE out of `$000`–`$03E` on OCS whatever CDANG holds, and let it reach every register when the Agnus is ECS and CDANG is set; Copperline asserts both halves in its own tests |

---

## Why it was withdrawn

The three red assertions require a Copper MOVE to SERDATR (`$018`), SERDAT
(`$030`) and SERPER (`$032`) to be blocked with `ecs` high and CDANG set. They
grade Appendix B's `*` on those rows, p. 289, with the legend on p. 287,
*"Address not writable by the Copper"*, which names no chipset.

**Appendix C extends that rule for ECS, in so many words.** p. 310 introduces
COPCON as a register whose *"interpretation has been extended for the ECS"* and
says *"Now, in the ECS, if this bit is set, the Copper can access all of the
Amiga chip registers."* The three registers are chip registers.

**Every page that states the band without a chipset sends the reader to
Appendix C for ECS.** p. 26's *"cannot affect at all"* is chapter 2's, and
chapter 2 sends the ECS Copper to Appendix C (p. 36). Appendix A's COPCON entry
does the same (p. 274). Appendix B marks COPCON's own row `(E)` (p. 289), and
p. 288 says what `(E)` means and sends the reader to Appendix C's register map,
which lists COPCON as changed (p. 298).

**Nothing in the `*` band can only describe ECS.** Appendix C's register map
lists no register below `$05A` as new (p. 298). The six `(E)` rows in the band,
VPOSR, POT0DAT, POT1DAT, DSKPTH, COPCON and STRLONG, are all changed registers
the standard chip set has too, and SERDATR, SERDAT and SERPER carry no `(E)`.
That is where this band differs from the `~` band of
[NM-HRM-004](../NM-HRM-004.md): there Appendix B puts `~` on four registers that
exist only in ECS, so it speaks of ECS and contradicts p. 310. Here the manual
gives one rule for the standard chip set, on p. 26 and in Appendix B, and
states the ECS difference on p. 310.

**For these three registers p. 310 and the `*` differ at one corner only.** The
two other ranges p. 310 prints, the standard chip set with CDANG set and ECS
with CDANG clear, both run from `$DFF03E` through `$DFF07E` and leave all three
out, as the `*` does; the `-CDANG0` twins grade the second and are green. The
corner where they differ, ECS with CDANG set, is the one p. 310 introduces with
*"Now, in the ECS"*.

**The suite grades p. 310's reading of the same corner, and it is green.**
[`hrm/ch2-copper-move-sweep`](../../../hrm/ch2-copper-move-sweep)'s
`SWEEP ecs=1 cdang=1 vs Appendix C` expects every destination accepted, these
three included, and passes; [`hrm/ch2-copper-move-range`](../../../hrm/ch2-copper-move-range)'s
`T10` has the Copper write COPCON, `*02E` on p. 289, at ECS with CDANG set,
from p. 310, and passes. No implementation passes both those assertions and the
three reds here, and none that does what p. 310 says can pass the three reds.

---

## What the HRM says

**Appendix B, printed p. 287**, the legend (`appB3/p287.txt:22-24`):

> `*    Address not writable by the Copper.`
>
> `~     Address not writable by the Copper unless the "copper danger bit", COPCON is set true.`

**Printed p. 289**, the three graded rows and COPCON's (`appB3/p289.txt:21`,
`:32-34`):

> `SERDATR      *018   R           P    Serial port data and status read`
>
> `COPCON       *02E   W     A(E)       Coprocessor control register (CDANG)`
>
> `SERDAT       *030   W           P    Serial port data and stop bits write`
>
> `SERPER       *032   W           P    Serial port period and control`

Every row from `$000` to `$03E` carries `*` (`appB3/p289.txt:9-40`), and every
row from `$040` to `$07E` carries `~`.

**Printed p. 288**, below the legend's continuation (`appB3/p288.txt:24-29`):

> "About the ECS registers. Registers denoted with an "(E)" in the chip column
> means that those registers have been changed in the Enhanced Chip Set (ECS).
> ... See the register map in Appendix C for information on which ECS registers
> are new and which have been modified."

**Appendix C, printed p. 298**, that register map (`appC-audio/p298.txt:18`):

> `02E  COPCON   chg   W  A   Coprocessor control`

**Appendix C, printed p. 310**, under *Interpretational Differences*
(`appC-ecs/p310.txt:8-9`, `:25-36`; the printed scan, `text/p310.txt:3-4`,
`:16-24`, reads the same words):

> "The following registers have the same functionality as the standard chip
> set, however, their behavior is interpreted differently."
>
> "Another register where the interpretation has been extended for the ECS is
> COPCON."
>
> "This 1-bit register, the danger bit (CDANG), when set allows the Coprocessor
> to write to the Blitter hardware. In the standard chip set, if this is set,
> the Copper can access the address range from $DFF03E through $DFF07E. Now, in
> the ECS, if this bit is set, the Copper can access all of the Amiga chip
> registers. If this bit is clear, the Copper can access the address range from
> $DFF03E through $DFF07E, the same range as when the danger bit is set in the
> standard chip set."

and closes by giving Appendices A and B to the standard chip set
(`text/p310.txt:35-36`):

> "For more information on the AUDxPER register, and any other register in the
> Amiga standard chip set, see Appendices A and B."

**Chapter 2, printed p. 26**, the band without a chipset (`ch2/p26.txt:24-27`):

> "The registers that the Copper can always affect are numbered $80 through $FF
> inclusive. (See Appendix B for a list of registers in address order.) Those it
> cannot affect at all are numbered $00 to $3E inclusive. The Copper control
> register is within this group ($00 to $3E)."

and **p. 36**, in the same chapter (`ch2/p36.txt:51-52`):

> "For information relating to the Copper in the Enhanced Chip Set (ECS), see
> Appendix C."

**Appendix A, printed p. 274**, the COPCON entry (`appA-copper/p274.txt:10-14`):

> "This is a 1-bit register that when set true, allows the Copper to access the
> blitter hardware. This bit is cleared by power-on reset, so that the Copper
> cannot access the blitter hardware. See Appendix C for ECS operation."

p. 283 and p. 265, the Appendix A pages for these three registers and
ADKCONR, do not mention the Copper: `grep -il copper` over both returns
nothing.

p. 310's standard-chip-set range starts at `$DFF03E`, where Appendix B prints
STRLONG `*` (`appB3/p289.txt:40`) and p. 26 puts the last address the Copper
cannot affect. That one address is [NM-HRM-004](../NM-HRM-004.md)'s question and
none of the three graded here.

---

## What NanoMig does

It keeps the band closed on OCS and opens it with ECS and CDANG, as p. 26,
Appendix B and p. 310 together describe.
[`src/minimig-aga/agnus_copper.v#L220-L228`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L220-L228):

```verilog
// detect illegal register access
// CDANG = 0 (OCS/ECS) : $080-$1FE allowed
// CDANG = 1 (OCS)     : $040-$1FE allowed
// CDANG = 1 (ECS)     : $000-$1FE allowed
always @(*)
if (ir2[8:7]==2'b00 && !cdang || ir2[8:6]==3'b000 && !ecs) // illegal access
	illegalreg = 1'b1;
```

`$000`–`$03E` is `ir2[8:6]==3'b000`: illegal whenever `ecs` is low, whatever
CDANG holds, and with `ecs` high only while CDANG is clear. An illegal MOVE
halts the Copper at
[`:421`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L421).

### Measured

[`hrm/ch8-appb-copper-access-marks`](../../../hrm/ch8-appb-copper-access-marks),
`ecs` high: a Copper MOVE to SERDATR, SERDAT and SERPER is blocked with CDANG
clear and admitted with CDANG set. `SERDAT-CDANG1`, `SERPER-CDANG1` and
`SERDATR-CDANG1` are red, three of the package's `5 passed, 3 failed`; their
`-CDANG0` twins are green.

[`hrm/ch2-copper-move-sweep`](../../../hrm/ch2-copper-move-sweep), `92 passed, 1
failed`: with `ecs` low, `APPBSTAR $0C SERDATR`, `APPBSTAR $18 SERDAT` and
`APPBSTAR $19 SERPER` are rejected with CDANG clear and with it set, green, as
is `APPBSTAR LEGEND` for the whole band; with `ecs` high and CDANG set,
`SWEEP ecs=1 cdang=1 vs Appendix C` measures every one of the 256 destinations
accepted, green. The one red is `SWEEP ecs=1 cdang=0`, which is
[NM-HRM-004](../NM-HRM-004.md)'s.

---

## What the test measures

The package grades Appendix B's `*` at `ecs` high only; its specification
records that p. 310 is not among the pages its slice was assigned, and that
it grades Appendix B *"on its own terms"*
([`SPECIFICATION.md`](../../../hrm/ch8-appb-copper-access-marks/SPECIFICATION.md),
*Why this package is scoped to ECS*). The measurement itself is sound: the
package grades a `~`-marked register on the same rig, and `CTRL-BLTCON0` finds
BLTCON0 (`$040`) blocked with CDANG clear and admitted with it set, as its own
legend says, so the fixture tells the two marks apart; `LIVENESS` shows the
Copper writing DMACON at both CDANG states. The three reds measure the admission
p. 310 prescribes.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:2806-2818`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2806-L2818) | <pre>int addr = reg & 0x01fe;<br>if (addr &lt; ((copcon & 2) ? (ecs_agnus ? 0 : 0x40) : 0x80)) {</pre> floor `0` with CDANG set on an ECS Agnus, so no address is refused; `$40` with CDANG set on OCS, `$80` with it clear. `ecs_agnus` comes from the chipset mask and is cleared for the A1000 Agnus models ([`:1187-1192`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1187-L1192)); every Copper MOVE path calls the test (`:2874`, `:9134`, `:9208`) |
| **vAmiga** | [`Core/Components/Agnus/Copper/Copper.cpp:395-402`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L395-L402) | <pre>if (cdang) {<br>&nbsp;&nbsp;&nbsp;&nbsp;return agnus.isOCS() ? addr &lt; 0x40 : false;<br>} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;return addr &lt; 0x80;</pre> with CDANG set, no address is illegal unless the Agnus is one of the two OCS revisions ([`Agnus.cpp:248-251`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L248-L251)); the MOVE stops on an illegal one at [`CopperEvents.cpp:145`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperEvents.cpp#L145) |
| **Denise** | [`emulation/libami/agnus/copper.cpp:342`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L342) | <pre>cdang = (value & 2) ? (agnus.ecsAndHigher() ? 0 : 0x40) : 0x80;</pre> stores the floor rather than the bit: `0` with CDANG set on ECS or AGA ([`agnus.h:311`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.h#L311)), and a MOVE below it switches the Copper off at [`:203-207`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L203-L207) |
| **Copperline** | [`src/bus/dma_slots.rs:697-708`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L697-L708) | <pre>if off &lt;= 0x03E {<br>&nbsp;&nbsp;&nbsp;&nbsp;return !matches!(self.agnus.revision(), AgnusRevision::Ocs)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&& self.agnus.copper_danger_enabled();<br>}</pre> the band needs a non-OCS Agnus and CDANG. Its test table ([`src/bus/tests.rs:4165-4194`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L4165-L4194)) asserts `$000` and `$03E` blocked with CDANG set on OCS and reached with it set on its ECS Agnus, and a second test ([`:4210-4233`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L4210-L4233)) has the ECS Copper write COPCON itself with CDANG set. Its register reference gives the reason: "ECS/AGA extend the permitted range when CDANG is set" ([`02E-COPCON.md:12`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/docs/reference/custom-registers/02E-COPCON.md#L12)) |
| **MiSTer** | [`rtl/agnus_copper.v:225`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L225) | descendant — identical to NanoMig: `if (ir2[8:7]==2'b00 && !cdang \|\| ir2[8:6]==3'b000 && !ecs) // illegal access`, the same decode, which lifts the block when `ecs` and `cdang` are both high |
| **TC64** | [`rtl/minimig/agnus_copper.v:225`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L225) | descendant — the same line, byte for byte. The file differs from NanoMig by the same 18 lines in both Minimig lines: nine assignments to `copjmp1`, `copjmp2`, `strobe1`, `strobe2` and `strobe` (`:237-271`), non-blocking in NanoMig and blocking in both lines, which do not reach this decode |

**All four do what p. 310 and Appendix B say together**: the band closed on
OCS whatever CDANG holds, open on ECS once CDANG is set. WinUAE, vAmiga and
Denise give no reason in their code; Copperline names ECS as the reason in its
register reference and tests both chipsets. Both Minimig lines share NanoMig's
line, which confirms the ancestry, not the reading.

---

## What this is not

**Not [NM-HRM-044](../NM-HRM-044.md).** There two sentences that name no chipset,
p. 279's *"processor or Copper"* and p. 289's `*` on DSKPTH, disagree about the
standard chip set, where p. 310 changes nothing. Here no register-specific
passage stands opposite the `*`, and the passage that does, p. 310, is the
manual's own ECS extension of the rule the `*` states.

**Not [NM-HRM-004](../NM-HRM-004.md)**, which is the `~` band at ECS with CDANG
clear: there Appendix B marks rows that exist only in ECS, and those marks
contradict p. 310.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](../README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988; Commodore, *AmigaMail* vol. 1, "The Lost Blitter Docs", 1989; Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

## The AA specification marks the band `~`

**Commodore's AA functional specification** (06/07/91, PRELIMINARY) prints its
register list in address order with one Copper mark, the conditional one. Its
legend, PDF page 6, defines no `*`:

```
~=Address not writable by the Coprocessor unless COPCON bit 1 is set true.
```

and every row from `$000` to `$03E` carries it (PDF pages 6-7), these four
included:

```
SERDATR     ~018  R         P   Serial Port Data and Status read
COPCON      ~02E  W    A        Coprocessor control register (CDANG)
SERDAT      ~030  W         P   Serial Port Data and stop bits write
SERPER      ~032  W         P   Serial Port Period and control
```

Its COPCON entry, PDF pages 19-20:

```
 01     CDANG     Coprocessor danger mode. Allows coprocessor
                  access to all RGA registers if true.
                  (if 0, access to RGA>7E)
          (on old chips- access to only RGA>3E
           if CDANG=1) (see VPOSR)
```

It specifies a different chip set and does not decide what the ECS Copper does.
It states the rule every implementation follows: on the old chips CDANG opens
nothing below `$040`, on the later ones everything, these three registers
included.

---

## Why this entry is kept

Appendix B's legend is where a reader looks up whether the Copper may write a
register, and its `*` names no chipset. The ECS rule is on p. 310, reached
from chapter 2, from Appendix A's COPCON entry and from the `(E)` on COPCON's
own row. A test that grades the `*` with `ecs` high and without p. 310 is red
on NanoMig and would be red on every implementation read here; this page is
where that reader finds p. 310.
