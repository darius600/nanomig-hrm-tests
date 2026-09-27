# NM-HRM-080 — with the motor on, RDY* is asserted for a drive with no disk in it, where pp. 334 and 368 make it mean a disk installed and up to speed [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. Four more reds on the same line are explained under *The other four reds on `_ready`*; none of them is part of this finding. |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L688-L694) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L386-L392) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer**, if the manual's reading is accepted. NanoMig has the statement and its comment from there and TC64 carries the same, so there is no fix to port; every independent implementation read here already gates the line on a disk — see *What the others say* |
| **Test** | [`hrm/appH-drive-sense-and-motor`](../../hrm/appH-drive-sense-and-motor) · `RDY-DISK-PRESENCE`, claims `opus:AHJ-060`, `sol:H-036` (p. 368), red. On the same statement and not this finding: [`hrm/appDE-disk-interface`](../../hrm/appDE-disk-interface) · `sol:E-disk.c26`, `opus:E-extdisk-iface.c4`, and [`hrm/appH-device-id-protocol`](../../hrm/appH-device-id-protocol) · `ID-CHANNEL-EXISTS`, `MOTOR-CYCLE-INIT-CHANGES-ID-STREAM` |
| **Chapter** | Appendix E, printed pp. 334–335; Appendix H, printed pp. 368–371; 8 — Interface Hardware, printed p. 244 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red of 7 graded (`RDY-DISK-PRESENCE`; the package reads `7 passed, 1 failed.` with its fixture-liveness check). In the same run `CHANGE-NO-CLEAR-NO-DISK`, green, reads the fixture's disk presence through `_change`, and `INDEX-SEL-MOTOR-GATE`, green, reads its motor latch through `index` |
| **Verdict of the four** | **unanimous against NanoMig.** Once the motor is up to speed, WinUAE, vAmiga, Denise and Copperline assert RDY* only for a drive with a disk in it; WinUAE, Denise and Copperline never assert it for an empty drive with the motor on, and vAmiga only while its identification mode lasts through spin-up. With the motor off all four give an external drive an identification register, and a standard external drive's ID is $FFFF FFFF, the constant NanoMig returns |
| **See also** | [NM-HRM-046](NM-HRM-046.md) — the same module's drive light, which does not read the motor latch either |

---

## What the HRM says

**Appendix E, printed p. 334**, the external disk connector's pin table, pin 1
RDY*, direction I/O:

> If motor on, indicates disk installed and up to speed. If motor not on,
> identification mode. See below.

**Appendix H, printed p. 368**, *Signals When Driving a Disk*, under RDY-:

> When a disk drive's motor is on, this line indicates the selected disk is
> installed and rotating at speed. The driver ignores this signal. When the
> motor is off this is used as a ID data line. See below.

Two pages of the manual, in two appendices, give the same condition with the
motor on: a disk **installed** and turning at speed.

**Chapter 8, printed p. 244**, Table 8-5, is the description that also covers
the internal drive:

> Disk ready (active low). The drive will pull this line low when the motor is
> known to be rotating at full speed. This signal is only valid when the motor
> is ON, at other times configuration information may obscure the meaning of
> this input.

> After turning on the motor, software must further wait for one half second
> (500ms), or for the DSKRDY* line to go low.

It names the motor's speed and says nothing about a disk; it does not say that
an empty drive asserts the line. Nothing else in the manual speaks to RDY* with
the motor on.

### What "the motor is on" means

The motor state is a latch in the drive, not the level of the motor line.
**p. 334**, pin 8 MTRXD*:

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

### With the motor off: the identification mode

**Appendix E, printed p. 335**:

> An identification mode is provided for reading a 32-bit serial
> identification data stream from an external device. To initialize this
> mode, the motor must be turned on, then off.

> The transition from motor on to motor off reinitializes the serial shift
> register.

and the IDs it defines:

> $0000 0000 - no drive present.

> $FFFF FFFF - Amiga standard 3.25 diskette.

> The serial input data is active low and must therefore be inverted to be
> consistent with the above table.

