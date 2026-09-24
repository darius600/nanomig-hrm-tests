# NM-HRM-047 — one step-spacing threshold where the page prints two

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. It is a ratio finding: what is measured is that the two printed cases are gated identically, not that either absolute value is wrong. |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L594-L603) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L292-L301) |
| **Report to** | **Amiga specialists first.** The page gives two numbers; whether an implementation is *required* to distinguish them is a reading question, not a measurement — see *For the specialists* |
| **Test** | [`hrm/ch8-drive-step-timing`](../../hrm/ch8-drive-step-timing) — assertion `RATIO-A` |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 1 red of 5 graded, plus two explicit `record()` declines (`STEP-POLARITY`, `PULSE-WIDTH-FLOOR`) and `RATIO-B` declined for measuring two quantities that are not of the same kind |
| **Verdict of the four** | **unanimous with NanoMig on the gate**, and none of the four uses the printed numbers for it. All four accept step pulses on a single direction-independent floor in the tens of microseconds; three of them carry 3 ms and 18 ms elsewhere, as a *settle* effect NanoMig does not model at all |

---

## What the HRM says

**Chapter 8, printed p.245**, under PB0 `DSKSTEP*`, in two paragraphs separated
by one blank line.

Printed p.245 lines 31–35:

> "The drives used for the Amiga are guaranteed to get to the next track within 3
> milliseconds.  Some drives will support a much faster rate, others will fail.
> Loops that decrement a counter to provide delay are not acceptable.  See
> Appendix F for a better solution."

Printed p.245 lines 37–39:

> "When reversing directions, a minimum of 18 milliseconds delay is required from
> the last step pulse.  Settle time for Amiga drives is specified at 15
> milliseconds."

**Figure 8-8** prints the pair as arrows in one frame — 18 ms, 18 ms, 3 ms — a
**6:1 ratio** between the reversal case and the same-direction case.

**The two sentences are not phrased alike, and the entry should not pretend they
are.** The 18 ms sentence states a requirement in so many words: *"a minimum …
is required"*. The 3 ms sentence states a drive guarantee — the head *arrives*
within 3 ms — from which the corresponding programmer's interval follows but is
not spelled out. Figure 8-8 is what puts them on the same axis as two spacings,
and it is the figure that makes the 6:1 comparison the page's own.

**What the page does not say** is that an implementation must gate the two cases
differently. It prints two numbers for two situations. A single threshold placed
at or above the larger of them would violate neither stated minimum.

## What NanoMig does

One counter, with no direction input anywhere in it.

[`src/minimig-aga/paula_floppy.v#L594-L603`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L594-L603):

```verilog
always @(posedge clk) begin
if (clk7_en) begin
if (!step_ena)
step_ena_cnt <= step_ena_cnt + 9'd1;
else if (_step && !_step_del)
step_ena_cnt <= 0;
end
end

assign step_ena = step_ena_cnt[8];
```

A nine-bit saturating counter. It counts up while `step_ena` is low, and every
accepted `_step` rising edge resets it to zero. `step_ena` is bit 8, so by
construction the gate opens 256 `clk7_en` ticks after the last reset — **one
threshold, and the counter has no port through which a second one could
exist.** `direc` is not read by this block, and neither is `_sel`.

