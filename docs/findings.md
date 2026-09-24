# Findings

**44 of the 65 HRM findings ask you to decide whether the RTL should change. The table is below the explanation — jump to [The register](#the-register).** The AA specification has a register of its own: [The AA register](#the-aa-register).

> **One page per finding now lives in [`findings/`](findings/).** Each has the
> same six parts — what the HRM says, what NanoMig does, what the other four say,
> a suggested change, what holds the finding up, and what is left for
> specialists — with every source quoted and permalinked at a pinned revision.
> Those pages are the maintained form and are written to be readable by someone
> who has never seen this project.
>
> "The other four" are the independent emulators WinUAE, vAmiga, Denise and
> Copperline. Each page also reads the two Minimig lines NanoMig descends from,
> six in all, but marks them `descendant` and does not count them: sharing
> NanoMig's code, they cannot witness what the manual means, only whether a
> finding is inherited
> ([why](findings/README.md#the-two-minimig-lines-are-ancestors-not-witnesses--and-nothing-merges-them-down)).
>
> This file stays as the **register**: the table below is what
> `hrm-testpipeline/tools/list_reds.py` reads — that script lives in the
> separate `hrm-testpipeline` repository, not this one — and the sections under
> it are the original text. The table is readable without it.

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
prejudge which side is wrong: on several of the findings below, independent
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

Older commit messages and artefacts cite `FPGA-nnn` as a finding key, and a few
`//` comments still name a package that no longer exists;
[`legacy-names.md`](legacy-names.md) resolves both.

**No test source takes a frozen specification as its oracle any more.** The
packages that did were removed, because a specification that describes what the
module does cannot decide whether it is right. Twelve chapter-2 packages do
still name one in their `Primary oracle` line, and they name it *second*: the
manual and its printed pages first, then the lab document the package was
planned from. That document is a planning artefact, not an oracle, and it is not
published — the claim ids beside it are the public record of the same work.

## The register

Every finding, with the package that measures it and what the third witness says.
**The emulators are not an oracle and never decide a finding** -- they are read
only for the report to the hardware developers
([`third-witness-ch3-ch6.md`](third-witness-ch3-ch6.md), whose section letters
this column cites). The manual decides; a red result stands until an Amiga
specialist rules on it.

**The status column reads `lifecycle · filed against · evidence`**, and every row
is built from the three fields at the top of the page it links to, so the two can
never disagree. Eight values occur:

| status | what it asks of you |
|---|---|
| `open · RTL · **red**` | decide whether the RTL should change |
| `open · RTL · **red**, contested` | the same, knowing the implementations split |
| `open · RTL · **red**, inverted` | the manual documents a defect NanoMig does not reproduce |
| `open · RTL · source observation` | nothing yet — read from the source, not measured |
| `open · page · **red**` | **no RTL change**; the printed page is the likely defect |
| `open · page · none` | the same, with no test built against it yet |
| `open · classification · **red**` | **no RTL change**; how the claim was classified is what is contested |
| `**withdrawn**` | nothing; kept so the claim is not raised a second time |

**If you have one evening: 40 of these 65 ask you to decide something.** The
rest are withdrawn, answered, or observations kept for context. The table below
is sorted by key, not by how much it wants from you — so the first row is not
the place to start. Start at one of these, whichever is nearest your own
interest:

| start here | because |
|---|---|
| [NM-HRM-003](findings/NM-HRM-003.md) | OCS Copper pointer width — a red, and two of the four implementations already do what the manual says |
| [NM-HRM-004](findings/NM-HRM-004.md) | all four implementations read the appendix the other way round from the manual — the clearest four-way signal in the register |
| [NM-HRM-064](findings/NM-HRM-064.md) | AUTOCONFIG register 4C — six red assertions, and the fix is one line MiSTer already has |

**[NM-HRM-002](findings/NM-HRM-002.md) sorts first and is the worst starting
point**: the manual contradicts itself there and the four implementations split
evenly, so it is the one finding in the register that reports to nowhere. Read
it for what an unresolvable case looks like, not for something to do.


| Key | Subject | Status | Package | Third witness |
|---|---|---|---|---|
| [NM-HRM-002](findings/NM-HRM-002.md) | audio channels on the wrong stereo side | open · RTL · **red** | `hrm/ch5-audio-stereo-routing` | **2–2**: WinUAE and vAmiga with chapter 5; Denise and Copperline with the pin list |
| [NM-HRM-003](findings/NM-HRM-003.md) | OCS Copper pointer uses five high bits, not three | open · RTL · **red** | `hrm/ch2-copper-control`, `hrm/ch2-copper-locreg-width` | **Copperline narrows the REGISTER to 19 bits on OCS**, matching the manual; **WinUAE narrows the BUS** on OCS (`memory.cpp:2619-2623`), which has the same effect on a fetched address; Denise and vAmiga neither |
| [NM-HRM-004](findings/NM-HRM-004.md) | ECS danger bit admits too small a range | open · RTL · **red** | `hrm/ch2-copper-move-range`, `hrm/ch2-copper-move-sweep` | both block below `$80` regardless of chipset |
| [NM-HRM-005](findings/NM-HRM-005.md) | WAIT and SKIP take longer than the manual states | open · RTL · **red** | `hrm/ch2-copper-cycle-times`, `hrm/ch2-copper-cycles` | vAmiga gives NanoMig's three numbers |
| [NM-HRM-006](findings/NM-HRM-006.md) | DDFSTRT bit H3 is respected in lores, where p.91 says hires only | open · page · **red** | `hrm/ch3-ddf-fetch-window` | **unanimous with NanoMig**: all four respect H3 at every resolution; what they gate on the chipset is register bit 1 |
| [NM-HRM-007](findings/NM-HRM-007.md) | hires fetch cannot reach the manual's 49 words | open · RTL · **red** | `hrm/ch3-ddf-fetch-window`, `hrm/ch3-bitplane-memory-layout` | **B4** for the first two; Copperline asserts an even hires word count in its own test |
| [NM-HRM-008](findings/NM-HRM-008.md) | playfield-1 collision depends on playfield 2 | open · RTL · **red** | `hrm/ch3-collision-registers` | **all four** gate playfield 1 on playfield 2 in single-playfield mode |
| [NM-HRM-010](findings/NM-HRM-010.md) | the scan line is a whole number of colour clocks | open · RTL · source observation | `hrm/ch3-beam-counters-and-blanking` | Denise and Copperline both alternate whole line lengths |
| [NM-HRM-011](findings/NM-HRM-011.md) | PAL vertical blanking is 26 lines, manual says 29 | open · RTL · **red** | `hrm/ch3-beam-counters-and-blanking` | **B6** for the first two; Denise's PAL-NTSC gap is 5 lines like NanoMig's, not the manual's 8 |
| [NM-HRM-014](findings/NM-HRM-014.md) | the manual applies its own ECS pointer note inconsistently | open · RTL · source observation | `hrm/ch3-register-map-agnus` | one of four extends every DMA pointer by chipset; no implementation can settle a question about the manual's own text |
| [NM-HRM-015](findings/NM-HRM-015.md) | BRDNTRAN is declared and never connected | open · RTL · **red** | `hrm/ch3-horizontal-display-window` | **B3** for the first two; **Copperline implements it, with a test** |
| [NM-HRM-016](findings/NM-HRM-016.md) | SuperHires sprite colour encoding not implemented | open · RTL · **red** | `hrm/ch3-shres-sprite-colour-encoding` | **2-2**: WinUAE and vAmiga reinterpret; Denise and Copperline do not |
| [NM-HRM-017](findings/NM-HRM-017.md) | SuperHires bitplane colour encoding not implemented | open · RTL · **red** | `hrm/ch3-shres-bitplane-colour-encoding` | **2-2**: the same split as NM-HRM-016, from the same reading |
| [NM-HRM-019](findings/NM-HRM-019.md) | SPRxCTL bit 4 (SHSH1) is not implemented | open · RTL · **red** | `hrm/ch3-sprite-registers` | **C2** for the first two; Denise and Copperline both implement bit 4 |
| [NM-HRM-020](findings/NM-HRM-020.md) | BLTSIZV is not gated on ECS, while its siblings are | open · RTL · **red** | `hrm/ch6-blitter-ecs` | **A2** — **all four** gate it; NanoMig alone does not |
| [NM-HRM-021](findings/NM-HRM-021.md) | DOFF (BLTCON1 bit 7) has no observable effect | open · RTL · **red** | `hrm/ch6-blitter-ecs` | **C1**; Denise implements it and ECS-gates it |
| [NM-HRM-022](findings/NM-HRM-022.md) | BLTxPTH carries five high bits in both chipsets | open · RTL · **red** | `hrm/ch6-blitter-ecs` | **B2** — Copperline narrows every DMA pointer by Agnus revision, and WinUAE narrows the Agnus bus on OCS (`memory.cpp:2619-2623`) rather than the register |
| [NM-HRM-023](findings/NM-HRM-023.md) | the ATTACH bit is honoured in the even sprite, not only the odd | open · RTL · **red** | `hrm/ch4-attached-sprites` | **Denise applies the manual's rule exactly**: `(nr & 1) && (value & 0x80)` |
| [NM-HRM-024](findings/NM-HRM-024.md) | the audio DMA pointer takes five high bits in both chipsets | open · RTL · **red** | `hrm/ch5-audio-location-pointer` | **B2**, as NM-HRM-003 and NM-HRM-022 — Copperline alone narrows the REGISTER by revision; WinUAE narrows the bus instead (`memory.cpp:2619-2623`) |
| [NM-HRM-025](findings/NM-HRM-025.md) | volume 64 and volume 63 are the same gain on the multiply path | open · RTL · **red** | `hrm/ch5-audio-volume-scale`, `hrm/ch5-audio-register-map` | **all four** clamp at 64 and keep it distinct; two say why in their own comments |
| [NM-HRM-026](findings/NM-HRM-026.md) | the audio state codes are not the ones Figure 5-8 prints | open · RTL · source observation | `hrm/ch5-audio-channel-state` | **all four** use the figure's numbering; Copperline names the five constants with it |
| [NM-HRM-027](findings/NM-HRM-027.md) | in direct mode the interrupt is raised before a word is output | open · RTL · **red** | `hrm/ch5-audio-direct-output` | not read — and the manual states the moment four different ways |
| [NM-HRM-028](findings/NM-HRM-028.md) | ADKCON's audio attach bits are stored and reach nothing | open · RTL · **red** | `hrm/ch5-audio-dmacon-adkcon` | **all four** implement attach and modulation; NanoMig's own first line says it does not |
| [NM-HRM-030](findings/NM-HRM-030.md) | Paula does not decode DMACONR, though the appendix gives it part ownership | open · RTL · **red** | `hrm/ch5-audio-dmacon-adkcon` | not read — the emulators are monolithic and model no per-chip decode |
| [NM-HRM-031](findings/NM-HRM-031.md) | INTREQ bit 14 is a settable request bit where the appendix says enable-only | open · RTL · **red** | `hrm/ch5-audio-interrupt` | **vAmiga and Denise also allow it**; NanoMig's own comment calls it undocumented |
| [NM-HRM-032](findings/NM-HRM-032.md) | BEAMCON0's three sync-polarity bits are decoded nowhere | open · RTL · **red** | `hrm/ch7-beamcon0-ecs-control-bits`, `aa/aa-beamcon0-control-bits` | **split**: only WinUAE implements all three, Denise implements CSYTRUE alone, two model none |
| [NM-HRM-033](findings/NM-HRM-033.md) | LOLDIS does not disable the long-line toggle on its own | open · RTL · **red** | `hrm/ch7-beamcon0-ecs-control-bits` | **all four** use LOLDIS alone; NanoMig also requires VARBEAMEN |
| [NM-HRM-034](findings/NM-HRM-034.md) | VPOSR reports the video standard in bit 12 of an OCS configuration | open · RTL · **red**, contested | `hrm/ch7-beam-position-word-layout` | **2-2**: WinUAE and Denise do the same; and no implementation leaves bits 14-1 unused |
| [NM-HRM-035](findings/NM-HRM-035.md) | any write to CIA-A clears the kickstart overlay, not only a write to the OVL bit | open · RTL · **red** | `hrm/ch7-overlay-source` | **all four** follow the OVL bit's value; three name the only registers that may move it |
| [NM-HRM-037](findings/NM-HRM-037.md) | the right controller port's X pot input does not exist | open · RTL · **red** | `hrm/ch8-pot-counter-start-and-charge` and three siblings | **none of the four** reproduces it — all give the right X counter what the other three get |
| [NM-HRM-038](findings/NM-HRM-038.md) | the pot counters are not held in reset after POTGO | open · RTL · **red** | `hrm/ch8-pot-counter-start-and-charge` | **all four hold, all four put PAL at 8**; they split 2-2 on NTSC, which reads the page's own "seven or eight" as a video-standard split |
| [NM-HRM-039](findings/NM-HRM-039.md) | Table 8-2's wrapped rows compute against 255 on a 256-state counter | open · page · **red** | `hrm/ch8-mouse-counter-wraparound` | **unanimous**: 256 states per axis, and not one uses 255 as a modulus |
| [NM-HRM-040](findings/NM-HRM-040.md) | CIAADDRA's one-to-one direction mask holds for 2 bit positions of 8 | open · RTL · **red**, contested | `hrm/ch8-ciaa-port-direction` | **three-way split**; Copperline reproduces it bit for bit, and two of four gate the printed behaviour on a CIA part number |
| [NM-HRM-041](findings/NM-HRM-041.md) | DSKBYTR is a constant word in three of its five fields | open · RTL · **red** | `hrm/ch8-dskbytr-status-register` and two siblings, `aa/aa-disk-status-bits` | **unanimous against** — all four clear DSKBYT on the read, drive WORDEQUAL from a live comparator and put a real disk byte in DATA; two implement p.248's own two-microsecond window |
| [NM-HRM-042](findings/NM-HRM-042.md) | the disk DMA pointer stores 20 word-address bits where two pages print 32 | open · RTL · **red** | `hrm/ch8-disk-dma-pointer` | **no implementation reaches 32 bits either**: WinUAE and vAmiga keep a 32-bit variable they can never fill, Denise and Copperline mask the stored value to 21 bits or fewer |
| [NM-HRM-043](findings/NM-HRM-043.md) | DSKPTH carries five high bits in both chipsets | open · RTL · **red** | `hrm/ch8-disk-pointer-ecs-and-copper` | **B2, as NM-HRM-003, NM-HRM-022 and NM-HRM-024 — Copperline alone narrows the register by Agnus revision**, here with a test naming DSKPT; WinUAE narrows the Agnus bus instead, which the three earlier entries do not record |
| [NM-HRM-044](findings/NM-HRM-044.md) | the manual permits Copper initialisation of DSKPTH on p.279 and forbids it on p.289 | open · RTL · source observation | `hrm/ch8-disk-pointer-ecs-and-copper` | **unanimous, and it settles nothing**: all four block the Copper at $020 on OCS and allow it under ECS with CDANG — evidence about which reading practitioners took, not about which page is right |
| [NM-HRM-045](findings/NM-HRM-045.md) | DMAON needs a third term that p.278 does not print | open · page · **red** | `hrm/ch8-dmaon-enable-composition` | **unanimous against p.278**: all four require the DSKEN term, and p.248's three-term reading is green in the same run |
| [NM-HRM-046](findings/NM-HRM-046.md) | the drive activity light follows DMA, not the motor | open · RTL · **red** | `hrm/ch8-drive-select-motor-led` | **unanimous against NanoMig**: all four drive their indicator from the motor; two add a select term, two use DMA only to colour it, none to light it |
| [NM-HRM-047](findings/NM-HRM-047.md) | one step-spacing threshold where the page prints two | open · RTL · **red** | `hrm/ch8-drive-step-timing` | **unanimous with NanoMig on the gate**: none of the four gates acceptance on direction or on the printed numbers; three spend 3 ms/18 ms on a read-data settle NanoMig does not model |
| [NM-HRM-048](findings/NM-HRM-048.md) | a documented hardware bug that does not occur here | open · RTL · **red**, inverted | `hrm/ch8-dsklen-dma-arming` | **3–1**: Copperline reproduces the bug with a named constant; WinUAE implements the paragraph's *other* sentence instead and calls it a Paula bug; vAmiga and Denise neither |
| [NM-HRM-049](findings/NM-HRM-049.md) | the MFM encoder is context-free | open · RTL · **red** | `hrm/ch8-mfm-cell-encoding` | **unanimous against** — all four carry the previous data bit across the word or byte boundary; Denise reprints the manual's table as the comment above its encoder |
| [NM-HRM-050](findings/NM-HRM-050.md) | one of the two international cutout keys reaches no code | open · RTL · **red** | `hrm/ch8-keycode-table` | **unanimous against** — all four carry both `$30` and `$2B` as distinct keys; NanoMig's table emits `$2B` for both and `$30` for none |
| [NM-HRM-051](findings/NM-HRM-051.md) | Caps Lock is delivered as an ordinary key | open · RTL · **red** | `hrm/ch8-keyboard-events` | **3–1 against** — WinUAE mirrors the host's lock, Denise models the LED, Copperline owns the toggle in its keyboard MCU; vAmiga has no Caps Lock case and behaves as NanoMig does |
| [NM-HRM-052](findings/NM-HRM-052.md) | p.252 and p.254 disagree about the right Alt key | open · RTL · source observation | `hrm/ch8-keycode-table` | **unanimous for p.252, and it settles nothing**: all four name `$65` for the right Alt, which is evidence about which page implementers read, not about which page is right |
| [NM-HRM-053](findings/NM-HRM-053.md) | Appendix B's `*` mark has no ECS qualifier, and nothing honours it under ECS | open · page · **red** | `hrm/ch8-appb-copper-access-marks` | **unanimous with NanoMig against the page**: all four lift the block when ECS and CDANG are both set, on the same formula, and Copperline ships a test table asserting it |
| [NM-HRM-054](findings/NM-HRM-054.md) | the parallel port has a direction register and no data register | open · RTL · **red** | `hrm/ch8-parallel-port-direction` | **unanimous against**: all four store CIA-A's port B data register and read it back through the direction mask; Copperline models the same joystick-adapter wiring and still implements it |
| [NM-HRM-055](findings/NM-HRM-055.md) | SERDAT written with all zeros still transmits | open · RTL · **red** | `hrm/ch8-serdat-transmit`, `hrm/ch8-serial-interface` | **3-1, with a split inside one of them**: vAmiga and Denise implement p.259's second sentence in a single added token, and WinUAE implements it in its ordinary path (`if (!w) return;`) but not in its cycle-accurate one; Copperline writes an explicit `word == 0` branch to transmit a full frame anyway |
| [NM-HRM-056](findings/NM-HRM-056.md) | SERDATR's TBE bit cannot be observed clear from outside | open · RTL · **red**, contested | `hrm/ch8-serdatr-read-word` | **2–2**: Denise and Copperline set and clear the buffer flag inside one event as NanoMig does; vAmiga derives TBE from a latch with its own drain time and WinUAE holds the bit clear by an explicit timer |
| [NM-HRM-057](findings/NM-HRM-057.md) | SERDATR bit 11 is a registered copy of RXD, not a direct read | open · RTL · **red**, contested | `hrm/ch8-serdatr-read-word`, `hrm/ch8-serial-interface` | **3–1 for the page** — vAmiga and Denise read the pin immediately, WinUAE has no pin, and Copperline's two synchroniser stages are still one short of NanoMig's three |
| [NM-HRM-058](findings/NM-HRM-058.md) | the received-character window is longer than p.256 prints | open · page · **red** | `hrm/ch8-serdatr-read-word`, `hrm/ch8-serial-interface` | **three of four land on 10 and 11 bit times, exactly as NanoMig does**; WinUAE has no receiver and its only 10/11 constant is on the transmit side |
| [NM-HRM-059](findings/NM-HRM-059.md) | ADKCONR's bit 15 always reads 0, and so does everyone's | open · page · **red** | `hrm/ch8-adkcon-and-interrupt-relations` | **unanimous with NanoMig**: all four read bit 15 as 0, but only WinUAE masks on the read, so a crafted savestate would expose the bit in the other three |
| [NM-HRM-060](findings/NM-HRM-060.md) | the instruction cache monitors writes, and p.13 says it does not | open · RTL · **red** | `hrm/ch1-cpu-and-instruction-cache` | **3–0 against, one silent**: WinUAE, vAmiga and Copperline all leave the instruction cache untouched by writes; Denise models a 68000 and has no cache to disagree with |
| [NM-HRM-061](findings/NM-HRM-061.md) | two reds that are not defects: a whole-machine sentence graded at a board's config register | open · classification · **red** | `hrm/ch1-cpu-options` | **not applicable, and that is the evidence**: no emulator has a `system_cpu` field, because none of them is a board that must be told which CPU to instantiate |
| [NM-HRM-062](findings/NM-HRM-062.md) | Appendix F prints the two CIA address patterns interchanged | open · page · none | none yet · raised at classification | **unanimous against the print**: WinUAE, vAmiga, Denise and Copperline all decode the way the page's own prose says, as does NanoMig |
| [NM-HRM-063](findings/NM-HRM-063.md) | FLAG is negative edge-sensitive; the RTL ORs a level | open · RTL · **red** | `hrm/appF-ciab-port-assignments` | **unanimous, and all four implement the page**: WinUAE, vAmiga and Denise raise the bit from a transition and have no level input at all; Copperline keeps the pin state and latches only on `flag_pin_high && !high` |
| [NM-HRM-064](findings/NM-HRM-064.md) | the board advertises that it can be shut up, and register 4C has no handler | open · RTL · **red** | `hrm/appK-autoconfig-base-address` | **3–1**: WinUAE, Denise and Copperline all handle a write to 4C — Denise gates it on the very capability bit p.433 defines — while vAmiga has no `0x4C` case, its boards being a fixed internal set it plugs and unplugs itself |
| [NM-HRM-065](findings/NM-HRM-065.md) | register 48 does not configure a Zorro III board, and the implementations side with NanoMig | open · page · **red** | `hrm/appK-autoconfig-base-address` | **two of four do exactly this**: WinUAE and Copperline both configure a Zorro III board on register 44 inside the Zorro II block; vAmiga and Denise have no Zorro III board type and configure on 48 because every board they own is a Zorro II one |

### Withdrawn — kept on purpose

A withdrawn finding is the record of a claim that was raised, measured and then
**disproved**. Deleting it would mean the next reader raises it again. They sit
in their own table so the status column above never has to be read twice, and
they keep their numbers: a key is issued once and never reused.

**NM-HRM-029 is the one worth the minute.** Its assertion tested *"dmasen is
true only when LENFIN=1"* by looking for a case where both held and failing
when none existed — but absence satisfies "only when". A one-way implication
had been tested as a biconditional, which is a trap somebody will walk into
again.

| Key | Subject | Status | Package | Third witness |
|---|---|---|---|---|
| [NM-HRM-001](findings/NM-HRM-001.md) | PAL field terminates at 313 lines, not 312 | **withdrawn** | `hrm/ch3-field-geometry` | **all four** run a 313-line PAL field; Table 7-5 sanctions 313 |
| [NM-HRM-009](findings/NM-HRM-009.md) | CLXDAT bit 15 reads as 1, not 0 | **withdrawn** | `hrm/ch3-collision-registers` | **all four** set bit 15 on every CLXDAT read |
| [NM-HRM-012](findings/NM-HRM-012.md) | SPRxCTL bit 0 (SH0) honoured at every resolution | **withdrawn** | `hrm/ch3-sprite-registers` | three of four take SH0 at every resolution, as NanoMig does |
| [NM-HRM-013](findings/NM-HRM-013.md) | SPRxCTL bits 3 and 4 are read nowhere | **withdrawn** | `hrm/ch3-sprite-registers` | bit 4 is real from ECS on, bit 3 only on AGA -- the split this was withdrawn into |
| [NM-HRM-018](findings/NM-HRM-018.md) | ECS BPLCON0 bit 4 (SPRHWRM) has no effect | **withdrawn** | `hrm/ch3-bplcon0-display-modes` | neither Denise nor Copperline gives bit 4 an effect on ECS |
| [NM-HRM-029](findings/NM-HRM-029.md) | Figure 5-8's `dmasen` note does not hold outside its own exception | **withdrawn** | `hrm/ch5-audio-channel-state` | not comparable — `dmasen` appears at no pin |
| [NM-HRM-036](findings/NM-HRM-036.md) | chip memory banks 1-3 stay reachable while the overlay is active | **withdrawn** | `hrm/ch7-reset-overlay` | **3-1**: vAmiga, Denise and Copperline replace only the ROM's 512K, as NanoMig does; p.223 itself allows part of the RAM to respond |


Chapter 1 was read and accepted on 17.09.2026 and contributes NM-HRM-060
and NM-HRM-061. Appendix F, read the same day, contributes NM-HRM-062
and NM-HRM-063. Appendix K contributes NM-HRM-064 and NM-HRM-065, both from
the AUTOCONFIG responder — the one part of the Zorro bus the Minimig family
implements. Both pages were corrected after their first version: the responder
is inherited from MiSTer, not NanoMig’s own, and NM-HRM-065’s behaviour is
inherited with it.
Chapter 8's first slice — the Controller Port Interface — contributes
NM-HRM-037 to NM-HRM-040; its floppy slice NM-HRM-041 to NM-HRM-049 and its
keyboard slice NM-HRM-050 to NM-HRM-052 and its serial slice NM-HRM-053 to
NM-HRM-059.

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

## The AA register

**These are findings against a different document and they are counted
separately.** The oracle is Commodore's *Functional Specification for the
Advanced Amiga Chip Set (AA)*, 06/07/91, PRELIMINARY, not the Hardware
Reference Manual, and the tests that hold them up live in `aa/` rather than
`hrm/`. Mixing the two counts would make both harder to read.

**Three of the five are filed against the printed page, and all three are held
up by GREEN assertions.** That is the shape a document defect takes here: the
RTL does something sensible, the test says so, and what is in question is
whether the document can be cited for that register at all. There is no red to
point at, so each of those findings names its own inverse control instead — the
assertion that would have gone red had the RTL followed the other half of the
document.

**The four were read for those three as well, and they are unanimous in all
three.** A document's self-contradiction is not something an implementation can
settle, but an address, a Coprocessor restriction and a shift field are each
something every implementation embodies an answer to. Three of the four decode
CLXCON2 and all three put it at `$10E`, with BPLCON4 at the `$10C` the
alphabetical entry claims; all four refuse a Coprocessor MOVE to `$078` with
CDANG clear, and all four decide it by comparing the address against `$080`
rather than by consulting a table of marks; and all four take `bltcon0[15:12]`
as one field, two of them naming it ASH. **What that settles is narrow and
worth stating: nothing downstream was misled by any of the three defects.**

**The other two are filed against the RTL, and for those the four were read.**
Both readings came out sharper than the column has room for. On NM-AA-004 every
implementation that models the identifier at all gives the RTL's `$23` and none
gives the document's `$22`, and two of them separately doubt the neighbouring
row of the same list. On NM-AA-005 the four disagree with each other about the
long line and agree on one thing NanoMig does not do: wherever the toggle runs
the line grows, and where the line cannot grow the toggle is stopped. **Neither
reading moves a verdict.** The oracle is the printed page in both cases, and an
implementation — however many of them agree — is evidence for the reader who
decides about the RTL, never a second oracle.

| Key | Subject | Status | Package | Verdict of the four |
|---|---|---|---|---|
| [NM-AA-001](findings/NM-AA-001.md) | the AA list gives CLXCON2 two addresses, and $10C belongs to BPLCON4 | open · page · green with an inverse control | `aa/aa-clxcon2-address` | **unanimous for $10E**: WinUAE (`custom.cpp:7412`), vAmiga (`Memory.cpp:2894`) and Copperline (`custom_regs.rs:248`) decode it there with BPLCON4 at $10C; Denise implements neither and still names both at those addresses |
| [NM-AA-002](findings/NM-AA-002.md) | SPRHDAT is the one register in the Coprocessor-restricted range left unmarked | open · page · green with an inverse control | `aa/aa-copper-register-map` | **unanimous, by range not by mark**: all four block a Coprocessor MOVE to $078 with CDANG clear by comparing the address against $080 — WinUAE (`custom.cpp:2809`), vAmiga (`Copper.cpp:394`), Denise (`copper.cpp:203`), Copperline (`dma_slots.rs:697`). None models the document's marks |
| [NM-AA-003](findings/NM-AA-003.md) | the bit table spells BLTCON0 bit 12 ASA0 in one column and ASH0 in the other | open · page · green with an inverse control | `aa/aa-blitter-function-and-shift` | **unanimous, one four-bit field**: WinUAE (`blitfunc.cpp:12`), vAmiga (`Blitter.h:404`), Denise (`blitterCycle.cpp:10`) and Copperline (`blitter.rs:1014`) all take `bltcon0[15:12]` in one expression; two name it ASH and none spells ASA0 |
| [NM-AA-004](findings/NM-AA-004.md) | VPOSR reports chip identifier $23 where the AA list gives an Alice $22 | open · RTL · **red** | `aa/aa-beam-position-words` | **3-0 against the document**: WinUAE (`custom.cpp:2399`), vAmiga (`Agnus.cpp:272`) and Copperline (`agnus.rs:98`) all return **$23** for AGA, this RTL's value; Denise casts no vote, its AGA term lands in bits 1:0 |
| [NM-AA-005](findings/NM-AA-005.md) | the long-line toggle runs, but no line is a colour clock longer | open · RTL · **red** | `aa/aa-programmable-sync-windows` | **none of the four does what this core does**: WinUAE and Denise lengthen the alternate line under a programmed HTOTAL, vAmiga lengthens it but does not implement HTOTAL, Copperline stops the toggle in that mode — nowhere does the bit alternate over equal-length lines |

**33 AA packages carry 184 graded assertions. 5 are red, in 4 packages,
answering 4 keys** -- three counts that are easy to collapse into one and are
not the same number. `aa-disk-status-bits` goes red on two of its own
assertions, which is why the assertion count is one above the package count.
Sixteen packages cover the RTL modules that decode an address the AA register
list prints; four are from slice 2, five from slice 4, one from slice 3,
one from slice 5, five from slice 1 and one, the scroll range, from slice 6.
All seven slices are classified to their last record.

Two of the four keys are findings of their own: NM-AA-004, VPOSR's chip
identifier, and NM-AA-005, the long line that is no longer than a short one.
The other two are corroborations, recorded on the pages of findings already
raised against the Hardware Reference Manual -- `aa-beamcon0-control-bits`
reaches NM-HRM-032, and `aa-disk-status-bits` reaches NM-HRM-041 on **both** of
its reds, which that page states in its own words. A second document saying the
same thing is not a second key.

These five keys are what the AA suite has turned up so far. A package that
finds nothing has still done its work: it says the document and the silicon
agree on those rows.

## The analyses that produced groups of findings

Where the five oldest findings came from and whether they hold, why
NM-HRM-001's entry stays after withdrawal, how appendix C's five ECS blitter
changes are implemented five different ways, and what the emulators say about
the three Copper findings:
**[`finding-analyses.md`](finding-analyses.md)**.

## How a finding is meant to end

A red test is closed by a decision about the RTL, never by relaxing the
assertion. If a deviation turns out to be intentional, the assertion stays and
the reason is written down beside it — a test whose expectation was quietly moved
to match the implementation stops being evidence about anything.
