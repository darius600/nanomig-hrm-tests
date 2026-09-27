# NM-HRM-004 — p. 310 gives the ECS Copper $03E–$07E with CDANG clear, Appendix B marks that range closed, and every implementation keeps it closed

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The manual says two things about the ECS Copper with CDANG clear: Appendix C p. 310 lets it write `$03E`–`$07E`, and Appendix B marks every address from `$040` to `$07E`, the rows of registers that exist only in ECS included, as not writable by the Copper unless CDANG is set. NanoMig follows Appendix B, and so do all four implementations. The question is which page describes the ECS Copper; the entry exists so the next reader does not take the red for an implementation fault. |
| **Module** | `agnus_copper.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v) |
| **Report to** | **the manual, not the implementation.** NanoMig, both Minimig lines, all four implementations and Commodore's AA specification block the range with CDANG clear |
| **Test** | [`hrm/ch2-copper-move-range`](../../hrm/ch2-copper-move-range), [`hrm/ch2-copper-move-sweep`](../../hrm/ch2-copper-move-sweep) |
| **Chapter** | 2 — Coprocessor · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 3 red: `T9` (`$040`) and `T16` (both ends Appendix C prints, `$03E` and `$07E`, claim `ch2-appc-ecs.c21`) in `hrm/ch2-copper-move-range`, and the `ecs=1 cdang=0` sweep in `hrm/ch2-copper-move-sweep` |
| **Verdict of the four** | **unanimous with NanoMig and with Appendix B**: WinUAE, vAmiga, Denise and Copperline all block a Copper MOVE below `$080` while CDANG is clear, whatever the chipset; none cites Appendix C, and Copperline asserts `$040` and `$07E` blocked on its ECS Agnus in a test |

---

## What the HRM says

**The manual says two things about the ECS Copper with CDANG clear, and they
cannot both hold.**

**Appendix C, printed p. 310:** with ECS and `CDANG` **clear**, the Copper reaches
`$DFF03E`–`$DFF07E`:

> "the same range as when the danger bit is set in the standard chip set"

The whole sentence pair (`appC-ecs/p310.txt:33-36`):

> "Now, in the ECS, if this bit is set, the Copper can access all of the Amiga
> chip registers. If this bit is clear, the Copper can access the address range
> from $DFF03E through $DFF07E, the same range as when the danger bit is set in
> the standard chip set."

**Appendix B, printed pp. 287-290,** the register summary in address order.
The legend (`appB3/p287.txt:24`):

> `~     Address not writable by the Copper unless the "copper danger bit",
> COPCON is set true.`

Every row from `$040` to `$07E` carries that mark, all 32 addresses, the reserved
`$068`–`$06E`, `$076` and `$07A` included (`appB3/p289.txt:41-66`,
`appB3/p290.txt:7-12`). Seven of those rows carry the table's own `(E)`: BLTCON1
and BLTAPTH, which the standard chip set has too (`appB3/p289.txt:42`, `:49`),
and these five (`appB3/p289.txt:54-56`, `appB3/p290.txt:9`, `appB3/p290.txt:11`):

> `BLTCON0L     ~05A    w    A (E)      Blitter control 0, lower 8 bits (minterms)`
>
> `BLTSIZV      ~05C    w    A (E)      Blitter V size (for 15 bit vertical size)`
>
> `BLTSIZH      ~05E    w    A (E)      Blitter H size and start (for 11 bit H size)`
>
> `SPRHDAT      ~078     W   A (E)       Ext. logic UHRES sprite pointer and data id`
>
> `DENISEID     ~07C     R   D(E)        Chip revision level for Denise (video out chip)`

and p. 288 says what `(E)` means (`appB3/p288.txt:24-27`):

> "About the ECS registers. Registers denoted with an "(E)" in the chip column
> means that those registers have been changed in the Enhanced Chip Set (ECS).
> ... Certain ECS registers are completely new, others have been extended in
> their functionality."

Appendix C's register map lists four of the five as `new` in ECS
(`appC-audio/p298.txt:22-25`):

> `05A  BLTCON0L new   W  A   Blitter control 0, lower 8 bits (minterms)`
>
> `07C  DENISEID new   R  D   Chip revision level for Denise (video out chip)`

In the standard chip set there is no register at `$05A`, `$05C`, `$05E` or `$07C`,
so on those rows the `~` can describe only ECS. **For those addresses Appendix B
says a Copper MOVE with CDANG clear does not write, and p. 310 says it does.**
The rendered scan of pp. 289-290 carries the `~` on all five rows.

**p. 310 itself ties Appendices A and B to the standard chip set** in its last
sentence (`text/p310.txt:35-36`):

> "For more information on the AUDxPER register, and any other register in the
> Amiga standard chip set, see Appendices A and B."

That is the one sentence that reads Appendix B's column as a standard-chip-set
column; p. 288 and the `(E)` rows are the other side.

**Chapter 2 and Appendix A defer ECS to Appendix C.** Chapter 2 gives the CDANG range
without a chipset, `$40` to `$7E`, and *"When CDANG is 0, these registers cannot
be written by the Copper"* (`ch2/p26.txt:31-35`); it sends ECS to Appendix C
(`ch2/p36.txt:51-52`), and so does Appendix A's COPCON entry (`appA-copper/p274.txt:13-14`).

**p. 310's lower end, `$03E`, is outside every other statement of the range.**
p. 26 puts `$00` to `$3E` among the registers the Copper *"cannot affect at all"*
(`ch2/p26.txt:26-27`), and Appendix B marks `STRLONG` at `$03E`, itself an `(E)`
row, `*`: not writable by the Copper at all (`appB3/p289.txt:40`). p. 310's own
standard-chip-set sentence, CDANG set reaching `$DFF03E` (`appC-ecs/p310.txt:31-33`),
disagrees with both.

---

## What NanoMig does

It rejects that range. With `CDANG` clear the accepted set is `$080`–`$1FE` at
`ecs = 0` and `ecs = 1` alike, which is what Appendix B's marks say.

### Measured

| | |
|---|---|
| falsely rejected | 32 addresses, `$040`–`$07E` |
| falsely accepted | none |

Two single-address probes and an exhaustive sweep agree. `T9` moves to `$040`,
`T16` to both ends Appendix C prints, `$03E` and `$07E`, and none of the three
addresses is written. The table is the sweep's, whose expected set starts at
`$040`; `$03E` is the one address the appendix names that only `T16` probes.

*Addresses here are the manual's byte addresses. A Copper instruction encodes
DA8-1, so the module's register ports carry them halved and the sweep prints them
that way.*

**Appendix B's reading is green on the same fixture.**
[`hrm/ch8-appb-copper-access-marks`](../../hrm/ch8-appb-copper-access-marks)
grades the `~` at the corner `T9` grades: its `CTRL-BLTCON0` drives a Copper MOVE
to `$040` at `ecs = 1` and asserts it blocked with CDANG clear and admitted with
CDANG set, and it passes. `T9` asserts the opposite at the same address, chipset
and CDANG state. No implementation can make both green, and NanoMig makes the
Appendix B one green.

The sweep grades Appendix B's `~` row by row, the five rows quoted above included
(`APPBTILDE $2D`, `$2E`, `$2F`, `$3C`, `$3E`), at `ecs = 0` only, a configuration
in which four of those registers do not exist; no assertion grades those five
rows at `ecs = 1`.

---

## What the others say

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:2809`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2809) | <pre>if (addr &lt; ((copcon & 2) ? (ecs_agnus ? 0 : 0x40) : 0x80)) {</pre> floor `$80` when the danger bit is clear, on every chipset; the one test every Copper MOVE path calls (`:2874`, `:9134`, `:9208`) |
| **vAmiga** | [`Core/Components/Agnus/Copper/Copper.cpp:395-402`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L395-L402) | <pre>if (cdang) {<br>&nbsp;&nbsp;&nbsp;&nbsp;return agnus.isOCS() ? addr &lt; 0x40 : false;<br>} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;return addr &lt; 0x80;</pre> blocks below `$80` when the danger bit is clear, whatever the chipset; the MOVE stops on it at [`CopperEvents.cpp:145`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperEvents.cpp#L145) |
| **Denise** | [`emulation/libami/agnus/copper.cpp:342`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L342) | <pre>cdang = (value & 2) ? (agnus.ecsAndHigher() ? 0 : 0x40) : 0x80;</pre> floor `$80` when the danger bit is clear |
| **Copperline** | [`src/bus/dma_slots.rs:697-705`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L697-L705) | <pre>pub(super) fn copper_can_write_custom(&self, off: u16) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;let off = off & 0x01FE;<br>&nbsp;&nbsp;&nbsp;&nbsp;if off &lt;= 0x03E {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return !matches!(self.agnus.revision(), AgnusRevision::Ocs)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&& self.agnus.copper_danger_enabled();<br>&nbsp;&nbsp;&nbsp;&nbsp;}<br>&nbsp;&nbsp;&nbsp;&nbsp;if (0x040..=0x07E).contains(&off) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return self.agnus.copper_danger_enabled();</pre> needs the bit **set** for `$040`–`$07E`, on every revision |
| **MiSTer** | [`rtl/agnus_copper.v:225`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L225) | descendant — identical to NanoMig: `if (ir2[8:7]==2'b00 && !cdang \|\| ir2[8:6]==3'b000 && !ecs) // illegal access`, the same CDANG-clear range |
| **TC64** | [`rtl/minimig/agnus_copper.v:225`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L225) | descendant — the same line, byte for byte. The only difference either line has from NanoMig in this file (blocking where NanoMig assigns non-blocking, lines 234-274) does not reach the CDANG decode. If p. 310's reading is upheld, a fix diverges from neither line |

**All four independent implementations do what Appendix B marks and what
NanoMig does**: CDANG clear blocks every address below `$080` on every chipset,
and CDANG set opens `$040` on OCS and everything on ECS. None of them cites
Appendix C. Copperline names chapter 2 of the HRM as its reference for the gate
([`docs/internals/timing.md:411-413`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/docs/internals/timing.md#L411-L413)),
and its test table asserts `$040` and `$07E` blocked with CDANG clear on its ECS
Agnus, revision `Ecs8372Rev4`
([`src/bus/tests.rs:4181-4194`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L4181-L4194)).

---

## Why this is filed against the page

The manual contradicts itself, and NanoMig takes Appendix B's side. p. 310 gives
the ECS Copper `$03E`–`$07E` with CDANG clear; Appendix B marks every address
from `$040` to `$07E` `~`, four of them registers p. 298 lists as new in ECS.
NanoMig blocks the range, and so do both Minimig lines,
all four independent implementations and Commodore's AA specification. Nothing
read here lets the Copper write below `$080` with CDANG clear on any chipset.

The finding key names the pair that disagrees, NanoMig and p. 310, and does not
prejudge which side is wrong.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988; Commodore, *Denise* specification, part 252126-01 (ECS), 1988; US patent 4,874,164, Commodore-Amiga, filed 1986, granted 1989.

## The AA specification says what NanoMig does

**Commodore's AA functional specification** (06/07/91, PRELIMINARY) specifies
the AA chip set, and its COPCON entry states the rule the implementations
follow, PDF pages 19-20:

```
COPCON   h 02E  W   A        Coprocessor control register
                    This is a 1 bit register that when set true, allows
                    the Coprocessor to access the Blitter hardware. This
                    bit is cleared by power on reset, so that the
                    Coprocessor cannot access the Blitter hardware.
                    BIT#    NAME      FUNCTION
                    ----    -------   ------------------
                     01     CDANG     Coprocessor danger mode. Allows coprocessor
                                      access to all RGA registers if true.
                                      (if 0, access to RGA>7E)
                              (on old chips- access to only RGA>3E
                               if CDANG=1) (see VPOSR)
```

With CDANG clear, only addresses above `$7E`; with it set, all of them, and on
"old chips" only those above `$3E`. The entry is marked `h`, *"stuff added or
changed in hires chips"* in the legend of the section it sits in (p. 10); the
address list puts `h`, *"new for HiRes chip set"* (p. 6), on `BLTCON0L`,
`BLTSIZV` and `BLTSIZH` (p. 7), three of the registers HRM p. 298 lists as new
in ECS. That address list marks `$040`–`$07E` `~` as Appendix B does (pp. 6-7).

It specifies a different chip set and does not decide what the ECS Copper does.
Beside the HRM it is the only Commodore document on this page that states the
CDANG-clear range, and it states NanoMig's.

---

## Suggested change

**Not an RTL change.** NanoMig already does what Appendix B marks and what every
implementation does. The question is for someone who knows the ECS Agnus:
whether p. 310's last COPCON sentence describes it. If it does not, `T9`, `T16`
and the `ecs=1 cdang=0` sweep grade a sentence the manual itself contradicts on
pp. 289-290.

---

## What holds the finding up

**Both printed statements are quoted above, and they disagree over named
addresses, not over an inference.**

- **p. 310 carries in both readings of the 3rd edition.** The Developer-CD text
  and the OCR of the printed book give the sentence word for word.
- **Appendix B's side carries on the scan.** The `~` stands before `05A`, `05C`,
  `05E`, `078` and `07C` on the rendered pp. 289-290, and all 32 addresses from
  `$040` to `$07E` appear in the table with it.
- **The measured side** has the single-address probe and the exhaustive sweep
  agreeing, and `CTRL-BLTCON0` green on the same fixture for Appendix B's reading.
- **Every implementation read here, and the AA specification,** block the range
  with CDANG clear.

## What weakens it

- **p. 310 is the only page that states the ECS Copper's range.** Chapter 2 and Appendix
  A send the reader to Appendix C for ECS, and p. 310 sends the reader to
  Appendices A and B for "any other register in the Amiga standard chip set".
  Read that way, Appendix B's column describes the standard chip set and p. 310
  overrides it for ECS.
- **The marks are an address range, printed without exception.** Every row from
  `$000` to `$03E` carries `*` and every row from `$040` to `$07E` carries `~`,
  reserved addresses included, and the `*` does not hold under ECS with CDANG set,
  by p. 310's own sentence and in every implementation ([NM-HRM-053](NM-HRM-053.md),
  withdrawn for that reason). The `~` on the `(E)` rows
  may be the range's mark carried onto new rows rather than a statement about ECS.
  The implementations and the AA specification side with NanoMig either way.
