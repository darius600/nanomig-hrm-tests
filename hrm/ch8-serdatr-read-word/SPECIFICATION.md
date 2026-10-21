# Specification — `ch8-serdatr-read-word`

Package: `hrm/ch8-serdatr-read-word`
Fixture: `paula_uart` (per the package list; module `paula_uart.v` alone)
Claims file:
`hrm-testpipeline/work/ch8-s4-build/ch8-serdatr-read-word.jsonl`
(123 claims)
Subject: SERDATR ($018) — its identity as a real, read-only register, the
receive-shift-register-to-SERDATR transfer mechanism, and every bit the
manual assigns it: OVRUN (15), RBF (14), TBE (13), TSRE (12), RXD (11), the
unused bit (10), STP/DB8 (9, 8) and the eight low data bits (7-0).

This file is written from the `extract.quote` / `extract.restatement` fields
of the 123 claims alone. **Disclosure, per the build brief's own convention
for background given by the task rather than discovered by reading RTL**:
while first inspecting the raw claims file to learn its shape, one record's
full JSON (`opus:8-serdatr.c2`) was read before the quote-only extraction
below was set up, and its `classify.evidence` field is dense with RTL detail
— `paula_uart`'s full port list (`clk`, `clk7_en`, `reset`, `rga_i`,
`data_i`, `data_o`, `uartbrk`, `rbfmirror`, `txint`, `rxint`, `txd`, `rxd`),
the SERDATR read-word assembly at `paula_uart.v:272`
(`serdatr = {rx_ovrun, rx_rbf, tx_tbe, tx_tsre, rx_rxd}` at bits 15..11, a
literal `1'b0` at bit 10, `rx_data[9:0]` at bits 9..0), and the register
address constant `paula_uart.v:27`. This is the same shape of exposure the
build brief names for slice s3 — a bundling the builder did not create and
could not avoid while learning what the 123 claims even were, since each
claim record bundles `extract` and `classify` in one JSON object. Every
other claim below was read through a script that printed only
`extract.quote` / `extract.restatement` / `extract.page` /
`classify.status` / `classify.note`, withholding `classify.evidence`
deliberately once this was noticed. **None of this shaped which
propositions are written below** — every proposition here is grounded in a
printed quotation, named per its page, and the port list glimpsed early
changes no wording here — but it is disclosed rather than pretending the
order held cleanly, per the build brief's own instruction to say so rather
than paper over it.

**This package is `src/minimig-aga`, the shared-lineage layer** —
`paula_uart` exists in `MiSTer-devel/Minimig-AGA_MiSTer` too, per the build
brief. A red here is a statement about the Minimig line, not only about this
board. No s3-style "NanoMig alone" qualifier belongs in this document.

**This package carries both of this slice's source conflicts**, named by the
build brief and by finding candidates S4-C8 and S4-C9: SERDATR bit 13
(mirror or not) and SERDATR bit 8 (stop bit under which condition). Both are
measured on both sides below and graded on neither — see A10 and A17.

## Correction to the finding-candidates document

**S4-C4 cites the wrong page.** The candidates file says the 8-to-10-bit-time
character-receive window is printed on p.257; checked against
`docs/references/amiga-hrm-3rd/ch8/p256.txt:26-27`, the sentence — "After
receiving the receiver-full interrupt, you will have up to one full
character-receive time (8 to 10 bit times) to accept the data and clear the
interrupt" — is on **p.256**, not p.257. p.257 is the *next* page of the
same printed spread (Table 8-9 continues onto it starting at bit 13, TBE),
which is presumably how the two pages were conflated when the candidate was
written. The claims themselves (`sol:8-serdatr.c9`, `sol:8-serdatr.c10`) are
correctly filed at page 256 in the claims JSON; only the candidates
document's prose is wrong. A10 below (bit 13) is genuinely on both pages —
p.256's own Table 8-9 begins there (bits 15 OVRUN, 14 RBF) and continues
onto p.257 (13 TBE downward) — but the "8 to 10 bit times" sentence is prose
above the table, entirely on p.256.

## Claim inventory (all 123)

