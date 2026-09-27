# Using an emulator as a third witness

The manual is the oracle of this suite. Sometimes the manual is not enough: it
can state a thing twice and disagree with itself. When that happens the question
is no longer "does the RTL follow the manual" but "which page of the manual is
right", and no amount of re-reading the manual settles it.

This file records when an outside source was consulted, exactly what was read,
and what it established.

## The rule

An emulator is **not** promoted to oracle. It is a witness about the hardware,
which is a different thing from a witness about the manual, and it is admitted
only under these conditions:

1. **Only for a contradiction inside the manual.** If the manual is clear and the
   RTL disagrees with it, the finding stands; an emulator agreeing with the RTL
   would mean the manual is wrong about the hardware, and that is a separate
   question with a separate answer. It never quietly cancels a finding.
2. **Two independent implementations.** One emulator agreeing with one reading is
   an opinion. WinUAE and vAmiga were written separately, by different people.
3. **Read the code that computes it, not the comments.** A comment repeats what
   its author believed; the arithmetic is what the emulator does.
4. **Cite it so it can be checked** — permalink to the commit, not to a branch.
   A branch link points somewhere else next month.

Reading someone else's implementation is the same category as reading NanoMig's
RTL, which this project deliberately refuses to use as a source of expectations.
That refusal still holds. The emulators are used here to decide between two
sentences of the manual, never to supply an expectation the manual does not
contain.

## Sources consulted

