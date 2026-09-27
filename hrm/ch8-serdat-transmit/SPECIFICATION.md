# Specification — `ch8-serdat-transmit`

Package: `hrm/ch8-serdat-transmit`
Fixture: `paula_uart` (compiled alone; port names and any submodule identity
are step-2 material, fixed only after this specification is committed — see
"Disclosure" below for the one exception this package cannot avoid).
Claims file:
`hrm-testpipeline/work/ch8-s4-build/ch8-serdat-transmit.jsonl`
(143 claims, all `classify.status: testable`)
Subject: printed pages 258, 259, 283 and 289 — "How Output Data Is
Transmitted" and "Specifying the Register Contents" (pp.258-259, with
Figures 8-11 and 8-12), the SERDAT row of the Appendix A register table
(p.283), and the SERDAT row of the Appendix B register-summary table
(p.289).

This file is written from the `extract.quote`, `extract.page` and
`extract.restatement` fields of the 143 claims, cross-read against the
primary source text for pages 258, 259 and 283
(`docs/references/amiga-hrm-3rd/ch8/p258.txt`, `p259.txt`,
`docs/references/amiga-hrm-3rd/appA-controller/p283.txt`) and the printed
page images for Figures 8-11 and 8-12
(`docs/references/amiga-hrm-3rd/ch8/figures/p258.png`, `p259.png`, opened
directly with the Read tool — the image IS the page for those two figures)
and for page 289 (`docs/references/amiga-hrm-3rd/appB3/p289.txt`). It fixes
the printed propositions to be tested; port names, concrete stimulus
encodings and the `expect()` calls themselves are added only after this file
is committed and step 2 (opening the RTL) begins, per the build brief.

## Scope — this is a shared-lineage boundary, not a NanoMig-only one

**Boundary: `paula_uart`, in `nanomig/src/minimig-aga/`.** Per the build
brief, `paula_uart` (like `paula`, `ciaa`, `agnus_copper`, `denise_colortable`
and `minimig`) exists in `MiSTer-devel/Minimig-AGA_MiSTer` too, so a red
result here is a statement about the Minimig line, not only about this board.
Slice s3's standing qualifier — that a red is carved out as NanoMig-specific
because the layer under test has no upstream analogue — **does not apply to
this package and is deliberately not repeated here**.

**Two fixture hazards apply to every stimulus below**, both already measured
by the classify stage and named in the build brief and package list, repeated
here because they shape how every `expect()` in step 2 must be written:

1. **`paula_uart` has no read/write strobe and no chip select.** Presenting
   an address on `rga_i` alongside `data_i` writes it, unconditionally.
   Addressing SERDAT ($030) or SERPER ($032) "to see what is there" is
   itself a write and corrupts that register. Every measurement below reads
   status only through SERDATR ($018, a genuinely defined read address) or
   through `txd`, never by "reading" $030 or $032.
2. **A stop bit and the idle line are both a `1` on `txd`.** `0x0155`
   (one stop bit) and `0x0355` (two stop bits, one bit higher) print an
   identical *level* pattern on `txd` — they differ only in *when* `TSRE`
   (SERDATR bit 12) asserts, 104 ticks against 112 for one worked SERPER
   value. Any proposition below that turns on stop-bit *count* is measured
   through `TSRE`'s timing, never through reading levels off `txd` — a test
   that reads the level pattern for that purpose cannot fail and proves
   nothing.

## What this package does and does not grade at this boundary

**SERDATR and SERPER are not this package's registers.** `ch8-serdatr-read-word`
grades SERDATR's own bit format and `ch8-serper-baud-divider` grades SERPER's
own bit format, each as a sibling package against the same claims corpus.
Several propositions below (baud-rate sharing, TBE timing, TSRE timing) can
only be observed by reading SERDATR or by writing SERPER first — that is
using those registers as a measurement instrument for a *SERDAT* claim, not
grading SERDATR's or SERPER's own printed rows. No uid whose printed subject
is SERDATR's or SERPER's own bit table appears in this package's claims file,
and none is graded here.

## Claim inventory (143 of 143 accounted for, 32 propositions)