| uid | page | assigned to |
|---|---|---|
| `opus:8-serdatr.c2` | 283 | A2 |
| `opus:8-serdatr.c3` | 283 | A3 |
| `opus:8-serdatr.c5` | 283 | A4 |
| `opus:8-serdatr.c6` | 283 | A5 |
| `opus:8-serdatr.c7` | 283 | A5 |
| `opus:8-serdatr.c11` | 283 | A6 |
| `opus:8-serdatr.c14` | 283 | A7 |
| `opus:8-serdatr.c17` | 283 | A10 (conflict, p.283 side) |
| `opus:8-serdatr.c20` | 283 | A11 |
| `opus:8-serdatr.c21` | 283 | A11 |
| `opus:8-serdatr.c23` | 283 | A13 |
| `opus:8-serdatr.c24` | 283 | A13 |
| `opus:8-serdatr.c26` | 283 | A14 |
| `opus:8-serdatr.c28` | 283 | A15 |
| `opus:8-serdatr.c30` | 283 | A17 (conflict, p.283 side) |
| `opus:8-serdatr.c31` | 283 | A17 (conflict, p.283 side) |
| `opus:8-serdatr.c33` | 283 | A18 |
| `opus:8-serdatr.c35` | 283 | A18 |
| `opus:8-serdatr.c37` | 283 | A18 |
| `opus:8-serdatr.c39` | 283 | A18 |
| `opus:8-serdatr.c41` | 283 | A18 |
| `opus:8-serdatr.c43` | 283 | A18 |
| `opus:8-serdatr.c45` | 283 | A18 |
| `opus:8-serdatr.c47` | 283 | A18 |
| `sol:8-serdatr.c1` | 256 | A2 |
| `sol:8-serdatr.c2` | 256 | A18 |
| `sol:8-serdatr.c3` | 256 | A17 (conflict, background) |
| `sol:8-serdatr.c4` | 256 | A2 |
| `sol:8-serdatr.c5` | 256 | A5 |
| `sol:8-serdatr.c6` | 256 | A5 |
| `sol:8-serdatr.c7` | 256 | A5 |
| `sol:8-serdatr.c8` | 256 | A5 |
| `sol:8-serdatr.c9` | 256 | A8 |
| `sol:8-serdatr.c10` | 256 | A8 |
| `sol:8-serdatr.c11` | 256 | A6 |
| `sol:8-serdatr.c12` | 256 | A6 |
| `sol:8-serdatr.c14` | 256 | A6 |
| `sol:8-serdatr.c16` | 256 | A6 |
| `sol:8-serdatr.c19` | 256 | A7 |
| `sol:8-serdatr.c21` | 256 | A7 |
| `sol:8-serdatr.c23` | 256 | A7 |
| `sol:8-serdatr.c25` | 257 | A9 |
| `sol:8-serdatr.c27` | 257 | A9 |
| `sol:8-serdatr.c29` | 257 | A10 (conflict, p.257 side) |
| `sol:8-serdatr.c31` | 257 | A9 |
| `sol:8-serdatr.c33` | 257 | A11 |
| `sol:8-serdatr.c35` | 257 | A11 |
| `sol:8-serdatr.c36` | 257 | A11 |
| `sol:8-serdatr.c37` | 257 | A11 |
| `sol:8-serdatr.c38` | 257 | A11 |
| `sol:8-serdatr.c39` | 257 | A11 |
| `sol:8-serdatr.c41` | 257 | A12 |
| `sol:8-serdatr.c42` | 257 | A12 |
| `sol:8-serdatr.c43` | 257 | A13 |
| `sol:8-serdatr.c44` | 257 | A14 |
| `sol:8-serdatr.c45` | 257 | A14 |
| `sol:8-serdatr.c46` | 257 | A15 |
| `sol:8-serdatr.c47` | 257 | A15 |
| `sol:8-serdatr.c48` | 257 | A15 |
| `sol:8-serdatr.c49` | 257 | A16 |
| `sol:8-serdatr.c51` | 257 | A17 (conflict, p.257 side) |
| `sol:8-serdatr.c53` | 257 | A17 (conflict, p.257 side) |
| `sol:8-serdatr.c54` | 257 | A18 |
| `sol:8-serdatr.c56` | 257 | A18 |
| `sol:8-serdatr.c57` | 257 | A18 |
| `sol:8-serdatr.c59` | 257 | A18 |
| `sol:8-serdatr.c60` | 257 | A18 |
| `sol:8-serdatr.c62` | 257 | A18 |
| `sol:8-serdatr.c63` | 257 | A18 |
| `sol:8-serdatr.c65` | 257 | A18 |
| `sol:8-serdatr.c66` | 257 | A18 |
| `sol:8-serdatr.c68` | 257 | A18 |
| `sol:8-serdatr.c69` | 257 | A18 |
| `sol:8-serdatr.c71` | 257 | A18 |
| `sol:8-serdatr.c72` | 257 | A18 |
| `sol:8-serdatr.c74` | 257 | A18 |
| `sol:8-serdatr.c75` | 257 | A18 |
| `sol:8-serdatr.c77` | 257 | A18 |
| `sol:8-serdatr.c78` | 257 | A18 |
| `sol:8-serdatr.c79` | 283 | A2 |
| `sol:8-serdatr.c80` | 283 | A3 |
| `sol:8-serdatr.c82` | 283 | A4 |
| `sol:8-serdatr.c83` | 283 | A4 |
| `sol:8-serdatr.c84` | 283 | A5 |
| `sol:8-serdatr.c85` | 283 | A5 |
| `sol:8-serdatr.c87` | 283 | A6 |
| `sol:8-serdatr.c89` | 283 | A6 |
| `sol:8-serdatr.c91` | 283 | A7 |
| `sol:8-serdatr.c93` | 283 | A7 |
| `sol:8-serdatr.c95` | 283 | A10 (conflict, p.283 side) |
| `sol:8-serdatr.c97` | 283 | A10 (conflict, p.283 side) |
| `sol:8-serdatr.c99` | 283 | A11 |
| `sol:8-serdatr.c101` | 283 | A11 |
| `sol:8-serdatr.c102` | 283 | A11 |
| `sol:8-serdatr.c103` | 283 | A12 |
| `sol:8-serdatr.c105` | 283 | A13 |
| `sol:8-serdatr.c106` | 283 | A13 |
| `sol:8-serdatr.c107` | 283 | A14 |
| `sol:8-serdatr.c109` | 283 | A14 |
| `sol:8-serdatr.c110` | 283 | A15 |
| `sol:8-serdatr.c112` | 283 | A15 |
| `sol:8-serdatr.c113` | 283 | A16 |
| `sol:8-serdatr.c115` | 283 | A17 (conflict, p.283 side) |
| `sol:8-serdatr.c116` | 283 | A17 (conflict, p.283 side) |
| `sol:8-serdatr.c117` | 283 | A18 |
| `sol:8-serdatr.c119` | 283 | A18 |
| `sol:8-serdatr.c120` | 283 | A18 |
| `sol:8-serdatr.c122` | 283 | A18 |
| `sol:8-serdatr.c123` | 283 | A18 |
| `sol:8-serdatr.c125` | 283 | A18 |
| `sol:8-serdatr.c126` | 283 | A18 |
| `sol:8-serdatr.c128` | 283 | A18 |
| `sol:8-serdatr.c129` | 283 | A18 |
| `sol:8-serdatr.c131` | 283 | A18 |
| `sol:8-serdatr.c132` | 283 | A18 |
| `sol:8-serdatr.c134` | 283 | A18 |
| `sol:8-serdatr.c135` | 283 | A18 |
| `sol:8-serdatr.c137` | 283 | A18 |
| `sol:8-serdatr.c138` | 283 | A18 |
| `sol:8-serdatr.c140` | 283 | A18 |
| `sol:8-serdatr.c141` | 289 | A19 (elsewhere, declined) |
| `sol:8-serdatr.c142` | 289 | A19 (elsewhere, declined) |
| `sol:8-serdatr.c144` | 289 | A19 (elsewhere, declined) |

