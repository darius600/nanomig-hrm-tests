# NM-HRM-004 — p. 310 gives the ECS Copper $03E–$07E with CDANG clear, Appendix B marks that range closed, and every implementation keeps it closed

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The red is against p. 310, not against the RTL, and the entry exists so the next reader does not take it for an implementation fault. Question: with CDANG clear, does the ECS Copper write `$040`–`$07E`, as Appendix C p. 310 prints, or is it blocked below `$080`, as Appendix B marks every address in that range? Closes when an Amiga specialist records that p. 310's COPCON sentence does not describe the ECS Copper (Appendix B's marks hold), or confirms that the ECS Copper writes `$040`–`$07E` with CDANG clear, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Appendices B and C of the *Amiga Hardware Reference Manual*, 3rd ed. The manual contradicts itself; NanoMig, both Minimig lines, all four implementations and Commodore's AA specification block the range with CDANG clear |
| **Scope** | Agnus with `ecs = 1`, Copper MOVE with CDANG clear, byte addresses `$040`–`$07E` (32 addresses). `$03E` is isolated: p. 310 reads two ways there, NanoMig does not write it, and no assertion grades it. At `ecs = 0` NanoMig matches Appendix B. Chip revision unknown; no hardware capture cited |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix C, printed p. 310 (ECS COPCON) against Appendix B, printed pp. 287–290 (legend p. 287, `(E)` p. 288, rows pp. 289–290) and p. 298 · *Amiga Hardware Reference Manual*, 3rd ed., 1991. Claim ids carried by the red assertions: `T16`: `ch2-appc-ecs.c21`, `COP2-copcon-ecs.c13`; `SWEEP ecs=1 cdang=0`: `COP2-copcon-ecs.c14`, `ch2-appc-ecs.c22`. `T9` carries no claim id. None of the three carries an `opus:` or `sol:` id |
| **Module** | `agnus_copper.v:225` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L225) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L225); inherited (MiSTer and TC64 have the same line) |
| **Test** | [`hrm/ch2-copper-move-range`](../../hrm/ch2-copper-move-range): `make -C hrm/ch2-copper-move-range test`; [`hrm/ch2-copper-move-sweep`](../../hrm/ch2-copper-move-sweep): `make -C hrm/ch2-copper-move-sweep test`; green control for Appendix B's reading: [`hrm/ch8-appb-copper-access-marks`](../../hrm/ch8-appb-copper-access-marks): `make -C hrm/ch8-appb-copper-access-marks test`. An expected red exits nonzero |
| **Assertions** | 3 red. `T9` in `hrm/ch2-copper-move-range`: expected a MOVE to BLTCON0 (`$040`) to be written, observed it not written. `T16` in the same package: expected a MOVE to DSKSYNC (`$07E`) to be written, observed it not written; the package reports `14 passed, 2 failed`. `SWEEP ecs=1 cdang=0` in `hrm/ch2-copper-move-sweep`: expected accepted set `{$20-$ff}` in DA8-1 units (byte `$040` up), observed `{$40-$ff}` (byte `$080` up), 32 falsely rejected, none falsely accepted; the package reports `97 passed, 1 failed` |
| **Verdict of the four** | **unanimous with NanoMig and with Appendix B**: WinUAE, vAmiga, Denise and Copperline all block a Copper MOVE below `$080` while CDANG is clear, whatever the chipset; none cites Appendix C, and Copperline asserts `$040` and `$07E` blocked on its ECS Agnus in a test. Hardware: no capture cited |
| **Effort** | none — no RTL change; the open question concerns p. 310's last COPCON sentence |

---

## Source requirement

**The manual says two things about the ECS Copper with CDANG clear, and they
cannot both hold.**

**Appendix C, printed p. 310** (`appC-ecs/p310.txt:33-36`):

> "If this bit is clear, the Copper can access the address range
> from $DFF03E through $DFF07E, the same range as when the danger bit is set in
> the standard chip set."

The Developer-CD text and the OCR of the printed book give the sentence word for word.

**Appendix B, printed pp. 287-290,** the register summary in address order. The
legend (`appB3/p287.txt:24`):

