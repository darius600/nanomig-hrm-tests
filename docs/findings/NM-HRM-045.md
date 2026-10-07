# NM-HRM-045 — DMAON needs a third term that p.278 does not print

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault. The red is against p.278; the chapter's own p.248 prints the composition NanoMig implements. Closes when an Amiga specialist decides whether the DSKBYTR bit-14 row of Appendix A gets the DSKEN clause given under *Decision*, or is recorded as shorthand. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Appendix A of the *Amiga Hardware Reference Manual*, 3rd ed. This is a defect in one printed page, measured against another printed page of the same manual |
| **Scope** | unknown: the page does not name a chip revision; measured on NanoMig's `paula` at the pinned revision, with DSKLEN.DMAEN and DMACON.DMAEN held true and only DMACON.DSKEN varied |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed p.248, Table 8-7, and Appendix A, printed p.278, DSKBYTR bit 14 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims `opus:8-dskbytr.c9` / `sol:8-dskbytr.c10` (p.248), `opus:8-appA-dskbytr.c11` / `sol:8-appa-dskbytr.c11` (p.278) |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L699) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L397); inherited, unchanged in MiSTer and TC64 |
| **Test** | [`hrm/ch8-dmaon-enable-composition`](../../hrm/ch8-dmaon-enable-composition): `make -C hrm/ch8-dmaon-enable-composition test`; the expected red exits nonzero |
| **Assertions** | 1 red of 6 graded: `A2-DMAON_P278_TWO_TERM_COMPOSITION` — expected DMAON=1 with DSKEN=0, observed DMAON=0. `A1` grades p.248's reading across all eight term combinations and is green on the identical measurement basis |
| **Verdict of the four** | **unanimous against p.278.** All four require the DSKEN term; not one implements the two-term formula as Appendix A prints it |
| **Effort** | none: no RTL change; the correction is one clause in the manual |
| **See also** | [NM-HRM-041](NM-HRM-041.md), which is about the same register's *constant* fields. This finding is only about bit 14 |

---

## Source requirement

The manual defines DSKBYTR bit 14 twice, and the two definitions do not agree.
This entry grades **p.278** and only p.278.

**Chapter 8, printed p.248**, Table 8-7, bit 14:

```
14      DMAON       Indicates when DMA is actually enabled. All the various
                    DMA bits must be true. This means the DMAEN bit in
                    DKSLEN, and the DSKEN & DMAEN bits in DMACON.
```

**Appendix A, printed p.278**, the DSKBYTR bit table:

```
                 14    DMAON      Mirror of bit 15 (DMAEN) in  DSKLEN ,
                                  ANDed with Bit09 (DMAEN) in  DMACON
```

p.248 names three terms, with "all the various DMA bits must be true" standing
in front of them; `DKSLEN` is the print's spelling of `DSKLEN` in this row, and
the bit 13 row below it spells it `DSKLEN`. p.278 names two: DSKEN, DMACON bit
04, is not among them, although the DMACON table directly above this row on the
same page prints it, and p.278 offers no "and the other DMA bits" clause that
would let it in. The readings differ on exactly one input: with DSKLEN's DMAEN armed and
DMACON's master DMAEN true, p.248 predicts DMAON follows DSKEN and p.278
predicts it stays 1 either way.

## RTL and measurement

Bit 14 of the DSKBYTR read is `trackrd|trackwr`
([`paula_floppy.v#L699`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L699)),
which are non-zero only once the DMA state machine has left idle. Unrolled, the
bit the CPU reads back is

    DSKBYTR[14]  =  DSKLEN.DMAEN (double-written)
                 &  DMACON.DSKEN
                 &  DMACON.DMAEN (master)
                 &  DSKLEN[13:0] != 0

— p.248's three terms, and a nonzero-length term that neither page names.

| DMACON.DSKEN (DSKLEN.DMAEN=1, DMACON.DMAEN=1 fixed) | p.278 expects | measured |
|---|---|---|
| 0 | DMAON=1 | `DSKBYTR=0x9000`, DMAON=0 |
| 1 | DMAON=1 | `DSKBYTR=0xD000`, DMAON=1 |

