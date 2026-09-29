# Specification — `appF-cia-register-map`

Package: `hrm/appF-cia-register-map`
Fixture: `ciaa` (compiled alone), per the build task's assignment. `ciab`'s
decoder is read for comparison only (see "Chip-generic scope" below); it is
not compiled by this package.
Claims file: `hrm-testpipeline/work/2026-09-17_appF_packages.jsonl`, filtered
on `classify.package == "appF-cia-register-map"` — 152 claims.
Oracle: *Amiga Hardware Reference Manual*, 3rd edition, printed pages 339,
340, 351, 352, 353, 354 — and nothing else. An emulator is never an oracle;
the RTL is never an oracle.

Subject: the CIAA address map and CIAB address map (printed p.339/p.340 — one
table per chip, byte address → register name → meaning) and the Chip
Register Map (printed p.340 — RS3-RS0 → register number → name → meaning,
stated as common to both 8520s), plus the general-operation prose on
pp.353-354 (CS, RS3-RS0, DB7-DB0, RES) and the scattered p.351/352 items this
package's claims touch.

## Order of work, and where it could not hold blind

The build brief requires `SPECIFICATION.md` to be written from the
quotations alone, committed, and only then have the RTL opened. That could
not hold in full here, for the same reason `ch8-ciaa-port-direction` and
every other appendix-F package disclosed: `classify.evidence` carries a
prior reader's RTL-derived measurements — file:line citations, register
names, even simulated-fixture readings — inline in the same JSON records
that carry `extract.quote`/`extract.page`/`extract.restatement`. Reading the
152 claims to build the inventory below unavoidably exposed that content.

What follows is stated from the *pages* — read directly as page images for
every figure-kind claim, and as page text for text-kind claims — not derived
from what the RTL happens to compute. Where `classify.evidence` had already
measured something (e.g. that `ciaa.v` and `ciab.v` decode `rs` identically,
or that the TOD alarm register is not readable through the register
interface), that is used only to decide *how* to test a page proposition at
this fixture's boundary, never to change what the proposition being tested
*is*, and it is flagged inline where it mattered.

## The three ground-truth tables

The text layer for p.339 and p.340 is **shuffled** (`corpus-defects.md`):
wrapped fragments sit on the wrong physical row, and on p.340 specifically
the Chip Register Map's MEANING column is off by one row, stranding
"Peripheral data register A" above the `0 pra` row it belongs to. Every
figure-kind claim's `quote` is therefore the page's bracketed marker line,
not prose — the two tables below are transcribed directly from
`figures/p339.png` and `figures/p340.png`, read with the Read tool, not from
`p339.txt`/`p340.txt`.

### CIAA Address Map (p.339)

    Byte     Register  Data bits                                  Meaning
    Address  Name
    BFE001   pra       /FIR1 /FIR0 /RDY /TK0 /WPRO /CHNG /LED OVL
    BFE101   prb       Parallel port
    BFE201   ddra      Direction for port A (BFE001); 1=output (set to 0x03)
    BFE301   ddrb      Direction for port B (BFE101); 1=output (can be in or out)
    BFE401   talo      CIAA timer A low byte (.715909 Mhz NTSC; .709379 Mhz PAL)
    BFE501   tahi      CIAA timer A high byte
    BFE601   tblo      CIAA timer B low byte (.715909 Mhz NTSC; .709379 Mhz PAL)
    BFE701   tbhi      CIAA timer B high byte
    BFE801   todlo     50/60 Hz event counter bits 7-0 (VSync or line tick)
    BFE901   todmid    50/60 Hz event counter bits 15-8
    BFEA01   todhi     50/60 Hz event counter bits 23-16
    BFEB01             not used
    BFEC01   sdr       CIAA serial data register (connected to keyboard)
    BFED01   icr       CIAA interrupt control register
    BFEE01   cra       CIAA control register A
    BFEF01   crb       CIAA control register B

    Note: CIAA can generate interrupt INT2.