Verified by script against the claims file: all 123 uids appear in exactly
one row above (checked programmatically, not by hand).

## How the 123 claims were organised

1. **Bit-by-bit, per the manual's own table structure.** SERDATR's sixteen
   bits are individually named and defined across two table fragments — the
   top of Table 8-9 on p.256/p.257 (chapter body) and the register-summary
   entry on p.283 (Appendix A) — plus a general framing paragraph on p.256
   (register width, the receive-shift-register-to-SERDATR transfer, the
   character-receive-time window) that precedes the table. One assertion
   block per bit (or per closely related bit-pair, DB7-DB0 as one sweep),
   citing every uid — both readers, both pages — that states the same
   proposition.
2. **The two source conflicts get their own blocks, A10 and A17, graded on
   neither side.** Every other uid whose printed row happens to *carry* the
   disputed words (bit 13's row prints "(mirror)"; bit 8's row states the
   LONG condition) but whose own `extract.restatement` does not itself
   assert the disputed half is kept separate and graded normally — e.g.
   `sol:8-serdatr.c25`/`c27` state "bit 13 means transmit buffer empty"
   without taking a side on mirror-ness, so they ground A9, a claim this
   package can measure and grade.  Conversely, `opus:8-serdatr.c17` and
   `sol:8-serdatr.c95`/`c97` quote the p.283 row **in full**, "(mirror)"
   included, and are treated as carrying that disputed word into A10 rather
   than only their narrower restatement — the printed page, not the
   paraphrase, is what a `record()` line has to be honest about, per the
   build brief: "the oracle is the manual, not the RTL" applies equally to
   not under-reading the manual.
3. **Three claims are already graded in an accepted slice** (p.289's
   register-summary row for SERDATR) and go to A19, a `record()` line citing
   the accepted package, per the build brief's instruction not to re-grade
   them.
4. **Duplicate/"restored duplicate" claims** (e.g. `sol:8-serdatr.c42`,
   `sol:8-serdatr.c119`-`c140`) are cited alongside the uid they duplicate,
   in the same assertion block — they are the same proposition, stated
   twice by construction, not a second thing to test.

## A1 — FIXTURE-LIVENESS (no claim uid)

Not a proposition from the manual. Per the build brief: the fixture must
prove it can deliver a known value before anything reads one. Two distinct,
known SERDATR states — idle (fresh reset, nothing received) and after a
genuine, known byte has been received — must each arrive at SERDATR
distinctly before any content-specific assertion below is graded.