| proposition | page(s) | uids | grading |
|---|---|---:|---|
| WRITE-SENDS | 258 | 1 | full |
| WRITEONLY | 258, 283 | 3 | full |
| ADDR030 | 283 | 4 | full |
| FUNCTION-BUFFER | 259, 283 | 6 | full |
| RATE-SHARED | 258 | 1 | full |
| RATE-SELECTED | 258 | 1 | full |
| IMMEDIATE-START | 258 | 1 | full |
| DOUBLE-BUFFER | 258, 283 | 3 | full |
| TBE-ON-FREE | 258, 283 | 4 | full |
| LSB-FIRST | 258 | 2 | full |
| STOP-AT-LAST-ONE | 258, 259 | 3 | full |
| ARBITRARY-COMBO | 258 | 1 | full |
| WIDTH16 | 258 | 1 | full |
| CONTROLS-FORMAT | 258 | 1 | full (umbrella; graded by the same measurement as BITMAP/LENGTH) |
| TYPICAL-1-8-1 | 258 | 3 | full |
| STARTBIT-AUTO | 259 | 2 | full |
| STARTBIT-INDEP | 259 | 1 | full — **control case** |
| WRITE-STARTS-TX | 259 | 1 | full — **control case** |
| ALLZERO-NO-TX | 259 | 1 | full — **red expected (S4-C1)** |
| RESTART-ON-RELOAD | 259 | 1 | full |
| STOPBITS-ABOVE-DATA | 259 | 1 | full |
| NORMAL-STOPBITS | 259 | 2 | full — measured via TSRE timing, not levels |
| STOPBIT-REQUIRED | 283 | 2 | full |
| LENGTH-BY-STOPBIT-POSITION | 283 | 2 | full |
| BITMAP | 283 | 60 | full — one sweep, many uids |
| FIG11-ALLZERO-BITS | 258 | 11 | full — via TSRE, not internal-signal inspection |
| FIG11-TRAILING-ONE | 258 | 2 | full |
| FIG11-ARROW-DIRECTION | 258 | 1 | full |
| FIG12-DATABITS | 259 | 8 | full |
| FIG12-STOPBIT-AT-8 | 259 | 1 | full |
| FIG12-UPPER-ZERO | 259 | 7 | full |
| FIG12-ARROW | 259 | 1 | full |
| ALREADY-GRADED-P289 | 289 | 4 | **not graded — `record()` citing the accepted package** |

139 uids graded in full (their own uid appears on a graded `expect()` line
that tests exactly what they assert — several propositions share one
`expect()` across many uids where the claims are independently-sourced
restatements of the identical printed fact, matching the practice of sibling
package `ch8-keyboard-serial-register`). 4 uids (the p.289 SERDAT row cells
already graded by slice s1's `hrm/ch8-appb-serial-register-rows`) are
declined on a `record()` line per the build brief's rule against re-grading
accepted coverage. **No claim in this package loses its home.**

The full uid membership of every proposition above is fixed in
`scratchpad/build-serdat/buckets.json`, produced by a validated pass over
the 143-claim file before this document was written (every one of the 143
uids appears in exactly one proposition, checked by script, zero missing,
zero duplicated). The prose below cites the representative uid(s) for each
proposition's own text; the C++ source's labels carry the complete uid list
per assertion.

## The propositions

### WRITE-SENDS (p.258, `sol:8-serdat.c1`)

> You send data out on the transmit lines by writing into the serial data
> output register (SERDAT).

**Testable:** writing SERDAT causes activity on the transmit output — the
register is a live vehicle for outgoing data, not a dead port.

### WRITEONLY (p.258 `sol:8-serdat.c2`; p.283 `opus:8-serdat.c3`, `sol:8-serdat.c39`)

> This register is write-only. / SERDAT 030 W P ...

**Testable, on the one half this boundary can show:** writing $030 is
accepted (data reaches the transmit path). What "write-only" positively
forbids — a defined *read* function at $030 — cannot be tested by "reading"
$030, because addressing it at all is itself a write (fixture hazard #1
above); there is no read/write strobe to hold `rd`-only. This proposition is
graded on the writable half; a decline note explains why the "only" half is
not separately asserted, so no assertion here can fail for a reason other
than the one it names.

### ADDR030 (p.283 `opus:8-serdat.c1`, `opus:8-serdat.c2`, `sol:8-serdat.c38`, `sol:8-serdat.c95`)

> SERDAT 030 W P Serial port data and stop bits write (transmit data buffer)

**Testable:** $030 is SERDAT's own address — writing a neighbouring address
(e.g. $032, SERPER) does not produce the same transmit effect, and writing
$030 does.

### FUNCTION-BUFFER (p.259 `sol:8-serdat.c29`; p.283 `opus:8-serdat.c5`,
`opus:8-serdat.c6`, `sol:8-serdat.c41`, `sol:8-serdat.c42`, `sol:8-serdat.c43`)

