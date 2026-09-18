# appK-zorro2-memory-space-decode

Amiga Hardware Reference Manual, 3rd edition, Appendix K, p.386, Figure K-1
("Expansion Memory Map"). Claims file:
`hrm-testpipeline/work/appK-build/appK-zorro2-memory-space-decode.jsonl`, 3
claims (sol:K-0044, opus:K-0051, opus:K-0056), all `status: testable`,
`boundary: gary`.

**Order of work, stated plainly.** The task assignment that produced this
package already named the top module (`gary`) and gave specific RTL
citations (`gary.v:159-162`, `gary.v:166`) before this file was written --
that context could not be un-seen. What follows is nonetheless argued from
each claim's own `quote`/`restatement` fields, not reverse-engineered from
reading the decode logic first; the port names, stimulus values and pass/
fail conditions below are step-3 work, decided only after this section was
drafted.

## What the three claims assert

Figure K-1 is a linear address map (`figures/p386.png`; the text layer
scatters the printed bands out of position, per each claim's own
`corpus_damage` flag). Two of the three claims name the SAME band from two
independent extractions:

- **sol:K-0044** -- quote "Amiga Chip $00200000", restatement "Amiga chip
  memory extends up to $00200000 in the expansion memory map."
- **opus:K-0051** -- quote "Amiga Chip memory $00200000 ... $00000000",
  restatement "Amiga Chip memory runs from $00000000 up to $00200000."

Read together, these two claims assert one proposition: **the Amiga chip
memory band, as printed on the figure, spans byte addresses $000000 up to
(not including) $200000.**

- **opus:K-0056** -- quote "Motherboard ROM $01000000 ... $00F00000",
  restatement "Motherboard ROM runs from $00F00000 up to $01000000." This
  is a second, independent proposition: **the motherboard ROM band spans
  $F00000 up to (not including) $1000000.**

## Boundary

The task assignment states plainly, and this package accepts as given:
`gary` decodes chip memory (motivating the K-0044/K-0051 pair) and a
kickstart-ROM band (motivating K-0056); the Zorro II expansion-RAM band the
same figure also prints ($200000-$9FFFFF) is decoded by a *different*
module (`cpu_wrapper.v`'s `sel_z2ram`), outside this package's one-top-
module fixture. None of this package's 3 claims are themselves a
Zorro-II-expansion-band assertion -- both K-0044/K-0051 name the chip band
and K-0056 names the ROM band, both of which the assignment places at
`gary` itself -- so no claim in this specific file needs a `record()`
decline for that reason. (If a future claim in this package's line named
the $200000-$9FFFFF band directly, it would be declined here with
`cpu_wrapper.v`/`sel_z2ram` named as the boundary that answers it; none of
the 3 present claims do.)

## What will be driven and observed

Fixture: `gary` alone, clocked, `reset` pulsed once at start. All CPU-side
inputs (`cpu_address_in`, `cpu_rd`, `cpu_hwr`, `cpu_lwr`, `cpu_hlt`, `ovl`,
`dbr`, `dbwe`, `memory_config`, `ecs`, `hdc_ena`, `a1k`, `bootrom`, and the
three data-bus inputs) are driven to known values; a plain CPU read cycle
(`dbr=0`, `cpu_rd=1`, `ovl=0`, `cpu_hlt=0`) is used throughout, matching a
normal post-boot CPU access (Kickstart overlay off, Agnus not driving the
bus). `cpu_address_in` is a 23-bit `[23:1]` port carrying a byte address
with bit 0 dropped; every address below is driven as `byte_address >> 1`.

- **Chip-memory band (K-0044/K-0051):** read `sel_chip[3:0]` at the band's
  base ($000000), near its printed top ($1FFFFE), and at the printed upper
  boundary itself ($200000) -- expect asserted/asserted/deasserted,
  matching "extends up to $200000" read as an exclusive upper bound (the
  conventional reading of a printed address-map boundary: it is where the
  NEXT band starts).
- **ROM band (K-0056):** read `sel_kick` at the claim's own stated lower
  boundary, $F00000. Fails would mean `gary` does not consider $F00000
  itself part of the ROM band it decodes.

A liveness check with no claim uid (labelled `[not graded -- apparatus
check, no claim uid]`) confirms `sel_chip` actually changes with address
before any graded read is trusted.

## What would make each assertion fail

- K-0044/K-0051: `sel_chip` not asserted somewhere inside $000000-$1FFFFE,
  or still asserted at $200000 -- either would mean the RTL's chip-memory
  decode does not match the figure's printed $000000-$200000 band.
- K-0056: `sel_kick` not asserted at $F00000 -- would mean `gary`'s own
  ROM decode does not begin where the figure says it does.

## Evidence not used until step 3

`classify.evidence` for all three claims cites `gary.v:159-162` (chip
select) and `gary.v:166` (kickstart select); those exact lines were opened
only after the propositions above were fixed, to build the fixture.
