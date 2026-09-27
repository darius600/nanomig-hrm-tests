# NM-HRM-038 — after POTGO START, the pot counters are not held in reset and the input lines are not dumped [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `userio.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L136-L150) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L157-L171) |
| **Report to** | **Minimig-AGA and NanoMig.** All four implementations keep the reset interval out of the final count; this one counts through it |
| **Test** | [`hrm/ch8-pot-counter-start-and-charge`](../../hrm/ch8-pot-counter-start-and-charge) reads the counters; [`integration/ch8-controller-ports`](../../integration/ch8-controller-ports) reads POTGOR at the system boundary: `C0-Fc` to `C4-Fc`, claims `opus:8-dio.c21`, `sol:8-dio.c19` (p. 240), graded on their own in each of PAL+ECS, NTSC+ECS, PAL+OCS and NTSC+OCS on both CPU cores, red in all four on fx68k and on TG68K |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 4 red in the counter package — two from the body text, `opus:8-potchg.c1` and `sol:8-pot.c16`, two from Figure 8-5, `opus:8-fig8-5.c1` and `sol:8-fig8-5.c2`; 20 red per core at the system boundary, `C0-Fc` to `C4-Fc` in each of the four configurations (labelled with a `-NTSC+ECS`, `-PAL+OCS` or `-NTSC+OCS` suffix outside PAL+ECS: `C0-Fc`, `C1-Fc`, `C2-Fc`, `C3-Fc`, `C4-Fc`; `C0-Fc-NTSC+ECS`, `C1-Fc-NTSC+ECS`, `C2-Fc-NTSC+ECS`, `C3-Fc-NTSC+ECS`, `C4-Fc-NTSC+ECS`; `C0-Fc-PAL+OCS`, `C1-Fc-PAL+OCS`, `C2-Fc-PAL+OCS`, `C3-Fc-PAL+OCS`, `C4-Fc-PAL+OCS`; `C0-Fc-NTSC+OCS`, `C1-Fc-NTSC+OCS`, `C2-Fc-NTSC+OCS`, `C3-Fc-NTSC+OCS`, `C4-Fc-NTSC+OCS`), with the output-driven line (`C1-Fc-OUT` to `C4-Fc-OUT`) and the chip's own low drive (`PC2`) green in each on the same measurement basis; the `VGREEN` record turns `C3-Fc` green through the same comparator |
| **Verdict of the four** | **2–2 on the counter read, 3–1 on the POTGOR discharge, unanimous on the final count.** Denise and Copperline hold the counters at zero through the reset interval, so a read inside it returns 0, as the counter package's assertions require; WinUAE and vAmiga run the counters through the interval and restart them from zero at its end, so they too read non-zero after one line. On POTGOR, WinUAE, vAmiga and Denise discharge an input line through the interval, so a line nothing pulls up reads 0 there, as the integration package's assertions require; Copperline's POTGOR read has no discharge and returns 1 on a released line, as NanoMig's does. vAmiga's joystick, its mouse at the default setting, and Denise's joypad and mouse pull a released button's line high in every read, so with one attached those two read 1 as well. All four keep the interval out of the final count, which NanoMig includes, and all four put PAL at eight lines; on NTSC WinUAE and vAmiga use a flat 8, Denise and Copperline 7 |

---

## What the HRM says

**p.235**, body text: the read circuitry stays in a reset state for the first
**seven or eight** horizontal scan lines after a POTGO write, and the POT counter
is incremented only on the lines after that interval. The page is explicit that
this is the **hardware** doing it, not advice about what software should wait for.
With **p.236**'s "All counts are reset to zero when POTGO is written with bit
zero high", a counter read inside the interval holds zero.

**Figure 8-5**, p.235: the charging curves begin **eight lines** after POTGO.

The page gives two numbers for one quantity and never resolves them. That
wobble is part of the finding, and the implementations have something to say
about it.

## What NanoMig does

`userio.v` has **no reset-hold construct of any kind** — no counter, no state,
no comparison against a line count. A POTGO write with bit 0 set zeroes the four
counters, and the very next `pot_cnt_en` tick increments them.

### Measured