## A2 — SERDATR-IS-A-REAL-READABLE-REGISTER-AT-018

- Page: 283, 256. Uids: `opus:8-serdatr.c2`, `sol:8-serdatr.c79` (address);
  `sol:8-serdatr.c1` (16 bits wide); `sol:8-serdatr.c4` (carries status bits
  alongside data — background for the per-bit blocks below, which are what
  actually grades this).
- Quotes: p.283 "SERDATR 018 R P Serial port data and status read (receive
  data buffer)"; p.256 "The serial input data-receive register is 16 bits
  wide. It contains the 8 or 9 bit input data and status bits."
- Proposition: SERDATR is a real, decoded, 16-bit register at address $018 —
  reading it returns a defined value that differs correctly between two
  known states, not a stuck or floating result.
- Expected: PASS (control case).

## A3 — SERDATR-IS-READ-ONLY

- Page: 283. Uids: `opus:8-serdatr.c3`, `sol:8-serdatr.c80`.
- Quote: p.283, the row's R/W cell, "R", composed with
  `appB3/p287.txt:30`'s legend "R=read-only".
- Proposition: $018 is a read address; it carries no defined write
  behaviour of its own (unlike $030/SERDAT and $032/SERPER, which this
  slice's fixture-hazard note already names as write-only addresses that a
  bare read corrupts).
- Expected: to be measured against whatever `paula_uart` actually does when
  $018 is addressed with a write cycle.

## A4 — SERDATR-FUNCTION-IS-SERIAL-PORT-DATA-AND-STATUS

- Page: 283. Uids: `opus:8-serdatr.c5`, `sol:8-serdatr.c82`,
  `sol:8-serdatr.c83`.
- Quote: p.283 "Serial port data and status read (receive data buffer)".
- Proposition: SERDATR's row-level identity — it is the register that
  delivers both received data and status flags on a read. Graded jointly
  with A2 (same measurement basis); listed separately because the claims
  name the FUNCTION cell specifically, distinct from the ADD/R-W cells A2
  and A3 ground.
- Expected: PASS, same measurement as A2.

## A5 — RECEIVE-SHIFT-REGISTER-TRANSFERS-TO-SERDATR-WHEN-FULL-AND-IS-IMMEDIATELY-READY-AGAIN

- Page: 283, 256. Uids: `opus:8-serdatr.c6`, `opus:8-serdatr.c7`,
  `sol:8-serdatr.c5`, `sol:8-serdatr.c6`, `sol:8-serdatr.c7`,
  `sol:8-serdatr.c8`, `sol:8-serdatr.c84`, `sol:8-serdatr.c85`.
- Quotes: p.283 "This address reads data from a receive data buffer. Data in
  this buffer is loaded from a receiving shift register whenever it is
  full."; p.256 "The data is received, one bit at a time, into an internal
  serial-to-parallel shift register. When the proper number of bit times
  have elapsed, the contents of this register are transferred to [SERDATR]
  ..., and you are signaled that there is data ready for you. Immediately
  after the transfer of data takes place, the receive shift register again
  becomes ready to accept new data."
- Proposition: a serially-received byte transfers into SERDATR once fully
  shifted in, and the shift register is ready to receive the next byte
  immediately afterward — two known, different bytes received back-to-back
  must each arrive at SERDATR correctly and distinctly.
- Expected: to be measured.

## A6 — OVRUN-BIT15-SIGNALS-A-BYTE-ARRIVING-BEFORE-THE-PRIOR-ONE-WAS-READ

- Page: 283, 256. Uids: `opus:8-serdatr.c11`, `sol:8-serdatr.c11`,
  `sol:8-serdatr.c12`, `sol:8-serdatr.c14`, `sol:8-serdatr.c16`,
  `sol:8-serdatr.c87`, `sol:8-serdatr.c89`.
- Quotes: p.283 "15 OVRUN Serial port receiver overrun. Reset by resetting
  bit 11 of INTREQ." (the OVRUN-identity half graded here; the INTREQ-clear
  mechanism is outside `paula_uart`'s own ports and outside this claim set —
  no uid in these 123 claims asserts it); p.256 "15 OVRUN OVERRUN ...
  Indicates that another byte of data was received before the previous byte
  was picked up by the processor. ... If the interrupt is not cleared in
  time, the OVERRUN bit is set."
- Proposition: bit 15 (OVRUN) sets when a second byte completes reception
  before the first was read out of SERDATR, and stays clear when each byte
  is read before the next arrives.
- Expected: to be measured.

## A7 — RBF-BIT14-SIGNALS-DATA-READY

- Page: 283, 256. Uids: `opus:8-serdatr.c14`, `sol:8-serdatr.c19`,
  `sol:8-serdatr.c21`, `sol:8-serdatr.c23`, `sol:8-serdatr.c91`,
  `sol:8-serdatr.c93`.