> `~     Address not writable by the Copper unless the "copper danger bit",
> COPCON is set true.`

Every row from `$040` to `$07E` carries that mark, all 32 addresses, the reserved
`$068`–`$06E`, `$076` and `$07A` included (`appB3/p289.txt:41-66`,
`appB3/p290.txt:7-12`). Seven of those rows carry the table's own `(E)`: BLTCON1 and BLTAPTH, which the
standard chip set has too (`appB3/p289.txt:42`, `:49`), and these five
(`appB3/p289.txt:54-56`, `appB3/p290.txt:9`, `appB3/p290.txt:11`); the rendered scan
of pp. 289-290 carries the `~` on all five:

> `BLTCON0L     ~05A    w    A (E)      Blitter control 0, lower 8 bits (minterms)`
>
> `BLTSIZV      ~05C    w    A (E)      Blitter V size (for 15 bit vertical size)`
>
> `BLTSIZH      ~05E    w    A (E)      Blitter H size and start (for 11 bit H size)`
>
> `SPRHDAT      ~078     W   A (E)       Ext. logic UHRES sprite pointer and data id`
>
> `DENISEID     ~07C     R   D(E)        Chip revision level for Denise (video out chip)`

p. 288 says what `(E)` means (`appB3/p288.txt:24-27`): *"Registers denoted with an
"(E)" in the chip column means that those registers have been changed in the
Enhanced Chip Set (ECS)."* Appendix C's register map lists four of the five as `new`
in ECS (`appC-audio/p298.txt:22-25`), for example
`05A  BLTCON0L new   W  A   Blitter control 0, lower 8 bits (minterms)`. In the
standard chip set there is no register at `$05A`, `$05C`, `$05E` or `$07C`, so on
those rows the `~` can describe only ECS. **For those addresses Appendix B says a
Copper MOVE with CDANG clear does not write, and p. 310 says it does.**

**p. 310 ties Appendices A and B to the standard chip set** in its last sentence
(`text/p310.txt:35-36`): *"For more information on the AUDxPER register, and any
other register in the Amiga standard chip set, see Appendices A and B."*

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

The finding key names the pair that disagrees, NanoMig and p. 310, and does not
prejudge which side is wrong.

## RTL and measurement

NanoMig rejects that range ([`agnus_copper.v:225`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_copper.v#L225)):

```verilog
if (ir2[8:7]==2'b00 && !cdang || ir2[8:6]==3'b000 && !ecs) // illegal access
```

With `CDANG` clear the accepted set is `$080`–`$1FE` at `ecs = 0` and `ecs = 1`
alike, which is what Appendix B's marks say. Measured: 32 addresses falsely
rejected, `$040`–`$07E`; none falsely accepted. Two single-address probes and an
exhaustive sweep agree: `T9` moves to `$040` and `T16` to `$07E`, and neither is
written. `T16` also records `$03E`, where p. 310 reads two ways (its literal range
admits it, "the same range as when the danger bit is set" does not), with both
readings, as the sweep does; NanoMig does not write it. Addresses here are the
manual's byte addresses; a Copper instruction encodes DA8-1, so the module's ports
carry them halved and the sweep prints them that way.

**Appendix B's reading is green on the same fixture.**
[`hrm/ch8-appb-copper-access-marks`](../../hrm/ch8-appb-copper-access-marks) drives a
Copper MOVE to `$040` at `ecs = 1` in its `CTRL-BLTCON0` and asserts it blocked with
CDANG clear and admitted with CDANG set; it passes. `T9` asserts the opposite at the
same address, chipset and CDANG state, so no implementation can make both green.

## Risks and unknowns

- **p. 310 is the only page that states the ECS Copper's range.** Chapter 2 and
  Appendix A send the reader to Appendix C for ECS, and p. 310 sends the reader to
  Appendices A and B for "any other register in the Amiga standard chip set". Read
  that way, Appendix B's column describes the standard chip set and p. 310 overrides
  it for ECS.
- **The marks are an address range, printed without exception.** Every row from
  `$000` to `$03E` carries `*` and every row from `$040` to `$07E` carries `~`,
  reserved addresses included, and the `*` does not hold under ECS with CDANG set,
  by p. 310's own sentence and in every implementation ([NM-HRM-053](withdrawn/NM-HRM-053.md),
  withdrawn for that reason). The `~` on the `(E)` rows may be the range's mark
  carried onto new rows rather than a statement about ECS.
- **The five `(E)` rows are graded for ECS only as accepted with CDANG set.** The
  sweep grades Appendix B's `~` row by row at `ecs = 0` (`APPBTILDE $2D`, `$2E`,
  `$2F`, `$3C`, `$3E`), where four of those registers do not exist. At `ecs = 1` the
  `APPBTILDE-ECS1-*` assertions grade them admitted with CDANG set and only record
  the CDANG-clear result; the `ecs = 1` CDANG-clear half is carried by the sweep's
  `SWEEP ecs=1 cdang=0` line.
