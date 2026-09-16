# Findings

> **One page per finding now lives in [`findings/`](findings/).** Each has the
> same six parts — what the HRM says, what NanoMig does, what the other four say,
> a suggested change, what holds the finding up, and what is left for
> specialists — with every source quoted and permalinked at a pinned revision.
> Those pages are the maintained form and are written to be readable by someone
> who has never seen this project.
>
> This file stays as the **register**: the table below is what
> `tools/list_reds.py` reads, and the sections under it are the original text.

> **Deciding what to do about these:** see [`rtl-decisions.md`](rtl-decisions.md),
> which sets each open finding beside WinUAE's and vAmiga's implementation of the
> same question, with line citations verified against the commits
> `third-witness.md` records.

Every red test in this suite points at a place where the RTL does something the
*Amiga Hardware Reference Manual* does not describe. This file says which, so
that a red result is readable without access to anything outside this
repository.

**These are not broken tests.** Adjusting an assertion to make one pass deletes
the finding.

**There is exactly one exception:** an assertion that misstates the source must be corrected, not because
the implementation fails it, but because it never said what the manual says. The
test is the thing being fixed there, not the finding. Telling the two apart is
the whole discipline — the question is never "does this pass?" but "does the
printed page require what this asserts?".

> **An independent validation revised seven of chapter 3's findings.** Every
> affected entry below carries its own note; the round's own account is in
> [`record-2026-09-11-validation.md`](record-2026-09-11-validation.md).

## The finding keys

Findings are numbered **`NM-HRM-nnn`** -- `NM` for the implementation under test,
`HRM` for the *Amiga Hardware Reference Manual* that judges it. The key names the
**pair that disagrees**, and it does so deliberately, because it must not
prejudge which side is wrong: on six of the findings below, two independent
emulators do what NanoMig does and the printed sentence is the outlier.

A number is issued once. It is never reused, never reordered, and a withdrawn
finding keeps both its number and its entry -- the reasoning that withdrew it is
the result, not a deletion.

## Two layers are under test, and a finding must say which

The *Amiga Hardware Reference Manual* specifies a **system**, not a chip.
Chapter 8 alone covers the controller ports, the disk drive, the keyboard, the
serial and parallel connectors and the monitor. So the thing a page specifies is
not always a custom chip, and NanoMig answers for it in one of two layers.

| layer | what it is | how a finding there travels |
|---|---|---|
| `src/minimig-aga` | the custom-chip reimplementation, shared with `MiSTer-devel/Minimig-AGA_MiSTer` — byte-identical in places | **transfers to the Minimig family.** Several findings below are marked `inherited` and belong upstream |
| `src/misc` | the board's substitutes for Amiga peripherals — `hid.v` and `amiga_keymap.v` stand in for the keyboard and its microprocessor | **does not transfer.** Every Minimig-derived core solves this differently |

That difference is measured, not assumed. In the upstream tree at `da7632e`
there is no `amiga_keymap.v`, no `keymap` and no `hid.v`: `rtl/minimig.v:207`
takes `kbd_mouse_data` as a **top-level input** and the HID translation happens
above the chipset, on the MiSTer framework's ARM side. MiST and other Minimig
boards solve it differently again.

**A finding graded against the second layer therefore carries two extra
statements, and is incomplete without them:**

- its **Upstream** row says *NanoMig's own — no counterpart upstream*, naming
  where upstream does the same work instead;
- its **Report to** row says *NanoMig only*, because a reader who takes it as a
  statement about Minimig will be wrong.

Which layer answers for a page is decided by what the page specifies, not by a
directory. Appendix G specifies the keyboard as a device with its own
microprocessor, so the keyboard layer answers for it. p.260 specifies a 23-pin
connector for which NanoMig has no counterpart at all — the board has HDMI,
which is a replacement rather than a reimplementation and which the manual does
not specify. Such a claim is not a finding; it is outside the system the manual
describes.

**Why the keys were renumbered.** Findings previously carried
`FPGA-nnn` keys borrowed from the private workspace where this work is planned.
That workspace is not published. No reader outside it can resolve such a
reference, so a failing assertion printed a key that led nowhere -- and the
borrowed numbers carried the gaps of a task sequence (`050, 051, 060, 062, 069`)
that has nothing to do with findings. Where an older commit message or artefact
cites an `FPGA` number, this resolves it:

