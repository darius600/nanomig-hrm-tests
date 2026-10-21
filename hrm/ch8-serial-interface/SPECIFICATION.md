# Specification — `ch8-serial-interface`

Package: `hrm/ch8-serial-interface`
Fixture: `paula_uart` (module `paula_uart.v`, compiled alone)
Claims file: `hrm-testpipeline/work/ch8-s4-build/ch8-serial-interface.jsonl` (106 claims)
Subject: the chapter's own prose on pp.255-259 — the UART's general behaviour
(programmable rate and length, overrun, status bits, the receive path down to
SERDATR, the transmit path down to SERDAT, and the two shift-register
figures) — rather than one register's bit table in isolation. Unlike its four
sibling `paula_uart`-top packages, this one carries no finding candidate of
its own in `2026-09-16_ch8_s4_finding_candidates.md`.

**A note on the order of work, made honestly rather than pretended away.**
The build brief's rule is: write this file from the quotations alone, commit
it, and only then open the RTL. That rule already failed once in slice s3,
where a package list handed a builder its module's line numbers before the
spec was written. It has failed again here, through no fault of mine: this
package's own claims file — `ch8-serial-interface.jsonl`, handed to me
unconditionally by the task sheet — carries a `classify.evidence` field on
every one of its 106 records, and those fields quote `paula_uart.v` port
names, line numbers and measured register values (e.g. "reload terms
`paula_uart.v:112` and `:120`", "`rga_i`=0x018 -> `data_o`=0x3800"). I read
those fields before writing a single word below, because reading the claims
file is unavoidable — it is where the propositions themselves live. So the
port names were visible before this document existed, exactly the failure
mode the build brief warned about. I disclose it here and in my final report
rather than writing a paragraph claiming otherwise.

What I *did* keep clean: every expected value below is taken from the
printed sentence cited beside it, not from what the evidence field says the
RTL measured. Where the evidence field's own reasoning (its `split` or
`note`) settles what is and is not constructible at this fixture — the
divider-vs-frequency split, the color-clock boundary — I say so and cite it
as reasoning about the fixture's shape, not as a source of an expected
value. No port name, no line number and no measured number from the
evidence field appears below; those are step-3 material, in the `.cpp` file,
committed separately after this one.

## The slice's three fixture hazards, as they bite this package

1. **Reading a write-only address is a write.** `paula_uart` has no
   read/write strobe on its register bus — the same address input serves
   both directions, gated only by whether the write-enable condition inside
   the module happens to be true for that cycle. Addressing SERDAT ($030) or
   SERPER ($032) "to see what's there" is itself a write of whatever is
   currently presented on the data input. Every write-only proposition below
   (SERPER, SERDAT) is graded by observable effect, never by "expect the
   readback to equal the last written value" — and where a genuine read-back
   probe is needed to show the address returns a fixed, dead value, it is
   done last, in a harness instance nothing afterward depends on.
2. **One and two stop bits are not separable on `txd` by level.** A stop bit
   is a 1 and idle is also a 1; a fixture that just counts trailing 1-cells
   on `txd` cannot tell "one stop bit then idle" from "two stop bits" — it
   would read the same string either way and could not fail. The claim this
   slice's own candidates file names for this trap (normally one or two stop
   bits are sent, p.259) is graded here by total *frame length in bit
   cells*, measured through the transmit-shift-register-empty condition, not
   by parsing the level pattern.
3. **No frequency or period figure in this chapter is constructible at this
   fixture.** `ntsc` never reaches `paula_uart`, and this design's only
   colour-clock signal exists as a named wire in a different module
   (`amiga_clk`), never as a port of `paula_uart`, and its nominal frequency
   is carried nowhere but a source comment. A test built against `paula_uart`
   alone cannot construct 110 bits/second, 1,000,000 bits/second, 9600 baud,
   279.36 ns or 281.94 ns, and nothing under `nanomig/` may be added or
   changed to manufacture one — a board top's clock is a property of that
   board, not of this unit. Every claim below that names an absolute rate or
   period is declined on this basis and recorded, not graded. What survives
   is the *dimensionless* relationship the manual states in the same
   breath — the divisor's own range, and the shape "period is proportional
   to divisor+1" — which needs no clock in seconds to confirm.