- **No hardware capture is cited**; the implementations and the AA specification
  side with NanoMig either way.

## Independent evidence

| | where | result |
|---|---|---|
| **WinUAE** | [`custom.cpp:2809`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2809) | floor `$80` when the danger bit is clear, on every chipset; the one test every Copper MOVE path calls (`:2874`, `:9134`, `:9208`) |
| **vAmiga** | [`Core/Components/Agnus/Copper/Copper.cpp:395-402`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L395-L402) | blocks below `$80` when the danger bit is clear, whatever the chipset; the MOVE stops on it at [`CopperEvents.cpp:145`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperEvents.cpp#L145) |
| **Denise** | [`emulation/libami/agnus/copper.cpp:342`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/copper.cpp#L342) | floor `$80` when the danger bit is clear |
| **Copperline** | [`src/bus/dma_slots.rs:697-705`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/dma_slots.rs#L697-L705) | needs the bit **set** for `$040`–`$07E`, on every revision; its reference for the gate is chapter 2 ([`docs/internals/timing.md:411-413`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/docs/internals/timing.md#L411-L413)); its test table asserts `$040` and `$07E` blocked with CDANG clear on `Ecs8372Rev4` ([`src/bus/tests.rs:4181-4194`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/tests.rs#L4181-L4194)) |
| **MiSTer** | [`rtl/agnus_copper.v:225`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_copper.v#L225) | descendant: identical to NanoMig, the same CDANG-clear range |
| **TC64** | [`rtl/minimig/agnus_copper.v:225`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_copper.v#L225) | descendant: the same line, byte for byte; its one difference in the file (blocking where NanoMig assigns non-blocking, lines 234-274) does not reach the CDANG decode |

None of the four cites Appendix C.

**Commodore's AA functional specification** (06/07/91, PRELIMINARY) specifies a
different chip set, so it does not decide what the ECS Copper does; beside the HRM it
is the only Commodore document on this page that states the CDANG-clear range, and
it states NanoMig's. Its COPCON entry, PDF pages 19-20:

```
                     01     CDANG     Coprocessor danger mode. Allows coprocessor
                                      access to all RGA registers if true.
                                      (if 0, access to RGA>7E)
                              (on old chips- access to only RGA>3E
                               if CDANG=1) (see VPOSR)
```

With CDANG clear, only addresses above `$7E`; with it set, all of them, and on "old
chips" only those above `$3E`. The entry is marked `h`, *"stuff added or changed in
hires chips"* (p. 10); the address list marks `$040`–`$07E` `~` as Appendix B does
(pp. 6-7) and puts `h`, *"new for HiRes chip set"* (p. 6), on `BLTCON0L`, `BLTSIZV`
and `BLTSIZH` (p. 7), three of the registers HRM p. 298 lists as new in ECS.

## Decision

**Not an RTL change.** NanoMig already does what Appendix B marks and what every
implementation does. The question is for someone who knows the ECS Agnus: whether
p. 310's last COPCON sentence describes it. If it does not, `T9`, `T16` and the
`ecs=1 cdang=0` sweep grade a sentence the manual itself contradicts on pp. 289-290.
If p. 310's reading is upheld, a fix diverges from neither Minimig line. Re-run:
`make -C hrm/ch2-copper-move-range test` and `make -C hrm/ch2-copper-move-sweep test`
(an expected red exits nonzero).
