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

## NM-HRM-002 — audio stereo routing — FINDING RESTORED

**The conflict.** Chapter 5, p.137: *"Channels 1 and 2 are connected to the
left-side stereo output jack. Channels 0 and 3 are connected to the right-side
output jack."* The appendix pin list says the opposite: *"AUDO — Audio output
from left (channels 0, 3) port"*, *"AUDI — Audio input to right (channels 1, 2)
port"*. Both appear in the printed and the Developer-CD edition.

### WinUAE

[`audio.cpp` L1093–L1104](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1093-L1104)
— the same assignment repeats at
[L1133–L1144](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1133-L1144)
in a second output path:

```c
put_sound_word_right(data0);
put_sound_word_left(data1);
...
put_sound_word_right2(data3);
put_sound_word_left2(data2);
```

Channels 0 and 3 to the right, 1 and 2 to the left.

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

Same assignment as WinUAE.

### Result

Two independent implementations agree with chapter 5. The appendix pin list is
the outlier. `paula_audio_mixer.v` follows the pin list:

```verilog
ldatasum <= msample0 + msample3;
rdatasum <= msample1 + msample2;
```

The finding stands and the four assertions in `hrm/paula_audio_mixer` were
restored. **The NanoMig stereo channels are swapped.**

This corrects a withdrawal made earlier the same day. The withdrawal was reasoned
from the manual alone — the manual said both things, so nothing could be asserted
— and it was wrong to stop there rather than fetch a witness.

## NM-HRM-003, NM-HRM-004, NM-HRM-005 — read out of interest, and the result is uncomfortable

These three are not contested: the manual is clear and the RTL disagrees with it,
so by the rule above an emulator cannot cancel them. They were read anyway, to see
*why* the RTL deviates. **On all three, both emulators do what NanoMig does and
not what the manual says.**

That does not close the findings. It moves them: the question is no longer
"is the RTL wrong" but "is the manual wrong about the hardware", and that is a
decision, not a measurement.

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

The sentence the finding rests on is Appendix C, p.310:

> "Now, in the ECS, if this bit is set, the Copper can access all of the Amiga
> chip registers. **If this bit is clear**, the Copper can access the address
> range from $DFF03E through $DFF07E, the same range as when the danger bit is
> set in the standard chip set."

Read literally, clearing the danger bit grants the range that setting it grants —
which would make the bit do nothing. Neither emulator implements that.

### NM-HRM-003 — the location register width

Neither emulator narrows the Copper pointer by chipset.

[vAmiga `Copper.cpp` L29–L31](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/Copper.cpp#L29-L31):
`setPC(u32 addr) { coppc = addr; }` — no mask. The only `isOCS()` anywhere in
vAmiga's Copper directory is the CDANG check above.

[WinUAE `custom.cpp` L2888–L2890](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L2888-L2890):
`cop1lc = (cop1lc & 0xffff) | ((uae_u32)v << 16);` — the whole written word goes
in.

Both bound the effective address later, by chip RAM size, which is a different
mechanism from the manual's "OCS has three high bits, ECS five".

### NM-HRM-005 — WAIT and SKIP timing

vAmiga schedules the Copper as an event chain,
[`CopperEvents.cpp` L300–L303](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Copper/CopperEvents.cpp#L300-L303):
`schedule(next, delay = 2)` → `scheduleRel(DMA_CYCLES(delay), next)`, and
`DMA_CYCLES(n) = n << 3`. Two DMA cycles per state transition. Counting the
transitions from one `COP_FETCH` to the next:

| instruction | vAmiga path | DMA cycles | NanoMig measured | manual |
|---|---|---:|---:|---:|
| MOVE | FETCH → MOVE → FETCH | 4 | 4 | 2 |
| SKIP | FETCH → WAIT_OR_SKIP → SKIP1 → SKIP2 → FETCH | 8 | 8 | 4 |
| satisfied WAIT | FETCH → WAIT_OR_SKIP → WAIT1 → WAIT2 → (`delay == 0` → `DMA_CYCLES(2)`) → FETCH | 8 | 8 | 6 |

vAmiga and NanoMig agree on all three numbers.

Note what that does to the manual's own figures. MOVE and SKIP differ from the
implementations by exactly a factor of two — the unit question tracked as
FPGA-064. The satisfied WAIT does not: 6 × 2 = 12, and both implementations give
8. So the manual's WAIT figure does not fit its own MOVE and SKIP figures under
any single unit convention, whichever one is chosen.

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

Both make the count depend on the long-frame bit: 312 or 313, neither wrong.

### NanoMig

`agnus_beamcounter.v` states the same rule and behaves the same way:

```verilog
reg long_frame; // 1 : long frame (313 lines); 0 : normal frame (312 lines)
...
long_frame <= 1'b1;                                        // at reset
else if (end_of_frame && lace) long_frame <= ~long_frame;  // interlace only
```

It sets a long frame at reset and toggles only in interlace — which is what both
emulators do. The fixture ran non-interlaced from reset, so 313 is the correct
answer for what it drove.

### Result

The withdrawal stands. There is no finding here, and the test now reports the
vertical count without asserting it.
