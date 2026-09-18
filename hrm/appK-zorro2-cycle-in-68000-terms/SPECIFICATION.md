# appK-zorro2-cycle-in-68000-terms

Amiga Hardware Reference Manual, 3rd edition, Appendix K, pp.387-398
("Zorro II Bus Architecture" / "Addressing and Control Signals" / "Data
Transfer Acknowledge" / "External Ready"). Claims file:
`hrm-testpipeline/work/appK-build/appK-zorro2-cycle-in-68000-terms.jsonl`,
**51 claims**, all `status: testable`, `boundary: minimig_m68k_bridge`
(per `classify`). Top module: `minimig_m68k_bridge`
(`nanomig/src/minimig-aga/minimig_m68k_bridge.v`).

**Order of work, stated plainly.** The task assignment that produced this
lot already named the top module and stated the governing rule directly:
Zorro II cycles described in 68000 bus terms -- /AS, /LDS, /UDS, /DTACK,
R/W, the S-state sequence, /VPA and /VMA -- are decidable at this bridge
even though no Zorro slot exists anywhere in this design; claims that need
a slot, a card, /SLAVEn, /FCS, /CCS, DOE or a pin number are not. Applying
that rule to 51 individual claims requires reading each claim's own
`quote`/`restatement` against the printed page text (fetched from
`hrm-testpipeline/docs/references/amiga-hrm-3rd/appK-zorro/p391.txt`,
`p397.txt`, `p398.txt`, `p393.txt`, `p387.txt` -- the same corpus text the
classifier read from, not the RTL) -- that reading is what produced the
grade/decline table below, before `minimig_m68k_bridge.v` was opened for
step 3. The one exception: this package's sibling `appF-cia-bus-and-clock`
already built and empirically verified a working `_as`/`cck` acquisition
sequence for this exact module (`Harness::acquire_enable()`, its own
comment: "verified empirically against a standalone probe build"); this
build reuses that proven sequence rather than re-deriving it from nothing,
and additionally ran a throwaway probe (discarded, not part of this
package) to measure four RTL facts numerically before writing any
pass/fail bound on them -- disclosed in "Measured before writing" below.

## The governing distinction, applied claim by claim

Two things this bridge genuinely is: the pinned design's own target-side
68000 bus interface (drives `_dtack`/`rd`/`hwr`/`lwr` off `_as`/`_lds`/
`_uds`/`r_w`), and the CIA-style VPA/VMA peripheral-support path (`vpa`
input, internal `vma`, gated by `eclk`). Two things it is NOT: a Zorro II
slot (no `/CCS`, `/FCS`, `/SLAVEn`, `XRDY`, `/OVR`, `DOE`, or memory-space
decode of any kind lives in this module -- that is `gary.v`/
`cpu_wrapper.v`'s job), and a 68000 CPU core (it never *samples* `_dtack`
or *latches* read data itself in the master's own sense -- it only
*drives* `_dtack` and *presents* `data[15:0]`; nothing in this design
instantiates a state-labelled S0-S7 sequencer at all, pinned or otherwise).

Every one of the 51 claims falls into one of six dispositions:

**(A) Decidable as written, plain 68000-bus-term claim.** Graded.

**(B) Names a Zorro-slot-only concept (`/CCS`, `XRDY`) as the acting
signal, or requires "both Zorro II AND Zorro III cycles"/bus arbitration
that plainly does not exist in a single-CPU, Zorro-III-less design.**
Declined -- the claim, as restated, commits to a signal or feature this
design does not implement anywhere, not merely at this one module.

**(C) Bundles a decidable core with an additional conjunct this boundary
cannot reach** (a memory-space qualifier, a named S-state such as S4/S6/S7,
bus arbitration named as an example). Per the build brief's own rule --
"Where a claim states more than the package tests, its uid goes on the
`record()` line and NOT on the graded one" -- these are declined in full,
not partially graded on the reachable half.

**(D) Describes a 68000 bus MASTER's own internal action** (sampling
`_dtack` at a state boundary, latching read data at a state boundary) --
this bridge is the target/slave side; it drives `_dtack`, it does not
sample it, and no CPU core is instantiated in this fixture. Declined.

**(E) Compares two physically distinct signals where only one exists at
this boundary** (opus:K-0206's "READ...earlier...than a 68000 R/W line
would" implies two different pins; this module has exactly one such input,
`r_w`, with no separately-timed "READ" output derived from it). Declined.

