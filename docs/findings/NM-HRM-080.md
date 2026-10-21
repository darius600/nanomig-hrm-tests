# NM-HRM-080 — with the motor on, RDY* is asserted for a drive with no disk in it, where pp. 334 and 368 make it mean a disk installed and up to speed [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `paula_floppy.v` hold RDY* deasserted for a selected drive whose motor latch is on and which has no disk, where pp. 334 and 368 print "disk installed and up to speed"? Closes when `_ready` reads the motor latch and the disk and `RDY-DISK-PRESENCE`, `RDY-DISK-PRESENCE-DRIVE1`, `sol:E-disk.c26`, `opus:E-extdisk-iface.c4` and `P-READY-MOTOR-INVARIANT-DRIVE1` are green, or when an Amiga specialist rejects the reading; if that means pp. 334 and 368 are wrong, the page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | Paula's floppy interface: RDY* (CIAA PRA bit 5) of a selected drive with its motor latched on and no disk. Graded on drives 0 and 1, replayed on drives 1, 2 and 3 (a measurement for this page, not an assertion); the motor-off identification stream is left as it is. Chip revision: unknown; real hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | Appendix E, printed pp. 334–335; Appendix H, printed pp. 368–371; 8 — Interface Hardware, printed p. 244 (Table 8-5) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `RDY-DISK-PRESENCE`: `opus:AHJ-060`, `sol:H-036` (p. 368); `sol:E-disk.c26`, `opus:E-extdisk-iface.c4` (p. 334, pin 1 RDY*); `RDY-DISK-PRESENCE-DRIVE1` and `P-READY-MOTOR-INVARIANT-DRIVE1` carry no claim id of their own |
| **Module** | `paula_floppy.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L688-L694) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L386-L392); inherited (the same statement in MiSTer and TC64) |
| **Test** | [`hrm/appH-drive-sense-and-motor`](../../hrm/appH-drive-sense-and-motor): `make -C hrm/appH-drive-sense-and-motor test`; [`hrm/appDE-disk-interface`](../../hrm/appDE-disk-interface): `make -C hrm/appDE-disk-interface test`; an expected red exits nonzero |
| **Assertions** | 5 red, each requiring the drive's motor latch, read back through `index` right after the select edge, to be on before it grades RDY*; it reads on in all five. In `hrm/appH-drive-sense-and-motor` (`8 passed, 2 failed.`, 2 red of 8 graded, plus a fixture-liveness check for each drive): `RDY-DISK-PRESENCE`, drive 0, expected RDY* to differ between no disk and a disk mounted, observed asserted in both (`test_appH-drive-sense-and-motor.cpp:413-456`); `RDY-DISK-PRESENCE-DRIVE1`, the same on drive 1 (`:494-528`). Green in the same run: `CHANGE-NO-CLEAR-NO-DISK` reads the fixture's disk presence through `_change`, `INDEX-SEL-MOTOR-GATE` its motor latch through `index`. In `hrm/appDE-disk-interface` (`13 passed, 3 failed.`, 3 red of 14 graded, plus a fixture-liveness check for each drive): `sol:E-disk.c26` and `opus:E-extdisk-iface.c4`, drive 0, no disk, expected RDY* deasserted, observed asserted (`test_appDE-disk-interface.cpp:343-404`); `P-READY-MOTOR-INVARIANT-DRIVE1`, the same on drive 1 (`:443-481`) |
| **Verdict of the four** | **unanimous against NanoMig.** Once the motor is up to speed, WinUAE, vAmiga, Denise and Copperline assert RDY* only for a drive with a disk in it; WinUAE, Denise and Copperline never assert it for an empty drive with the motor on, and vAmiga only while its identification mode lasts through spin-up. With the motor off all four give an external drive an identification register, and a standard external drive's ID is $FFFF FFFF, the constant NanoMig returns. Hardware: not measured |
| **Effort** | S — `paula_floppy.v` only — the `_ready` statement at L688-L694 gains the `motor_on` / `disk_present` term the sync interrupt already uses; a spin-up delay is not part of it |
| **See also** | [NM-HRM-046](NM-HRM-046.md) — the same module's drive light, which does not read the motor latch either |

---

## Source requirement

**Appendix E, printed p. 334**, the external disk connector's pin table, pin 1 RDY*, direction I/O:

> If motor on, indicates disk installed and up to speed. If motor not on,
> identification mode. See below.

**Appendix H, printed p. 368**, *Signals When Driving a Disk*, under RDY-:

> When a disk drive's motor is on, this line indicates the selected disk is
> installed and rotating at speed. The driver ignores this signal. When the
> motor is off this is used as a ID data line. See below.

**Chapter 8, printed p. 244**, Table 8-5, the description that also covers the internal drive:

> Disk ready (active low). The drive will pull this line low when the motor is
> known to be rotating at full speed. This signal is only valid when the motor
> is ON, at other times configuration information may obscure the meaning of
> this input.

> After turning on the motor, software must further wait for one half second
> (500ms), or for the DSKRDY* line to go low.

The motor state is a latch in the drive, not the level of the motor line. **p. 334**, pin 8 MTRXD*:

> Motor on data, clocked into drive's motor-on flip-flop by the active
> transition of SELxB*.

**p. 369**, under MTRXD-:

> All external drives must have logic equivalent to a D flip-flop, whose D
> input is the MTRXD- signal, and whose clock input is activated by the
> off-to-on (high-to-low) transition of its SELxB- signal.

**p. 244**, under DSKMOTOR*:

> Each drive will latch the motor signal at the time its select signal turns
> on. The disk drive motor will stay in this state until the next time select
> turns on.

With the motor off, **Appendix E, printed p. 335**:

> An identification mode is provided for reading a 32-bit serial
> identification data stream from an external device. To initialize this
> mode, the motor must be turned on, then off.

> The transition from motor on to motor off reinitializes the serial shift
> register.

> $0000 0000 - no drive present.

> $FFFF FFFF - Amiga standard 3.25 diskette.

> The serial input data is active low and must therefore be inverted to be
> consistent with the above table.

**Appendix H, printed p. 371**:

> Steps 1 through 4 in the algorithm above turn on and off the disk drive
> motor. This initializes the serial shift register.

A standard drive's stream is therefore RDY* active on all 32 reads.

## RTL and measurement

**NanoMig asserts RDY* for any selected drive within the configured count, and reads neither the motor latch nor the disk.**
[`src/minimig-aga/paula_floppy.v#L688-L694`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L688-L694):