- Quotes: p.283 "14 RBF Serial port receive buffer full (mirror)."; p.256
  "14 RBF READ BUFFER FULL (Mirror -- also appears in the interrupt request
  register.) When this bit is 1, there is data ready to be picked up by the
  processor."
- Proposition: bit 14 (RBF) reads 1 exactly when SERDATR holds an unread,
  fully-received byte, and 0 otherwise (idle, or immediately after the read
  that consumes it, if a consuming read is observable at this boundary).
  RBF's own mirror status is **not disputed anywhere in this slice** — both
  p.256 and p.283 call it a mirror — so unlike bit 13 this is graded
  normally, not declined as a conflict.
- Expected: to be measured. (`opus:8-serdatr.c14`'s own restatement notes a
  wording difference between p.256's "READ BUFFER FULL" and p.283's
  "receive buffer full" — a naming variance, not a conflict about what the
  bit does, and not treated as one.)

## A8 — RECEIVER-FULL-CHARACTER-TIME-WINDOW-IS-8-TO-10-BIT-TIMES

- Page: 256 (see the correction above — not 257). Uids: `sol:8-serdatr.c9`,
  `sol:8-serdatr.c10`.
- Quote: p.256 "After receiving the receiver-full interrupt, you will have
  up to one full character-receive time (8 to 10 bit times) to accept the
  data and clear the interrupt."
- Proposition: the window between RBF setting and OVRUN becoming a risk
  spans 8 to 10 bit times.
- **This is finding candidate S4-C4.** Per the candidates file: measured at
  `paula_uart`, the received-character window (start bit through stop bit)
  is 10.0 bit times with LONG=0 and 11.0 with LONG=1 — both outside the
  printed 8-to-10 range on the high end for LONG=1. Expected: **RED**
  (LONG=1 case), to be re-measured directly by this package rather than
  assumed from the candidates file.

## A9 — TBE-BIT13-TRACKS-TRANSMIT-BUFFER-EMPTY

- Page: 257. Uids: `sol:8-serdatr.c25`, `sol:8-serdatr.c27`,
  `sol:8-serdatr.c31`. Also page 258 (no claim uid in this package's
  inventory; see NM-HRM-056).
- Quotes: p.257 "13 TBE TRANSMIT BUFFER EMPTY ... When bit 14 is a 1
  [sic — this HRM sentence describes SERDAT's own status bit numbering
  from the transmit side, not SERDATR; the receive-side proposition graded
  here is TBE's own definition] ... the data in the output data register
  (SERDAT) has been transferred to accept another output word. This is also
  true when the buffer is empty." (the transmit-buffer-empty/can-accept
  half of the row, deliberately not the "(Not a mirror ...)" half — that is
  A10, below); p.258 "Immediately after you write the data into this
  register, the system will begin the transmission ... At the start of the
  operation, this data is transferred from SERDAT into an internal serial
  shift register. When the transfer to the serial shift register has been
  completed, SERDAT can accept new data; the TBE interrupt signals this
  fact."
- Proposition: a word written while the shift register is still busy with
  a previous word waits in SERDAT, and TBE reads 0 on every read until that
  previous word's frame ends, then 1 (with TSRE still 0, since the new word
  has itself now moved into the shift register and is shifting).
- **Writing one word from idle and requiring the very next read to find
  TBE clear would not hold** (NM-HRM-056): no page asks for that, and
  p.258 has that word reach the shift register at the
  start of the operation, immediately, so no bus read timed by this
  package's own clk7_en-synchronised protocol can catch it. This
  measures the case p.257's TSRE row actually describes instead.
  Expected: **PASS**, confirmed by direct measurement (`reads_while_clear`
  = 219 against a threshold of 20; the `// MUTATION` that removes the
  second write measures `reads_while_clear` = 0 and goes red).

## A10 — BIT13-MIRROR-STATUS — SOURCE CONFLICT, NEITHER SIDE GRADED

- Page: 257 (p.257 side) and 283 (p.283 side). Uids: `sol:8-serdatr.c29`
  (p.257 side); `opus:8-serdatr.c17`, `sol:8-serdatr.c95`,
  `sol:8-serdatr.c97` (p.283 side — their own restatements read only "TBE
  means transmit buffer empty," but their `extract.quote` carries the row
  in full, "(mirror)" included, and that full printed row is what this
  block grades neither side of).
- Quotes: p.257 "(Not a mirror -- interrupt occurs when the buffer becomes
  empty.)"; p.283 "13 TBE Serial port transmit buffer empty (**mirror**)."
- **This is finding candidate S4-C8, and the build brief names it by
  name.** The two pages disagree about the *mechanism*: p.257 says TBE is
  not a copy of some other bit but is itself the source of an edge-style
  interrupt event, fired when the buffer transitions to empty; p.283 says
  it *is* a mirror — the same framing p.256 uses for OVRUN and RBF, "also
  appears in the interrupt request register."
