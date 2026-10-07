# NM-HRM-020 — BLTSIZV is not gated on ECS, while its two siblings are [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should `BLTSIZV` (`$05C`) load the blitter height only when `ecs` is set, as `BLTSIZH` and `BLTCON0L` do, where the RTL loads height 7 with `ecs = 0` and with `ecs = 1`? The decision belongs to an Amiga specialist, not to this repository. Closes when an Amiga specialist accepts or rejects `&& ecs` on the `BLTSIZV` decode at `agnus_blitter.v:548`. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer; its `rtl/agnus_blitter.v:566-569` carries the same decode, and a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Scope** | Agnus `BLTSIZV` write with the `ecs` input 0 and 1; chip revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 6 — Blitter · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · Appendix C, ECS register map, printed p. 298 · `BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS`: `6-appc-modifications.c12`, `ch6-appc-map.bltsizv`, `6-appa-registers.c70`, `6-appa-registers.c71`, `ch6-appc-bltsizv.c1`, `6-appc-big-blits.c65`, `ch6-appc-bigblits.c8`; the control `BLTCON0L-GATED-BY-ECS`: `6-appa-registers.c25`, `6-appa-registers.c26`, `6-appa-registers.c27`, `ch6-appa-bltcon0l.c3`, `ch6-appc-map.bltcon0l`, `6-appc-modifications.c11`, `ch6-appc-ecs.c9`; both assertions carry claim ids |
| **Module** | `agnus_blitter.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter.v#L546-L549) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L566-L569); inherited |
| **Test** | [`hrm/ch6-blitter-ecs`](../../hrm/ch6-blitter-ecs): `make -C hrm/ch6-blitter-ecs test`; an expected red exits nonzero |
| **Assertions** | 1 red, `BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS`: expected the height loaded with `ecs = 0` to differ from the one loaded with `ecs = 1`, observed 7 and 7; the package reports `12 passed, 4 failed`, the other three reds being other findings |
| **Verdict of the four** | **all four gate it on ECS. NanoMig alone does not.** Hardware status: not measured |
| **Effort** | S — one `&& ecs` on the `BLTSIZV` decode at `agnus_blitter.v:548` |

## Source requirement

**Appendix C's ECS register map, printed p. 298**, marks `BLTSIZV $05C` **new for
ECS** — the same status it gives `BLTSIZH $05E` and `BLTCON0L $05A`.

## RTL and measurement

NanoMig gates `BLTCON0L` on `ecs`
([`agnus_blitter.v#L257-L260`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter.v#L257-L260))
and does not gate `BLTSIZV`, and **the line carries the contradiction in its own
comment**, which calls the register ECS while the code accepts it
unconditionally
([`agnus_blitter.v#L546-L549`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/agnus_blitter.v#L546-L549)):

```verilog
  	else if (reg_address_in[8:1]==BLTSIZV[8:1]) // ECS BLTSIZV register
  		height[14:0] <= data_in[14:0];
```

| | height loaded |
|---|---|
| `BLTSIZV` write with `ecs = 0` | 7 |
| `BLTSIZV` write with `ecs = 1` | 7 |

Identical, so `BLTSIZV-LOADS-HEIGHT-REGARDLESS-OF-ECS` is red. An OCS Agnus
should ignore the write entirely. `BLTCON0L-GATED-BY-ECS` is green on the same
fixture, so the `ecs` input demonstrably changes behaviour elsewhere in the same
module; the red is `BLTSIZV` ignoring it, not a dead `ecs` signal.

## Risks and unknowns

- Real Agnus hardware is not measured, and no named software depends on it.
- The expectation that an OCS Agnus ignores the write comes from the "new for
  ECS" marking, not from a printed sentence saying so.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`custom.cpp:3915-3921`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L3915-L3921) | gates all three on `ecs_agnus` |
| **vAmiga** | [`Core/Components/Agnus/Blitter/BlitterRegs.cpp:324-332`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Blitter/BlitterRegs.cpp#L324-L332) | gates all three, returning on `agnus.isOCS()` |
| **Denise** | [`emulation/libami/agnus/register.cpp:222-224`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/register.cpp#L222-L224) | gates on `ecsAndHigher()` |
| **Copperline** | [`src/bus/custom_regs.rs:787-792`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/bus/custom_regs.rs#L787-L792) | returns false unless the blitter's ECS registers are enabled |
| **MiSTer** | [`rtl/agnus_blitter.v:566-569`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/agnus_blitter.v#L566-L569) | descendant — identical to NanoMig, no `&& ecs`, the same comment/code contradiction |
| **TC64** | [`rtl/minimig/agnus_blitter.v:546-549`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/agnus_blitter.v#L546-L549) | descendant — the same four lines at the same line numbers; its whitespace-only changes in this file do not touch this decode; a fix here diverges from neither line |

## Decision

Add `&& ecs`, as the `BLTSIZH` decodes in the same file at lines 523 and 557 do:

```verilog
  	else if (reg_address_in[8:1]==BLTSIZV[8:1] && ecs) // ECS BLTSIZV register
  		height[14:0] <= data_in[14:0];
```

That matches `BLTCON0L` and `BLTSIZH` in the same file and the appendix page that
gives all three the same status. The finding closes when an Amiga specialist accepts or rejects it.