```verilog
// drive _ready signal control
// Amiga DD drive activates _ready whenever _sel is active and motor is off
// or whenever _sel is active, motor is on and there is a disk inserted (not implemented - _ready is active when _sel is active)
assign _ready   = (_sel[3] | ~(drives[1] & drives[0])) 
        & (_sel[2] | ~drives[1]) 
        & (_sel[1] | ~(drives[1] | drives[0])) 
        & (_sel[0]);
```

The comment names the motor-on branch the pages print and says it is not implemented. `drives` is the configured drive count, latched at reset ([`#L554-L558`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L554-L558)). Both terms the pages name exist in the module: each drive's motor latch is clocked at its own select edge as p. 369 prescribes ([`#L625-L632`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L625-L632)), and each drive's disk presence is set from its mounted image ([`#L959-L966`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L959-L966)). The sync interrupt already ANDs the two for a selected drive ([`#L917`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L917)); `_ready` reads neither. The motor-off half is deliberate: the module's changelog ([`#L54`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/paula_floppy.v#L54)) says `_READY signal should respond to _SELx even when the motor is off`.

### Measured

All five reds deselect the drive for one clock, drive the motor line low and select it, so the next clock is a select edge and the latch takes the motor on; they read the latch back through `index` with no clock edge (`test_appH-drive-sense-and-motor.cpp:275-286`, `test_appDE-disk-interface.cpp:234-245`, `:247-263`), and it reads on. `RDY-DISK-PRESENCE`, drive 0, then removes and mounts the disk:

| drive 0, motor latched on | RDY* |
|---|---|
| no disk | **asserted** |
| disk mounted | asserted |

The label requires the two readings to differ; they do not. `RDY-DISK-PRESENCE-DRIVE1` does the same on drive 1 with drive 1 configured: latch on, RDY* asserted with and without a disk. `sol:E-disk.c26` and `opus:E-extdisk-iface.c4` grade p. 334's pin 1 separately on the same measurement with no disk mounted, requiring RDY* deasserted; the latch reads on and RDY* asserted. `P-READY-MOTOR-INVARIANT-DRIVE1` makes that measurement on drive 1 and is red too.