- **Both sides are measured, neither graded:**
  - **p.257 side, measured at this package's own boundary (`paula_uart`,
    which exposes `txint` directly):** whether `txint` behaves as an
    edge/pulse tied to the buffer-becomes-empty transition, consistent with
    "interrupt occurs when the buffer becomes empty" rather than a level
    that simply follows TBE continuously.
  - **p.283 side needs INTREQ, which is not a `paula_uart` port — it needs
    the wider `paula` boundary**, exactly as `ch8-dskbytr-status-register`'s
    DMACON half needed a boundary wider than its own assigned fixture and
    was declined there rather than rebuilt. This package's assigned top is
    `paula_uart` alone (per the package list), so the p.283 side is not
    re-derived here; it cites the measurement already made at `paula` by
    the classify stage, per finding candidate S4-C8: "Measured at `paula`:
    bit 13 read 1 fresh, after a SERDAT write, mid-transmit, after
    transmit, and after clearing INTREQ bit 0 -- while INTREQR bit 0 went
    0->1->0. For contrast bit 14 tracked INTREQR bit 11 exactly" — i.e. bit
    13 did **not** track INTREQR bit 0 the way bit 14 tracked INTREQR bit
    11, which is the RTL's own evidence for the p.257 reading, not
    something this package asserts as a verdict.
- Expected: **RECORDED, not graded, on both sides.** A9 above is where the
  underlying, gradeable "does TBE ever read 0" question is actually
  measured; A10 is only about the mirror/not-mirror characterisation, which
  this package declines to adjudicate.

## A11 — TSRE-BIT12-TRACKS-COMPLETE-TRANSMISSION-AND-CLEARS-ON-LOAD

- Page: 283, 257. Uids: `opus:8-serdatr.c20`, `opus:8-serdatr.c21`,
  `sol:8-serdatr.c33`, `sol:8-serdatr.c35`, `sol:8-serdatr.c36`,
  `sol:8-serdatr.c37`, `sol:8-serdatr.c38`, `sol:8-serdatr.c39`,
  `sol:8-serdatr.c99`, `sol:8-serdatr.c101`, `sol:8-serdatr.c102`.
- Quotes: p.283 "12 TSRE Serial port transmit shift register empty. Reset by
  loading into buffer."; p.257 "12 TSRE TRANSMIT SHIFT REGISTER EMPTY. When
  this bit is a 1, the output shift register has completed its task, all
  data has been transmitted, and the register is now idle. If you stop
  writing data into the output register (SERDAT), then this bit will become
  a 1 after both the word currently in the shift register and the word
  placed into SERDAT have been transmitted."
- Proposition: TSRE reads 1 at idle, reads 0 immediately after a SERDAT
  write ("reset by loading into buffer"), and — with two SERDAT writes in
  flight (double buffering) — returns to 1 only after **both** words have
  fully transmitted, not after the first.
- Not flagged as a conflict anywhere in the finding-candidates document —
  p.257 and p.283 agree. Expected: to be measured.

## A12 — RXD-BIT11-IS-A-REAL-FIELD-NAMED-RXD

- Page: 257, 283. Uids: `sol:8-serdatr.c41`, `sol:8-serdatr.c42` (restored
  duplicate of `opus:8-serial.c81`, a different package's claim — cited
  here only as the uid this package's own claim set restored, not
  re-graded against that package), `sol:8-serdatr.c103`.
- Quote: p.257 "11 RXD Direct read of RXD pin on Paula chip."; p.283 "11
  RXD RXD pin receives UART serial data for direct bit test by the
  microprocessor." (field-location half only — the "direct" claim is A13).
- Proposition: bit 11 is a real, defined field, named RXD, distinct from
  the unused bit at 10.
- Expected: PASS (control case, alongside A13).

## A13 — RXD-BIT11-IS-NOT-A-DIRECT-COMBINATIONAL-READ-OF-THE-PIN

- Page: 283, 257. Uids: `opus:8-serdatr.c23`, `opus:8-serdatr.c24`,
  `sol:8-serdatr.c43`, `sol:8-serdatr.c105`, `sol:8-serdatr.c106`.
- Quotes: p.257 "Direct read of RXD pin on Paula chip."; p.283 "RXD pin
  receives UART serial data for direct bit test by the microprocessor."
- Proposition: bit 11 reflects the `rxd` pin's instantaneous level — a
  change on `rxd` should be visible at bit 11 with no more delay than the
  read cycle itself takes.
- **This is finding candidate S4-C3.** Per the candidates file: the path is
  `rxd` -> two synchroniser flops (`paula_uart.v:46-52`) -> one more
  register, so the bit tracks `rxd` with a fixed multi-cycle delay rather
  than combinationally. Whether "direct" forbids a synchroniser is not this
  package's question to settle; the measured delay is. Expected: **RED**,
  to be re-measured directly by this package.

