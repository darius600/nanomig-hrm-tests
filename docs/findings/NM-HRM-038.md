# NM-HRM-038 — after POTGO START, the pot counters count the reset interval into their final count and the input lines are not dumped [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should `userio.v` keep the reset interval after a POTGO write with bit 0 set (p.235: seven or eight lines; Figure 8-5: eight) out of the four pot counters' final count, and read POTGOR's input bits as 0 through it, where it now counts from the very next `pot_cnt_en` tick (final count 18 after 8 + 10 lines) and reads 1 on a released input line? Closes when `userio.v` models the interval and the 28 final-count and POTGOR reds turn green (four in `hrm/ch8-pot-counter-start-and-charge`, 24 on each core in `integration/ch8-controller-ports`), the four first-increment reds, whose expectation a photograph named A500 ECS contradicts (*What photographs of real machines show*), being decided apart, or when an Amiga specialist rejects the change, in which case, if the rejection means p.235 is wrong, this page is refiled against the printed page. |
| **Report to** | `NanoMig` — MiSTle-Dev/NanoMig |
| **Scope** | The pot counters and POTGOR's input bits after a POTGO write with bit 0 (START) set. Counters measured at `userio.v`; POTGOR measured at the system boundary in PAL+ECS, NTSC+ECS, PAL+OCS and NTSC+OCS on both CPU cores. Chip revision: unknown; whether NTSC takes seven lines or eight is open |
| **Software impact** | unknown; no named affected software; on NanoMig, after the suggested change: the POTGOR input bits come from the emulated mouse and joystick (`userio.v:174-176` for port 1, `:164-169` for port 2), so reading them as 0 for the eight lines after a START would show a released right or middle button as pressed |
| **Chapter** | 8 — Interface Hardware, printed pp. 234, 235 (body text and Figure 8-5), 236, 237 (Figure 8-6), 240 (Table 8-4); Appendix A — Register Summary, printed p. 282, as cited in `P1-START`'s message · *Amiga Hardware Reference Manual*, 3rd ed., 1991. Claim ids by assertion: `opus:8-potchg.c1` and `sol:8-pot.c16` (p.235 body text) are carried by the assertions of those names and by `FINALCOUNT-BODY-OPUS` and `FINALCOUNT-BODY-SOL`; `opus:8-fig8-5.c1` and `sol:8-fig8-5.c2` (Figure 8-5) by the assertions of those names and by `FINALCOUNT-FIG8-5-OPUS` and `FINALCOUNT-FIG8-5-SOL`; `opus:8-dio.c21` and `sol:8-dio.c19` (p. 240) by `P1-START` in each configuration. `C0-Fc` to `C4-Fc` and `C1-Fc-OUT` to `C4-Fc-OUT` carry no claim id |
| **Module** | `userio.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/userio.v#L136-L150) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L157-L171); inherited (from Minimig-AGA) |
| **Test** | [`hrm/ch8-pot-counter-start-and-charge`](../../hrm/ch8-pot-counter-start-and-charge): `make -C hrm/ch8-pot-counter-start-and-charge test`; [`integration/ch8-controller-ports`](../../integration/ch8-controller-ports): `make -k -C integration/ch8-controller-ports test`; an expected red exits nonzero |
| **Assertions** | 8 red in `hrm/ch8-pot-counter-start-and-charge` (76 passed, 13 failed; the other five are [NM-HRM-037](NM-HRM-037.md)'s): on the first increment `opus:8-potchg.c1`, `sol:8-pot.c16` (p.235 body text), `opus:8-fig8-5.c1` and `sol:8-fig8-5.c2` (Figure 8-5), expected 7 or 8 lines (Figure 8-5: 8), observed 0; on the final count after 8 + 10 lines `FINALCOUNT-BODY-OPUS`, `FINALCOUNT-BODY-SOL`, `FINALCOUNT-FIG8-5-OPUS` and `FINALCOUNT-FIG8-5-SOL`, expected 10 or 11 (Figure 8-5: 10), observed 18 on all four counters. 24 red per core in `integration/ch8-controller-ports` (109 passed, 24 failed on each core), expected every input line 0 in POTGOR after START, observed $5500 in every counted read: `C0-Fc` to `C4-Fc` in each of the four configurations (labelled with a `-NTSC+ECS`, `-PAL+OCS` or `-NTSC+OCS` suffix outside PAL+ECS: `C0-Fc`, `C1-Fc`, `C2-Fc`, `C3-Fc`, `C4-Fc`; `C0-Fc-NTSC+ECS`, `C1-Fc-NTSC+ECS`, `C2-Fc-NTSC+ECS`, `C3-Fc-NTSC+ECS`, `C4-Fc-NTSC+ECS`; `C0-Fc-PAL+OCS`, `C1-Fc-PAL+OCS`, `C2-Fc-PAL+OCS`, `C3-Fc-PAL+OCS`, `C4-Fc-PAL+OCS`; `C0-Fc-NTSC+OCS`, `C1-Fc-NTSC+OCS`, `C2-Fc-NTSC+OCS`, `C3-Fc-NTSC+OCS`, `C4-Fc-NTSC+OCS`) and each configuration's aggregate over them (`P1-START`, `P1-START-NTSC+ECS`, `P1-START-PAL+OCS`, `P1-START-NTSC+OCS`), on fx68k and on TG68K. Green in each configuration on the same measurement basis: the output-driven line (`C1-Fc-OUT` to `C4-Fc-OUT`) and the chip's own low drive (`PC2`) |
| **Verdict of the four** | **2–2 on the counter read, 3–1 on the POTGOR discharge, unanimous on the final count.** Denise and Copperline hold the counters at zero through the reset interval, so a read inside it returns 0, as the counter package's first-increment assertions require; WinUAE and vAmiga run the counters through the interval and restart them from zero at its end, so they too read non-zero after one line. On POTGOR, WinUAE, vAmiga and Denise discharge an input line through the interval, so a line nothing pulls up reads 0 there, as the integration package's assertions require; Copperline's POTGOR read has no discharge and returns 1 on a released line, as NanoMig's does. vAmiga's joystick, its mouse at the default setting, and Denise's joypad and mouse pull a released button's line high in every read, so with one attached those two read 1 as well. All four keep the interval out of the final count, which NanoMig includes, and all four put PAL at eight lines; on NTSC WinUAE and vAmiga use a flat 8, Denise and Copperline 7. Hardware: not run here; photographs named A500 ECS show the final count with the page and against NanoMig's 18, a read inside the interval not zero, which is against the hold at zero, and a POTGOR read of $0000 only if that exposure is `potgo1`'s (*What photographs of real machines show*) |
| **Effort** | M — `userio.v` only — the reset interval on the four pot counters (eight lines on PAL), a hold at zero or, as the photograph reads, a count through the interval and a restart at its end, and a START term in `potcap`, which START does not reach today |

---

## Source requirement

**p.235**, body text:

> "The read circuitry stays in a reset state for the first seven or eight horizontal video scan lines."

The POT counter is incremented only on the lines after that interval. The page is
explicit that this is the **hardware** doing it, not advice about what software
should wait for.
**p.236**:

> "All counts are reset to zero when POTGO is written with bit zero high."

With it, a counter read inside the interval holds zero. **Figure 8-5**, p.235:
the charging curves begin **eight lines** after POTGO.

For the POTGOR half: **p.234** prescribes the POTGO value:

> "For a standard X-Y joystick, this value is hex 0001."

**p.240**, Table 8-4, says what START does:

> "Start pots (dump capacitors, start counters)"

The capacitors sit on the pot lines (p.240; Figure 8-6, p.237), and POTGOR reads
the pins (p.240):

> "To use the entire register as an input, sensing the current state of the pot pins, write all 0s to POTGO."

A dumped input line therefore reads 0.

## RTL and measurement

`userio.v` has **no reset-hold construct of any kind** — no counter, no state, no
comparison against a line count. A POTGO write with bit 0 set zeroes the four
counters, and the very next `pot_cnt_en` tick increments them.
[`hrm/ch8-pot-counter-start-and-charge`](../../hrm/ch8-pot-counter-start-and-charge)
releases pot0x at the POTGO start and counts the lines before its first
increment (`test_userio.cpp:834-889`), and runs all four counters 8 + 10 lines
unbroken (`:913-1026`):

| | lines before the first increment | final count after 8 + 10 lines |
|---|---|---|
| p.235 body | 7 or 8 | 10 or 11 |
| Figure 8-5 | 8 | 10 |
| **NanoMig** | **0** | **18**, all four counters |

The package grades every other clause after a warm-up longer than either interval, so only these eight depend on when counting starts.

**At the system boundary, POTGOR shows the same missing interval.**
[`integration/ch8-controller-ports`](../../integration/ch8-controller-ports)
writes POTGO with START set in vertical blanking in five pin configurations: all
four lines inputs ($0001), then each line in turn an output driven high ($0301,
$0C01, $3001, $C001). It reads POTGOR six lines later, inside the reset interval
under either reading of p.235 and at least 300 µs after the write (p.240's
settling time, `PC4`). NanoMig reads $5500 on bits 14, 12, 10 and 8 just before
START (`PC3`) and $5500 in every counted read after it: every input line reads 1,
as before START. The output-driven line reads 1 in the same reads, lines the chip
drives low read $0000 at the same line of their frame (`PC2`), and `PC1` to `PC4`
hold.

A patched copy of the RTL (`start_model.patch` in the package; see [`integration/README.md`](../../integration/README.md)) does read 0 there. `make sm-test-fx68k` and
`make sm-test-tg68k` build it, not run by `make test`, in their own object directories, from the
package's `start_model.patch`: for eight lines after a POTGO write with START,
POTGOR reads the input pot lines as 0 (Figure 8-5's reading). On it the same
unchanged test prints 133 passed, 0 failed on each core, so `C0-Fc` to `C4-Fc`
and `P1-START` pass in all four configurations. In
`integration/ch8-controller-ports/test_ch8_controller_ports.cpp`, the four
configurations are `kConfigs` at `:358-360` and `grade_all` at `:874-892` runs
and grades each; one configuration's grading is `:732-870`, whose red `expect`
calls are `C0-Fc` to `C4-Fc` at `:831` and `P1-START` at `:864`.

The cause is the same absence on the register side: POTGOR's input bits are the
button levels, gated only by each line's output-enable and data bits
([`userio.v:160-178`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/userio.v#L160-L178),
read at [`:300-305`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/userio.v#L300-L305)),
and START (`potreg[0]`, [`:130-133`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/userio.v#L130-L133))
reaches only the counters.

## Risks and unknowns

- p.235 gives two numbers for one quantity, "seven or eight" in the body text and
  eight in Figure 8-5, and never resolves them. Whether NTSC takes seven lines or
  eight is a hardware question this project cannot settle; real hardware is not
  measured for NTSC, and photographs named A500 ECS do not say which standard.
- The counter half splits 2–2: WinUAE and vAmiga count the interval's lines in the
  same counter, so a read inside it is non-zero there and the four
  first-increment reds would be red on them too. The four final-count reds
  separate NanoMig from all four. A photograph named A500 ECS reads non-zero
  inside the interval, `$0404` at line 4, so the four first-increment reds
  expect what the photograph does not show (*What photographs of real machines show*).
- The POTGOR half splits 3–1, and a device on the port can lift the line: vAmiga's
  joystick, its mouse at the default setting, and Denise's joypad and mouse pull a
  released button's line high in every read, so with one attached those two read 1
  as well. On NanoMig the POTGOR input bits always come from the emulated mouse and
  joystick (`userio.v:174-176` for port 1, `:164-169` for port 2), the case in which vAmiga and Denise read 1.

## Independent evidence

| | counters through the interval | POTGOR input bit after START, six lines on |
|---|---|---|
| **WinUAE** | [`inputdevice.cpp:485`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L485): `#define POTDAT_DELAY 8`; while `pot_dat < POTDAT_DELAY` the charge is forced to `-2` ([`:3905-3917`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3905-L3917)). `pot_dat` is the POTxDAT count itself: it counts the delay lines, is zeroed on the eighth h-sync, and the count proper starts on the ninth, so a read inside the delay returns 1 to 8, not 0. No PAL/NTSC branch | discharges. An input line gets `charge = -2; /* fast discharge delay */` on the line strobe (`cancharge = hsync;`, [`:3909-3913`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3909-L3913)), which calls `cap_check(true)` once per line ([`:4375-4378`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L4375-L4378)); [`charge_cap`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3856-L3865) makes that −160 of a charge of at most 511, and POTGOR reads a bit as 1 only while `pot_cap[i][0] > 100` ([`:4105-4115`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L4105-L4115)). A read before the next strobe returns the level before START; after at most three strobes the bit reads 0, to the end of the delay. The pull-ups it emulates for mice and gamepads do not lift it: they set the charge before the discharge overwrites it ([`:3902-3903`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3902-L3903)), or act only while it is zero ([`:3959-3975`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L3959-L3975)) |
| **vAmiga** | [`PaulaRegs.cpp:188`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L188): `pokePOTGO` schedules `POT_DISCHARGE` with a payload of **8**, decremented once per line; hard-wired to `PAL::HPOS_CNT` even in NTSC. The end-of-line handler keeps incrementing the counters while the capacitors are discharged ([`Agnus.cpp:877-880`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.cpp#L877-L880)); the last event sets input counters to -1 ([`PaulaEvents.cpp:88-93`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaEvents.cpp#L88-L93)), so the count proper starts after the eighth line and a read inside the interval is non-zero | discharges. The first `POT_DISCHARGE` is scheduled for the end of the START line; it and the six after it set an input line's charge to zero (`if (!outly) chargeY0 = 0.0;`, [`PaulaEvents.cpp:75-81`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaEvents.cpp#L75-L81)), and POTGOR reads a bit as `chargeY1 >= 1.0` ([`PaulaRegs.cpp:148-151`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L148-L151)). A read in the START line returns the level before START; with nothing on the port the bit reads 0 from the next line on. The port's device has the last word ([`:156-157`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L156-L157)): a joystick with buttons 2 and 3 released sets both bits (`potgo \|= maskR;`, [`Joystick.cpp:180-190`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Joystick/Joystick.cpp#L180-L190)), and so does a mouse with its pull-up option on ([`Mouse.cpp:142-152`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Mouse/Mouse.cpp#L142-L152)), which is the default ([`Defaults.cpp:186`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Defaults.cpp#L186)); with either attached the bit reads 1 throughout |
| **Denise** | [`libami/paula/pot.cpp:88`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L88): `pot.dischargeCounter = agnus.ntsc ? 7 : 8;`, and only when it is exhausted does counting begin ([`:100-117`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L100-L117)); until then the counters keep the zero the POTGO write gave them | discharges. Each line strobe zeroes an input line's capacitor while the discharge count runs (`if ((pot.go & POT_DIR_X0) == 0) pot.capX0 = 0;`, [`:103-111`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L103-L111)), and POTGOR reads a bit as `pot.capX0 == 255` ([`:64-78`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/pot.cpp#L64-L78)). With nothing on the port a read before the next strobe returns the level before START, and the bit reads 0 from that strobe on. But `potGoR` first lets the port's device write the capacitor, and a joypad or a mouse writes `0xff` for a released button ([`joypad.cpp:39-46`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/controlPort/joypad.cpp#L39-L46), "with pullup resistor"; [`mouse.cpp:42-45`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/controlPort/mouse.cpp#L42-L45)), so with either attached the bit reads 1 throughout |
| **Copperline** | [`bus.rs:7930-7934`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus.rs#L7930-L7934): `VideoStandard::Pal => 8, Ntsc => 7`, held in `tick_pot_hsync` while `pot_discharge_lines < discharge_lines`, with the counters forced to zero on each of those lines: `self.pot_counters = [0; 4];` ([`src/chipset/paula.rs:1298-1301`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1298-L1301)) | does not discharge. `read_potgor` returns 0 only for a line driven low or grounded by its button (`if driven_low \|\| !released {`), and 1 otherwise (`v \|= mask;`, [`paula.rs:1217-1243`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1217-L1243)); START ([`:1196-1204`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1196-L1204)) changes only `self.pot_counters`. A released input line reads 1 right after START and through the interval, as in NanoMig |
| **MiSTer** | [`rtl/userio.v:151-172`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L151-L172): descendant, byte-identical; `potcnt` is set to `4'b1111` on a POTGO write, but that flag only gates the analog-joystick match logic, and the digital branch increments on the very next `pot_cnt_en` tick, as in NanoMig | descendant. NanoMig's `potcap` ([`:177-197`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L177-L197), read at [`:319-324`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L319-L324)); START (`potreg[0]`, [`:151`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/userio.v#L151)) reaches only the counters |
| **TC64** | [`rtl/minimig/userio.v:189-201`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L189-L201): descendant; the same construct with `anajoy` for `joy_ana_en` and no analog branch for port 1: `potcnt <= 4'b1111;` on POTGO write (`:191`), and the digital increments (`:194-195` unconditionally, `:200-201` in the non-analog branch) happen on the next `pot_cnt_en` | descendant. `potcap` from button and output-enable terms, with more button terms than NanoMig's and no START term either ([`:210-234`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L210-L234), read at [`:466-471`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L466-L471)); START (`potreg[0]`, [`:189`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/userio.v#L189)) reaches only the counters |

### What photographs of real machines show

vAmigaTS, vAmiga's test suite, publishes a screen photograph per program. The photographs used here are named A500 ECS, a label and nothing more, and what is attached to the ports is not known. The programs write POTGO bit 0 and draw the value read as bars:

| Program | What it does | What the photograph shows |
|---|---|---|
| [`pot0dat3`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat3/pot0dat3.s) ([photo](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat3/pot0dat3_A500_ECS.JPG)) | POTGO = 1 in the vertical-blank handler ([`pot0dat3.s:56`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat3/pot0dat3.s#L56)), POT0DAT read at line 4 ([`pot0dat3.s:177`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat3/pot0dat3.s#L177)) | `$0404` |
| [`pot0dat4`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat4/pot0dat4.s) ([photo](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat4/pot0dat4_A500_ECS.JPG)) | read at line `$C0`, 192 ([`pot0dat4.s:278`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat4/pot0dat4.s#L278)) | `$B8B8`, 184, which is 192 - 8 |
| [`pot0dat6`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat6/pot0dat6.s) ([photo](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat6/pot0dat6_A500_ECS.JPG)) | the wait for line `$37` ([`pot0dat6.s:278`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/pot0dat6/pot0dat6.s#L278)) follows one for `$B0` and is already passed, so the read falls at line `$B0` to `$B1` | `$A9A9`, 169, which is 177 - 8 |
| [`cmp0hi`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/cmp0hi/cmp0hi.s), [`cmp1hi`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/cmp1hi/cmp1hi.s) ([photo](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/cmp0hi/cmp0hi_A500_ECSJPG.JPG), [photo](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/cmp1hi/cmp1hi_A500_ECS.JPG)) | POTGO = 1 each frame ([`cmp0hi.s:48`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/cmp0hi/cmp0hi.s#L48)); the high byte of POT0DAT ([`cmp0hi.s:35`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/cmp0hi/cmp0hi.s#L35)) or POT1DAT against the beam's line minus 8, green if equal, red if not | no red pixel; 74,942 and 72,559 green samples from x 88, a dark comb to the left of it with no red |
| [`potgo1`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/potgo1/potgo1.s), [`potgo5`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/potgo5/potgo5.s) ([photo](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/potgo1/potgo1_A500_ECS.JPG), [photo](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/potgo5/potgo5_A500_ECS.JPG)) | POTGO = 1, then POTINP read ([`potgo1.s:42`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/potgo1/potgo1.s#L42), [`potgo1.s:45`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Mainboard/Ports/POT/potgo1/potgo1.s#L45)); `potgo5` writes POTGO = 0 | sixteen equal bars, `$0000`; `$0500` |

The final count stands with the page: a read at line L returns L - 8, the eight lines of the interval are out of it, where NanoMig's count is 18 after 8 + 10 lines. The first increment does not stand with the page: a read inside the interval is not zero, `$0404` at line 4, so the counter runs inside the interval, as NanoMig's, WinUAE's and vAmiga's do, and is not held at zero, as Denise's and Copperline's are; it restarts near line 8, which NanoMig's does not.

POTGOR: `potgo1` and `potgo5` differ in START, `$0000` against `$0500`, but the file of `potgo1` is byte-identical to those of `potgo2`, `potgo3` and `potgo7`, and `potgo7` has no START; that of `potgo5` to `potgo0`'s and `potgo6`'s. If the exposure is `potgo1`'s, a released input line reads 0 after START, with the page and against NanoMig. The limits: the lines 5 to `$2B` are not seen; the devices on the ports are unknown; `potgo5` reads bits 14 and 12 clear without START, so `$5500` is not this machine's released state; NTSC is open. The bars are read from the photographs, with brightness and position measured; the programs were not run on a machine here.

## Decision

Model the reset interval after a POTGO start once, for both halves the page
describes. For eight scan lines on PAL, keep the interval out of the final count: hold the
four pot counters at zero, as Denise and Copperline do, or, as the photograph
has it and WinUAE and vAmiga do, let them run and restart them at the end of
the interval; and read POTGOR's bit as 0 for every pot line that is
an input, whose capacitor START dumps (p.240 Table 8-4) while the read circuitry
stays in its reset state (p.235), as WinUAE does, and vAmiga and Denise do with
nothing attached to the port; on NanoMig the bits always come from the emulated mouse
and joystick (`userio.v:174-176` for port 1, `:164-169` for port 2), the case in which vAmiga and Denise read 1. By a reading of the code, the hold satisfies the
counter package's eight reds, the restart the four final-count reds only; and on the
POTGOR side `C0-Fc` to `C4-Fc` in the controller-ports package, bit 12 included,
since the dump does not depend on a port; holding the counters alone leaves the
latter red, because START does not reach `potcap`.

A photograph named A500 ECS contradicts the hold at zero for a read inside the interval: it reads `$0404` at line 4, so `opus:8-potchg.c1`, `sol:8-pot.c16`, `opus:8-fig8-5.c1` and `sol:8-fig8-5.c2`, the four first-increment assertions, expect 7 or 8 lines before the first increment where the photograph shows a count already running, and the final-count assertions are not touched (*What photographs of real machines show*).