| now | was | | now | was | | now | was |
|---|---|---|---|---|---|---|---|
| NM-HRM-001 | FPGA-050 | | NM-HRM-009 | FPGA-091-NM-HRM-009 | | NM-HRM-016 | FPGA-091-NM-HRM-016 |
| NM-HRM-002 | FPGA-051 | | NM-HRM-010 | FPGA-091-NM-HRM-010 | | NM-HRM-017 | FPGA-091-NM-HRM-017 |
| NM-HRM-003 | FPGA-060 | | NM-HRM-011 | FPGA-091-NM-HRM-011 | | NM-HRM-018 | FPGA-091-NM-HRM-018 |
| NM-HRM-004 | FPGA-062 | | NM-HRM-012 | FPGA-091-NM-HRM-012 | | NM-HRM-019 | FPGA-091-NM-HRM-019 |
| NM-HRM-005 | FPGA-069 | | NM-HRM-013 | FPGA-091-NM-HRM-013 | | NM-HRM-020 | *newly issued* |
| NM-HRM-006 | FPGA-091-NM-HRM-006 | | NM-HRM-014 | FPGA-091-NM-HRM-014 | | NM-HRM-021 | *newly issued* |
| NM-HRM-007 | FPGA-091-NM-HRM-007 | | NM-HRM-015 | FPGA-091-NM-HRM-015 | | NM-HRM-022 | *newly issued* |
| NM-HRM-008 | FPGA-091-NM-HRM-008 | | NM-HRM-037 | *newly issued* | | NM-HRM-039 | *newly issued* |
| | | | NM-HRM-038 | *newly issued* | | NM-HRM-040 | *newly issued* |

`FPGA-nnn` still appears in test sources in a second, unrelated role, and those
references are kept: `// Primary oracle: Frozen FPGA-019 specification` marks a
package whose oracle is a frozen module specification rather than the manual, and
therefore **not** an HRM-conformance test. That distinction is worth more than
the resolvability of the number.

## The register

Every finding, with the package that measures it and what the third witness says.
**The emulators are not an oracle and never decide a finding** -- they are read
only for the report to the hardware developers
([`third-witness-ch3-ch6.md`](third-witness-ch3-ch6.md), whose section letters
this column cites). The manual decides; a red result stands until an Amiga
specialist rules on it.

