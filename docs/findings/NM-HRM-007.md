# NM-HRM-007 — the hires fetch width cannot reach the manual's maximum of 49 words [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change is proposed now. Does real Agnus, with DDFSTOP at its $D8 limit in hires, fetch 49 words per plane (the last fetch one word), as p. 80 prints, where NanoMig fetches 50? Closes when the hires sequencer in `agnus_bitplanedma.v` delivers one word in the last fetch at the $D8 limit and `MAX-WORDS-49-HIRES-AT-HW-LIMITS` and `MAX-WORDS-49-HIRES` turn green, or when an Amiga specialist records that the sequence runs whole and gives 50, in which case this page is refiled against the printed page. |
| **Report to** | `NanoMig` — MiSTle-Dev/NanoMig |
| **Scope** | Agnus bitplane fetch in hires, FMODE 0, at DDFSTRT $18 / DDFSTOP $D8, and driven from $00/$FE beyond both limits; lores 25 is reachable. Chip revision: unknown; hardware not measured at $18/$D8; photographs named A500 OCS and A500 ECS reach stop $D8 at start $38 only |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 3 — Playfield, printed p. 80 (limits and the 25/49 table, Table 3-14) and p. 60 (word-count relation) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims: `MAX-WORDS-49-HIRES-AT-HW-LIMITS` carries `sol:3-max-window.c13`, `opus:3-max.c06`, `opus:3-max.c07`, `opus:3-t14.c10` (p. 80); `MAX-WORDS-49-HIRES` carries `sol:3-max-window.c28` (p. 80); p. 60 carries no claim id in either red assertion |
| **Module** | `agnus_bitplanedma.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v#L417-L461) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_bitplanedma.v#L417-L461); inherited |
| **Test** | [`hrm/ch3-ddf-fetch-window`](../../hrm/ch3-ddf-fetch-window): `make -C hrm/ch3-ddf-fetch-window test`; [`hrm/ch3-bitplane-memory-layout`](../../hrm/ch3-bitplane-memory-layout): `make -C hrm/ch3-bitplane-memory-layout test`; an expected red exits nonzero |
| **Assertions** | 2 red. `MAX-WORDS-49-HIRES-AT-HW-LIMITS` in `hrm/ch3-ddf-fetch-window`, at $18/$D8: expected 49 words, observed 50; the package reports `19 passed, 2 failed`, its other red being [NM-HRM-006](NM-HRM-006.md)'s. `MAX-WORDS-49-HIRES` in `hrm/ch3-bitplane-memory-layout`, driven from $00/$FE: expected at most 49, observed 50; the package reports `17 passed, 1 failed` |
| **Verdict of the four** | WinUAE and vAmiga fetch 50, as NanoMig does; Copperline's own test requires an even count; the manual says 49. Hardware: not run here; photographs named A500 OCS and A500 ECS show the last fetch at stop $D8, start $38, not one word, and do not reach 49 or 50 words (*What photographs of real machines show*) |
| **Effort** | none; if a specialist confirms the one-word cut at $D8, M: the stop condition of the hires sequencer in `agnus_bitplanedma.v` (`ddfend` / `ddfseq_match`, lines 417-461), so that the last fetch at the $D8 limit delivers one word |

---

## Source requirement

**Printed p. 80**:

> "Horizontally, the situation is similar. Strictly speaking, the hardware sets
> a rightmost limit to DDFSTOP of ($D8) and a leftmost limit to DDFSTRT of
> ($18). This gives a maximum of 25 words fetched in low resolution mode. In
> high resolution mode the maximum here is 49 words, because the rightmost limit
> remains ($D8) and only one word is fetched at this limit."

The accompanying table gives `max words fetched   25   49`.

**Printed p. 60** gives the general relation:

> DDFSTRT = DDFSTOP - (4 * (word count - 2)) for high resolution

At $18/$D8 that is 192/4 + 2 = **50** words, the full window; the 49 is that window with its last word cut at the limit, which is what p. 80 says.

## RTL and measurement

**The fetch runs in whole sequences of eight colour clocks, and in hires every sequence fetches each plane twice.** The module's own comment says *"One full display DMA sequence lasts 8 CCKs"* (line 328); `ddfseq` counts the colour clocks of a run and the plane number comes from its two low bits ([`#L432-L441`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v#L432-L441), [`#L460-L461`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v#L460-L461)). A run ends only at the end of a sequence, once the DDFSTOP comparison has dropped `ddfena` ([`#L417-L430`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v#L417-L430), [`#L443-L451`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/agnus_bitplanedma.v#L443-L451)):

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

- Whether real Agnus cuts the last hires fetch at the $D8 limit to one word, as p. 80 says, decides which side is wrong. The implementations agree with each other and not with the page: all that fetch at the limits fetch 50, the p. 60 window with nothing cut; photographs named A500 OCS and A500 ECS show the last fetch at stop $D8 not one word, at start $38 (*What photographs of real machines show*).
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
| **TC64** | [`rtl/minimig/agnus_bitplanedma.v:393-437`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_bitplanedma.v#L393-L437) | descendant — the same sequencer, byte for byte (lines 302 and 393-437, shifted by 26 and 24), despite 74 differing lines elsewhere in the file |

### What photographs of real machines show

[vAmigaTS](https://github.com/dirkwhoffmann/vAmigaTS/tree/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF), vAmiga's test suite, publishes a screen photograph per program. The photographs of [`farright1`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1.s) are named A500 OCS and A500 ECS, labels and nothing more ([OCS](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1_A500_OCS.JPG), [ECS](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1_A500_ECS.JPG)). The program draws one hires plane ([`farright1.s:247`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1.s#L247)) from DDFSTRT `$38` ([`farright1.s:9`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1.s#L9)), not the `$18` of this page's Scope, and steps DDFSTOP through `$D4`, `$D6`, `$D8`, `$DA`, `$DC` and `$DE` ([`farright1.s:330`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1.s#L330), [`farright1.s:337`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1.s#L337), [`farright1.s:349`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1.s#L349), [`farright1.s:356`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1.s#L356), [`farright1.s:363`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1.s#L363), [`farright1.s:370`](https://github.com/dirkwhoffmann/vAmigaTS/blob/e807e3256d73135f1960e7f9aef2c16d8a6a56b4/Agnus/DDF/DDF/farright1/farright1.s#L370)). No program of the family has hires at `$18` and `$D8`.

The right edge of the hires strip, in colour clocks, steps by 8 with the stop and then meets the screen limit: A500 OCS 146.7, 154.7, 162.6 and 169.4, A500 ECS 146.9, 155.0, 163.2 and 170.2, the last for the stops `$D4` to `$DE`, so the rows either side of `$D8` end alike. The lores strips in the same photographs end at 169.1 and 169.6 to 169.8. A last fetch of one word would end near 166.6 on A500 OCS and 167.2 on A500 ECS, and would be visible. So at DDFSTRT `$38` the last fetch at stop `$D8` is not one word.

The limits: the count of 49 or 50 words is not reached, since the display is clipped at the limit, as p. 80 itself says, and the strip holds 42 words, not 50; that the same holds at `$18`, which shares the 8-colour-clock grid, is an inference. The page stays open on the question of 49 against 50. The bars are read from the photographs, with the edge measured in colour clocks; the programs were not run on a machine here.

## Decision

**None until a specialist confirms the cut.** What p. 80 describes is a last hires fetch at $D8 that delivers one word instead of two. If confirmed, the change is to stop the hires fetch one word early when DDFSTOP is at the $D8 limit. The closure is in the header.
