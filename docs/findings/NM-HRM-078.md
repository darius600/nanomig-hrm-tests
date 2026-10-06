# NM-HRM-078 — CRA bit 5 and CRB bit 5 are stored and never read: a timer set to count CNT counts φ2 [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option, the one MiSTer's line has already taken; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `cia_timera.v`, `cia_timerb.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L32) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timera.v#L50) |
| **Report to** | **NanoMig and minimig-dev/MinimigAGA-MiST-TC64.** MiSTer already selects each timer's count source from INMODE, so a fix exists to port — see *Where a fix already exists*. There is no automatic merge in either direction |
| **Test** | [`hrm/appF-cia-timer-modes`](../../hrm/appF-cia-timer-modes) — two assertions, each on one timer module alone, with no CPU core: `CRA-INMODE`, claims `sol:F-cra.c8`, `opus:F-cra.c8` (p. 348), on the `cia_timera` fixture (`test_cia_timera.cpp:475-515`); `CRB-INMODE-ARITY-00-01`, claim `opus:F-crb.c6` (p. 349), on the `cia_timerb` fixture (`test_cia_timerb.cpp:476-518`). The same claim's codes `10` and `11` are `CRB-INMODE-ARITY-10-11`, a RECORDED line, declined (`test_cia_timerb.cpp:539-571`): the fixture has no CNT pin, and with CNT held high p. 349 itself predicts that modes 10 and 11 read alike. [`integration/appF-cia-system`](../../integration/appF-cia-system) grades the zero-edge half of the same bits at the system boundary, on fx68k and TG68K: `P7.5` (CRA bit 5, pp. 348, 351; CIA-B's timer A), `P7.5-CIAA-INMODE` (CRA bit 5; CIA-A's timer A over an interval with no CNT edge) and `P8.65-MODE01` (CRB mode 01, pp. 349, 351; CIA-B's timer B), which carry no claim id, are red on both cores. The counting half ("counts positive CNT transitions", pp. 348, 349) is not graded: each of the three prints a RECORDED line for it, `P7.5-COUNTING`, `P7.5-CIAA-INMODE-COUNTING` and `P8.65-MODE01-COUNTING`, because it needs a CNT source whose edges are known to reach the timer, and nothing in the package observes KCLK at CIA-A's CNT input. For the same reason the decrement CIA-A's timers make while a key code is sent is a MEASURED line, not graded, and so is `P8.65-MODE11-PAIRK` (CRB mode 11, pp. 349, 357); only mode 11's first half, with no key sent, is graded, and it is green. The claim ids sit on the package's P10 rows, which join these lines and measure nothing of their own |
| **Chapter** | Appendix F — 8520 Complex Interface Adapters, printed pp. 343, 348–351, 353; 8 — Interface Hardware, printed p. 251; Appendix G — Keyboard Interface, printed p. 357 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red: `CRA-INMODE` (`cia_timera`: 9 passed, 1 failed) and `CRB-INMODE-ARITY-00-01` (`cia_timerb`: 7 passed, 1 failed); `CRB-INMODE-ARITY-10-11` is a RECORDED line, not graded. In the same run `CRB-INMODE-CODE-00` and `CRB-INMODE-CODE-10`, green, show CRB bit 6 choosing between E-clock pulses and timer A's underflows on the same fixture. `integration/appF-cia-system`: 97 passed, 26 failed on each core, the same lines on both; 3 red lines are in this finding's scope, each the zero-edge half of INMODE = CNT: `P7.5` and `P8.65-MODE01` through their CIA-B legs, and `P7.5-CIAA-INMODE` through CIA-A's timer A over an interval with no CNT edge. The counting half is three RECORDED lines (`P7.5-COUNTING`, `P7.5-CIAA-INMODE-COUNTING`, `P8.65-MODE01-COUNTING`), not graded. Two P10 rows are red through these lines, `sol:F-cra.c13` (CRA INMODE) and `sol:F-crb.c7, sol:F-crb.c10` (CRB INMODE; its mode-11 member is not graded), and two more, `opus:F-cra.c13` and `opus:F-crb.c14`, through them and through lines of [NM-HRM-074](NM-HRM-074.md) and, for CRA, `P7.6-CIAB` ([NM-HRM-086](NM-HRM-086.md)); the other reds are those findings'. In the same run `P5-LEGA` (φ2 counted, one per E period), `P7.5-CONTRAST` and `P8.65-MODE01-CONTRAST` (CR = $01 counts the E cycles of the window on both CIAs), `P8.65-MODE10`, `P8.65-MODE11-PAIRN`, the window preconditions (`P7.5-WINDOW`, `P7.5-WINDOW-CIAB`, `P7.5-CIAA-INMODE-PRE`, `P8.65-MODE01-WINDOW`, `P8.65-MODE01-WINDOW-CIAB`, `P8.65-MODE11-WINDOW`, `P8.65-MODE11-WINPRE`) and `P7.6-CIAA` are green; `KP-A` and `KP-B` are recorded, not graded |
| **Verdict of the four** | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline all stop a timer's E-clock count once its INMODE selects CNT, and count CNT's rising edges instead. vAmiga, Denise and Copperline drive CIA-A's CNT from the keyboard's KCLK and CIA-B's from PA1, and count CRB mode 11 only while CNT is high; WinUAE drives CIA-B's alone, from PA1, and counts mode 11 as mode 10 |