> The data to be transmitted is placed in the output register (SERDAT). /
> SERDAT ... Serial port data and stop bits write (transmit data buffer). /
> This address writes data to a transmit data buffer.

**Testable:** the value presented at the write is exactly the value the
transmit path subsequently shifts out (content fidelity, graded together
with LSB-FIRST and BITMAP below, which is where content-position is
actually checked bit by bit).

### RATE-SHARED (p.258 `sol:8-serdat.c3`)

> Data will be sent out at the same rate as you have established for the
> read.

**Testable:** the bit-cell width the transmit path uses is governed by the
same SERPER value the receive path uses — i.e. one baud-rate register, not
two. Measured by comparing the transmit bit-cell width (in `clk7_en` ticks)
against the sibling `hrm/ch8-uart-frame-and-status` fixture's own already-published
receive-side calibration for the same SERPER value (its
`test_paula_uart.cpp:87-88`, 22 `clk7_en` enables per bit for SERPER=10) — a
second, independent measurement of the same SERPER value on this package's
own transmit side, not a re-use of that package's result as this package's
evidence.

### RATE-SELECTED (p.258 `sol:8-serdat.c5`)

> Immediately after you write the data into this register, the system will
> begin the transmission at the baud rate you selected.

**Testable:** changing SERPER changes the transmit bit-cell width
proportionally (a second SERPER value, distinct from RATE-SHARED's, produces
a distinct and correctly-related tick count).

### IMMEDIATE-START (p.258 `sol:8-serdat.c4`)

> Immediately after you write the data into this register, the system will
> begin the transmission at the baud rate you selected.

**Testable:** no separate "go" trigger exists — the write itself starts
shifting, observable within a small, fixed number of ticks of the write,
not only after some unrelated later event.

### DOUBLE-BUFFER (p.258 `sol:8-serdat.c6`; p.283 `opus:8-serdat.c7`, `sol:8-serdat.c44`)

> At the start of the operation, this data is transferred from SERDAT into
> an internal serial shift register. / Data from this buffer is moved into a
> serial shift register for output transmission whenever it is empty.

**Testable:** the transfer from the holding register into the actively
shifting register happens promptly at the start of the operation — this is
what makes SERDAT a *buffer* distinct from the shift register itself, not a
single combined register with no double-buffering at all.

### TBE-ON-FREE (p.258 `sol:8-serdat.c7`, `sol:8-serdat.c8`; p.283
`opus:8-serdat.c8`, `sol:8-serdat.c45`)

> When the transfer to the serial shift register has been completed, SERDAT
> can accept new data; the TBE interrupt signals this fact. / This sets the
> interrupt request TBE (transmit buffer empty).

**Testable:** TBE (SERDATR bit 13, used here purely as a status instrument,
not graded on its own account) asserts promptly after the SERDAT→shift-
register transfer — soon after the write, not only once the entire frame has
finished shifting out. This is the direct behavioural consequence of
DOUBLE-BUFFER: a design without double buffering would only free SERDAT (set
TBE) once the whole frame is gone, which this proposition distinguishes.

### LSB-FIRST (p.258 `sol:8-serdat.c9`, `sol:8-serdat.c10`)

> Data will be moved out of the shift register, one bit during each time
> interval, starting with the least significant bit.

**Testable:** presenting a value with exactly one bit set produces that
single `1` on `txd` at a cell offset equal to the bit's own index — bit 0
appears first, bit 1 second, and so on, not most-significant-bit first and
not some other order. Also grades FIG11-ARROW-DIRECTION and FIG12-ARROW
below, which depict the same shift-out direction pictorially.

