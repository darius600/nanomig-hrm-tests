# NM-HRM-070 — disk write DMA takes a word in every disk slot whatever FAST says, where pp. 249 and 265 give bit cells of 2 µs and 4 µs [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `paula.v`, `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L269) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v#L264) |
| **Report to** | **NanoMig.** The in-core drain that ties a disk write to the disk slots is NanoMig's own SD-card write path; in both Minimig lines the host link empties the FIFO instead. The unrouted bit 8 is inherited: neither line passes it out of `paula.v` either — see *What the others say* |
| **Test** | [`integration/ch5-audio-output-scanrate`](../../integration/ch5-audio-output-scanrate) — proposition P6, run on both CPU cores in ECS/PAL, OCS/PAL and ECS/NTSC: `G1` (FAST = 1, about 2 µs) and `G2` (FAST = 0, about 4 µs), claims `A-adkcon.b08`, `5-appA-p265.c21` (pp. 265, 249), and `G3` (a factor of two between the two, pp. 265, 249), red on fx68k and on TG68K in all three configurations; [`integration/ch8-disk-serial`](../../integration/ch8-disk-serial) — proposition P4, run on both CPU cores in ECS/NTSC and ECS/PAL: `P4-NTSC-FAST1`, `P4-PAL-FAST1` (FAST = 1, 1.5 µs ≤ T < 2.5 µs) and `P4-NTSC-FAST0`, `P4-PAL-FAST0` (FAST = 0, 3.5 µs ≤ T < 4.5 µs), claim `opus:8-adkcon.c33` (pp. 265, 249), red on fx68k and on TG68K |
| **Chapter** | 8 — Interface Hardware, printed pp. 241, 249, 250; Appendix A, printed pp. 265, 279; 6 — Blitter, printed p. 194; Appendix H, printed p. 369 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 9 red on each core, the same on fx68k and TG68K: `G1`, `G2` and `G3` in each of ECS/PAL, OCS/PAL and ECS/NTSC. In the same runs the preconditions are green on both cores: `B1` reads ADKCONR bit 8 back as 1 in the FAST run and 0 in the slow one, `B2` shows DMAON and DISKWRITE set, `B3` finds all 544 words of each buffer fetched in order, and `B4` finds the refresh groups 227 colour clocks apart inside each run's own graded interval. In `integration/ch8-disk-serial`, 4 red on each core, `P4-NTSC-FAST1`, `P4-NTSC-FAST0`, `P4-PAL-FAST1` and `P4-PAL-FAST0`, with each run's preconditions `4a` to `4f` green on both cores |
| **Verdict of the four** | **unanimous against NanoMig, and none implements the slow setting for a write**: WinUAE, vAmiga, Denise and Copperline all shift a disk write out at a fixed cell of about 1.97 µs (PAL) and take a word from memory only when their FIFO has room, so none takes every disk slot. None lets ADKCON bit 8 reach the write path: WinUAE uses it only when reading a disk, Denise only for DSKBYTR, vAmiga only to report it cleared, Copperline not at all. With a drive selected all four would pass `G1` and fail `G2` and `G3`; Copperline refuses a disk DMA with no drive selected, the package's condition |
| **See also** | [NM-HRM-028](NM-HRM-028.md) — ADKCON's audio bits, stored and read back and reaching nothing, in the same register; [NM-HRM-041](NM-HRM-041.md) — DSKBYTR, the other disk register whose fields a serial disk stream would drive |

---

## What the HRM says

**Two pages give the bit-cell time, and both give two values for one bit.**
Appendix A, **printed p. 265**, in the ADKCON entry:

> 08 FAST   Disk data clock rate control 1=fast(2us) 0=slow(4us).
> (fast for MFM, slow for MFM or GCR)

Chapter 8, **printed p. 249**, Table 8-8:

> Value of 1 selects two microseconds per bit cell (usually MFM). Data must be valid raw MFM. 0 selects four microseconds per bit (usually GCR).

Both sentences were checked on the scan. They differ only in the advice in
parentheses; the values are 2 µs and 4 µs on both pages, and neither restricts
them to reading or to writing.