| Key | Subject | Status | Package | Third witness |
|---|---|---|---|---|
| [NM-HRM-001](findings/NM-HRM-001.md) | PAL field terminates at 313 lines, not 312 | **withdrawn** | `hrm/agnus_beamcounter` | **all four** run a 313-line PAL field; Table 7-5 sanctions 313 |
| [NM-HRM-002](findings/NM-HRM-002.md) | audio channels on the wrong stereo side | **red** | `hrm/paula_audio_mixer` | **2–2**: WinUAE and vAmiga with chapter 5; Denise and Copperline with the pin list |
| [NM-HRM-003](findings/NM-HRM-003.md) | OCS Copper pointer uses five high bits, not three | **red** | `hrm/agnus_copper_control`, `hrm/agnus_copper_locreg_width` | **Copperline narrows the REGISTER to 19 bits on OCS**, matching the manual; **WinUAE narrows the BUS** on OCS (`memory.cpp:2619-2623`), which has the same effect on a fetched address; Denise and vAmiga neither |
| [NM-HRM-004](findings/NM-HRM-004.md) | ECS danger bit admits too small a range | **red** | `hrm/agnus_copper_move_range`, `hrm/agnus_copper_move_sweep` | both block below `$80` regardless of chipset |
| [NM-HRM-005](findings/NM-HRM-005.md) | WAIT and SKIP take longer than the manual states | **red** | `hrm/agnus_copper_cycle_times`, `hrm/agnus_copper_cycles` | vAmiga gives NanoMig's three numbers |
| [NM-HRM-006](findings/NM-HRM-006.md) | DDFSTRT bit H3 gated by chipset, not by resolution | **red** | `hrm/ch3-ddf-fetch-window` | **all four** gate DDF on the chipset; none on resolution |
| [NM-HRM-007](findings/NM-HRM-007.md) | hires fetch cannot reach the manual's 49 words | **red** | `hrm/ch3-ddf-fetch-window`, `hrm/ch3-bitplane-memory-layout` | **B4** for the first two; Copperline asserts an even hires word count in its own test |
| [NM-HRM-008](findings/NM-HRM-008.md) | playfield-1 collision depends on playfield 2 | **red** | `hrm/ch3-collision-registers` | **all four** gate playfield 1 on playfield 2 in single-playfield mode |
| [NM-HRM-009](findings/NM-HRM-009.md) | CLXDAT bit 15 reads as 1, not 0 | **withdrawn** | `hrm/ch3-collision-registers` | **all four** set bit 15 on every CLXDAT read |
| [NM-HRM-010](findings/NM-HRM-010.md) | the scan line is a whole number of colour clocks | source observation | `hrm/ch3-beam-counters-and-blanking` | Denise and Copperline both alternate whole line lengths |
| [NM-HRM-011](findings/NM-HRM-011.md) | PAL vertical blanking is 26 lines, manual says 29 | **red** | `hrm/ch3-beam-counters-and-blanking` | **B6** for the first two; Denise's PAL-NTSC gap is 5 lines like NanoMig's, not the manual's 8 |
| [NM-HRM-012](findings/NM-HRM-012.md) | SPRxCTL bit 0 (SH0) honoured at every resolution | **withdrawn** | `hrm/ch3-sprite-registers` | three of four take SH0 at every resolution, as NanoMig does |
| [NM-HRM-013](findings/NM-HRM-013.md) | SPRxCTL bits 3 and 4 are read nowhere | **withdrawn**, split into NM-HRM-019 | `hrm/ch3-sprite-registers` | bit 4 is real from ECS on, bit 3 only on AGA -- the split this was withdrawn into |
| [NM-HRM-014](findings/NM-HRM-014.md) | the manual applies its own ECS pointer note inconsistently | source observation | `hrm/ch3-register-map-agnus` | one of four extends every DMA pointer by chipset; no implementation can settle a question about the manual's own text |
| [NM-HRM-015](findings/NM-HRM-015.md) | BRDNTRAN is declared and never connected | **red** | `hrm/ch3-horizontal-display-window` | **B3** for the first two; **Copperline implements it, with a test** |
| [NM-HRM-016](findings/NM-HRM-016.md) | SuperHires sprite colour encoding not implemented | **red** | `hrm/ch3-shres-sprite-colour-encoding` | **2-2**: WinUAE and vAmiga reinterpret; Denise and Copperline do not |
| [NM-HRM-017](findings/NM-HRM-017.md) | SuperHires bitplane colour encoding not implemented | **red** | `hrm/ch3-shres-bitplane-colour-encoding` | **2-2**: the same split as NM-HRM-016, from the same reading |
| [NM-HRM-018](findings/NM-HRM-018.md) | ECS BPLCON0 bit 4 (SPRHWRM) has no effect | **withdrawn** | `hrm/ch3-bplcon0-display-modes` | neither Denise nor Copperline gives bit 4 an effect on ECS |
| [NM-HRM-019](findings/NM-HRM-019.md) | SPRxCTL bit 4 (SHSH1) is not implemented | **red** | `hrm/ch3-sprite-registers` | **C2** for the first two; Denise and Copperline both implement bit 4 |
| [NM-HRM-020](findings/NM-HRM-020.md) | BLTSIZV is not gated on ECS, while its siblings are | **red** | `hrm/agnus_blitter_ecs` | **A2** — **all four** gate it; NanoMig alone does not |
| [NM-HRM-021](findings/NM-HRM-021.md) | DOFF (BLTCON1 bit 7) has no observable effect | **red** | `hrm/agnus_blitter_ecs` | **C1**; Denise implements it and ECS-gates it |
| [NM-HRM-022](findings/NM-HRM-022.md) | BLTxPTH carries five high bits in both chipsets | **red** | `hrm/agnus_blitter_ecs` | **B2** — Copperline narrows every DMA pointer by Agnus revision, and WinUAE narrows the Agnus bus on OCS (`memory.cpp:2619-2623`) rather than the register |
| [NM-HRM-023](findings/NM-HRM-023.md) | the ATTACH bit is honoured in the even sprite, not only the odd | **red** | `hrm/ch4-attached-sprites` | **Denise applies the manual's rule exactly**: `(nr & 1) && (value & 0x80)` |
| [NM-HRM-024](findings/NM-HRM-024.md) | the audio DMA pointer takes five high bits in both chipsets | **red** | `hrm/ch5-audio-location-pointer` | **B2**, as NM-HRM-003 and NM-HRM-022 — Copperline alone narrows the REGISTER by revision; WinUAE narrows the bus instead (`memory.cpp:2619-2623`) |
| [NM-HRM-025](findings/NM-HRM-025.md) | volume 64 and volume 63 are the same gain on the multiply path | **red** | `hrm/ch5-audio-volume-scale`, `hrm/ch5-audio-register-map` | **all four** clamp at 64 and keep it distinct; two say why in their own comments |
| [NM-HRM-026](findings/NM-HRM-026.md) | the audio state codes are not the ones Figure 5-8 prints | source observation | `hrm/ch5-audio-channel-state` | **all four** use the figure's numbering; Copperline names the five constants with it |
| [NM-HRM-027](findings/NM-HRM-027.md) | in direct mode the interrupt is raised before a word is output | **red** | `hrm/ch5-audio-direct-output` | not read — and the manual states the moment four different ways |
| [NM-HRM-028](findings/NM-HRM-028.md) | ADKCON's audio attach bits are stored and reach nothing | **red** | `hrm/ch5-audio-dmacon-adkcon` | **all four** implement attach and modulation; NanoMig's own first line says it does not |
| [NM-HRM-029](findings/NM-HRM-029.md) | Figure 5-8's `dmasen` note does not hold outside its own exception | **withdrawn** | `hrm/ch5-audio-channel-state` | not comparable — `dmasen` appears at no pin |
| [NM-HRM-030](findings/NM-HRM-030.md) | Paula does not decode DMACONR, though the appendix gives it part ownership | **red** | `hrm/ch5-audio-dmacon-adkcon` | not read — the emulators are monolithic and model no per-chip decode |
| [NM-HRM-031](findings/NM-HRM-031.md) | INTREQ bit 14 is a settable request bit where the appendix says enable-only | **red** | `hrm/ch5-audio-interrupt` | **vAmiga and Denise also allow it**; NanoMig's own comment calls it undocumented |
| [NM-HRM-032](findings/NM-HRM-032.md) | BEAMCON0's three sync-polarity bits are decoded nowhere | **red** | `hrm/ch7-beamcon0-ecs-control-bits` | **split**: only WinUAE implements all three, Denise implements CSYTRUE alone, two model none |
| [NM-HRM-033](findings/NM-HRM-033.md) | LOLDIS does not disable the long-line toggle on its own | **red** | `hrm/ch7-beamcon0-ecs-control-bits` | **all four** use LOLDIS alone; NanoMig also requires VARBEAMEN |
| [NM-HRM-034](findings/NM-HRM-034.md) | VPOSR reports the video standard in bit 12 of an OCS configuration | **red · contested** | `hrm/ch7-beam-position-word-layout` | **2-2**: WinUAE and Denise do the same; and no implementation leaves bits 14-1 unused |
| [NM-HRM-035](findings/NM-HRM-035.md) | any write to CIA-A clears the kickstart overlay, not only a write to the OVL bit | **red** | `hrm/ch7-overlay-source` | **all four** follow the OVL bit's value; three name the only registers that may move it |
| [NM-HRM-036](findings/NM-HRM-036.md) | chip memory banks 1-3 stay reachable while the overlay is active | **red · manual likely wrong** | `hrm/ch7-reset-overlay` | **3-1**: vAmiga, Denise and Copperline replace only the ROM's 512K, as NanoMig does |
| [NM-HRM-037](findings/NM-HRM-037.md) | the right controller port's X pot input does not exist | **red** | `hrm/ch8-pot-counter-start-and-charge` and three siblings | **none of the four** reproduces it — all give the right X counter what the other three get |
| [NM-HRM-038](findings/NM-HRM-038.md) | the pot counters are not held in reset after POTGO | **red** | `hrm/ch8-pot-counter-start-and-charge` | **all four hold, all four put PAL at 8**; they split 2-2 on NTSC, which reads the page's own "seven or eight" as a video-standard split |
| [NM-HRM-039](findings/NM-HRM-039.md) | Table 8-2's wrapped rows compute against 255 on a 256-state counter | source observation | `hrm/ch8-mouse-counter-wraparound` | **unanimous**: 256 states per axis, and not one uses 255 as a modulus |
| [NM-HRM-040](findings/NM-HRM-040.md) | CIAADDRA's one-to-one direction mask holds for 2 bit positions of 8 | **red · contested** | `hrm/ch8-ciaa-port-direction` | **three-way split**; Copperline reproduces it bit for bit, and two of four gate the printed behaviour on a CIA part number |
| [NM-HRM-041](findings/NM-HRM-041.md) | DSKBYTR is a constant word in three of its five fields | **red** | `hrm/ch8-dskbytr-status-register` and two siblings | **unanimous against** — all four clear DSKBYT on the read, drive WORDEQUAL from a live comparator and put a real disk byte in DATA; two implement p.248's own two-microsecond window |
| [NM-HRM-042](findings/NM-HRM-042.md) | the disk DMA pointer stores 20 word-address bits where two pages print 32 | **red** | `hrm/ch8-disk-dma-pointer` | **no implementation reaches 32 bits either**: WinUAE and vAmiga keep a 32-bit variable they can never fill, Denise and Copperline mask the stored value to 21 bits or fewer |
| [NM-HRM-043](findings/NM-HRM-043.md) | DSKPTH carries five high bits in both chipsets | **red** | `hrm/ch8-disk-pointer-ecs-and-copper` | **B2, as NM-HRM-003, NM-HRM-022 and NM-HRM-024 — Copperline alone narrows the register by Agnus revision**, here with a test naming DSKPT; WinUAE narrows the Agnus bus instead, which the three earlier entries do not record |
| [NM-HRM-044](findings/NM-HRM-044.md) | the manual permits Copper initialisation of DSKPTH on p.279 and forbids it on p.289 | source observation | `hrm/ch8-disk-pointer-ecs-and-copper` | **unanimous, and it settles nothing**: all four block the Copper at $020 on OCS and allow it under ECS with CDANG — evidence about which reading practitioners took, not about which page is right |
| [NM-HRM-045](findings/NM-HRM-045.md) | DMAON needs a third term that p.278 does not print | **red** — against p.278, not against the RTL | `hrm/ch8-dmaon-enable-composition` | **unanimous against p.278**: all four require the DSKEN term, and p.248's three-term reading is green in the same run |
| [NM-HRM-046](findings/NM-HRM-046.md) | the drive activity light follows DMA, not the motor | **red** | `hrm/ch8-drive-select-motor-led` | **unanimous against NanoMig**: all four drive their indicator from the motor; two add a select term, two use DMA only to colour it, none to light it |
| [NM-HRM-047](findings/NM-HRM-047.md) | one step-spacing threshold where the page prints two | **red** | `hrm/ch8-drive-step-timing` | **unanimous with NanoMig on the gate**: none of the four gates acceptance on direction or on the printed numbers; three spend 3 ms/18 ms on a read-data settle NanoMig does not model |
| [NM-HRM-048](findings/NM-HRM-048.md) | a documented hardware bug that does not occur here | **red · inverted** | `hrm/ch8-dsklen-dma-arming` | **3–1**: Copperline reproduces the bug with a named constant; WinUAE implements the paragraph's *other* sentence instead and calls it a Paula bug; vAmiga and Denise neither |
| [NM-HRM-049](findings/NM-HRM-049.md) | the MFM encoder is context-free | **red** | `hrm/ch8-mfm-cell-encoding` | **unanimous against** — all four carry the previous data bit across the word or byte boundary; Denise reprints the manual's table as the comment above its encoder |

