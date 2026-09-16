# ch8-adkcon-and-interrupt-relations — SPECIFICATION

Written from the quotations in
`hrm-testpipeline/work/ch8-s4-build/ch8-adkcon-and-interrupt-relations.jsonl`
(the 97 claims assigned to this package) and the printed pages they cite —
`ch8/p255.txt`, `p256.txt`, `p257.txt`, `appA-audio/p265.txt`,
`appA-controller/p283.txt`, `appB3/p289.txt`, `appB3/p287.txt` — before any
RTL file is opened by me.

## Order-of-work disclosure

The build brief requires the specification to be written and committed
before `paula.v` (or any other RTL file) is opened, and says plainly: if the
claims file itself hands a builder port names or line numbers before the
spec is written, that compromises the rule through no fault of the builder,
and the builder should say so rather than pretend the order held.

That happened here. Every claim's `classify.evidence` field in this
package's own jsonl already quotes `paula.v` line numbers, port names
(`reg_address_in`, `data_in`, `data_out`, `txd`, `rxd`), and register
addresses ($09E, $010, $09C, $01E, $030, $018) — put there by the classify
stage, not by me, as part of the file I was told to read to get my claims.
I did not independently open `paula.v` before writing this document, and
every testable proposition below is derived from the manual quotations
alone; but I cannot claim not to have seen RTL specifics, because they were
already sitting in the file the build brief pointed me to. Disclosing this
plainly, per the brief's own instruction, rather than presenting the commit
order as clean.

## Scope and disposition of the 97 claims

- **26 claims are already graded in an accepted package** on the same
  printed row. Their `classify.note` says only "subject of slice s2" or (one
  claim) names the package directly; the real citation — which package, and
  under which sibling uid — sits in the `classify.evidence` field's "ALREADY
  GRADED IN SLICE s2/s1: `<uid>` [status]" fragment. I independently
  followed every one of those fragments into the accepted packages
  themselves (not just trusted the fragment) before treating a claim as
  already graded — see "Verification of the 26 already-graded claims" below,
  including three corrections where the fragment's claim did not hold up.
  Each of the 26 goes on a `record()` line citing the package and assertion
  that grades it. **Not re-graded.**
- **4 claims sit on one side of a source conflict** between p.257 and p.283
  about SERDATR bit 13 (candidate S4-C8). Per the build brief: "Measure both
  sides, record both, grade neither." Both sides happen to have claims
  inside *this* package's own 97 (not only in `ch8-serdatr-read-word`, which
  the candidates file names as the conflict's home); I am grading neither
  side, consistent with the brief, and noting that the adjudication itself
  belongs to whichever package's report addresses S4-C8 first.
- **67 claims are graded here**, through 17 `expect()` assertions (one
  citing three claims that the classify stage's "subject of slice s2" note
  flagged as already graded, but which I traced to *no* accepted package —
  see the correction below).

26 + 4 + 67 = 97. Every claim accounted for; none is silently dropped.

## Verification of the 26 already-graded claims, and three corrections

I did not take "subject of slice s2" at face value. For each of the 28
`8-adkcon.cNN` claims carrying that note (plus `sol:8-serdat.c76`, which
names its package directly), I read the sibling slice-s2 uid(s) named in the
claim's own `evidence` field, then searched the accepted packages under
`hrm/` for those exact uids to confirm a real, graded (not merely
`record()`-only, not `not-asserted`) assertion exists.

**23 of the 28 checked out:**