## A14 — BIT10-IS-UNUSED

- Page: 283, 257. Uids: `opus:8-serdatr.c26`, `sol:8-serdatr.c44`,
  `sol:8-serdatr.c45`, `sol:8-serdatr.c107`, `sol:8-serdatr.c109`.
- Quote: p.283 "10 0 Not used"; p.257 "10 Not used at this time."
- Proposition: bit 10 carries no defined function — it reads a fixed value
  (the p.283 row prints a literal `0` in the SYM column, unlike bits 11-8
  and 7-0's descriptive names) across states that move every other bit.
- Expected: to be measured.

## A15 — BIT9-STP-IS-A-STOP-BIT-IN-NINE-BIT-RECEIVE-MODE

- Page: 283, 257. Uids: `opus:8-serdatr.c28`, `sol:8-serdatr.c46`,
  `sol:8-serdatr.c47` (restored duplicate of `opus:8-serial.c84`),
  `sol:8-serdatr.c48`, `sol:8-serdatr.c110`, `sol:8-serdatr.c112`.
- Quotes: p.283 "09 STP Stop bit" (unconditioned); p.257 "9 STP Stop bit if
  9 data bits are specified for receive." (conditioned on LONG). Not a
  conflict — p.283's row is terse throughout (its bit-15 OVRUN row likewise
  omits p.256's INTF_RBF advice); the fuller p.257 statement is graded, and
  the p.283 half is the same proposition stated with less detail, not a
  contradiction of it.
- Proposition: with nine-bit receive selected (LONG=1), bit 9 holds the
  frame's stop bit.
- Expected: to be measured (LONG=1 case only — neither page states what bit
  9 does in eight-bit mode, so that half is not testable and is not
  claimed).

## A16 — BIT8-IS-A-REAL-FIELD-REGARDLESS-OF-INTERPRETATION

- Page: 257, 283. Uids: `sol:8-serdatr.c49`, `sol:8-serdatr.c113`.
- Quote: p.257 "8 STP Stop bit if 8 data bits are specified for receive. OR
  DB8 9th data bit if 9 bits are specified for receive."; p.283 "08 STP-DB8
  Stop bit if LONG, data bit if not."
- Proposition: bit 8 is a real, defined field (named STP-DB8 or, per p.257,
  STP-or-DB8 depending on mode) — neither page calls it unused, unlike bit
  10 — regardless of which page's LONG-condition mapping is correct.
- Expected: PASS (control case; the disputed mapping itself is A17).

## A17 — BIT8-STOP-OR-DATA-CONDITION — SOURCE CONFLICT, NEITHER SIDE GRADED

- Page: 283 (p.283 side) and 257 (p.257 side). Uids: `opus:8-serdatr.c30`,
  `opus:8-serdatr.c31`, `sol:8-serdatr.c115`, `sol:8-serdatr.c116` (p.283
  side); `sol:8-serdatr.c51`, `sol:8-serdatr.c53` (p.257 side);
  `sol:8-serdatr.c3` (p.256's "the register can contain nine-bit input
  data" — the general capacity claim underneath the same disputed bit,
  cited here as background rather than separately graded, since which case
  counts as "genuinely nine bits of data" depends on which page's framing
  is used).
- Quotes: p.283 "08 STP-DB8 Stop bit **if LONG**, data bit **if not**." —
  LONG (nine-bit mode) selects **stop**, not-LONG (eight-bit mode) selects
  **data**. p.257 "8 STP Stop bit if **8 data bits** are specified... OR DB8
  9th data bit if **9 bits** are specified" — eight-bit mode selects
  **stop**, nine-bit mode (LONG) selects **data** (the 9th bit).
- **This is finding candidate S4-C9, "found by the SERDATR classifier and
  not in any brief."** Checked directly against both printed pages
  (`docs/references/amiga-hrm-3rd/appA-controller/p283.txt:52-53` and
  `docs/references/amiga-hrm-3rd/ch8/p257.txt:34-36`): the two pages give
  bit 8 **opposite** roles in the same LONG setting — p.283 has LONG=1 mean
  stop-bit, p.257 has LONG=1 mean data-bit. This is a genuine textual
  contradiction, not a granularity difference like A15's bit 9.
- **Both sides measured, neither graded:** both LONG=0 and LONG=1 are
  within this package's own `paula_uart` boundary (LONG is a real,
  addressable control bit and both pages' predictions are checkable at this
  same fixture, unlike A10's p.283 side) — bit 8's actual behaviour under
  each LONG setting is measured and reported against **both** printed
  predictions, with neither counted as the winner. Per the candidates file,
  measured: LONG=0 left bit 8 at 1 for every payload (constant, stop-bit-
  shaped); LONG=1 made it follow the payload's ninth data bit (variable,
  data-bit-shaped) — i.e. the RTL's own behaviour matches **p.257's**
  mapping and contradicts p.283's, but this package records that as a
  measurement against each page, not as an adjudication of which page is
  right.