| | completed reset lines before the first increment |
|---|---|
| p.235 body | 7 or 8 |
| Figure 8-5 | 8 |
| **NanoMig** | **0** |

The first increment arrives on pulse 1, which is zero completed lines. The
counter package's control drives the same counter to a known count, 7, and routes it
through the identical comparator, which accepts it.

**At the system boundary, POTGOR shows the same missing interval.**
[`integration/ch8-controller-ports`](../../integration/ch8-controller-ports)
writes POTGO with START set in vertical blanking, as p.234 prescribes ("For a
standard X-Y joystick, this value is hex 0001"), in five pin configurations:
all four lines inputs ($0001), then each line in turn an output driven high
($0301, $0C01, $3001, $C001). It reads POTGOR six lines later, inside the reset
interval under either reading of p.235 and at least 300 µs after the write
(p.240's settling time, `PC4`). p.240 Table 8-4 says what START does, "Start
pots (dump capacitors, start counters)", the capacitors sit on the pot lines
(p.240; Figure 8-6, p.237), and POTGOR reads "the current state of the pot pins"
(p.240), so a dumped input line reads 0. NanoMig reads $5500 on bits 14, 12, 10
and 8 just before START (`PC3`) and $5500 in every counted read after it, which
the package prints in hex: every input line reads 1, as before START. The
package grades each of PAL+ECS, NTSC+ECS, PAL+OCS and NTSC+OCS on its own, and
`C0-Fc` to `C4-Fc` are red in all four on fx68k and on TG68K, 20 reds per core.
In every configuration the output-driven line reads 1 in the same reads
(`C1-Fc-OUT` to `C4-Fc-OUT`), lines the chip drives low read $0000 at the same
line of their frame (`PC2`), and `PC1` to `PC4` hold; all of these are green.

The same comparator does read 0 on a line that is at 0. The package's `VGREEN`
record grounds each configuration's reachable input lines at the ports from just
after START to the end of the read, in PAL+ECS: `C3-Fc`, whose three input
lines all have a port, turns green on both cores, and `C0-Fc`, `C1-Fc`, `C2-Fc`
and `C4-Fc` stay red on bit 12 alone ($1000, $1100, $1400, $5000), because no
port at this boundary reaches RX ([NM-HRM-037](NM-HRM-037.md)).

The grading and its hex record are
[`test_ch8_controller_ports.cpp:546-582`](../../integration/ch8-controller-ports/test_ch8_controller_ports.cpp#L546-L582),
the four configurations
[`:138-140`](../../integration/ch8-controller-ports/test_ch8_controller_ports.cpp#L138-L140)
and [`:643-661`](../../integration/ch8-controller-ports/test_ch8_controller_ports.cpp#L643-L661),
and the `VGREEN` record
[`:614-636`](../../integration/ch8-controller-ports/test_ch8_controller_ports.cpp#L614-L636).

The cause is the same absence on the register side. POTGOR's input bits are
the button levels, gated only by each line's output-enable and data bits
([`userio.v:160-178`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L160-L178),
read at [`:300-305`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L300-L305)),
and START (`potreg[0]`, [`:130-133`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/userio.v#L130-L133))
reaches only the counters.

## What the others say

| | | hold |
|---|---|---|
| **WinUAE** | [`inputdevice.cpp:485`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L485) | `#define POTDAT_DELAY 8`; while `pot_dat < POTDAT_DELAY` the charge is forced to `-2` ([`:3905-3917`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3905-L3917)). `pot_dat` is the POTxDAT count itself: it counts the delay lines, is zeroed on the eighth h-sync, and the count proper starts on the ninth, so a read inside the delay returns 1 to 8, not 0. No PAL/NTSC branch |
| **vAmiga** | [`PaulaRegs.cpp:188`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L188) | `pokePOTGO` schedules `POT_DISCHARGE` with a payload of **8**, decremented once per line. Hard-wired to `PAL::HPOS_CNT` even in NTSC. The end-of-line handler keeps incrementing the counters while the capacitors are discharged ([`Agnus.cpp:877-880`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L877-L880)); the last event sets input counters to -1 ([`PaulaEvents.cpp:88-93`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaEvents.cpp#L88-L93)), so the count proper starts after the eighth line and a read inside the interval is non-zero |
| **Denise** | [`libami/paula/pot.cpp:88`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L88) | `pot.dischargeCounter = agnus.ntsc ? 7 : 8;` — and only when it is exhausted does counting begin ([`:100-117`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L100-L117)); until then the counters keep the zero the POTGO write gave them |
| **Copperline** | [`bus.rs:7930-7934`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L7930-L7934) | `VideoStandard::Pal => 8, Ntsc => 7`, held in `tick_pot_hsync` while `pot_discharge_lines < discharge_lines`, with the counters forced to zero on each of those lines: `self.pot_counters = [0; 4];` ([`src/chipset/paula.rs:1298-1301`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1298-L1301)) |
| **MiSTer** | [`rtl/userio.v:151-172`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L151-L172) | descendant — byte-identical: `potcnt` is set to `4'b1111` on a POTGO write, but that flag only gates the analog-joystick match logic; the digital branch increments on the very next `pot_cnt_en` tick, same as NanoMig. |
| **TC64** | [`rtl/minimig/userio.v:189-201`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L189-L201) | descendant — the same construct with `anajoy` for `joy_ana_en` and no analog branch for port 1: `potcnt <= 4'b1111;` on POTGO write (`:191`), and the digital increments (`:194-195` unconditionally, `:200-201` in the non-analog branch) happen on the next `pot_cnt_en`. No reset-hold construct in either line. |

**All four delay the count proper by the reset interval; only two hold the
counter at zero through it.** Denise and Copperline leave POTxDAT at zero until
the interval ends, as p.235 and p.236 read together predict. WinUAE and vAmiga
count the interval's lines in the same counter and restart it from zero at its
end, so a read inside the interval is non-zero in both — after one line, as
in NanoMig. The counter package's four red assertions take the first non-zero read, so
they would be red on WinUAE and vAmiga as well; what separates NanoMig from all
four is the final count, which in NanoMig includes the interval's lines.

The page's unresolved "seven **or** eight" is read by two of four independent
implementations as a **video-standard distinction**, with PAL landing on exactly
the 8 that Figure 8-5 draws. The other two use a flat 8. None of the four
independent implementations uses zero — though both maintained Minimig lines
do: MiSTer and TC64 inherit NanoMig's absent hold unchanged, so as descendants
they say nothing about the manual and everything about where the fix has not
yet travelled.

**On POTGOR the four split differently.** The integration package reads
POTGOR's input bits six lines after START (*Measured*, above), where the dump
of p.240 predicts 0. WinUAE, vAmiga and Denise discharge an input line inside
the interval; Copperline, like NanoMig, reads it as it read before START.

| | | POTGOR input bit after START |
|---|---|---|
| **WinUAE** | [`inputdevice.cpp:3909-3913`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3909-L3913) | discharges. While the delay runs, an input line gets `charge = -2; /* fast discharge delay */`, applied only on the line strobe (`cancharge = hsync;`), which calls `cap_check(true)` once per line ([`:4375-4378`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L4375-L4378)); [`charge_cap`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3856-L3865) makes that −160 of a charge of at most 511, and POTGOR reads a bit as 1 only while `pot_cap[i][0] > 100` ([`:4105-4115`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L4105-L4115)). A read before the next strobe returns the level before START; after at most three strobes the bit reads 0, to the end of the delay. The pull-ups WinUAE emulates for mice and gamepads do not lift it: they set the charge before the discharge overwrites it ([`:3902-3903`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3902-L3903)), or act only while it is zero ([`:3959-3975`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3959-L3975)) |
| **vAmiga** | [`PaulaEvents.cpp:75-81`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaEvents.cpp#L75-L81) | discharges. `pokePOTGO` schedules the first `POT_DISCHARGE` for the end of the START line ([`PaulaRegs.cpp:188`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L188)); it and the six after it set an input line's charge to zero (`if (!outly) chargeY0 = 0.0;`), and POTGOR reads a bit as `chargeY1 >= 1.0` ([`PaulaRegs.cpp:148-151`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L148-L151)). A read in the START line returns the level before START; with nothing on the port the bit reads 0 from the next line on. The port's device then has the last word in every read ([`:156-157`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L156-L157)): a joystick with buttons 2 and 3 released sets both bits (`potgo \|= maskR;`, [`Joystick.cpp:180-190`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Joystick/Joystick.cpp#L180-L190)), and so does a mouse with its pull-up option on ([`Mouse.cpp:142-152`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Mouse/Mouse.cpp#L142-L152)), which is the default ([`Defaults.cpp:186`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Defaults.cpp#L186)). With either attached the bit reads 1 throughout, as in NanoMig |
| **Denise** | [`pot.cpp:103-111`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L103-L111) | discharges. After START ([`:88`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L88)) each line strobe zeroes an input line's capacitor while the discharge count runs (`if ((pot.go & POT_DIR_X0) == 0) pot.capX0 = 0;`), and POTGOR reads a bit as `pot.capX0 == 255` ([`:64-78`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L64-L78)). With nothing on the port a read before the next strobe returns the level before START, and the bit reads 0 from that strobe on. But `potGoR` first lets the port's device write the capacitor, and a joypad or a mouse writes `0xff` for a released button ([`joypad.cpp:39-46`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/controlPort/joypad.cpp#L39-L46), "with pullup resistor"; [`mouse.cpp:42-45`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/controlPort/mouse.cpp#L42-L45)), so with either attached the bit reads 1 throughout, as in NanoMig |
| **Copperline** | [`paula.rs:1217-1243`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1217-L1243) | does not discharge. `read_potgor` returns 0 only for a line driven low or grounded by its button (`if driven_low \|\| !released {`), and 1 otherwise (`v \|= mask;`); START ([`:1196-1204`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1196-L1204)) and the hold in `tick_pot_hsync` ([`:1298-1301`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1298-L1301)) change only `self.pot_counters`. A released input line reads 1 right after START and through the interval, as in NanoMig |
| **MiSTer** | [`rtl/userio.v:177-197`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L177-L197) | descendant — NanoMig's `potcap`, read at [`:319-324`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L319-L324); START (`potreg[0]`, [`:151`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L151)) reaches only the counters |
| **TC64** | [`rtl/minimig/userio.v:210-234`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L210-L234) | descendant — `potcap` from button and output-enable terms, with more button terms than NanoMig's and no START term either, read at [`:466-471`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L466-L471); START (`potreg[0]`, [`:189`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L189)) reaches only the counters |

The discharge is three of four in the chip. With a controller attached, vAmiga
and Denise read 1 as NanoMig does, because their joystick and mouse models
(vAmiga's mouse by default) pull a released button's line high in every read;
WinUAE still reads 0.

## Suggested change

Model the reset interval after a POTGO start once, for both halves the page
describes. For eight scan lines on PAL, hold the four pot counters at zero, as
Denise and Copperline do, and read POTGOR's bit as 0 for every pot line that is
an input, whose capacitor START dumps (p.240 Table 8-4) while the read circuitry
stays in its reset state (p.235), as WinUAE does, and vAmiga and Denise do with
nothing attached to the port. That satisfies the counter package's first-read
assertions, `C0-Fc` to `C4-Fc` in the controller-ports package, and the final
count all four agree on.
Holding the counters alone leaves `C0-Fc` to `C4-Fc` red: POTGOR's input bits
come from `potcap`, which START does not reach. Two of four implementations use
seven lines on NTSC and two use eight; that difference is a hardware question
this project cannot settle, and whoever implements it should decide it
deliberately rather than inheriting whichever number is convenient. Whether and
how the RTL changes is for an Amiga specialist to decide.

## What holds the finding up

- The page says **hardware**, not software, so this is not advice being read as
  a requirement.
- The figure independently draws the same interval.
- All four implementations keep the reset interval out of the final count, and
  all four agree on PAL. Two of them, Denise and Copperline, also hold the
  counter at zero through the interval, which is what the counter package's red
  assertions measure; WinUAE and vAmiga do not.
- The deviation is not an off-by-one against an unclear boundary: it is zero
  against seven or eight.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

