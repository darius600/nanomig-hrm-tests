# Specification — `ch8-keyboard-serial-register`

Package: `hrm/ch8-keyboard-serial-register`
Fixture: `ciaa` (compiled alone, plus whatever submodules it instantiates and
Verilator needs to elaborate — their identity is not looked up here; per the
build brief, port names and file identity are step-2 material, fixed only
after this specification is committed).
Claims file:
`hrm-testpipeline/work/ch8-s3-build/ch8-keyboard-serial-register.jsonl`
(69 claims, all `classify.status: testable`)
Subject: printed pages 251, 357, 358, 359 and 364 — the keyboard's path
through one 8520 CIA's serial shift register (p.251), and Appendix G's
byte-rotation rule (pp.357-358) that puts the printed KCLK/KDAT waveform's
bit order and the two worked examples ($35, $B5) into that register, plus
the two follow-on mentions of the same rotation on pp.359 and 364.

This file is written from the `extract.quote`, `extract.page` and
`classify.extract.restatement` fields of the 69 claims, cross-read against
the primary source text for pages 251, 357, 358, 359 and 364
(`hrm-testpipeline/docs/references/amiga-hrm-3rd/ch8/p251.txt` and
`.../appG-keyboard/p357.txt`, `p358.txt`, `p359.txt`, `p364.txt`). It fixes
the printed propositions to be tested; port names, concrete stimulus
encodings and the `expect()` calls themselves are added only after this file
is committed and step 2 (opening the RTL) begins, per the build brief.

**What is already known before step 2, and why.** Unlike a package built
from the claims file alone, this package's build task itself (not the claims
file) discloses five RTL facts up front, as the reason 53 of the 69 claims
are gradeable at all: `ciaa.v:186`'s
`sdr_latch[7:0] <= ~{kbd_mouse_data[6:0], kbd_mouse_data[7]}` realises
p.357's rotation and p.358's active-low KDAT convention in one line; the
register is read via `ciaa.v:147` (`rs==4'hC`), `:195`, `:155`; the receive
strobe is generated at `:167-174` and assigned at `:172`; and `ciaa.v:161`
declares a `keydat` that nothing in the file ever uses. These five facts are
used below only to state *scope* (which printed propositions are reachable
at a whole-byte, no-wire boundary and which are not) — they supply no
expected value that is not independently derivable from the printed pages
themselves (see the worked-example arithmetic under PROP-7/PROP-9 below,
carried out by hand from the page's own stated rotation rule and active-low
convention, not read off the RTL). Beyond these five disclosed facts, no RTL
file — `ciaa.v` included — has been opened by this package before this
specification was committed.

## Scope

**Boundary: `ciaa`, in `nanomig/src/minimig-aga/`.** This is a
`src/minimig-aga` boundary, not `src/misc`. The build brief's standing
qualifier — that a red result from the `src/misc` layer (`amiga_keymap.v`,
`hid.v`) is about NanoMig specifically and does not generalise to the
Minimig family, because upstream `MiSTer-devel/Minimig-AGA_MiSTer` has no
such layer — **does not apply to this package** and is deliberately not
repeated here. The package list names exactly two of this slice's three
packages as the first built against `src/misc` (`ch8-keycode-table` at
`keymap`, `ch8-keyboard-events` at `hid`); this package is the third, at
`ciaa`, which is the ordinary CIA-A chipset module. The same build brief
notes upstream's `minimig.v:207` takes `kbd_mouse_data` as a top-level input
straight into the chipset — i.e. a `ciaa` receiving that same signal is not
a NanoMig-only insertion the way the keymap/hid translation layer is. A red
result here is not carved out from the Minimig family by this specification
because nothing in the build task's own reasoning supports that carve-out
for this particular boundary.