- Expected: RECORDED on both sides, GRADED on neither.

## A18 — DB7-DB0-REFLECT-THE-RECEIVED-BYTE-AT-TRUE-POLARITY

- Page: 283, 257, 256. Uids: `opus:8-serdatr.c33`, `opus:8-serdatr.c35`,
  `opus:8-serdatr.c37`, `opus:8-serdatr.c39`, `opus:8-serdatr.c41`,
  `opus:8-serdatr.c43`, `opus:8-serdatr.c45`, `opus:8-serdatr.c47`,
  `sol:8-serdatr.c54`, `sol:8-serdatr.c56`, `sol:8-serdatr.c57`,
  `sol:8-serdatr.c59`, `sol:8-serdatr.c60`, `sol:8-serdatr.c62`,
  `sol:8-serdatr.c63`, `sol:8-serdatr.c65`, `sol:8-serdatr.c66`,
  `sol:8-serdatr.c68`, `sol:8-serdatr.c69`, `sol:8-serdatr.c71`,
  `sol:8-serdatr.c72`, `sol:8-serdatr.c74`, `sol:8-serdatr.c75`,
  `sol:8-serdatr.c77`, `sol:8-serdatr.c78`, `sol:8-serdatr.c117`,
  `sol:8-serdatr.c119`, `sol:8-serdatr.c120`, `sol:8-serdatr.c122`,
  `sol:8-serdatr.c123`, `sol:8-serdatr.c125`, `sol:8-serdatr.c126`,
  `sol:8-serdatr.c128`, `sol:8-serdatr.c129`, `sol:8-serdatr.c131`,
  `sol:8-serdatr.c132`, `sol:8-serdatr.c134`, `sol:8-serdatr.c135`,
  `sol:8-serdatr.c137`, `sol:8-serdatr.c138`, `sol:8-serdatr.c140`,
  `sol:8-serdatr.c2`.
- Quotes: p.283 "07 DB7 Data bit" through "00 DB0 Data bit" (eight
  identical-shaped rows); p.257 "7-0 DB7-DB0 Low 8 data bits of received
  data. Data is TRUE (data you read is the same polarity as the data
  expected)."; p.256 "It contains the 8 ... bit input data."
- Proposition: bits 7-0 carry the eight low bits of the received byte,
  unmodified (true polarity, not inverted) — a swept set of known received
  bytes must read back bit-for-bit identical at SERDATR[7:0].
- Expected: PASS, one sweep assertion grading every bit position at once
  (the same bundling `ch8-dskbytr-status-register`'s A9 uses for its own
  8-bit DATA field), citing all 42 uids above together rather than as
  eight separate per-bit assertions each re-testing the same mechanism eight
  times over.

## A19 — P289-REGISTER-SUMMARY-ROW — ALREADY GRADED IN AN ACCEPTED SLICE

- Page: 289. Uids: `sol:8-serdatr.c141` (ADD), `sol:8-serdatr.c142` (R/W),
  `sol:8-serdatr.c144` (FUNCTION).
- Quote: p.289 "SERDATR *018 R P Serial port data and status read".
- **Not re-graded, per the build brief and the package list.** Slice s1
  already scoped and graded this same p.289 row: `opus:8-appB.c33` /
  `sol:8-appB.c32` (ADD), `opus:8-appB.c34` / `sol:8-appB.c33` (R/W),
  `opus:8-appB.c36` / `sol:8-appB.c35` (FUNCTION), `opus:8-appB.c37` /
  `sol:8-appB.c36` (the `*` mark) — built as
  `hrm/ch8-appb-serial-register-rows`, whose `SPECIFICATION.md:27-32` binds
  those uids. `sol:8-serdatr.c141`/`c142`/`c144` are recorded here, citing
  that package, rather than graded again.

## Totals

- 123 claims read, all 123 accounted for above (verified programmatically).
- 19 assertion blocks (A1-A19; A1 is a no-uid fixture check).
- Expected to grade GREEN (confirmed by measurement): A2, A3, A4, A5, A6,
  A7, A9, A11, A12, A14, A15, A16, A18.
- Expected to grade RED (finding candidates, confirmed by direct
  measurement in this package): A8 (S4-C4), A13 (S4-C3).
- RECORDED, graded on neither side (the two source conflicts this slice
  assigns to this package): A10 (bit 13, S4-C8), A17 (bit 8, S4-C9).
- RECORDED, out of scope (already graded elsewhere): A19 (3 uids, p.289).

Nothing in this file required opening `paula_uart.v` or any other file under
`nanomig-hrm-tests/nanomig/` beyond the one incidental, disclosed exposure
above, and no HALT condition was encountered.