### STOP-AT-LAST-ONE (p.258 `sol:8-serdat.c11`; p.259 `sol:8-serdat.c26`,
`sol:8-serdat.c27`)

> The shifting continues until all 1 bits have been shifted out. / The
> register stops shifting and signals "shift register empty" (TSRE) when
> there is a 1 bit present in the bit-shifted-out position and the rest of
> the contents of the shift register are 0s.

**Testable, and the load-bearing mechanism for most of this package:**
shifting terminates exactly when the bit just shifted out was a `1` and
everything still held above it is `0`. Consequence, derivable by hand from
this sentence alone: writing a value with its *highest* set bit at index H
produces a frame of exactly H+1 non-start cells (H zero/data cells for
whatever sits below the top bit, then the terminating `1`), because that is
the first position at which "shifted-out bit is 1 and the rest is 0" can
possibly become true. This H-indexed length rule is the shared measurement
basis for ARBITRARY-COMBO, WIDTH16, TYPICAL-1-8-1, STOPBITS-ABOVE-DATA,
NORMAL-STOPBITS, STOPBIT-REQUIRED, LENGTH-BY-STOPBIT-POSITION, BITMAP, and
the two figures below — one measurement, many restatements of it in the
printed source, decomposed into these separate propositions because they are
separate printed sentences (mirroring `ch8-keyboard-serial-register`'s
PROP-4/PROP-5 split for the identical reason).

### ARBITRARY-COMBO (p.258 `sol:8-serdat.c12`)

> Any number or combination of data and stop bits may be specified this way.

**Testable:** the H-indexed length rule above holds not just at one point but
across a sweep of distinct H values (and distinct sub-H content) — i.e., the
frame length is a free choice of where the top bit sits, not fixed at one
frame shape.

### WIDTH16 (p.258 `sol:8-serdat.c13`)

> SERDAT is a 16-bit register that allows you to control the format
> (appearance) of the transmitted data.

**Testable:** the H-indexed rule holds all the way to H=15 (the register's
top bit) — sixteen, not fewer, distinct bit positions are each individually
observable through a distinct frame length.

### CONTROLS-FORMAT (p.258 `sol:8-serdat.c14`)

> SERDAT ... allows you to control the format (appearance) of the
> transmitted data.

**Testable only as the union of the concrete propositions that follow it**
(BITMAP, LENGTH-BY-STOPBIT-POSITION, ARBITRARY-COMBO): this sentence's own
words name no specific format fact beyond what those already test. Carried
on the same `expect()` as BITMAP; no separate stimulus.

### TYPICAL-1-8-1 (p.258 `sol:8-serdat.c15`, `sol:8-serdat.c16`, `sol:8-serdat.c17`)

> To form a typical data sequence, such as one start bit, eight data bits,
> and one stop bit, you write into SERDAT the contents shown in Figures 8-11
> and 8-12.

**Testable:** the documented typical value (bit 8 set as the sole bit above
an 8-bit data field, matching Figure 8-12 — see FIG12-STOPBIT-AT-8) produces
exactly one start cell, eight data cells, and one stop cell — ten cells
total, TSRE asserting at the tenth.

### STARTBIT-AUTO (p.259 `sol:8-serdat.c34`, `sol:8-serdat.c35`)

> One start bit is automatically generated before the first data bit (bit 0)
> is sent.

**Testable:** every transmitted frame begins with exactly one low cell before
bit 0's own cell appears, generated by the hardware with no corresponding bit
position in SERDAT itself (the H-indexed rule above counts only the H+1
cells *after* the start cell).

### STARTBIT-INDEP (p.259 `sol:8-serdat.c33`) — control case

> The transmission of the start bit is independent of the contents of this
> register.