---

## What the HRM says

**Printed p. 343**, under *INPUT MODES*:

> Control bits allow selection of the clock used to decrement the timer. Timer A can count 02
> clock pulses or external pulses applied to the CNT pin. Timer B can count 02 pulses, external
> CNT pulses, timer A underflow pulses, or timer A underflow pulses while the CNT pin is held
> high.

"02" is the φ2 clock input, which **printed p. 353** ties to the E clock:

> The 02 clock is a TTL compatible input used for internal device operation and as a timing
> reference for communicating with the system data bus. On the Amiga, this is connected to the
> 680x0 "E" clock.

**Printed p. 348**, Control Register A, bit 5:

> 5 INMODE 1 = Timer A counts positive CNT transitions,
> 0 = Timer A counts 02 pulses.

**Printed p. 349**, Control Register B, bits 6 and 5:

> 6,5 INMODE Bits CRB6 and CRB5 select one of four possible
> input modes for Timer B, as follows:

and the table under it, as the page prints it:

> CRB6 CRB5 Mode Selected
> 0 0 Timer B counts 02 pulses
> 0 1 Timer B counts positive CNT transitions
> 1 0 Timer B counts Timer A underflow pulses
> 1 1 Timer B counts Timer A underflow pulses
> while CNT pin is held high.

The two bitmaps repeat it, *read from the printed image, not quoted*: CRA's
INMODE column on p. 349 reads `0=02` over `1=CNT`, and CRB's on p. 350 reads
`00=02`, `01=CNT`, `10=Timer A` and `11=CNT+ Timer A`.

**What CNT is on the Amiga.** p. 351's port signal assignments, *read from the
printed image*, give CIA-A `CNT..KCLK` under `SP...KDAT keyboard data`, and
CIA-B `SP...BUSY` and `CNT..POUT`, each joined by a bracket labelled
*commodore* to the port-A line of the same name, `PA0..BUSY` and `PA1..POUT`.
**Printed p. 251** says the same of the keyboard:

> The keyboard is interfaced to the system via the serial shift register on one of the 8520 CIA chips.
> The keyboard data line is connected to the SP pin, the keyboard clock is connected to the CNT
> pin.

and **printed p. 357** says what that pin does:

> Before the transmission starts, both KCLK and KDAT are high. The keyboard starts the transmission
> by putting out the first data bit (on KDAT), followed by a pulse on KCLK (low then high); then it
> puts out the second data bit and pulses KCLK until all eight data bits have been sent.

**Read with the rest of the appendix, no page gives INMODE no effect.** pp. 341–342
introduce the timers as counting external pulses through the CNT input, p. 345
uses CNT as the serial port's shift clock, and p. 352's worked example clears
bit 5 with the rest of CRA's low six bits (`and.b #%11000000,d0`) before it
starts timer A, so the timer it times counts φ2. p. 340 calls CIA-B's serial
data register "unused" and p. 345 calls its shift register "unassigned"; both
are about the shift register, and p. 351 still wires CIA-B's CNT to POUT.
Whatever drives CNT, a timer counting CNT transitions does not count φ2 pulses.

---

## What NanoMig does

**Timer A counts the E clock whatever CRA bit 5 holds, and timer B's source is
chosen by CRB bit 6 alone.**
[`src/minimig-aga/cia_timera.v#L31-L32`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L31-L32):

```verilog
// count enable signal
assign count = eclk;
```

