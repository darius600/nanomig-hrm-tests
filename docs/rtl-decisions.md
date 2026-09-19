# Six RTL decisions, and how two emulators answered them

Every finding in this suite ends the same way: the manual says one thing, the
measurement says another, and **the pipeline stops there**. Deciding what the RTL
should do is an Amiga engineering question, not a testing one.

This file exists so that decision can start from evidence instead of from
scratch. For each open finding it gives the manual's words, what NanoMig
measurably does, the test that measures it, and - where it could be established -
what WinUAE and vAmiga do about the same question.

## How to read the emulator columns

`docs/third-witness.md` sets the rules for consulting an emulator, and they still
hold. The important one:

> An emulator is **not** promoted to oracle. If the manual is clear and the RTL
> disagrees with it, the finding stands; an emulator agreeing with the RTL would
> mean the manual is wrong about the hardware, and that is a separate question
> with a separate answer. It never quietly cancels a finding.

Most of the findings below are that shape - the manual is clear, NanoMig differs.
So the emulator columns are here as **engineering context for whoever fixes it**,
not as a verdict. Two independent implementations agreeing tells you a chosen
behaviour is at least workable and probably matches real silicon; they are not
evidence about what the manual meant.

Where they *do* bear on a manual-internal contradiction, that is said explicitly.

### Sources

