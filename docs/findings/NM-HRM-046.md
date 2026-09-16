# NM-HRM-046 — the drive activity light follows DMA, not the motor

| | |
|---|---|
| **Status** | open · red — **a visible, physical divergence**, not a register-level one |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L971-L972) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L543-L545) |
| **Upstream** | **inherited** — the line is byte-identical to upstream. Upstream carries a commented-out alternative on the next line, `//assign disk_led = \|motor_on;`, which NanoMig dropped; the live line is the same in both |
| **Report to** | **the NanoMig board maintainers**, not the RTL alone — `fdd_led` leaves the core and lights a real LED on the FPGA board |
| **Test** | [`hrm/ch8-drive-select-motor-led`](../../hrm/ch8-drive-select-motor-led) — assertions `opus:8-cia.c26` and its twin `sol:8-cia.c24` |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 2 red of 10 graded, a claim and its independently-read twin. **Six greens in the same run** grade the rest of the same printed paragraph, including the motor latch this light fails to read |
| **Provenance** | **recovered by the attack stage** after classification had discarded both claims as not testable. See *How this finding was nearly lost* |
| **Verdict of the four** | **unanimous against NanoMig.** All four model a drive light, and all four drive it from the motor. Two add a select term; two use DMA only to change the light's *colour*, never whether it is lit |

---

## What the HRM says

**Chapter 8, printed p.244**, Table 8-5, the PB7 row:

> ```
> PB7   DSKMOTOR*    Disk motor control (active low).  This signal is
>                    nonstandard on the Amiga system.  Each drive will latch
>                    the motor signal at the time its select signal turns
>                    on.  The disk motor will stay in this state until
>                    the next time select turns on.  DSKMOTOR* also controls
>                    the activity light on the front of the disk drive.
> ```

The last sentence is the claim. It is short, it is unambiguous, and it names a
physical object: the lamp on the front of the drive. The paragraph makes no
mention of DMA, of transfers, or of DSKLEN.

**The rest of that paragraph is measured green in the same run.** The per-drive
latch on the select edge, the state holding until the next select edge, and a
drive remembering its motor state while another drive is operated — all six
assertions pass. NanoMig implements the DSKMOTOR paragraph correctly, up to its
last sentence.

## What NanoMig does

One line, with no motor term in it:

[`src/minimig-aga/paula_floppy.v#L971-L972`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L971-L972):

```verilog
//disk activity LED
assign fdd_led = (dskstate!=DISKDMA_IDLE);
```

`dskstate` is the disk DMA state machine
([`#L974-L992`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L974-L992)),
which leaves `DISKDMA_IDLE` only when a transfer is armed and running. The
port's own declaration says as much
([`#L100`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L100)):

```verilog
	output		  fdd_led, //disk activity LED, active when DMA is on
```

**The motor state exists in this module and the light never reads it.**
`motor_on[3:0]` is latched per drive on that drive's own select edge, exactly as
p.244 requires
([`#L624-L659`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L624-L659)):

```verilog
    else if (!_sel[0] && _sel_del[0])
      motor_on[0] <= ~_motor;
```

`motor_on` is read by `index`
([`#L576`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L576))
and by `syncint`
([`#L917`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L917)),
and by nothing else. The signal the page names as the light's cause is present,
correct, and unused by the light.

**There is also no per-drive light.** p.244 describes *"the activity light on the
front of the disk drive"* — one lamp per mechanism, each following that drive's
own latched motor bit. `fdd_led` is a single scalar for the whole controller.

### Measured

    A5/opus:8-cia.c26: fdd_led with motor OFF (DMA idle)=0,
                       fdd_led with motor ON  (DMA idle)=0

> **FAIL: opus:8-cia.c26** (p.244, quote *"DSKMOTOR* also controls the activity
> light on the front of the disk drive."*, recovered by the attack stage after
> classification discarded it as not-hardware): with disk DMA idle, `fdd_led`
> follows the selected drive's motor into the ON state — `paula_floppy.v:972`
> drives `fdd_led` purely from `dskstate!=DISKDMA_IDLE`.

> **FAIL: sol:8-cia.c24** — twin of the above, same printed quote, independently
> read, graded separately under its own uid: `fdd_led` off-state=0,
> on-state=0.

**The port is live, and the control names the real cause.**