**Appendix H, printed pp. 370–371**, gives the same procedure in nine numbered
steps and a table of defined IDs whose first two rows, *read from the page
image, not quoted*, are all zeros, "Reserved", and all ones, "Amiga standard
3.25". p. 371 on the motor cycle:

> Steps 1 through 4 in the algorithm above turn on and off the disk drive
> motor. This initializes the serial shift register.

So a standard drive's identification stream is RDY* active on all 32 reads.

---

## What NanoMig does

**It asserts RDY* for any selected drive within the configured count, and
reads neither the motor latch nor the disk.**
[`src/minimig-aga/paula_floppy.v#L688-L694`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L688-L694):

```verilog
// drive _ready signal control
// Amiga DD drive activates _ready whenever _sel is active and motor is off
// or whenever _sel is active, motor is on and there is a disk inserted (not implemented - _ready is active when _sel is active)
assign _ready   = (_sel[3] | ~(drives[1] & drives[0])) 
        & (_sel[2] | ~drives[1]) 
        & (_sel[1] | ~(drives[1] | drives[0])) 
        & (_sel[0]);
```

`drives` is the configured drive count, latched at reset,
[`#L554-L558`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L554-L558),
`drives <= floppy_drives;`. The comment names the motor-on branch the pages
print and says it is not implemented.

**Both terms the pages name exist in the module.** Each drive's motor latch is
clocked at its own select edge, as p. 369 prescribes,
[`#L625-L632`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L625-L632),
`else if (!_sel[0] && _sel_del[0])` / `motor_on[0] <= ~_motor;`, and each
drive's disk presence is set from its mounted image,
[`#L959-L966`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L959-L966),
`disk_present[i] <= |sdc_image_size;`. `motor_on` feeds the index pulse and the
sync interrupt, `disk_present` the change latch and the sync interrupt; the
sync interrupt already ANDs the two for a selected drive,
[`#L917`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L917),
`assign syncint = sync_match | ~dmaen & |(~_sel & motor_on & disk_present) & sof;`.
`_ready` reads neither.

**The motor-off half is deliberate.** The module's changelog,
[`#L54`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L54),
`_READY signal should respond to _SELx even when the motor is off`. With the
motor off, RDY* active on every select is the identification stream of the
standard drive p. 335 defines.

### Measured

[`hrm/appH-drive-sense-and-motor`](../../hrm/appH-drive-sense-and-motor),
`RDY-DISK-PRESENCE`: drive 0 selected, the motor line low before the first
clock after selection, so the latch takes it; then the disk is removed and
mounted:

| drive 0, motor latched on | RDY* |
|---|---|
| no disk | **asserted** |
| disk mounted | asserted |

The label requires the two readings to differ and they do not: red. With the
package's mutation, `MUTATE=RDY-DISK-PRESENCE`, the same comparator is fed a
deselection instead and reads green, `8 passed, 0 failed.`

**The same, on the drives the pages describe.** Pages 334 and 368 describe the
external connector, drives 1 to 3, and the package measures drive 0. Replayed
with the package's rig on drives 1, 2 and 3, four drives configured, the motor
latched on at a select edge exactly as p. 369 prints it — a measurement for
this page, not a graded assertion:

| each of drives 1, 2, 3 | RDY* |
|---|---|
| motor latched on, no disk | **asserted** |
| motor latched on, disk mounted | asserted |
| motor latched off, no disk | asserted |

The same replay reads the latch through `index` and confirms it on in the
first two rows and off in the third.

---

## What the others say

