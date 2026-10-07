# NM-HRM-047 — one step-spacing threshold where the page prints two [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether `paula_floppy.v` should gate a step pulse that reverses direction on a longer floor than one that continues the direction, where p.245 prints 18 ms against 3 ms (6:1 in Figure 8-8) and the RTL measures 253 ticks for both. The question stops at the ratio: whether the single floor is long enough is outside it. It closes when `paula_floppy.v` gates a reversal on a second, longer floor and `RATIO-A` turns green, or when a specialist rejects that change, recording p.245's two numbers as properties of the mechanism |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | the step-to-step acceptance gate of `paula_floppy.v` (`step_ena_cnt`), a direction reversal against a same-direction continuation. Chip revision: unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed p.245 (PB0 `DSKSTEP*`, lines 31–35 and 37–39) and Figure 8-8, printed p.242–243 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · <details><summary>claim ids by assertion (30)</summary> claims by assertion: `RATIO-A`: p. 242: `opus:8-time.c15`, `opus:8-time.c16`, `opus:8-time.c17`, `sol:8-fig8-8-text.c1`, `sol:8-fig8-8-text.c2`; p. 243: `opus:8-fig8-8.c4`, `opus:8-fig8-8.c5`, `opus:8-fig8-8.c6`, `sol:8-fig8-8.c2`, `sol:8-fig8-8.c3`, `sol:8-fig8-8.c4`, `sol:8-fig8-8.c19`, `sol:8-fig8-8.c21`, `sol:8-fig8-8.c23`; `DIREC-PROTOCOL`: p. 245: `opus:8-cia.c46`, `sol:8-cia.c40`; `TRACK00-POLARITY`: p. 242: `sol:8-fig8-8-text.c4`; p. 243: `sol:8-fig8-8.c27`; `SEL-GATING`: p. 242: `opus:8-time.c21`; p. 243: `opus:8-fig8-8.c10`, `sol:8-fig8-8.c7`, `sol:8-fig8-8.c6`; declined `STEP-POLARITY`: p. 243: `sol:8-fig8-8.c26`; declined `PULSE-WIDTH-FLOOR`: p. 242: `sol:8-fig8-7.c3`, `sol:8-fig8-9-text.c1`; declined `RATIO-B`: p. 243: `sol:8-fig8-8.c1`, `opus:8-fig8-8.c8`, `opus:8-fig8-8.c3`, `sol:8-fig8-8.c17`, `sol:8-fig8-8.c29`; `FIXTURE-LIVE` carries no claim id </details> |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L594-L603) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L292-L301); inherited |
| **Test** | [`hrm/ch8-drive-step-timing`](../../hrm/ch8-drive-step-timing): `make -C hrm/ch8-drive-step-timing test` |
| **Assertions** | `RATIO-A` red, 1 of 5 graded (4 passed, 1 failed): expected the reversal and same-direction minimum step-to-step gaps in the printed 6:1 ratio, observed 253:253 ticks. `STEP-POLARITY` and `PULSE-WIDTH-FLOOR` are declined by `record()`, and `RATIO-B` is declined for measuring two quantities that are not of the same kind |
| **Verdict of the four** | **unanimous with NanoMig on the gate**, and none of the four uses the printed pair for it. All four accept step pulses on a direction-independent floor — tens of microseconds in WinUAE, vAmiga and Copperline, off by default in Denise; three of them carry a seek or *settle* delay elsewhere (Copperline 3 ms and 18 ms by direction, vAmiga 3 ms plus 9 ms, Denise up to 18 ms and off by default), an effect NanoMig does not model at all. Hardware: not measured |
| **Effort** | none; if a specialist records p.245's two numbers as requirements on the controller, M: `paula_floppy.v:594-603` — a direction-history bit and a second compare on `step_ena_cnt`; the second floor's value needs a tick-to-millisecond binding this page does not have |

---

## Source requirement

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

**Figure 8-8** prints the pair as arrows in one frame — 18 ms, 3 ms, 18 ms — a
**6:1 ratio** between the reversal case and the same-direction case. It is the
figure that puts the two sentences on one axis as two spacings, and so makes the
comparison the page's own.

