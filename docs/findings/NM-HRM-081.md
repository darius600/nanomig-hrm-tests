# NM-HRM-081 — after a reset WPRO* reads a writable disk as writable, where pp. 334 and 370 have a drive set a write-protect flip-flop on DRESB*

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. pp. 334 and 370 have an external drive set a write-protect flip-flop on DRESB*, and name neither what clears it nor a line that shows it: WPRO* is asserted by a write-protected disk (p. 334 pin 14, p. 369). The assertions read WPRO* 16 clocks after the reset ends, a level and a time no page gives. The one consequence the pages fix, no writing while DRESB* is active, holds in NanoMig: the reset stops a disk write and holds its sector-write request off. |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L664) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L533-L541) |
| **Test** | [`hrm/appDE-disk-interface`](../../hrm/appDE-disk-interface) — `sol:E-disk.c35` and `opus:E-extdisk-iface.c13` (p. 334 pin 10) still fail, because each requires WPRO* asserted for a writable disk 16 clocks after the reset is released, on drive 0; the motor half of each is green |
| **Chapter** | Appendix E, printed p. 334; Appendix H, printed pp. 367–370; 8 — Interface Hardware, printed p. 244 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red of 13 graded (`sol:E-disk.c35`, `opus:E-extdisk-iface.c13`; the package reads `9 passed, 4 failed.`, and its other two reds are [NM-HRM-080](NM-HRM-080.md)'s). Each reads drive 0's motor latch on before the reset and off after it, green, and WPRO* inactive 16 clocks after the reset with `floppy_wrprot` at 0, red |
| **Error class** | **a silence read as a requirement** |
| **Verdict of the four** | **3–1 with NanoMig**: WinUAE, vAmiga and Denise keep no write-protect state across a reset and take DSKPROT from the disk's own protection, so a writable disk reads writable straight after one; Copperline asserts DSKPROT on each external drive at every reset and releases it after 1 ms, a figure no page prints, and leaves DF0 alone |
| **See also** | [NM-HRM-080](NM-HRM-080.md) — the same package's two reds on RDY* |

---

## Why it was withdrawn

Both red assertions latch drive 0's motor on, keep the configured disk
writable (`floppy_wrprot` 0), hold the module's reset for 16 clocks, release
it, wait 16 clocks, and require WPRO* asserted. NanoMig reads it inactive,
and that half of each assertion is red; the motor half is green.

**No page ties the flip-flop to WPRO*.** Every page that describes the line
names the disk: p. 334 pin 14, *"Asserted by selected, write-protected disk."*;
p. 369, *"A selected drive pulls this signal low whenever it has a
write-protected diskette installed."*; p. 244, DSKPROT*, *"Disk is write
protected (active low)."* None of them names a flip-flop, and pp. 334 and 370,
which name the flip-flop, name no line it drives. A drive that follows the
pages may hold WPRO* inactive for a writable disk throughout.

**No page says how long the flip-flop stays set, or what clears it.** The
pages give the drive's other two latches a rule at both ends: the motor
flip-flop is clocked at each select (p. 334 pin 8, p. 369), and the change flop
is set at power-up or with no disk and reset by a select and a step with a disk
in (p. 334 pin 11, p. 369). The write-protect flip-flop gets only its setting,
from DRESB* (p. 334 pin 10, p. 370) and from a supply below 3.75 V (p. 334
pin 12, p. 370). p. 370 gives DRESB-'s own durations, about one second at
power-up and about 17 µs for a RESET instruction; nothing says whether a drive
stops protecting itself when DRESB- ends or at some later event. Sixteen clocks
after the release is not a time any page names.

**What the pages do fix is that a drive does not write while DRESB* is
active, and NanoMig writes nothing then.** In a replay, a disk write DMA armed
before the reset stops at the reset: the DMA request and the drive light are
off on every one of the 16 reset clocks and still off 1016 clocks after the
release, and the reset itself holds the sector-write request off (*What NanoMig
does*, below). The motor half, p. 334 pin 10's
*"reset their motor-on flip-flops"* and p. 244's *"All drive motors turn off
after system reset"*, is graded and green. So at every point the pages fix,
NanoMig reads as they predict; the red is the assertion's own level at the
assertion's own time.

**The pages describe external drives, and the package reads drive 0.** p. 334
is the external connector and p. 370 says *"External disk drives"*; no page
gives the internal drive a write-protect duty. The replay reads drives 1, 2
and 3 the same as drive 0.

---

## What the HRM says

**Appendix E, printed p. 334**, the external disk connector's pin table, pin 10
DRESB*, direction OC:

> Amiga system reset. Drives should reset their motor-on flip-flops and set
> their write-protect flip-flops.

pin 12, +5V:

> When below 3.75V, drives are required to reset their motor-on flops, and set
> their write-protect flops.

pin 14, WPRO*, direction I/O:

> Asserted by selected, write-protected disk.

and, for contrast, the two other latches the table describes, pin 8 MTRXD* and
pin 11 CHNG*:

> Motor on data, clocked into drive's motor-on flip-flop by the active
> transition of SELxB*.

> Drive's change flop is set at power up or when no disk is not installed.
> Flop is reset when drive is selected and the head stepped, but only if a disk
> is installed.

**Appendix H, printed p. 367**, gives DRESB- the note *Reset*, and **p. 368**
gives pin 14, WRPRO-, the note *Write protect* (both read from the page
images). **p. 369**:

> A selected drive pulls this signal low whenever it has a write-protected
> diskette installed.

> This latch is set when the drive is first powered on, or whenever there is no
> diskette in the drive. To reset the latch, the system must select the drive,
> and step the head.

**p. 370**, under DRESB- (Pin #10):

> This signal is a buffered version of the system reset signal. Three things
> can make it go active (low):

> System power-up (DRESB- will go low for approximately one second);

> System CPU executes a RESET instruction (DRESB- will go low for approximately
> 17 microseconds);

and a hard reset from the keyboard, lasting as long as the keyboard reset is
held (read from the page image). Then:

> External disk drives should respond to DRESB- by shutting off their motor
> flip-flops and write protecting themselves.

> A level of 3.75v or below on the 5v+ requires external disks to write-protect
> and reset the motor on line.

**Chapter 8, printed p. 244**, Table 8-5:

> Disk is write protected (active low).

> All drive motors turn off after system reset.

The write-protect flip-flop appears on pp. 334 and 370 and nowhere else in the
manual. Neither page says what clears it, how long it outlasts DRESB-, or
whether WPRO* shows it.

---

## What NanoMig does

**WPRO* follows a configuration bit, and nothing a reset does reaches it.**
[`src/minimig-aga/paula_floppy.v#L664`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L664),
`assign _wprot = &(_sel | disk_writable);`, and
[`#L968-L969`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L968-L969),
`// for now, either all or none of the floppy drives is write protected` /
`assign disk_writable = floppy_wrprot?4'b0000:4'b1111;`. `floppy_wrprot` is the
board's configuration,
[`src/minimig-aga/minimig.v#L546`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L546),
`.floppy_wrprot(floppy_config[1]),`. The module holds no write-protect state
of its own, and the sector-write request carries no write-protect term,
[`#L406-L411`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L406-L411),
`sdc_wr <= ~_sel;`.

**The reset reaches the rest of the drive and the whole write path.** The
module's `reset` is the system reset ORed with the 68000's reset line,
[`src/minimig-aga/minimig.v#L422`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L422),
`wire        reset = sys_reset | ~_cpu_reset_in;`. It clears each drive's motor
latch,
[`#L625-L632`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L625-L632),
`if (reset)` / `motor_on[0] <= 0;`; it holds the sector-buffer machine idle
with no read or write requested,
[`#L256-L259`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L256-L259),
`fd_dma_buf_wr_state <= 4'h0;` / `sdc_wr <= 4'b0000;`; and it returns disk DMA
to idle and clears DSKLEN and its enable,
[`#L977-L978`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L977-L978),
`dskstate <= DISKDMA_IDLE;`, and
[`#L711-L745`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L711-L745),
`dsklen[14:0] <= 0;`, `dsklen[15] <= 0;`, `dmaen <= 0;`.

### Measured

[`hrm/appDE-disk-interface`](../../hrm/appDE-disk-interface) reads
`9 passed, 4 failed.`; for `sol:E-disk.c35` it prints `motor_on latched before
reset=1, after reset=0 (motor half GREEN); _wprot after reset
(floppy_wrprot=0)=0 (write-protect half RED)`, and for its twin
`opus:E-extdisk-iface.c13` `motor half=GREEN, wprot half=RED`.

Replayed with the package's rig, a writable disk mounted, the motor latched on
at a select edge, the drive selected throughout — a measurement for this page,
not a graded assertion:

| | WPRO* before | during the 16 reset clocks | at release, +16, +100016 clocks | motor latch after |
|---|---|---|---|---|
| drive 0, one drive configured, `floppy_wrprot` 0 | inactive | inactive on 16 of 16 | inactive | off |
| each of drives 1, 2, 3, four configured, `floppy_wrprot` 0 | inactive | inactive on 16 of 16 | inactive | off |
| drives 0 and 1, `floppy_wrprot` 1 | asserted | asserted on 16 of 16 | asserted | off |

A disk write DMA on drive 0, DSKLEN written with $C00A:

| | before | during 16 clocks | 16 and 1016 clocks later |
|---|---|---|---|
| reset held for the 16 clocks | DMA request and drive light on | both off on 16 of 16 | both off |
| control, no reset | DMA request and drive light on | both on during 16 of 16 | both on |

No sector write is requested in either run: the rig feeds no sector, so the
write machine never reaches its request.

---

## What the others say

The question put to each: does a reset leave a drive write-protected, and does
DSKPROT show it for a disk that is writable?

| | code | |
|---|---|---|
| **WinUAE** | [`newcpu.cpp:8038-8059`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L8038-L8059), [`:5485-5497`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L5485-L5497), [`custom.cpp:6556-6566`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6556-L6566), [`devices.cpp:232-247`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/devices.cpp#L232-L247), [`disk.cpp:5482-5497`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L5482-L5497), [`:849-897`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L849-L897), [`:1346-1366`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L1346-L1366), [`:834-847`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L834-L847), [`:3742-3743`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3742-L3743), [`:2929`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L2929) | a RESET instruction, `bool cpureset (void)`, calls `custom_reset_cpu(false, false);`, which reaches `devices_reset(hardreset);` and `DISK_reset();`; that stops disk DMA, `dskdmaen = 0;`, and runs `reset_drive (dr);` on every drive: `drv->motoroff = 1;`, then the image goes back in with its forced protection off, `if (!drive_insert (drv, &currprefs, num, currprefs.floppyslots[num].df, false, false))` / `drv->forcedwrprot = forcedwriteprotect;`. DSKPROT is `if (drive_writeprotected (drv))` / `st &= ~8;`, and `drive_writeprotected` is `return currprefs.floppy_read_only \|\| drv->wrprot \|\| drv->forcedwrprot \|\| drv->diskfile == NULL;`, the configuration's and the image's protection, which also gate a write, `if (drive_writeprotected (drv) \|\| drv->trackdata[tr].type == TRACK_NONE) {`. **No write-protect state from a reset**: a writable disk reads writable straight after one |
| **vAmiga** | [`Core/Components/CPU/CPU.cpp:177-186`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/CPU.cpp#L177-L186), [`Core/Peripherals/Drive/FloppyDrive.h:165-189`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.h#L165-L189), [`Core/Peripherals/Drive/FloppyDrive.cpp:704-705`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L704-L705), [`:439-442`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L439-L442), [`Core/Peripherals/Drive/Drive.h:69`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/Drive.h#L69) | a RESET instruction is a soft reset, `case Instr::RESET:` / `amiga.softReset();`, and a soft reset leaves the drive untouched, `if (isSoftResetter(worker)) return;`; a hard reset clears `<< motor` and the rest of that list, which holds no protection flag. DSKPROT is `if (!hasUnprotectedDisk()) { result &= 0b11110111; }`, with `bool hasUnprotectedDisk() const { return hasDisk() && !hasProtectedDisk(); }` and `return hasDisk() ? disk->isWriteProtected() : false;`, the disk's own flag. **No write-protect state from a reset**; on a RESET instruction not even the motor latch is cleared |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:285-287`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L285-L287), [`emulation/libami/system/system.cpp:231-260`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L231-L260), [`emulation/libami/drive/diskDrive.cpp:363-366`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L363-L366), [`:337-338`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L337-L338), [`:418`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L418), [`:266`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L266), [`emulation/libami/interface.cpp:663-668`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/interface.cpp#L663-L668) | a RESET instruction, `system->power( true, true );`, and every other reset run `auto System::power(bool softReset, bool resetInstruction) -> void {`, which ends in `drive.power();` for each drive: `motor = false;`, and the protection is left alone. The protection belongs to the medium: `structure.writeProtected = state;` in `auto DiskDrive::writeProtect(bool state) -> void {`, which the media interface calls, `auto Interface::writeProtectDisk(Media* media, bool state) -> void {`; no reset path writes it. DSKPROT is `if (structure.writeProtected) out &= ~8;`, and a write is dropped under `if (!selected \|\| structure.writeProtected)`. **No write-protect state from a reset** |
| **Copperline** | [`src/cpu.rs:5039-5041`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L5039-L5041), [`src/bus.rs:4971-4978`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L4971-L4978), [`:5304-5306`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L5304-L5306), [`:5133-5289`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L5133-L5289), [`src/floppy/mod.rs:1149-1153`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1149-L1153), [`:3079-3092`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3079-L3092), [`:3213-3217`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3213-L3217), [`:84`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L84), [`:498-503`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L498-L503), [`:2302-2309`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2302-L2309), [`src/floppy/tests.rs:1307-1342`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/tests.rs#L1307-L1342) | a RESET instruction, `self.bus.reset_custom_chips_from_cpu_reset();`, and a power-on reset both call `self.reset_for_keyboard_reset();`, the keyboard reset, `pub fn reset_for_keyboard_reset(&mut self) {`, which calls `self.floppy.reset_external_drives();`; for every drive but DF0, `for drive in self.drives.iter_mut().skip(1) {`, that turns the motor off and sets `self.write_protected_sense = true;` with `self.status_settle_cck = DISK_STATUS_SETTLE_CCK;`, and when the count runs out `self.write_protected_sense = self.write_protected_target;`, the disk's own state. `const DISK_STATUS_SETTLE_CCK: u32 = PAULA_CLOCK_HZ / 1_000;` is 1 ms. DSKPROT reads the sense, `if !drive.write_protected_sense {` / `bits \|= CIAA_DSKPROT;`; a write checks the image instead, `if image.write_protected {`. Two unit tests state it, `fn dresb_resets_external_motor_latch_and_write_protect_sense() {` and `fn dresb_does_not_reset_internal_df0_motor_latch() {`. **Asserts DSKPROT on each external drive for 1 ms after every reset**, then follows the disk; DF0 is left alone |
| **MiSTer** | [`rtl/paula_floppy.v:533-541`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L533-L541), [`:362`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L362) | descendant — the reset clears the drive status register, `{disk_writable[3:0],disk_present[3:0]} <= 8'b0000_0000;`, and only a status word from the host loads it again, `else if (rx_data[15:12]==4'b0001 && stb7 && !cmd_cnt)`; with `assign _wprot = &(_sel \| disk_writable);` every selected drive reads write-protected, and empty, from the reset until that word arrives. NanoMig replaced the register with the configuration bit |
| **TC64** | [`rtl/minimig/paula_floppy.v:764-772`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L764-L772), [`:598`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L598) | descendant — the same register and the same reset, `{disk_writable[3:0],disk_present[3:0]} <= 8'b0000_0000;`, loaded under `else if (~direct_flag && rx_data[15:12]==4'b0001 && rx_strobe7 && rx_cnt7==0)`, and the same `assign _wprot = &(_sel \| disk_writable);` |

**Three of the four keep no write-protect state across a reset.** WinUAE,
vAmiga and Denise take DSKPROT from the disk's own protection whatever reset
came before, and WinUAE stops disk DMA at the reset, as NanoMig does.
Copperline alone carries p. 370 into its drives, on the external ones only, as
p. 370 names them, and ends the protection after 1 ms, the settle time it also
gives a disk change and a change of the disk's protection. Both Minimig lines
read every drive write-protected from a reset until their host sends the next
status word, a consequence of resetting that register. Each implementation
that shows the flip-flop picks its own end for it; none of them finds one in
the pages.

---

## What the pages let a test grade

- **The motor half.** p. 334 pin 10, p. 370 and p. 244 turn the motor latch off
  at a reset. The package grades it on drive 0 and it is green.
- **No writing while DRESB* is active.** At this module that is the disk write
  DMA and the sector-write request under a held reset; the replay above reads
  both off on every reset clock.
- **Nothing about WPRO* for a writable disk**, during a reset or after one: the
  pages tie the line to the disk, and give the flip-flop no end.
- **Nothing about a supply below 3.75 V**: no supply level exists at any
  boundary of the RTL.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 9 — agrees with the manual:

  > "Amiga system reset. Drives should reset their motor on flip flops and set their write protect flip flops."
  <!-- corpus: a500-a2000-trm-1987/p009.txt -->

  The external disk connector's pin table prints p. 334's notes for pins 10, 12 and 14: the reset and the low supply set the write-protect flip-flops, and WPRO* is asserted by a selected, write-protected disk. It names no clearing event either.

Silent on this point: techtravels, *Inside Paula's Floppy Controller*, part I, 2026, which names reset among the inputs of Paula's write-gate sequencer and does not say what it does there; 6502.org dissection of Paula 8364R7, 2023.

---

## For the specialists

1. **Does a real external drive assert WPRO* while its write-protect flip-flop
   is set, and what clears the flip-flop?** Resetting a machine with a
   writable disk in drive 1 and reading CIAAPRA bit 3 at intervals after the
   reset would answer the first; a write started at intervals after it would
   answer the second. The pages answer neither.
2. **Where the line comes from in NanoMig.** Its only write-protect state is
   the board's configuration bit, which no reset sets and which gates no
   write inside the module. The Minimig lines' reset-cleared status register
   and Copperline's 1 ms are two readings of the flip-flop; neither is on the
   page.