The question put to each: with the motor latched on and no disk in the drive,
is RDY* asserted? And what does the line carry with the motor off?

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:3715-3739`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3715-L3739), [`:4716-4720`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4716-L4720), [`:170-173`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L170-L173), [`:3618-3628`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3618-L3628) | <pre>if (drive_running (drv)) {<br>...<br>&nbsp;&nbsp;&nbsp;&nbsp;if (drive_diskready(drv) &amp;&amp; !drv-&gt;indexhack &amp;&amp; currprefs.floppyslots[dr].dfxtype != DRV_35_DD_ESCOM)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;st &amp;= ~0x20;</pre> with the motor running the line follows the drive's ready flag, which the spin-up count sets, and that count runs only while a disk is in, `if (drv->dskready_up_time > 0 && !drive_empty (drv)) {` — **an empty drive never asserts RDY* with the motor on**. With the motor off an external drive answers its ID bit, `if (drv->idbit && currprefs.floppyslots[dr].dfxtype != DRV_35_DD_ESCOM)`, shifted out of `#define DRIVE_ID_35DD  0xFFFFFFFF` at each select edge, where the motor latch is clocked too, `/* motor/id flipflop is set only when drive select goes from high to low */` |
| **vAmiga** | [`Core/Peripherals/Drive/FloppyDrive.cpp:692-699`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L692-L699), [`:597-601`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L597-L601), [`:565-595`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L565-L595), [`:1426-1438`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L1426-L1438) | <pre>if (idMode()) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (idBit) result &amp;= 0b11011111;<br>} else if (hasDisk()) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (motorAtFullSpeed() \|\| motorSlowingDown()) result &amp;= 0b11011111;<br>}</pre> `idMode()` is `return motorStopped() \|\| motorSpeedingUp();`, so with the motor on the line carries the ID bit until the motor is at full speed, and after that **only a drive with a disk asserts it**. An external 3.5" DD drive's ID is `return 0xFFFFFFFF;`, the internal drive's `return 0x00000000;`. The motor changes only at a select edge, `// The motor state can only change on a falling edge on the select line` |
| **Denise** | [`emulation/libami/drive/diskDrive.cpp:397-409`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L397-L409), [`:463-470`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L463-L470), [`:431-433`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L431-L433) | <pre>if (motor) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (inserted &amp;&amp; (speed &gt;= 2))<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;out &amp;= ~0x20;<br>} else {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (getId() &amp; (1 &lt;&lt; (31 - idPos)))</pre> with the motor on **only an inserted disk asserts the line**; with it off the line is the ID bit, and `getId()` gives a DD drive other than drive 0 `return 0xffffffff;` and drive 0 `return 0;`. The motor latches at a select edge, `if (selectedLineOld && !selectedLine) {` / `setMotor( !(value & oldValue & 0x80) );` |
| **Copperline** | [`src/floppy/mod.rs:3036-3045`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3036-L3045), [`:61`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L61), [`:182`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L182), [`:513-525`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L513-L525), [`:1195-1201`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1195-L1201), [`:3065-3077`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3065-L3077) | <pre>fn ready(&amp;self) -&gt; bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;self.has_media() &amp;&amp; self.motor_on &amp;&amp; self.motor_cck &gt;= MOTOR_READY_CCK<br>}</pre> media, the motor, and a quarter second of spin-up, `const MOTOR_READY_CCK: u32 = PAULA_CLOCK_HZ / 4;`; with the motor on `rdy_line_asserted()` returns `self.ready()`, so **an empty drive never asserts the line**. With the motor off an external drive in its identification mode answers its ID bit, `const STANDARD_EXTERNAL_DRIVE_ID: u32 = 0xFFFF_FFFF;`, and drive 0, which has no ID circuit, makes the line follow track 0, `let rdy_asserted = if idx == 0 && !drive.motor_on {`. The motor latches at a select edge, `let motor_on = (prev & CIAB_DSKMOTOR == 0) \|\| (val & CIAB_DSKMOTOR == 0);`, and the identification mode begins only when the motor goes from on to off, `self.external_id_mode = true;` |
| **MiSTer** | [`rtl/paula_floppy.v:386-392`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L386-L392) | descendant — the same statement, comment included: `// or whenever _sel is active, motor is on and there is a disk inserted (not implemented - _ready is active when _sel is active)` and `assign _ready   = (_sel[3] \| ~(drives[1] & drives[0]))` |
| **TC64** | [`rtl/minimig/paula_floppy.v:622-628`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L622-L628) | descendant — the same statement and the same comment, `assign _ready   = (_sel[3] \| ~(drives[1] & drives[0]))`. Neither line has changed it; the finding is inherited unchanged |

