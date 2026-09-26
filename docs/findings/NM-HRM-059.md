# NM-HRM-059 — ADKCONR's bit 15 always reads 0, and so does everyone's

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | No RTL change follows from this. The printed page is the likely defect, and the entry exists so the next reader does not take the red for an implementation fault. The red is against p.265, not against the RTL. |
| **Module** | `paula.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula.v#L132) |
| **Report to** | **the manual.** All four reference implementations produce the same observable result |
| **Test** | [`hrm/ch8-adkcon-and-interrupt-relations`](../../hrm/ch8-adkcon-and-interrupt-relations) `ADKCONR-BIT15-SETCLR-NEVER-READS-BACK` |
| **Chapter** | 8 · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red, with an **oracle** mutation — the defect is in the value, not the stimulus |
| **Verdict of the four** | **unanimous with NanoMig.** All four read bit 15 as 0; only WinUAE guarantees it the same way |

---

## What the HRM says

**p.265** lists ADKCON bit 15 as SET/CLR, the bit that decides whether the other
bits written with a 1 are set or cleared, and lists ADKCONR as the read
companion of the same table. Every other row of that table reads back.

## What NanoMig does

    reg [14:0] adkcon;                                         // paula.v:132
    assign adkconr = ... ? {1'b0, adkcon[14:0]} : 16'h0000;    // paula.v:198

Fifteen bits stored, a literal zero in bit 15 on every read. The register cannot
hold the bit, so the question of whether a write-only control bit *should* read
back never arises in this design.

The assertion is graded with an **oracle** mutation rather than a stimulus one,
because no stimulus can change the answer: the zero is in the RTL's own value.

### Measured

[`hrm/ch8-adkcon-and-interrupt-relations`](../../hrm/ch8-adkcon-and-interrupt-relations)
writes ADKCON bit 15 and reads ADKCONR back: `0x7FFF`, bit 15 clear
regardless of what was written — `ADKCONR-BIT15-SETCLR-NEVER-READS-BACK`,
red, one of the package's `17 passed, 2 failed`.

## Why this is filed against the page

| | | |
|---|---|---|
| **WinUAE** | bit 15 always 0 | masked **twice** — in `setclr` (`custom.cpp:835`, called at `:3321`) and again at the read, `return adkcon & 0x7fff;` (`:2270`) |
| **vAmiga** | bit 15 always 0 in practice | read unmasked (`Core/Components/Paula/PaulaRegs.cpp:22`), write masks with `value & 0x7FFF` (`:47`) |
| **Denise** | bit 15 always 0 in practice | read unmasked (`emulation/libami/paula/paula.cpp:80`), write masks with `value & 0x7fff` (`:87`) |
| **Copperline** | bit 15 always 0 in practice | read unmasked (`src/bus/custom_regs.rs:41`), write masks (`src/chipset/paula.rs:1182-1184`) |
| **MiSTer** | bit 15 always 0 | `rtl/paula.v:127` (register) and `:193` (`adkconr` assign) — descendant, carries the identical lines |
| **TC64** | bit 15 always 0 | `rtl/minimig/paula.v:146` and `:212` — descendant, carries them too: `reg [14:0] adkcon;` and `assign adkconr[15:0] = (reg_address_in[8:1]==ADKCONR[8:1]) ? {1'b0,adkcon[14:0]} : 16'h0000;`, word for word, even though `paula.v` as a whole diverges by 52 (MiSTer) and 193 (TC64) lines elsewhere. The fifteen-bit register and the forced-zero bit 15 on read are inherited unchanged by both lines; the red is against p.265, not against any of the three RTL trees |

**All four store sixteen bits and none stores fifteen**, so NanoMig's narrower
register is its own; but the observable result — ADKCONR bit 15 reads 0 despite
p.265 listing it — is what every one of them produces.

One structural difference the survey drew out and it is worth keeping. Only
WinUAE masks on the **read**, so its zero survives any path that assigns
`adkcon` directly. Savestate restore is exactly such a path: WinUAE's
`custom.cpp:7768` `adkcon = RW;` is unmasked and its read-side mask is what saves
it. The other three restore the same unmasked way, so in those three a savestate
carrying bit 15 set would read back out of ADKCONR. **Under normal register
traffic the four are indistinguishable; under a crafted savestate three of them
are not.** NanoMig, storing fifteen bits, cannot be put in that state at all.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: US patent 4,780,844, Commodore-Amiga, filed 1986, granted 1988; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023; techtravels, *Inside Paula's Floppy Controller*, part I, 2026.