**The reset is ungated, which is what makes the two cases share the floor.** The
track update does consult direction and selection
([`#L671-L680`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_floppy.v#L671-L680)):

```verilog
    if (!_selx && _step && !_step_del && step_ena) begin
      if (!dsktracklast && !direc)
        dsktrack[sel] <= dsktrack[sel] + 7'd1;
      else if (_dsktrack0 && direc)
        dsktrack[sel] <= dsktrack[sel] - 7'd1;
```

— but the counter reset at `:594-603` carries no such gate. A reversal edge and
a same-direction edge reset the same register and are then admitted by the same
`step_ena_cnt[8]`.

### Measured

    minimum accepted step-to-step gap,
      direction REVERSAL     (p.245:37-39, "18 ms" in Figure 8-8) = 253 tick(s)
      SAME-DIRECTION cont.   (p.245:32,    "3 ms"  in Figure 8-8) = 253 tick(s)
    printed ratio 6:1, measured ratio 253:253

> **FAIL: RATIO-A**: the printed 18ms-reversal and 3ms-same-direction step-to-step
> minima (p.245:32, p.245:37-39; Figure 8-8) are in a 6:1 ratio — measured
> against `paula_floppy`'s single `step_ena_cnt` spacing counter
> (`paula_floppy.v:594-603`), which resets on every accepted `_step` rising edge
> regardless of selection (`paula_floppy.v:671-680` gates the TRACK update on
> selection and direction, but the counter reset at 594-603 carries no such
> gate) and so gates a reversal and a same-direction edge alike.

A tick here is one `clk7_en`-enabled clock edge. 253 rather than 256 is the
pulse framing of the probe, not a different threshold.

**The measurement is a ratio, and that is deliberate.** No constant in
`paula_floppy.v` is tied to a real-time unit anywhere reachable from this
fixture, which is why the package *declined* `PULSE-WIDTH-FLOOR` and `RATIO-B`
rather than grading tick counts against printed microseconds. A ratio is
dimensionless, so it survives the missing unit: 253:253 is 1:1 whatever a tick
turns out to be worth, and 1:1 is not 6:1.

**What is therefore not measured here is whether NanoMig's single floor is long
enough.** That would need the tick-to-millisecond binding this boundary does not
offer. The finding is that the two cases are not distinguished — not that either
is too short.

### How both cases were made reachable

**The fixture construction is stage 8's and is described in
`2026-09-15_ch8_s2_stage8_repairs.md`; it is cited here rather than re-derived.**

The obstacle that document records: `_track0` is the only external read-back of
head position, and it is one bit meaning "at track 0 or not". A reversal probe
can be read against it directly, because it steps back toward track 0. A
same-direction probe cannot — it moves the head *further* from track 0, so
acceptance and rejection both land off the boundary and look alike.

The builder's answer, quoting that record:

> "after the probed same-direction edge, drive one more generously-spaced
> **reversal** that is accepted regardless, folding the state back onto the
> track-0 boundary — 2→1 if the probe was accepted, 1→0 if it was rejected. That
> fold-back is never blocked by the track-0 underflow guard, because the head is
> never at track 0 when it fires."

**This matters because of what it repaired.** The same record states it plainly:

> "RATIO-A's old form probed a direction reversal at both positions, so its two
> 253-tick captures were one case twice. The repair drives both cases for real."

The earlier version of this assertion did not measure a same-direction
continuation at all. It measured a reversal, twice, and reported the resulting
1:1 as if it were the comparison the page asks for. That was caught during stage
8 and the assertion was restated. **The 253:253 quoted above is one genuine
reversal gap and one genuine same-direction gap**, each binary-searched for its
own minimum accepted spacing, and the finding was restated with it.

The package's own mutation run agrees the assertion now discriminates:
`ch8-drive-step-timing` came back `7/7` — 6 killed, 1 green-at-red — with
nothing surviving.

## What the others say

**Not one of the four independent implementations gates step-pulse acceptance
on the printed numbers, and only one of the four gates it on direction at
all.** Three of them do carry 3 ms and 18 ms in their code — as a settle
effect on read data, which is a different mechanism from admitting the pulse.

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:1855-1864`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L1855-L1864), [`:246`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L246) | **one threshold, no direction term** — the same shape as NanoMig: <pre>if (drv-&gt;steplimit && get_cycles() - drv-&gt;steplimitcycle &lt; MIN_STEPLIMIT_CYCLE) {<br>&nbsp;&nbsp;write_log(_T(" step ignored drive %ld, %lu\n"), ...);<br>&nbsp;&nbsp;return;<br>}</pre> with `#define MIN_STEPLIMIT_CYCLE (CYCLE_UNIT * 140)`. `drive_step` receives the direction as an argument and never consults it for spacing. The gate is **off entirely unless cycle-exact**: `set_steplimit` returns immediately when `currprefs.m68k_speed != 0` ([`:1795-1802`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L1795-L1802)) |
| **vAmiga** | [`FloppyDrive.cpp:881-899`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L881-L899) | **the only one of the four with a direction-conditioned acceptance term** — and its two values are equal: <pre>if (agnus.clock - latestStep &lt; getStepPulseDelay()) return false;<br>// If the step direction reverses, some extra-time is needed (?)<br>if (agnus.clock - latestStepDown &lt; getRevStepPulseDelay()) return false;</pre> mirrored in `readyToStepDown` ([`:901-919`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L901-L919)). Under the default `A1010` mechanics both are `USEC(40)` ([`:636-659`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L636-L659)), so the structure is two thresholds and the **measured ratio is 1:1, exactly NanoMig's**. The `(?)` in the author's own comment is his |
| **Denise** | [`emulation/libami/drive/diskDrive.cpp:526-532`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L526-L532) | **one threshold, and its comment is the 3 ms sentence verbatim**: <pre>auto DiskDrive::step(bool dir, bool updTrack) -&gt; void {<br>&nbsp;&nbsp;// The drives used for the Amiga are guaranteed to get to the next<br>&nbsp;&nbsp;// track within 3 milliseconds. Some drives will support a much<br>&nbsp;&nbsp;// faster rate, others will fail.<br>&nbsp;&nbsp;if (stepperMinTime && (agnus.fallBackCycles(stepClock) &lt; stepperMinTime))<br>&nbsp;&nbsp;&nbsp;&nbsp;return;</pre> `dir` is the very next thing read, for the move — never for the gate. Both its stepper timings are **user sliders that default to 0**, i.e. off ([`libami/interface.cpp:43-44`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/interface.cpp#L43-L44), applied at [`program/program.cpp:257`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/program/program.cpp#L257)); at maximum they reach 3.0 ms and 18.0 ms |
| **Copperline** | [`src/floppy/mod.rs:3131-3153`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3131-L3153) | **two checks, one constant** — it tests the reversal case explicitly and then applies the same floor to it: <pre>if since(self.last_step_cck) &lt; MIN_STEP_PULSE_CCK { return false; }<br>let opposite = if inward { self.last_step_outward_cck }<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;else { self.last_step_inward_cck };<br>if since(opposite) &lt; MIN_STEP_PULSE_CCK { return false; }</pre> its own comment says so: *"a reversal needs the same gap after the last step the other way"*. `MIN_STEP_PULSE_CCK = 140` ([`:112`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L112)) — the same 140 WinUAE uses, and its comment says it was calibrated against vAmigaTS `Drive/step2` and `step3` |
| **MiSTer** | [`rtl/paula_floppy.v:292-301`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L292-L301) | descendant — byte-identical to NanoMig's `:594-603` |
| **TC64** | [`rtl/minimig/paula_floppy.v:528-537`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L528-L537) | descendant — the same counter and the same `assign step_ena = step_ena_cnt[8];`, with one literal written more explicitly: `step_ena_cnt <= 9'd0;` where NanoMig and MiSTer both write `step_ena_cnt <= 0;` — same value, same reset condition. No line in any of the three trees reads `direc` or `_sel` in this block; the single, direction-independent floor is inherited by both lines |

**Where the printed numbers actually live in those three.** This is the part
worth passing to the hardware developers, because it is a mechanism NanoMig does
not model at all:

- **Copperline** keeps the 3:18 pair, direction-conditioned, as a read-data
  settle window: `SEEK_STEP_SETTLE_CCK` is 3 ms and `SEEK_REVERSAL_SETTLE_CCK`
  is 18 ms ([`mod.rs:103`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L103),
  [`:113`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L113)),
  selected at [`:3173-3179`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3173-L3179)
  by `let reversal = self.last_step_inward.is_some_and(|prev| prev != inward);`.
  **This is the only place in any of the four where the printed 6:1 appears as a
  6:1**, and its comment is explicit that position sense and /TRK0 are
  unaffected: the head moves instantly, only the *data* is withheld.
- **vAmiga** has `getTrackToTrackDelay()` = `MSEC(3)` and `getHeadSettleTime()` =
  `MSEC(9)` ([`FloppyDrive.cpp:661-685`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L661-L685)),
  summed into `latestStepCompleted` and spent on one thing — returning random
  bits instead of track data while the head is in flight
  ([`:802`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L802)):
  `if (agnus.clock < latestStepCompleted) return u8(amiga.random() & 0x55);`
- **Denise** defers the step itself by `stepperSeekTime` when it is enabled
  ([`diskDrive.cpp:449-458`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L449-L458),
  [`:289-298`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L289-L298)),
  with its own comment: *"Continuous stepping can be done very quickly. The last
  step, however, requires about 10 ms until data can be reliably read."*

**The four agree with NanoMig about the gate and disagree with it about the
settle.** On the question RATIO-A grades — does the acceptance threshold depend
on direction? — the answer is *no* in WinUAE, Denise and Copperline, and *yes
structurally but 1:1 numerically* in vAmiga. NanoMig is in the majority. What
separates it is that three of the four then spend the printed 3 ms and 18 ms on
withholding read data during the seek, and `paula_floppy.v` has no equivalent:
the track register updates and the data follows immediately.

**None of them decides this finding.** The manual does. Their agreement is
reported because four independent authors, each with the page in front of them,
all put the printed numbers somewhere other than the step gate.

## Suggested change

**None on the strength of this measurement alone**, and the reason is in the
page rather than in the RTL.

If p.245's two numbers are read as two *minimum spacings a drive enforces*, then
a conforming implementation would need a second, longer floor selected by
direction reversal — the shape vAmiga already has, and would need
`SEEK_REVERSAL_SETTLE_CCK`-style values rather than vAmiga's equal ones. That is
a real change to `paula_floppy.v`: a direction-history bit and a second compare.

If they are read as **the mechanism's own characteristics, which software must
respect and the controller need not enforce**, then nothing in `paula_floppy.v`
is wrong. A drive that accepts pulses faster than the printed rate is exactly
what the page means by *"some drives will support a much faster rate"*, and
software that steps too fast breaks against the mechanism, not against the gate.

The second reading is the one the other four implementations act on. It is not,
however, what the page says, and this project does not get to choose between
them.

## What holds the finding up

- **Both cases are now driven for real**, one genuine reversal and one genuine
  same-direction continuation, each read through the same `_track0` observable —
  the stage-8 repair, cited above, that replaced a form measuring one case
  twice.
- **The RTL cannot have two thresholds.** `step_ena_cnt` is one nine-bit counter
  with no direction input and an ungated reset; this is a structural fact, not a
  value that happened to come out equal.
- **The measured quantity is dimensionless**, so it does not depend on a
  tick-to-millisecond binding this boundary cannot supply.
- **Figure 8-8 puts the two numbers on one axis**, which is what makes 6:1 the
  page's own comparison rather than the test's construction.
- **The package's own liveness and protocol assertions are green in the same
  run** — `FIXTURE-LIVE`, `DIREC-PROTOCOL`, `TRACK00-POLARITY` and `SEL-GATING`
  — so direction, stepping and the track-0 sensor all demonstrably work before
  RATIO-A leans on them.
- **Mutation: 7/7, nothing survived** — 6 killed, 1 green-at-red.

## What weakens it

- **The page never says the two must be gated differently.** It prints two
  numbers for two situations; a single floor at or above the larger violates
  neither. This finding measures that NanoMig does not distinguish them, which
  is a weaker statement than "NanoMig is non-conforming".
- **The absolute value is not measured.** Whether NanoMig's 256-tick floor is
  above 18 ms, between the two, or below both is not determinable at this
  boundary — the same absence that caused `PULSE-WIDTH-FLOOR` and `RATIO-B` to
  be declined rather than graded. If the floor turned out to sit above 18 ms,
  the ratio would be the only thing wrong and nothing would be observably wrong
  at all.
- **The two sentences are not parallel in form.** One states a requirement, the
  other a drive guarantee. The 6:1 comparison rests on Figure 8-8 treating them
  as two spacings; a reader who takes only the prose could argue the page states
  one minimum, not two.
- **Three of the four implementations gate exactly as NanoMig does**, and the
  fourth's two thresholds are numerically equal. No implementation of the four
  reproduces the printed 6:1 as an acceptance rule.

## For the specialists

The question this entry hands over is **not** "is 253 ticks the right number".
It is: **does p.245 impose two step-spacing requirements on a controller, or
describe two properties of the mechanism?**

Three pieces of evidence bear on it, and they do not all point the same way:

1. **The page's own framing.** The 18 ms sentence says *"is required"* — the
   language of a requirement. The 3 ms sentence describes what the drive
   guarantees. That asymmetry may be deliberate.
2. **What the implementations did.** All four put a short, direction-independent
   floor on pulse acceptance, and three of them spent the printed 3 ms and 18 ms
   on read-data settle instead. Copperline's is the closest thing to the page's
   6:1 anywhere in the four — and it deliberately leaves /TRK0 and head position
   instant, so recalibration is unaffected.
3. **vAmiga's author was unsure.** His comment on the reversal check reads *"If
   the step direction reverses, some extra-time is needed (?)"* — with the
   question mark — and he then gave both delays the same value.

If the answer is "properties of the mechanism", this finding closes as
documentation and NanoMig needs no change. If it is "requirements on the
controller", the RTL needs a direction-history term, and the more useful change
may be the settle behaviour three of the four model and NanoMig does not — since
that is what period software could actually observe.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: US patent 4,780,844, Commodore-Amiga; US patent 4,829,473, Commodore-Amiga; 6502.org dissection of Paula 8364R7; techtravels, *Inside Paula's Floppy Controller*, part I.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.