| [NM-HRM-050](findings/NM-HRM-050.md) | one of the two international cutout keys reaches no code | **red** | `hrm/ch8-keycode-table` | **unanimous against** — all four carry both `$30` and `$2B` as distinct keys; NanoMig's table emits `$2B` for both and `$30` for none |
| [NM-HRM-051](findings/NM-HRM-051.md) | Caps Lock is delivered as an ordinary key | **red** | `hrm/ch8-keyboard-events` | **3–1 against** — WinUAE mirrors the host's lock, Denise models the LED, Copperline owns the toggle in its keyboard MCU; vAmiga has no Caps Lock case and behaves as NanoMig does |
| [NM-HRM-052](findings/NM-HRM-052.md) | p.252 and p.254 disagree about the right Alt key | source observation | `hrm/ch8-keycode-table` | **unanimous for p.252, and it settles nothing**: all four name `$65` for the right Alt, which is evidence about which page implementers read, not about which page is right |

Chapter 1 has not been extracted, so no finding below comes from it.
Chapter 8's first slice — the Controller Port Interface — contributes
NM-HRM-037 to NM-HRM-040; its floppy slice NM-HRM-041 to NM-HRM-049 and its
keyboard slice NM-HRM-050 to NM-HRM-052. The serial slice is read but not
yet built.