| | Repository | Commit read | Date |
|---|---|---|---|
| vAmiga | [dirkwhoffmann/vAmiga](https://github.com/dirkwhoffmann/vAmiga) | [`35de004`](https://github.com/dirkwhoffmann/vAmiga/tree/35de004a2f1502c2811b9a8ebcde4d0979a98937) | 2026-09-06 |
| WinUAE | [tonioni/WinUAE](https://github.com/tonioni/WinUAE) | [`ccc407e`](https://github.com/tonioni/WinUAE/tree/ccc407e9c636668b99b5f19d6e9aa03ea04b3052) | 2026-09-06 |

Both were shallow-cloned outside the repositories, never built, and nothing from
them is copied into a test.

## NM-HRM-002 — audio stereo routing — FILED AGAINST THE PRINTED PAGE

**The conflict.** Chapter 5, p.137: *"Channels 1 and 2 are connected to the
left-side stereo output jack. Channels 0 and 3 are connected to the right-side
output jack."* The appendix pin list, p.326, says the opposite: *"AUDO — Audio output
from left (channels 0, 3) port"*, *"AUDI — Audio input to right (channels 1, 2)
port"*. Both appear in the printed and the Developer-CD edition.

### WinUAE

[`audio.cpp` L1153–L1165](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1153-L1165),
the stereo mix:

```c
data1 = datas[0] + datas[3];
data2 = datas[1] + datas[2];
...
put_sound_word_right(data1);
put_sound_word_left(data2);
```

The function is named for the right, but it writes channels 0 and 3 as the
**first** sample of each stereo frame (*"Always put the right word before the
left word"*,
[L516](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L516)),
and the frame is played as `KSAUDIO_SPEAKER_STEREO`
([`od-win32/sounddep/sound.cpp` L1026](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/sounddep/sound.cpp#L1026)),
whose first sample is the left speaker. Channels 0 and 3 are heard on the left
unless *Swap channels* is set: the pin list's side.

### vAmiga

[`Core/Ports/AudioPort.cpp` L472–L473](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/AudioPort.cpp#L472-L473):

```cpp
double l = ch0 * (1 - pan0) + ch1 * (1 - pan1) + ch2 * (1 - pan2) + ch3 * (1 - pan3);
double r = ch0 * pan0 + ch1 * pan1 + ch2 * pan2 + ch3 * pan3;
```

so `pan = 0` is fully left and `pan = 1` fully right. The conversion is at
[`AudioPort.cpp` L250](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/AudioPort.cpp#L250):

```cpp
pan[channel] = float(0.5 * (sin(double(config.pan[channel]) * M_PI / 200.0) + 1));
```

and the defaults at
[`Core/Infrastructure/Defaults.cpp` L195–L198](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Defaults.cpp#L195-L198)
are `AUD_PAN0 = 50`, `PAN1 = 350`, `PAN2 = 350`, `PAN3 = 50`:

| channel | cfg | sin(cfg·π/200) | pan | side |
|---|---:|---:|---:|---|
| 0 | 50 | +0.707 | 0.85 | right |
| 1 | 350 | −0.707 | 0.15 | left |
| 2 | 350 | −0.707 | 0.15 | left |
| 3 | 50 | +0.707 | 0.85 | right |

Channels 0 and 3 mostly right, 1 and 2 mostly left: chapter 5's side. The
settings pane's *Stereo* preset, 100, 300, 300 and 100
([AudioSettings.swift L149–L153](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/GUI/Settings/ViewControllers/AudioSettings.swift#L149-L153)),
puts channels 0 and 3 fully right.

### Result

The two emulators split: WinUAE plays channels 0 and 3 on the left, as the pin
list prints, and vAmiga on the right, as chapter 5 prints.
`paula_audio_mixer.v` follows the pin list
([L76–L77](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_mixer.v#L76-L77)):

```verilog
  ldatasum <= #1 {msample0[13], msample0} + {msample3[13], msample3};
  rdatasum <= #1 {msample1[13], msample1} + {msample2[13], msample2};
```

Denise and Copperline put channels 0 and 3 on the left too
([`fourth-witness-denise-copperline.md`](fourth-witness-denise-copperline.md)),
so NanoMig's side is that of three of the four. The finding is filed against
the printed page: p.137 is contradicted by p.326 of the same manual, and
[`NM-HRM-002`](../findings/NM-HRM-002.md) sets out the evidence beyond the
emulators. The four assertions in `hrm/ch5-audio-stereo-routing` stay red,
because each printed sentence is graded on its own authority and they measure
p.137.

## NM-HRM-003, NM-HRM-004, NM-HRM-005 — read out of interest

These three were read to see *why* the RTL deviates, and they come out three
different ways. **On NM-HRM-003 both emulators narrow an OCS fetch address to the
manual's width**, on the bus rather than in the register. **On NM-HRM-004 both do
what NanoMig does and what Appendix B marks**, against Appendix C's p.310, and
the finding is filed against the printed page because the manual contradicts
itself. **On NM-HRM-005 vAmiga gives NanoMig's three numbers**; WinUAE was not
compared.

Where an emulator agrees with NanoMig against a page that is clear, as vAmiga
does on NM-HRM-005, that does not close the finding. It moves it: the question
is no longer "is the RTL wrong" but "is the manual wrong about the hardware",
and that is a decision, not a measurement.

**Limits of this section.** The emulators were read, not run. The vAmiga cycle
counts below come from following its event schedule in the source, not from
instrumenting it.

### NM-HRM-004 — the danger bit

[vAmiga `Copper.cpp` L395–L402](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L395-L402):

```cpp
bool Copper::isIllegalAddress(u32 addr) const
{
    if (cdang) {
        return agnus.isOCS() ? addr < 0x40 : false;
    } else {
        return addr < 0x80;
    }
}
```

[WinUAE `custom.cpp` L2809](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2809)
says the same thing in one line:

```c
if (addr < ((copcon & 2) ? (ecs_agnus ? 0 : 0x40) : 0x80)) {
```

Both: **CDANG=0 blocks below $80 regardless of chipset**; CDANG=1 opens $40 on
OCS and everything on ECS. That is exactly what `agnus_copper.v` does, and it is
what the test calls a deviation.

The finding sets two printed statements against each other. Appendix C, p.310:

> "Now, in the ECS, if this bit is set, the Copper can access all of the Amiga
> chip registers. **If this bit is clear**, the Copper can access the address
> range from $DFF03E through $DFF07E, the same range as when the danger bit is
> set in the standard chip set."

Appendix B, pp.289-290, marks every address from `$040` to `$07E` with the `~`
its legend on p.287 defines as *"Address not writable by the Copper unless the
"copper danger bit", COPCON is set true"*, the rows of BLTCON0L, BLTSIZV,
BLTSIZH and DENISEID included, registers that exist only in ECS.

Neither emulator implements p.310's CDANG-clear range; both do what Appendix B
marks, as NanoMig does, and so do Denise and Copperline.
[`NM-HRM-004`](../findings/NM-HRM-004.md) is filed against the printed page.

### NM-HRM-003 — the location register width

Neither emulator narrows the Copper pointer **register** by chipset.

[vAmiga `Copper.cpp` L29–L31](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L29-L31):
`setPC(u32 addr) { coppc = addr; }` — no mask. The only `isOCS()` anywhere in
vAmiga's Copper directory is the CDANG check above.

[WinUAE `custom.cpp` L2888–L2890](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2888-L2890):
`cop1lc = (cop1lc & 0xffff) | ((uae_u32)v << 16);` — the whole written word goes
in.

Both narrow the address the Copper fetches from instead, and by chipset.
WinUAE sets `chipmem_full_mask = 0x80000 - 1` when `CSMASK_ECS_AGNUS` is clear
([`memory.cpp` L2619–L2623](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2619-L2623)),
19 bits on OCS, on the Agnus-side chip-RAM path every DMA fetch uses; in the
same function `cs_agnussize` overrides it (L2627–L2644), and with JIT on the
mask becomes the chip RAM size (L2646–L2648). vAmiga begins every Agnus DMA read
with `addr &= agnus.ptrMask;`
([`Memory.cpp` L1623](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L1623)),
the Copper's included
([`AgnusDma.cpp` L264](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusDma.cpp#L264)),
and sets `ptrMask = 0x07FFFF` for an OCS Agnus
([`Agnus.cpp` L190–L191](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L190-L191)).
On an OCS fetch address both give the 19 bits of pp.273 and 309, not NanoMig's
unconditional five high bits ([`NM-HRM-003`](../findings/NM-HRM-003.md)).

### NM-HRM-005 — WAIT and SKIP timing

vAmiga schedules the Copper as an event chain,
[`CopperEvents.cpp` L300–L303](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperEvents.cpp#L300-L303):
`schedule(next, delay = 2)` → `scheduleRel(DMA_CYCLES(delay), next)`, and
`DMA_CYCLES(n) = n << 3`. Two DMA cycles per state transition. Counting the
transitions from one `COP_FETCH` to the next:

| instruction | vAmiga path | DMA cycles | NanoMig measured | manual |
|---|---|---:|---:|---:|
| MOVE | FETCH → MOVE → FETCH | 4 | 4 | 4 |
| SKIP | FETCH → WAIT_OR_SKIP → SKIP1 → SKIP2 → FETCH | 8 | 8 | 4 |
| satisfied WAIT | FETCH → WAIT_OR_SKIP → WAIT1 → WAIT2 → (`delay == 0` → `DMA_CYCLES(2)`) → FETCH | 8 | 8 | 6 |

vAmiga and NanoMig agree on all three numbers.

Note what that does to the manual's own figures. The manual column is in memory
cycle times, the manual's own unit: p.20 gives MOVE and SKIP two memory cycles
each, *"four memory cycle times ... per instruction"*, and a WAIT *"six memory
cycle times"*, and p.275 gives the same factor of two. In that unit MOVE agrees
with both implementations, SKIP is exactly double, and a satisfied WAIT is 8
against 6. No single factor maps the manual's three figures onto the
implementations' three.

## NM-HRM-001 — PAL line count — WITHDRAWAL CONFIRMED

**The claim.** The test asserted a flat 312 positions and reported 313 as an
error.

### vAmiga

[`Core/Components/Agnus/Beam.h` L84–L86](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.h#L84-L86):

```cpp
isize vMaxPal() const { return lof ? 312 : 311; }
isize vCntPal() const { return lof ? 313 : 312; }
```

and the PAL constants at
[`Core/Infrastructure/Constants.h` L51–L55](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Constants.h#L51-L55):
`VPOS_CNT_LF = 313`, `VPOS_CNT_SF = 312`, default `VPOS_CNT = VPOS_CNT_LF`.

### WinUAE

[`include/custom.h` L119](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/custom.h#L119):
`#define MAXVPOS_PAL 312`, and
[`custom.cpp` L1422](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1422):

```c
return maxvpos + (lof_store ? 1 : 0);
```

and a hard reset clears the bit, `lof_store = lof_display = 0;`
([`custom.cpp` L6739](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L6739)).

Both make the count depend on the long-frame bit: 312 or 313, neither wrong.

### NanoMig

`agnus_beamcounter.v` states the same rule and behaves the same way:

```verilog
reg long_frame; // 1 : long frame (313 lines); 0 : normal frame (312 lines)
...
long_frame <= 1'b1;                                        // at reset
else if (end_of_frame && lace) long_frame <= ~long_frame;  // interlace only
```

It sets a long frame at reset and toggles only in interlace. vAmiga also starts
with the bit set; WinUAE clears it at a hard reset and runs 312-line fields
after one ([`NM-HRM-001`](../findings/NM-HRM-001.md)). The fixture ran
non-interlaced from reset, so 313 is the correct answer for what it drove.

### Result

The withdrawal stands. There is no finding here.
`hrm/ch3-beam-counters-and-blanking` grades the manual's 312 with the
long-frame bit cleared first, and `hrm/ch3-field-geometry` carries the 313-line
control.