> **PASS: CTRL-DMA-DRIVES-FDD-LED** (control case, no manual citation, same
> `fdd_led` readout as A5/A6): with **no drive selected at all**, arming disk DMA
> is what actually moves `fdd_led` from 0 to 1 — proving the port is live and
> naming DMA, not the motor, as its real cause.

That control is what makes the two reds a measurement rather than a dead output.
The light can be lit; it simply cannot be lit by the motor, and it *can* be lit
with no drive selected — a state in which no real drive's lamp would be on.

## What a user would actually see

This is not a register a program reads. It is a lamp, and the divergence is
visible from across the room.

- **The light stays dark through the whole of a seek.** A real drive lights up
  the moment the motor is latched on — before the 500 ms spin-up wait p.244
  itself prescribes, through the head stepping, through the settle. NanoMig's
  light is dark for all of it and flicks on only once the first DMA transfer
  begins.
- **The light goes dark between transfers.** trackdisk.device keeps the motor
  running after a read, for the next access. A real drive stays lit through
  that idle window. NanoMig's light blinks off the instant the state machine
  returns to `DISKDMA_IDLE`, so a multi-sector load reads as a stutter of
  flashes rather than one steady lamp.
- **Motor-only operations are invisible.** Spinning a drive up to read
  DSKRDY*, or to check whether a disk is present, lights a real drive and
  leaves NanoMig's dark.
- **The light comes on with no drive selected.** The control case above drives
  exactly this: DMA armed, `_sel` all inactive, light on. On real hardware the
  lamp belongs to a drive, and an unselected drive's lamp does not answer the
  controller.
- **It is one lamp, not four.** Two drives with different latched motor states
  show two different lamps on real hardware, which is how a user tells which
  drive is working. NanoMig has a single `fdd_led`.

The net effect is a light that reports *data transfer* where the Amiga's reports
*motor state* — the same substitution a PC floppy light makes, which is likely
why it reads as unremarkable until the page is put beside it.

## How this finding was nearly lost

This provenance belongs in the record, because it says something about the
pipeline as well as about the RTL.

**Classification discarded both claims**, as `not-asserted` with
`reason_kind: not-hardware`. Its written reason for `opus:8-cia.c26` was:

> "A lamp on the front of a disk drive is outside the chip and has no port. The
> one LED output in the disk path, `fdd_led` (paula_floppy.v:100), is not it: it
> is driven by DMA activity, not by `_motor`, so even the nearest port
> contradicts the sentence rather than carrying it."

**The reasoning that rejected the claim contains the finding.** The stage looked
at the right line, read it correctly, observed that it contradicts the page —
and concluded from the contradiction that the claim was untestable rather than
that it was false.

**The attack stage overturned both**, with the fixture the run above executes:

> verdict: overturned — "`fdd_led` is an explicit disk-activity-LED output on the
> module boundary, so the claimed motor-to-light relationship is directly
> gradeable. The RTL drives it solely from `dskstate != DISKDMA_IDLE`
> (paula_floppy.v:972), not from `_motor`, so the fixture would grade the claim
> false."

The lesson is narrow and worth stating plainly: *"the implementation does not do
this"* is a grading outcome, never a reason to decline to grade. A claim that a
port contradicts is the most testable kind there is.

## What the others say

**An emulator has no LED** — so the question is not whether it has one but what
it drives its UI indicator from. All four maintain one, and all four take it
from the motor.