**NM-HRM-050 and NM-HRM-051 are the first findings graded at `src/misc`**, the
peripheral-substitute layer admitted for the keyboard because Appendix G
specifies the keyboard as a device with its own microprocessor. Both carry the
qualifier the table above states: upstream has no `hid.v` and no `keymap` at
all, so neither finding transfers to the Minimig family. They are about this
core.

**Two of chapter 8's four are inherited from Minimig-AGA byte for byte**,
which is unusual in this register: NM-HRM-037's two lines and NM-HRM-040's
pair are identical upstream but for a trailing comment. Whoever they are
reported to, it is not only NanoMig.
The register is a record of what has been looked at, not of what is there.

**Chapter 7 also strengthened two existing findings without adding a number.**
NM-HRM-008 gains two printed sources it did not have, and NM-HRM-011 gains
chapter 7's own page and a contradiction inside it. Each says so on its own page.
A chapter that corroborates an existing finding from a different page has done
more for it than a new number would.

## Where the five oldest findings come from, and whether they hold


Five findings, but not of equal standing. Three came out of the pipeline and are
witnessed more than once. Two came out of the earlier hand-written run, rest on a
single citation each, and both of those citations turn out to be contradicted
elsewhere in the same manual.

| Finding | Origin | Standing |
|---|---|---|
| [NM-HRM-003](findings/NM-HRM-003.md) | pipeline | corroborated — p273 and p309 give two different widths |
| [NM-HRM-004](findings/NM-HRM-004.md) | pipeline | corroborated — two editions carry the appendix C sentence; challenged and upheld |
| [NM-HRM-005](findings/NM-HRM-005.md) | pipeline | corroborated — p20 and p275, units settled on p275 itself |
| [NM-HRM-002](findings/NM-HRM-002.md) | hand-written, Aug | **stands** — the manual contradicts itself, and four implementations split two against two |
| [NM-HRM-001](findings/NM-HRM-001.md) | hand-written, Aug | **withdrawn** — the manual sanctions the measured value |

Both August findings rested on a manual that says two different things, and
neither could be decided by reading the manual harder. They were settled by a
third witness — see [`third-witness.md`](third-witness.md) — and they went
opposite ways: one survived, one did not.

## NM-HRM-001 is withdrawn — and why the entry stays

The assertion was removed rather than left red. A red test is a claim that the
RTL is wrong, and this one could not support that claim.

The *file* was not deleted: `agnus_beamcounter` still asserts that a PAL field
terminates with EOF and that the horizontal counter spans 0..453. The second one
matters beyond this file — `agnus_copper_waitskip_range`'s W4b rests on the line
being 454 units long.

It was not replaced with a corrected assertion either. Doing that properly means
driving the long-frame bit deliberately and checking both cases, which is work
nobody has done, and an assertion guessing at it would be worse than none.

The reasoning:

### NM-HRM-001 — PAL frame has one line too many