**Testable, and one of this package's two control cases:** the start cell
appears whatever value is written — including the all-zero case
ALLZERO-NO-TX below claims should transmit *nothing at all*. Read plainly,
this sentence and ALLZERO-NO-TX's sentence describe different outcomes for
the same stimulus (a start bit "independent of contents" versus "no data
transmission" for all-zero contents); this specification does not adjudicate
between them and grades each against its own printed sentence, per the build
brief's rule for source tension. STARTBIT-INDEP is graded on the same
measurement basis (presence of a start-bit low cell) as ALLZERO-NO-TX's red,
which is exactly the "control case expected to PASS on the same measurement
basis" the build brief requires for a red to survive the units-error
objection — though it is graded here for its own printed sentence, not
introduced only as a foil.

### WRITE-STARTS-TX (p.259 `sol:8-serdat.c36`) — control case

> Writing this register starts the data transmission.

**Testable, and this package's primary control case:** writing a nonzero
value (the same fixture, same measurement basis as ALLZERO-NO-TX) transmits
— per the candidates file's own framing of S4-C1, this is the control a red
at ALLZERO-NO-TX is measured against.

### ALLZERO-NO-TX (p.259 `sol:8-serdat.c37`) — **red expected**

> If this register is written with all zeros, no data transmission is
> initiated.

**Testable, and expected to fail.** Per finding candidate S4-C1 (already
measured by the classify stage, cited here as the reason this red is
anticipated rather than stumbled on, not as this package's own evidence):
writing `0x0000` to SERDAT still drives a start-bit low cell on `txd`. This
package re-runs that stimulus itself -- another party's measurement does
not substitute for this package's own -- with WRITE-STARTS-TX as the
same-basis control.

### RESTART-ON-RELOAD (p.259 `sol:8-serdat.c28`)

> When new nonzero contents are loaded into this register, shifting begins
> again.

**Testable:** after a first transmission completes (TSRE asserted, no
pending data), writing a *second* nonzero value restarts shifting — the
mechanism is not a one-shot that only ever fires once per fixture lifetime.

### STOPBITS-ABOVE-DATA (p.259 `sol:8-serdat.c30`)

> The data to be transmitted is placed in the output register (SERDAT).
> Above the data bits, 1 bits must be added as stop bits.

**Testable:** the H-indexed rule again — the bits above the data field must
be the ones carrying the terminating `1`; a data field with no `1` above it
does not terminate at the data field's own top (consistent with
ALLZERO-NO-TX's stimulus never producing a terminated frame the way a
correctly stop-bit-terminated one does, though ALLZERO-NO-TX's own defect is
that it transmits *at all*, not a termination-shape difference).

### NORMAL-STOPBITS (p.259 `sol:8-serdat.c31` "one", `sol:8-serdat.c32` "two")

> Normally, either one or two stop bits are sent.

**Testable, and where fixture hazard #2 applies directly:** two stimuli whose
data field is identical and whose highest set bit differs by exactly one
position (one stop bit vs two) must produce `TSRE` assertions exactly one
bit-cell width apart in tick count. This is measured through `TSRE`'s timing
only — the two stimuli's `txd` *level* patterns are expected to be
indistinguishable (stop and idle are both `1`), so no assertion here is
written against a level read.

### STOPBIT-REQUIRED (p.283 `opus:8-serdat.c9`, `sol:8-serdat.c46`)

> A stop bit must be provided as part of the data word.

**Testable:** a data word with *no* `1` anywhere above its data field (i.e.
no bit set at all, the all-zero case) does not behave like a correctly
stop-bit-terminated word — this is the same stimulus ALLZERO-NO-TX grades,
and this proposition is graded on the same measurement, since both printed
sentences describe the identical fixture state (an absent stop bit) from
two pages.

### LENGTH-BY-STOPBIT-POSITION (p.283 `opus:8-serdat.c10`, `sol:8-serdat.c47`)

> The length of the data word is set by the position of the stop bit.

**Testable:** this is ARBITRARY-COMBO's rule stated the other way around —
moving the stop bit's position (the highest set bit) changes the measured
frame length by exactly the corresponding number of cells, swept across
several positions.

### BITMAP (p.283, the BIT# / USE row and its Note — 60 uids, the package's
largest proposition)

> BIT# 15,14,13,12,11,10,09,08,07,06,05,04,03,02,01,00 / USE 0 0 0 0 0 0 S D8
> D7 D6 D5 D4 D3 D2 D1 D0 / Note: S = stop bit = 1, D = data bits.