| | Repository | Commit | Read |
|---|---|---|---|
| vAmiga | [dirkwhoffmann/vAmiga](https://github.com/dirkwhoffmann/vAmiga) | [`35de004`](https://github.com/dirkwhoffmann/vAmiga/tree/35de004a2f1502c2811b9a8ebcde4d0979a98937) | 2026-09-10 |
| WinUAE | [tonioni/WinUAE](https://github.com/tonioni/WinUAE) | [`ccc407e`](https://github.com/tonioni/WinUAE/tree/ccc407e9c636668b99b5f19d6e9aa03ea04b3052) | 2026-09-10 |

The same commits `third-witness.md` already records, so earlier citations and
these agree. Both are shallow clones under `_oracle/`, never built, nothing
copied.

**Read the arithmetic, not the comments** - the rule that caught a mistake while
this file was being written. vAmiga defines a chipset-dependent `ptrMask` and I
first recorded it as enforcing the pointer width. It appears in info and log paths
throughout; the enforcement is somewhere else entirely, in `Memory.cpp`. A
definition is not an implementation.

---

# Chapter 6 - Blitter

## NM-HRM-020 — BLTSIZV is not gated on ECS - and both emulators gate it

**Unanimous. The clearest of the nine.**

| | |
|---|---|
| Manual | Appendix C's ECS register map, printed p.298, marks `BLTSIZV $05C` **new**; printed p.308 names it among "Two new registers ... which make larger blits possible", "h = Height (15 bit height = 32768 lines max)" |
| NanoMig | `agnus_blitter.v:548` loads `height[14:0]` with **no `&& ecs`** - and the comment on that same line calls it ECS |
| Measured | height loads identically at `ecs=0` and `ecs=1` |
| Test | `hrm/ch6-blitter-ecs`, `BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS` (red) |

**WinUAE** drops the write entirely on OCS - `custom.cpp:3915`:

    static void BLTSIZV(int hpos, uae_u16 v)
    {
        if (ecs_agnus) {
            int wait = currprefs.m68k_speed == 0 && currprefs.blitter_cycle_exact;
            pipelined_custom_write(BLTSIZV_func, v, wait);
        }
    }

**vAmiga** makes the same decision - `Blitter/BlitterRegs.cpp:324`:

    void Blitter::pokeBLTSIZV(u16 value)
    {
        // ECS only register
        if (agnus.isOCS()) return;
        agnus.recordRegisterChange(DMA_CYCLES(2), Reg::BLTSIZV, value);
    }

**What a fix touches.** One condition on `agnus_blitter.v:548`. NanoMig already
does exactly this for `BLTCON0L` (`:259`) and `BLTSIZH` (`:500,523,557`), so the
pattern to copy is a few lines away in the same file. The decode for this
register's two ECS siblings is gated; only this one is not.

**Note for whoever decides.** The manual, WinUAE and vAmiga agree. That is as
close to settled as this project's evidence gets, and the remaining question is
only whether NanoMig deliberately targets a machine where the distinction cannot
matter.

## NM-HRM-021 — DOFF is not implemented - and the emulators disagree with each other

**Split. The most interesting of the nine.**

| | |
|---|---|
| Manual | Appendix C p.309: `DOFF`, BLTCON1 bit 7 - disables the D output in line mode |
| NanoMig | `bltcon1[7]` appears nowhere in `agnus_blitter.v`. The bits it reads are 15-12, 4, 3, 2, 1, 0 |
| Measured | setting bit 7 changes no cycle, no `we` pulse and no written word |
| Test | `hrm/ch6-blitter-ecs`, `DOFF-NO-OBSERVABLE-EFFECT` (red) |

**WinUAE implements it**, ECS-gated - `blitter.cpp:1541`:

    blit_dof = ecs_agnus && (bltcon1_next & BLTDOFF);

with `BLTDOFF 0x80` at `blitter.cpp:52` and the flag consumed at `blitter.cpp:470`.

**vAmiga does not.** `Blitter/BlitterTypes.h:33` defines the constant:

    constexpr u16 BLTCON1_DOFF    = 0x0080;

and a search of the whole `Core/` tree finds **exactly one occurrence - that
definition**. The name exists; nothing reads it. That is the position NanoMig is
in, arrived at independently.

**What a fix touches.** Line mode's D-channel store is `BLT_L4`
(`agnus_blitter.v:851-869`), where `dma_req = usec & (~bltcon1[1] | ~bltcon1[4] |
first_pixel)`. A DOFF implementation gates the same store on `~(bltcon1[7] & ecs)`.
The SING mechanism in that very expression is the working example of the shape.

**Note for whoever decides.** One mature emulator implements it, one does not.
That is a signal about how much software depends on it, not about what the manual
says. The manual is unambiguous, so the finding stands either way - but the cost
of leaving it out is evidently something a widely used emulator has been willing
to pay.
## NM-HRM-022 — BLTxPTH takes five high pointer bits in both chipsets

| | |
|---|---|
| Manual | Appendix A p.270 "high 3 bits, 5 bits if ECS"; Appendix C p.309 "high 5 bits, was 3 bits" |
| NanoMig | `agnus_blitter_adrgen.v:62` takes `data_in[4:0]` unconditionally. That module has **no `ecs` port at all**, and `agnus_blitter.v:588-592` passes it none |
| Measured | `ecs=0` and `ecs=1` both give `address_out = 0xF8000` from `BLTAPTH=$001F` |
| Test | `hrm/ch6-blitter-ecs`, `BLTXPTH-WIDTH-REGARDLESS-OF-ECS` (red) |

**This is finding 4 in a second engine.** The Copper does the same thing from the
same bits with no chipset condition. Two findings, one design decision.

**WinUAE** stores the full high word at the register and never narrows it there -
`custom.cpp:3800`:

    static void BLTAPTH(uae_u16 v)
    {
        blt_info.bltapt = (blt_info.bltapt & 0xffff) | ((uae_u32)v << 16);
    }

**vAmiga** does the same at the register - `BlitterRegs.cpp:157` - but **narrows on
access**. `ptrMask` is chipset-dependent, `Agnus.cpp:191-194`:

    case AgnusRevision::OCS:     ptrMask = 0x07FFFF; break;
    case AgnusRevision::ECS_1MB: ptrMask = 0x0FFFFF; break;
    case AgnusRevision::ECS_2MB: ptrMask = 0x1FFFFF; break;
    case AgnusRevision::AGA:     ptrMask = 0x1FFFFF; break;

and it is enforced in the memory path - `Memory.cpp:1623`, `:1639`, `:2159`:

    addr &= agnus.ptrMask;

with `assert((addr & agnus.ptrMask) == addr)` at `:1579`, `:1593`, `:1601` and
`:2141` holding the invariant elsewhere.

**The architectural point, and it is the useful one.** Both emulators keep the
written value wide and constrain the *address* rather than the *register*. NanoMig
constrains neither. A fix therefore has two shapes and they are not equivalent:

- narrow at the register, which makes a written-then-read pointer differ between
  chipsets; or
- narrow at the address, the emulators' shape, which leaves the register wide and
  masks what reaches memory.

Which is right depends on what real silicon does with a written-then-read pointer,
and that is exactly the question this suite cannot answer.

---

# Chapter 2 - Copper

## NM-HRM-003 — OCS Copper address uses five high bits - the same decision as NM-HRM-022

| | |
|---|---|
| Manual | Appendix A p.273 "(high 3 bits, high 5 bits if ECS)"; p.276 "These are 18-bit pointer registers"; Appendix C p.309 "COP1LCH ... (high 5 bits, was 3 bits)" |
| NanoMig | `agnus_copper.v:144` `cop1lch[20:16] <= data_in[4:0];` then `:197` `address_out[20:1] <= {cop1lch[20:16], cop1lcl[15:1]}` - unconditional |
| Measured | five high bits in both chipsets; a 20-bit pointer |
| Tests | `hrm/ch2-copper-control` `T4`; `hrm/ch2-copper-locreg-width` `LW1`, `LW3`, `LW4` (all red) |

`LW3` is the assertion that matters: the appendix gives *two different* widths for
the two chipsets, so a module reporting the same width in both contradicts the
manual whatever that width is.

**WinUAE** - `custom.cpp:2888`, the same shape as its blitter pointer:

    STATIC_INLINE void COP1LCH(uae_u16 v)
    {
        cop1lc = (cop1lc & 0xffff) | ((uae_u32)v << 16);
    }

**vAmiga** - `Copper/CopperRegs.cpp:82` `cop1lc = REPLACE_HI_WORD(cop1lc, value);`,
with the same chipset-dependent narrowing on access as in finding 3.

**Decide findings 3 and 4 together.** They are one question asked in two engines,
and both emulators answer it the same way in both. Fixing one and not the other
would leave Agnus internally inconsistent in a way neither the manual nor either
emulator is.

## NM-HRM-004 — ECS with CDANG=0 rejects 32 addresses the appendix allows

**The one finding where the emulators disagree with the manual as well - and with
NanoMig, in the other direction.**

| | |
|---|---|
| Manual | Appendix C p.310: with ECS and CDANG=0 the writable range equals the OCS CDANG=1 range, `$20`-`$FF` |
| NanoMig | `$40`-`$FF`; 32 addresses `$20`-`$3F` rejected, none falsely accepted |
| Tests | `hrm/ch2-copper-move-range` `T9`; `hrm/ch2-copper-move-sweep` (red) |

**WinUAE** - `custom.cpp:2806`:

    static int test_copper_dangerous(uae_u16 reg, bool testonly)
    {
        int addr = reg & 0x01fe;
        if (addr < ((copcon & 2) ? (ecs_agnus ? 0 : 0x40) : 0x80)) {
            ...  // rejected

**vAmiga** - `Copper/Copper.cpp:395`, the identical rule written differently:

    bool Copper::isIllegalAddress(u32 addr) const
    {
        if (cdang) {
            return agnus.isOCS() ? addr < 0x40 : false;
        } else {
            return addr < 0x80;
        }
    }

So all three sources give three different answers for **ECS with CDANG=0**:

| source | lowest writable address |
|---|---|
| Appendix C p.310, as read | `$20` |
| NanoMig | `$40` |
| WinUAE **and** vAmiga | `$80` |

Both emulators apply **no ECS relaxation at all while CDANG is clear** - the
threshold is `$80` on every chipset - and they relax only for CDANG=1 on ECS.
NanoMig sits between the manual and the emulators.

**This is the one place the emulator evidence may bear on the oracle rather than
on the fix**, because two independent implementations reading the same appendix
both concluded something the appendix does not appear to say. Before changing RTL,
re-read p.310 against the printed scan and ask whether the sentence means what the
inventory took it to mean. `third-witness.md` rule 1 admits an emulator for
exactly that question.

## NM-HRM-005 — WAIT and SKIP take longer than the manual states - **compared; needs hardware**

| | |
|---|---|
| Manual | p.20 and Appendix A p.275: a SKIP spans four memory cycle times, a satisfied WAIT six; all instructions except WAIT take two bus cycles and two instruction words |
| NanoMig | SKIP spans 8 memory cycle times, a satisfied WAIT 8 instead of 6 |
| Tests | `hrm/ch2-copper-cycle-times` `P3`-`P6`, `AC5`, `AC6`; `hrm/ch2-copper-cycles` `C5`, `C6` (red) |

`AC6` is the sharpest: for a SKIP, memory cycle times should be exactly twice the
granted bus cycles, and the appendix states that conversion rather than only the
numbers.

**No emulator comparison is recorded here, deliberately.** Copper instruction
timing in both emulators is a cycle-accurate state machine spread across many
branches - WinUAE's `cop_state` machine in `custom.cpp`, vAmiga's
`Copper/CopperEvents.cpp`. Establishing what either does with a SKIP takes a
focused reading, not a search, and a shallow answer would be worse than none: a
timing claim that looks sourced is harder to distrust than one that is plainly
absent.

**What it would take:** trace one satisfied WAIT and one SKIP through each state
machine, counting the DMA slots consumed, and compare against the same two
instructions in `hrm/ch2-copper-cycle-times`. That is a session's work, and it
is the obvious next piece if this is the finding you want to act on first.

---

# Summary

| Key | Finding | Manual | NanoMig | WinUAE | vAmiga |
|---|---|---|---|---|---|
| NM-HRM-020 | `BLTSIZV` ECS gating | ECS-only | **not gated** | gated | gated |
| NM-HRM-021 | `DOFF` line-mode D disable | ECS, disables D | **absent** | implemented | **absent** (constant only) |
| NM-HRM-022 | `BLTxPTH` width | 3 bits OCS / 5 ECS | **5 in both** | wide register | wide register, narrowed on access by chipset |
| NM-HRM-003 | `COP1LCH` width | 3 bits OCS / 5 ECS | **5 in both** | wide register | wide register, narrowed on access by chipset |
| NM-HRM-004 | CDANG=0 lowest address | `$20` on ECS | `$40` | `$80` | `$80` |
| NM-HRM-005 | WAIT/SKIP cycle times | 4 / 6 | 8 / 8 | not compared | **4 / 8 / 8 — NanoMig's own numbers** |

**NM-HRM-022 and NM-HRM-003 are one decision.** NM-HRM-020 and NM-HRM-021 are
independent, and NM-HRM-020 is the cheapest to act on. NM-HRM-004 should start
with a re-reading of the manual rather than with RTL. NM-HRM-005 has since been
compared and does not need more comparison: vAmiga measures exactly NanoMig's
three numbers, so no emulator separates the two readings. What it needs is real
hardware. The maintained statement of that is
[`findings/NM-HRM-005.md`](findings/NM-HRM-005.md), which this section now
defers to.

Every red assertion named here re-runs with `make -C hrm/<package> test` from
`nanomig-hrm-tests`. That builds the **pinned** submodule; to test a tree you
have changed, add `NANOMIG=/path/to/your/NanoMig` — without it the reds below
reproduce unchanged however good the fix. Each carries its page citation in its
own text and a recorded stimulus mutation showing it discriminates.
