# NM-HRM-063 — FLAG is negative edge-sensitive; the RTL ORs a level [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Question: with `flag` held at 1 across two clocked ICR reads at `ciab`'s port, should the read clear bit 4 and IR (p.346, with p.341 giving no transition to set them again), where the RTL reads `0x90` both times, so that `cia_int.v:57` sets the bit only on a negative transition of FLAG? In NanoMig `flag` is a one-cycle pulse, so the difference shows only for a driver that holds FLAG. Closes when `cia_int.v` sets bit 4 only on a negative transition of FLAG and `PROP-FLAG-EDGE-POLARITY` turns green, or when an Amiga specialist rejects that change; if the rejection means p.341 is wrong, this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer and minimig-dev/MinimigAGA-MiST-TC64 |
| **Scope** | the 8520's FLAG input and ICR bit 4 (FLG) as built into `ciab`, measured with `flag` held at 1 across a clocked ICR read (red) and with `flag` returned to 0 before the first read (control, passes); chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed p.341 (FLAG) and p.346 (ICR read clears), with p.245 (Chapter 8), p.334 and p.379 for the pin's polarity · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims `PROP-FLAG-EDGE-POLARITY`: `opus:F-handshake.c6` / `sol:F-handshake.c7` (p.341); the assertion's message cites p.346, p.245, p.334 and p.379 without a claim id, and the control carries none |
| **Module** | `cia_int.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_int.v#L57) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_int.v#L81); inherited |
| **Test** | [`hrm/appF-ciab-port-assignments`](../../hrm/appF-ciab-port-assignments): `make -C hrm/appF-ciab-port-assignments test`; an expected red exits nonzero. `PROP-FLAG-EDGE-POLARITY` (`test_appF-ciab-port-assignments.cpp:491-509`) is red at its second clocked ICR read; its control (`test_appF-ciab-port-assignments.cpp:749-759`) passes |
| **Assertions** | 1 red: `PROP-FLAG-EDGE-POLARITY` — expected bit 4 and IR clear at the second read, observed `0x90` after the transition and `0x90` again after five more ticks with `flag` held (not printed by `make test`; from a run that printed the ICR reads); the package reports `7 passed, 3 failed` |
| **Verdict of the four** | **unanimous, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline all set the bit once per index pulse, on a transition — vAmiga and Copperline name it the pin's falling edge, the transition at which NanoMig's `flag` rises — and none of them from a held level. Hardware: not measured; the 8520R4 dissection, one revision, agrees with pp. 341 and 346 |
| **Effort** | none; if an Amiga specialist decides the Minimig line carries an edge detector for FLAG, S: `cia_int.v` only: a previous-state register for `flag`, with the set at line 57 and the clear at line 49 made edge-based; `ciab.v` already builds the same pattern for its TOD input (`tick_del` at `ciab.v:166-169`, used as `tick & ~tick_del` at `ciab.v:246`) |

---

## Source requirement

Printed page 341, on the 8520's handshaking and interrupt inputs:

> FLAG is a negative edge-sensitive input that can be used for receiving the PC
> output from another 8520 or as a general-purpose interrupt input. **Any
> negative transition on FLAG will set the FLAG interrupt bit.**

Printed page 346 says what a read of the interrupt data register does:

> When you read the DATA register, its contents are cleared (set to 0), and the
> IRQ line returns to a high state.

The requirement is a *transition*, and specifically a negative one; p.341 is the
only page in the corpus that calls FLAG edge-sensitive. CIA-B's FLAG is the disk
index; Chapter 8, p.245:

> FLAG DSKINDEX\* Disk index pulse ($BFDD00, bit 4). Can be used to create a
> level 6 interrupt.

p.334 has the interrupt come *"whenever the INDEX\* input goes active"*, and `*`
marks an active-low signal (p.379). So the negative transition is INDEX\* going
active, the start of each index pulse. Together: a negative transition sets the bit, a read clears
it, and while FLAG stays low after that read nothing sets it again.

## RTL and measurement

`cia_int.v`, the latching branch:

```verilog
icr[4] <= icr[4] | flag;   // external flag input
```

A level-sensitive OR on an active-high line. There is no edge detector and no
previous-state register for `flag` in the module, so it cannot distinguish a
transition from a level. The clearing branch, eight lines up, is
`icr[4] <= flag;`, so a read while `flag` is still high re-sets the bit in the
same cycle: a held level survives the read that p.346 says clears it.

`flag` is 1 while INDEX\* is active and 0→1 is the negative transition. Its one
driver is `index` (`minimig.v:647`, `paula_floppy.v:576`), one `clk7_en` cycle
wide (`agnus.v:485`, `agnus_beamcounter.v:374`), and CIA-A's `flag` is tied to
`1'b0` (`ciaa.v:321`), so in NanoMig `flag` is never held and the bit is set
once per pulse, as the page predicts.

`PROP-FLAG-EDGE-POLARITY` (`test_appF-ciab-port-assignments.cpp:491-509`) runs
on the `ciab` fixture and reads ICR only through `read_icr_clocked()`
(`test_appF-ciab-port-assignments.cpp:320-335`): a CPU read cycle with `aen=1`,
`rd=1`, `wr=0`, `rs=4'hD`, then one rising `clk` edge with `clk7_en=1`.

1. After reset with `flag=0`, write ICR `0x90` (set FLG in the mask).
2. Drive `flag` 0→1, the negative transition, and hold it three ticks. Read
   ICR: bit 4 must be **set** (p.341).
3. Keep `flag` at 1 for five more ticks, no transition. Read ICR again: bit 4
   must be **clear**, and bit 7 (IR) with it (p.346, *"When you read the DATA
   register, its contents are cleared"*; p.341 gives nothing that sets it
   again).