Sixty uids restate this one table three ways: the BIT# header row naming
each of the sixteen positions (`sol:8-serdat.c79`-`c94`), the USE row's
per-position role (`opus:8-serdat.c11`-`c27`, `sol:8-serdat.c48`-`c54`,
`c56`, `c58`, `c60`, `c62`, `c64`, `c66`, `c68`, `c70`, `c72`), and the Note
line's restatement of the same roles (`opus:8-serdat.c29`,
`sol:8-serdat.c55`, `c57`, `c59`, `c61`, `c63`, `c65`, `c67`, `c69`, `c71`,
`c73`).

**Testable:** every one of the sixteen bit positions is individually
observable (the H-indexed rule, single bit at a time, per STOP-AT-LAST-ONE —
this alone grades "the table includes bit position N" for all sixteen);
positions 0-7 additionally carry content (toggling bit N alone, below a
fixed stop marker above it, moves exactly transmitted cell N and no other —
this grades the "D7"..."D0" roles and FIG12-DATABITS below); position 8 has
a *dual* role, exactly as the printed table's own "D8" together with
SERDATR's printed description of its own bit 8 ("stop bit if LONG, data bit
if not", cited here only to explain *why* two behaviours are expected of the
same SERDAT bit position, not to grade SERDATR's own row) predicts: when bit
8 is the topmost set bit it functions as position 9's stop marker's
neighbour per TYPICAL-1-8-1/FIG12-STOPBIT-AT-8, and when a bit above it
(bit 9) is set instead, bit 8 carries content the same way bits 0-7 do; bit 9
functions as a stop position when set alone (grading "S", "S=1", and "bit 9's
role"); positions 10-15 are "unused" in the documented typical pattern in the
sense that the typical figures leave them `0`, but — per the same H-indexed
rule — are not electrically inert: setting any of them alone still moves the
frame length by exactly its own index, which is what WIDTH16 already
establishes up to bit 15. No claim here asserts that bits 10-15 are
functionally dead, only that the documented typical pattern's own figures
leave them at `0`; FIG12-UPPER-ZERO below grades that figure-specific fact
directly.

### FIG11-ALLZERO-BITS (p.258, Figure 8-11, 11 uids: `sol:8-serdat.c18`
bit 15, `sol:8-serdat.c102`-`c111` bits 9 down to 0)

Figure 8-11 ("Starting Appearance of SERDAT and Shift Register") draws the
register as sixteen `0`s, captioned "All zeros from last shift". Per the
classify stage's own pixel measurement (disclosed below), the figure is
missing one glyph among the unlabelled interior positions (bits 14-10); the
five claims that would rest on those positions are **already excluded from
this package's claims file as `not-asserted`** — this package grades only
the eleven bit positions the figure does label unambiguously (15, and 9
through 0) and does not re-raise the figure's own defect, which is recorded
elsewhere (finding candidate S4-C10) as one figure's fault, not this
package's to re-litigate.

**Testable, without inspecting any internal signal SERDAT's write-only
status makes architecturally unreadable:** "all zero" is definitionally the
state STOP-AT-LAST-ONE's own quoted sentence assigns to `TSRE` asserting (a
`1` was shifted out and the rest is `0`) — so the figure's claim is graded by
confirming `TSRE` (SERDATR bit 12, used here only as an instrument) reads
asserted at the point the figure depicts (after a transmission has completed
and before new data is written), which is the architecturally-observable
consequence of the internal register being what the figure draws. This
covers all eleven uids on one assertion.

### FIG11-TRAILING-ONE (p.258, Figure 8-11, `sol:8-serdat.c19`, `sol:8-serdat.c20`)

The figure draws a one-cell box holding a `1`, one bit-width beyond the
register's low (bit 0) end, labelled "one bit".

**Testable:** `txd` reads `1` (idle) at the same point FIG11-ALLZERO-BITS
grades — the last thing shifted out, still on the line.

### FIG11-ARROW-DIRECTION (p.258, Figure 8-11, `sol:8-serdat.c21`)

A dashed arrow runs from the register's low-order (bit 0) end toward the
one-bit box.

**Testable:** graded by LSB-FIRST above (the same shift-out direction, drawn
pictorially here) — no separate stimulus.