| my claim(s) | sibling s2 uid(s) cited | accepted package | assertion |
|---|---|---|---|
| `opus:8-adkcon.c17`, `c18` (PRECOMP name/width) | `opus:8-appA-adkcon.c11`, `sol:8-appa-adkbit.c3`, `.c33` | `hrm/ch8-adkcon-disk-control-bits` | A3 (PRECOMP-FIELD-POSITION), A4, A5 |
| `opus:8-adkcon.c27`, `c28` (WORDSYNC name/effect) | `opus:8-appA-adkcon.c20`, `sol:8-appa-adkbit.c11` | `hrm/ch8-adkcon-disk-control-bits` | A6 (WORDSYNC-BIT10-EFFECT) |
| `opus:8-adkcon.c29` (DISK SYNC CODE address) | `opus:8-appA-adkcon.c21`, `sol:8-appa-adkbit.c12` | `hrm/ch8-disk-register-map-paula-floppy` | S1-DSKSYNC-ADDRESS |
| `opus:8-adkcon.c35`..`c52` (18 claims: USE3PN..USE0V1 names+"modulate" behaviour, and the period/volume alternation NOTE) | `opus:8-appA-adkcon.c28`..`c36`, `sol:8-appa-adkbit.c22`..`c32` | `hrm/ch5-audio-dmacon-adkcon` | ADKCON-SETCLR-SET (position/storage half) and ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS (behaviour half — **expected/graded RED**: paula_audio_channel.v's own first line says attached modes are not supported, and `paula.v` instantiates `paula_audio` with no `adkcon` port at all) |
| `opus:8-adkcon.c54` (AUDxDAT address/direction/function) | `opus:8-appA-adkcon.c37` | `hrm/ch5-audio-channel-registers` | AUDXDAT-IS-PERCHANNEL-DATA-BUFFER |
| `opus:8-adkcon.c55` (AUDxDAT full paragraph) | `opus:8-appA-adkcon.c38` | `hrm/ch5-audio-channel-registers` | SAMPLE-SIGNED-RANGE-FULL-SCALE, WORD-CONTAINS-TWO-SAMPLES |
| `sol:8-serdat.c76` (SERDAT p.289 CHIP=P) | `opus:8-appB.c40`, `sol:8-appB.c39` (named directly in the note) | `hrm/ch8-appb-chip-column-paula` | section T5 |

That is 2+2+1+18+1+1+1 = 26 claims genuinely already graded. **These go on
`record()` lines and are not re-graded.**

**3 of the 28 did NOT check out — correction to the package list/candidates
file:**

- `opus:8-adkcon.c23` ("ADKCON bit 12 is named MFMPREC") cites sibling uids
  `opus:8-appA-adkcon.c16` and `sol:8-appa-adkbit.c8`, both marked
  `[not-asserted]` in slice s2's own classify output — and confirmed absent
  from `hrm/ch8-adkcon-disk-control-bits`'s own thirteen claims (that
  package's SPECIFICATION.md says explicitly: "no claim in this package's
  thirteen names MFMPREC, MSBSYNC or FAST at all", reporting bit 12 only via
  `note()`).
- `opus:8-adkcon.c30` ("ADKCON bit 9 is named MSBSYNC") — same situation,
  sibling uids `opus:8-appA-adkcon.c22`/`sol:8-appa-adkbit.c13`, both
  `[not-asserted]`.
- `opus:8-adkcon.c32` ("ADKCON bit 8 is named FAST") — same situation,
  sibling uids `opus:8-appA-adkcon.c24`/`sol:8-appa-adkbit.c15`, both
  `[not-asserted]`.

I grepped every `.cpp`/`.md` file under `hrm/` for all six of those sibling
uids: no hits anywhere. **These three claims are not graded in any accepted
package.** They are testable and observable at `paula` per their own
`classify` entries, so I am grading them myself (assertion 17 below) rather
than declining them on a false "already graded" premise.

## The 4 source-conflict claims (S4-C8, SERDATR bit 13)

p.257 (Table 8-9): *"13 TBE TRANSMIT BUFFER EMPTY (Not a mirror —
interrupt occurs when the buffer becomes empty.)"*
p.283 (SERDATR): *"13 TBE Serial port transmit buffer empty (mirror)."*

The two pages give bit 13 opposite roles. This package's 97 claims include
both sides:

- p.257 side: `opus:8-serial.c72`, `sol:8-serdatr.c28`
- p.283 side: `opus:8-serdatr.c18`, `sol:8-serdatr.c98`

Per the build brief: *"Measure both sides, record both, grade neither."*
All four go on `record()` lines carrying a `MEASURED:` note (SERDATR bit 13
against INTREQR bit 0, both directions of the set/clear cycle) but no
`expect()` — grading either side through `expect()` would be asserting which
page is right, which is an adjudication this package does not make. The
formal finding (S4-C8) is the candidates file's, filed against
`ch8-serdatr-read-word`; this package only avoids contradicting it.

## The 17 graded assertions

Each assertion names the page(s) and claim(s) it grades. All are expected
**GREEN** except assertion 8 (S4-C3, a genuine RTL-vs-manual mismatch) and
half of assertion 10 (S4-C6, likewise).

1. **SERIAL-REGS-REACHABLE-AT-PAULA** — p.255 *"The Paula custom chip
   contains a ... UART."* `opus:8-serial.c7`, `sol:8-serial.c5`. The serial
   registers answer at `paula`'s own bus with no separate top level needed.
