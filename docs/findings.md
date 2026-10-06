# Findings

**48 of the 70 HRM findings ask you to decide whether the RTL should change.** Their titles end in **[RTL review]**, and [`rtl-review.md`](rtl-review.md) lists them, with the AA register's, by the module they are about. **The table is below the explanation — jump to [The register](#the-register).** The AA specification has a register of its own: [The AA register](#the-aa-register).

> **One page per finding lives in [`findings/`](findings/).** Each starts
> with the same header table and says what the printed page says, what NanoMig
> does and what the other four do. Most pages open with those three sections in
> that order; the others put what their subject needs between them, such as the
> measurement or why the finding is filed against the page. Then come, where
> they apply, a suggested change, what holds the finding up, and what is left
> for specialists. What other Commodore documents say sits either after the other
> four or at the end. A withdrawn page keeps only the reasoning that withdrew
> it. Every source is quoted and permalinked at a pinned revision.
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

> **Deciding what to do about these:** start at [`rtl-review.md`](rtl-review.md),
> the findings that ask for an RTL review by module. For six of them, chapter 2's
> Copper and chapter 6's blitter findings, [`evidence/rtl-decisions.md`](evidence/rtl-decisions.md)
> sets the question beside WinUAE's and vAmiga's implementation, with line
> citations verified against the commits `evidence/third-witness.md` records.

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
> [`record-2026-09-11-validation.md`](https://github.com/darius600/hrm-testpipeline/blob/main/docs/2026-09-11_ch3-validation.md).

## The finding keys

Findings are numbered **`NM-HRM-nnn`** -- `NM` for the implementation under test,
`HRM` for the *Amiga Hardware Reference Manual* that judges it. The key names the
**pair that disagrees**, and it does so deliberately, because it must not
prejudge which side is wrong: on several of the findings below, independent
emulators do what NanoMig does and the printed sentence is the outlier.

A number is issued once. It is never reused, never reordered, and a withdrawn
finding keeps both its number and its entry -- the reasoning that withdrew it is
the result, not a deletion. NM-HRM-076 and NM-HRM-081 to NM-HRM-083 are not in
use.

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
published — the claim ids beside it (the stable id of each cited statement of
the manual) are the public record of the same work.

## The register

Every open finding, with the package that measures it and what the four
independent implementations do; withdrawn findings ask nothing and are kept apart, with their own list, in
[`findings/withdrawn/`](findings/withdrawn/).
**The emulators are not an oracle and never decide a finding** -- they are read
only for the report to the hardware developers
([`evidence/third-witness-ch3-ch6.md`](evidence/third-witness-ch3-ch6.md), whose section letters
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
| `open · RTL · source observation` | nothing yet — read from the source and not measured, or measured only inside the module, where no pin shows it |
| `open · page · **red**` | **no RTL change**; the printed page is the likely defect |
| `open · page · none` | the same, with no test built against it yet |
| `open · page · green with an inverse control` | **no RTL change**; the printed page is the likely defect, and the assertion that holds NanoMig's reading is green |
| `open · classification · **red**` | **no RTL change**; how the claim was classified is what is contested |

### If you have one evening

**If you have one evening: 48 of these 70 ask you to decide something.** The
rest are answered, or observations kept for context. The table below
is sorted by key, not by how much it wants from you — so the first row is not
the place to start. Start at one of these, whichever is nearest your own
interest:

| start here | because | what it takes |
|---|---|---|
| [NM-HRM-003](findings/NM-HRM-003.md) | OCS Copper pointer width — a red, and three of the four implementations already do what the manual says | a scope decision, read together with NM-HRM-022 and NM-HRM-024; one central mask would close all three |
| [NM-HRM-004](findings/NM-HRM-004.md) | the manual contradicts itself on the ECS danger bit, and NanoMig, all four implementations and Commodore's AA specification take Appendix B's side | reading two appendices, B pp. 289-290 and C p. 310; no RTL change |
| [NM-HRM-064](findings/NM-HRM-064.md) | AUTOCONFIG register 4C — six red assertions, and a one-line fix: stop advertising the shut-up the whole Minimig family never implemented | one `case` entry, then one package run |

**[NM-HRM-002](findings/NM-HRM-002.md) sorts first and asks for no RTL change**:
the manual assigns the stereo sides twice and the two disagree, and NanoMig,
three of the four implementations, the 8364R7 silicon and Commodore's A500,
A2000 and B2000 schematics all side with p. 326 against p. 137.


| Key | Subject | Status | Package | Verdict of the four |
|---|---|---|---|---|
| [NM-HRM-002](findings/NM-HRM-002.md) | audio channels 0 and 3 play on the left, where p. 137 says right and p. 326 says left | open · page · **red** | `hrm/ch5-audio-stereo-routing` | **3–1 with NanoMig against p. 137**: WinUAE, Denise and Copperline put channels 0 and 3 on the left, as p. 326 prints, and vAmiga on the right, as p. 137 prints; the 8364R7 silicon feeds them to its left pad, and Commodore's A500, A2000 and B2000 schematics take Paula's left pin to the left jack |
| [NM-HRM-003](findings/NM-HRM-003.md) | OCS Copper pointer uses five high bits, not three | open · RTL · **red** | `hrm/ch2-copper-control`, `hrm/ch2-copper-locreg-width` | **Copperline narrows the REGISTER to 19 bits on OCS**, matching the manual; **WinUAE and vAmiga narrow the BUS** on OCS (`memory.cpp:2619-2623`, `Memory.cpp:1623`), which has the same effect on a fetched address; Denise neither |
| [NM-HRM-004](findings/NM-HRM-004.md) | p. 310 gives the ECS Copper $03E–$07E with CDANG clear, Appendix B marks that range closed, and every implementation keeps it closed | open · page · **red** | `hrm/ch2-copper-move-range`, `hrm/ch2-copper-move-sweep` | **unanimous with NanoMig and with Appendix B**: WinUAE (`custom.cpp:2809`), vAmiga (`Copper.cpp:395`), Denise (`copper.cpp:342`) and Copperline (`dma_slots.rs:697`) all block a Copper MOVE below `$080` while CDANG is clear, whatever the chipset; none cites Appendix C |
| [NM-HRM-005](findings/NM-HRM-005.md) | WAIT and SKIP take longer than the manual states | open · RTL · **red** | `hrm/ch2-copper-cycle-times`, `hrm/ch2-copper-cycles` | **all four give NanoMig's 4 / 8 / 8**, against the manual's 4 / 6 / 2: WinUAE, vAmiga, Denise and Copperline, read from their sources; Copperline names Minimig and vAmiga as its model for these cycles |
| [NM-HRM-006](findings/NM-HRM-006.md) | DDFSTRT bit H3 is respected in lores, where p.91 says hires only | open · page · **red** | `hrm/ch3-ddf-fetch-window` | **unanimous with NanoMig**: all four respect H3 at every resolution; what they gate on the chipset is register bit 1 |
| [NM-HRM-007](findings/NM-HRM-007.md) | hires fetch cannot reach the manual's 49 words | open · RTL · **red** | `hrm/ch3-ddf-fetch-window`, `hrm/ch3-bitplane-memory-layout` | **B4** for the first two; Copperline asserts an even hires word count in its own test |
| [NM-HRM-008](findings/NM-HRM-008.md) | playfield-1 collision depends on playfield 2 in single-playfield mode, where pp. 214 and 273 name no mode | open · page · **red** | `hrm/ch3-collision-registers`, `hrm/ch7-collision-playfield-mode` | **unanimous with NanoMig, against pp. 214 and 273**: WinUAE, vAmiga, Denise and Copperline all register a playfield-1 collision in single-playfield mode only where playfield 2's enabled planes match too; WinUAE's changelog calls p. 214's sentence wrong, vAmiga's comment calls the gate a hardware oddity, and Copperline's expression is NanoMig's term for term; outside the emulators, the 8373R4 dissection gates the playfield-1 match on the even planes when DPF is clear, as NanoMig does |
| [NM-HRM-010](findings/NM-HRM-010.md) | the NTSC line is 227 colour clocks every time, where the manual alternates 228 and 227 | open · RTL · **red** | `hrm/ch3-beam-counters-and-blanking` · `COLOUR-CLOCKS-PER-LINE-227POINT5 (NTSC)` and its bundled form; `integration/ch2-interlace-fields` · `P3` on fx68k and TG68K | **unanimous against NanoMig**: WinUAE, vAmiga, Denise and Copperline all make every other NTSC line 228 colour clocks; NanoMig toggles the long-line bit and keeps every line at 227 |
| [NM-HRM-011](findings/NM-HRM-011.md) | Table 3-13 blanks 29 PAL lines, p. 219 ends the blank at line 25, and no implementation reaches 29 | open · page · **red** | `hrm/ch3-beam-counters-and-blanking` · `VBLANK-DURATION-PAL-DISAGREES-WITH-MANUAL`; `hrm/ch7-vertb-vertical-blank-timing` · `PAL-VBLANK-DURATION-25-LINES`, `NTSC-VBLANK-DURATION-20-LINES` | **unanimous against Table 3-13's PAL row**: WinUAE and vAmiga end the PAL blank after line 25, as NanoMig does, Copperline after 24 and Denise after 23; none reaches the $1D (29) the table prints, and WinUAE matches NanoMig in NTSC as well |
| [NM-HRM-014](findings/NM-HRM-014.md) | the manual applies its own ECS pointer note inconsistently | open · page · none | `hrm/ch3-register-map-agnus` | **unanimous with NanoMig and with chapter 1**: WinUAE (`memory.cpp:2619-2623`), vAmiga (`Agnus.cpp:190-194`), Denise and Copperline give the bitplane and sprite pointers the reach of every other DMA pointer, past 512K under ECS; none holds them to the three bits Appendices A to C print, and Copperline asserts it for `BPLxPT` and `SPRxPT` in a test |
| [NM-HRM-016](findings/NM-HRM-016.md) | SuperHires sprite colour encoding not implemented | open · RTL · **red**, contested | `hrm/ch3-shres-sprite-colour-encoding` | **one of four** reads `$10`-`$1F` as the page describes (WinUAE, ECS Denise only); vAmiga pair-encodes only the lower sixteen; Denise and Copperline do not pair-encode; outside the emulators, the 8373R4 dissection indexes the upper sixteen by two sprite pixels and shows two bits per component, as WinUAE does |
| [NM-HRM-017](findings/NM-HRM-017.md) | SuperHires bitplane colour encoding not implemented | open · RTL · **red**, contested | `hrm/ch3-shres-bitplane-colour-encoding` · all nine reds: `SHRES-REINTERPRETS-SAME-REGISTERS`, `SHRES-BITPLANE-TABLE-COLOR-01` to `-03`, `-05`, `-0A`, `-0F`, `SHRES-USES-SIXTEEN-LOWER-REGISTERS`, `SHRES-TWO-BITS-RESOLUTION` | **2-2**: WinUAE and vAmiga pair SuperHires pixels into the sixteen lower registers and show two bits per component, ECS Denise only; Denise and Copperline show each pixel's own register in full; outside the emulators, the 8373R4 dissection pairs SuperHires pixels into the lower sixteen and shows two bits per component, as WinUAE and vAmiga do |
| [NM-HRM-019](findings/NM-HRM-019.md) | SPRxCTL bit 4 (SHSH1) is not implemented | open · RTL · **red** | `hrm/ch3-sprite-registers` | **all four implement it**, and on ECS all four confine its effect on position to SuperHires; outside the emulators, the 8373R4 dissection latches bit 4 for sprites 0 to 3 and applies it in the sprite's data path, not the position compare |
| [NM-HRM-020](findings/NM-HRM-020.md) | BLTSIZV is not gated on ECS, while its siblings are | open · RTL · **red** | `hrm/ch6-blitter-ecs` | **A2** — **all four** gate it; NanoMig alone does not |
| [NM-HRM-021](findings/NM-HRM-021.md) | DOFF (BLTCON1 bit 7) has no observable effect | open · RTL · **red** | `hrm/ch6-blitter-ecs` | **C1**; three of four implement it, each gated on ECS Agnus (WinUAE, Denise, Copperline); vAmiga does not |
| [NM-HRM-022](findings/NM-HRM-022.md) | BLTxPTH carries five high bits in both chipsets | open · RTL · **red** | `hrm/ch6-blitter-ecs` · `BLTXPTH-WIDTH-REGARDLESS-OF-ECS`, `BLTXPTH-3-BIT-OCS-5-BIT-ECS-RANGE` | **B2** — three of the four narrow by chipset: Copperline narrows every DMA pointer by Agnus revision; WinUAE (`memory.cpp:2619-2623`) and vAmiga (`Agnus.cpp:190-194`) narrow the address the DMA drives rather than the register; Denise masks to installed chip RAM only |
| [NM-HRM-023](findings/NM-HRM-023.md) | the ATTACH bit is honoured in the even sprite, not only the odd | open · RTL · **red** | `hrm/ch4-attached-sprites` | **Denise applies the manual's rule exactly**: `(nr & 1) && (value & 0x80)`; outside the emulators, the 8373R4 dissection stores bit 7 for the odd sprites only |
| [NM-HRM-024](findings/NM-HRM-024.md) | the audio DMA pointer takes five high bits in both chipsets | open · RTL · **red** | `hrm/ch5-audio-location-pointer` | **B2**, as NM-HRM-022 — three of the four narrow by chipset: Copperline the REGISTER; WinUAE (`memory.cpp:2619-2623`) and vAmiga (`Agnus.cpp:190-194`) the address the DMA drives; Denise masks to installed chip RAM only |
| [NM-HRM-025](findings/NM-HRM-025.md) | volume 64 and volume 63 are the same gain on the multiply path | open · RTL · **red** | `hrm/ch5-audio-volume-scale`, `hrm/ch5-audio-register-map` | **all four** clamp at 64 and keep it distinct; two say why in their own comments |
| [NM-HRM-026](findings/NM-HRM-026.md) | the audio state codes are not the ones Figure 5-8 prints | open · RTL · source observation | `hrm/ch5-audio-channel-state` | **all four** use the figure's numbering; Copperline names the five constants with it |
| [NM-HRM-028](findings/NM-HRM-028.md) | ADKCON's audio attach bits are stored and reach nothing | open · RTL · **red** | `hrm/ch5-audio-dmacon-adkcon` · `ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS`; `integration/ch5-audio-attach-modulation` · 15 lines on fx68k and TG68K: `P4 (c)`, `P6 (a)` for six attach bits, `P8` in five sessions, the `P1/P8` joint line, `P1 (a)`, `P4 (a)` | **all four route a modulator's words into the next channel, and all four leave the package's modulators silent**; they split 2-2 on p. 150's rule that either attach bit mutes the modulator: WinUAE and Copperline mute it, vAmiga and Denise only stop reloading its output buffer under a volume attach, so a period-only modulator still plays; NanoMig's own first line says it implements neither |
| [NM-HRM-030](findings/NM-HRM-030.md) | Paula does not decode DMACONR, though the appendix gives it part ownership | open · page · **red** | `hrm/ch5-audio-dmacon-adkcon` | **three of four with NanoMig, the fourth cannot answer**: vAmiga, Denise and Copperline model Paula apart from Agnus and read `$002` from Agnus alone (`Memory.cpp:2411-2412`, `agnus/register.cpp:64-65`, `custom_regs.rs:12-27`); WinUAE has one `DMACONR()` for the whole chip set (`custom.cpp:7048`); the dissected 8364R7 gives Paula's DMACON copy no read path |
| [NM-HRM-031](findings/NM-HRM-031.md) | INTREQ bit 14 is a settable request bit where the appendix says enable-only | open · page · green with an inverse control | `hrm/ch5-audio-interrupt`, `hrm/ch7-interrupt-priority-encoder` · `P3-INTREQ-FULLY-WRITABLE` | **unanimous with NanoMig and with Figure 7-4**: WinUAE, vAmiga, Denise and Copperline all latch a written bit 14 in INTREQ, read it back through INTREQR and serve it at level 6; none implements p. 280's parenthesis |
| [NM-HRM-032](findings/NM-HRM-032.md) | BEAMCON0's three sync-polarity bits are decoded nowhere | open · RTL · **red** | `hrm/ch7-beamcon0-ecs-control-bits`, `aa/aa-beamcon0-control-bits` | **split**: only WinUAE implements all three, Denise implements CSYTRUE alone, two model none |
| [NM-HRM-033](findings/NM-HRM-033.md) | LOLDIS does not disable the long-line toggle on its own | open · RTL · **red** | `hrm/ch7-beamcon0-ecs-control-bits` | **all four** use LOLDIS alone; NanoMig also requires VARBEAMEN |
| [NM-HRM-035](findings/NM-HRM-035.md) | any write to CIA-A clears the kickstart overlay, not only a write to the OVL bit | open · RTL · **red** | `hrm/ch7-overlay-source` | **unanimous against NanoMig on every model the manual covers**: vAmiga, Denise and Copperline follow the OVL bit's value on every machine they model, vAmiga's and Copperline's A1200 included, and WinUAE does on every model without Gayle, the AGA A4000 included; only WinUAE's Gayle-based A600 and A1200, its CD32 and its CDTV-CR release the overlay on any CIA-A write, as NanoMig does in all four of its modes |
| [NM-HRM-037](findings/NM-HRM-037.md) | the right controller port's X pot input does not exist | open · RTL · **red** | `hrm/ch8-pot-counter-start-and-charge` and three siblings | **none of the four** reproduces it — all give the right X counter what the other three get |
| [NM-HRM-038](findings/NM-HRM-038.md) | after POTGO START, the pot counters are not held in reset and the input lines are not dumped | open · RTL · **red**, contested | `hrm/ch8-pot-counter-start-and-charge`; `integration/ch8-controller-ports` · `C0-Fc` to `C4-Fc` and their aggregate `P1-START` in all four PAL/NTSC × ECS/OCS configurations | **2–2 on the first counter read, 3–1 on the POTGOR discharge, unanimous on the final count**: Denise and Copperline hold the counters at zero through the reset interval; WinUAE and vAmiga count through it and restart from zero at its end, so they read non-zero after one line as NanoMig does; WinUAE, vAmiga and Denise discharge an input line through the interval, so POTGOR reads 0 there on a line nothing pulls up, while Copperline reads it as before START as NanoMig does (vAmiga and Denise also read 1 with a joystick or mouse attached, whose models pull the line up); all four keep the interval out of the final count, which NanoMig includes; all four put PAL at 8, and they split 2-2 on NTSC |
| [NM-HRM-039](findings/NM-HRM-039.md) | Table 8-2's wrapped rows compute against 255 on a 256-state counter | open · page · **red** | `hrm/ch8-mouse-counter-wraparound` | **unanimous**: 256 states per axis, and not one uses 255 as a modulus |
| [NM-HRM-040](findings/NM-HRM-040.md) | CIAADDRA's one-to-one direction mask holds for 2 bit positions of 8 | open · RTL · **red**, contested | `hrm/ch8-ciaa-port-direction` | **four-way split** where p.341 predicts the value, an output driven 0 on a released line: Denise honours all eight, Copperline reproduces NanoMig's two bit for bit, vAmiga honours all eight until a mouse's pull-up overrides a fire bit, WinUAE follows the write on four; the chip-variant switches, vAmiga's modelled on WinUAE's, restore all eight only in vAmiga and by their own comments separate only a line held from outside, so the part number does not decide the red. Over a line held low, a case no page decides and the package does not present, Denise and vAmiga return the written 1 on PA2–PA5, WinUAE and Copperline the line. The 8520R4 silicon drives PA0 low for a written 0 and reads the pin |
| [NM-HRM-041](findings/NM-HRM-041.md) | DSKBYTR is a constant word in three of its five fields | open · RTL · **red** | `hrm/ch8-dskbytr-status-register` and two siblings, `aa/aa-disk-status-bits` | **unanimous against** — all four clear DSKBYT on the read, drive WORDEQUAL from a live comparator and put a real disk byte in DATA; two implement p.248's own two-microsecond window |
| [NM-HRM-042](findings/NM-HRM-042.md) | the disk DMA pointer stores 20 word-address bits where two pages print 32 | open · RTL · **red** | `hrm/ch8-disk-dma-pointer` | **no implementation reaches 32 bits either**, outside one emulator-only option: WinUAE and vAmiga keep a 32-bit variable and mask it on the DMA path (WinUAE's UAE-only Mega Chip RAM leaves it unmasked); Denise and Copperline mask the stored value to 21 bits or fewer |
| [NM-HRM-043](findings/NM-HRM-043.md) | DSKPTH carries five high bits in both chipsets | open · RTL · **red** | `hrm/ch8-disk-pointer-ecs-and-copper` | **B2, as NM-HRM-003, NM-HRM-022 and NM-HRM-024 — Copperline alone narrows the register by Agnus revision**, here with a test naming DSKPT; WinUAE and vAmiga narrow the address Agnus drives instead, as the three earlier entries also record |
| [NM-HRM-044](findings/NM-HRM-044.md) | the manual permits Copper initialisation of DSKPTH on p.279 and forbids it on p.289 | open · page · **red** | `hrm/ch8-disk-pointer-ecs-and-copper` · `P1`; `hrm/ch2-copper-move-sweep` · `APPBSTAR $10 DSKPTH ecs=0`, green | **unanimous with NanoMig**: all four block the Copper at $020 on OCS, as p.289 marks it, and allow it under ECS with CDANG set, as p.310 allows; none follows p.279 on OCS — evidence about which reading practitioners took, not about which page is right |
| [NM-HRM-045](findings/NM-HRM-045.md) | DMAON needs a third term that p.278 does not print | open · page · **red** | `hrm/ch8-dmaon-enable-composition` | **unanimous against p.278**: all four require the DSKEN term, and p.248's three-term reading is green in the same run |
| [NM-HRM-046](findings/NM-HRM-046.md) | the drive activity light follows DMA, not the motor | open · RTL · **red** | `hrm/ch8-drive-select-motor-led`; `integration/appF-cia-system` · `P1-LP/P1-O2`, recorded, not graded, on fx68k and TG68K | **unanimous against NanoMig**: all four drive their indicator from the motor; two add a select term, two use DMA only to colour it, none to light it |
| [NM-HRM-047](findings/NM-HRM-047.md) | one step-spacing threshold where the page prints two | open · RTL · **red** | `hrm/ch8-drive-step-timing` | **unanimous with NanoMig on the gate**: none of the four uses the printed pair for it; three carry a seek or settle delay elsewhere (Copperline 3 ms and 18 ms by direction, vAmiga 3 ms plus 9 ms, Denise up to 18 ms and off by default) that NanoMig does not model |
| [NM-HRM-048](findings/NM-HRM-048.md) | a documented hardware bug that does not occur here | open · RTL · **red**, inverted | `hrm/ch8-dsklen-dma-arming` | **3–1**: Copperline reproduces the bug with a named constant, in images that keep raw MFM cells; WinUAE and Denise implement the paragraph's *other* sentence instead, and WinUAE calls it a Paula bug; vAmiga neither |
| [NM-HRM-049](findings/NM-HRM-049.md) | the MFM encoder is context-free | open · RTL · **red** | `hrm/ch8-mfm-cell-encoding` | **unanimous against** — all four carry the previous data bit across the word or byte boundary; Denise reprints the manual's table as the comment above its encoder |
| [NM-HRM-050](findings/NM-HRM-050.md) | one of the two international cutout keys reaches no code | open · RTL · **red** | `hrm/ch8-keycode-table` | **unanimous against** — all four carry both `$30` and `$2B` as distinct keys; NanoMig's table emits `$2B` for both and `$30` for none |
| [NM-HRM-051](findings/NM-HRM-051.md) | Caps Lock is delivered as an ordinary key | open · RTL · **red** | `hrm/ch8-keyboard-events` | **3–1 against** — WinUAE mirrors the host's lock, Denise models the LED, Copperline owns the toggle in its keyboard MCU; vAmiga has no Caps Lock case and behaves as NanoMig does |
| [NM-HRM-052](findings/NM-HRM-052.md) | p.252 and p.254 disagree about the right Alt key | open · page · none | `hrm/ch8-keycode-table` | **unanimous with NanoMig and with p.252 and p.363**: all four name `$65` for the right Alt and none takes the `64` both p.254 drawings print — evidence about which page implementers read, not about which page is right |
| [NM-HRM-054](findings/NM-HRM-054.md) | the parallel port has a direction register and no data register | open · RTL · **red** | `hrm/ch8-parallel-port-direction` · `PROP-1`; `hrm/appDE-parallel-port-ciaa` · `PORTB-DIRECTION`; `integration/appF-cia-system` · `P1-PRE1.4/O1` on CIA-A, recorded, not graded, on fx68k and TG68K | **unanimous against**: all four store CIA-A's port B data register and read it back through the direction mask; Copperline models the same joystick-adapter wiring and still implements it |
| [NM-HRM-055](findings/NM-HRM-055.md) | SERDAT written with all zeros still transmits | open · RTL · **red** | `hrm/ch8-serdat-transmit`, `hrm/ch8-serial-interface` | **3-1, with a split inside one of them**: vAmiga and Denise implement p.259's second sentence in a single added token, and WinUAE implements it in its ordinary path (`if (!w) return;`) but not in its cycle-accurate one; Copperline writes an explicit `word == 0` branch to transmit a full frame anyway |
| [NM-HRM-057](findings/NM-HRM-057.md) | SERDATR bit 11 is a registered copy of RXD, not a direct read | open · RTL · **red**, contested | `hrm/ch8-serdatr-read-word`, `hrm/ch8-serial-interface`, `hrm/ch8-adkcon-and-interrupt-relations` | **2–1 for the page, one cannot answer** — vAmiga and Denise read the pin immediately, WinUAE has no pin, and Copperline's two synchroniser stages advance per colour clock, a lag longer than NanoMig's three `clk7_en` edges |
| [NM-HRM-058](findings/NM-HRM-058.md) | the nine-bit received-character window is longer than p.256 prints | open · page · **red** | `hrm/ch8-serdatr-read-word`, `hrm/ch8-serial-interface` | **three of four land on 10 and 11 bit times, exactly as NanoMig does**; WinUAE has no receiver and its only 10/11 constant is on the transmit side |
| [NM-HRM-059](findings/NM-HRM-059.md) | ADKCONR's bit 15 always reads 0, and so does everyone's | open · page · **red** | `hrm/ch8-adkcon-and-interrupt-relations` | **unanimous with NanoMig**: all four read bit 15 as 0, but only WinUAE masks on the read, so a crafted savestate would expose the bit in the other three |
| [NM-HRM-060](findings/NM-HRM-060.md) | the instruction cache monitors writes, and p.13 says it does not | open · RTL · **red** | `hrm/ch1-cpu-and-instruction-cache` | **3–0 against, one silent**: WinUAE, vAmiga and Copperline all leave the instruction cache untouched by writes; Denise models a 68000 and has no cache to disagree with |
| [NM-HRM-061](findings/NM-HRM-061.md) | two reds that are not defects: a whole-machine sentence graded at a board's config register | open · classification · **red** | `hrm/ch1-cpu-options` | **not applicable, and that is the evidence**: no emulator has a `system_cpu` field, because none of them is a board that must be told which CPU to instantiate |
| [NM-HRM-062](findings/NM-HRM-062.md) | Appendix F prints the two CIA address patterns interchanged | open · page · none | none yet · raised at classification | **unanimous against the print**: WinUAE, vAmiga, Denise and Copperline all decode the way the page's own prose says, as does NanoMig |
| [NM-HRM-063](findings/NM-HRM-063.md) | FLAG is negative edge-sensitive; the RTL ORs a level | open · RTL · **red** | `hrm/appF-ciab-port-assignments` · `PROP-FLAG-EDGE-POLARITY` | **unanimous, and all four implement the page**: WinUAE, vAmiga and Denise raise the bit from a transition and have no level input at all; Copperline keeps the pin state and latches only on `flag_pin_high && !high` |
| [NM-HRM-064](findings/NM-HRM-064.md) | the board advertises that it can be shut up, and register 4C has no handler | open · RTL · **red** | `hrm/appK-autoconfig-base-address`; `integration/appK-autoconfig` · `GATE-G` on fx68k and TG68K | **3–1**: WinUAE, Denise and Copperline all handle a write to 4C — Denise gates it on the very capability bit p.433 defines — while vAmiga's boards make the same promise and its `pokeACF8` has no `0x4C` case |
| [NM-HRM-065](findings/NM-HRM-065.md) | register 48 does not configure a Zorro III board, and the implementations side with NanoMig | open · page · **red** | `hrm/appK-autoconfig-base-address` | **two of four do exactly this**: WinUAE and Copperline both configure a Zorro III board on register 44 inside the Zorro II block; vAmiga and Denise have no Zorro III board type and configure on 48 because every board they own is a Zorro II one |
| [NM-HRM-066](findings/NM-HRM-066.md) | DDFSTRT = $0030 disables sprite 7 only when enough bitplanes are fetched | open · RTL · **red**, contested | `hrm/ch7-dmacon-register-bits` · the three one-bitplane assertions `DDFSTRT-EARLIER-THAN-0038-COSTS-SOME-SPRITES-FETCH-CYCLES`, `DDFSTRT-0030-SPRITE-7-AT-ONE-BITPLANE`, `DDFSTRT-0030-SOME-SPRITE-AFFECTED-AT-ONE-BITPLANE` | **3–1 for p. 82**: WinUAE, vAmiga and Denise block sprite DMA for the whole bitplane fetch at any plane count; Copperline keeps sprite 7 at $0030 at any plane count. NanoMig leaves sprite 7 all 172 fetches with one lores plane, 90 with two or three, 8 with four or more — the counts Figure 6-9 predicts when its slots are read by plane; TC64, a descendant, already blocks for the whole fetch |
| [NM-HRM-067](findings/NM-HRM-067.md) | sprites 6 and 7 fetch their control words after horizontal blanking ends, and no implementation keeps them inside it | open · page · **red** | `hrm/ch4-sprite-dma-control` · `SPRXPOS-SPRXCTL-FETCH-DURING-HBLANK` | **two of four end blanking before the last sprite slots, none keeps all sixteen inside it**: vAmiga ends it at $23 and rejects the $0F and $35 bounds p. 23 prints, Denise ends it before both of sprite 6's slots and both of sprite 7's, one more than NanoMig loses; WinUAE blanks on Denise's own counter and Copperline has no hardwired end, so neither can be placed on the slot grid |
| [NM-HRM-068](findings/NM-HRM-068.md) | p. 304 gives a PAL line 227.5 colour clocks, p. 23 gives it 227, and every implementation makes it 227 | open · page · **red** | `hrm/ch3-beam-counters-and-blanking` · `COLOUR-CLOCKS-PER-LINE-227POINT5 (PAL)` and its bundled form | **unanimous with NanoMig and with p. 23**: WinUAE, vAmiga, Denise and Copperline all end every PAL line at 227 colour clocks and alternate line lengths only in NTSC |
| [NM-HRM-069](findings/NM-HRM-069.md) | the 68000 is given chip-bus slots of both parities, where p. 196 allows it only the even ones | open · RTL · **red** | `integration/ch6-blitter-cpu-sharing` · `PARITY` on fx68k (no DMA and beside a share-mode blit) and on TG68K (beside the blit only) | **unanimous with NanoMig, against p. 196**: WinUAE, vAmiga, Denise and Copperline all let a CPU access take any colour clock DMA leaves free, and hand the bus back from a non-nasty blitter after a count of missed cycles, not at a parity; three of them spend BRA's internal operation as one colour clock, as fx68k does |
| [NM-HRM-071](findings/NM-HRM-071.md) | in SuperHires a sprite pixel lasts 70 ns, where p. 101 keeps it one low-resolution pixel | open · page · **red** | `integration/ch4-shres-sprite-b` · `PC2w` (a precondition, no claim id) on fx68k and TG68K | **unanimous with NanoMig and with Commodore's AA specification**: WinUAE, vAmiga, Denise and Copperline all give an ECS sprite a 70 ns pixel in SuperHires and 140 ns otherwise, the AA specification's "ECS defaults(LORES,HIRES=140nS,SHRES=70nS"; none keeps p. 101's low-resolution pixel there; outside the emulators, the 8373R4 dissection shifts a sprite out two pixels per 140 ns in SuperHires, 70 ns each |
| [NM-HRM-072](findings/NM-HRM-072.md) | a stop bit with a word waiting behind it lasts N+1½ colour clocks, where p. 255 gives every transmitted bit N+1 | open · RTL · **red** | `integration/ch8-disk-serial` · `P1`, `P2`, `P3` on fx68k and TG68K | **unanimous against NanoMig**: vAmiga, Denise and Copperline load a waiting word in the step that ends the stop bit, so its start bit follows N+1 colour clocks after the stop bit's; WinUAE has no TxD pin and, in its cycle-exact mode with no serial device, starts the waiting word's frame 10(N+1) colour clocks after the previous one began, 3 colour clocks later in its loopback mode; none adds half a colour clock |
| [NM-HRM-073](findings/NM-HRM-073.md) | a COPJMP2 written before Copper DMA is turned on starts the Copper at COP1LC, where pp. 25, 26, 30 and 276 start it at COP2LC | open · RTL · **red** | `integration/ch2-copper-control` · `P4` (through Run 2), `P4-Run2`, `P4-Run3`, `P4-Run3-reorder` and `P4-Run3-PAIR` on fx68k and TG68K, one behaviour: the first fetch is A's first word in every run | **unanimous against NanoMig on the package's runs**: WinUAE, vAmiga, Denise and Copperline all start Run 1 at A and Run 2 at B; WinUAE, vAmiga and Copperline start at COP2LC whatever vertical blank came before the COPJMP2, while Denise, like NanoMig, keeps the vertical blank's COP1LC strobe pending but merges it and starts at COP1LC \| COP2LC, which the package's addresses make B's |
| [NM-HRM-074](findings/NM-HRM-074.md) | PBON and OUTMODE are stored and read back but reach no logic, so PB6 and PB7 never carry the timer output pp. 341, 342 and 348–350 put there | open · RTL · **red**, contested | `hrm/appF-ciab-port-assignments` · `PROP-CRA-PBON`, `PROP-CRA-OUTMODE`; `integration/appF-cia-system` · `P1-MAIN-CRA`, `P1-MAIN-CRB`, `P1-MAIN`, `P2-TOGGLE` and its component lines `P2-LEGR`, `P2-MAIN`, `P2-LEGH`, and `P3`, on fx68k and TG68K, graded on CIA-B only; `P4`, `P9-LEGF-TRANSITIONS` and `P9-LEGW`, red on both, are not counted | **unanimous against NanoMig on PBON; split on p. 342's toggle and pulse clauses, which the silicon follows and none of the four follows in full**: WinUAE, vAmiga, Denise and Copperline all put a timer output on PB6 or PB7 when PBON is set, chosen by OUTMODE; vAmiga and Denise pulse high for one cycle, WinUAE shows 0 in pulse mode and Copperline pulses low; vAmiga, Denise and Copperline invert the toggle at each underflow, where WinUAE shows the START bit; Denise's and Copperline's toggle is high after reset, and Copperline's is not set on a start; vAmiga's CIA-B lets DDRB keep an input line at its pull-up; outside the emulators, the 8520R4 dissection's netlist gives a positive pulse, a toggle low after RES, high after a start and inverted at every underflow, and a line driven while PBON or the DDRB bit is set, as p. 342 does |
| [NM-HRM-075](findings/NM-HRM-075.md) | the CIAs answer only at $BF0000–$BFFFFF, where p. 353 selects them on the upper three address bits | open · RTL · **red**, contested | `hrm/appF-cia-address-decode` · `CIA-DECODE-DONTCARE` | **3–1 for p. 353 on the machines it describes**: WinUAE's models with neither Gayle nor Fat Gary, vAmiga and Denise select the CIAs anywhere in $A00000–$BFFFFF; Copperline, and WinUAE's Gayle and Fat Gary models, answer only in $BFD000–$BFDFFF and $BFE000–$BFEFFF; no named model has NanoMig's $BF0000–$BFFFFF. The package's `CIA-AB-TWOBIT-RULE`, green, measures both chips selected at $BFC100, as p. 351 predicts and the same three do, and is not part of the finding |
| [NM-HRM-077](findings/NM-HRM-077.md) | CIA-A has no PC output and ties FLAG to 0, where p. 351 wires them to the parallel port's DRDY\* and ACK\* | open · RTL · source observation | none: `hrm/appF-cia-register-map` records `CHIP-CAPABILITIES-FIVE`, not graded, as `ciaa` has no PC or FLAG pin; `integration/appF-cia-system` grades neither pin | **split, two and two, and all four read as NanoMig does with nothing attached**: Copperline pulses CIA-A's PC on every port-B access as the device's strobe and feeds its acknowledge into CIA-A's FLAG, and WinUAE sets CIA-A's FLAG from a configured printer, host port or parallel-port SCSI adapter; vAmiga and Denise have no PC and never set CIA-A's FLAG, driving FLAG only from the floppy index on CIA-B |
| [NM-HRM-078](findings/NM-HRM-078.md) | CRA bit 5 and CRB bit 5 are stored and never read: a timer set to count CNT counts φ2 | open · RTL · **red** | `hrm/appF-cia-timer-modes` · `CRA-INMODE` (`cia_timera` fixture) and `CRB-INMODE-ARITY-00-01` (`cia_timerb` fixture); `CRB-INMODE-ARITY-10-11` recorded, not graded (no CNT pin); `integration/appF-cia-system` · `P7.5` and `P8.65-MODE01`, carried by their CIA-B legs, on fx68k and TG68K; `P8.65-MODE11-PAIRK`, red on both, is not counted | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline all stop a timer's E-clock count once its INMODE selects CNT and count CNT's rising edges instead; vAmiga, Denise and Copperline drive CIA-A's CNT from KCLK and CIA-B's from PA1 and count mode 11 only while CNT is high, WinUAE drives CIA-B's alone and counts mode 11 as mode 10. MiSTer, a descendant, already selects the source from INMODE, with CNT tied high; TC64 has NanoMig's code |
| [NM-HRM-080](findings/NM-HRM-080.md) | with the motor on, RDY* is asserted for a drive with no disk in it, where pp. 334 and 368 make it mean a disk installed and up to speed | open · RTL · **red** | `hrm/appH-drive-sense-and-motor` · `RDY-DISK-PRESENCE`, `RDY-DISK-PRESENCE-DRIVE1`; `hrm/appDE-disk-interface` · `sol:E-disk.c26`, `opus:E-extdisk-iface.c4`, `P-READY-MOTOR-INVARIANT-DRIVE1` | **unanimous against NanoMig**: WinUAE, vAmiga, Denise and Copperline assert RDY* for a drive at speed only when a disk is in it, and all four identify a standard external drive as $FFFF FFFF with the motor off, the constant NanoMig returns |
| [NM-HRM-084](findings/NM-HRM-084.md) | the first pass after audio DMA starts is one word short of AUDxLEN from 4 up, and at AUDxLEN = 3 runs two words past the data set | open · RTL · **red** | `hrm/ch5-audio-dma-first-pass` · `FIRST-PASS-LEN-3`, `FIRST-PASS-LEN-4`, `FIRST-PASS-LEN-5`, `FIRST-PASS-LEN-7`, `FIRST-PASS-LEN-8`, `FIRST-PASS-LEN-16` and `NO-FETCH-BEYOND-DATA-SET-LEN-3`; `hrm/ch5-audio-channel-registers` · `AUDXLEN-BLOCK-1-INCLUDES-DISCARD` and `INTERRUPT-CADENCE-BLOCK-1-INCLUDES-DISCARD`; `hrm/ch5-audio-channel-state` · `F58-LENCOUNT-STARTUP-ARCS`, `5-p166.c4 F58.t101_010`, `5-p164.c23 5-p164.c24 5-sm.c25` and `5-p164.c28 5-sm.c29`; `integration/ch5-audio-output-scanrate` · precondition `A1` for T1 on fx68k and TG68K and for T3 on fx68k | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline all count the thrown-away start-up word and not the word that takes the channel from 101 to 010, as Figure 5-8's brackets do, so their first pass is AUDxLEN words like every later one and never runs past the data set; MiSTer and TC64, descendants, carry NanoMig's line |
| [NM-HRM-085](findings/NM-HRM-085.md) | a configured Zorro II memory board answers across the whole 8 MB Zorro II space, not only at the base software wrote to it | open · RTL · **red** | `integration/appK-autoconfig` · `P3-NOTOTHER`, `P3-BASE` (`sol:K-0837`, `opus:K-0869`) on fx68k and TG68K; their two controls on the same comparator, `P3-CONTROL-IO` and `P3-CONTROL-UNCONF`, green on both | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline take the base from the byte written to register 48 and map a Zorro II memory board from that base, for its own size and nowhere else; MiSTer and TC64, descendants, select $200000–$9FFFFF once the board is configured, whatever was written, as NanoMig does |
| [NM-HRM-086](findings/NM-HRM-086.md) | CIA-B has no serial port: in output mode a byte written to its SDR never sets the ICR's SP bit, where pp. 339, 345 and 348 give both 8520s one | open · RTL · **red** | `integration/appF-cia-system` · `P7.6-CIAB` on fx68k and TG68K; its green control `P7.6-CIAB-GREENCTL` (`NMH_MUTATE=P76CIAB-GREENCTL`) runs the same program on CIA-A and is green on both | **unanimous against NanoMig, and all four implement the page**: WinUAE, vAmiga, Denise and Copperline run one CIA model for both chips, and on CIA-B shift a byte written to SDR in output mode out on timer A's underflows and set ICR bit 3 when it is out; WinUAE, vAmiga and Denise take two underflows a bit, as p. 345 does, Copperline one. MiSTer and TC64, descendants, tie CIA-B's serial interrupt to 0 as NanoMig does |
| [NM-HRM-087](findings/NM-HRM-087.md) | with BPLCON0 bit 3 set, VPOSR and VHPOSR keep counting: neither a light-pen trigger nor the end of a field freezes them, where pp. 238 and 239 do both | open · RTL · **red**, contested | `integration/ch8-light-pen-beam` · `P1`, `P2`, `P2(ii)`, `P3`, `P3(a)`, `P3(b)`, `P3(c)`, `P4`, `P5`, `P6`, `P7`, `P8`, `P8(release-at-20)`, `P9`, `P9(boundary)`, `P10(a)`, `P11`, `P12` in OCS/PAL and OCS/NTSC on fx68k and TG68K; recorded, not graded, in ECS/PAL and ECS/NTSC | **split, two and two**: WinUAE and Copperline latch VPOSR and VHPOSR in OCS while BPLCON0 bit 3 is set, on a pulse from their light-pen device and, with none, at the end of the field; WinUAE releases the latch at the end of vertical blanking, Copperline holds it into the next field; vAmiga and Denise have no light-pen latch and freeze the counters only for ERSY; none of the four takes the trigger from a joystick's fire line. MiSTer and TC64, descendants, read the running counter as NanoMig does |
| [NM-HRM-088](findings/NM-HRM-088.md) | BPLxPTH carries five high bits on OCS, where pp. 272 and 291 give it three | open · RTL · **red** | `hrm/ch3-bitplane-pointers-and-modulo` · `BPLXPTH-OCS-HIGH-THREE-BITS`; its control `BPLXPTH-OCS-HIGH-THREE-BITS-CONTROL`, green | **three of the four narrow by chipset, as for NM-HRM-003, NM-HRM-022, NM-HRM-024 and NM-HRM-043**: Copperline masks the stored bitplane pointer by Agnus revision and names it in a test; WinUAE (`memory.cpp:2619-2623`) and vAmiga (`Agnus.cpp:190-194`) mask the address the bitplane fetch drives; Denise masks to the installed chip RAM only. MiSTer and TC64, descendants, carry NanoMig's five bits |

Chapter 1 contributes NM-HRM-060 and NM-HRM-061, and Appendix F NM-HRM-062
and NM-HRM-063. Appendix K contributes NM-HRM-064 and NM-HRM-065, both from
the AUTOCONFIG responder — the one part of the Zorro bus the Minimig family
implements. The responder is inherited from MiSTer, not NanoMig’s own, and
NM-HRM-065’s behaviour is inherited with it. Chapter 3 contributes NM-HRM-066,
measured in chapter 7's DMACON package because only the Agnus top level
compiles the bitplane and the sprite engine together. Appendix C contributes
NM-HRM-067 and NM-HRM-068, both filed against the printed page: the first
measured in chapter 4's sprite DMA package, the second beside the NTSC half of
the same p. 304 sentence, which is NM-HRM-010's. Chapter 6 contributes
NM-HRM-069, the first finding measured by an integration package — the whole
design at `nanomig.v`'s own ports, run once with each CPU core — and red on
both cores. Six more are measured by integration packages alone:
- NM-HRM-071, from chapter 4, by chapter 4's SuperHires sprite package;
- NM-HRM-072, from chapter 8, by the disk and serial package;
- NM-HRM-073, from chapter 2, by chapter 2's Copper control package;
- NM-HRM-085, from Appendix K, by Appendix K's AUTOCONFIG package; the Zorro II
  memory select it is about sits beside the responder in `cpu_wrapper.v` and is
  inherited from MiSTer as well;
- NM-HRM-086, from Appendix F, by Appendix F's CIA system package;
- NM-HRM-087, from chapter 8, by chapter 8's light-pen package.

Chapter 3 contributes NM-HRM-088 as well, measured in chapter 3's bitplane
pointer package: the bitplane pointer's OCS width, the fifth engine with the
cause NM-HRM-003, NM-HRM-022, NM-HRM-024 and NM-HRM-043 record.

Appendix F also contributes NM-HRM-074, NM-HRM-075, NM-HRM-077 and NM-HRM-078,
and Appendices E and H NM-HRM-080. Chapter 5 contributes NM-HRM-084, measured by
three of its unit packages: the first-pass, channel-register and channel-state
packages.

Nine findings that unit packages hold up show in integration packages too.
Six are graded red there: NM-HRM-010 by chapter 2's interlace package,
NM-HRM-028 by chapter 5's attach package, NM-HRM-038 by chapter 8's
controller-port package, NM-HRM-064 by Appendix K's AUTOCONFIG package, and
NM-HRM-074 and NM-HRM-078 by the CIA system package. The CIA system package also reproduces
NM-HRM-046 and NM-HRM-054, recorded and not graded, and chapter 5's audio
scan-rate package meets NM-HRM-084's short first pass through a precondition.

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

**Chapter 7 also strengthens two existing findings without adding a number.**
NM-HRM-008 is graded against p. 214 as well as p. 273: two of the three places
the manual states its rule, p. 214's sentence and Table 7-3, are chapter 7's,
and `hrm/ch7-collision-playfield-mode` measures it in both playfield modes.
NM-HRM-011 has chapter 7's own page and a contradiction inside it. Each says so
on its own page.
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
| [NM-AA-001](findings/NM-AA-001.md) | the AA list gives CLXCON2 two addresses, and $10C belongs to BPLCON4 | open · page · green with an inverse control | `aa/aa-clxcon2-address` | **unanimous for $10E**: WinUAE (`custom.cpp:7412`), vAmiga (`Memory.cpp:2894`) and Copperline (`custom_regs.rs:1154`) decode it there with BPLCON4 at $10C; Denise implements neither and still names both at those addresses |
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
**[`evidence/finding-analyses.md`](evidence/finding-analyses.md)**.

## How a finding is meant to end

A red test is closed by a decision about the RTL, never by relaxing the
assertion. If a deviation turns out to be intentional, the assertion stays and
the reason is written down beside it — a test whose expectation was quietly moved
to match the implementation stops being evidence about anything.