Measured, not printed by `make test`; from a run that printed the ICR reads:
the step-2 read returns `0x90`, as p.341 requires; the step-3 read returns
`0x90` again and the assertion is red there. Step 3 separates edge from
level: a level-OR sets bit 4 again at the read (`cia_int.v:49`) and on every
cycle after it (`cia_int.v:57`); an edge detector does not. The control
(`test_appF-ciab-port-assignments.cpp:749-759`) runs the same steps with `flag`
returned to 0 before the first read, is reported rather than graded, and
passes, in the same run, not printed by `make test`: `0x90`, then `0x00`. So a set bit at step 3 is the held level and
nothing else.

## Risks and unknowns

- Whether a real Amiga ever holds FLAG at a level long enough for this to
  matter. On CIA-B the pin is the floppy index pulse; but p.341 offers the other
  use, *"receiving the PC output from another 8520 or as a general-purpose
  interrupt input"*, where a held level is plausible. In NanoMig it is not
  observable at the pins.
- Whether `flag` is meant as the pin at all: `cia_int.v:8-12` describes it as an
  input that sets its ICR bit, like `ta`, `tb` and `alrm`, and its one driver
  delivers a single-cycle event.
- The four agree with the page, but only Copperline can hold the pin active
  across an ICR read; the other three cannot represent a held level.
- The 6526 specification is not an 8520 document. The dissection is of one
  revision, and its ICR cell is drawn for bit 3 with bit 4's taken to be alike.

## Independent evidence

| | | |
|---|---|---|
| **WinUAE** | an event; no level input exists | `cia.cpp:823-825`: `cia_diskindex()` calls `CIA_sync_interrupt(1, ICR_FLAG)`, and `disk.cpp:3973` calls it once per index pulse |
| **vAmiga** | explicit, in two separate methods | `CIA.cpp:250-266`: `emulateRisingEdgeOnFlagPin()` only calls `wakeUp()`; `emulateFallingEdgeOnFlagPin()` calls `wakeUp()` and then `icr \|= 0x10` |
| **Denise** | an event; no level input exists | `emulation/cia/new/cia.cpp:302-304`'s `setFlag()` sets `intIncomming \|= 0x10`, called from `libami/drive/diskDrive.cpp:79` once per revolution, when the head wraps past the end of a track |
| **Copperline** | an explicit edge detector over retained pin state | `src/chipset/cia.rs:373-380`: `set_flag_pin` computes `let falling_edge = self.flag_pin_high && !high;` and latches `ICR_FLG` only then. `assert_flag()` is `self.set_flag_pin(false)` (`cia.rs:365-366`), called at `src/bus.rs:8056` and followed by `release_flag()`. Its own test holds the pin low across a read: a second `assert_flag()` with no release between leaves ICR reading 0 (`cia.rs:1496-1499`) |
| **MiSTer** | descendant — same statement, in an otherwise-rewritten file | `rtl/cia_int.v:81`: `icr[4] <= icr[4] \| flag;`; 115 of the file's lines differ from NanoMig, nearly all added commentary, and this one is unchanged by it |
| **TC64** | descendant — byte-identical to NanoMig | `rtl/minimig/cia_int.v:57`: identical line for line, down to the "intterupts" typo in the comment on the ICR-clear branch, so a fix is one clean edit there; MiSTer's file has drifted around the same statement |

Not one of the four sets the bit from a level: WinUAE and Denise have only the
event, vAmiga only its two edge methods, and Copperline, which models the pin as
a level, detects the edge explicitly. Copperline is the closest structural
comparison and keeps the previous-state bit `cia_int.v` lacks.

### What other Commodore documents say

- **Commodore, 6526 CIA specification, 1981**, PDF page 6 — agrees with the manual:

  > "Any negative transition on FLAG will set the FLAG interrupt bit."
  <!-- corpus: cia-6526-spec/p006.txt -->

  The datasheet states the manual's p.341 rule almost word for word, 6526 where the manual has 8520: FLAG is 'a negative edge sensitive' input and a transition, not a level, sets the FLAG bit. It also has a read of the ICR data register clear it (p007), and on its edge rule a held level would not set the bit again.

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, schematic sheets 5a, 10a and 10b — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 5a, drawn transistor by transistor, takes the FLAG# pad through an inverting Schmitt trigger and a chain of five inverters; a NOR of the pad's level and the delayed one gives a short pulse only when the pad falls. A latch catches the pulse and is cleared by FLAG_INT, which a PHI2 and a PHI1 stage then raise for one cycle; a pad held low gives no further pulse. On sheet 10a, drawn for the cell of ICR bit 3, the flag latch is set by its interrupt input, sampled on PHI2 and PHI1, and cleared by RES or IFR_CLR, and the bit reads through R_ICR; on sheet 10b IFR_CLR rises in the PHI1 after a PHI2 in which the ICR read strobe R_ICR was active, and clears the IR latch as well.
  <!-- drawing: cia-8520r4-dissection/sheet_006.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_015.txt -->

  FLAG is edge-sensitive: each negative transition sets the FLAG bit once, and a read of the ICR clears it in the next PHI1, which a pad held low does not undo, as pp. 341 and 346 print. A source still high at the moment of the clear would keep its bit set, but FLAG_INT is high for one cycle per edge.

## Decision

If the Minimig line carries an edge detector for FLAG, the fix lands in both
MiSTer's and TC64's `cia_int.v` and is ported to NanoMig; the step-3 read then
returns bit 4 and IR clear and `PROP-FLAG-EDGE-POLARITY` turns green, with step 2
still reading `0x90`.
