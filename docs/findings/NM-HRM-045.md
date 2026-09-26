# NM-HRM-045 — DMAON needs a third term that p.278 does not print

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault. The red is against p.278; the chapter's own p.248 prints the composition NanoMig implements. |
| **Module** | `paula_floppy.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L699) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L397) |
| **Report to** | **nobody in the RTL.** This is a defect in one printed page, measured against another printed page of the same manual |
| **Test** | [`hrm/ch8-dmaon-enable-composition`](../../hrm/ch8-dmaon-enable-composition) — assertion `A2-DMAON_P278_TWO_TERM_COMPOSITION` |
| **Chapter** | 8 — Interface Hardware, and Appendix A · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red of 6 graded. `A1` sweeps all eight term combinations against p.248's reading and is **green on the identical measurement basis** |
| **Verdict of the four** | **unanimous against p.278.** All four require the DSKEN term; not one implements the two-term formula as Appendix A prints it |
| **See also** | [NM-HRM-041](NM-HRM-041.md), which is about the same register's *constant* fields. This finding is only about bit 14 |

---

## Which page this finding is against

The manual defines DSKBYTR bit 14 twice, and the two definitions do not agree.
This entry grades **p.278** and only p.278.

**Chapter 8, printed p.248**, Table 8-7, bit 14:

```
     14      DMAON      Indicates when DMA is actually enabled.  All the
                        various DMA bits must be true.  This means the
                        DMAEN bit in  DSKLEN , and the DSKEN & DMAEN bits
                        in  DMACON .
```

Three terms, named individually, with "all the various DMA bits must be true"
standing in front of them.

**Appendix A, printed p.278**, the DSKBYTR bit table:

```
                 14    DMAON      Mirror of bit 15 (DMAEN) in  DSKLEN ,
                                  ANDed with Bit09 (DMAEN) in  DMACON
```

Two terms. DSKEN — DMACON bit 04, the neighbour of the bit 09 this row does
name — is not among them, and p.278 offers no "and the other DMA bits" clause
that would let it in.

**The two readings differ on exactly one input.** Hold DSKLEN's DMAEN armed and
DMACON's master DMAEN true, then vary DMACON's DSKEN: p.248 predicts DMAON
follows DSKEN, p.278 predicts DMAON stays 1 either way. That single row is what
the red assertion drives.

## What NanoMig does

The DSKBYTR read is one combinational line, and bit 14 of it is
`trackrd|trackwr`:

[`src/minimig-aga/paula_floppy.v#L699`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L699):

```verilog
assign dskbytr = reg_address_in[8:1]==DSKBYTR[8:1] ? {1'b1,(trackrd|trackwr),dsklen[14],5'b1_0000,8'h00} : 16'h00_00;
```

`trackrd` and `trackwr` are driven non-zero only in `DISKDMA_ACTIVE`
([`#L1007-L1011`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L1007-L1011)),
and the state machine leaves `DISKDMA_IDLE` only on `dma_active`
([`#L248`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L248),
[`#L1002-L1003`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L1002-L1003)):

```verilog
wire dma_active = dmaen && !lenzero && enable;
```

`dmaen` is the DSKLEN bit-15 double-write latch
([`#L742-L743`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L742-L743)),
and `enable` is the port that `paula` wires to `dsken`
([`paula.v#L174`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L174),
[`paula.v#L251`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L251)):

```verilog
assign	dsken = dmacon[4] & dmaen;
```

where `dmacon[4]` is DMACON's DSKEN and the local `dmaen` there is DMACON bit
09, the master enable. Unrolled, the bit the CPU reads back is

    DSKBYTR[14]  =  DSKLEN.DMAEN (double-written)
                 &  DMACON.DSKEN
                 &  DMACON.DMAEN (master)
                 &  DSKLEN[13:0] != 0

— p.248's three terms, and a nonzero-length term that neither page names.