### CIAB Address Map (p.340)

    BFD000   pra       /DTR /RTS /CD /CTS /DSR SEL POUT BUSY
    BFD100   prb       /MTR /SEL3 /SEL2 /SEL1 /SEL0 /SIDE DIR /STEP
    BFD200   ddra      Direction for Port A (BFD000); 1=output (set to 0xFF)
    BFD300   ddrb      Direction for Port B (BFD100); 1=output (set to 0xFF)
    BFD400   talo      CIAB timer A low byte (.715909 Mhz NTSC; .709379 Mhz PAL)
    BFD500   tahi      CIAB timer A high byte
    BFD600   tblo      CIAB timer B low byte (.715909 Mhz NTSC; .709379 Mhz PAL)
    BFD700   tbhi      CIAB timer B high byte
    BFD800   todlo     Horizontal sync event counter bits 7-0
    BFD900   todmid    Horizontal sync event counter bits 15-8
    BFDA00   todhi     Horizontal sync event counter bits 23-16
    BFDB00             not used
    BFDC00   sdr       CIAB serial data register (unused)
    BFDD00   icr       CIAB interrupt control register
    BFDE00   cra       CIAB Control register A
    BFDF00   crb       CIAB Control register B

    Note: CIAB can generate INT6.

(`corpus-defects.md`: the text layer reads `cra`/`crb` as `era`/`erb` on
both tables, and reads digit `0` as letter `O` in several places on p.340's
CIAB column. The renders are unambiguous on both points — this package uses
the render's reading, `cra`/`crb`, throughout.)

### Chip Register Map (p.340, same render page as the CIAB address map)

    RS3 RS2 RS1 RS0  Register #(hex)  NAME     MEANING
    0   0   0   0    0                pra      Peripheral data register A
    0   0   0   1    1                prb      Peripheral data register B
    0   0   1   0    2                ddra     Data direction register A
    0   0   1   1    3                ddrb     Direction register B
    0   1   0   0    4                talo     Timer A low register
    0   1   0   1    5                tahi     Timer A high register
    0   1   1   0    6                tblo     Timer B low register
    0   1   1   1    7                tbhi     Timer B high register
    1   0   0   0    8                todlow   Event LSB
    1   0   0   1    9                todmid   Event 8-15
    1   0   1   0    A                todhi    Event MSB
    1   0   1   1    B                         No connect
    1   1   0   0    C                sdr      Serial data register
    1   1   0   1    D                icr      Interrupt control register
    1   1   1   0    E                cra      Control register A
    1   1   1   1    F                crb      Control register B

Printed lead sentence: "Each 8520 has 16 registers that you may read or
write. Here is the list of registers and the access address of each within
the memory space dedicated to the 8520" — stated once, for either chip.

### General-operation prose (pp.353-354)

> The CS input controls the activity of the 8520. A low level on CS while
> 02 is high causes the device to respond to signals on the R/W and address
> (RS) lines. A high on CS prevents these lines from controlling the 8520.
> The CS line is normally activated (low) at 02 by the appropriate address
> combination. (p.353)

> Address bits A11, A10, A9, and A8 are used to specify which of the 16
> internal registers you want to access. (p.353 — printed "A11"; the text
> layer misreads it "A ll")

> RS3-RS0 — address Inputs. The address inputs select the internal
> registers as described by the register map. (p.354)

> DB7-DB0 — data bus Inputs/outputs. The eight data bus output pins
> transfer information between the 8520 and the system data bus. These pins
> are high impedance inputs unless CS is low and R/W and 02 are high, to
> read the device. During this read, the data bus output buffers are
> enabled, driving the data from the selected register onto the system data
> bus. (p.354)

> RES — reset Input. A low on the RES pin resets all internal registers.
> The port pins are set as inputs and port registers to zero (although a
> read of the ports will return all highs because of passive pull-ups). The
> timer control registers are set to zero and the timer latches to all
> ones. All other registers are reset to zero. (p.354)

## Chip-generic scope: why testing `ciaa` alone grades a "both chips" claim

The Chip Register Map's own lead sentence ("Each 8520 has 16 registers...")
and every RS3-RS0 row in it describe the 8520 part generically, not a
CIAA-specific or CIAB-specific fact — unlike the two *address maps*, which
are chip-specific by construction (different byte addresses, different
port-pin meanings per chip). `ciaa.v`'s and `ciab.v`'s own address decoders
turn out (this is the one piece of RTL-evidence-shaped-testing-method this
document uses, per the disclosure above) to assign the same fifteen `rs`
values to the same fifteen register identifiers in both modules, with the
same 4'hB gap. This package's fixture builds `ciaa` only, per the task
assignment; the Chip-Register-Map assertions below are graded once, at
`ciaa`, as sufficient evidence for the chip-generic claim.

## Claim inventory (152 claims)

**38 claims carry a `classify.note` naming prior work.** Per the build
brief, all 38 go on `record()` lines, citing that note, not on assertions —
**with one honesty caveat**: four of the 38 (see "Brief accuracy" below) do
not actually name an accepted package; their note says the opposite
("New here", nothing grades this). Those four are still declined here, for
an independent boundary reason given below, not because a sibling package
already covers them.

**2 further claims** (`opus:F-conn.c4`, `sol:F-signals.c61`) are declined
for a boundary reason of this package's own finding: both restate "address
bits A11-A8 select the register", which is a fact about `minimig.v`'s
address bridge (`cpu_address_out[11:8]` → `rs`), not about `ciaa`. `ciaa`'s
own `rs` port is already the decoded nibble; it has no A11-A8 input to
observe. This is the same boundary/ownership reasoning
`ch8-ciaa-port-direction` used to decline CIAADDRA's bus-address claims
("`ciaa` alone has no address decoder").

**The remaining 111 claims** are graded across 20 assertions, one per
register or per general-operation proposition, listed below with every uid
each assertion carries. Multiple claims land on one assertion where they
restate the *same* proposition at different granularity (a per-register
triplet from one reader crossed with a per-register pair from the other,
sometimes plus a matching CIAA-address-map row) — the same "byte-wide claim,
not a claim about one bit" reasoning `ch8-ciaa-port-direction`'s PROP-1/
PROP-2 used to sweep eight bit positions under one assertion, applied here
to sweep a register's three or five near-duplicate restatements under one
assertion instead of writing near-identical assertions with near-identical
bodies fifteen times over.

| assertion | registers / topic | uids |
|---|---|---|
| REG0-PRA | rs=0, `pra` | sol:F-chip-map.c3,c4,c5; opus:F-chipmap.c2,c3 |
| REG1-PRB | rs=1, `prb` | sol:F-chip-map.c6,c7,c8; opus:F-chipmap.c4,c5 |
| REG2-DDRA | rs=2, `ddra` | sol:F-chip-map.c9,c10,c11; opus:F-chipmap.c6,c7 |
| REG3-DDRB | rs=3, `ddrb` | sol:F-chip-map.c12,c13,c14; opus:F-chipmap.c8,c9 |
| REG45-TIMERA | rs=4,5, `talo`/`tahi` | sol:F-chip-map.c15-c20; opus:F-chipmap.c10-c13 |
| REG67-TIMERB | rs=6,7, `tblo`/`tbhi` | sol:F-chip-map.c21-c26; opus:F-chipmap.c14-c17 |
| REG89A-TOD | rs=8,9,A, `todlow`/`todmid`/`todhi` | sol:F-chip-map.c27-c35; opus:F-chipmap.c18-c23; opus:F-ciaa-map.c18,c20,c22; sol:F-ciaa-map.c34,c37,c40 |
| REGB-UNUSED | rs=B, "No connect" | sol:F-chip-map.c36,c37; opus:F-chipmap.c24,c25; sol:F-ciaa-map.c42 |
| REGC-SDR | rs=C, `sdr` | sol:F-chip-map.c38,c39,c40; opus:F-chipmap.c26,c27 |
| REGD-ICR | rs=D, `icr` | sol:F-chip-map.c41,c42,c43; opus:F-chipmap.c28,c29; opus:F-ciaa-map.c26; sol:F-ciaa-map.c48 |
| REGE-CRA | rs=E, `cra` | sol:F-chip-map.c44,c45,c46; opus:F-chipmap.c30,c31; opus:F-ciaa-map.c28; sol:F-ciaa-map.c51 |
| REGF-CRB | rs=F, `crb` | sol:F-chip-map.c47,c48,c49; opus:F-chipmap.c32,c33; opus:F-ciaa-map.c30; sol:F-ciaa-map.c54 |
| REG-COUNT-16 | "16 registers you may read or write" | sol:F-chip-map.c1; opus:F-chipmap.c1 |
| CS-GATES-ACCESS | CS enables/disables R/W+RS response | opus:F-signals.c5,c6,c7; sol:F-signals.c73,c74,c75 |
| RS-SELECTS-DISTINCT | RS3-RS0 select the register per the map | opus:F-signals.c11; sol:F-signals.c80 |
| DATA-BUS-8BIT | DB7-DB0 carry data both ways | opus:F-signals.c12; sol:F-signals.c81 |
| READ-DRIVES-BUS | read enables output buffers | opus:F-signals.c14; sol:F-signals.c83 |
| RESET-AFFECTS-REGS | RES low resets internal registers | opus:F-signals.c18; sol:F-signals.c87 |
| RESET-ZEROS-REMAINING | "all other registers reset to zero" | opus:F-signals.c21; sol:F-signals.c90 |
| IO-PINS-16 | "16 general purpose I/O pins" | sol:F-overview.c3 |

That is 12 + 1 + 6 + 1 = 20 assertions covering 111 uids (94 in the
per-register/count groups + 16 in the signals groups + 1 in IO-PINS-16),
plus a `FIXTURE-LIVE` control and a `CONTROL-CAPABILITIES-CORE`
substitution-proof control for the three CHIP-CAPABILITIES-FIVE conjuncts
that do hold at `ciaa`'s own boundary (16 pins, the serial register, three
timers — see below). CHIP-CAPABILITIES-FIVE itself (`opus:F-intro.c3`) is
not one of the 20: it is a `record()`, listed with the declined claims
below.