2. **STATUS-AND-INTERRUPT-EXIST-RBF** — p.255 *"status bits and interrupts
   for ... receive buffer full."* `opus:8-serial.c14`, `sol:8-serial.c15`.
   After a received frame, both a status bit and an interrupt-request bit
   for RBF are observably set.
3. **STATUS-AND-INTERRUPT-EXIST-TBE** — p.255, same sentence, transmit
   half. `opus:8-serial.c15`, `sol:8-serial.c17`. After a SERDAT write, an
   interrupt-request bit for TBE is observably set.
4. **RBF-BIT14-NAME-AND-MIRROR** — p.256 *"14 RBF READ BUFFER FULL
   (Mirror ...)"*, p.283 *"14 RBF Serial port receive buffer full
   (mirror)."* `opus:8-serial.c64`, `c66`, `c67`; `sol:8-serdatr.c18`,
   `c22`, `c94`; `opus:8-serdatr.c15`. SERDATR bit 14 tracks INTREQR bit 11
   exactly (INTF_RBF).
5. **OVRUN-BIT15-MIRROR** — p.256 *"15 OVRUN OVERRUN (Mirror ...)"*, p.283
   *"15 OVRUN ... Reset by resetting bit 11 of INTREQ."*
   `opus:8-serial.c61`; `sol:8-serdatr.c15`; `opus:8-serdatr.c12`;
   `sol:8-serdatr.c90`. SERDATR bit 15 sets on a second, uncollected receive
   and tracks INTREQR bit 11's clear.
6. **RESET-INTF-RBF-CLEARS-RBF-AND-OVRUN** — p.256 *"you must reset the
   INTF_RBF bit in INTREQ to prevent an overrun."* `opus:8-serial.c63`,
   `c69`; `sol:8-serdatr.c17`, `c24`. Clearing INTREQ bit 11 clears both
   SERDATR bit 14 and bit 15.
7. **INTREQ-BITS-VISIBLE-AT-SERDATR-ADDRESS** — p.283 *"Several interrupt
   request bits are also read at this address, along with the data."*
   `opus:8-serdatr.c8`, `sol:8-serdatr.c86`.
8. **SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT** (S4-C3, expected RED) —
   p.257 *"11 RXD Direct read of RXD pin on Paula chip."* `opus:8-serial.c82`.
   Whether "direct" forbids a synchroniser is the adjudication (not mine to
   make); the measurement is a latency in clk7_en edges between `rxd`
   changing and SERDATR bit 11 changing, which the manual's own word
   "direct" would predict to be zero.