**A note on naming.** The module has an internal register also called `dmaon`
([`#L137`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_floppy.v#L137)),
which gates the FIFO and `dmal`. It is *not* what DSKBYTR bit 14 reports. Both
are asserted only inside `DISKDMA_ACTIVE`, so the distinction does not change
this measurement, but a reader tracing the RTL by name meets the wrong signal
first.

### Measured

    A2-ROW DSKEN=0 (DSKLEN.DMAEN=1, DMACON.DMAEN=1 fixed): DSKBYTR=0x9000 DMAON=0
    A2-ROW DSKEN=1 (DSKLEN.DMAEN=1, DMACON.DMAEN=1 fixed): DSKBYTR=0xD000 DMAON=1

> **FAIL: A2-DMAON_P278_TWO_TERM_COMPOSITION** (p.278,
> `opus:8-appA-dskbytr.c11` / `sol:8-appa-dskbytr.c11`): graded as printed —
> with DSKLEN.DMAEN=1 and DMACON.DMAEN(master)=1 held, this page's own two-term
> reading predicts DMAON=1 regardless of DMACON.DSKEN, since DSKEN is not one of
> the two named terms. Measured: DMAON follows DSKEN instead (0 when DSKEN=0, 1
> when DSKEN=1) — the DSKEN=0 row contradicts the page as printed.

**The same run grades p.248's reading green across all eight combinations.**

> **PASS: A1-DMAON_P248_THREE_TERM_COMPOSITION** (p.248, `opus:8-dskbytr.c9`,
> `sol:8-dskbytr.c10`): across all 8 combinations of DSKLEN.DMAEN, DMACON.DSKEN
> and DMACON.DMAEN(master), DMAON reads 1 if and only if all three are true.

Rows 0–6 each measure `DSKBYTR=0x9000` (DMAON=0); row 7, with all three true,
measures `0xD000` (DMAON=1). `A3`, `A4` and `A5` then knock out one term each
and confirm each is individually necessary — `A4` is precisely the DSKEN term
p.278 omits, and it is green.

**The comparator is not stuck.** `A2-SUBSTITUTE-TWO-TERM-COMPARATOR-LIVE`
applies the identical two-term-AND-predicts-output shape to a pair the RTL does
satisfy, and grades green in the same run. A2's red is a measured divergence,
not a dark check.

## What the others say

Not one of the four independent implementations implements the two-term
formula. All four require a DSKEN term, and all four require the DSKLEN
arming as well — p.248's reading, in four independent codebases.

| | code | |
|---|---|---|
| **WinUAE** | [`disk.cpp:4590-4591`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4590-L4591) | <pre>if (dskdmaen != DSKDMA_OFF && dmaen(DMA_DISK))<br>&nbsp;&nbsp;v \|= 0x4000;</pre> and `dmaen()` is itself two terms — [`include/custom.h:87-90`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/custom.h#L87-L90): `return (dmamask & dmacon) && (dmacon & 0x200);`, DSKEN (`0x0010`) **and** the master bit. `dskdmaen` leaves `DSKDMA_OFF` only on the DSKLEN bit-15 double write ([`disk.cpp:4870-4878`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/disk.cpp#L4870-L4878)) |
| **vAmiga** | [`DiskControllerRegs.cpp:133`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskControllerRegs.cpp#L133) | <pre>if (agnus.dskdma() && state != DriveDmaState::OFF) SET_BIT(result, 14);</pre> with [`Agnus.h:554`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Agnus.h#L554): `static bool dskdma(u16 v) { return (v & DMAEN) && (v & DSKEN); }`. The state leaves `OFF` at [`DiskControllerRegs.cpp:60`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/DiskController/DiskControllerRegs.cpp#L60), under `if (oldValue & newValue & 0x8000)` |
| **Denise** | [`emulation/libami/paula/fdc.cpp:214`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/fdc.cpp#L214) | <pre>if (dmaDisk && (diskState != DiskState::OFF)) out \|= 0x4000;</pre> and `dmaDisk` comes from one masked compare, [`paula/paula.cpp:120`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/paula.cpp#L120): `dmaDisk = (value & 0x210) == 0x210;`. `0x210` is DSKEN **and** the master bit together, so a DMACON write clearing either one clears `dmaDisk` |
| **Copperline** | [`src/floppy/mod.rs:2733-2738`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L2733-L2738) | <pre>fn dma_enabled(&self, dmacon: u16) -&gt; bool {<br>&nbsp;&nbsp;self.dma.is_some()<br>&nbsp;&nbsp;&nbsp;&nbsp;&& self.dsklen & DSKLEN_DMAEN != 0<br>&nbsp;&nbsp;&nbsp;&nbsp;&& dmacon & DMACON_DMAEN != 0<br>&nbsp;&nbsp;&nbsp;&nbsp;&& dmacon & DMACON_DISK != 0<br>}</pre> **the most literal of the four** — an armed transfer, the live DSKLEN bit, the master bit and DSKEN, each named separately. Called for bit 14 at [`mod.rs:1430-1432`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/floppy/mod.rs#L1430-L1432) |
| **MiSTer** | [`rtl/paula_floppy.v:397`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_floppy.v#L397) and [`rtl/paula.v:169`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v#L169)/[`:246`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula.v#L246) | descendant — `rtl/paula_floppy.v:397` and `rtl/paula.v:169`/`:246` — the DSKBYTR line this entry shares with [NM-HRM-041](NM-HRM-041.md) is byte-identical, and so is the `dsken` wiring: `assign dsken = dmacon[4] & dmaen;` and `.enable(dsken)` |
| **TC64** | [`rtl/minimig/paula_floppy.v:633`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_floppy.v#L633) and [`rtl/minimig/paula.v:188`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L188)/[`:264`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula.v#L264) | descendant — `rtl/minimig/paula_floppy.v:633` and `rtl/minimig/paula.v:188`/`:264` — the same DSKBYTR composition and the same `dsken` wiring, unchanged. The three-term composition p.278 does not print — DSKLEN.DMAEN, DMACON.DSKEN, DMACON.DMAEN — is built from the same signals in all three trees. The red is against the manual's own appendix, not against any of the RTL, so no fix belongs to any of the three |

**The agreement is total among the four independent implementations, and it is
on p.248's side.** Four implementations written independently, in three
languages, from the same manual, each arrived at the three-term composition.
NanoMig makes five — and both maintained Minimig lines carry the identical
signals, so as descendants they add a sixth and seventh instance of the same
inherited two-term line, not a second reading of the page.

**None of them decides this finding.** They are reported because a unanimous
verdict is itself a result: p.278's line is not a reading anybody has taken.

## Suggested change

**No RTL change.** NanoMig already implements the composition p.248 prints, and
p.248 is the chapter's own definition of the register.

The change belongs to the manual. **p.278's DSKBYTR bit-14 row is an incomplete
restatement of p.248's**, and an implementer working from Appendix A alone —
which is what a register summary is for — would build a DMAON that ignores
DSKEN. The correction is one clause: *"ANDed with bits 09 (DMAEN) and 04
(DSKEN) in DMACON"*.

That is a decision for Amiga specialists, and a documentation decision rather
than a hardware one.

## What holds the finding up

- **Both pages are quoted in full above**, and the disagreement is over a named
  bit, not over an inference.
- **The measurement isolates the disputed term.** DSKLEN.DMAEN and the DMACON
  master bit are held true across both rows; only DSKEN moves.
- **p.248's reading is green in the same run, over all eight combinations**, and
  the three single-term-necessity assertions are green as well. The red is not a
  fixture incapable of producing a pass.
- **A live-comparator control passes** on the identical two-term shape applied
  to a pair the RTL does satisfy.
- **All four other implementations require the omitted term.**

## What weakens it

- **Nothing in NanoMig is wrong.** That this is scored as a red at all follows
  from the register's convention of grading each printed sentence on its own
  authority; the RTL's behaviour is correct under the other page.
- **p.278 may be meant as shorthand rather than as a definition.** "Mirror of
  bit 15 … ANDed with Bit09" names the two bits a programmer arms explicitly;
  DSKEN is arguably assumed by the fact that disk DMA is running at all. The
  page does not say so, which is why it is graded as printed — the same
  treatment [NM-HRM-042](NM-HRM-042.md) gives the "32-bit wide address" sentences.
- **The nonzero-length term appears on no page at all.** NanoMig, WinUAE,
  vAmiga, Denise and Copperline all require a non-empty transfer, and neither
  p.248 nor p.278 names that condition. It falls outside this finding's claim,
  but it means neither page is a complete specification of the bit.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, 8371 Agnus specification, part 318071-01, 1987, revised 1988; US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