## RTL and measurement

`step_ena_cnt` is one nine-bit counter that counts up while `step_ena` is low
and is reset by every accepted `_step` rising edge
([`paula_floppy.v#L594-L603`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L594-L603)).
`step_ena` is bit 8, so the gate opens 256 `clk7_en` ticks after the last reset:
one threshold, and `direc` and `_sel` are not read in the block. The track update
does consult direction and selection
([`#L671-L680`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L671-L680)),
but the counter reset carries no such gate, so a reversal edge and a
same-direction edge are admitted by the same `step_ena_cnt[8]`.

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

A tick is one `clk7_en`-enabled clock edge; 253 rather than 256 is the pulse
framing of the probe, not a different threshold. The 253:253 is one genuine
reversal gap and one genuine same-direction gap, each binary-searched for its
own minimum accepted spacing. A same-direction probe cannot be read at `_track0`
directly, so `gap_accepted_same_direction` in
[`test_ch8-drive-step-timing.cpp`](../../hrm/ch8-drive-step-timing/test_ch8-drive-step-timing.cpp)
drives one more reversal 600 ticks later that is accepted regardless, folding the
state back onto the track-0 boundary (2→1 if the probe was accepted, 1→0 if
rejected).

`FIXTURE-LIVE`, `DIREC-PROTOCOL`, `TRACK00-POLARITY` and `SEL-GATING` are green
in the same run.

## Risks and unknowns

- **The page never says the two cases must be gated differently.** It prints two
  numbers for two situations; a single floor at or above the larger violates
  neither. The finding measures that NanoMig does not distinguish them, a weaker
  statement than "NanoMig is non-conforming".
- **The two sentences are not parallel in form.** The 18 ms sentence states a
  requirement in so many words (*"a minimum … is required"*); the 3 ms sentence
  states a drive guarantee, the head *arrives* within 3 ms, from which the
  programmer's interval follows but is not spelled out. That asymmetry may be
  deliberate. The 6:1 comparison rests on Figure 8-8 treating them as two
  spacings; a reader who takes only the prose could argue the page states one
  minimum, not two.
- **The absolute value is not measured.** No constant in `paula_floppy.v` is tied
  to a real-time unit anywhere reachable from this fixture, which is why
  `PULSE-WIDTH-FLOOR` and `RATIO-B` are declined and `RATIO-A` is a ratio: 253:253
  is 1:1 whatever a tick is worth. Whether the 256-tick floor is above 18 ms,
  between the two or below both is not determinable at this boundary. If it sat
  above 18 ms, the ratio would be the only thing wrong and nothing would be
  observably wrong at all.
- **No implementation of the four reproduces the printed 6:1 as an acceptance
  rule.** Three gate exactly as NanoMig does, and the fourth's two thresholds are
  numerically equal.

## Independent evidence

### What the others say