| | code | |
|---|---|---|
| **Denise** | [`emulation/libami/drive/diskDrive.cpp:645-659`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L645-L659) | its own comment is the page: <pre>auto DiskDrive::updateDeviceState(bool force) -&gt; void {<br>&nbsp;&nbsp;// drive LED is hardwired to motor state<br>&nbsp;&nbsp;...<br>&nbsp;&nbsp;uint8_t LED = 0;<br>&nbsp;&nbsp;if (motor)<br>&nbsp;&nbsp;&nbsp;&nbsp;LED = system-&gt;getModel() &gt; 1 ? 1 : 2;</pre> **per drive**, called straight out of `setMotor` ([`:515`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L515)). `paula.fdcWriteMode()` ORs in a `0x80` write flag alongside — it changes what the indicator *says*, never whether it is lit |
| **vAmiga** | [`FloppyDrive.cpp:743-759`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L743-L759) | **`Msg::DRIVE_LED` is emitted from exactly one place in the whole codebase** — `FloppyDrive::setMotor(bool value)`, with `value` itself as the payload: <pre>msgQueue.put(Msg::DRIVE_LED, DriveMsg { i16(objid), value, 0, 0 });</pre> and `setMotor` is reached only from the select-edge latch at [`FloppyDrive.cpp:1427-1438`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L1427-L1438). p.244's two sentences, wired together |
| **WinUAE** | [`disk.cpp:917`, `:930`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L917-L930) | <pre>gid-&gt;drive_motor = drv-&gt;state;<br>...<br>gui_led (num + LED_DF0, (gid-&gt;drive_motor ? 1 : 0) \| (gid-&gt;drive_writing ? 2 : 0), -1);</pre> bit 0 — lit or not — is the motor. Bit 1 is `dskdmaen == DSKDMA_WRITE && drv->state && !selected` ([`:904`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L904)), a write indicator that colours the lamp and cannot light it |
| **Copperline** | [`src/floppy/mod.rs:558-561`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L558-L561) | <pre>pub fn activity_led_on(&self) -&gt; bool {<br>&nbsp;&nbsp;self.selected_drive()<br>&nbsp;&nbsp;&nbsp;&nbsp;.is_some_and(\|idx\| self.drives[idx].motor_on)<br>}</pre> motor of the selected drive, and nothing else. Consumed as `fdd_led_on` at [`src/bus.rs:5318`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L5318) and lit unconditionally by the status bar at [`src/video/window/statusbar.rs:475-482`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/window/statusbar.rs#L475-L482). **No DMA term anywhere on the path** |

**Where the four differ is only in the select term**, which p.244 does not
mention:

- **WinUAE adds it for external drives only**, with a comment saying why
  ([`disk.cpp:3659-3661`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3659-L3661)):
  *"external drives usually (always?) light activity led when selected. Internal
  only when motor is running."* — `floppy[dr].state = selected_led ||
  !floppy[dr].motoroff;`
- **Copperline requires selection for every drive**, so a spinning but
  deselected drive shows dark.
- **vAmiga and Denise use the latched motor alone**, per drive, with no select
  term at all — the page as printed.

**That disagreement is itself a result.** Two implementations read the page
literally; two decided a real drive's lamp also answers the select line. None of
them decides this finding, and none of them needed to: the split is about a
refinement the page is silent on, while all four agree on the term the page does
name, and NanoMig implements none of it.

## Suggested change

**One line, and it is already written in the upstream file.** Minimig-AGA's own
`paula_floppy.v` carries the alternative on the line after the live one
([`:545`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L545)):

```verilog
//assign disk_led = |motor_on;
```

Commented out, and NanoMig did not carry it forward. `|motor_on` is the page's
reading for a single aggregate lamp; `|(~_sel & motor_on)` would add
Copperline's select term.

**But this is a board decision, not only an RTL one.** `fdd_led` leaves the core
and lights a physical LED on the FPGA board, where its job may be to tell the
user the SD-card image is being accessed — for which DMA activity is the more
useful signal, and a light that stays on for the seconds a motor idles would be
worse. Changing it trades Amiga fidelity against board usability.

**The honest position:** the page is unambiguous, the RTL contradicts it, and
the right answer depends on what the LED on that board is *for*. That is a
question for the NanoMig maintainers, and it should be answered before the line
is touched.

## What holds the finding up

- **The sentence is unambiguous and names a physical object.** No conditional,
  no chipset qualification, no "may".
- **The line has no motor term at all.** This is not a masking or gating
  subtlety — `dskstate` and `motor_on` are unrelated signals.
- **The motor latch is implemented correctly and is green in the same run**, six
  assertions covering the rest of the same paragraph, so the module is not
  simply missing the motor concept.
- **The control case proves the port is live** and names DMA as its cause, with
  no drive selected.
- **All four other implementations drive their indicator from the motor**, and
  not one uses a DMA term to decide whether it is lit.

## What weakens it

- **Nothing a program can read is affected.** No register changes value, no
  timing changes, and no Amiga software can observe `fdd_led`. A conformance
  register whose other entries are about bus-visible behaviour contains nothing
  else quite like this.
- **NanoMig has no drive mechanism.** The floppy is served from an SD-card
  image, so "the front of the disk drive" has no referent in this hardware. The
  claim survives because the core exports a signal named for that lamp and the
  board wires it to one — but an implementer could reasonably say the page is
  describing a mechanism this design does not have.
- **The select term the page omits** is implemented by two of the four, which
  suggests the printed sentence is not a complete description of the real
  lamp's behaviour either.