Pages 334 and 368 describe the external connector, drives 1 to 3. Replayed with `hrm/appH-drive-sense-and-motor`'s rig on drives 1, 2 and 3, four drives configured, the motor latched on at a select edge as p. 369 prints it (a replay outside `make test`, which the package does not print; at most a reading), reading the latch through `index` on in the first two rows and off in the third:

| each of drives 1, 2, 3 | RDY* |
|---|---|
| motor latched on, no disk | **asserted** |
| motor latched on, disk mounted | asserted |
| motor latched off, no disk | asserted |

## Risks and unknowns

- **Real hardware is not measured.** Whether a real external drive with its motor on and no disk holds RDY* inactive is settled by selecting drive 1 in that state and reading CIAAPRA bit 5 on a real machine. Pages 334 and 368 say so and all four implementations model it. No named software depends on it: p. 368 says the driver ignores this signal, so the difference reaches only software that reads DSKRDY* itself, as p. 244 allows.
- **p. 244 names only speed.** Table 8-5, quoted above, says nothing about a disk and does not say that an empty drive asserts the line; nothing else in the manual speaks to RDY* with the motor on.
- **Drive 0 against drives 1 to 3.** The two pages that name the disk describe the external connector, while three of the five reds are on drive 0, which only p. 244 describes; the two reds on drive 1 and the replay on drives 1 to 3 carry it to the pages' own drives.
- **Spin-up time.** NanoMig asserts the line from the clock the motor latches on. The pages imply a delay ("up to speed", "rotating at speed", "wait for one half second") and print no figure. WinUAE counts `DSKREADY_UP_TIME * 312 + (uaerand() & 511)` lines ([`disk.cpp:1978`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L1978), `#define DSKREADY_UP_TIME 18` at [`:120`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L120)), vAmiga `case DriveMechanics::A1010:   return MSEC(380);` ([`FloppyDrive.cpp:615`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L615)) and none with its `NONE` mechanics, Denise asserts at 2 % of `#define LIBAMI_MOTOR_ACCELERATION_CYCLES Agnus::msecToDMACycles(360)` ([`diskDrive.cpp:9`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L9)), and Copperline after a quarter second.
- **The identification stream is not graded.** No package grades it. p. 335 defines the value a drive of a given type answers, not the mechanism, and says the motor cycle reinitialises the shift register, not that it changes what a drive answers. For a drive whose ID is 32 identical bits, a live channel and a line tied to that value read the same, as do a reinitialised register and one that was not: the page cannot tell NanoMig's constant apart from a live channel. In a replay outside `make test`, for one to four configured drives, each configured drive answers $FFFF FFFF and each drive beyond the configured count $0000 0000, "no drive present", with and without the motor cycle. Copperline alone enters its identification mode only on a motor-on-to-off transition and reads RDY* inactive before one; WinUAE, vAmiga and Denise shift the ID at every select edge while the motor is off. The pages do not say what an uninitialised drive reads.
- **The internal drive in the identification mode.** NanoMig answers $FFFF FFFF on drive 0 as on the external drives. vAmiga and Denise give drive 0 no ID, Copperline makes its line follow track 0, and WinUAE reports an ID there only with its `cs_df0idhw` option. The pages describe the identification mode for external devices only.
- **Readings.** Appendix H's table of defined IDs (pp. 370–371), whose first two rows are all zeros, "Reserved", and all ones, "Amiga standard 3.25", was read from the page image, not quoted. The *A500/A2000 Technical Reference Manual* quotation below is from OCR, which reads the dollar sign as S.
- **The suggested change has not been run**, and "up to speed" would still not be modelled.

## Independent evidence