**What p.251 and Appendix G describe, and what of it is reachable at `ciaa`
alone.** Appendix G describes a full serial link: two wires (KCLK, KDAT),
voltage levels, pulse widths (~20 microseconds per phase, ~60
microseconds/bit), a handshake protocol (1-85 microsecond pulses), and an
out-of-sync/resync recovery procedure. P.251 names the two pins this link
uses on the CIA — SP (data) and CNT (clock) — and describes a per-bit
receive process ("on each transition of this [CNT] line, one bit of data is
clocked in"). **None of that survives to this boundary.** `ciaa` has no CNT
port and no SP port; nothing in this specification tests a voltage, an edge,
a pulse width, or a clocked bit-by-bit receive, because there is nothing at
this boundary that could make such an assertion fail for the right reason.
What *is* reachable: `kbd_mouse_data` presents a whole byte, a strobe
signal latches it in one step, and the register a program reads back is an
**8-bit image of that byte, permuted and inverted** — and Appendix G's own
rotation rule (p.357: "the transmitted order is therefore 6-5-4-3-2-1-0-7")
together with p.358's own active-low statement ("The KDAT line is active
low; that is, a high level (+5V) is interpreted as 0, and a low level (0V)
is interpreted as 1") is a complete, checkable description of that
permute-and-invert image, provable by hand from the printed page alone (see
below). That is why bit identity and bit ordering are this package's
subject, never a wire's voltage or timing. If a claim reaches this file that
is actually about the wire, it is declined, not graded.

## Claim inventory (69 of 69 accounted for)

| assertion | page(s) | uids | grading |
|---|---|---:|---|
| PROP-1 | 251 | `opus:8-kbd.c1`, `sol:8-kbd.c1` | full |
| PROP-2 | 251 | `opus:8-kbd.c12`, `sol:8-kbd.c9` | full |
| PROP-3 | 358 | `opus:8-kbdtiming.c11`, `sol:8-appG.c50` | full |
| PROP-4 | 357 | `opus:8-appG.c26`, `sol:8-appG.c34` | full |
| PROP-5 | 357, 358 | `opus:8-appG.c27`, `sol:8-appG.c35`, `opus:8-kbdtiming.c23`, `opus:8-kbdtiming.c24`, `opus:8-kbdtiming.c25`, `sol:8-appG.c41`, `sol:8-appG.c42`, `sol:8-appG.c179`..`c186` (8 uids) | full (15 uids) |
| PROP-6 | 357 | `sol:8-appG.c36` | full |
| PROP-7 | 358 | `opus:8-kbdtiming.c18`, `sol:8-appG.c57`, `opus:8-kbdtiming.c28`..`c35` (8 uids), `sol:8-appG.c187`..`c194` (8 uids) | full (18 uids) |
| PROP-8 | 358 | `opus:8-kbdtiming.c19`, `sol:8-appG.c58` | full |
| PROP-9 | 358 | `opus:8-kbdtiming.c22`, `sol:8-appG.c62`, `opus:8-kbdtiming.c37`..`c44` (8 uids), `sol:8-appG.c195`..`c202` (8 uids) | full (18 uids) |
| WIDTH-8BIT (untagged) | 357, 364 | `opus:8-appG.c13`, `sol:8-appG.c20`, `opus:8-appG.c142`, `sol:8-appG.c170` | **partial** — uid on `record()` only |
| MAPPING-SAME-CLASS (untagged) | 364 | `opus:8-appG.c144`, `sol:8-appG.c172` | **partial** — uid on `record()` only |
| FLAG-LAST-GENERAL (untagged) | 359 | `sol:8-appG.c82` | **partial** — uid on `record()` only |

62 uids fully graded (their own uid appears on the graded `expect()` line
that tests exactly what they assert), 7 uids partially graded (the position/
width half of a compound claim is graded by an untagged assertion; the
serial-cable half is not observable here and is declined on a `record()`
line carrying that uid alone, per the build brief's rule on partial
grading). **No claim in this package is declined outright**: every one of
the 69 has at least its byte-level half tested.

## PROP-1 — the keyboard reaches the system through one CIA's serial shift register

p.251 (`opus:8-kbd.c1`, twin `sol:8-kbd.c1`):

> The keyboard is interfaced to the system via the serial shift register on
> one of the 8520 CIA chips.

**Testable proposition:** presenting a keyboard byte at this boundary's
keyboard-data input and driving the boundary's own strobe input makes that
byte become readable back out through the register this boundary exposes at
the register-select value the build task names (`ciaa.v:147`, `rs==4'hC`) —
i.e. the serial data register is genuinely the vehicle, not a dead port.
This is a claim about *which* register mediates the interface, not about
*how* the bits arrive on it — the "via a cable, bit by bit" half of the full
picture is Appendix G's territory and belongs to the PROP-4/PROP-5/PROP-7/
PROP-9 propositions below, each grading a specific facet of that mechanism.

Status: testable in full, no compound half left over — the printed sentence
makes exactly one claim (identity of the mediating register) and that claim
is reachable at `ciaa` alone.

## PROP-2 — receiving a byte issues an interrupt to the processor

p.251 (`opus:8-kbd.c12`, twin `sol:8-kbd.c9`):

> After a data byte has been received from the keyboard, an interrupt from
> the 8520 is issued to the processor.

**Testable proposition:** with the relevant interrupt source armed at this
boundary, presenting a keyboard byte and driving the strobe (the same
stimulus PROP-1 uses) makes the boundary's interrupt output assert. Exact
register offsets for arming the source, and the interrupt output's own
name, are step-2 material.

Status: testable in full. "After a data byte has been received" is
stimulated as the one whole-byte strobe event this boundary has — there is
no bit counter here, so the sentence's phrase cannot be read as "after eight
individually clocked bits," only as "after the byte-latch event," which is
exactly what this boundary offers.

## PROP-3 — a keycode is always 7 bits long

p.358 (`opus:8-kbdtiming.c11`, twin `sol:8-appG.c50`):

> Keycodes are always 7 bits long.

**Testable proposition:** the code portion of the transmitted word occupies
exactly 7 bit positions, and its identity is independent of the eighth
(flag) bit — sweeping the 7 code bits must move only 7 of the register's 8
output positions, and toggling the remaining bit alone must move only the
one position the code bits never touch. That is what distinguishes "a 7-bit
code plus a 1-bit flag" from "an undifferentiated 8-bit code" — a different
layout (e.g. no split, or a split at a different width) would fail this
test.

Status: testable in full.

## PROP-4 — all codes are rotated one bit before transmission

p.357 (`opus:8-appG.c26`, twin `sol:8-appG.c34`):

> All codes transmitted to the computer are rotated one bit before
> transmission.

**Testable proposition:** the map from the presented byte to the register's
readback is a genuine permutation that *moves* bit positions — for every
input bit, exactly one output position responds, and it is not the
same-numbered position for at least one bit (otherwise the mapping would be
identity, or identity-with-inversion, neither of which is "rotated"). This
is deliberately a coarser question than PROP-5's exact ordering below: a
transform could move bits without moving them by exactly the printed amount,
which is why the manual states rotation and its exact order as two separate
sentences and this specification grades them as two separate propositions.

Status: testable in full.

## PROP-5 — the transmitted order is 6-5-4-3-2-1-0-7

p.357 (`opus:8-appG.c27`, twin `sol:8-appG.c35`):

> The transmitted order is therefore 6-5-4-3-2-1-0-7.

p.358, Appendix G's KCLK/KDAT figure (`figures/p358.png`), the general
(non-key-specific) label row under the first waveform: positions read left
to right as "(6) (5) (4) (3) (2) (1) (0) (7)", with "First sent" under (6)
and "Last sent" under (7). Fifteen uids restate this one fact, whole
(`opus:8-kbdtiming.c23`, its two halves `opus:8-kbdtiming.c24` "bit 6 is
first" and `opus:8-kbdtiming.c25` "bit 7 is last", and their sol twins
`sol:8-appG.c41`/`c42`) or decomposed bit by bit (`sol:8-appG.c179`
through `c186`, one uid per code-bit-to-serial-position pairing: 6→1, 5→2,
4→3, 3→4, 2→5, 1→6, 0→7, 7→8).

**Testable proposition:** the exact position-to-bit map holds — code bit 6
lands in serial position 1 (the register's bit 7, its first-received slot),
bit 5 in position 2 (register bit 6), bit 4 in position 3 (bit 5), bit 3 in
position 4 (bit 4), bit 2 in position 5 (bit 3), bit 1 in position 6 (bit
2), bit 0 in position 7 (bit 1), and bit 7 in position 8 (bit 0, the last-
received slot). Tested by toggling one presented bit at a time and reading
which single register position responds; the two aggregate uids
(`c23`/`c41`+`c42`) and the eight per-position uids (`c179`-`c186`) are
different restatements of the identical row of labels in the identical
figure, so one sweep across all eight positions grades all fifteen.

Status: testable in full.

## PROP-6 — the up/down flag is transmitted last (p.357's own reason-giving sentence)

p.357 (`sol:8-appG.c36`):

> The reason for this is to transmit the up/down flag last, in order to
> cause a key-up code to be transmitted in case the keyboard is forced to
> restore lost sync (explained in more detail below).

**Testable proposition, the ordering half only:** bit 7 (the flag) lands in
the last serial position (position 8 / register bit 0), and only bit 7
governs that position. The clause this sentence exists to explain — *why*
that ordering helps recovery from a lost-sync condition described later on
p.359 — is about the out-of-sync/resync protocol, which needs KCLK, KDAT,
and a notion of "restore lost sync" that does not exist at this boundary;
that part is not graded and not claimed to be. What is graded is exactly
the same fact PROP-5 already establishes for bit 7 specifically — this
proposition earns its own assertion because it is its own printed sentence,
not because the underlying measurement differs.

Status: testable in full, on the ordering fact alone; the reason for the
ordering is not a further requirement this restatement adds (`extract.
restatement` for this uid is simply "The up/down flag is transmitted
last"), so nothing is left ungraded.

## PROP-7 — worked example: the B key pushed down transmits 01101010

p.358 (`opus:8-kbdtiming.c18`, twin `sol:8-appG.c57`):

> due to the rotation of the byte, the bits transmitted are 01101010

...for keycode $35 (00110101). Eighteen uids restate this fact: the two
above, whole, plus sixteen figure-derived uids that read the same
"B pushed down" waveform in the printed figure one serial position at a
time (`opus:8-kbdtiming.c28`-`c35` for the eight positions in order, digits
"0 1 1 0 1 0 1 0"; `sol:8-appG.c187`-`c194`, the same eight positions
restated independently).

**Verification by hand, from the page alone, before any RTL is opened:**
$35 = `00110101`; applying p.357's stated order (bit 6, 5, 4, 3, 2, 1, 0,
then 7) to those bits gives `0,1,1,0,1,0,1,0` — exactly the digit row
printed under the figure and exactly `opus:8-kbdtiming.c18`'s quoted
"01101010". Applying p.358's own active-low statement (a printed digit of
this figure is the *logical* bit the keyboard means to send; what a
receiving 8520 shift register would actually hold, bit for bit, for an
active-low line is the complement of that logical value) gives the byte
`~01101010 = 10010101` = `$95`. This is the expected register value; it is
derived from two printed sentences (the rotation order and the active-low
statement) and the printed keycode $35, not from the RTL.

**Testable proposition:** presenting keycode $35 and strobing reception
makes the register read exactly `$95`. A single 8-bit equality subsumes
each of the sixteen per-position figure claims (a whole-byte match implies
every one of its eight bit positions individually matches the printed
digit's active-low counterpart), so one assertion grades all eighteen
uids.

Status: testable in full.

## PROP-8 — releasing the key does not change the 7-bit code

p.358 (`opus:8-kbdtiming.c19`, twin `sol:8-appG.c58`):

> the B key is released. The keycode is still $35 [, except that bit 7 is
> set to indicate "key-up"]

**Testable proposition:** the register readback for $35 (key down) and $B5
(key up, same code with bit 7 set) agree on every position except the one
PROP-5/PROP-6 identify as bit 7's — i.e. the 7-bit code is invariant under
the flag, read back through the same permutation PROP-3/PROP-5 already
establish. This is a comparison between two stimuli, not a fact about
either one alone, which is why it earns a separate assertion from PROP-7
and PROP-9's single-stimulus checks.

Status: testable in full.

## PROP-9 — worked example: the B key released transmits 01101011

p.358 (`opus:8-kbdtiming.c22`, twin `sol:8-appG.c62`):

> After rotating, the transmission will be 01101011

...for keycode $B5 (10110101, i.e. $35 with bit 7 set). Eighteen uids
restate this fact the same way PROP-7's do, for the second ("B released")
waveform: the two above, whole, plus sixteen per-position figure uids
(`opus:8-kbdtiming.c37`-`c44`; `sol:8-appG.c195`-`c202`).

**Verification by hand, from the page alone:** $B5 = `10110101`; the same
p.357 order applied to those bits gives `0,1,1,0,1,0,1,1` — exactly the
printed "01101011" `opus:8-kbdtiming.c22` quotes. The same active-low
complement gives `~01101011 = 10010100` = `$94`, matching the task's own
stated arithmetic.

**Testable proposition:** presenting keycode $B5 and strobing reception
makes the register read exactly `$94`. As with PROP-7, one 8-bit equality
subsumes all sixteen per-position uids.

Status: testable in full.

## Partially-gradeable claims (7 of 69) — byte-level half graded, cable half absent

Three printed sentences are compound: part names a fact this boundary can
show, part names a fact only a real KCLK/KDAT cable (or, for two of them, a
special-code *generator* this RTL does not contain) could show. Per the
build brief's rule on partial grading, **no uid below is placed on a graded
`expect()` line** — each is graded only by an untagged assertion, and
appears on its own `record()` line naming exactly what was and was not
tested.

**WIDTH-8BIT — "the keyboard transmits 8-bit data words *serially*"
(p.357, `opus:8-appG.c13`/`sol:8-appG.c20`) and "the special codes are 8-bit
numbers" (p.364, `opus:8-appG.c142`/`sol:8-appG.c170`).** The 8-bit-width
half is testable: sweeping the presented byte must produce 256 distinct
register readbacks, i.e. no information is lost between an 8-bit input and
the register. "...serially" (p.357) names the KCLK-clocked, bit-at-a-time
delivery this boundary does not have (one whole-byte latch on one strobe,
not eight edges). "The special codes are..." (p.364) additionally requires
this RTL to distinguish a *special* code from an ordinary one so that "8-bit
number" is a claim about the right category of byte — `ciaa` has no such
distinction; every value of the presented byte is carried identically,
whatever it is taken to mean upstream. Both halves are declined; the width
half is graded.

**MAPPING-SAME-CLASS — "however, the transmission bit order is the same as
previously described" (p.364, `opus:8-appG.c144`/`sol:8-appG.c172`).** The
sentence's subject ("the transmission bit order") is Appendix G's
KCLK-clocked serial order, described on the cable; what is reachable here is
only its byte-level shadow — that the identical permute-and-invert formula
PROP-5/PROP-7/PROP-9 establish for ordinary keycodes also holds when the
presented byte is one of the specific special-code values p.364 itself
prints (`$78`, `$F9`, `$FC`, etc.), which is testable because `ciaa` applies
that formula uniformly with no notion of code class at all. The cable-timing
half of "transmission bit order" is not graded.

**FLAG-LAST-GENERAL — "That is why the key-up/key-down flag is always
transmitted last" (p.359, `sol:8-appG.c82`).** This is PROP-6's fact
restated on the page that explains *why* (out-of-sync recovery), with the
word "always" read as spanning every transmission over time on a wire this
boundary does not have. The position fact — bit 7 uniquely determines the
last register position, independent of every other bit, on the one
whole-byte latch this boundary offers — is graded by an assertion with no
uid on its label; "always," across a stream of individually clocked
transmissions, is not.

## On a control case and mutation

The build brief requires (a) a fixture-liveness proof before any
data-dependent grading, (b) at least one control case expected to PASS on
the same measurement basis as anything under suspicion, and (c) a stimulus
mutation per assertion. Concrete stimulus encodings and exact port names
are step-2 material; the shape of each is already fixed by the claims
above:

- **Fixture liveness.** Before any PROP is graded, the harness must drive
  at least two distinct, known bytes through this boundary's keyboard-data
  input and confirm — by the register readback PROP-1 depends on — that
  both arrived and did not collapse to one value or to nothing.
- **Control case.** `kbd_mouse_data = $00` is a third, hand-computable
  point distinct from the two worked examples: rotating an all-zero byte
  leaves it all zero, and its active-low complement is `$FF`. This is
  checked once, expected to PASS, on the same register-readback basis
  PROP-7/PROP-8/PROP-9 use, before those are graded — it is the "does the
  measurement basis itself work" check the build brief asks for,
  independent of the two printed worked examples.
- **Mutations.** Every `expect()` above — PROP-1 through PROP-9, the
  fixture-liveness check, the control case, and the three untagged
  WIDTH-8BIT/MAPPING-SAME-CLASS/FLAG-LAST-GENERAL assertions — gets one
  stimulus mutation: a value, bit position, or comparison changed so the
  same condition must now read FAIL, applied, run, observed, and reverted.
  Since the RTL is believed (per the build task, and per the by-hand
  arithmetic above) to implement the printed rotation and active-low
  convention exactly, **every assertion here is expected to be green at
  baseline** — this is not the six-repair-lots situation of a red baseline
  needing a substitution proof; it is the ordinary case of proving a
  passing assertion is not vacuously true by showing its natural negation
  fails. No `stimulus green` record is expected or written for that reason:
  nothing here is read at a red baseline.
- Whether any mutation turns out `impossible` at this boundary is not
  assumed here; that is decided in step 2 once the actual port list and
  timing are known, and reported if it happens.

## Disclosure — what is already known, and the limit of it

This specification is written from `extract.quote` / `extract.page` /
`classify.extract.restatement` and the primary page text for pp.251, 357,
358, 359, 364, plus exactly the five RTL facts the build task itself
disclosed up front (listed at the top of this document) to explain *why*
this package's boundary is `ciaa` and *why* bit-level claims are reachable
here at all. The `classify.evidence` field in the 69-claim source file
carries far more than those five facts — specific register-select values
beyond `rs==4'hC`, exact signal names, the interrupt-controller
instantiation path, `ser_tx_cnt`, and more — and that field was necessarily
read once (it lives in the same JSON records as `uid`/page/quote/
restatement, which could not be parsed without encountering it), but beyond
the five facts named above, none of the rest of it shaped any proposition,
expected value, or scope decision in this document. This is disclosed
rather than concealed, following the practice already recorded by sibling
packages `ch8-potgo-register-bit-layout` and `ch8-ciaa-port-direction`
before it.

No RTL file has been opened directly by this package before this
specification was committed. Step 2 opens `ciaa.v` (and whatever it
instantiates) for the first time under this package's own effort, to
confirm the port list, the register-select encoding, and the exact
mechanism behind the five disclosed facts, and to write the `expect()`
calls, stimulus values and `// MUTATION` records themselves.

## What this specification deliberately does not contain

Beyond the five facts disclosed above (needed for scope, not for any
expected value), no port name, signal name, or file:line location is used
to derive a testable proposition or an expected value in this document.
Every expected value above (`$95`, `$94`, `$FF`, the position-to-bit map,
the 256-distinct-values width check) is computed by hand from a printed
sentence and a printed keycode, and would be the same regardless of what
`ciaa.v` turns out to contain. Which two "other" bit positions, which
specific special-code values beyond the three named above, and the exact
register-select constants for arming the interrupt path, are step-2
choices, made after this file is committed.
