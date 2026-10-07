# NM-HRM-046 — the drive activity light follows DMA, not the motor [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `fdd_led` light while a drive's latched motor bit (DSKMOTOR*, PB7) is on, as the last sentence of the PB7 row on p.244 prints, instead of only while disk DMA runs (`dskstate!=DISKDMA_IDLE`)? The divergence is visible and physical, not register-level. Closes when `fdd_led` follows the latched motor (`opus:8-cia.c26` and `sol:8-cia.c24` turn green), or when an Amiga specialist or the NanoMig board maintainers reject the change; if the rejection means p.244 is wrong, the page is refiled against the printed page. |
| **Report to** | `NanoMig direct` — harbaum/NanoMig and MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | Paula's disk-activity output `fdd_led` in `paula_floppy.v`, with disk DMA idle and drive 0 selected with its motor latched off, then on; chip revision unknown; hardware not measured; the per-drive lamp is not graded |
| **Software impact** | unknown; no named affected software. The page states that no register or timing changes and that no Amiga software can observe `fdd_led`; the effect is the lamp |
| **Chapter** | 8 — Interface Hardware, printed p.244, Table 8-5, the PB7 row · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `A5` `opus:8-cia.c26` and `A6` `sol:8-cia.c24` (the activity-light sentence, both red); `A1` `opus:8-cia.c24`, `A2` `sol:8-cia.c22`, `A3` `opus:8-cia.c25`, `A4` `sol:8-cia.c23`, `A7` `opus:8-cia.c28`, `A8` `sol:8-cia.c26` (the rest of the row, green); `A0` and `CTRL-DMA-DRIVES-FDD-LED` carry no claim id |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L971-L972) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L543-L545); fixed in TC64 (the motor-following line is live there; MiSTer has NanoMig's line) |
| **Test** | [`hrm/ch8-drive-select-motor-led`](../../hrm/ch8-drive-select-motor-led): `make -C hrm/ch8-drive-select-motor-led test`; [`integration/appF-cia-system`](../../integration/appF-cia-system): `make -k -C integration/appF-cia-system test`; an expected red exits nonzero. The integration package reproduces the light at the system's ports on fx68k and TG68K, recorded, not graded |
| **Assertions** | 2 red of 10 graded: `A5/opus:8-cia.c26` — expected `fdd_led`=1 with the motor on and DMA idle, observed 0 (0 with the motor off too); its twin `A6/sol:8-cia.c24`, the same. The package reports 8 passed, 2 failed. `integration/appF-cia-system` adds none: `P1-LP/P1-O2` and `LED-R` are RECORDED lines, and nothing in that package is graded on the light |
| **Verdict of the four** | **unanimous against NanoMig.** All four model a drive light, and all four drive it from the motor. Two add a select term; two use DMA only to change the light's *colour*, never whether it is lit. Hardware: not measured |
| **Effort** | S — for the aggregate lamp: the one `assign` at `paula_floppy.v:972`. The page does not size a per-drive lamp, which would change the single scalar `fdd_led` port, nor the board's use of the LED |

---

## Source requirement

**Chapter 8, printed p.244**, Table 8-5, the PB7 row:

> ```
> PB7   DSKMOTOR*    Disk motor control (active low).  This signal is
>                    nonstandard on the Amiga system.  Each drive will latch
>                    the motor signal at the time its select signal turns
>                    on.  The disk drive motor will stay in this state until
>                    the next time select turns on.  DSKMOTOR* also controls
>                    the activity light on the front of the disk drive.
> ```

The last sentence is the claim: it names a physical object, the lamp on the front
of the drive, and the paragraph makes no mention of DMA, of transfers or of
DSKLEN.

## RTL and measurement

[`paula_floppy.v#L971-L972`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L971-L972) has no motor term:

```verilog
//disk activity LED
assign fdd_led = (dskstate!=DISKDMA_IDLE);
```

`dskstate` is the disk DMA state machine
([`#L974-L992`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L974-L992)),
and the port's own declaration says "active when DMA is on"
([`#L100`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L100)).
`motor_on[3:0]` is latched per drive on that drive's own select edge, as p.244
requires
([`#L624-L659`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L624-L659)),
and is read by `index`
([`#L576`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L576))
and `syncint`
([`#L917`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L917))
and by nothing else. `fdd_led` is also a single scalar for the whole controller,
where p.244 describes one lamp per drive.

    A5/opus:8-cia.c26: fdd_led with motor OFF (DMA idle)=0,
                       fdd_led with motor ON  (DMA idle)=0

> **FAIL: opus:8-cia.c26** (p.244, quote *"DSKMOTOR* also controls the activity
> light on the front of the disk drive."*): with disk DMA idle, `fdd_led`
> follows the selected drive's motor into the ON state — `paula_floppy.v:972`
> drives `fdd_led` purely from `dskstate!=DISKDMA_IDLE`.

> **FAIL: sol:8-cia.c24** — twin of the above, same printed quote, independently
> read, graded separately under its own uid: `fdd_led` off-state=0,
> on-state=0.

> **PASS: CTRL-DMA-DRIVES-FDD-LED** (control case, no manual citation, same
> `fdd_led` readout as A5/A6): with **no drive selected at all**, arming disk DMA
> is what actually moves `fdd_led` from 0 to 1 — proving the port is live and
> naming DMA, not the motor, as its real cause.

The light also comes on with no drive selected, a state in which no real drive's
lamp would be on.

Six greens in the same run (`A1`–`A4`, `A7`, `A8`) grade the rest of the PB7
row, including the latch on the select edge and a drive remembering its motor
state while another is operated.

**At the system's ports, on both CPU cores.**
`integration/appF-cia-system` runs 68000 programs on the whole of `nanomig.v`
with DF0 configured and reads `fdd_led` at the top-level port on every clock
cycle of its CIA-B timer-B runs. Its P1 program writes PRB so that MTR* (PB7) is
low when SEL0* (PB3) falls, then with MTR* high, and reads `fdd_led` at the end
of each wait. The package prints, the same on both cores:

    RECORDED: P1-LP/P1-O2 [p244]: not graded -- measured: LED when the CPU leaves reset=off, after R-SELECT with MTR*=0 (on leg)=off, after R-SELECT with MTR*=1 (off leg)=off -- the same level at all three points, so no input of this package is shown to move the light at this port; O2's own legs (LED-A1, LED-C, LED-D1, LED-D2) are not graded

    RECORDED: LED-R [p244]: not graded -- O2 is not graded at this port; see P1-LP/P1-O2

`EP2-PER-RUN` is green: between each run's first marker and its last, the
chip-memory port shows only the CPU and memory refresh, so no disk DMA runs while
the light is read. The package prints no FAIL for the light; its 26 FAILs on each core
belong to other findings, eleven of them to [NM-HRM-074](NM-HRM-074.md).

What a user would see:

- **The light stays dark through a seek**, from the moment the motor is latched on
  until the first DMA transfer begins.
- **The light goes dark between transfers.** NanoMig's light goes off whenever
  the state machine returns to `DISKDMA_IDLE`, whether or not the motor is still
  running.
- **Motor-only operations are invisible**, such as spinning a drive up to read
  DSKRDY* or to check whether a disk is present.

## Risks and unknowns

- **Nothing a program can read is affected.** No register changes value, no
  timing changes, and no Amiga software can observe `fdd_led`. A conformance
  register whose other entries are about bus-visible behaviour contains nothing
  else quite like this.
- **NanoMig has no drive mechanism.** The floppy is served from an SD-card
  image, so "the front of the disk drive" has no referent in this hardware. The
  claim survives because the core exports a signal named for that lamp and the
  board wires it to one, but an implementer could reasonably say the page
  describes a mechanism this design does not have.
- **The LED may have another job on the board.** `fdd_led` leaves the core and
  lights a physical LED on the FPGA board, where its job may be to tell the user
  the SD-card image is being accessed, for which DMA activity is the more useful
  signal, and a light that stays on for the seconds a motor idles would be worse.
  Changing it trades Amiga fidelity against board usability.
- **The select term the page omits** is implemented by two of the four (see
  Independent evidence), which suggests the printed sentence is not a complete
  description of the real lamp's behaviour either.
- **The system-level record shows a dark light, not a comparison.** The package
  starts no disk DMA, the one input known to light the LED, and prints no value
  for the four O2 legs nor for P2's leg R. At leg R, PB7 is an input and reads
  high on this build (P2's r1 = $FF on CIA-B timer B), so DF0's motor latch is off
  at that select whatever the light follows.
- **The per-drive half is open in all three Minimig trees.** TC64's lamp is still
  a single aggregate for the whole controller.
- **Real hardware is not measured**, and the chip revision is unknown.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:917-930`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L917-L930) | `gui_led (num + LED_DF0, (gid->drive_motor ? 1 : 0) \| (gid->drive_writing ? 2 : 0), -1)`: bit 0, lit or not, is the motor; bit 1 is a write indicator that colours the lamp and cannot light it. A select term only for external drives, [`disk.cpp:3659-3661`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3659-L3661): *"external drives usually (always?) light activity led when selected. Internal only when motor is running."* |
| **vAmiga** | [`FloppyDrive.cpp:743-759`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L743-L759) | `Msg::DRIVE_LED` is emitted from exactly one place, `FloppyDrive::setMotor(bool value)`, with `value` as the payload; `setMotor` is reached only from the select-edge latch ([`:1427-1438`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L1427-L1438)). Per drive, no select term |
| **Denise** | [`emulation/libami/drive/diskDrive.cpp:645-659`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L645-L659) | its own comment, `// drive LED is hardwired to motor state`; per drive, no select term. A `0x80` write flag changes what the indicator says, never whether it is lit |
| **Copperline** | [`src/floppy/mod.rs:558-561`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L558-L561) | `activity_led_on` is the motor of the selected drive and nothing else, so a spinning but deselected drive shows dark; it reaches the status bar through [`bus.rs:5318`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L5318) and [`statusbar.rs:475-482`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/window/statusbar.rs#L475-L482) with no DMA term on the path |
| **MiSTer** | [`rtl/paula_floppy.v:544`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L544) | descendant — byte-identical to NanoMig's `fdd_led` line, with the motor-driven alternative commented out below at [`:545`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L545), `//assign disk_led = \|motor_on;` |
| **TC64** | [`rtl/minimig/paula_floppy.v:796`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L796) | descendant — does not share this defect: the live statement is `assign disk_led = \|motor_on;`, with the DMA version commented out at [`:795`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L795). The port still carries the stale comment "disk activity LED, active when DMA is on" |

The four agree on the motor term the page names and differ only in the select
term, which p.244 does not mention: WinUAE (external drives only) and Copperline
add it; vAmiga and Denise use the latched motor alone. MiSTer is in NanoMig's
position, with the motor term commented out; TC64 has it live and no select term.

## Decision

**One line, and it is already written upstream.** Minimig-AGA's `paula_floppy.v`
carries the alternative on the line after the live one
([`:545`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L545)),
`//assign disk_led = |motor_on;`, which NanoMig did not carry forward; TC64 ships
it live, so one maintained descendant has been running the motor-following lamp.
`|motor_on` is the page's reading for a single aggregate lamp; `|(~_sel &
motor_on)` would add Copperline's select term.

Nothing brings it down by itself: there is no automatic merge of fixes from either
maintained line into NanoMig, so a correction arrives only if somebody ports it
deliberately.

**The honest position:** the page is unambiguous, the RTL contradicts it, and the
right answer depends on what the LED on that board is *for*.