### FIG12-DATABITS (p.259, Figure 8-12, 8 uids: `sol:8-serdat.c22` bit 7,
`sol:8-serdat.c112`-`c118` bits 6 down to 0)

Figure 8-12 ("Ending Appearance of Shift Register") draws bits 7-0 under an
"8 bits data" bracket.

**Testable:** with bit 8 fixed at `1` (the figure's own stop marker, see
FIG12-STOPBIT-AT-8), sweeping bits 7-0 individually each moves exactly one
transmitted cell and none other, and never changes the overall frame length
— the content half of BITMAP's D7-D0 roles, specifically for this figure's
own depicted stimulus shape.

### FIG12-STOPBIT-AT-8 (p.259, Figure 8-12, `sol:8-serdat.c23`)

The figure draws a narrower `1` glyph at bit 8, immediately above the
8-bit data bracket.

**Testable:** this is the concrete stimulus TYPICAL-1-8-1 already grades —
bit 8 set alone above an 8-bit data field produces the documented
1-start/8-data/1-stop shape.

### FIG12-UPPER-ZERO (p.259, Figure 8-12, 7 uids: `sol:8-serdat.c24` bit 9,
`sol:8-serdat.c119`-`c124` bits 10-15)

The figure draws seven `0`s above the stop-bit glyph (bits 15-9). Per the
classify stage's own pixel measurement (cited in the disclosure below),
Figure 8-12, unlike Figure 8-11, is complete: seven zeros, one narrower `1`,
an eight-wide data bracket — 7+1+8=16, no glyph missing. All seven uids here
are graded, none excluded.

**Testable:** for the documented typical stimulus (bit 8 set, bits 0-7 data),
bits 9-15 read `0` in what is written — and, going beyond the figure's own
static drawing to its functional consequence (per STOP-AT-LAST-ONE), setting
any one of bits 9-15 *in addition* changes the measured frame length, which
is what distinguishes "the typical pattern requires these bits at 0" from
"these bits are simply not looked at".

### FIG12-ARROW (p.259, Figure 8-12, `sol:8-serdat.c25`)

A dashed arrow captioned "Data gets shifted out this way" points toward the
register's bit-0 end.

**Testable:** graded by LSB-FIRST above — no separate stimulus.

### ALREADY-GRADED-P289 (p.289, 4 uids: `sol:8-serdat.c74`, `c75`, `c77`,
`c96`) — **not graded**

> SERDAT *030 W P Serial port data and stop bits write

Each of these four uids carries a `classify.note` naming the same fact: this
printed row cell is already graded in slice s1's accepted package
`hrm/ch8-appb-serial-register-rows` (uids `opus:8-appB.c38` / `c39` / `c41`
and `sol:8-appB.c37` / `c38` / `c40`, `out.txt` P4-P6). Per the build brief's
rule against re-grading accepted coverage, each of these four is placed on
its own `record()` line citing that package and those uids, not on a graded
`expect()` line — keeping the claim's home in this slice's ledger while
avoiding a second, possibly divergent, verdict on the same printed row.

## On a control case and mutation

**Fixture liveness.** Before any proposition above is graded, the harness
must drive at least two distinct, known nonzero values through SERDAT and
confirm — via distinct measured frame lengths — that both arrived and did
not collapse to one value or to nothing. This exercises the write-decode and
clocking plumbing only, not any printed proposition.

**Control cases.** Two are named above at the point they are graded:
WRITE-STARTS-TX (a nonzero write transmits, same measurement basis as
ALLZERO-NO-TX) and STARTBIT-INDEP (the start cell appears regardless of
content, graded on its own printed sentence). Both pass at baseline on the
same basis ALLZERO-NO-TX's expected red is measured on, satisfying the build
brief's requirement that a red not stand without a same-basis PASS beside it.