## Claims declined with `record()` (41 total)

### 38 carrying `classify.note`

Grouped by what the note actually says (not all 38 say the same kind of
thing — see "Brief accuracy" below):

- **CIAA-map rows already graded by a register-behaviour package** (16):
  the `talo`/`tahi`/`tblo`/`tbhi` rate claims (`opus:F-ciaa-map.c10,c12,c14,
  c16,c22,c25,c28,c31` — `ch1-cia-timers` grades the generic timer
  proposition, not the rate); the `ddra`/`ddrb`/`prb` direction-and-parallel-
  port rows (`opus:F-ciaa-map.c4,c6,c8`, `sol:F-ciaa-map.c13,c16,c19` —
  `ch8-ciaa-port-direction` / `ch8-parallel-port-direction`); the `sdr`
  rows (`opus:F-ciaa-map.c24`, `sol:F-ciaa-map.c45` — `ch8-keyboard-
  serial-register`).
- **Overview-paragraph facts already graded elsewhere** (2): `sol:F-overview.
  c4` (serial register — `ch8-keyboard-serial-register`), `sol:F-overview.
  c5` (three timers — `ch1-cia-timers`).
- **p.351 CIA-A port-A gloss column, already graded by disk-signal and
  fire-button packages** (16): `opus:F-portsig.c8-c14`, `sol:F-signals.c9,
  c11-c18` — `ch8-cia-disk-signal-bindings`, `ch8-ciaapra-disk-sense-bits`,
  `ch8-ciaapra-fire-buttons`, `ch8-controller-port-io-path`,
  `ch8-cia-disk-port-windows`, `ch8-ciaapra-address-window`,
  `ch7-interrupt-priority-encoder` (the last two only "Related", not a full
  match — quoted honestly on the `record()` line, not smoothed into a claim
  of full coverage).