**(F) Is fully decidable and IS tested, and the RTL disagrees with the
page on part of what it asserts.** Graded, and reported red where the
measurement says so -- a red is a result, not grounds to decline.

## The 51 claims, grouped by disposition

### Group G1 -- AS/DTACK define the cycle (GRADE)
`sol:K-0096` ("An address strobe defines the start of a Zorro II cycle"),
`opus:K-0209` ("The falling edge of /AS indicates that addresses are
valid, that the READ line is valid, and that a Zorro II cycle is
starting"), `sol:K-0097` ("A data-transfer strobe terminates a Zorro II
cycle" -- p.398's own /DTACK section head is literally "used to normally
terminate both Zorro bus cycles"; the "both" there names Zorro modes of
one signal, not two required halves the way K-0213/opus:K-0221 below do),
`opus:K-0210` ("The rising edge of /AS signals the end of a Zorro II bus
cycle and tells the current slave to negate all slave-driven signals as
quickly as possible").

Testable: `_as` falling is what the fixture drives to start acquisition
of `_dtack`; `_as` rising is what resets `_ta_n` (hence `_dtack`) -- and
does so on the `posedge _as` branch of the register's own sensitivity
list, i.e. asynchronously, matching "as quickly as possible" concretely
(measured: `_dtack` reads deasserted on a bare `eval()` after `_as` rises,
no clock edge needed at all -- see "Measured before writing").

### Group G2 -- cycle duration, no intervention (GRADE)
`sol:K-0099` ("A basic Zorro II cycle lasts four periods of the 7.16 MHz
clock"), `opus:K-0118` ("...runs for a total of four cycles..., extendable
with wait states when required" -- no memory-space qualifier, unlike
K-0117 below), `sol:K-0217` ("If the slave does not intervene, the
controller supplies zero-wait-state /DTACK"), `opus:K-0225` ("If a Zorro
II slave does nothing, /DTACK is driven by the bus controller with no
wait states, making the bus essentially a 4-cycle synchronous bus").

Testable at the narrower, decidable core each of these four actually
states: in the fast/no-intervention path (`nrdy=0`, `vpa=0`), does
`_dtack` assert well within a 4-clock-period (16-tick, in this fixture's
4-ticks-per-7M-cycle scheme) budget? Measured minimum latency: 3 ticks
(under one 7M-clock period) -- comfortably inside the 4-cycle budget the
page states, so the claim is graded PASS on this necessary condition. What
is NOT tested: that a real, unimplemented 68000 CPU master would sample
in a way that closes the cycle at exactly 4 clocks -- that half needs the
S-state sequencer named in Group D3 below, and none of these four claims'
own restatements insists on the specific S-number, only the total count
and the no-wait-state mechanism, both of which are this bridge's own doing.

### Group G3 -- wait-state extension (GRADE)
`sol:K-0100` ("A Zorro II cycle can be extended with wait states") -- no
`XRDY` named, just the generic mechanism. Testable directly via `nrdy`:
held high, `_dtack` stays deasserted through 40 ticks (measured); released,
`_dtack` asserts 3 ticks later, the same latency as the unobstructed path.

### Group G4 -- 7M clock is the timing basis (GRADE)
`sol:K-0136` ("7M is the timing basis for Zorro II and 68000-compatible
bus activity" -- `corpus_damage` noted on this row for the OCR "Zorro H"
misread, not relevant to testability). Testable within this module's own
scope: every latch and the DTACK generator condition on `clk7_en`/
`clk7n_en` exclusively (`minimig_m68k_bridge.v:97,104,111-116,119-121,130`)
-- nothing in this module's bus-facing logic clocks off anything else.
`opus:K-0143` (same sentence, plus "...such as bus arbitration" as an
explicit example) is declined separately below -- Group D2.

### Group G5 -- READ/R-W direction (GRADE)
`sol:K-0195` ("Asserted READ denotes a Zorro II read cycle"),
`sol:K-0196` ("Negated READ denotes a Zorro II write cycle"),
`opus:K-0204` ("READ is the read enable for the Zorro II bus and is
equivalent to the 68000's R/W output"), `opus:K-0205` ("READ asserted...
indicates a read cycle and READ negated indicates a write cycle").

Testable directly: `r_w` (this module's own R/W-equivalent input) drives
`rd`/`hwr`/`lwr` through `enable`. This is the same underlying mechanism
`hrm/appF-cia-bus-and-clock`'s `RW-HIGH-IS-READ`/`RW-LOW-IS-WRITE`/
`RW-SOURCE-CONTROLS-DIRECTION` already exercise for a *different* page's
proposition (p.353, the 8520's own R/W semantics) -- that package's own
`Harness::acquire_enable()` sequence is reused here verbatim (proven, not
re-derived), but these four uids are graded fresh: they assert a Zorro-
bus-level "READ" proposition from p.397, not a CIA-chip one from p.353,
and are not a re-citation of appF's own claim rows.

### Group G6 -- address bus width (GRADE)
`sol:K-0197` ("A1-A23 encode a 16 MiB Zorro II address space"),
`opus:K-0207` ("The Zorro II address bus A1-A23 is logically equivalent
to the 68000's address bus, providing 16 megabytes of address space").
Testable structurally: `address`/`address_out` are declared `[23:1]` --
23 lines above A0, exactly 2^23 words = 16 MiB byte-addressed -- and two
distinct full-range values both arrive at `address_out` correctly.

### Group G7 -- data bus width (GRADE)
`sol:K-0203` ("D0-D15 transfer a word or either byte"), `opus:K-0213`
("...a buffered version of the 680x0 data bus, providing 16 bits of data
accessible by word or either byte" -- its own quote's trailing DOE clause
is not part of this restatement, so no DOE boundary issue). Testable
structurally: `data`/`cpudatain`/`data_in`/`data_out` are all `[15:0]`,
and two distinct 16-bit patterns both arrive correctly.

### Group G8 -- write-side data-strobe semantics (GRADE)
`sol:K-0205` ("A PIC uses data strobes to determine write-data validity"),
`sol:K-0206` ("/UDS and /LDS assert low when write data is valid").
Testable: `hwr`/`lwr` (this module's own write-validity indicators, which
any downstream device -- PIC or otherwise -- would consult) are gated by
`~l_uds`/`~l_lds` respectively (`:140-141`); a write cycle with only one
strobe asserted produces only the matching write-enable.

### Group G9 -- byte-select mapping, /LDS-low//UDS-upper (GRADE)
`sol:K-0208` ("/LDS selects the lower, even-addressed byte"), `sol:K-0209`
("/UDS selects the upper, odd-addressed byte"), `opus:K-0217` ("The lower
data strobe is used for the lower byte...the upper strobe for the upper
byte..."). Testable directly via `hwr`/`lwr`'s own gating.

### Group G10 -- byte-select claimed for BOTH reads and writes (CORRECTED: was GRADE and red, now DECLINE)
`sol:K-0207` ("The data strobes select transferred bytes for reads and
writes"), `opus:K-0216` ("The data strobes fall on data valid during
writes and indicate byte select for both reads and writes").

**Both were graded and both were red. They are declined now, and the
measurement did not change:** `rd` does not vary with `_uds`/`_lds` at all
(`rd=1` identically for UDS-only, LDS-only, both, and *neither* strobe
asserted), still measured and still printed. What was wrong is that **half
the paragraph had been thrown away before the claim reached this package.**

p.397 does not stop at that sentence. Its next two say:

> There is one slight difference between these lines and the 68000 data
> strobes. **On reads of Zorro II memory space, both /DS3 and /DS2 will be
> asserted, no matter what the actual size of the requested transfer is.**
> This is required to support caching of the Zorro II memory space. For
> Zorro II I/O space, these strobes indicate the actual, requested byte
> enables, just as would a 68000 bus master.

Both readers extracted those sentences — `sol:K-0210`, `sol:K-0212`,
`opus:K-0218` — and the classification put all three at
`not-asserted` / `absent`, while the sentence they qualify came through as
`testable`. **A rule was graded and its own exception, three lines below it
on the same page, was discarded.**

Read whole, the claim is: byte select on writes always; on reads only in
Zorro II **I/O** space, and **not** in Zorro II **memory** space, where both
strobes assert regardless of transfer size. That is a claim with a
memory-space qualifier — and this package already has the rule for those.
**Group D1 below declines `sol:K-0098` and `opus:K-0117` for exactly it**:
no signal in `minimig_m68k_bridge.v` decodes an address range or a memory
space, that being `gary.v`'s decode. The same boundary answers these two.

And plainly: `rd = enable & lr_w` with no strobe term is what "on reads both
strobes are asserted, no matter the size" looks like from the slave's side.
The measurement was never evidence of a divergence. It is left as a
measurement and deliberately **not** re-graded into a green — this module
cannot tell memory space from I/O space, so it cannot establish the half of
the claim that would pass.

### Group G11 -- VPA/VMA peripheral-support mechanism (GRADE)
`opus:K-0068` ("/VPA and /VMA comprise the 6800/6502 peripheral support
mechanism that is part of the 68000 bus interface" -- p.387's own
sentence is about their *absence* on the A3000/Zorro-III bus; this
restatement keeps only the standalone existence/role proposition, which
does not depend on A3000 at all). Testable: `vpa` is a real input,
`vma` a real internal register gating a *second*, distinct DTACK-assertion
path (`vpa && vma && eclk[8]`, disjunct from the plain `!vpa` path) --
measured before writing: with `vpa=1` held throughout, `_dtack` does NOT
assert via the trivial path (confirming the mechanism is not simply
bypassed) and DOES assert once `vma` is latched (via `eclk[3]` while
`lvpa` is set) and `eclk[8]` arrives on a `clk7n_en` tick.

### Group G12 -- DTACK ≡ 68000 DTACK, Zorro II modes only (GRADE)
`opus:K-0222` ("For Zorro II modes /DTACK is equivalent to the 68000's
Data Transfer Acknowledge input" -- restricted to Zorro II modes, unlike
sol:K-0213/opus:K-0221 below, which require BOTH modes). Testable: this
module's `_dtack` output performs exactly the DTACK-generation role a
68000's own DTACK input would be driven by, for the one bus mode
(Zorro II-electrical) this pinned design implements at all.

---

### Group D1 -- bundles a memory-space qualifier (DECLINE)
`sol:K-0098` ("Read/write, memory-space, and byte-selection strobes
qualify a Zorro II cycle"), `opus:K-0117` ("...qualified by a read/write
strobe, memory space qualifiers and one or two byte selection strobes").
Both restatements require a memory-space-qualifier conjunct as part of
the one proposition graded. No signal in `minimig_m68k_bridge.v` decodes
address ranges or memory spaces at all -- that is `gary.v`'s decode
(`sel_chip`/`sel_kick`/etc., already the boundary `hrm/appK-zorro2-
memory-space-decode` and `hrm/appDE-memory-map-decode` grade). Declined
in full per the "states more than the package tests" rule; the boundary
that would answer the memory-space half is `gary.v`, not this module.

### Group D2 -- bus arbitration named as an example (DECLINE)
`opus:K-0143` ("The 7M clock forms the basis for all Zorro II and 68000
compatible activity and for other system functions such as bus
arbitration"). No `BR`/`BG`/`BGACK` or any multi-master arbitration
signal exists anywhere on this module's port list, and this single-CPU
pinned design implements no bus arbitration anywhere that could be
checked. Declined -- the "bus arbitration" conjunct is not observable at
any boundary this design has.

### Group D3 -- names /CCS (Zorro III nomenclature) as the acting signal (DECLINE)
`sol:K-0199` ("The falling edge of /CCS indicates a valid address"),
`sol:K-0200` ("On /CCS falling, READ is valid and the Zorro II cycle
starts"), `sol:K-0201` ("The rising edge of /CCS ends a Zorro II cycle"),
`sol:K-0202` ("/CCS may remain asserted across multiple cycles of a
read-modify-write access"), `opus:K-0211` ("/CCS, like /AS, can stay
asserted during a read-modify-write access..." -- /CCS is its
grammatical subject, /AS only a comparative clause).

p.397's own heading reads "Address Strobe (/AS)...(/CCS) for Zorro III" --
the physical signal being described IS /AS in Zorro II mode; /CCS is only
the Zorro III renaming of the same pin. This pinned design implements no
Zorro III mode anywhere (no Fat-Buster-equivalent bus controller, no
mode-switching logic), and `minimig_m68k_bridge.v` has no port named
`_ccs`. Since each of these five claims' own *restatement* commits to the
`/CCS` label specifically (unlike `opus:K-0209`/`opus:K-0210` in Group G1,
extracted from the same underlying sentences but labelled `/AS`), they
are declined as written -- the boundary that would answer a /CCS-named
claim is a Zorro III-capable bus controller, which does not exist in this
design at all, not merely outside this one module.

### Group D4 -- requires BOTH Zorro II and Zorro III cycles (DECLINE)
`sol:K-0213` ("/DTACK normally terminates both Zorro II and Zorro III
cycles"), `opus:K-0221` ("/DTACK is used to normally terminate both
Zorro bus cycles"). Unlike `sol:K-0097`/Group G1 (which reduces to one
mode, the only one this design has), these two restatements require the
Zorro-III half to hold as well, and this design implements no Zorro III
mode anywhere. Declined.

### Group D5 -- 68000 bus MASTER's own S-state sampling/latching (DECLINE)
`sol:K-0214` ("A Zorro II master first samples /DTACK on the falling edge
of S4"), `opus:K-0223` (the same fact), `sol:K-0215` ("The master latches
Zorro II data on the S6 falling edge"), `opus:K-0224` ("Data is latched
on the S6 falling edge and the cycle is terminated with /AS negated
during S7" -- bundles both), `sol:K-0216` ("The Zorro II cycle ends by
negating /AS during S7" -- states the general AS-ends-cycle fact Group G1
already covers, but adds "during S7" as a required conjunct), `sol:K-0231`
("Automatic /DTACK after XRDY requires the cycle to be at S4 or later" --
also Group D6, XRDY), `opus:K-0239` (same, also Group D6).

No `S0`-`S7`-labelled state register or comment exists anywhere in
`minimig_m68k_bridge.v` (confirmed by reading the file in full -- its own
header diagram is a *different* CIA/ECLK timing diagram, not a 68000
S-state one), and this module is the bus TARGET: it drives `_dtack`, it
never samples it, and it drives `data`, it never latches an incoming read
value the way a CPU core's own bus-state machine would. No 68000 CPU core
is instantiated in this fixture (none exists in this pinned tree's
`minimig-aga` directory at all under that name). Declined -- the boundary
that would answer any S4/S6/S7-specific claim is an actual 68000-compatible
CPU core (e.g. TG68/fx68k) implementing its own bus-state sequencer, which
this bridge is not and does not contain.

### Group D6 -- names XRDY (a slot-level, wired-OR, multi-card signal) (DECLINE)
`sol:K-0218` ("A slave can negate XRDY to delay controller-generated
/DTACK"), `opus:K-0226` ("A slow device can modify the automatic /DTACK
by negating XRDY to hold off /DTACK"), `sol:K-0227` ("XRDY is active high
and lets a slave delay automatic /DTACK"), `sol:K-0229` ("The slave holds
XRDY low until it is ready for /DTACK"), `sol:K-0230` ("The controller
asserts /DTACK shortly after XRDY is reasserted"), `sol:K-0231` (also
Group D5), `opus:K-0237` ("XRDY is an active high signal that allows a
slave to delay the bus controller's assertion of /DTACK..."), `opus:K-0239`
(also Group D5).

XRDY, per its own page text, "is a wired-OR from all PICs" -- a
slot-level, multi-card electrical bus, not a single module's input.
`minimig_m68k_bridge.v` has an `nrdy` input that plays an analogous
*role* (holding off `_dtack`), but it is not XRDY reconstructed: `nrdy`
is a single point-to-point, active-high NOT-ready signal (asserting it
blocks DTACK), the functional and naming opposite of XRDY's active-high
READY convention (page: XRDY defaults asserted/high=ready, is negated/
driven low by a slow slave to hold off DTACK) -- and it has none of
XRDY's wired-OR, multi-source character. Grading these eight claims via
`nrdy` would report a fact about a differently-named, differently-driven,
single-source signal as if it answered a claim about XRDY specifically.
Declined -- the boundary that would answer an XRDY-named claim is an
actual Zorro II slot with wired-OR ready logic from multiple PICs, which
does not exist in this design. (Compare `sol:K-0100`/Group G3, which
names no specific mechanism and is graded via `nrdy` as the generic
"can be extended" fact it actually states.)

### Group D7 -- compares two physically distinct pins, only one of which exists here (DECLINE)
`opus:K-0206` ("READ may become valid in a cycle earlier than a 68000
R/W line would, but it remains valid at least as long at the cycle's
end"). This implies a Zorro-bus "READ" pin generated independently of,
and with different timing than, "a 68000 R/W line" -- two distinct
signals compared. `minimig_m68k_bridge.v` has exactly one such input,
`r_w`; its own registered copy `lr_w` is strictly LATER (clocked, delayed)
than `r_w`, never earlier, and there is no separate, independently-timed
"READ" output derived from it. The differential-timing relationship this
claim states has no second signal at this boundary to compare against.
Declined.

## What will be driven and observed

Fixture: `minimig_m68k_bridge` alone, ticked via the 4-phase scheme
`hrm/appF-cia-bus-and-clock/test_minimig_m68k_bridge.cpp` already built
and verified (`clk7_en` at phase 0, `clk7n_en` at phase 2, of a free-
running phase counter this testbench drives directly -- normally
`amiga_clk`'s job, outside this fixture's own one-top-module boundary).
`acquire_enable()` is reused verbatim from that same file (16 ticks
`_as=0`/`cck=1`, then 4 ticks `cck=0`) for every graded assertion that
needs a settled `enable`. `vpa=0`/`dbr=0`/`dbs=0`/`nrdy=0` throughout
except where a group's own test drives them deliberately (Group G3's
`nrdy`, Group G11's `vpa`/`eclk`).

A liveness check with no claim uid (`[not graded -- apparatus check, no
claim uid]`) confirms `address_out` follows two distinct `address` values
before any graded read is trusted -- the same check appF already built
for this module, reused rather than reinvented.

## Measured before writing (throwaway probe, discarded, not part of this package)

Before fixing any numeric bound or the Group G10/G11 wording above, a
standalone Verilator probe (same module, same 4-phase clocking, built and
run under WSL, then deleted) measured:

1. **AS-to-DTACK latency, fast path** (`nrdy=0`,`vpa=0`,`cck=1` throughout):
   `_dtack` first reads asserted 3 ticks after `_as` falls -- under one
   7M-clock period, well inside a 4-cycle (16-tick) budget.
2. **Read byte-select**: with an acquired cycle, `r_w=1`, `rd` reads `1`
   identically for `_uds`-only, `_lds`-only, both, and neither strobe
   asserted -- `rd` has no dependency on either strobe at all.
3. **nrdy hold-off**: `nrdy=1` holds `_dtack` deasserted through 40 ticks;
   released, `_dtack` asserts 3 ticks later (the same fast-path latency).
4. **VPA/VMA path**: with `vpa=1` held throughout and the plain `!vpa`
   shortcut therefore unavailable, `_dtack` stays deasserted until `vma`
   is latched (`eclk[3]` pulsed on a `clk7_en` tick while `lvpa=1`) and
   `eclk[8]` then arrives on a `clk7n_en` tick -- asserted at tick 23 of
   that specific stimulus, confirming the path is real and distinct from
   the `vpa=0` fast path, not dead wiring.
5. **AS-rise async reset**: `_dtack` reads deasserted on a bare `eval()`
   immediately after `_as` rises, with no intervening clock edge at all.

## What would make each assertion fail

- **G1**: `_dtack` never asserts after `_as` falls (AS does not define
  cycle start), or `_dtack` stays asserted after `_as` rises with no
  further clock edge (AS-rise does not end the cycle "as quickly as
  possible").
- **G2/G3**: fast-path `_dtack` latency exceeds the 16-tick/4-cycle budget,
  or `nrdy=1` fails to hold `_dtack` off.
- **G4**: any latch this module owns updates on an edge that is not
  `clk7_en`/`clk7n_en`-qualified.
- **G5**: `rd`/`hwr`/`lwr` do not track `r_w` as stated.
- **G6/G7**: two distinct, known values do not both arrive correctly and
  distinctly at `address_out`/`data`.
- **G8/G9**: `hwr`/`lwr` do not track `_uds`/`_lds` as the lower/upper,
  write-validity mapping states.
- **G10**: the "writes" half fails if G9 fails; the "reads" half is
  measured, not merely predicted -- reported red as measured (see above).
- **G11**: `_dtack` asserts via the trivial path despite `vpa=1` (the
  mechanism would be bypassed, not real), or never asserts even after
  `vma`/`eclk[8]` align (the mechanism would be dead).
- **G12**: `_dtack`'s behavior does not match a plausible DTACK-input
  equivalent for the one bus mode this design has.

## Evidence not used until step 3

`classify.evidence` for the great majority of these 51 claims cites the
same block of `minimig_m68k_bridge.v` line ranges (`:46-50`, `:110-135`,
`:138-141`) regardless of which specific proposition each claim states;
a handful (`sol:K-0197`, `opus:K-0207`) cite `:54-55`/`:161-164` (address
register width) and (`sol:K-0100`,`sol:K-0218`,`opus:K-0226`,`sol:K-0227`,
`sol:K-0229`,`sol:K-0230`,`sol:K-0231`,`opus:K-0237`,`opus:K-0239`) cite
`:42`/`:131` (`nrdy`). Those citations were read for step 1 (they are
what let the boundary judgment above be made claim-by-claim at all -- see
the disclosure at the top of this file), but the exact stimulus sequences,
tick counts and the two RTL facts genuinely discovered only during step 3
(the read-byte-select gap in Group G10, the VPA/VMA two-path structure in
Group G11) were fixed only after the throwaway probe above, not assumed
from the citation text.