> **FAIL: A2-DMAON_P278_TWO_TERM_COMPOSITION** (p.278,
> `opus:8-appA-dskbytr.c11` / `sol:8-appa-dskbytr.c11`): graded as printed —
> with DSKLEN.DMAEN=1 and DMACON.DMAEN(master)=1 held, this page's own two-term
> reading predicts DMAON=1 regardless of DMACON.DSKEN, since DSKEN is not one of
> the two named terms. Measured: DMAON follows DSKEN instead (0 when DSKEN=0, 1
> when DSKEN=1) — the DSKEN=0 row contradicts the page as printed.

p.248 control green: `A1-DMAON_P248_THREE_TERM_COMPOSITION` (p.248,
`opus:8-dskbytr.c9` / `sol:8-dskbytr.c10`) holds across all eight combinations
of DSKLEN.DMAEN, DMACON.DSKEN and DMACON.DMAEN(master): DMAON reads 1 if and
only if all three are true. `A3`, `A4` and `A5` knock out one term each and are
green; `A4-DMAON_REQUIRES_DMACON_DSKEN` is the DSKEN term p.278 omits.

## Risks and unknowns

- **Nothing in NanoMig is wrong.** That this is scored as a red at all follows
  from the register's convention of grading each printed sentence on its own
  authority; the RTL's behaviour is correct under the other page.
- **p.278 may be meant as shorthand rather than as a definition.** "Mirror of
  bit 15 … ANDed with Bit09" names the two bits a programmer arms explicitly;
  DSKEN is arguably assumed by the fact that disk DMA is running at all. The
  page does not say so, which is why it is graded as printed — the same
  treatment [NM-HRM-042](NM-HRM-042.md) gives the "32-bit wide address" sentences.
- **The nonzero-length term appears on no page at all.** NanoMig, vAmiga and
  Copperline require a non-empty transfer, and WinUAE and Denise do unless
  ADKCON's WORDSYNC holds the transfer waiting for a sync word; neither
  p.248 nor p.278 names that condition. In NanoMig the length term gates only the
  start of a transfer: in write mode `trackwr` stays set in `DISKDMA_ACTIVE` after
  the count reaches zero, until the FIFO has drained
  ([`paula_floppy.v#L1007-L1011`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L1007-L1011)).
  It falls outside this finding's claim,
  but it means neither page is a complete specification of the bit.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:4590-4591`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4590-L4591) | requires an armed transfer and DSKEN with the master bit |
| **vAmiga** | [`DiskControllerRegs.cpp:133`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskControllerRegs.cpp#L133) | requires an armed transfer and DSKEN with the master bit |
| **Denise** | [`emulation/libami/paula/fdc.cpp:214`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L214) | requires an armed transfer and DSKEN with the master bit |
| **Copperline** | [`src/floppy/mod.rs:2733-2738`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2733-L2738) | names the live DSKLEN bit, the master bit and DSKEN separately |
| **MiSTer** | [`rtl/paula_floppy.v:397`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L397) | descendant — the DSKBYTR line and the `dsken` wiring are byte-identical |
| **TC64** | [`rtl/minimig/paula_floppy.v:633`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L633) | descendant — the same composition and wiring, unchanged |

Four implementations written independently, in two languages, from the same
manual, each arrived at p.248's three-term composition; NanoMig makes five. The
two descendants carry the same signals and add no second reading of the page.
None of them decides this finding, but p.278's line is not a reading anybody
has taken.

## Decision

**No RTL change.** NanoMig already implements the composition p.248 prints, and
p.248 is the chapter's own definition of the register.

The change belongs to the manual. **p.278's DSKBYTR bit-14 row is an incomplete
restatement of p.248's**, and an implementer working from Appendix A alone —
which is what a register summary is for — would build a DMAON that ignores
DSKEN. The correction is one clause: *"ANDed with bits 09 (DMAEN) and 04
(DSKEN) in DMACON"*.

That is a decision for Amiga specialists, and a documentation decision rather
than a hardware one. The finding closes when they either adopt that clause for
p.278 or record the row as shorthand for p.248.

The two pages are quoted above, only DSKEN moves between the measured rows, and
p.248's reading is green in the same run.