**All four gate RDY* on a disk once the motor is on.** Each keeps a disk-present
state and a motor state per drive, and each asserts the line for a drive at
speed only when a disk is in it; each also makes it wait for a spin-up of its
own, vAmiga only with its A1010 mechanics. With the motor off all four give an
external drive an identification register, and a standard external drive the
constant $FFFF FFFF, which is what NanoMig's selected, configured drive gives.

---

## The other four reds on `_ready`

Three packages grade `_ready` against these pages, and five of their
assertions are red. One of them measures this finding. The other four are red
on the same statement, and none of them is evidence of a defect as it stands.

### `sol:E-disk.c26` and `opus:E-extdisk-iface.c4`: the motor is never turned on

[`hrm/appDE-disk-interface`](../../hrm/appDE-disk-interface) reads
`9 passed, 4 failed.` Each of these two blocks selects drive 0, reads RDY*,
drives the motor line low while the drive stays selected, and reads again,
with no clock edge between. The label requires the two readings to differ.
Pages 334, 369 and 244 latch the motor at the select transition only, so the
drive's motor is off at both readings, both are in the identification mode,
and the page predicts the same reading twice. NanoMig does what the page
predicts here: read through `index` in a replay of the block, drive 0's motor
latch is off at both readings. WinUAE, vAmiga, Denise and Copperline also change
a drive's motor only at its select edge (the lines above), so all four would
read the same twice and fail the assertion as it is built.

With the motor latched at a select edge and no disk, the page does predict two
different readings, and NanoMig would still read the same twice; that is the
measurement `RDY-DISK-PRESENCE` makes. The same package's other two reds,
`sol:E-disk.c35` and `opus:E-extdisk-iface.c13`, are on DRESB* and the
write-protect line, a different statement of the module, and are not this
finding.

### `ID-CHANNEL-EXISTS` and `MOTOR-CYCLE-INIT-CHANGES-ID-STREAM`: the page defines the constant NanoMig returns

[`hrm/appH-device-id-protocol`](../../hrm/appH-device-id-protocol) reads
`1 passed, 2 failed`. The first assertion requires the 32 reads of the
procedure not to be all alike; the second requires them to change when steps 1
to 4 are left out. NanoMig reads RDY* active on all 32, with and without the
motor cycle. Inverted as p. 335 says, that is $FFFF FFFF, the ID pp. 335 and 371
give the Amiga standard drive. Replayed on every drive for one to four
configured drives, each configured drive answers $FFFF FFFF and each drive
beyond the configured count $0000 0000, "no drive present", with and without
steps 1 to 4.

So the page predicts the first measurement for the only drive NanoMig models,
and a constant ID cannot change with the motor cycle, so the page does not
require the second. The package's own statement that there is no shift
register is true of the module; for a drive whose ID is all ones it cannot be
seen at the pin. All four implementations identify a standard external DD
drive as $FFFF FFFF. They split on what an uninitialised drive reads: WinUAE,
vAmiga and Denise shift the ID at every select edge while the motor is off,
initialised or not, and Copperline enters its identification mode only on a
motor-on-to-off transition and reads RDY* inactive before one, so on an
external drive Copperline alone would pass the second assertion. The pages do
not say what an uninitialised drive reads; p. 244 says only that with the
motor off configuration information may obscure the line.

---

## Suggested change

**One option: assert RDY* for a selected, configured drive only while its
motor latch is off or its disk is present** — the conjunction the sync
interrupt already forms on line 917:

```verilog
wire [3:0] drive_cfg = {drives[1] & drives[0], drives[1], drives[1] | drives[0], 1'b1};
assign _ready = ~|(~_sel & drive_cfg & (~motor_on | disk_present));
```

With `(~motor_on | disk_present)` all ones this is today's statement, so the
motor-off answer, the identification stream, is unchanged. This has not been
run here. What it would change:

- `RDY-DISK-PRESENCE` would read deasserted with no disk and asserted with one;
- a drive with its motor on and no disk would read not ready, as all four
  implementations make it;
- "up to speed" would still not be modelled: the line would be asserted from
  the clock the motor latches on. The pages print no spin-up time, so nothing
  here grades one.

---

## What holds the finding up

- **The stimulus is the page's.** The package's motor line is low before the
  first clock after selection, and the replay confirms drive 0's latch on
  through `index`; on drives 1 to 3 the latch was taken at a select edge exactly
  as p. 369 prints it.
- **The fixture reaches both terms.** In the same run `CHANGE-NO-CLEAR-NO-DISK`,
  green, reads the mounted and removed disk through `_change`, and
  `INDEX-SEL-MOTOR-GATE`, green, reads the motor latch through `index`. The
  mutation feeds the unchanged comparator a deselection and reads green.
- **Two appendices say it, and a second document repeats it.** p. 334 and
  p. 368 name a disk installed; the *A500/A2000 Technical Reference Manual*
  prints p. 334's sentence. p. 244 names only the motor's speed and does not
  say the opposite.
- **The RTL says it itself.** The comment above the statement names the
  motor-on branch and calls it not implemented.
- **The four agree with the page**, each with a disk term of its own.
- **What weakens it.** p. 368 says the driver ignores this signal, so the
  difference reaches only software that reads DSKRDY* itself, as p. 244 allows.
  And the two pages that name the disk describe the external connector: the
  package's red is on drive 0, which only p. 244 describes; the replay on drives
  1 to 3 is what carries it to the pages' own drives.

---

## For the specialists

1. **Does a real external drive with its motor on and no disk hold RDY*
   inactive?** Pages 334 and 368 say so and all four implementations model it.
   Selecting drive 1 with the motor latched on and no disk, and reading CIAAPRA
   bit 5 on a real machine, would settle it.
2. **The spin-up time.** NanoMig asserts the line from the clock the motor
   latches on. The pages imply a delay, "up to speed", "rotating at speed",
   "wait for one half second", and print no figure. Each implementation has its
   own: WinUAE counts `DSKREADY_UP_TIME * 312` lines
   ([`disk.cpp:1978`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L1978),
   `#define DSKREADY_UP_TIME 18` at
   [`:120`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L120)),
   vAmiga `case DriveMechanics::A1010:   return MSEC(380);`
   ([`FloppyDrive.cpp:615`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L615))
   and none with its `NONE` mechanics, Denise asserts at 2 % of
   `#define LIBAMI_MOTOR_ACCELERATION_CYCLES Agnus::msecToDMACycles(360)`
   ([`diskDrive.cpp:9`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L9)),
   and Copperline after a quarter second.
3. **The internal drive in the identification mode.** NanoMig answers
   $FFFF FFFF on drive 0 as on the external drives. vAmiga and Denise give
   drive 0 no ID, Copperline makes its line follow track 0, and WinUAE reports
   an ID there only with its `cs_df0idhw` option. The pages describe the
   identification mode for external devices only.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 9 — agrees with the manual:

  > "If motor on, indicates disk installed and up to speed."
  <!-- corpus: a500-a2000-trm-1987/p009.txt -->

  The external disk connector's pin table gives pin 1 RDY* the note p. 334 prints: with the motor on the line indicates a disk installed and up to speed.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 11 — agrees with the manual:

  > "SFFFF FFFF — Amiga standard 3.25 diskette"
  <!-- corpus: a500-a2000-trm-1987/p011.txt -->

  The defined identifications are p. 335's, $0000 0000 for no drive present and $FFFF FFFF for the standard drive, with the same instruction to invert the active-low serial data; the OCR reads the dollar sign as S.

Silent on this point: techtravels, *Inside Paula's Floppy Controller*, part I, 2026, which names the ready line among the drive's status signals and does not describe it; 6502.org dissection of Paula 8364R7, 2023.