- **p.354 port-reset claim, partially graded** (2): `opus:F-signals.c19`,
  `sol:F-signals.c88` — `ch8-ciabprb-disk-control-bits` grades the CIAB
  port-B leg (`portb_out`=0xFF after reset) only; the note says to decline
  that leg, and this package declines the whole claim rather than split a
  single uid's citation across a decline and a fresh assertion.
- **p.352 LED-blink example, and the p.351 PA1/LED gloss row — genuinely
  new, no accepted package** (4): `sol:F-example.c18`, `opus:F-example.c25`,
  `opus:F-portsig.c14`, `sol:F-signals.c18`. See "Brief accuracy" below —
  these are declined for a boundary reason, not a duplicate-grading reason.

### 2 declined for this package's own boundary reason

`opus:F-conn.c4`, `sol:F-signals.c61` — "address bits A11-A8 select the
register" needs `minimig.v`'s address bridge, outside `ciaa`'s own port
list. See above.

### 1 declined for this package's own boundary finding (NM-HRM-077)

`opus:F-intro.c3` — CHIP-CAPABILITIES-FIVE's own claim, "an output pulse
pin and an edge detection input" (p.339). p.341 names these PC and FLAG,
both CIA-A's per p.351, p.337, p.324 and p.259. `ciaa` has neither pin at
its own boundary: no PC-equivalent output, no FLAG-equivalent input. A
comparator built from constants the test writes itself is not a
measurement of them — see docs/findings/NM-HRM-077.md. The other three of
the five conjuncts this claim names (16 pins, the serial register, three
timers) do hold at `ciaa`'s boundary and are graded by
`CONTROL-CAPABILITIES-CORE` above.

## Brief accuracy — checked, and not fully right