9. **SERDAT-CHIP-COLUMN-P283** — p.283 *"SERDAT 030 W P Serial port data
   and stop bits write."* `opus:8-serdat.c4`, `sol:8-serdat.c40`. A
   deliberate SERDAT write (never a "probe read" — $030 has no read decode,
   so reading it is itself a write, per the build brief's fixture hazard)
   produces its transmit effect at the `paula` boundary alone.
10. **ADKCON-REGISTER-SUMMARY-ROW** — p.265 *"ADKCON 09E W P Audio, disk,
    control write."* `opus:8-adkcon.c2`, `c3`, `c4`, `c6`; `sol:8-adkcon.c9`,
    `c10`, `c12`, `c26`. Address $09E, write-only, reaches real logic.
11. **ADKCONR-REGISTER-SUMMARY-ROW** — p.265 *"ADKCONR 010 R P Audio, disk,
    control read."* `opus:8-adkcon.c7`, `c8`, `c9`, `c11`; `sol:8-adkcon.c13`,
    `c14`, `c16`, `c27`. Address $010, read-only (a write attempt at $010
    changes nothing stored), reports real state.
12. **ADKCON-BITS-0-14-SYMMETRIC-VIA-ADKCONR** — p.265's BIT#/USE table,
    read as one table shared by both registers. `opus:8-adkcon.c12`
    (position/storage half). Every named bit 0-14 written through ADKCON
    reads back at the same position through ADKCONR. Also this package's
    fixture-liveness control: two distinct, known words delivered and read
    back correctly before any other assertion is trusted.
13. **ADKCONR-BIT15-SETCLR-NEVER-READS-BACK** (S4-C6, expected RED) —
    p.265, same table's row 15: *"15 SET/CLR Set/clear control bit..."*
    `opus:8-adkcon.c12` (same claim, the row-15 half — the "one shared
    table" prediction the claim makes, that fails for exactly this one row).
    Bit 15 is a write-side mode selector consumed during the write cycle,
    not stored state; ADKCONR's bit 15 is measured, not assumed, to always
    read 0 regardless of what was written. Whether a write-only control bit
    *should* read back is the adjudication (not mine); the measurement is
    not in doubt.
14. **ADKCON-SETCLR-SET** — p.257 *"15 SET/CLR ... If bit 15 is a 1
    specified bits are set."*, p.265 *"15 SET/CLR Set/clear control bit.
    Determines if bits written with a 1 get set..."* `opus:8-serial.c96`,
    `c97`, `c98`; `opus:8-adkcon.c13`, `c14`, `c15`; `sol:8-adkcon.c1`, `c2`,
    `c3`, `c4`, `c17`, `c18`, `c19`, `c20`.
15. **ADKCON-SETCLR-CLR** — p.257 *"If bit 15 is a 0 specified bits are
    cleared."* `opus:8-serial.c99`; `sol:8-adkcon.c5`. A CLR-mode write
    (bit 15=0) actually clears an already-set bit (not merely leaves an
    already-0 bit alone — the distinct half `ch5-audio-dmacon-adkcon`'s own
    "route" extension found missing until its own ADKCON-SETCLR-CLR was
    added).
16. **ADKCON-SETCLR-PRESERVE** — p.265 *"Bits written with a zero are
    always unchanged."* `opus:8-adkcon.c16`; `sol:8-adkcon.c21`.
17. **ADKCON-BIT11-UARTBRK** — p.257 *"11 UARTBRK Force the transmit pin to
    zero."*, p.265 *"11 UARTBRK Forces a UART break (clears TXD) if true."*
    `opus:8-serial.c100`, `c101`; `sol:8-adkcon.c7` (this last one's own
    classify `note` calls it a "restored duplicate of opus:8-serial.c100" —
    same proposition, cited together). Setting bit 11 forces `txd` low;
    clearing releases it.
18. **ADKCON-BITS-12-9-8-STORE-ONLY-NO-CONSUMER** — p.265 *"12 MFMPREC"*,
    *"09 MSBSYNC"*, *"08 FAST"* (name/position only — the corrected three
    from above). `opus:8-adkcon.c23`, `c30`, `c32`. Each bit stores and
    reads back individually at its own position (position claim graded,
    matching the scope `ch8-adkcon-disk-control-bits` gave PRECOMP); whether
    each bit's named function does anything is not asserted by any of these
    three claims and is reported via `note()` only, following the same
    non-overreach discipline as the sibling package's own C8 treatment.

(Assertion count is 18, not 17 — 12 and 13 split one claim's two halves,
as explained above; I miscounted before writing the table and am leaving
the correction visible rather than silently renumbering.)

## Mutation plan

Each of the 18 assertions gets its own
`// MUTATION <label> <oracle|stimulus|none> <red|green|impossible> <what changed>`
record in the C++ source, `<label>` being the exact leading token of the
`expect()` label, replayable as `make MUT=MUT_n test` (`#if MUT_n` guards,
`MUT ?=` / `-D$(MUT)=1` in the Makefile — the suite-wide convention, e.g.
`hrm/ch8-keyboard-serial-register`'s Makefile and test source, read for
build/rig plumbing only, not as an oracle for this package's own content).

16 of 18 assertions are expected GREEN at baseline: **stimulus red**
mutations (change what is written/injected so the same expected condition
now fails), the standard shape.

Assertion 8 (SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT) is expected RED at
baseline: an **oracle** mutation, since the defect is in the RTL's latency,
not the stimulus — the baseline oracle demands zero-cycle latency (what
"direct" would mean); the mutation relaxes the accepted-latency bound to the
value actually measured, flipping the same measurement to green and thereby
showing the red is a genuine latency, not a broken fixture.

Assertion 13 (ADKCONR-BIT15-SETCLR-NEVER-READS-BACK) is expected RED at
baseline for the same reason: an **oracle** mutation relaxing "must read 0"
to "must read back what was written" flips it green on the identical
measurement, showing the red is the real always-0 readback, not a fixture
bug.

## What happens next

Only after this file is committed: open `paula.v` (and `paula_uart.v`,
`paula_intcontroller.v` only as far as needed to confirm the register
addresses and bit positions already named above from disclosed evidence),
read `hrm/ch8-adkcon-disk-control-bits/Makefile` and
`hrm/ch5-audio-dmacon-adkcon/test_paula.cpp` again for build/rig plumbing
(fixture tick/settle_edges pattern, not as an oracle for this package's own
claims), then write this package's Makefile and test source.
