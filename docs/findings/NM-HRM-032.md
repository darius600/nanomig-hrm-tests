# NM-HRM-032 — BEAMCON0's three sync-polarity bits are decoded nowhere

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `agnus_beamcounter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L140-L142) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L146-L148) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch7-beamcon0-ecs-control-bits`](../../hrm/ch7-beamcon0-ecs-control-bits) · corroborated by [`aa/aa-beamcon0-control-bits`](../../aa/aa-beamcon0-control-bits), which reaches the same red from the AA specification |
| **Chapter** | 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | `P11-BEAMCON0-CSYTRUE-BIT2-POLARITY-CONTROL-FOR-C-SYNC-PIN`, `P12-BEAMCON0-VSYTRUE-BIT1-POLARITY-CONTROL-FOR-V-SYNC-PIN`, `P13-BEAMCON0-HSYTRUE-BIT0-POLARITY-CONTROL-FOR-H-SYNC-PIN` — all three red |
| **Verdict of the four** | **split**: only WinUAE implements all three; Denise implements one; two do not model them at all |

---

## What the HRM says

**Appendix C, printed p. 305**, in the `BEAMCON0` bit table:

> ```
> 2     CSYTRUE    Polarity control for C sync pin
> 1     VSYTRUE    Polarity control for V sync pin
> 0     HSYTRUE    Polarity control for H sync pin
> ```

Three bits, each naming a pin whose polarity it controls.

---

## What NanoMig does

The three bits are declared and **commented out**, and nothing reads
`beamcon0_reg[2:0]`:

[`src/minimig-aga/agnus_beamcounter.v#L138-L142`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L138-L142)

```verilog
//wire varcsyen     = beamcon0_reg[ 4];
//wire blanken      = beamcon0_reg[ 3];
//wire csynctrue    = beamcon0_reg[ 2];
//wire vsynctrue    = beamcon0_reg[ 1];
//wire hsynctrue    = beamcon0_reg[ 0];
```

The composite sync output is formed without them:

[`src/minimig-aga/agnus_beamcounter.v#L421`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/agnus_beamcounter.v#L421)

```verilog
assign _csync = _hsync & _vsync | vser; //composite sync with serration pulses
```

### Measured

Each bit written alone, and again with every other implemented BEAMCON0 bit set,
over a full field, comparing the trace byte for byte against the same run with
the bit clear:

| bit | trace | edges observed |
|---|---|---|
| 2 — CSYTRUE | `_csync` **identical** | 883 |
| 1 — VSYTRUE | `_vsync` **identical** | 3 |
| 0 — HSYTRUE | `_hsync` **identical** | 881 |

The edge counts are there to make the comparison non-vacuous: the traces are not
identical because nothing is happening on them.

---

## This is not `absent_signal`

`hrm/ch3-beam-counters-and-blanking` discarded these bits on the reasoning that
there was nothing at this boundary to observe. That reasoning does not hold:
`_csync`, `_vsync` and `_hsync` are **real outputs of this module** (port list,
l.44-46), and the manual says each bit transforms one of them. A bit the RTL
never decodes, driving a pin that exists, is a deviation and not an unobservable.

Chapter 7 therefore overrules chapter 3 on this point.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a local checkout
at the pinned revision named in the link.

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:9809-9817`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L9809-L9817) | <pre>if (beamcon0 & BEAMCON0_CSYTRUE) flags ^= DENISE_RGA_FLAG_CSYNC;<br>if (beamcon0 & BEAMCON0_VSYTRUE) flags ^= DENISE_RGA_FLAG_VSYNC;<br>if (beamcon0 & BEAMCON0_HSYTRUE) flags ^= DENISE_RGA_FLAG_HSYNC;</pre> all three |
| **Denise** | [`emulation/libami/agnus/agnus.cpp:1051-1053`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/agnus.cpp#L1051-L1053) | <pre>inline auto Agnus::csyncPolTrue(bool state) -> bool {<br>&nbsp;&nbsp;&nbsp;&nbsp;return (beamCon & CSYTRUE) ? !state : state;<br>}</pre> **CSYTRUE only** — no VSYTRUE or HSYTRUE exists anywhere in the tree |
| **vAmiga** | [`Core/Components/Agnus/AgnusRegs.cpp:550-570`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/AgnusRegs.cpp#L550-L570) | `pokeBEAMCON0` names all three in its bit-map comment and acts on **PAL and LOLDIS only** |
| **Copperline** | [`src/chipset/agnus.rs:27-42`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/agnus.rs#L27-L42) | defines `BEAMCON0_` constants for bits 3 and 5-14 — **no constant exists for bits 0, 1 or 2** |
| **MiSTer** | [`rtl/agnus_beamcounter.v:146-148`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_beamcounter.v#L146-L148) | descendant — identical to NanoMig: `csynctrue`/`vsynctrue`/`hsynctrue` are commented-out declarations, nothing reads `beamcon0_reg[2:0]` |
| **TC64** | [`rtl/minimig/agnus_beamcounter.v:171-173`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_beamcounter.v#L171-L173) | descendant — `rtl/minimig/agnus_beamcounter.v:171-173` diverges here: the three wires are **not** commented out — `wire hsynctrue = displaydual ? beamcon0_sh[0] : beamcon0_reg[0];` and likewise for `vsynctrue`, feeding two new module outputs, `hsyncpol`/`vsyncpol` (`:175-176`), that `agnus.v` forwards on outward. `csynctrue` (CSYTRUE, bit 2) is declared the same uncommented way but, like NanoMig's commented-out version, used nowhere else in the file — `_csync` is still formed at `:458` as `(_hsync & _vsync) vser`, no `csynctrue` term anywhere. So HSYTRUE and VSYTRUE reach a real output in TC64 that NanoMig and MiSTer both lack outright; CSYTRUE stays exactly as dead in TC64 as in the other two. A fix to CSYTRUE diverges from all three; a fix decoding HSYTRUE/VSYTRUE would diverge from MiSTer but meet TC64, which has already built that plumbing. **and not alike across its own three bits**. |

**Among the four independent implementations: one implements all three bits;
a second implements one of the three; two do not model them at all.** This
finding stands on the printed page, not on a consensus among implementations
— and it should not be reported as though everyone else did this and NanoMig
did not.

TC64 adds a fourth pattern found nowhere among the four independent readings:
two of the three bits live (HSYTRUE, VSYTRUE) and the third (CSYTRUE) dead,
same as NanoMig and MiSTer. MiSTer itself matches NanoMig exactly — none of
the three implemented — which is expected of a descendant and is not a fifth
vote either way on what the manual requires.

That WinUAE implements all three is the strongest evidence that the bits do
something on real hardware: WinUAE's chipset work is the longest-running
reverse-engineering effort of the four and its source carries hardware
measurements.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *Denise* specification, part 252126-01 (ECS); US patent 4,874,164, Commodore-Amiga.

---

## A second document says the same thing

**The 1991 AA functional specification reaches this finding independently**, and
the AA suite's own package for BEAMCON0 goes red on it. The AA bit table names
the three bits in its own column, and the paragraph below reads:

```
HSYTRUE,VSYTRUE,CSYTRUE= These change the polarity of the sync
```

`aa/aa-beamcon0-control-bits` runs a whole video line twice at the
`agnus_beamcounter` boundary -- once with the three bits clear, once with them
set -- sampling `_hsync`, `_vsync` and `_csync` once per colour clock, and
compares the traces:

```
MEASURED: over a line the three sync pins differ in 0 of 260 samples between
          bits clear and bits set; _hsync takes 2 level(s) either way
```

**Zero of 260.** The pins pulse -- the trace has two levels, so the fixture is
driving a beam -- and the bits change nothing about them.

That assertion's baseline is red, so its mutation runs the other way and must
make it GREEN: under `MUTATE=1` the second trace starts three colour clocks
later, which does move the pins, showing the comparison can see a difference
when there is one.

**This is corroboration, not a second finding.** Two independent documents, read
by four different readers across two rounds, state the same requirement, and one
RTL answers neither. No new key was issued for it.

## Suggested change

Uncomment the three declarations and invert the outputs they name:

```verilog
wire csynctrue    = beamcon0_reg[ 2];
wire vsynctrue    = beamcon0_reg[ 1];
wire hsynctrue    = beamcon0_reg[ 0];
...
assign _csync = csynctrue ^ (_hsync & _vsync | vser);
```

The polarity convention needs care: these outputs are active low by name
(`_csync`), while the manual's "TRUE" bits are written from the active-high
perspective. WinUAE's XOR against its own sync flags is the model to follow.

---

## What holds the finding up

- **The comparison is not vacuous.** Each trace carries hundreds of edges
  (`_vsync` three, one per field), so "identical" means the waveform did not
  change, not that there was no waveform.
- **Each bit is tested twice**: alone, and with every other implemented BEAMCON0
  bit set, which rules out a missing enable elsewhere in the register.
- **The fixture reaches BEAMCON0.** Other bits of the same register, written
  through the same path in the same package, do change behaviour — PAL (bit 5)
  moves the frame from 262 to 312 lines.
- **The mutation is the `stimulus green` form**, which is the only available
  demonstration for an assertion that is red at baseline, and the baseline
  colour was checked rather than assumed.

---

## For the specialists

The narrow question: do CSYTRUE, VSYTRUE and HSYTRUE change anything on a real
ECS Agnus, or are they inert there too? Two of the four emulators model them not
at all, which could mean the bits are unimportant in practice — or only that no
software exercises them. WinUAE implementing all three, and Denise implementing
exactly one, is the kind of split that usually means somebody measured one of
them and nobody measured the others.