**A word of DMA is sixteen of those cells.** Appendix A, **printed p. 279**, on
DSKDAT:

> This register is the disk DMA data buffer. It contains two bytes of data that are either sent (written) to or received (read) from the disk.

and **printed p. 250**, on what memory holds:

> The raw MFM data that must be presented to the disk controller will be twice as large as the unencoded data.

So memory holds one bit per cell, and a word written to the disk is sixteen
cells: **32 µs with FAST = 1 and 64 µs with FAST = 0.** Against p. 194's line
that is about two words a line and about one:

> During a horizontal scan line (about 63 microseconds), there are 227.5 "color clocks", or memory access cycles.

**The disk slots are an allowance, not a rate.** p. 194 goes on:

> The time-slot allocation per horizontal line is:
>
> 4 cycles for memory refresh
> 3 cycles for disk DMA

and, of the devices it allocates slots to:

> If a device does not request one of its allocated time slots, the slot is open for other uses.

Figure 6-9 on p. 195, *read from the drawing, not quoted*, draws the three disk
slots after the refresh group, and its legend's entry for disk DMA carries the
footnote mark whose note says that such operations take slots only if the
operation is being performed. Three slots a line are room for the two words a
2 µs cell needs; no page says disk DMA takes all three.

**Printed p. 241** describes the controller's reach in the same terms, a track
per revolution of the disk:

> It can DMA an entire track of raw MFM data into memory in a single disk revolution.

**The data stream does not wait for a selected drive.** Appendix H, **printed
p. 369**, on the write-data signal of the external disk connector:

> The system drives write data to all disks via this signal.

> Data is written only to selected drives.

**Read with the rest of the manual, nothing predicts a rate that ignores FAST,**
or one faster than 2 µs a cell. The only per-line number the manual gives disk
DMA is p. 194's allocation of three slots, which p. 194 itself leaves open to
other uses when the device does not ask.

---

## What NanoMig does

**ADKCON bit 8 is stored, read back, and goes nowhere.** Paula keeps fifteen
bits, [`src/minimig-aga/paula.v#L132`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L132), `reg [14:0] adkcon;`,
returns them through ADKCONR at [`#L198`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L198), `{1'b0,adkcon[14:0]}`,
and hands two of them on: bit 11 to the UART, [`#L211`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L211), `.uartbrk(adkcon[11]),`,
and bit 10 to the floppy controller, [`#L269`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L269), `.wordsync(adkcon[10]),`.
No other file of the design names the register. The floppy controller's one
ADKCON input is [`src/minimig-aga/paula_floppy.v#L98`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L98), `input wordsync,`.

**The write path has no bit-cell clock.** A disk write leaves the FIFO for
NanoMig's SD-card sector path, not for a serial shifter, and a word is taken
whenever the FIFO holds one,
[`paula_floppy.v#L767-L768`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L767-L768), `wire stbdat = trackwr & !fifo_empty & buffer_write_allowed;`,
on every tick of the 7 MHz enable, two a colour clock, unless the sector parser
is waiting for the SD card; the FIFO's read strobe is that and nothing else on a
write, [`#L911`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L911), `assign fifo_rd = (busrd & dmaon) | stbdat;`.

