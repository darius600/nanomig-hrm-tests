# NM-HRM-007 — the hires fetch width cannot reach the manual's maximum of 49 words [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Does real Agnus, with DDFSTOP at its $D8 limit in hires, fetch 49 words per plane (the last fetch one word), as p. 80 prints, where NanoMig fetches 50? The decision belongs to an Amiga specialist, not to this repository. Closes when an Amiga specialist records that the hires maximum is 49 through a one-word last fetch at $D8, in which case *Decision* applies, or that the sequence runs whole and gives 50, in which case this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer, if the manual's reading is accepted: MiSTer and TC64 have the same sequencer code as NanoMig, so there is no fix to port; nothing is filed before the specialist's answer |
| **Scope** | Agnus bitplane fetch in hires, FMODE 0 (the branch quoted below), at DDFSTRT $18 / DDFSTOP $D8, and driven from $00/$FE beyond both limits; lores 25 is reachable. Chip revision: unknown; not measured on hardware |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 3 — Playfield, printed p. 80 (limits and the 25/49 table, Table 3-14) and p. 60 (word-count relation) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `MAX-WORDS-49-HIRES-AT-HW-LIMITS` carries `3-max-window.c13`, `3-max.c06`, `3-max.c07`, `3-t14.c10`; `MAX-WORDS-49-HIRES` carries `3-max-window.c28`; p. 60 carries no claim id in either red assertion |
| **Module** | `agnus_bitplanedma.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L417-L461) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L417-L461); inherited |
| **Test** | [`hrm/ch3-ddf-fetch-window`](../../hrm/ch3-ddf-fetch-window): `make -C hrm/ch3-ddf-fetch-window test`; [`hrm/ch3-bitplane-memory-layout`](../../hrm/ch3-bitplane-memory-layout): `make -C hrm/ch3-bitplane-memory-layout test`; an expected red exits nonzero |
| **Assertions** | 2 red. `MAX-WORDS-49-HIRES-AT-HW-LIMITS` in `hrm/ch3-ddf-fetch-window`, at $18/$D8: expected 49 words, observed 50; the package reports `19 passed, 2 failed`, its other red being [NM-HRM-006](NM-HRM-006.md)'s. `MAX-WORDS-49-HIRES` in `hrm/ch3-bitplane-memory-layout`, driven from $00/$FE: expected at most 49, observed 50; the package reports `17 passed, 1 failed` |
| **Verdict of the four** | **no witness fetches 49**: WinUAE and vAmiga fetch 50 at the limits, as NanoMig does; Denise not read; Copperline's own test requires an even count; the manual says 49. Hardware status: unknown |
| **Effort** | M, if a specialist confirms the cut: the stop condition of the hires sequencer in `agnus_bitplanedma.v` (`ddfend` / `ddfseq_match`, lines 417-461) would end one word early at the $D8 limit. None until then |

---

## Source requirement

**Printed p. 80**:

> "Strictly speaking, the hardware sets a rightmost limit to DDFSTOP of ($D8)
> and a leftmost limit to DDFSTRT of ($18). This gives a maximum of 25 words
> fetched in low resolution mode. In high resolution mode the maximum here is
> 49 words, because the rightmost limit remains ($D8) and only one word is
> fetched at this limit."

The accompanying table gives `max words fetched   25   49`.

**Printed p. 60** gives the general relation:

> DDFSTRT = DDFSTOP - (4 * (word count - 2)) for high resolution

At $18/$D8 that is 192/4 + 2 = **50** words, the full window; the 49 is that window with its last word cut at the limit, which is what p. 80 says.

## RTL and measurement

**The fetch runs in whole sequences of eight colour clocks, and in hires every sequence fetches each plane twice.** The module's own comment says *"One full display DMA sequence lasts 8 CCKs"* (line 328); `ddfseq` counts the colour clocks of a run and the plane number comes from its two low bits ([`#L432-L441`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L432-L441), [`#L460-L461`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L460-L461)). A run ends only at the end of a sequence, once the DDFSTOP comparison has dropped `ddfena` ([`#L417-L430`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L417-L430), [`#L443-L451`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_bitplanedma.v#L443-L451)):

```verilog
      else if ((ddfend || !vdiwena) && ddfseq_match) // cleared at the end of last bitplane DMA cycle
        ddfrun <= #1 1'b0;
...
    else if (hpos[0] && (ddfseq[2:0]==7) && !ddfena)
      ddfend <= #1 1'b1;
```

So the hires word count per plane is always even and nothing cuts the last fetch at the $D8 limit: 49 is not reachable by any register pair.

| | words |
|---|---:|
| manual, hires maximum | 49 |
| p. 60's formula at $18/$D8, nothing cut | 50 |
| NanoMig, measured at $18/$D8 | **50** |
| WinUAE and vAmiga at the same limits | **50** |

Nobody produces 49. `MAX-WORDS-49-HIRES-AT-HW-LIMITS` grades the $18/$D8 case, and `MAX-WORDS-49-HIRES` the ceiling from beyond both limits.

## Risks and unknowns

- Whether real Agnus cuts the last hires fetch at the $D8 limit to one word, as p. 80 says, decides which side is wrong. The implementations agree with each other and not with the page: all that fetch at the limits fetch 50, the p. 60 window with nothing cut.
- No implementation read models the one-word fetch; a change that followed the page would give a behaviour nobody else has, on the strength of one sentence.
- The third-witness reading offers a mechanism ([B4](../evidence/third-witness-ch3-ch6.md#b4--hires-fetches-50-words-at-both-hardware-limits-not-49-2-assertions)): $D8 − $18 = 192 is an exact multiple of the 8-CCK block, so the stop lands on a block boundary and one whole further block completes; "only one word is fetched at this limit" would describe a stop that lands mid-block, which this register pair never produces.
- Copperline's even-count test runs at the standard `$3C`/`$D4` window, not at the limits. Denise was not read.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | — | 50 words at the limits |
| **vAmiga** | — | 50 words at the limits |
| **Denise** | — | not read |
| **Copperline** | [`src/chipset/ddf_sequencer.rs:662`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/ddf_sequencer.rs#L662) | <pre>assert_eq!(plane0 % 2, 0);</pre> two words per plane per 8-cck hires unit, written as an invariant |
| **MiSTer** | [`rtl/agnus_bitplanedma.v:417-461`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L417-L461) | descendant — identical to NanoMig: lines 328 and 417-461 match line for line |
| **TC64** | [`rtl/minimig/agnus_bitplanedma.v:393-437`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L393-L437) | descendant — the same sequencer, byte for byte (lines 302 and 393-437, shifted by 24), despite 74 differing lines elsewhere in the file |

## Decision

**None until a specialist confirms the cut.** What p. 80 describes is a last hires fetch at $D8 that delivers one word instead of two. If confirmed, the change is to stop the hires fetch one word early when DDFSTOP is at the $D8 limit. The closure is in the header.