The build brief states "38 of them carry a `classify.note` naming an
accepted package that already grades them." **Four of the 38 do not.**
`sol:F-example.c18`, `opus:F-example.c25` ("Not graded anywhere in hrm/: no
test source drives minimig's pwr_led. New here."), `opus:F-portsig.c14` and
`sol:F-signals.c18` ("Not graded anywhere in hrm/... New here.") all say the
opposite of "an accepted package already grades them" — they say nothing
does.

These four are all one fact: CIAA PRA bit 1 (PA1) is printed as `/LED`,
driving the power light, 0 = bright. It is genuinely untested anywhere in
this suite. It is *not*, however, testable by a `ciaa`-alone fixture: PA1's
value reaches a physical LED only through `minimig.v` (`_led` / `pwr_led`),
outside this module's own port list, the same boundary
`ch8-ciaa-port-direction` already drew around CIAADDRA/CIAAPRA's bus
address. `ciaa`'s own half of the fact — that PA1 is one of the chip's two
genuinely bidirectional port-A output bits — is already exercised (blindly,
as one of eight swept positions) by `ch8-ciaa-port-direction`'s PROP-1/
PROP-2. So these four are declined here too, but for a boundary reason this
package is finding fresh, not because the brief's cited "accepted package"
exists for them. This is disclosed, not silently folded into the other 34.

No other brief inaccuracy was found by this package while building it.

## Assertion design notes

- **FIXTURE-LIVE.** Two distinct, known bytes must both arrive at a chosen
  register (DDRA) and read back correctly and distinctly before anything
  else is trusted, per every sibling package's own convention.
- **Register-identity assertions test the *distinguishing* behaviour named
  by the manual, not a generic write-then-read-back check that would pass
  even if two registers' roles were swapped.** `pra`/`prb` are shown to
  reflect their external input ports (distinct from a stored register);
  `ddra`/`ddrb` are shown to store the full byte written *and* to gate
  `porta_out`/an equivalent; `talo`/`tahi` and `tblo`/`tbhi` are shown to be
  the low/high halves of a register that actually counts down once started,
  not just two inert bytes; the TOD trio is shown to be the three bytes of
  one counter that increments on `tick`; `sdr` is shown to return exactly
  what was written; `icr` is shown to have read-clears-latched-bits
  semantics, the one register on this map that behaves that way; `cra`/
  `crb` are shown to actually start/stop their timers, not just store a
  byte. This is the same standard `ch5-audio-register-map`'s PER-REGISTER-
  IDENTITY / LEN-REGISTER-IDENTITY / VOL-REGISTER-IDENTITY assertions used.
- **What register-identity assertions deliberately do *not* test:** the
  exact NTSC/PAL clock rate (declined elsewhere, see above, and
  `classify.evidence` already found no RTL path distinguishes them); the
  exact bit-level semantics of CRA/CRB beyond "controls timer run state"
  (owned by `appF-cia-control-registers` and `appF-cia-timer-modes`); the
  keyboard rotation semantics of `sdr` (owned by
  `ch8-keyboard-serial-register`); the mask/bit-position semantics of ICR
  beyond "sets/clears on bit 7, clears latched bits on read" (owned by
  `appF-cia-interrupt-control`).
- **CHIP-CAPABILITIES-FIVE is a `record()`, not an assertion.** p.341 names
  the output pulse pin PC and the edge detection input FLAG, both CIA-A's
  per p.351, p.337, p.324 and p.259; `ciaa` has neither pin at its own
  boundary (docs/findings/NM-HRM-077.md). A comparator built from
  constants the test writes itself is not a measurement of them, so the
  two conjuncts are recorded, not graded. `CONTROL-CAPABILITIES-CORE` still
  asserts the same capability-presence comparator restricted to the three
  conjuncts that do hold (16 pin positions exist, the serial register
  exists, three timer instances exist), as the substitution proof that the
  comparator is not vacuously false — `ch8-ciaa-port-direction`'s
  CONTROL-PA0-POSITIONAL/CONTROL-PA0-OUTPUT precedent.
- **RESET-ZEROS-REMAINING has a genuine, disclosed gap.** `cia_timerd.v:
  107-109` sets the TOD ALARM register to all-ones on reset, not zero — a
  real counter-example to "All other registers are reset to zero". But
  ALARM has no read path through `rs` at all (`cia_timerd.v`'s `data_out`
  mux only ever returns `tod_latch`, never `alarm`, regardless of CRB7);
  it is not observable at this fixture's boundary without reaching past
  `ciaa`'s own ports into `cia_timerd`'s internals, which this package does
  not do (no debug port, no RTL change). This is reported via `note()`,
  citing the RTL location, as a measurement graded by nothing — adjudicating
  nothing about whether it is a defect.