Not one of the four gates step-pulse acceptance on the printed pair, and only
one of the four conditions it on direction at all.

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:1855-1864`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L1855-L1864), [`:246`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L246) | one threshold, no direction term, the same shape as NanoMig: `MIN_STEPLIMIT_CYCLE` is `CYCLE_UNIT * 140`, and `drive_step` receives the direction and never consults it for spacing. The gate is off entirely unless the CPU runs at real speed: `set_steplimit` returns when `currprefs.m68k_speed != 0` ([`:1795-1802`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L1795-L1802)) |
| **vAmiga** | [`FloppyDrive.cpp:881-899`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L881-L899) | the only one with a direction-conditioned acceptance term, and its two values are equal: under the default `A1010` mechanics both are `USEC(40)` ([`:636-659`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L636-L659)), so the ratio is 1:1, NanoMig's. The author's own comment on the reversal check reads *"some extra-time is needed (?)"*; `readyToStepDown` mirrors it ([`:901-919`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L901-L919)) |
| **Denise** | [`emulation/libami/drive/diskDrive.cpp:526-532`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L526-L532) | one threshold, and its comment is the 3 ms sentence verbatim; `dir` is read for the move, never for the gate. Both stepper timings are user sliders that default to 0, off ([`libami/interface.cpp:43-44`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/interface.cpp#L43-L44), applied at [`program/program.cpp:257`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/program/program.cpp#L257)); at maximum the acceptance floor ("Drive Stepper Access") reaches 3.0 ms for both directions and the step deferral ("Drive Stepper Delay") 18.0 ms |
| **Copperline** | [`src/floppy/mod.rs:3131-3153`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3131-L3153) | two checks, one constant: its comment reads *"the mechanism can move (~40 us), and a reversal needs the same gap after the last step the other way"*, and it applies the same floor, `MIN_STEP_PULSE_CCK = 140` ([`:112`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L112)), the 140 WinUAE uses, calibrated per its comment against vAmigaTS `Drive/step2` and `step3` |
| **MiSTer** | [`rtl/paula_floppy.v:292-301`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L292-L301) | descendant — byte-identical to NanoMig's `:594-603` |
| **TC64** | [`rtl/minimig/paula_floppy.v:528-537`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L528-L537) | descendant — the same counter and `assign step_ena = step_ena_cnt[8];`, with `step_ena_cnt <= 9'd0;` where NanoMig and MiSTer write `step_ena_cnt <= 0;`, same value and reset condition; no line of the block in any of the three reads `direc` or `_sel` |

**Where the printed numbers live in three of them**, a mechanism NanoMig does not
model:

- **Copperline** keeps the 3:18 pair, direction-conditioned, as a read-data
  settle window: `SEEK_STEP_SETTLE_CCK` is 3 ms and `SEEK_REVERSAL_SETTLE_CCK`
  is 18 ms ([`mod.rs:103`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L103),
  [`:113`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L113)),
  selected at [`:3173-3179`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L3173-L3179).
  It is the only place in the four where the printed 6:1 appears as 6:1, and
  its comment is explicit that position sense and /TRK0 are unaffected: the head
  moves instantly, only the data is withheld.
- **vAmiga** has `getTrackToTrackDelay()` = `MSEC(3)` and `getHeadSettleTime()` =
  `MSEC(9)` ([`FloppyDrive.cpp:661-685`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L661-L685)),
  summed into `latestStepCompleted` and spent on returning random bits instead of
  track data while the head is in flight
  ([`:802`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Peripherals/Drive/FloppyDrive.cpp#L802)).
- **Denise** defers the step itself by `stepperSeekTime` when it is enabled
  ([`diskDrive.cpp:449-458`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L449-L458),
  [`:289-298`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/drive/diskDrive.cpp#L289-L298)),
  with its own comment: *"Continuous stepping can be done very quickly. The last
  step, however, requires about 10 ms until data can be reliably read."*

On the question `RATIO-A` grades, does the acceptance threshold depend on
direction, the answer is *no* in WinUAE, Denise and Copperline, and *yes
structurally but 1:1 numerically* in vAmiga. NanoMig is in the majority; what
separates it is the seek delay the three others spend and `paula_floppy.v` does
not: the track register updates and the data follows immediately.

## Decision

The question this entry hands over is not "is 253 ticks the right number". It is
whether p.245 imposes two step-spacing requirements on a controller, or describes
two properties of the mechanism.

### Suggested change

**None on the strength of this measurement alone**, and the reason is in the
page rather than in the RTL.

If p.245's two numbers are read as two *minimum spacings a drive enforces*, a
conforming implementation would need a second, longer floor selected by direction
reversal — the shape vAmiga already has, with `SEEK_REVERSAL_SETTLE_CCK`-style
values rather than vAmiga's equal ones. That is a real change to
`paula_floppy.v`: a direction-history bit and a second compare. The more useful
change may then be the settle behaviour three of the four model and NanoMig does
not, since that is what period software could actually observe.

If they are read as **the mechanism's own characteristics, which software must
respect and the controller need not enforce**, nothing in `paula_floppy.v` is
wrong. A drive that accepts pulses faster than the printed rate is what the page
means by *"some drives will support a much faster rate"*, and software that steps
too fast breaks against the mechanism, not against the gate. This is the reading
the other four implementations act on. It is not what the page says.
