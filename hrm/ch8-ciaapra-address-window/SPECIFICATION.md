# ch8-ciaapra-address-window — SPECIFICATION

Written from the quotations in `ch8-ciaapra-address-window.jsonl` alone. No RTL,
Makefile, or other package's files have been opened yet. This file is committed
before any of that happens.

## Source claims (verbatim quotations)

| uid | page | quote |
|---|---|---|
| `opus:8-regs.c5` | 229 | "CIAAPRA ($BFE001) Input and output for pin 6 (port 1 & 2 fire buttons)" |
| `sol:8-reg.c5` | 229 | "CIAAPRA ($BFE001) Input and output for pin 6 (port 1 & 2 fire buttons)" (twin of `opus:8-regs.c5`) |
| `opus:8-mbtn.c4` | 232 | "The left button on the Amiga mouse is connected to CIAAPRA ($BFE001). Port 1 uses bit 6 and port 2 uses bit 7. A logic" |
| `sol:8-button.c3` | 232 | "The left button on the Amiga mouse is connected to CIAAPRA ($BFE001). Port 1 uses bit 6 and port 2 uses bit 7." |

Each classified restatement reduces to the same testable proposition:

- `opus:8-regs.c5` — "CIAAPRA is at address $BFE001."
- `sol:8-reg.c5` — "CIAAPRA is located at $BFE001."
- `opus:8-mbtn.c4` — "CIAAPRA is at address $BFE001."
- `sol:8-button.c3` — "The left mouse button is connected to CIAAPRA at $BFE001."

**The one proposition under test in this package is: CIAAPRA answers at CPU bus
address $BFE001.** The bit-level content that pages 229/232 also carry (which
port uses which bit, fire buttons, mouse button polarity) is a *different*
proposition and is out of this package's scope — the task names
`hrm/ch8-ciaa-port-direction` and `hrm/ch8-ciaapra-fire-buttons` as already
built and green on `ciaa` alone for that content.

## Declined-elsewhere uids — checked, not present

The task names three uids as address claims that two sibling builders declined
by name: `opus:8-dio.c43` ("CIAADDRA is at $BFE201"), `opus:8-dio.c45`
("CIAAPRA ($BFE001) is the data read register"), and the address half of
`sol:8-dio.c42`. **None of these three appear in
`ch8-ciaapra-address-window.jsonl`.** That file holds exactly the four uids
listed above. I am not asserting, declining, or otherwise disposing of
`opus:8-dio.c43`, `opus:8-dio.c45`, or `sol:8-dio.c42` in this package — they
are not mine to dispose of, and I have not invented lines for them.

## Why `ciaa` alone cannot settle these claims

`ciaa.v`'s own external interface (per the two already-built sibling packages)
is an internal register-select input, not a CPU bus address. Driving that
register-select input directly to the value that happens to select the PRA
register would make every one of these four assertions pass **regardless of
whether the manual's number is $BFE001, $BFE101, or anything else** — the
check would be vacuous with respect to the actual proposition, which is about
an address, not about an internal register index. A single-module `ciaa`
fixture cannot distinguish "the manual's address claim is true" from "I typed
the register-select value the assertion expected."

This package's own claims file carries the reason a second module is needed,
as an evidentiary note from a prior RTL reading (`classify.evidence`, all four
claims, `observable_detail`: "ciaa for the register select, gary for the
$BFE00x window"): the $BFE001 window is decoded a level up from `ciaa`, in
`gary`, which is reported to compute `ciaa`'s chip-enable from the CPU address
and to hand `ciaa` its register-select value from a different slice of that
same address. I have not opened `gary.v` or `minimig.v` myself to confirm this
— that citation is disclosed here as the justification for the rig boundary,
not as a fact I have personally verified from RTL. Verification happens in the
next step, after this file is committed.

A rig over `ciaa` + `gary`, wired the way `minimig.v` wires them, is therefore
the minimum boundary that can let a CPU-bus-address stimulus reach `ciaa`'s
answer through an independent decode path, rather than through a hand-set
register-select value chosen to match the expectation.

## Boundary risk: $BFE001 may not be fully reachable at this rig

The task is explicit that $BFE001 is a CPU-bus address, not a chip-register
offset, and that if the decode observable at this rig does not reach that far,
the correct action is to name the module that would carry the rest and decline
with `record()` — not to add a third module to the rig on my own judgment.

I do not yet know how much of the 24-bit CPU address `gary.v` itself resolves
versus how much it receives pre-resolved (e.g. a `sel_cia`-class enable
computed by some other module before it reaches `gary`). That is resolved by
reading RTL in the next step. If `gary` turns out not to own the full decode
down to the literal value $BFE001, the affected assertion(s) below will be
declined via `record()`, naming the module that would be needed and the reason
the rig was not widened to include it.

## Assertions — one per citing quotation, each naming its page

All four grade the same measured fact (CPU-bus address $BFE001, presented to
the ciaa+gary rig on a read cycle, causes `ciaa`'s Port A data register — the
register `ciaa` answers with when its internal register-select equals the PRA
index — to be the register that responds), each from its own citing passage:

- **A1** — `opus:8-regs.c5`, page 229
- **A2** — `sol:8-reg.c5`, page 229
- **A3** — `opus:8-mbtn.c4`, page 232
- **A4** — `sol:8-button.c3`, page 232

A precondition each of these depends on — that the rig actually asserts
`ciaa`'s chip-enable for the driven address, i.e. that the decode path is live
at all for this address and not just coincidentally producing the right
register-select value while the chip stays deselected — is asserted
separately rather than assumed.

## Control case

At least one assertion, on the same measurement basis (a CPU-bus address
driven through the same rig, read the same way), must be expected to PASS
independent of whether A1–A4 turn out red or green: that presenting a
*different* address within the same chip-select window, corresponding to a
different internal register than PRA, causes `ciaa` to answer with that
different, distinct register rather than with PRA. This is what proves the
fixture's address-to-register data path is live end to end (per the mandate
that a fixture must prove it can deliver a known, distinct word before
anything reads one) — a dead address decode that happened to leave `ciaa`
stuck answering PRA on every input would otherwise pass A1–A4 for the wrong
reason.

## Mutation plan

Each of A1–A4 gets its own
`// MUTATION <label> stimulus <red|green> <what changed>` line in the file
header, `<label>` being the exact string passed to `expect()` for that
assertion. The planned stimulus mutation is: change the driven CPU address's
register-select field so it no longer selects the PRA index (asking the rig a
different question), leaving the expected PRA value alone, and confirm the
assertion goes red, then revert.

If, once built, any of A1–A4 is itself red against the RTL (the oracle is the
manual; a disagreeing RTL is a valid, finished red, not something to adjust),
the substitute rule applies instead: a different mechanism proven live
elsewhere in the same run is put through the same comparator and shown to go
green, rather than mutating an already-red assertion.

## What happens next

Only after this file is committed: read `minimig.v` for the `ciaa`+`gary`
wiring, read `hrm/ch8-ciaa-port-direction/Makefile` for the build pattern
(`cia_int.v`, `cia_timera.v`, `cia_timerb.v`, `cia_timerd.v` alongside
`ciaa.v`), and read `gary.v` and `ciaa.v` themselves to determine the actual
port names and whether the rig reaches $BFE001 in full or only in part. All of
that reading — and anything else opened beyond what is named above — will be
disclosed in the next commit.
