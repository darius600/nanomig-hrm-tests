# appK-cache-inhibit-per-cycle

Amiga Hardware Reference Manual, 3rd edition, Appendix K, p.402 ("Cache
Support"). Claims file:
`hrm-testpipeline/work/appK-build/appK-cache-inhibit-per-cycle.jsonl`, **1
claim** (`sol:K-0295`), `status: testable`, `boundary: cpu_cache_new`. Top
module: `cpu_cache_new` (`nanomig/src/minimig-aga/cpu_cache_new.v`).

**Order of work, stated plainly.** `sol:K-0295`'s own `classify.evidence`
already names the port (`cache_inhibit`), two RTL line numbers
(`cpu_cache_new.v:24,363`) and the top-level tie-off (`nanomig.v:423`)
before this file was written -- that context could not be un-seen, and
the build brief for this specific package explicitly directs "**Check
both facts yourself**" rather than treating the citation as established.
What follows is nonetheless argued from the claim's own `quote`/
`restatement` first; the exact stimulus sequence is step-3 work.

## What the one claim asserts

`sol:K-0295` -- quote "cards that absolutely must not be cached to assert
a cache inhibit line,", restatement "A slave may inhibit caching
independently on each cycle with /CINH." The proposition: **caching can
be suppressed for one bus transfer at a time, not only by a fixed region
map.**

## Checking the evidence myself

Both facts the classifier's own evidence cites were re-read directly,
not taken on trust:

1. **`cpu_cache_new.v:24`**: `input cache_inhibit, // cache inhibit` --
   confirmed, a real port on the module this package's assignment names
   as top.
2. **`cpu_cache_new.v:363`**: inside `CPU_SM_FILL1` (the state reached on
   a cache miss, after the first word arrives from SDRAM):
   ```
   if (cache_inhibit) begin
     // don't update cache if caching is inhibited
     cpu_sm_state <= CPU_SM_FILLW;
   end else begin
     ... (tag-ram write setup, cpu_sm_iram0_we/iram1_we/dram0_we/dram1_we,
          cpu_sm_state <= CPU_SM_FILL2 to continue the 4-word line fill)
   end
   ```
   Confirmed by reading the file in full: when `cache_inhibit` is high on
   the cycle the first word's `sdr_read_ack` arrives, the state machine
   skips straight to `CPU_SM_FILLW` -- no tag-write enable is ever set,
   and the 3-word burst that would otherwise complete the cache line
   (`CPU_SM_FILL2`/`3`/`4`) is skipped entirely. `cpu_dat_r`/`cpu_ack` are
   still driven either way (the CPU gets its data regardless) -- only the
   caching side-effect is suppressed. This is exactly the claim's own
   proposition.
3. **`nanomig.v:423`**: `.cache_inhibit (1'b0),` inside the `cpu_cache_new`
   instantiation -- confirmed by reading the surrounding instantiation
   (`nanomig.v:415-430`) directly: the top-level SoC ties this input
   permanently low. The port is real and the module honours it when
   driven (facts 1-2); the assembled system simply never drives it.

## Boundary

`cpu_cache_new` alone is exactly the right boundary for this claim: the
proposition is about the cache's own per-cycle behaviour, and every
signal it needs (`cache_inhibit`, `cpu_cs`/`cpu_dr`/`cpu_adr`, the SDRAM
handshake `sdr_read_req`/`sdr_read_ack`/`sdr_dat_r`, and the outputs
`cpu_dat_r`/`cpu_ack`) is this module's own port. No slot, card, pin
number, or higher-level integration fact is needed to grade it. What is
*not* graded here, and is not this claim's own subject either: who
asserts /CINH on a real Zorro card, or that it is driven at slave/PIC
time -- outside this module and outside this one-claim package.

## What will be driven and observed

Fixture: `cpu_cache_new` alone (default `ADDR_WIDTH=24`, matching the
module's own parameter default -- `nanomig.v`'s own instantiation
overrides it, but this fixture does not import that override, matching
"the module's own ports," not the integration). `dpram.v` and
`dpram_be_1024x16.v` (both `nanomig/src/minimig-aga/`) are compiled
alongside it -- `cpu_cache_new` instantiates six of them for its tag/data
RAMs; both are plain, unconditional support modules with no claim of
their own.

`rst` is pulsed, then held low for the ~260 cycles the module's own
`SDR_SM_INIT0`/`SDR_SM_INIT1` cache-clear sweep needs (confirmed by
reading `cpu_cache_new.v:472-492`: `sdr_sm_adr` counts by 4 until all of
`sdr_sm_adr[9:2]` are set, i.e. 256 steps) before any access is issued.
`cpu_cache_ctrl[0]` (cache-enable) is held asserted throughout (latched
only while `!cpu_cs`, per `:229-239`).

A small SDRAM stub in the testbench watches `sdr_read_req`: on seeing it
asserted, it supplies four consecutive `sdr_read_ack` cycles with four
distinct, known data words on `sdr_dat_r` -- one 4-word burst, whether or
not the DUT ends up consuming all four (a `cache_inhibit=1` fill only
consumes the first; the stub does not need to know which case it is in).

Two *different* addresses (so no aliasing question can arise) are each
read once as a cold miss -- one with `cache_inhibit=0` during the fill,
the other with `cache_inhibit=1` -- then each is read a second time (this
second read's own `cache_inhibit` value does not matter; nothing was
cached for the second address either way if the first read inhibited
it). The measured fact matching the evidence's own comparison recipe
("compares cpu_dat_r/cpu_ack and sdr_read_req...between the two runs"):
did the *second* read raise `sdr_read_req` again (a repeat miss) or not
(a hit, served from cache)?

A liveness check with no claim uid (`[not graded -- apparatus check, no
claim uid]`) confirms the first (cold) read of each address is itself a
genuine miss (`sdr_read_req` observed, `cpu_ack` eventually asserts) --
so a "second read never misses" reading cannot be a stuck fixture that
never really tested anything.

## What would make the assertion fail

The address filled with `cache_inhibit=0` failing to hit on its second
read (caching not actually happening when permitted), or the address
filled with `cache_inhibit=1` failing to miss again on its second read
(the inhibited fill was cached anyway, `cache_inhibit` not honoured) --
either would mean `cpu_cache_new.v:363`'s branch does not do what it
reads as doing.

## The standing qualifier, said plainly

`nanomig.v:423` ties `cache_inhibit` to `1'b0` in the assembled system.
The capability this claim describes exists and is real at `cpu_cache_new`
(graded below); nothing in the current pinned integration ever exercises
it. Both facts are true at once and neither one is graded away by the
other -- the RTL capability is what `sol:K-0295` asserts, and that is
what this package tests.

## Evidence not used until step 3

`classify.evidence`'s exact citations (`cpu_cache_new.v:24,363`,
`nanomig.v:423`) were read for the "Checking the evidence myself" section
above -- per the build brief's explicit instruction for this package, not
deferred the way a normal package's evidence would be. The stimulus
sequence, the SDRAM stub's timing, and the ~260-cycle init-sweep wait
were fixed only while writing the test itself.