## How propositions are grouped

106 claims from two independent readings collapse onto roughly forty
propositions once cross-reader twins and same-fact restatements at
different granularity are merged — the same shape slice s4's own merge note
found for this chapter overall (44.9% raw divergence, 9.3% once anchoring
and cross-slice coverage are discounted). Every uid below is accounted for,
either on an `expect()`-bearing proposition (cited by every uid that states
it) or on a `record()` line (cited alone, with the reason it is not graded).
No claim is silently dropped.

---

## A. Programmable rate and length (p.255)

### A1. Programmable rate range — NOT CONSTRUCTIBLE

> "This UART is programmable for any rate from 110 to over 1,000,000 bits
> per second."

Claims: `opus:8-serial.c8` (110 bps floor), `opus:8-serial.c9` (>1,000,000
bps ceiling), `sol:8-serial.c6` (floor, sol's wording), `sol:8-serial.c7`
(ceiling, sol's wording).

**Not constructible as worded.** Both figures are absolute bits-per-second
values; deriving either from this fixture needs the colour-clock frequency,
which is not a reachable value here (see hazard 3 above). **Declined and
recorded**, all four uids.

What the fixture *can* show, and what backs an untagged control instead of
these four uids: the baud-rate divider is 15 bits wide, so its span (the
ratio between the shortest and the longest programmable bit cell) covers
five orders of magnitude — a large, structurally-confirmable range even
though no absolute number in bits/second attaches to either end. See
`DIVIDER-SPAN` under section D.

### A2. Programmable data length, receive and send, compound (opus only)

> "It can receive or send data with a programmable length of eight or nine
> bits."

Claim: `opus:8-serial.c10`.

This single claim asserts four things at once: receive can be 8 bits,
receive can be 9 bits, send can be 8 bits, send can be 9 bits. Graded in
full as a conjunction of the four measurements described in A3-A6 below,
under this claim's own uid.

### A3/A4. Receive length, eight and nine bits

> "The number of bits that are to be received before the system tells you
> that the receive register is full may be defined either as eight or nine
> (this allows for 8 bit transmission with parity)."

Eight-bit claims: `opus:8-serial.c34`, `sol:8-serial.c8`, `sol:8-serial.c19`,
`sol:8-serial.c23` ("in eight-bit mode, the receive circuitry expects eight
data bits").
Nine-bit claims: `opus:8-serial.c35`, `sol:8-serial.c9`, `sol:8-serial.c20`,
`sol:8-serial.c24` ("in nine-bit mode, ... nine data bits").

**Testable form:** selecting each mode and receiving a known byte (or
9-bit word) returns exactly that many data bits at the expected register
position, and the two modes are distinguishable from each other on the same
wire protocol.

### A5/A6. Send length, eight and nine bits

> "It can receive or send data with a programmable length of eight or nine
> bits."

Eight-bit send: `sol:8-serial.c10`. Nine-bit send: `sol:8-serial.c11`.

**Testable form, per this claims file's own `split` note on A2 above:** the
transmit side has no dedicated length-select field; length is set entirely
by where the programmer places the highest 1-bit above the data (the point
where the "shift until all 1s are out" rule, section E, stops the frame).
Placing that top bit at the position documented for an 8-bit frame produces
a shorter frame than placing it one position higher for a 9-bit frame, and
both frames are separately producible and distinguishable by their total
length in bit cells.

### A7/A8. Overrun detection and definition

> "The UART is capable of detecting overrun errors, which occur when some
> other system sends in data faster than you remove it from the
> data-receive register."

Detection: `opus:8-serial.c12`, `sol:8-serial.c12`.
Definition: `opus:8-serial.c13`, `sol:8-serial.c13`.

**Testable form:** with the previous received word left unacknowledged,
receiving a second word sets a distinct overrun indication that a normally
cleared receive does not set; the condition that triggers it is exactly "a
new word completed while the old one was still marked unread."

### A9/A10/A11. The three status bits named on p.255

> "There are also status bits and interrupts for the conditions of receive
> buffer full and transmit buffer empty. An additional status bit is
> provided that indicates 'all bits have been shifted out'."

Receive-buffer-full: `sol:8-serial.c14`.
Transmit-buffer-empty: `sol:8-serial.c16`.
All-bits-shifted-out: `opus:8-serial.c16`, `sol:8-serial.c18`.

**Testable form:** each is a distinct, independently observable status
indication — receive-buffer-full follows a completed receive, transmit-
buffer-empty follows a completed transfer into the shift register, and
all-bits-shifted-out follows the shift register itself finishing (a stronger
condition than transmit-buffer-empty, since more than one word may be queued
between them — see D/TX sections).

### A12. Rate is controlled by SERPER's contents

> "The rate of transmission (the baud rate) is controlled by the contents of
> the register named SERPER."

Claim: `opus:8-serial.c18`.

**Testable form, dimensionless:** this does not require an absolute
bits/second figure — only that changing SERPER's contents changes the
transmitted bit-cell duration, and that no other register does. Two
distinct SERPER values must produce two distinct, measurable bit-cell
lengths on the same transmitted data.

### A13. Bits 14-0 of SERPER are the divider

> "Bits 14-0 of SERPER are the baud-rate divider bits."

Claim: `opus:8-serial.c19`.

**Testable form:** every one of the fifteen low bits participates in the
bit-cell length (confirmed at the extremes: the all-zero divisor and the
all-fifteen-bits-set divisor produce different, correctly-ordered cell
lengths), and bit 15 does not — the same fifteen-bit value with bit 15 set
and clear produces the *same* bit-cell length (bit 15 is shown elsewhere,
A3/A4, to select the receive-length mode instead).

### A14. The divisor formula's shape — partially constructible

> "Thus SERPER=(3,579,545/baud)-1."

Claim: `opus:8-serial.c25`.

The formula names an absolute frequency (3,579,545) that is not
constructible here (hazard 3). What *is* constructible is the formula's
*shape*, independent of which frequency: rearranged, it says bit-cell
duration is directly proportional to (divisor+1), with no additive offset
beyond that. This is graded in full, on the dimensionless relationship
alone — no frequency value appears in the assertion or its oracle.

### A15. The worked example — NOT CONSTRUCTIBLE

> "For example, the proper SERPER value for 9600 baud on an NTSC machine is
> (3,579,545/9600)-1=371."

Claim: `opus:8-serial.c28`.

Names an absolute baud rate on a clock domain (`ntsc`) that this design does
not route to `paula_uart` at all (hazard 3, doubly: no frequency, and the
NTSC/PAL selector itself never reaches this module). **Declined and
recorded.** A `note()` reports what *is* measurable — the bit-cell length
this divisor produces, in fixture-native units — without asserting it
equals 9600 baud.

### A16. All timing is on the "color clock" basis — partially constructible

> "All timing is done on the basis of a 'color clock'..."

Claim: `opus:8-serial.c20`.

`paula_uart` has no colour-clock port; its own timing primitive is a
different, faster enable this module does receive. What is graded is the
part reachable at this boundary: that the module's bit-cell length is always
a whole multiple of a fixed small unit (i.e., counted in matched pairs, not
odd single ticks) for every divisor value tested — consistent with, but not
by itself proof of, the manual's stronger claim that the unit is specifically
one colour clock (which would require observing a colour-clock signal this
module does not expose). **Declined and recorded** for the colour-clock
identification specifically; the whole-multiple structure is graded
untagged.

### A17/A18. N+1 color clocks between samples — partially constructible

> "If the SERPER divisor is set to the number N, then N+1 color clocks occur
> between samples of the state of the input pin (for receive) or between
> transmissions of output bits (for transmit)."

Receive: `opus:8-serial.c23`. Transmit: `opus:8-serial.c24`.

Same boundary limit as A16: the *proportional* relationship ("interval scales
as N+1, with the same small fixed unit per step") is fully constructible and
graded on each side (receive and transmit) independently, since the two
sides use separate counters in this module. The identification of that fixed
unit with one colour clock specifically is outside this boundary and
declined, same as A16.

### A19. Receive expects exactly one start bit

> "the receive circuitry expects to see one start bit, eight or nine data
> bits, and at least one stop bit."

Claims: `opus:8-serial.c37`, `sol:8-serial.c22`.

**Testable form:** a transition that looks like the start of a start bit but
does not persist through the sampling point (a glitch) is rejected — no
frame is registered from it — while a genuine, held start bit is accepted.
This is the same measurement basis both readers' wording points at (start
bit *detection*, not merely its presence in a diagram).

### A20. Data-bit count matches the selected mode, compound (opus only)

> same sentence as A19, both halves together: "...eight or nine data
> bits..." read against the two receive modes at once.

Claim: `opus:8-serial.c38`.

Graded as a conjunction: in 8-bit mode exactly 8 data bits are captured, and
in 9-bit mode exactly 9 are, under the same measurement used for A3/A4 but
asserted here as one compound fact about both modes together, matching how
this claim states it.

### A21. At least one stop bit — the frame includes a stop-bit slot

> "...and at least one stop bit."

Claims: `opus:8-serial.c39`, `sol:8-serial.c25`.

**Testable form:** the receive frame's fixed timing structure includes one
bit-cell's worth of time after the last data bit before the received word
is signalled ready — i.e. there genuinely is a designated stop-bit slot in
the frame, distinct from the data bits themselves. What this fixture cannot
show, and does not claim to: whether the hardware *rejects* a frame whose
stop-bit slot does not read 1. The manual states what the circuitry
"expects" as a protocol description, not a promise of framing-error
detection (framing errors are never named anywhere in this chapter, unlike
overrun, which gets its own status bit and its own paragraph). The slot's
existence is what is graded; whatever value arrives in it is reported by
`note()`, not judged by `expect()`.

---

## B. SERPER's own behaviour (p.256)

### B1. SERPER bit 15 selects receive mode, with its stated polarity

> "Receive mode is set by bit 15 of the write-only SERPER register. Bit 15
> is a 1 if you chose nine data bits for the receive-register full signal,
> and a 0 if you chose eight data bits."

Claims: `opus:8-serial.c40` (bit 15 selects mode), `opus:8-serial.c42` (1 =
nine), `opus:8-serial.c43` (0 = eight).

**Testable form:** same measurement as A3/A4/A20, but graded here
specifically against the stated bit position and its two named polarities,
each cited on its own line since each is its own sentence-level fact.

### B2. SERPER is write-only

> "...the write-only SERPER register."

Claim: `opus:8-serial.c41`.

**Testable form, per hazard 1:** a value written to SERPER reaches state
with an observable effect (the bit-cell length changes, section A12/A13's
own measurement basis), and — measured last, in a harness instance nothing
downstream depends on, per hazard 1's own warning — addressing SERPER for a
"read" does not return the value most recently written there.

### B3/B4/B5. SERDATR is 16 bits, holds data, holds status

> "The serial input data-receive register is 16 bits wide. It contains the
> 8 or 9 bit input data and status bits."

16 bits wide: `opus:8-serial.c45`. Holds the data: `opus:8-serial.c46`.
Holds status too: `opus:8-serial.c47`.

**Testable form:** a read of SERDATR after a receive returns a 16-bit word
whose low bits carry the data just received and whose high bits carry a
distinguishable status pattern that changes with UART state independent of
the data value.

### B6. Data shifts in serially

> "The data is received, one bit at a time, into an internal
> serial-to-parallel shift register."

Claim: `opus:8-serial.c48`.

**Testable form:** the received register only reflects a complete word once
every bit of a full frame has arrived; a partially-sent frame produces no
premature result.

### B7/B8. Transfer to SERDATR and the ready signal

> "When the proper number of bit times have elapsed, the contents of this
> register are transferred to the serial data read register (SERDATR)... and
> you are signaled that there is data ready for you."

Transfer: `opus:8-serial.c49`. Signal: `opus:8-serial.c50`.

**Testable form:** completion of a frame both (a) updates the readable
register with the received content and (b) raises a distinct ready
indication at the same event, not before it.

### B9. Shift register ready again immediately

> "Immediately after the transfer of data takes place, the receive shift
> register again becomes ready to accept new data."

Claim: `opus:8-serial.c52`.

**Testable form:** two frames sent back-to-back with no idle gap between
them are both received correctly (the second is not lost or corrupted by
starting immediately after the first).

### B10/B11. The receiver-full deadline and its size

> "After receiving the receiver-full interrupt, you will have up to one full
> character-receive time (8 to 10 bit times) to accept the data and clear
> the interrupt."

Deadline exists: `opus:8-serial.c53`. Quantified 8-10: `opus:8-serial.c54`.

**Testable form:** measure the character-receive time directly, in bit
cells, for both receive modes, and compare the measured count against the
printed range. This is graded, not declined — it needs no absolute
frequency, only a bit-cell count, and the printed figure is a concrete,
falsifiable number of bit times.

### B12. Overrun set if not cleared in time

> "If the interrupt is not cleared in time, the OVERRUN bit is set."

Claim: `opus:8-serial.c55`.

Same measurement basis as A7/A8: leaving a completed receive unacknowledged
through a second full frame sets the overrun indication.

### B13. SERDATR bit 15 — OVRUN

> "15 OVRUN OVERRUN (Mirror...) Indicates that another byte of data was
> received before the previous byte was picked up by the processor."

Claims: `opus:8-serial.c59` (name), `opus:8-serial.c60` (function label),
`opus:8-serial.c62` (semantics).

Graded together — a bit's name and function label are confirmed exactly by
observing that this bit position, and no other, carries the overrun
semantics already established in A7/A8/B12.

### B14. SERDATR bit 14 — RBF

> "14 RBF READ BUFFER FULL (Mirror...) When this bit is 1, there is data
> ready to be picked up by the processor."

Claims: `opus:8-serial.c65` (name), `opus:8-serial.c68` (semantics).

Graded together, same pattern as B13, against the ready-signal semantics
established in B7/B8/A9.

---

## C. SERDATR's remaining bits (p.257)

### C1. Bit 13 — TBE

> "13 TBE TRANSMIT BUFFER EMPTY (Not a mirror...) When bit 14 is a 1
> [sic — see below], the data in the output data register (SERDAT) has been
> transferred to [the shift register, ready] to accept another output word.
> ... This is also true when the buffer is empty."

Claims: `opus:8-serial.c70` (name), `opus:8-serial.c71` (function label),
`opus:8-serial.c74` ("also true when the buffer is empty").

The printed text itself is internally inconsistent here — "when bit 14 is a
1" describing bit 13's own condition is very likely this table's own
misprint (bit 14 is RBF, an unrelated status), and the sentence has a
visible dropped clause ("...transferred to to accept..."). This is a fact
about the manual, not something this package adjudicates; the *testable*
content of the three claims — that a bit named TBE exists, that its
function is "transmit buffer empty", and that this indication also holds
before anything has ever been written (an empty buffer, not just a buffer
that has been emptied by transmission) — is graded directly: the indication
reads asserted both immediately after a completed transfer and at idle,
before any SERDAT write has ever occurred.

### C2. Bit 12 — TSRE

> "12 TSRE TRANSMIT SHIFT REGISTER EMPTY When this bit is a 1, the output
> shift register has completed its task, all data has been transmitted, and
> the register is now idle."

Claims: `opus:8-serial.c76` (name), `opus:8-serial.c77` (function label),
`opus:8-serial.c78` (semantics).

**Testable form:** this indication reads clear from the moment a
transmission is triggered until the last bit has actually left the shift
register, and reads set both before any transmission has occurred and again
once one completes.

### C3. Bit 12 across two back-to-back writes

> "If you stop writing data into the output register (SERDAT), then this bit
> will become a 1 after both the word currently in the shift register and
> the word placed into SERDAT have been transmitted."

Claim: `opus:8-serial.c79`.

**Testable form:** write two words one bit-cell apart (the second landing
while the first is still shifting). The shift-register-empty indication
must stay clear through both words' worth of bit cells and only set after
the *second* one finishes — not after the first.

### C4. Bit 11 — RXD, "direct read of the pin"

> "11 RXD Direct read of RXD pin on Paula chip."

Claim: `opus:8-serial.c81`.

**Testable form:** change the input pin's level and observe how promptly
this bit reflects the new level, compared against a genuinely combinational
read elsewhere at the same register boundary (used as the control — same
measurement basis, known-opposite expected latency). "Direct" is graded on
its ordinary reading: an unsynchronized, same-cycle reflection of the pin.
Whatever the fixture measures is reported as measured; this is exactly the
kind of RTL-boundary question this package exists to answer honestly, since
this module is shared Minimig lineage and not NanoMig-specific (see the
build brief's note on that point).

### C5. Bit 10 — unused

> "10 Not used at this time."

Claim: `opus:8-serial.c83`.

**Testable form:** this bit position reads a fixed value (0) regardless of
any stimulus applied elsewhere in the register.

### C6/C7/C8/C9/C10. Bit 9 and bit 8's two meanings

> "9 STP Stop bit if 9 data bits are specified for receive. 8 STP Stop bit
> if 8 data bits are specified for receive. OR DB8 9th data bit if 9 bits
> are specified for receive."

Bit 9 (name+semantics): `opus:8-serial.c84`, `opus:8-serial.c85`.
Bit 8 as STP in 8-bit mode: `opus:8-serial.c86`, `opus:8-serial.c87`.
Bit 8 as DB8 in 9-bit mode: `opus:8-serial.c88`, `opus:8-serial.c89`.
The two meanings are exclusive alternatives: `opus:8-serial.c90`.

**Testable form:** in 8-bit receive mode, bit 8 of the read word carries the
stop-bit-slot's sampled value (section A21) and bit 9 is unused by that
mode's data; in 9-bit receive mode, bit 8 instead carries the 9th data bit
and bit 9 carries the stop-bit-slot's value. Both modes are exercised with
known, distinguishing stimuli so the two meanings are shown to depend on the
mode bit (B1), never on both being present together.

### C11/C12. Bits 7-0 — DB7-DB0

> "7-0 DB7-DB0 Low 8 data bits of received data. Data is TRUE (data you read
> is the same polarity as the data expected)."

Name: `opus:8-serial.c91`. Carries the low 8 data bits: `opus:8-serial.c92`.
Polarity: `opus:8-serial.c93`.

**Testable form:** a known byte presented on the receive line is read back
bit-for-bit identical (not inverted) at this position.

---

## D. How output data is transmitted (p.258)

### D1. Writing SERDAT sends data; SERDAT is write-only

> "You send data out on the transmit lines by writing into the serial data
> output register (SERDAT). This register is write-only." / p.259: "The data
> to be transmitted is placed in the output register (SERDAT)."

Write path: `opus:8-serial.c103`, `opus:8-serial.c134`.
Write-only: `opus:8-serial.c104`.

**Testable form:** same pattern as B2 (SERPER) applied to SERDAT — a write
produces an observable transmitted effect, and (measured last, per hazard 1)
addressing SERDAT for a "read" does not return the value most recently
written.

### D2. Same rate as established for receive; no separate transmit divisor

> "Data will be sent out at the same rate as you have established for the
> read."

Claim: `opus:8-serial.c105`.

**Testable form:** the same SERPER value that sets the receive bit-cell
length (A12/A13) also sets the transmit bit-cell length — there is exactly
one divisor register governing both directions, not two.

### D3. Transmission begins immediately, at the selected rate

> "Immediately after you write the data into this register, the system will
> begin the transmission at the baud rate you selected." / "At the start of
> the operation, this data is transferred from SERDAT into an internal
> serial shift register."

Claims: `opus:8-serial.c106`, `opus:8-serial.c107`.

**Testable form:** the start bit appears within a small, bounded number of
cycles after the write (not an arbitrary later time), and the bits that
follow match the word just written, confirming the transfer into the shift
register actually happened at that point.

### D4. SERDAT can accept new data once the transfer completes; TBE signals it

> "When the transfer to the serial shift register has been completed,
> SERDAT can accept new data; the TBE interrupt signals this fact."

Claims: `opus:8-serial.c108`, `opus:8-serial.c109`.

**Testable form:** a second write issued as soon as the transmit-buffer-
empty indication (C1) reads set is accepted and correctly transmitted after
the first word finishes — the TBE indication is what predicts write
acceptance, not an arbitrary fixed delay.

### D5. Shift-out timing, LSB first, one bit per interval

> "Data will be moved out of the shift register, one bit during each time
> interval, starting with the least significant bit."

Claims: `opus:8-serial.c110`, `opus:8-serial.c111`, and the two figure
readings of the same fact: `opus:8-serial.c120` ("the bit leaving the
register is the one that has been shifted out", Figure 8-11), and
`opus:8-serial.c128` ("data gets shifted out this way", pointing to the
LSB end, Figure 8-12).

**Testable form:** the transmitted bit sequence on the output pin matches
the written word's bits in order starting from bit 0, at a constant interval
per bit (the same interval measured in section A).

### D6. Shifting continues until all 1 bits are shifted out; the stop condition

> p.258: "The shifting continues until all 1 bits have been shifted out."
> p.259: "The register stops shifting and signals 'shift register empty'
> (TSRE) when there is a 1 bit present in the bit-shifted-out position and
> the rest of the contents of the shift register are 0s."

Claims: `opus:8-serial.c112` (p.258 statement), `opus:8-serial.c130` (the 1
in the shifted-out position), `opus:8-serial.c131` ("rest... are 0s", both
conditions together), `opus:8-serial.c132` (TSRE signals it).

**Testable form:** a word with a 1 at some middle position and further 1s
above it does *not* stop the shift at the middle 1 — shifting only stops
once the *last* 1 (nothing nonzero left above it) has left the register,
confirmed by testing a pattern where a naive "stop at the first 1" reading
would give the wrong length.

### D7. Shifting resumes on new nonzero contents

> "When new nonzero contents are loaded into this register, shifting begins
> again."

Claim: `opus:8-serial.c133`.

**Testable form:** after a transmission completes and the shift register is
idle, writing a fresh nonzero word starts a new transmission.

### D8. Any combination of data and stop bits; SERDAT controls format

> "Any number or combination of data and stop bits may be specified this
> way." / "SERDAT is a 16-bit register that allows you to control the format
> (appearance) of the transmitted data."

Claims: `opus:8-serial.c113`, `opus:8-serial.c115`, and the figure's own
statement of the 8-data-bit field: `opus:8-serial.c125` ("seven zeros"
above the data field, Figure 8-12), `opus:8-serial.c126` (bit 8 drawn as 1,
the stop bit immediately above the data field), `opus:8-serial.c127` (the
8-bit bracketed data field itself).

**Testable form:** several distinct bit patterns (differing in the position
of stop bits, not only in the data payload) each produce their own,
correctly-shaped transmitted frame — demonstrating the format is fully
programmer-controlled, not fixed to one layout.

### D9. SERDAT is 16 bits wide; the figure's bit span

> "SERDAT is a 16-bit register..."

Claims: `opus:8-serial.c114`, and the figure reading of the same span and
MSB-left order: `opus:8-serial.c118` (Figure 8-11's ruler, 15 down to 0).

**Testable form:** a word using the top bit (bit 15) shifts out in full,
proving all sixteen bits are live, not just the low byte.

### D10. Figure 8-12's ruler — same span and order as Figure 8-11

Claim: `opus:8-serial.c124`.

Same testable form and same measurement as D9; cited on its own line because
it is its own sentence about a second figure, not a restatement of D9's own
uid.

### D11. The typical frame the figures show — partially constructible

> "To form a typical data sequence, such as one start bit, eight data bits,
> and one stop bit, you write into SERDAT the contents shown in Figures 8-12
> and 8-13." [The printed page numbers the figures 8-11 and 8-12; the text
> layer disagrees by one, per this claim's own crossread note. That is a
> fact about the manual's 1999 re-edit, not about the hardware, and is
> reported below rather than graded.]

Claim: `opus:8-serial.c116`.

**Testable form, the graded half:** the frame the figures actually depict —
one start bit, eight data bits, one stop bit — is produced by writing the
corresponding SERDAT contents (the same case exercised in A5). **Declined
and recorded, the ungraded half:** the figure-numbering mismatch itself; no
port of this fixture carries a figure number.

### D12. Starting appearance — all-zero register

> "All zeros from last shift" / Figure 8-11's caption, "Starting Appearance
> of SERDAT and Shift Register."

Claim: `opus:8-serial.c119`.

**Testable form:** graded together with E2 below (the all-zero-write case) —
before any nonzero word has been written, the internal state this figure
depicts is genuinely all-zero, consistent with "no transmission is running."

---

## E. Specifying the register contents (p.259)

### E1. Stop bits are 1s placed above the data bits

> "Above the data bits, 1 bits must be added as stop bits."

Claim: `opus:8-serial.c135`.

**Testable form:** graded together with C6-C10 and D8 — the bit positions
documented as stop-bit slots, when set to 1, produce the frame lengths those
sections already establish.

### E2. Normally one or two stop bits — the fixture-hazard-3 claim

> "Normally, either one or two stop bits are sent."

Claim: `opus:8-serial.c136`.

**This is the slice's own named trap.** A fixture that reads the level
pattern on the transmit pin cannot distinguish one stop bit from two — both
end in a run of 1-cells indistinguishable from idle. This is graded instead
by *total frame length in bit cells*, measured through the transmit-shift-
register-empty transition (section C2/D6): a word whose highest 1 sits at
the one-stop-bit position produces one measured length; a word whose highest
1 sits one position higher (the two-stop-bit case) produces a measurably
longer one. The two cases must differ by exactly one bit cell, and the
comparison must be made on the completion-event timing, never on reading
the pin's level pattern by itself.

### E3/E4. The start bit is independent of SERDAT and always exactly one

> "The transmission of the start bit is independent of the contents of this
> register. One start bit is automatically generated before the first data
> bit (bit 0) is sent."

Claims: `opus:8-serial.c137`, `opus:8-serial.c138`.

**Testable form:** the first transmitted bit cell is always 0 (the start
bit) regardless of what value bit 0 of the written word holds — including
the case where bit 0 is itself 1, so the start bit is not merely "whatever
bit 0 happens to be."

### E5/E6. Writing SERDAT starts transmission; all-zero writes do not

> "Writing this register starts the data transmission. If this register is
> written with all zeros, no data transmission is initiated."

Positive half: `opus:8-serial.c139`. Negative half: `opus:8-serial.c140`.

**Testable form:** E5 is the ordinary positive case — a nonzero write
starts a transmission (start bit followed by the written pattern). E6 is
graded independently and directly: write all zeros and observe whether any
transmission activity (a start bit, or any deviation from idle) occurs on
the output pin at all. **This is the same printed sentence the slice's own
candidates file names for the RTL's most-anticipated red in this chapter**
(a red there is a statement about the Minimig line generally, not only this
board — see the build brief's note on `src/minimig-aga` being shared
lineage). This package grades its own uid on its own measurement rather than
importing that verdict, and reports independently what it finds.

---

## Claim inventory — accounting for all 106

Every uid listed above is graded or declined exactly once; the table below
is the cross-check, uid count by disposition:

| disposition | count |
|---|---:|
| graded in full (`expect()`, own or shared label) | 97 |
| declined, not constructible, no partial credit — absolute bps/baud (A1 x4, A15 x1) | 5 |
| declined, partially graded — dimensionless half graded on an untagged `expect()`, uid on `record()` only (A16, A17, A18) | 3 |
| declined, corpus/printing observation — frame half graded via A5's measurement, figure-numbering half recorded (D11) | 1 |
| **total** | **106** |

A16/A17/A18 and D11 are each a single uid whose claim splits between an
untagged graded measurement and a `record()`-only disposition for the part
this boundary cannot reach — consistent with the build brief's rule that a
partially-gradeable claim's uid lives on the `record()` line alone, never on
the `expect()` line.

## What this specification deliberately does not contain

Port names, signal names, concrete stimulus/expected register values, and
the literal `// MUTATION` records — step-3 material, added only once this
file is committed and `test_ch8-serial-interface.cpp` is written against the
RTL already read (and disclosed above) for this purpose.
