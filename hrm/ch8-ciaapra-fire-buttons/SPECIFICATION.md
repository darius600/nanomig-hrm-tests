# ch8-ciaapra-fire-buttons

Source: claims file `ch8-ciaapra-fire-buttons.jsonl` (10 claims), quoting the
Commodore Amiga Hardware Reference Manual, printed pages 232-233.

Register under discussion: **CIAAPRA** ($BFE001).

Fixture: `ciaa`, compiled alone. No other module is part of this package's
boundary.

This document is written from the claims file's `extract.quote` /
`extract.page` / `extract.restatement` fields only. No RTL file has been
opened yet; port names, bit widths and the exact stimulus mechanism are
deliberately not decided here.

## Propositions

### P1 — a logic 1 on a CIAAPRA controller-port bit means "switch open" (p.232)

Quote (p.232): *"...($BFE001). Port 1 uses bit 6 and port 2 uses bit 7. A
logic state of 1 means "switch open." A logic state of 0 means "switch..."*

- `opus:8-mbtn.c7` — "A CIAAPRA button bit reading 1 means the switch is
  open."
- `sol:8-button.c6` — "A CIAAPRA left-mouse-button value of 1 means the
  switch is open."

### P2 — a logic 0 on a CIAAPRA controller-port bit means "switch closed" (p.232)

Quote (p.232): *"state of 1 means "switch open." A logic state of 0 means
"switch closed." (See Appendix F for more information.)"*

- `opus:8-mbtn.c8` — "A CIAAPRA button bit reading 0 means the switch is
  closed."
- `sol:8-button.c7` — "A CIAAPRA left-mouse-button value of 0 means the
  switch is closed."

### P3 — the two controller ports' fire buttons sit on two distinct CIAAPRA bits, 6 and 7 — KNOWN CONFLICT 3

Quote (p.232): *"Port 1 uses bit 6 and port 2 uses bit 7."*

Quote (p.233): *"The fire buttons for ports 0 and 1 are connected to bits 6
and 7 of CIAAPRA ($BFE001). A 0 here indicates the switch is closed."*

- `opus:8-joy2.c1` — "The fire button of port 0 is connected to bit 6 of
  CIAAPRA."
- `opus:8-joy2.c2` — "The fire button of port 1 is connected to bit 7 of
  CIAAPRA."
- `sol:8-joy.c17` — "Controller port 0's fire button is connected to CIAAPRA
  bit 6."
- `sol:8-joy.c18` — "Controller port 1's fire button is connected to CIAAPRA
  bit 7."