| | code | with the motor on and no disk; with the motor off |
|---|---|---|
| **WinUAE** | [`disk.cpp:3715-3739`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3715-L3739), [`:4716-4720`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4716-L4720), [`:170-173`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L170-L173), [`:3618-3628`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3618-L3628) | with the motor running the line follows the drive's ready flag, `drive_diskready(drv)`, which the spin-up count sets, and that count runs only while a disk is in, `if (drv->dskready_up_time > 0 && !drive_empty (drv)) {` — **an empty drive never asserts RDY* with the motor on**. With the motor off an external drive answers its ID bit, `if (drv->idbit && currprefs.floppyslots[dr].dfxtype != DRV_35_DD_ESCOM)`, shifted out of `#define DRIVE_ID_35DD  0xFFFFFFFF` at each select edge, where the motor latch is clocked too, `/* motor/id flipflop is set only when drive select goes from high to low */` |
| **vAmiga** | [`Core/Peripherals/Drive/FloppyDrive.cpp:692-699`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L692-L699), [`:597-601`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L597-L601), [`:565-595`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L565-L595), [`:1426-1438`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L1426-L1438) | `idMode()` is `return motorStopped() \|\| motorSpeedingUp();`, so with the motor on the line carries the ID bit until the motor is at full speed, and after that **only a drive with a disk asserts it**, `else if (hasDisk())`. An external 3.5" DD drive's ID is `return 0xFFFFFFFF;`, the internal drive's `return 0x00000000;`. The motor changes only at a select edge, `// The motor state can only change on a falling edge on the select line` |
| **Denise** | [`emulation/libami/drive/diskDrive.cpp:397-409`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L397-L409), [`:463-470`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L463-L470), [`:431-433`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L431-L433) | with the motor on **only an inserted disk asserts the line**, `if (inserted && (speed >= 2))`; with it off the line is the ID bit, or an inserted disk still above 98 % of its speed, `else if (inserted && (speed > 98))`, and `getId()` gives a DD drive other than drive 0 `return 0xffffffff;` and drive 0 `return 0;`. The motor latches at a select edge, `if (selectedLineOld && !selectedLine) {` / `setMotor( !(value & oldValue & 0x80) );` |
| **Copperline** | [`src/floppy/mod.rs:3036-3045`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3036-L3045), [`:61`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L61), [`:182`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L182), [`:513-525`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L513-L525), [`:1195-1201`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1195-L1201), [`:3065-3077`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3065-L3077) | `ready()` is `self.has_media() && self.motor_on && self.motor_cck >= MOTOR_READY_CCK`, media, the motor, and a quarter second of spin-up, `const MOTOR_READY_CCK: u32 = PAULA_CLOCK_HZ / 4;`; with the motor on `rdy_line_asserted()` returns `self.ready()`, so **an empty drive never asserts the line**. With the motor off an external drive in its identification mode answers its ID bit, `const STANDARD_EXTERNAL_DRIVE_ID: u32 = 0xFFFF_FFFF;`, and drive 0, which has no ID circuit, makes the line follow track 0, `let rdy_asserted = if idx == 0 && !drive.motor_on {`. The motor latches at a select edge, `let motor_on = (prev & CIAB_DSKMOTOR == 0) \|\| (val & CIAB_DSKMOTOR == 0);`, and the identification mode begins only when the motor goes from on to off, `self.external_id_mode = true;` |
| **MiSTer** | [`rtl/paula_floppy.v:386-392`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L386-L392) | descendant — the same statement, comment included: `// or whenever _sel is active, motor is on and there is a disk inserted (not implemented - _ready is active when _sel is active)` and `assign _ready   = (_sel[3] \| ~(drives[1] & drives[0]))` |
| **TC64** | [`rtl/minimig/paula_floppy.v:622-628`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L622-L628) | descendant — the same statement and the same comment, `assign _ready   = (_sel[3] \| ~(drives[1] & drives[0]))`. Neither line has changed it; the finding is inherited unchanged |

### What other Commodore documents say

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 9 — agrees with the manual:

  > "If motor on, indicates disk installed and up to speed."
  <!-- corpus: a500-a2000-trm-1987/p009.txt -->

  The external disk connector's pin table gives pin 1 RDY* the note p. 334 prints.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 11 — agrees with the manual:

  > "SFFFF FFFF — Amiga standard 3.25 diskette"
  <!-- corpus: a500-a2000-trm-1987/p011.txt -->

  The defined identifications are p. 335's, $0000 0000 for no drive present and $FFFF FFFF for the standard drive, with the same instruction to invert the active-low serial data.

## Decision

**One option: assert RDY* for a selected, configured drive only while its motor latch is off or its disk is present** — the conjunction the sync interrupt already forms on line 917:

```verilog
wire [3:0] drive_cfg = {drives[1] & drives[0], drives[1], drives[1] | drives[0], 1'b1};
assign _ready = ~|(~_sel & drive_cfg & (~motor_on | disk_present));
```

With `(~motor_on | disk_present)` all ones this is today's statement, so the motor-off answer, the identification stream, is unchanged. What it would change:

- `RDY-DISK-PRESENCE` would read deasserted with no disk and asserted with one;
- `sol:E-disk.c26` and `opus:E-extdisk-iface.c4` would read deasserted;
- a drive with its motor on and no disk would read not ready, as all four implementations make it.
