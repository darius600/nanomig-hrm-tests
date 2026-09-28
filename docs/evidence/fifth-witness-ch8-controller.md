# Four implementations, read against chapter 8's findings

Information for developers. **None of these is an oracle**, and none decides
anything — the manual does. [`third-witness.md`](third-witness.md) sets out the
rules for consulting an implementation at all; they apply unchanged here, and in
particular its first: an emulator is admitted only for a contradiction *inside*
the manual, and never quietly cancels a finding.

| | Source | Commit | Language |
|---|---|---|---|
| **WinUAE** | [tonioni/WinUAE](https://github.com/tonioni/WinUAE) | `ccc407e` | C/C++ |
| **vAmiga** | [dirkwhoffmann/vAmiga](https://github.com/dirkwhoffmann/vAmiga) | `35de004` | C++ |
| **Denise** | [piciji/denise](https://github.com/piciji/denise) | `1cb7d451` | C++ |
| **Copperline** | [CopperlineHQ/Copperline](https://github.com/CopperlineHQ/Copperline) | `726bed0` | Rust |

Every citation is a file and line at those commits. **Nothing was run**; these
are source readings and carry the assurance a source reading carries and no
more. Two readers worked independently, one on A/B/C and one on D, neither
having measured the packages they were reading against.

---

## A — the right-hand proportional input

**Question.** NanoMig gives `pot1x` no external term: `potcap[2] <= joy2_pin5`
where `joy2_pin5` is a pure function of the POTGO write register, while
`potcap[0] <= _mthird & joy1_pin5` conjoins the same driven value with a real
input. Do the others leave one counter dead?

| | POT0X | POT0Y | POT1X | POT1Y | asymmetric |
|---|---|---|---|---|---|
| WinUAE | yes | yes | **yes** | yes | no — one loop covers all four |
| vAmiga | button | button | **button** | button | no |
| Denise | button | button | **button** | button | no |
| Copperline | button + resistance | " | **" ** | " | no |
| **NanoMig** | `_mthird` | `_mright & _djoy1[5]` | **none** | `_djoy2[5]` | **yes** |

- **WinUAE** `cap_check` (`inputdevice.cpp:3867`) is a double loop over
  `joy 0..1` and `i 0..1` with no special case. Every external term is indexed
  by both — button state `:3878`, `joydirpot` `:3899`, `analog_port` `:3900`,
  `digital_port`/`mouse_port` `:3901`. `joy=1, i=0` is POT1X and takes exactly
  what the other three take.
- **vAmiga** increments all four symmetrically at `Agnus.cpp:877-880`, POT1X's
  term being `controlPort2.mouse.MMB()`.
- **Denise** has four structurally identical blocks in `paula/pot.cpp:116,124,
  132,140`, POT1X's at `:132` identical to its siblings.
- **Copperline** carries a four-element array and loops
  `for i in 0..self.pot_counters.len()` (`chipset/paula.rs:1305`) with no index
  singled out; index 2 is filled from `p2.pot_x_released()`
  (`bus/custom_regs.rs:174-184`).

**None of the four reproduces NanoMig's asymmetry.** The finding stands
untouched under rule 1 — the pages are clear and the RTL disagrees with them.
What the reading adds is that the asymmetry is **not a shared implementation
convention** anybody inherited.

**Two things found while reading that belong to the developers, not to the
finding.** vAmiga has an asymmetry of its own, from a different cause: in
`PaulaEvents.cpp:110-113` all four per-line charge deltas accumulate into
`chargeX0`, while `chargeY0`, `chargeX1` and `chargeY1` are guarded and never
charged. And `ControlPort::chargeDX`/`chargeDY` (`ControlPort.h:61-62`) are
never assigned anywhere in that repository, so its analogue term is constant on
all four lines. Denise's is likewise constant zero: `getPotX`/`getPotY` are
virtuals whose base returns 0 (`input/controlPort/controlPort.h:27-28`) and no
`libami` device overrides them.

**The pin-to-register mapping is visible in every source.** Which chip answers a
read is a separate question, and not out of reach for all four:
[`NM-HRM-030`](../findings/NM-HRM-030.md) records vAmiga, Denise and Copperline
modelling Paula apart from Agnus, and WinUAE, with one register dispatch for the
whole chip set, as the one that cannot answer it.

---

## B — the reset interval after a POTGO write

**Question.** p.235 says the hardware holds the pot counters reset for **seven
or eight complete scan lines**, and Figure 8-5 draws charging beginning eight
lines later. NanoMig holds them for **zero**.

| | reset interval before the count proper | counter held at zero through it | depends on video standard |
|---|---|---|---|
| WinUAE | **8** h-sync ticks | no — counts the interval's lines | no |
| vAmiga | **8** line-length events | no — counts the interval's lines | no |
| Denise | **8** PAL / **7** NTSC | **yes** | **yes** |
| Copperline | **8** PAL / **7** NTSC | **yes** | **yes** |
| **NanoMig** | **0** | — | — |

- **WinUAE** `#define POTDAT_DELAY 8` (`inputdevice.cpp:485`); while
  `pot_dat < POTDAT_DELAY` the charge is forced to `-2` (`:3905-3917`) and real
  counting starts on the ninth h-sync. `pot_dat` is the POTxDAT count itself: it
  counts the delay lines and is zeroed on the eighth h-sync, so a read inside the
  delay returns 1 to 8, not 0. No PAL/NTSC branch in the function.
- **vAmiga** `pokePOTGO` schedules `POT_DISCHARGE` with a payload of 8
  (`PaulaRegs.cpp:188`); `servicePotEvent` decrements it once per line
  (`PaulaEvents.cpp:73-97`). The interval is hard-wired to `PAL::HPOS_CNT`
  even in NTSC, so there is no 7/8 split. The end-of-line handler keeps
  incrementing the counters through the interval (`Agnus.cpp:877-880`), and the
  last event sets the input counters to -1 (`PaulaEvents.cpp:88-93`), so a read
  inside the interval is non-zero.
- **Denise** says it in one line: `pot.dischargeCounter = agnus.ntsc ? 7 : 8;`
  (`paula/pot.cpp:88`), and only when it is exhausted does counting begin
  (`:100-117`); until then the counters keep the zero the POTGO write gave them.
- **Copperline** holds while `pot_discharge_lines < discharge_lines`
  (`chipset/paula.rs:1298-1301`), forcing the counters to zero on each of those
  lines, the value coming from `VideoStandard::Pal => 8, Ntsc => 7`
  (`bus.rs:7930-7934`).

**All four delay the count proper by the reset interval; all four put PAL at
eight; only Denise and Copperline hold the counter at zero through it.** WinUAE
and vAmiga count the interval's lines in the same counter and restart it from
zero at its end, so they too read non-zero after one line, as NanoMig does
([`NM-HRM-038`](../findings/NM-HRM-038.md)). They split on NTSC, and that split
is the useful part: **the page's own "seven or eight" is an unresolved wobble,
and two of four independent implementations read it as a video-standard
distinction** with PAL landing on the 8 that Figure 8-5 draws. What separates
NanoMig from all four is the final count, which in NanoMig includes the
interval's lines; on the first read it stands with WinUAE and vAmiga, against
Denise, Copperline and the page.

---

## C — Table 8-2's wrapped rows

**Question.** p.231 says every row measures 100 pulses; the two wrapping rows
compute against **255**. An 8-bit counter has **256** states, so NanoMig takes
101 quadrature state changes to reach the printed target.

| | storage | modulus at JOYxDAT | anything against 255 |
|---|---|---|---|
| WinUAE | `uae_s16`, host wrap at 16384 | **256**, masked at read | no |
| vAmiga | `i64`, unbounded | **256**, masked at read | no |
| Denise | `int16_t` | **256**, masked at read | no |
| Copperline | `u8`, `wrapping_add` | **256**, native | no |

- **WinUAE** `getjoystate` truncates each axis to eight bits
  (`inputdevice.cpp:3717`); the only `255` in the file is an input-calibration
  clamp at `:5638`.
- **vAmiga** composes `mouseCounterY & 0xFC` with `& 0x03`
  (`ControlPort.cpp:109-121`) — eight bits per axis, nothing wider.
- **Denise** returns `(posY & 0xff) << 8 | (posX & 0xff)`
  (`input/controlPort/mouse.cpp:67`). Its many `255` constants are all in
  `paula/pot.cpp` and are the pot **capacitor** saturation sentinel, a different
  counter entirely.
- **Copperline** stores `counter_x: u8` and advances it with `wrapping_add`
  (`bus.rs:2922,3342`), which is mod 256 by construction. Its one computation
  against 255 is the analogue paddle scale (`chipset/paula.rs:295-309`) — the
  POT counter, not Table 8-2's, and a calibration choice rather than a wrap.

**Unanimous on the half they can answer.** Four independent implementations give
the mouse counter 256 states per axis and not one uses 255 as a modulus. This
is the question rule 1 admits — the conflict is between p.231's "100 pulses" and
Table 8-2's own subtraction — and the answer supports reading the printed
worked example's arithmetic as **the manual's slip**.

**The honest limit.** None of the four models the quadrature *pulse* the way the
finding frames it for a mouse: all four add a host delta straight to the
counter, one unit to one count. Only the digital-joystick encode
(WinUAE `inputdevice.cpp:3653-3666`, Copperline `bus.rs:3514-3532`) touches real
Gray-code phases. **They answer C's modulus half and are silent on its
pulse-counting half.**

---

## D — CIAADDRA's direction mask

**Question.** p.241 says CIAADDRA corresponds **one-to-one** with CIAAPRA and a
1 makes the corresponding bit an output. NanoMig honours that on two positions
of eight.

| | full 8-bit DDRA | PA1/PA0 | PA5–PA2 | PA7/PA6 |
|---|---|---|---|---|
| WinUAE | **no — four of eight** | reads back | **pin only, never stored** | reads back while the fire button is released; a pressed button reads 0 (wired-AND) |
| vAmiga | **in the mux, yes** | reads back | reads back | overridden by an attached device |
| Denise | **yes, all eight** | reads back | reads back | reads back |
| Copperline | **no** | reads back | **pin only, DDRA ignored** | driven out, never read back |
| **NanoMig** | **no — 2 of 8** | reads back | no output driver at all | driven out, never read back |

- **Denise** is the only one that honours the mask everywhere:
  `uint8_t ioa = lines.pra | ~lines.ddra;` (`cia/new/register.cpp:194`), under
  its own comment *"without external influence, lines show 'pra' in output mode
  and will be pulled up in input mode"*. Every external contribution passes
  through `~ddra` at `libami/system/system.cpp:54`.
- **Copperline** implements NanoMig's split **bit for bit**. Its generic core is
  full width (`chipset/cia.rs:992-995`), and `Bus::cia_a_read` then discards it:
  `v = (v & !0x3C) | self.floppy.cia_a_status_bits();` (`bus.rs:9010`), with no
  DDR term. **Demonstrably deliberate** — eleven lines later the *port B*
  overlay masks with `!port_b_ddr()` under the comment *"A pin the guest has
  switched to an output stays CIA-driven"*, and CIA-B applies `!ddr` to all
  three of its overlays.
- **WinUAE** builds PRA from three hand-written pieces
  (`cia.cpp:1896-1917`). `:1917` implements the manual's rule for PA1/PA0, and
  `handle_joystick_buttons(c->pra, c->dra)` on the line above reads an
  output-low PA7/PA6 as 0 (`inputdevice.cpp:4012-4015`), so a written value
  comes back on those two while the fire button is released; a pressed button
  reads 0 whatever was written (`inputdevice.cpp:4005-4006`). It follows the
  write on four positions. It will not store a written value elsewhere:
  `c->pra = (c->pra & ~0xc3) | (val & 0xc3);` (`cia.cpp:2128`) latches the same
  four bits NanoMig latches. CIA-B's PRA one screen down is the generic form
  (`cia.cpp:2030`).
- **vAmiga** muxes all eight (`CIA.cpp:863`) and then lets an attached
  control-port device override PA7/PA6 *after* the mux (`:866-867`), neither
  `Joystick::changePra` nor `Mouse::changePra` consulting `ddra`. A port has no
  device until the host sends it mouse or joystick input (`ControlPort.h:50`),
  and until then all eight read back; a mouse with its default pull-ups then
  forces a released fire bit to 1 even when it is an output driven 0
  (`Mouse.cpp:155-169`).

**Two of the four carry an explicit chip-variant switch for what an output
bit reads, and only one restores all eight.** vAmiga's `MOS_8520_PLCC`
(`CIA.cpp:869-871`, default off) restores strict one-to-one on all eight.
WinUAE's `391078-01` type (`cia.cpp:1923-1927`, enabled for the A600 alone,
default off) returns PRA contents for every output bit, but the write at
`cia.cpp:2128` never stores PA2–PA5, so an output-mode read of those four
returns their reset value, zero (`cia.cpp:2389`), whatever was written. vAmiga's switch mimics WinUAE's option by its own header
(`CIATypes.h:17-20`), and WinUAE's changelog describes the -01 as returning
the output data where other parts read the pin (`winuaechangelog.txt:4655`):
on a released line both return the written 0, so the part number decides
only a line held from outside, not the red.

One corroboration worth passing on: vAmiga logs *"DDRA: Setting unusual value"*
for CIA-A whenever the write is not `0x03` (`CIARegs.cpp:230-231`) — its own
code treats PA1/PA0-as-outputs as the only expected mask, the same two positions
NanoMig implements.

**The four split four ways and none cancels the finding**
([`NM-HRM-040`](../findings/NM-HRM-040.md)). `hrm/ch8-ciaa-port-direction`
releases all six external lines and is red at PA2–PA7, where the written 0
reads back 1 and p.341 predicts 0. Over a line held low, a case no page decides
and the package does not present, they split again: Denise and vAmiga return
the written 1 on PA2–PA5, WinUAE and Copperline the line.