**Mutations.** Every graded `expect()` above gets one stimulus or oracle
mutation, recorded in the C++ source as `// MUTATION <label> <oracle|
stimulus|none> <red|green|impossible> <what changed>`, replayable as a `-D`
variant per the build brief's Makefile pattern. Since STOP-AT-LAST-ONE's
H-indexed rule is believed, from the manual's own text (not from having
opened the RTL), to hold throughout — with the one known exception,
ALLZERO-NO-TX, already anticipated as a red by finding candidate S4-C1 —
every assertion **except** ALLZERO-NO-TX is expected green at baseline, and
each of those mutations is an ordinary "prove the passing assertion is not
vacuous" demonstration, not a `stimulus green` substitution proof.
ALLZERO-NO-TX itself is baseline-red; its own mutation record uses the
`stimulus green` form the build brief describes for exactly that situation
(flipping the stimulus to a value WRITE-STARTS-TX already shows transmits,
demonstrating the fixture is not simply broken). Whether any mutation turns
out `impossible` at this boundary is decided in step 2 once the actual port
list and timing are known, and reported if it happens.

## Disclosure — what is already known, and the limit of it

This specification is written from `extract.quote` / `extract.page` /
`extract.restatement` and the primary page text/images for pp.258, 259, 283,
289. The `classify.evidence` field in the 143-claim source file necessarily
passed through the same parse that read `uid`/page/quote/restatement (they
live in the same JSON records) and could not be avoided entirely; per the
practice already recorded by sibling packages `ch8-keyboard-serial-register`,
`ch8-potgo-register-bit-layout` and `ch8-ciaa-port-direction`, the specific
facts it contributed to this document's scope decisions are named here
rather than left implicit:

- `paula_uart.v:4-17`'s port list (`clk`, `clk7_en`, `reset`, `rga_i`,
  `data_i`, `data_o`, `uartbrk`, `rbfmirror`, `txint`, `rxint`, `txd`,
  `rxd`) and the absence of a read/write strobe or chip select — this is
  also stated directly in the build brief and package list as a standing
  fact about this slice's fixtures, not something this package alone
  found.
- `paula_uart.v:21`, `REG_SERDAT = 9'h030` — used above only to justify that
  ADDR030 is reachable at all, not to supply any expected value.
- `paula_uart.v:100/113/122-123/255` — `tx_shift <= serdat` at load,
  termination on `tx_shift == 16'd0`, right-shift with zero-fill and
  `tx_txd <= tx_shift[0]`, `assign txd = tx_txd`. This **confirms**, after
  the fact, the H-indexed length rule this document derives by hand from
  STOP-AT-LAST-ONE's own printed sentence — it did not supply that rule.
  The rule and every expected value above follow from "shifting continues
  until all 1 bits have been shifted out" and "stops ... when there is a 1
  bit present in the bit-shifted-out position and the rest ... are 0s"
  (p.258/259, quoted in full above) and from the worked H=0, H=9, H=15
  arithmetic any reader can perform by hand from that rule; they would be
  the same regardless of what `paula_uart.v` turns out to contain.
- Finding candidate S4-C1's own measurement (writing `0x0000` still drives a
  start-bit low cell) — handed to this package directly by the task
  assignment and the candidates file, not discovered by this package's own
  classify-stage reading. This package's step 2 re-measures it
  independently rather than transcribing that prior measurement as its own
  evidence.
- Finding candidate S4-C10's own pixel measurement of Figure 8-11 (fifteen
  `0` glyphs for sixteen positions, the deficit in the unlabelled bits
  14-10) and of Figure 8-12 (complete, no glyph missing) — likewise handed
  to this package directly, used above only to explain why
  FIG11-ALLZERO-BITS has eleven uids rather than sixteen and why this
  package does not re-raise the figure's own defect as an RTL finding.

Beyond these, no RTL file has been opened directly by this package before
this specification was committed, and no other reasoning above depends on
anything beyond a printed sentence, a printed figure, and hand arithmetic
performed against it.

## What this specification deliberately does not contain

No exact register-select constant beyond `9'h030` (disclosed above only to
establish reachability), no exact tick counts, no concrete stimulus byte
values beyond the worked H=0/H=9/H=15 arithmetic implied directly by
STOP-AT-LAST-ONE's own printed rule, and no assertion of *why* the RTL
behaves as it does. Every expected value above is computed by hand from a
printed sentence or a printed figure and would be unchanged regardless of
`paula_uart.v`'s actual contents. Concrete SERPER values, exact tick
thresholds for TBE/TSRE promptness, and the `expect()` calls themselves are
step-2 choices, made after this file is committed.