[`src/minimig-aga/cia_timerb.v#L30-L31`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerb.v#L30-L31):

```verilog
// Timer B count signal source
assign count = tmcr[6] ? tmra_ovf : eclk;
```

Bit 5 is written with the rest of the register,
`tmcr[6:0] <= {data_in[6:5],1'b0,data_in[3:0]};`
([`cia_timera.v#L40`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L40),
[`cia_timerb.v#L39`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerb.v#L39)),
and read back, `({8{~wr&tcr}} & {1'b0,tmcr[6:0]});`
([`cia_timera.v#L108`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timera.v#L108),
[`cia_timerb.v#L103`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerb.v#L103));
no other line of either module reads it. Neither module has a CNT input, and
neither do the modules around them,
[`ciaa.v#L80-L104`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L80-L104)
and
[`ciab.v#L4-L22`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciab.v#L4-L22).

| CRA bit 5, CRB bits 6-5 | the page | NanoMig |
|---|---|---|
| CRA `0` | φ2 pulses | E-clock pulses |
| CRA `1` | positive CNT transitions | E-clock pulses |
| CRB `00` | φ2 pulses | E-clock pulses |
| CRB `01` | positive CNT transitions | E-clock pulses |
| CRB `10` | timer A underflow pulses | timer A underflow pulses |
| CRB `11` | timer A underflow pulses while CNT is high | timer A underflow pulses |

Nothing in NanoMig would drive a CNT pin if the timers had one. Key codes reach
CIA-A's serial data register as whole bytes, with no KCLK,
[`ciaa.v#L186`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/ciaa.v#L186),
`sdr_latch[7:0] <= ~{kbd_mouse_data[6:0],kbd_mouse_data[7]};`, and CIA-B's PA1,
POUT, is an input tied high,
[`minimig.v#L648`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/minimig.v#L648),
`.porta_in({cd,cts,dsr,ri&_joy3[4],1'b1,_joy4[4]}),`.

### Measured

[`hrm/appF-cia-timer-modes`](../../hrm/appF-cia-timer-modes) compiles each
timer module alone and drives its ports directly; no CPU core is involved.
`eclk` held high is one E-clock pulse per `clk7_en` cycle.

- **`CRA-INMODE`** (`test_cia_timera.cpp:475-515`). Two fresh fixtures load
  timer A with 5 while it is stopped, start it in continuous mode with CRA =
  `$01` (bit 5 clear) in one and `$21` (bit 5 set) in the other, and give both
  11 E-clock pulses. Both give 2 underflows and a remaining count of `$00`.
  With bit 5 set, p. 348 makes timer A count CNT transitions; the fixture has
  no CNT and so no transition, and by the page the count would stay at 5 with
  no underflow.
- **`CRB-INMODE-ARITY-00-01`** (`test_cia_timerb.cpp:476-518`) and
  **`CRB-INMODE-ARITY-10-11`** (`test_cia_timerb.cpp:539-571`). For each of the
  four codes, timer B is loaded with 255 and started, given 10 E-clock pulses,
  reloaded, and given 10 one-cycle pulses on `tmra_ovf`, timer A's underflow
  input. The decrements, as (E clock, underflows):

  | CRB bits 6-5 | NanoMig | what p. 349 gives for the same stimuli, with CNT not moving |
  |---|---|---|
  | `00` | (10, 0) | (10, 0) |
  | `01` | (10, 0) | (0, 0) |
  | `10` | (0, 10) | (0, 10) |
  | `11` | (0, 10) | (0, 10) with CNT high, (0, 0) with CNT low |

  `CRB-INMODE-ARITY-00-01` requires codes `00` and `01` to differ. NanoMig's
  two are identical, and it is red. `CRB-INMODE-ARITY-10-11` prints codes `10`
  and `11`, identical too, on a RECORDED line and declines to grade them: the
  fixture has no CNT pin to hold low, and with CNT high p. 349's mode 11 counts
  what mode 10 counts.

  That CRB `11` counts as `10` is read from the source, not shown by an
  assertion: [`cia_timerb.v#L30-L31`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cia_timerb.v#L30-L31),
  `assign count = tmcr[6] ? tmra_ovf : eclk;`, has no term for bit 5 and none
  for CNT.

`CRB-INMODE-CODE-00` and `CRB-INMODE-CODE-10`, green on the same fixture in the
same run, read (10, 0) and (0, 10): the fixture delivers both stimuli, and CRB
bit 6 is seen. On the `cia_timera` fixture `CRA-START` and `CRA-RUNMODE`, green,
count through the same `count` line. The run ends `9 passed, 1 failed` for
`cia_timera` and `7 passed, 1 failed` for `cia_timerb`, the one failure in each
being the assertion above.

At the system boundary,
[`integration/appF-cia-system`](../../integration/appF-cia-system) grades
the zero-edge half of both bits on fx68k and on TG68K, through 68000 programs
on the whole of `nanomig.v`. It reads 97 passed, 26 failed on each core, the
same lines on both. Three of its red lines carry this finding, and each shows
it differently:

- **`P7.5`** (`:2401-2403`) loads timer A with $1000,
  sets CRA to $21 (START, INMODE = CNT), waits, and reads the timer after the
  stop write; the package measures the window, from the marker after the
  start write to the one after the stop write, as 2236 E periods on fx68k and
  2666 on TG68K. It grades CIA-B, whose CNT is POUT and which nothing drives in
  this run, so the timer sees no positive transition: p. 348 leaves it where it
  was, v = $1000. NanoMig's timer reads $0744 on fx68k and $0596 on TG68K: 2236
  and 2666 counts, one for every E period of the window. `P7.5-WINDOW-CIAB`,
  green, has the stop write after the ready marker with W in [1500, 3000] E
  periods. This leg asks for nothing but a timer that reads bit 5, and it is
  this finding.
- **`P7.5-CIAA-INMODE`** (`:2442-2444`) is the same on CIA-A's timer A. One run
  starts the timer twice with CRA = $21. In the first interval no key code is
  sent and no CNT edge occurs: `P7.5-CIAA-INMODE-PRE`, green, shows that the ICR
  read after the interval has SP = 0, that the key code is not sent before the
  interval ends, and that the interval is long enough for a timer counting φ2 to
  count at least one pulse. p. 348 leaves the timer at $1000, and the line reads
  v1 = $0DFE on fx68k and $0D9D on TG68K. Because no edge is involved, this leg
  does not depend on whether KCLK reaches CNT. The second interval of the same
  run has one key code in it, and is printed, not graded: i2 = $08 and
  v2 = $0544 on fx68k, $0335 on TG68K.
- **`P8.65-MODE01`** (`:2481-2483`) is the first for timer B with CRB = $21, on
  CIA-B, and reads the same, $0744 and $0596; `P8.65-MODE01-WINDOW-CIAB` is
  green.

The counting half of INMODE = CNT, "counts positive CNT transitions", is not
graded in any of the three: `P7.5-COUNTING`, `P7.5-CIAA-INMODE-COUNTING` and
`P8.65-MODE01-COUNTING` are RECORDED lines, because CIA-B's CNT is undriven and
no output of the system shows KCLK at CIA-A's CNT input. So the P10 members of
INMODE = CNT are FAIL or not graded, never PASS.

With a key code in the window, CIA-A's timers are measured and not graded.
On CIA-A the same program sends one key code inside the window, where a
timer that counted CNT would count 8, one for each KCLK pulse p. 357 gives a code. The timer reads the same
$0744 and $0596, the φ2 count of the window. The key reaches SDR whole, with no
KCLK, as *What NanoMig does* shows, and nothing in the package observes KCLK at
the timer's CNT input, so that excess decrement is either an ignored bit 5 or a
missing KCLK path, and a timer that read bit 5 would not count 8 there either.
The package prints it as a MEASURED line (`P7.5-CIAA`, and `P8.65-MODE01 CIAA`
for timer B: $0744 and $0596 as well), not graded. `P7.5-WINDOW`, green,
reads SP = 1 in ICR before the stop write: the key arrived inside the window,
and `P7.6-CIAA`, graded under the timing clauses of that window, prints SP = 1
as its own result, green.

- **`P8.65-MODE11-PAIRK`** (`:2745-2748`) is a MEASURED line, not graded: CIA-A's
  timer B counts timer A's underflows (latch 3) in two runs, each with one key
  code sent, once in mode 10 (CRB = $41) and once in mode 11 (CRB = $61), and
  p. 349 counts in mode 11 only "while CNT pin is held high" while p. 357
  pulses KCLK low once per bit, so mode 11 should count fewer. Both runs count
  559 on fx68k and 666 on TG68K (`D_Y(K) >= D_X(K)`), over windows of equal
  length (`P8.65-MODE11-WINPRE`, green), and the key arrived in both
  (`P8.65-MODE11-WINDOW`, green). Mode 11 counting as mode 10 is what
  NanoMig's unread bit 5 gives; it is also what a timer that read bit 5 would
  give with a CNT that no key pulses, as MiSTer's does with CNT tied high. The
  line cannot tell the two apart, so it is not graded, and mode 11's P10 member
  is not graded either: only its first half, with no key sent, is graded.

Green in the same run: `P5-LEGA`, all four timers counting φ2 at one per E
period; `P8.65-MODE10`, timer B counting timer A's underflows, 48 of them, on
both CIAs; and `P8.65-MODE11-PAIRN`, the two modes counting alike, 559 and 666,
with no key sent, as p. 349 gives while CNT stays high. `KP-A` and `KP-B`, the
check that the keyboard is idle, are recorded rather than graded: their probe
is the other timer set to CNT, which on this build counts φ2 whether a key is
sent or not ($0748 with and without a key on fx68k, $0599 on TG68K).

**The controls.** `P7.5-CIAB-GREENCTL` and `P8.65-MODE01-CIAB-GREENCTL`
(`:2456-2460`, `:2513-2517`), which the package runs only when `NMH_MUTATE`
selects `P75-GREENCTL` or `P865M01-GREENCTL`, put a timer whose count input is
known through one equality comparator. CIA-A's timer A (CRA = $21) or timer B
(CRB = $21) is started twice with the same code, and stopped and read after each
start. No CNT edge occurs in the first interval: the key code is not sent
before it ends and the ICR read after it has SP = 0
(`P7.5-CIAB-GREENCTL-PRE` and `P8.65-MODE01-CIAB-GREENCTL-PRE`, green). One key
code, eight KCLK pulses, is sent in the second, and i has SP = 1. A timer that
read its INMODE bit and saw the key code's pulses would read $1000 after the
first interval and 8 below that after the second. On the repository's RTL both
controls print FAIL on fx68k, reading v1 = $0DFE, i1 = $00, i2 = $08 and
v2 = $0544: the timer counts φ2 in both intervals. The controls are not a
second measurement of the finding: they show what a passing line would read,
and need a timer that reads bit 5 and a key path that produces KCLK, which the
repository's RTL has neither of. The CIA-A decrement while a key code is sent
and `P8.65-MODE11-PAIRK` have no control of their own.

**The claim rows.** The package's P10 rows carry the INMODE claim ids
(`:3108-3172`). Each joins the lines named for its claim and measures nothing
of its own, so none is evidence beside those lines. `sol:F-cra.c13`, CRA
INMODE, is red through `P7.5` and `P7.5-CIAA-INMODE`, and
`sol:F-crb.c7, sol:F-crb.c10`, CRB INMODE, through `P8.65-MODE01`; its mode-11
member is not graded. `P5` and `P8.65-MODE10`, their other members, are green.
The bitmaps' 1-settings, `opus:F-cra.c13` and `opus:F-crb.c14`, are red through
the same lines and also through the PBON and OUTMODE lines of
[NM-HRM-074](NM-HRM-074.md) and, for CRA, through `P7.6-CIAB`
([NM-HRM-086](NM-HRM-086.md)).

---

## What the others say

The question put to each: does a timer whose INMODE selects CNT stop counting
φ2, and where does its CNT come from?

| | code | |
|---|---|---|
| **WinUAE** | [`cia.cpp:83-84`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L83-L84), [`:566`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L566), [`:596`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L596), [`:614-616`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L614-L616), [`:2236-2250`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L2236-L2250), [`:1091-1099`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/cia.cpp#L1091-L1099) | <pre>if ((c-&gt;t[0].cr &amp; (CR_INMODE \| CR_START)) == CR_START \|\| c-&gt;t[0].inputpipe) {</pre> timer A takes E-clock cycles only while `CR_INMODE`, `#define CR_INMODE 0x20`, is clear, and timer B only while both of its INMODE bits are, `if ((c->t[1].cr & (CR_INMODE \| CR_INMODE1 \| CR_START)) == CR_START \|\| c->t[1].inputpipe) {`. A CNT-mode timer is counted in `cia_inmode_cnt()`, whose one caller is CIA-B's port-A write on a rising PA1 driven as an output, `if ((val & 2) && !((odata & odir) & 2)) {` … `icr \|= cia_inmode_cnt(1);`. CIA-A has no CNT source: a key code is written into its SDR whole, `cia[num].sdr = code;`, so a CIA-A timer set to count CNT stands still. Modes 10 and 11 both take timer-A underflows, under the comment *"B INMODE=10 or 11 (B counting A underflows)"*, with no CNT-level term |
| **vAmiga** | [`Core/Components/CIA/CIARegs.cpp:439-445`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L439-L445), [`:526-535`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIARegs.cpp#L526-L535), [`Core/Components/CIA/CIA.cpp:270-281`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L270-L281), [`:446`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L446), [`:1035-1036`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CIA/CIA.cpp#L1035-L1036), [`Core/Peripherals/Keyboard/KeyboardEvents.cpp:51-66`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Keyboard/KeyboardEvents.cpp#L51-L66), [`Core/Infrastructure/Defaults.cpp:184`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Infrastructure/Defaults.cpp#L184) | <pre>if (value &amp; 0x20) {<br>&nbsp;&nbsp;&nbsp;&nbsp;delay &amp;= ~(CIACountA1 \| CIACountA0);<br>&nbsp;&nbsp;&nbsp;&nbsp;feed &amp;= ~CIACountA0;</pre> a CRA write with bit 5 set stops timer A's per-cycle count, and a CRB write with bit 6 or 5 set stops timer B's, `if (value & 0x60) {`. A rising edge on CNT counts them instead, `if ((cra & 0x21) == 0x21) delay \|= CIACountA1;` and `if ((crb & 0x61) == 0x21) delay \|= CIACountB1;`, and a timer-A underflow counts timer B in mode 10, or in mode 11 while the pin is high, `if ((crb & 0x61) == 0x41 \|\| ((crb & 0x61) == 0x61 && cnt)) {`. CIA-A's CNT is the keyboard's clock, `ciaa.emulateFallingEdgeOnCntPin();` then `ciaa.emulateRisingEdgeOnCntPin();` for each bit, in the accurate keyboard mode that is on by default, `registerDefault(Opt::KBD_ACCURACY, true);`; CIA-B's is PA1, `if (!(oldPA & 2) && (pa & 2)) emulateRisingEdgeOnCntPin();` |
| **Denise** | [`emulation/cia/new/register.cpp:387-391`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L387-L391), [`:422-437`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/register.cpp#L422-L437), [`emulation/cia/new/cia.cpp:273-277`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L273-L277), [`:369-378`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/cia/new/cia.cpp#L369-L378), [`emulation/libami/input/keyboard/keyboard.cpp:51-58`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/keyboard/keyboard.cpp#L51-L58), [`emulation/libami/system/system.cpp:139`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/system/system.cpp#L139) | <pre>// start + phase in<br>if ( (value &amp; 1) &amp;&amp; !(value &amp; 0x20) ) {</pre> timer A runs on φ2 only while CRA bit 5 is clear; for CRB, bit 6 set stops it, `if (value & 0x40) {`, and otherwise the same bit-5 test applies. A rising CNT edge steps timer A when `(timerA.control & 0x21) == 0x21` and timer B when `( timerB.control & 0x61) == 0x21`; a timer-A underflow steps timer B in mode 10, `if ( (timerB.control & 0x61) == 0x41 )`, and in mode 11 only with CNT high, `else if ( (delay & CIA_CNT1) && ((timerB.control & 0x61) == 0x61 ))`. CIA-A's CNT is the keyboard's clock, `cia.setCNTAndSP(false);` then `cia.setCNTAndSP(true, shiftOut & 0x80);` for each bit; CIA-B's is PA1, `cia2.setCNTAndSP( lines->ioa & 2, lines->ioa & 1 );` |
| **Copperline** | [`src/chipset/cia.rs:632`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L632), [`:738`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L738), [`:750-760`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L750-L760), [`:1066-1073`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L1066-L1073), [`:664-686`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L664-L686), [`src/chipset/keyboard.rs:709-721`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/keyboard.rs#L709-L721), test [`src/chipset/cia.rs:1546-1562`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/cia.rs#L1546-L1562) | <pre>TimerBInputMode::Phi2 =&gt; ticks,<br>TimerBInputMode::Cnt =&gt; 0,<br>TimerBInputMode::TimerA =&gt; ta_underflows,<br>TimerBInputMode::TimerAWhileCntHigh =&gt; {<br>&nbsp;&nbsp;&nbsp;&nbsp;if self.cnt_pin_high {</pre> CRB bits 6-5 decode to four sources, `match (val >> 5) & 0x03 {`, and timer B takes E-clock ticks only in the first. CRA bit 5 sets `self.ta_counts_cnt = val & 0x20 != 0;`, and timer A takes E-clock ticks only `if self.ta_running && !self.ta_counts_cnt && ticks > 0 {`. CIA-A's CNT is the keyboard's KCLK, `cia_a.cnt_falling_edge();` then `irq \|= cia_a.cnt_rising_edge(level);`; CIA-B's is PA1, under the comment *"A500 board tie: CIA-B PA0 is wired to this CIA's own SP pin and PA1 to its own CNT pin"*. Its own test `timer_b_gated_timer_a_mode_respects_cnt_pin` sets CRB to mode 11 and asserts no timer-B interrupt with CNT low and one with CNT high |
| **MiSTer** | [`rtl/cia_timera.v:41-50`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timera.v#L41-L50), [`rtl/cia_timerb.v:40-50`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timerb.v#L40-L50), [`rtl/minimig.v:696`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L696), [`:722`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L722) | descendant — **has made the change**: `assign count = tmcr[5] ? cnt_rise : eclk;` for timer A, and `assign count = tmcr[6] ? (tmcr[5] ? (tmra_ovf & cnt_sync[1]) : tmra_ovf)` … `: (tmcr[5] ? cnt_rise : eclk);` for timer B, from a new `cnt` input through a synchroniser, `cnt_sync[2:0] <= {cnt_sync[1:0],cnt};`. `minimig.v` ties both CIAs' `.cnt_in(1'b1),`, so a CNT-mode timer counts nothing and mode 11 counts as mode 10 — see *Where a fix already exists* |
| **TC64** | [`rtl/minimig/cia_timera.v:32`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timera.v#L32), [`rtl/minimig/cia_timerb.v:31`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/cia_timerb.v#L31) | descendant — identical to NanoMig on this point: `assign count = eclk;` and `assign count = tmcr[6] ? tmra_ovf : eclk;`, with bit 5 stored, read back and never selecting anything. Beyond a comment, TC64's two timer files differ from NanoMig's only in the path that loads the counter after a high-byte write, which does not reach the count source |

**All four implement the page, and none counts φ2 in a CNT mode.** Each stops
a timer's φ2 count once its INMODE selects CNT and counts CNT's rising edges
instead. vAmiga,
Denise and Copperline give CIA-A's CNT the keyboard's clock and CIA-B's the PA1
line p. 351 pairs it with, and count mode 11 only while CNT is high. WinUAE
drives only CIA-B's CNT and counts mode 11 as mode 10 whatever CNT's level,
which is what p. 349 gives while CNT is high, as CIA-A's is between key codes
(p. 357).


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **6502.org dissection of CIA 8520R4, undated — third-party, the silicon of one revision**, schematic sheets 8c, 9c, 12 and 5e — agrees with the manual:

  *Read from the drawing, not quoted:* sheet 8c, drawn transistor by transistor, enables timer A's low-byte carry input (TA_cin0#) only while TA_CEN, CRA0 as sampled, is set and either CRA5 is clear or CNT_EDGE is present. CNT_EDGE is a one-cycle pulse that sheet 12 derives from a falling edge of CNT#, the inverted CNT pad of sheet 5e, so from a rising edge of CNT. Sheet 9c, drawn the same way for timer B, selects with CRB6: with CRB6 clear, every cycle (CRB5 clear) or CNT_EDGE (CRB5 set); with CRB6 set, timer A's underflow TA_INT, alone (CRB5 clear) or only while CNT, sampled from CNT#, is high (CRB5 set).
  <!-- drawing: cia-8520r4-dissection/sheet_012.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_013.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_017.txt -->
  <!-- drawing: cia-8520r4-dissection/sheet_007.txt -->

  INMODE selects the count source: timer A counts every φ2 cycle or each rising edge of CNT, and CRB6 and CRB5 give timer B the four modes p. 349 prints, mode 11 counting timer A's underflows only while CNT is high. The dissection is of one revision.

- **Commodore, 6526 CIA specification, 1981**, PDF page 8 — agrees with the manual:

  > "5.6 INMODE Bits CRB5 and CRB6 select one of four input modes for TIMER B as:"
  <!-- corpus: cia-6526-spec/p008.txt -->

  The same page gives CRA bit 5, INMODE, as the manual does (1 = timer A counts positive CNT transitions, 0 = it counts phi2 pulses), and CRB bits 6 and 5 the four modes p. 349 prints: phi2 pulses, positive CNT transitions, timer A underflow pulses, and timer A underflow pulses while CNT is high. The OCR reads phi2 as @2. It is a 6526 document and does not itself speak for the 8520.

---

## Where a fix already exists

**MiSTer's line has made this change; TC64 has not, and NanoMig has the form
TC64 keeps.** At the pinned revision MiSTer's two timer modules each take a
`cnt` input, pass it through a three-stage synchroniser and select the count
source from INMODE —
[`rtl/cia_timerb.v:40-50`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cia_timerb.v#L40-L50):

```verilog
always @(posedge clk)
  if (reset)
    cnt_sync[2:0] <= 3'b111;
  else if (clk7_en)
    cnt_sync[2:0] <= {cnt_sync[1:0],cnt};

assign cnt_rise = cnt_sync[1] & ~cnt_sync[2];

// Count source selected by CRB bits 6:5 (INMODE)
assign count = tmcr[6] ? (tmcr[5] ? (tmra_ovf & cnt_sync[1]) : tmra_ovf)
                       : (tmcr[5] ? cnt_rise : eclk);
```

— and `ciaa.v` and `ciab.v` carry the input to both timers, `input cnt_in,`
([`rtl/ciaa.v:107`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L107),
[`rtl/ciab.v:28`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciab.v#L28)).

`minimig.v` ties both CIAs' CNT high. That gives the page's behaviour for a CNT
that never moves: a CNT-mode timer stops, and mode 11 counts what mode 10
counts. For CIA-A that is KCLK between key codes (p. 357), and for CIA-B a POUT
nothing drives. MiSTer's key codes, like NanoMig's, reach SDR whole,
`sdr_latch[7:0] <= ~{kbd_mouse_data[6:0],kbd_mouse_data[7]};`
([`rtl/ciaa.v:228`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/ciaa.v#L228)),
so its CIA-A timers do not see the eight KCLK pulses p. 357 gives a code either.

This is the case the findings README describes: a finding with a patch to point
at in one maintained line and the original code in the other.

---

## Suggested change

**One option: port MiSTer's count-source selection.** Add the synchroniser and
the two `count` expressions to `cia_timera.v` and `cia_timerb.v`, a `cnt` input
to `ciaa` and `ciab`, and tie it high in `minimig.v` as MiSTer does. CIA-B's
PA1, the POUT that p. 351 pairs with its CNT, is already an input tied to
`1'b1` there (`minimig.v:648`). This has not been run here. What it would
change:

- a timer whose INMODE selects CNT would stop counting; `CRA-INMODE` asks for
  exactly that difference;
- codes `00` and `01` would differ, the difference `CRB-INMODE-ARITY-00-01`
  asks for; codes `10` and `11`, which `CRB-INMODE-ARITY-10-11` declines, would
  differ only on a fixture that held the new CNT input low: with CNT high,
  p. 349's mode 11 counts what mode 10 counts;
- CIA-A's timers would count KCLK only if the key path produced it. p. 357's
  eight pulses per code are the page's KCLK, and nothing in NanoMig makes them.
  In `integration/appF-cia-system` the CIA-B legs of `P7.5` and
  `P8.65-MODE01` would read v = $1000, as they ask, and so would
  `P7.5-CIAA-INMODE`'s first interval; the decrement CIA-A's timers make while a
  key code is sent, which expects 8 counts, and `P8.65-MODE11-PAIRK`, which
  expects mode 11 to count fewer than mode 10, are not graded and would print
  what the key path gives.

---

## What holds the finding up

- **The page states the selection five times and never qualifies it**: p. 343's
  *INPUT MODES*, the CRA and CRB function tables on pp. 348–349 and both
  bitmaps on pp. 349–350. p. 351 wires both CIAs' CNT to a signal, and no page
  of Appendix F, chapter 8 or Appendix G gives INMODE no effect.
- **The code has no other reading.** Bit 5 is read nowhere in either timer
  module but the read-back mux, and no module above them has a CNT input.
- **The fixture is live on both of timer B's sources.** `CRB-INMODE-CODE-00`
  and `CRB-INMODE-CODE-10` are green on the stimuli `CRB-INMODE-ARITY-00-01` uses:
  CRB bit 6 is seen, and bit 5 is not.
- **Every implementation read here but TC64 implements the page**, the four
  independent ones and MiSTer's line alike; TC64 has NanoMig's code.

## What weakens it

- **Neither fixture has a CNT input**, so the tests show that bit 5 changes
  nothing, not what a CNT mode counts. For `CRA-INMODE` that is enough: with CNT
  at either level and not moving, p. 348's mode 1 counts nothing, and NanoMig
  counts every E-clock pulse. For `CRB-INMODE-ARITY-00-01` it is enough too:
  p. 349's mode 01 counts nothing and NanoMig counts 10. For codes `10` and `11`
  it is not: with CNT held high, p. 349's mode 11 counts every underflow mode 10
  counts, as WinUAE does and as MiSTer does with its CNT tied high, so
  `CRB-INMODE-ARITY-10-11` records the comparison and does not grade it.
- **In NanoMig nothing would drive CNT.** The difference a program can see is a
  timer that keeps counting E-clock pulses after it is set to count CNT, and a
  CRB mode 01 that counts them too; whether any Amiga software does that is not
  settled here.
- **At the system's ports the zero-edge half isolates bit 5.** It holds on
  CIA-B's two timers and on CIA-A's timer A over an interval with no CNT edge.
  The decrement CIA-A's timers make while a key code is sent, and
  `P8.65-MODE11-PAIRK`, expect what KCLK pulses on CIA-A's CNT would give. They
  read what an unread bit 5 gives, but with no KCLK they would not read the
  page's values with bit 5 read either, so none of the three is graded.
- **Only mode 11's first half is graded.** `P8.65-MODE11-PAIRN`, with CNT idle
  high, is green: mode 11 counts what mode 10 counts, as p. 349 gives. The
  half that needs CNT low is the pair-K line, and it cannot separate an ignored
  mode 11 from a KCLK that never reaches CNT.

---

## For the specialists

1. **Does Amiga software set INMODE?** A program that counts KCLK or POUT edges
   with a CIA timer, or that stops a timer by switching it to CNT, would see the
   timer keep counting E-clock pulses on NanoMig and TC64. In all four
   implementations it counts CNT edges only, and on MiSTer, whose CNT is tied
   high, it counts none.
2. **What should CIA-A's CNT be?** Tied high, as MiSTer has it, gives the page
   for an idle keyboard. Only a KCLK made from the key path, eight pulses per
   code (p. 357), would give the integration package's CIA-A cases the counts
   it specifies.