**KNOWN CONFLICT 3** (carried forward): printed p.233 numbers the controller
ports 0 and 1, where p.232 and the rest of the chapter (p.240, and per the
claims file's own conflict note also pp.234-237) number the very same two
ports 1 and 2. The page does not agree with itself on which numeral names
which port.

What the two pages do **not** disagree on: there are exactly two ports, each
reports its fire button on exactly one CIAAPRA bit, and the port named first
on the page maps to bit 6 while the port named second maps to bit 7 — i.e.
bit 6 and bit 7 belong to two distinct ports, whichever numerals those ports
carry. This package grades that undisputed core for all four uids above —
two independent single-bit reports, one on bit 6 and one on bit 7 — and does
not grade, for any of them, which decimal label (0, 1 or 2) attaches to
which bit, since the printed page contradicts itself on that point.

### P4 — restated at the point of connection: 0 on the fire-button bit means the switch is closed (p.233)

Quote (p.233): *"A 0 here indicates the switch is closed."*

- `opus:8-joy2.c3` — "A 0 in the CIAAPRA fire-button bit means the
  fire-button switch is closed."
- `sol:8-joy.c19` — "A 0 in the relevant CIAAPRA fire-button bit indicates a
  closed switch."

**Scope note.** This sentence chains two separate facts: (i) the CIAAPRA bit
reflects the pin it is wired to, and (ii) a mechanically-closed fire-button
switch drives that pin to the low logic level. Fact (i) is internal to the
CIA chip. Fact (ii) is a property of the wiring between the controller-port
connector and the chip: a CIA parallel-port input pin has no way to know
*why* it is low, only that it is — that is true of any parallel-port chip
regardless of which design implements it, not a fact learned from this
project's RTL. The `ciaa` fixture, compiled alone, is exactly the chip's own
boundary and nothing past it, so it cannot settle fact (ii).

This is the same shape of boundary argument that routes CIAAPRA *address*
claims to `ch8-ciaapra-address-window` (a rig over `ciaa` + `gary`) rather
than to a `ciaa`-alone fixture, which has no address decoder. No claim in
this claims file makes an address assertion, so nothing is declined on that
particular basis here — but P4 is declined on the analogous "outside this
module's boundary" basis. See "Declined claims" below.

## Declined claims

- `opus:8-joy2.c3`, `sol:8-joy.c19` — P4's full "closed switch -> bit reads
  0" chain needs the controller-port-to-pin wiring, which sits outside
  `ciaa` alone (see scope note above). Declined, not re-measured with a
  weakened restatement.

## Carried forward from `hrm/ch8-ciaa-port-direction` (already built, green, same fixture)

- `hrm/ch5-audio-filter-bypass` already builds this fixture and already
  asserts CIA-A PRA bit 1 (the LED / audio-filter bit). This package does
  not touch bit 1 and makes no assertion about it.
- `ch8-ciaa-port-direction` declined `opus:8-tbl8-1.c36`, `sol:8-pin.c40`
  (p.228 Table 8-1's pin-6 footnote) and `opus:8-dio.c42`, `sol:8-dio.c41`
  (p.241, fire buttons configurable as outputs) to this package by name.
  None of the four appear in `ch8-ciaapra-fire-buttons.jsonl` (whose 10
  claims are `8-mbtn.c7`, `8-mbtn.c8`, `8-button.c6`, `8-button.c7`,
  `8-joy2.c1`, `8-joy2.c2`, `8-joy2.c3`, `8-joy.c17`, `8-joy.c18`,
  `8-joy.c19`) — they are not this package's to build. Noted rather than
  invented.
- `ch8-ciaapra-address-window` owns WHERE CIAAPRA answers on the bus; no
  claim in this file asserts an address, so nothing is declined on that
  basis here (see P4's scope note for the analogous boundary argument that
  *is* used here).

## Assertions planned (uid -> verdict), pending the RTL read

| uid | page | verdict | what is graded |
|---|---|---|---|
| `opus:8-mbtn.c7` | 232 | expect | its bit reads 1 when its pin is driven to the manual's "open" level |
| `opus:8-mbtn.c8` | 232 | expect | its bit reads 0 when its pin is driven to the manual's "closed" level |
| `sol:8-button.c6` | 232 | expect | twin of `8-mbtn.c7`, graded independently under its own uid |
| `sol:8-button.c7` | 232 | expect | twin of `8-mbtn.c8`, graded independently under its own uid |
| `opus:8-joy2.c1` | 233 | expect | bit 6 follows only its own pin, independent of bit 7's pin (undisputed core of P3) |
| `opus:8-joy2.c2` | 233 | expect | bit 7 follows only its own pin, independent of bit 6's pin (undisputed core of P3) |
| `sol:8-joy.c17` | 233 | expect | twin of `8-joy2.c1` |
| `sol:8-joy.c18` | 233 | expect | twin of `8-joy2.c2` |
| `opus:8-joy2.c3` | 233 | record (decline) | out of `ciaa`-alone's boundary; see P4 scope note |
| `sol:8-joy.c19` | 233 | record (decline) | same as `opus:8-joy2.c3` |

Before any of the above are graded, the fixture must be shown to deliver a
known, distinguishable, non-default word on its input pins and read it back
correctly — a control case, on the same measurement basis as the claims
above, proving the harness's data path is live rather than silently stuck.

Port names, the CIAAPRA register-select encoding, and how many pins `porta`
actually exposes are unknown at this point and are read from `ciaa.v` (and
whatever sibling files Verilator needs to elaborate it) only after this
document is committed.