**So Paula asks for every disk slot.** Its request is up whenever the FIFO has
room, [`#L950`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L950), `assign dmal = dmaon & ((~dsklen[14] & ~fifo_empty) | (dsklen[14] & ~fifo_full));`,
which after the drain above is always. Agnus grants disk DMA in fixed colour
clocks of the line, [`src/minimig-aga/agnus_diskdma.v#L33-L46`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L33-L46), `//nominally disk DMA uses 3 slots: 08, 0A and 0C`, `8'h0C: dmaslot = 1;`,
and at [`#L49`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_diskdma.v#L49), `assign dma = dmal & dmaslot & hpos[0];`.
The slot table reads the beam position and nothing else. Four refresh slots
join the three only under the board's turbo-floppy option,
[`src/minimig-aga/minimig.v#L485`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L485), `.floppy_speed(floppy_config[0])`,
which the package's harness leaves clear. **The rate of a disk write is
therefore three words a line, set by the slot table, and ADKCON bit 8 cannot
change it.**

### Measured

[`integration/ch5-audio-output-scanrate`](../../integration/ch5-audio-output-scanrate)
runs two disk writes of 544 words each, from buffers holding raw MFM zeros
($AAAA), with no drive selected: run F with ADKCON = $8100 (FAST set), then run
S with ADKCON = $0100 (FAST cleared), each armed by p. 247's DSKLEN sequence.
It times the chip-RAM reads of words 16 to 528 of each buffer at the RAM port,
512 words, and converts through the package's 8 system-clock cycles per colour
clock and p. 141's clock interval.

| | FAST = 1 (run F) | FAST = 0 (run S) |
|---|---:|---:|
| ADKCONR bit 8 read back (`B1`) | 1 | 0 |
| p. 249 and p. 265: one bit cell | 2 µs | 4 µs |
| the page: one word, sixteen cells | 32 µs — about 2 words a line | 64 µs — about 1 word a line |
| the page: 512 words | about 256 lines | about 512 lines |
| measured: the same 512 words | **310520 cycles, 171 lines** | **310520 cycles, 171 lines** |
| measured: per word | 75.8 colour clocks — 3 words a line | 75.8 colour clocks — 3 words a line |
| measured: per bit cell | **1.336 µs** PAL, 1.324 µs NTSC | **1.336 µs** PAL, 1.324 µs NTSC |
| graded by | `G1`: 1.5 µs ≤ T < 2.5 µs | `G2`: 3.5 µs ≤ T < 4.5 µs |

`G3` grades the factor of two: |D_S − 2 D_F| is 310520 cycles, against a band
of three lines, 5472 cycles. The 171 lines are the package's own count: `B4`
finds 171 refresh groups, a mean 227.00 colour clocks apart, inside each run's
graded interval.

Every number is the same on fx68k and on TG68K, to the cycle, and the same in
ECS/PAL, OCS/PAL and ECS/NTSC. NanoMig's NTSC line is 227 colour clocks as its
PAL line is ([NM-HRM-010](NM-HRM-010.md)), so the NTSC run takes the same
cycles and only p. 141's clock interval changes the time per cell.

**1.336 µs is not a bit-cell time.** It is a third of a 227-colour-clock line
divided by sixteen: 512 words at three a line take 170.7 lines, and the package
counts 171. The measured rate is p. 194's allocation of three disk slots with
every slot of every line taken. It follows the one per-line number the manual
gives disk DMA, which the manual gives as room, not as a rate — and it follows
nothing either page says about FAST.

**A second package measures the same behaviour at the system boundary.**
[`integration/ch8-disk-serial`](../../integration/ch8-disk-serial), proposition
P4, writes 256 words of $AAAA from $20000 in ECS/NTSC and ECS/PAL, once with
ADKCON = $8100 after clearing bits 14–8 with $7F00 and once with the clear
alone, each armed by p. 247's DSKLEN sequence, and makes no CIA-B write. It
times the chip-RAM reads of words 64 and 192 at the RAM port and divides by
128 words of sixteen cells, with the colour clock taken from the refresh slots
(8 cycles, `4a`) and p. 255's colour-clock lengths. In all four runs, on both
cores, the two reads are 78072 cycles apart: 9759 colour clocks, two short of
43 lines of 227, three words a line as in the first package. That is a bit
cell of **1.331 µs** NTSC and **1.343 µs** PAL with FAST = 1 and with FAST = 0,
below both bands, so `P4-NTSC-FAST1` and `P4-PAL-FAST1` (1.5 µs ≤ T < 2.5 µs)
and `P4-NTSC-FAST0` and `P4-PAL-FAST0` (3.5 µs ≤ T < 4.5 µs) are red on fx68k
and on TG68K. The window is shorter, and its ends fall at other places in their
lines, which is why the values differ from the 1.324 µs and 1.336 µs above in
the last digits. In every run `4c` reads ADKCONR bit 8
back as written, `4d` finds DMAON and DISKWRITE set between the two reads,
`4e` finds words 0 to 192 read once each and in order, and `4f` finds the two
halves of the window 221 colour clocks apart in length, inside its bound of
456.

---

## What the others say

The question put to each: does the bit rate of a disk write follow ADKCON
bit 8, and what paces the words a write takes from memory?

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:85-86`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L85-L86), [`:645-652`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L645-L652), [`:4027-4041`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4027-L4041), [`:4839-4848`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4839-L4848), [`:5180-5183`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L5180-L5183), [`:3779-3782`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L3779-L3782) | <pre>/* 7 CCK per bit */<br>#define NORMAL_FLOPPY_SPEED (7 * 256)</pre> a write shifts one bit out per `trackspeed`, `while (floppybits >= trackspeed) {`, and gives up a FIFO word each time `bitoffset &= 15;` wraps, `if (!bitoffset) {`. With no drive selected the call is `disk_doupdate_write(cycles, get_floppy_speed());`, where `m = NORMAL_FLOPPY_SPEED * 100 / m;` is seven colour clocks a bit at the default 100 % floppy speed, 1.97 µs PAL; with a drive, the image's own speed. Paula asks for a word only for an empty FIFO place, `dmal ^= 63;`. **ADKCON bit 8 is read in one place, on the read side**, `return drv && !drv->fourms && !(adkcon & 0x0100) ? 2 : 1;`, which takes a slow bit from two cells of a normal image; the write path never calls it, so FAST = 0 writes at 1.97 µs as well |
| **vAmiga** | [`Core/Components/Paula/PaulaRegs.cpp:43-47`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L43-L47), [`Core/Components/Paula/DiskController/DiskControllerEvents.cpp:43-55`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskControllerEvents.cpp#L43-L55), [`Core/Components/Paula/DiskController/DiskController.cpp:331-347`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskController.cpp#L331-L347), [`:426-439`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskController.cpp#L426-L439), [`Core/Components/Paula/DiskController/DiskController.h:313`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskController.h#L313), [`Core/Components/Agnus/AgnusEvents.cpp:730-734`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusEvents.cpp#L730-L734) | <pre>if (clr &amp;&amp; GET_BIT(value, 8)) {<br>&nbsp;&nbsp;&nbsp;&nbsp;xfiles("ADKCON: FAST cleared (GCR) (%x)\n", value);<br>}</pre> clearing bit 8 is reported as unusual and stored; **no disk code reads it**. The disk's data clock is an event, one byte per `delay = (SEC(1) * 60.0) / (double)rpm / bytesPerTrack;` with `static constexpr double bytesPerTrack = 12668.0;` and 300 rpm, also with no drive, `isize rpm = drive ? drive->config.rpm : 300;`: 15.8 µs a byte, 1.97 µs a bit. Each event a write takes one byte from the FIFO, `u8 outgoing = readFifo();`, drive or not, `if (drive) drive->write8AndRotate(outgoing);`; the three disk slots, `paula.diskController.performDMA();`, take a word only while the six-byte FIFO has room, `if (!fifoCanStoreWord()) return;` |
| **Denise** | [`emulation/libami/paula/fdc.cpp:20-27`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L20-L27), [`:88-89`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L88-L89), [`:148-152`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L148-L152), [`:616`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L616), [`:200-203`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L200-L203), [`:467-470`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L467-L470), [`emulation/libami/paula/paula.h:183`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.h#L183), [`emulation/libami/paula/paula.cpp:57-61`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L57-L61) | <pre>#define FDC_BIT (7)<br>#define FDC_BYTE (FDC_BIT * 8)</pre> the event that shifts a write out comes every `agnus.updateEvent<Agnus::EVENT_FLOPPY>( fdcCycles );`, one bit's or, for an ADF, one byte's worth of these, under the comment `// The controller writes one bit in 7 DMA cycles.`, worked out there as `1.97356 us` PAL. Paula requests one slot per free FIFO place, `case 0: out \|= 63; break;`. The bit 8 accessor, `auto fast() -> bool const { return adkcon & 0x100; }`, is read only for DSKBYTR's WORDEQUAL window, `(!fast() ? 14 : 7)`. The write handler's own comment says *"the controller can only write with two fixed speeds"*, and the file's header that for an extended ADF the setting *"only plays a role when writing"*; the write path uses one |
| **Copperline** | [`src/floppy/mod.rs:55-56`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L55-L56), [`src/chipset/paula.rs:53`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L53), [`:1772-1777`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1772-L1777), [`src/floppy/mod.rs:1805-1806`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1805-L1806), [`:2753-2756`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2753-L2756), [`:149-154`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L149-L154), [`:2028-2033`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2028-L2033), [`src/bus/custom_regs.rs:424-425`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L424-L425) | <pre>const ADK_WORDSYNC: u16 = 1 &lt;&lt; 10;<br>const ADK_MSBSYNC: u16 = 1 &lt;&lt; 9;</pre> the floppy controller reads these two ADKCON bits, and Paula's model `const ADKCON_UARTBRK: u16 = 1 << 11;` and the audio bits; **bit 8 is read nowhere**. A write takes its next word from chip RAM when the head has turned one word further, `let word_cck = self.drives[idx].head_word_cck();`, a word of a standard track being `(PAULA_CLOCK_HZ / ROTATION_HZ / words).max(1)` with `const STANDARD_ADF_TRACK_WORDS: usize = 6334;`: 112 colour clocks, 7 a cell, 1.97 µs PAL. The transfer needs a selected drive with motor and media: with no drive selected, the arming write ends it at once, `return self.no_drive_completion();`, and raises DSKBLK with no word moved, `self.paula.intreq \|= crate::chipset::paula::INT_DSKBLK;` |
| **MiSTer** | [`rtl/paula.v:206`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v#L206), [`:264`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v#L264), [`rtl/paula_floppy.v:464`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L464), [`:477`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L477), [`:518`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L518) | descendant — the same two fan-outs, `.uartbrk(adkcon[11]),` and `.wordsync(adkcon[10]),`, so bit 8 reaches nothing here either, and the same request, `assign dmal = dmaon & (~dsklen[14] & ~fifo_empty \| dsklen[14] & ~fifo_full);`, over an identical `agnus_diskdma.v`. But the core does not empty a write FIFO itself: a word leaves when the host link reads one, `wire stbdat = cmd_fdd && stb7 && &cmd_cnt;`, `assign fifo_rd = (busrd & dmaon) \| (trackwr & stbdat);`, so the host firmware, not the slot table, sets the pace of a write |
| **TC64** | [`rtl/minimig/paula.v:225`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L225), [`:282`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L282), [`rtl/minimig/paula_floppy.v:420`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L420), [`:708`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L708) | descendant — the same two fan-outs, `.uartbrk(adkcon[11]),` and `.wordsync(adkcon[10]),`, and the write FIFO emptied by the SPI host, `assign spidat = cmd_fdd && rx_strobe7 && rx_cnt7==3 ? 1'b1 : 1'b0;`, `assign fifo_rd = (busrd & dmaon) \| (trackwr & spidat);`. Neither Minimig line has the in-core drain that ties NanoMig's write to the disk slots; that drain is NanoMig's own |

**All four shift a disk write out at a fixed cell of about 2 µs, and none takes
a word per disk slot.** Seven colour clocks a bit is a named constant in WinUAE
and Denise; vAmiga reaches the same 1.97 µs from 12668 bytes a revolution at
300 rpm, and Copperline from a standard track of 6334 words. Each takes a word
from memory only when its FIFO has room, so its memory reads follow its cell
clock, not the slot table. **None of them lets bit 8 reach the write path**:
WinUAE reads it only when it reads a disk, vAmiga only to report it cleared,
Denise only for DSKBYTR, Copperline not at all. So the four agree with the page
for FAST = 1 and with each other against it for FAST = 0: with a drive
selected, all four would pass `G1` and fail `G2` and `G3`, where NanoMig fails
all three. Copperline would not reach P6 in the package's own condition, since
it refuses a disk DMA with no drive selected.

**Neither Minimig line has an in-core write stage to pace.** Both leave bit 8
unrouted as NanoMig does, and both hand a write's words to the host over the
link that feeds them their disk data. NanoMig's slot-rate write comes from its
own SD-card path; the dead bit is inherited.

---

## Suggested change

**One option: give the write path a cell clock and let bit 8 set it.** Pass
`adkcon[8]` into `paula_floppy` beside `wordsync` (line 269 of `paula.v`), count
cells on the 7 MHz enable — 14 ticks a cell with FAST = 1, 1.97 µs PAL, and 28
with FAST = 0 — and let `stbdat` (line 768 of `paula_floppy.v`) take a word from
the FIFO once every sixteen cells. Paula's request already asks only while the
FIFO has room, so the disk slots would then carry about two words a line with
FAST = 1 and about one with FAST = 0. This has not been run here. What it would
change:

- `G1` and `G2` would measure about 1.97 µs and 3.95 µs, and `G3` a factor of
  two;
- every disk write would take the time the page gives it, and the SD-card
  sector path would receive words at that pace instead of at three a line;
- with FAST = 0, NanoMig would follow the page where none of the four
  implementations does, which is the other half of the decision;
- the board's turbo-floppy option, which adds the four refresh slots to disk
  DMA, would no longer speed up a write unless it also bypassed the cell clock.

---

## What holds the finding up

- **Two pages, the same two values.** p. 265 and p. 249, both checked on the
  scan, give 2 µs for FAST = 1 and 4 µs for FAST = 0. No page gives a disk data
  rate that ignores the bit, or one faster than 2 µs a cell, and the only
  per-line number for disk DMA, p. 194's three slots, is printed as an
  allocation a device may leave unused.
- **The measurement names its own inputs.** In both runs `B1` reads bit 8 back
  as written, `B2` shows the DMA on and writing, `B3` finds every one of the 544
  words fetched in order, and `B4` counts 171 refresh groups 227 colour clocks
  apart across the graded interval. The two runs differ in bit 8 alone.
- **The two durations are equal to the cycle**, 310520 in both runs, in all
  three configurations, on both cores: one rate, not two rates that happen to
  be close. The MEASURED line prints D_F, D_S, T_F and T_S, so each verdict can
  be read off the printed numbers.
- **The source says why.** Bit 8 has no fan-out, the write drain has no cell
  clock, and the slot table reads the beam position alone. The unit suite
  records the first of these at Paula's own boundary:
  [`hrm/ch8-adkcon-disk-control-bits`](../../hrm/ch8-adkcon-disk-control-bits)
  measures bits 12, 9 and 8 storing and reading back, and records from
  `paula.v` that they reach no logic (its MEASURED line
  `C8-MFMPREC-MSBSYNC-FAST`), and
  [`hrm/ch8-adkcon-and-interrupt-relations`](../../hrm/ch8-adkcon-and-interrupt-relations)
  grades their storage, green. Neither grades what bit 8 does; this package is
  the first that does.
- **What weakens it: for FAST = 0 no implementation follows the page on a
  write.** All four write at about 2 µs whatever bit 8 says, so `G2` and `G3`
  would be red against every implementation read here; `G1` is where NanoMig
  alone departs from the page and from all four. And the package measures with
  no drive selected and no disk image, a condition pp. 249 and 265 do not
  mention; p. 369 has the system drive write data to all disks.

---

## For the specialists

1. **What does Paula do with FAST = 0 on a write?** The page says 4 µs a cell,
   and p. 241 names GCR formats — Apple II, and Commodore 1541/1571 disks
   *"with motor speed tricks"* — that would use it. No implementation read here
   writes at 4 µs, so the slow half rests on the page alone.
2. **Should an SD-card write keep the slot rate?** NanoMig's writes end in SD
   sectors, and a paced drain makes every disk write take the time the page
   gives it. Whether the board's turbo-floppy option should keep a fast path
   for writes is a design question the page does not answer.
3. **The read side has the same shape, by the source.** The FIFO is filled from
   the SD sector buffer whenever it has room and emptied in the disk slots, and
   bit 8 reaches it no more than it reaches a write. No package measures the
   rate of a disk read.