See **[`findings/NM-HRM-001.md`](findings/NM-HRM-001.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

## Open findings

### NM-HRM-002 — audio channels on the wrong stereo side

See **[`findings/NM-HRM-002.md`](findings/NM-HRM-002.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-003 — OCS Copper address uses five high bits instead of three

See **[`findings/NM-HRM-003.md`](findings/NM-HRM-003.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-004 — ECS danger bit admits too small a range

See **[`findings/NM-HRM-004.md`](findings/NM-HRM-004.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-005 — WAIT and SKIP take longer than the manual states

See **[`findings/NM-HRM-005.md`](findings/NM-HRM-005.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

## Chapter 6 — the ECS blitter registers

Appendix C lists five ECS changes to the blitter. `agnus_blitter` takes an `ecs`
input and uses it for two of them. Three are findings and they all live in
`hrm/agnus_blitter_ecs`, each required to fail:

| Assertion | Measured |
|---|---|
| `BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS` | a BLTSIZV write with `ecs=0` and with `ecs=1` gives the identical height, r1=7 and r2=7 |
| `DOFF-NO-OBSERVABLE-EFFECT` | the same D write appears with BLTCON1 bit 7 clear and set |
| `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` | `BLTAPTH` written with `$001F` gives `address_out = 0xF8000` at both `ecs` settings |

Each names every claim it covers in its own message, so the red line is readable
without this file. The two remaining rows of the five-way table below are not
defects: BLTCON0L and BLTSIZH are gated correctly and pass.

### NM-HRM-020 — BLTSIZV is not gated on ECS, while its two siblings are

See **[`findings/NM-HRM-020.md`](findings/NM-HRM-020.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-021 — DOFF (BLTCON1 bit 7) has no observable effect

See **[`findings/NM-HRM-021.md`](findings/NM-HRM-021.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-022 — BLTxPTH carries five high bits in both chipsets

See **[`findings/NM-HRM-022.md`](findings/NM-HRM-022.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-023 — the ATTACH bit is honoured in the even sprite, not only the odd

See **[`findings/NM-HRM-023.md`](findings/NM-HRM-023.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### Appendix C's five ECS blitter changes are implemented five different ways

Appendix C, printed page 309, lists what ECS changed in the blitter. NanoMig's
`agnus_blitter` takes an `ecs` input. It uses it for two of the five:

| Appendix C | `agnus_blitter.v` | Gated by `ecs`? |
|---|---|---|
| `BLTCON0L` $05A new — control 0, lower 8 bits | line 259 | **yes** |
| `BLTSIZH` $05E new — H size and start | lines 500, 523, 557 | **yes** |
| `BLTSIZV` $05C new — V size, 15-bit | line 548 | **no** — and the comment on that line calls it ECS |
| `BLTxPTH` $050 chg — "high 5 bits, was 3 bits" | `agnus_blitter_adrgen.v:45`, `reg [20:16] bltpth [3:0]` | **no** — and now measured at the parent, see below |
| `DOFF`, BLTCON1 bit 7 — disable D output in line mode | nowhere | **not implemented** |

`bltcon1[7]` does not appear in `agnus_blitter.v`. The bits it reads are 15-12,
4, 3, 2, 1 and 0.

#### BLTxPTH, measured at the boundary that has the `ecs` port

The row above used to read "no — that module has no `ecs` port at all", which was
true and too weak. `agnus_blitter_adrgen` having no `ecs` input explains why the
gating cannot happen *there*; it leaves open whether the parent, which does take
`ecs`, gates it before handing the write down. It does not, and that is now
measured rather than inferred.

`hrm/agnus_blitter_ecs`'s `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` writes `BLTAPTH` with
`$001F` — all five low bits — twice on the same fixture, once with `ecs` low and
once with `ecs` high, and reads `agnus_blitter`'s own `address_out`:

    ecs=0 -> 0xF8000
    ecs=1 -> 0xF8000

Identical. All five bits are taken in both chip sets, so appendix A p.270's
"high 3 bits, 5 bits if ECS" is not implemented in either direction: an OCS
machine gets ECS width, and `ecs` changes nothing about the pointer.

The path is unbroken in the source. `agnus_blitter.v:588-592` instantiates the
address generator passing `data_in`, `reg_address_in` and `address_out` and **no
`ecs`**; the name does not occur anywhere in `agnus_blitter_adrgen.v`; and
`agnus_blitter_adrgen.v:62` is
`assign bltptr_in[20:1] = enaptr ? newptr[20:1] : {data_in[4:0], data_in[15:1]}`
— five bits, unconditionally.

**The control matters here.** A red that depends on toggling `ecs` is worthless if
the fixture's `ecs` toggle does nothing, so the assertion names
`BLTCON0L-GATED-BY-ECS` — green in the same file, on the same harness — as proof
that this fixture's `ecs` does change behaviour elsewhere. And a mutation confirms
the observable is live rather than stuck: driving `$0015` instead of `$001F` moves
both readings to `0xA8000`, still identical.

This finding exists because Prompt 8's independent validation rejected the earlier
handling. The three claims behind it — `6-appa-registers.c110`,
`ch6-appa-bltxpt.c2`, `ch6-appa-bltxpt.c6` — had been routed to `PKG-BLT-ADRGEN`,
one fixture below the `ecs` port, where the builder correctly declined them as not
constructible. The validator ruled that they needed real assertions at the parent
rather than relocated citations. They did, and the assertion came out red.

#### The same deviation, in both Agnus engines

The blitter measurement above sits next to NM-HRM-003 in this document, and the
two are worth reading together. NM-HRM-003 is the copper's version of exactly this finding: "OCS uses
five high address bits; the manual documents three." The blitter now shows the
same thing — and not merely something similar. The two pointers are built the same
way, from the same bits, with no chipset condition on either:

    agnus_copper.v:144    cop1lch[20:16] <= data_in[4:0];
    agnus_copper.v:197    address_out[20:1] <= {cop1lch[20:16], cop1lcl[15:1]};

    agnus_blitter_adrgen.v:62
        assign bltptr_in[20:1] = enaptr ? newptr[20:1]
                                        : {data_in[4:0], data_in[15:1]};

Five high bits from `data_in[4:0]`, fifteen low from `data_in[15:1]`, a 20-bit
word address, unconditional. Both engines take the ECS width in both chipsets, and
in both the manual documents three high bits for OCS and five for ECS.

That reframes what the two findings are. Separately they read as two omissions.
Together they look like one design decision applied consistently across Agnus:
this implementation does not model the OCS pointer narrowing anywhere. Whether
that is a deliberate simplification or an oversight is not something these tests
can answer — but it is a question about the whole chip, not about a register, and
neither the copper chapter nor the blitter chapter could have raised it alone.

Recorded rather than acted on. The copper half is NM-HRM-003 and is witnessed
more than once; the blitter half is NM-HRM-022. Whether one
decision closes both is the question this cross-reference exists to raise.

### The classified inventory says otherwise, in 109 records

Worth recording because it is the one place a wrong statement could have made
these findings disappear. **109 of the chapter-6 records carry an `evidence`
field asserting that `ecs` gates BLTSIZV** — wording like "`ecs` ... gates the
BLTCON0L, BLTSIZH and BLTSIZV decodes `agnus_blitter.v:500,523,557`". Those three
line numbers are BLTSIZH's and BLTCON0L's, and they are correct for those two.
BLTSIZV's decode is at line 548 and has no `&& ecs`:

```verilog
    else if (reg_address_in[8:1]==BLTSIZV[8:1]) // ECS BLTSIZV register
        height[14:0] <= data_in[14:0];
```

The comment on that line calls it ECS; the code does not gate it. The
classifier's evidence text generalised from the two registers that are gated to
the third that is not, and repeated it 109 times.

The **claims** are unaffected — appendix C does mark BLTSIZV "new" for ECS, and
the classification as testable is right. It is the stated reason that
misdescribes the RTL. A builder who had trusted it would have written an
assertion expecting a difference, seen none, and had no way to tell a defect from
its own mistake. PKG-BLT-ECS's builder read line 548 instead and sided with the
RTL, which is what the build brief's "if the brief and the document disagree, the
document wins — and if the RTL and both disagree, say so" rule is for.

The pointer row is the sibling of NM-HRM-003, one appendix over: the same register
family, the same "three bits in OCS, five in ECS" sentence, and the same
unconditional five bits in the RTL. The third-witness work already recorded that
neither vAmiga nor WinUAE narrows a pointer by chipset at all, which is worth
knowing before anyone calls the blitter case a defect on its own.

What this is *not* is a claim that any of the five is wrong. A design that is
always ECS/AGA has a defensible reason to widen a pointer unconditionally. What
it is: the same appendix, the same five lines, handled four different ways, with
no comment anywhere saying why the two ungated ones are ungated.

### `agnus_top_dmacon` asserts nothing

Found in the same pass and recorded where it belongs, in
[`test-inventory.md`](test-inventory.md), backlog item 5. It is the only test in
the suite that cannot fail.

## What the emulators say about the three Copper findings

Read out of interest rather than necessity — these three are not
contested, so an emulator cannot cancel them. The result is worth knowing anyway:

**On all three, WinUAE and vAmiga do what NanoMig does and not what the manual
says.**

| Finding | Manual | Both emulators, and NanoMig |
|---|---|---|
| [NM-HRM-003](findings/NM-HRM-003.md) | OCS keeps three high pointer bits | neither emulator narrows the pointer by chipset at all |
| [NM-HRM-004](findings/NM-HRM-004.md) | ECS with CDANG clear reaches $03E–$07E | CDANG clear blocks below $80 in both, chipset-independent |
| [NM-HRM-005](findings/NM-HRM-005.md) | MOVE 2, SKIP 4, satisfied WAIT 6 | 4 / 8 / 8 in vAmiga, the same three numbers NanoMig measures |

The citations are in [`third-witness.md`](third-witness.md).

This does not close any of them, and none of the assertions was touched. It
changes the question: not "is the RTL wrong" but "is the manual wrong about the
hardware". Two independent implementations agreeing against a manual sentence is
strong evidence, but choosing what NanoMig should follow — the manual it is
tested against, or the hardware it emulates — is a decision, not a measurement.

NM-HRM-004 is the sharpest case. Appendix C read literally says that *clearing* the
danger bit grants the range that *setting* it grants, which would leave the bit
with nothing to do.

## How a finding is meant to end

A red test is closed by a decision about the RTL, never by relaxing the
assertion. If a deviation turns out to be intentional, the assertion stays and
the reason is written down beside it — a test whose expectation was quietly moved
to match the implementation stops being evidence about anything.


### NM-HRM-006 — DDFSTRT/DDFSTOP bit H3 is gated by the chipset, not by resolution

See **[`findings/NM-HRM-006.md`](findings/NM-HRM-006.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-007 — the hires fetch width cannot reach the manual's maximum of 49 words

See **[`findings/NM-HRM-007.md`](findings/NM-HRM-007.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-008 — playfield-1 collision depends on playfield 2, by design, undocumented

See **[`findings/NM-HRM-008.md`](findings/NM-HRM-008.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-009 — CLXDAT bit 15 reads as 1, not 0

See **[`findings/NM-HRM-009.md`](findings/NM-HRM-009.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-010 — the horizontal line is a whole number of colour clocks, not 227.5

See **[`findings/NM-HRM-010.md`](findings/NM-HRM-010.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-011 — PAL vertical blanking is 26 lines, the manual says 29

See **[`findings/NM-HRM-011.md`](findings/NM-HRM-011.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-012 — SPRxCTL bit 0 (SH0) is honoured at every resolution, not only SuperHires

See **[`findings/NM-HRM-012.md`](findings/NM-HRM-012.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-013 — SPRxCTL bits 3 and 4 are read nowhere in the module

See **[`findings/NM-HRM-013.md`](findings/NM-HRM-013.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-014 — the manual applies its own ECS pointer extension inconsistently

See **[`findings/NM-HRM-014.md`](findings/NM-HRM-014.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-015 — BRDNTRAN is declared and never connected

See **[`findings/NM-HRM-015.md`](findings/NM-HRM-015.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-016 — SuperHires sprite colour encoding is not implemented

See **[`findings/NM-HRM-016.md`](findings/NM-HRM-016.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-017 — SuperHires bitplane colour encoding is not implemented

See **[`findings/NM-HRM-017.md`](findings/NM-HRM-017.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-018 — ECS BPLCON0 bit 4 (SPRHWRM) has no effect

See **[`findings/NM-HRM-018.md`](findings/NM-HRM-018.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### The finding was hidden by the test's own exit code

`main()` returned `0` unconditionally while printing
`=== ch3-bplcon0-display-modes: 225/227 PASS, 2 FAIL ===`. `run_all.sh` reads the
exit status, so the suite listed this package as **passing** for as long as it
existed, with both reds visible only inside its own directory. Fixed to
`return g_fail ? 1 : 0;` — a harness correction, no oracle touched. The package
now reports red to the suite, as it always should have.

This is the third instance in one day of the same shape: **a green report from
something that was not measuring.** The other two were the mutation gate reading
one assertion of nineteen, and six control assertions never mutated because a
`[control] ` prefix hid them.

*For the specialists:* NanoMig implements the AGA meaning of this bit and not the
ECS one. Whether SPRHWRM is worth having, or whether the AGA reading should win
outright, is a scope decision for the project — we only record that the printed
ECS behaviour is absent.


### NM-HRM-019 — SPRxCTL bit 4 (SHSH1) is not implemented

See **[`findings/NM-HRM-019.md`](findings/NM-HRM-019.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-024 — the audio DMA pointer takes five high bits in both chipsets

See **[`findings/NM-HRM-024.md`](findings/NM-HRM-024.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-025 — volume 64 and volume 63 are the same gain on the multiply path

See **[`findings/NM-HRM-025.md`](findings/NM-HRM-025.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-026 — the audio state codes are not the ones Figure 5-8 prints

See **[`findings/NM-HRM-026.md`](findings/NM-HRM-026.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-027 — in direct mode the interrupt is raised before a word is output

See **[`findings/NM-HRM-027.md`](findings/NM-HRM-027.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-028 — ADKCON's audio attach bits are stored and reach nothing

See **[`findings/NM-HRM-028.md`](findings/NM-HRM-028.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-029 — Figure 5-8's `dmasen` note does not hold outside its own exception

See **[`findings/NM-HRM-029.md`](findings/NM-HRM-029.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-030 — Paula does not decode DMACONR, though the appendix gives it part ownership

See **[`findings/NM-HRM-030.md`](findings/NM-HRM-030.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.

### NM-HRM-031 — INTREQ bit 14 is a settable request bit where the appendix says enable-only

See **[`findings/NM-HRM-031.md`](findings/NM-HRM-031.md)** — the manual quoted, the code quoted and permalinked at a pinned revision, what the other four do, a suggested change, and what holds the finding up.
