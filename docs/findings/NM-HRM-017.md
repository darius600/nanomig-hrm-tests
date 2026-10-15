# NM-HRM-017 — SuperHires bitplane colour encoding is not implemented [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Should NanoMig's Denise, in SuperHires with one or two bitplanes, show each pixel with two bits per component from the sixteen lower colour registers as p. 301's table encodes them (a solid colour-1 run reading COLOR05, not COLOR01 in full)? Closes when the colour table takes a SuperHires mode input and the depth lines named under *Effort* let a one- or two-bitplane address reach COLOR04–COLOR0F, turning `SHRES-REINTERPRETS-SAME-REGISTERS`, `SHRES-BITPLANE-TABLE-COLOR-01`, `SHRES-BITPLANE-TABLE-COLOR-02`, `SHRES-BITPLANE-TABLE-COLOR-03`, `SHRES-BITPLANE-TABLE-COLOR-05`, `SHRES-BITPLANE-TABLE-COLOR-0A`, `SHRES-BITPLANE-TABLE-COLOR-0F`, `SHRES-USES-SIXTEEN-LOWER-REGISTERS` and `SHRES-TWO-BITS-RESOLUTION` green, or when an Amiga specialist rejects the change; if the rejection means p. 301 is wrong, this page is refiled against the printed page. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | NanoMig's Denise colour table and bitplane path in SuperHires at BPU = 2, COLOR00–COLOR0F; isolated through solid runs of value 0 to 3 and 18 one-bit changes to colours 0, 2 and 3; two mixed runs are driven, not graded; the Denise revision the page describes is `unknown` |
| **Software impact** | `unknown; no named affected software` |
| **Chapter** | Appendix C, printed pp. 297, 300 and 301 (the colour table on p. 301) · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims by assertion: <details><summary>claim ids by assertion (349)</summary> `OCS-BASELINE-0RGB-FORMAT` (p. 301) `sol:C-shres-color.c4`, `sol:C-shres-color.c5`, `sol:C-shres-color.c6`, `sol:C-shres-color.c7`; `SHRES-REINTERPRETS-SAME-REGISTERS` (p. 297) `opus:C-shr.c06`, `sol:C-superhires-color.c1`; `SHRES-BITPLANE-TABLE-COLOR-00` (p. 301) `opus:C-shrc.r00`, `sol:C-shres-bpl-bits.c5`, `sol:C-shres-bpl-bits.c6`, `sol:C-shres-bpl-bits.c7`, `sol:C-shres-bpl-bits.c8`, `sol:C-shres-bpl-bits.c9`, `sol:C-shres-bpl-bits.c10`, `sol:C-shres-bpl-bits.c11`, `sol:C-shres-bpl-bits.c12`, `sol:C-shres-bpl-bits.c13`, `sol:C-shres-bpl-bits.c14`, `sol:C-shres-bpl-bits.c15`, `sol:C-shres-bpl-bits.c16`, `sol:C-shres-bpl-bits.c1`, `sol:C-shres-bpl-bits.c2`, `sol:C-shres-bpl-bits.c3`, `sol:C-shres-bpl-bits.c4`, `opus:C-shrc.srcb0`, `sol:C-shres-symbols.c1`, `sol:C-shres-symbols.c2`, `sol:C-shres-symbols.c3`; `SHRES-BITPLANE-TABLE-COLOR-01` (p. 301) `opus:C-shrc.r01`, `sol:C-shres-bpl-bits.c21`, `sol:C-shres-bpl-bits.c22`, `sol:C-shres-bpl-bits.c23`, `sol:C-shres-bpl-bits.c24`, `sol:C-shres-bpl-bits.c25`, `sol:C-shres-bpl-bits.c26`, `sol:C-shres-bpl-bits.c27`, `sol:C-shres-bpl-bits.c28`, `sol:C-shres-bpl-bits.c29`, `sol:C-shres-bpl-bits.c30`, `sol:C-shres-bpl-bits.c31`, `sol:C-shres-bpl-bits.c32`, `sol:C-shres-bpl-bits.c17`, `sol:C-shres-bpl-bits.c18`, `sol:C-shres-bpl-bits.c19`, `sol:C-shres-bpl-bits.c20`; `SHRES-BITPLANE-TABLE-COLOR-02` (p. 301) `opus:C-shrc.r02`, `sol:C-shres-bpl-bits.c37`, `sol:C-shres-bpl-bits.c38`, `sol:C-shres-bpl-bits.c39`, `sol:C-shres-bpl-bits.c40`, `sol:C-shres-bpl-bits.c41`, `sol:C-shres-bpl-bits.c42`, `sol:C-shres-bpl-bits.c43`, `sol:C-shres-bpl-bits.c44`, `sol:C-shres-bpl-bits.c45`, `sol:C-shres-bpl-bits.c46`, `sol:C-shres-bpl-bits.c47`, `sol:C-shres-bpl-bits.c48`, `sol:C-shres-bpl-bits.c33`, `sol:C-shres-bpl-bits.c34`, `sol:C-shres-bpl-bits.c35`, `sol:C-shres-bpl-bits.c36`; `SHRES-BITPLANE-TABLE-COLOR-03` (p. 301) `opus:C-shrc.r03`, `sol:C-shres-bpl-bits.c53`, `sol:C-shres-bpl-bits.c54`, `sol:C-shres-bpl-bits.c55`, `sol:C-shres-bpl-bits.c56`, `sol:C-shres-bpl-bits.c57`, `sol:C-shres-bpl-bits.c58`, `sol:C-shres-bpl-bits.c59`, `sol:C-shres-bpl-bits.c60`, `sol:C-shres-bpl-bits.c61`, `sol:C-shres-bpl-bits.c62`, `sol:C-shres-bpl-bits.c63`, `sol:C-shres-bpl-bits.c64`, `sol:C-shres-bpl-bits.c49`, `sol:C-shres-bpl-bits.c50`, `sol:C-shres-bpl-bits.c51`, `sol:C-shres-bpl-bits.c52`; `SHRES-BITPLANE-TABLE-COLOR-04` (p. 301) `opus:C-shrc.r04`, `sol:C-shres-bpl-bits.c133`, `sol:C-shres-bpl-bits.c134`, `sol:C-shres-bpl-bits.c135`, `sol:C-shres-bpl-bits.c136`, `sol:C-shres-bpl-bits.c137`, `sol:C-shres-bpl-bits.c138`, `sol:C-shres-bpl-bits.c139`, `sol:C-shres-bpl-bits.c140`, `sol:C-shres-bpl-bits.c141`, `sol:C-shres-bpl-bits.c142`, `sol:C-shres-bpl-bits.c143`, `sol:C-shres-bpl-bits.c144`, `sol:C-shres-bpl-bits.c129`, `sol:C-shres-bpl-bits.c130`, `sol:C-shres-bpl-bits.c131`, `sol:C-shres-bpl-bits.c132`; `SHRES-BITPLANE-TABLE-COLOR-05` (p. 301) `opus:C-shrc.r05`, `sol:C-shres-bpl-bits.c149`, `sol:C-shres-bpl-bits.c150`, `sol:C-shres-bpl-bits.c151`, `sol:C-shres-bpl-bits.c152`, `sol:C-shres-bpl-bits.c153`, `sol:C-shres-bpl-bits.c154`, `sol:C-shres-bpl-bits.c155`, `sol:C-shres-bpl-bits.c156`, `sol:C-shres-bpl-bits.c157`, `sol:C-shres-bpl-bits.c158`, `sol:C-shres-bpl-bits.c159`, `sol:C-shres-bpl-bits.c160`, `sol:C-shres-bpl-bits.c145`, `sol:C-shres-bpl-bits.c146`, `sol:C-shres-bpl-bits.c147`, `sol:C-shres-bpl-bits.c148`, `opus:C-shrc.srcb1`, `sol:C-shres-symbols.c4`, `sol:C-shres-symbols.c5`, `sol:C-shres-symbols.c6`; `SHRES-BITPLANE-TABLE-COLOR-06` (p. 301) `opus:C-shrc.r06`, `sol:C-shres-bpl-bits.c165`, `sol:C-shres-bpl-bits.c166`, `sol:C-shres-bpl-bits.c167`, `sol:C-shres-bpl-bits.c168`, `sol:C-shres-bpl-bits.c169`, `sol:C-shres-bpl-bits.c170`, `sol:C-shres-bpl-bits.c171`, `sol:C-shres-bpl-bits.c172`, `sol:C-shres-bpl-bits.c173`, `sol:C-shres-bpl-bits.c174`, `sol:C-shres-bpl-bits.c175`, `sol:C-shres-bpl-bits.c176`, `sol:C-shres-bpl-bits.c161`, `sol:C-shres-bpl-bits.c162`, `sol:C-shres-bpl-bits.c163`, `sol:C-shres-bpl-bits.c164`; `SHRES-BITPLANE-TABLE-COLOR-07` (p. 301) `opus:C-shrc.r07`, `sol:C-shres-bpl-bits.c181`, `sol:C-shres-bpl-bits.c182`, `sol:C-shres-bpl-bits.c183`, `sol:C-shres-bpl-bits.c184`, `sol:C-shres-bpl-bits.c185`, `sol:C-shres-bpl-bits.c186`, `sol:C-shres-bpl-bits.c187`, `sol:C-shres-bpl-bits.c188`, `sol:C-shres-bpl-bits.c189`, `sol:C-shres-bpl-bits.c190`, `sol:C-shres-bpl-bits.c191`, `sol:C-shres-bpl-bits.c192`, `sol:C-shres-bpl-bits.c177`, `sol:C-shres-bpl-bits.c178`, `sol:C-shres-bpl-bits.c179`, `sol:C-shres-bpl-bits.c180`; `SHRES-BITPLANE-TABLE-COLOR-08` (p. 301) `opus:C-shrc.r08`, `sol:C-shres-bpl-bits.c69`, `sol:C-shres-bpl-bits.c70`, `sol:C-shres-bpl-bits.c71`, `sol:C-shres-bpl-bits.c72`, `sol:C-shres-bpl-bits.c73`, `sol:C-shres-bpl-bits.c74`, `sol:C-shres-bpl-bits.c75`, `sol:C-shres-bpl-bits.c76`, `sol:C-shres-bpl-bits.c77`, `sol:C-shres-bpl-bits.c78`, `sol:C-shres-bpl-bits.c79`, `sol:C-shres-bpl-bits.c80`, `sol:C-shres-bpl-bits.c65`, `sol:C-shres-bpl-bits.c66`, `sol:C-shres-bpl-bits.c67`, `sol:C-shres-bpl-bits.c68`; `SHRES-BITPLANE-TABLE-COLOR-09` (p. 301) `opus:C-shrc.r09`, `sol:C-shres-bpl-bits.c197`, `sol:C-shres-bpl-bits.c198`, `sol:C-shres-bpl-bits.c199`, `sol:C-shres-bpl-bits.c200`, `sol:C-shres-bpl-bits.c201`, `sol:C-shres-bpl-bits.c202`, `sol:C-shres-bpl-bits.c203`, `sol:C-shres-bpl-bits.c204`, `sol:C-shres-bpl-bits.c205`, `sol:C-shres-bpl-bits.c206`, `sol:C-shres-bpl-bits.c207`, `sol:C-shres-bpl-bits.c208`, `sol:C-shres-bpl-bits.c193`, `sol:C-shres-bpl-bits.c194`, `sol:C-shres-bpl-bits.c195`, `sol:C-shres-bpl-bits.c196`; `SHRES-BITPLANE-TABLE-COLOR-0A` (p. 301) `opus:C-shrc.r0A`, `sol:C-shres-bpl-bits.c85`, `sol:C-shres-bpl-bits.c86`, `sol:C-shres-bpl-bits.c87`, `sol:C-shres-bpl-bits.c88`, `sol:C-shres-bpl-bits.c89`, `sol:C-shres-bpl-bits.c90`, `sol:C-shres-bpl-bits.c91`, `sol:C-shres-bpl-bits.c92`, `sol:C-shres-bpl-bits.c93`, `sol:C-shres-bpl-bits.c94`, `sol:C-shres-bpl-bits.c95`, `sol:C-shres-bpl-bits.c96`, `sol:C-shres-bpl-bits.c81`, `sol:C-shres-bpl-bits.c82`, `sol:C-shres-bpl-bits.c83`, `sol:C-shres-bpl-bits.c84`, `opus:C-shrc.srcb2`, `sol:C-shres-symbols.c7`, `sol:C-shres-symbols.c8`, `sol:C-shres-symbols.c9`; `SHRES-BITPLANE-TABLE-COLOR-0B` (p. 301) `opus:C-shrc.r0B`, `sol:C-shres-bpl-bits.c101`, `sol:C-shres-bpl-bits.c102`, `sol:C-shres-bpl-bits.c103`, `sol:C-shres-bpl-bits.c104`, `sol:C-shres-bpl-bits.c105`, `sol:C-shres-bpl-bits.c106`, `sol:C-shres-bpl-bits.c107`, `sol:C-shres-bpl-bits.c108`, `sol:C-shres-bpl-bits.c109`, `sol:C-shres-bpl-bits.c110`, `sol:C-shres-bpl-bits.c111`, `sol:C-shres-bpl-bits.c112`, `sol:C-shres-bpl-bits.c97`, `sol:C-shres-bpl-bits.c98`, `sol:C-shres-bpl-bits.c99`, `sol:C-shres-bpl-bits.c100`; `SHRES-BITPLANE-TABLE-COLOR-0C` (p. 301) `opus:C-shrc.r0C`, `sol:C-shres-bpl-bits.c213`, `sol:C-shres-bpl-bits.c214`, `sol:C-shres-bpl-bits.c215`, `sol:C-shres-bpl-bits.c216`, `sol:C-shres-bpl-bits.c217`, `sol:C-shres-bpl-bits.c218`, `sol:C-shres-bpl-bits.c219`, `sol:C-shres-bpl-bits.c220`, `sol:C-shres-bpl-bits.c221`, `sol:C-shres-bpl-bits.c222`, `sol:C-shres-bpl-bits.c223`, `sol:C-shres-bpl-bits.c224`, `sol:C-shres-bpl-bits.c209`, `sol:C-shres-bpl-bits.c210`, `sol:C-shres-bpl-bits.c211`, `sol:C-shres-bpl-bits.c212`; `SHRES-BITPLANE-TABLE-COLOR-0D` (p. 301) `opus:C-shrc.r0D`, `sol:C-shres-bpl-bits.c229`, `sol:C-shres-bpl-bits.c230`, `sol:C-shres-bpl-bits.c231`, `sol:C-shres-bpl-bits.c232`, `sol:C-shres-bpl-bits.c233`, `sol:C-shres-bpl-bits.c234`, `sol:C-shres-bpl-bits.c235`, `sol:C-shres-bpl-bits.c236`, `sol:C-shres-bpl-bits.c237`, `sol:C-shres-bpl-bits.c238`, `sol:C-shres-bpl-bits.c239`, `sol:C-shres-bpl-bits.c240`, `sol:C-shres-bpl-bits.c225`, `sol:C-shres-bpl-bits.c226`, `sol:C-shres-bpl-bits.c227`, `sol:C-shres-bpl-bits.c228`; `SHRES-BITPLANE-TABLE-COLOR-0E` (p. 301) `opus:C-shrc.r0E`, `sol:C-shres-bpl-bits.c117`, `sol:C-shres-bpl-bits.c118`, `sol:C-shres-bpl-bits.c119`, `sol:C-shres-bpl-bits.c120`, `sol:C-shres-bpl-bits.c121`, `sol:C-shres-bpl-bits.c122`, `sol:C-shres-bpl-bits.c123`, `sol:C-shres-bpl-bits.c124`, `sol:C-shres-bpl-bits.c125`, `sol:C-shres-bpl-bits.c126`, `sol:C-shres-bpl-bits.c127`, `sol:C-shres-bpl-bits.c128`, `sol:C-shres-bpl-bits.c113`, `sol:C-shres-bpl-bits.c114`, `sol:C-shres-bpl-bits.c115`, `sol:C-shres-bpl-bits.c116`; `SHRES-BITPLANE-TABLE-COLOR-0F` (p. 301) `opus:C-shrc.r0F`, `sol:C-shres-bpl-bits.c245`, `sol:C-shres-bpl-bits.c246`, `sol:C-shres-bpl-bits.c247`, `sol:C-shres-bpl-bits.c248`, `sol:C-shres-bpl-bits.c249`, `sol:C-shres-bpl-bits.c250`, `sol:C-shres-bpl-bits.c251`, `sol:C-shres-bpl-bits.c252`, `sol:C-shres-bpl-bits.c253`, `sol:C-shres-bpl-bits.c254`, `sol:C-shres-bpl-bits.c255`, `sol:C-shres-bpl-bits.c256`, `sol:C-shres-bpl-bits.c241`, `sol:C-shres-bpl-bits.c242`, `sol:C-shres-bpl-bits.c243`, `sol:C-shres-bpl-bits.c244`, `opus:C-shrc.srcb3`, `sol:C-shres-symbols.c10`, `sol:C-shres-symbols.c11`, `sol:C-shres-symbols.c12`; `SHRES-USES-SIXTEEN-LOWER-REGISTERS` (p. 301) `opus:C-shrc.c04`, `opus:C-shrc.c05`; `SHRES-TWO-BITS-RESOLUTION` (p. 301) `opus:C-shrc.c01`, `opus:C-shrc.c02`; `P2.R` (p. 301) `APPC-shr.c3`, `CH4-appC-context.c3`; `P2.G` (p. 301) `APPC-shr.c4`, `CH4-appC-context.c4`; `P2.B` (p. 301) `APPC-shr.c5`, `CH4-appC-context.c5`; `P4.n1b0` (p. 301) `APPC-bpltab.r01b00`; `P4.n1b1` (p. 301) `APPC-bpltab.r01b01`; `P4.n1b4` (p. 301) `APPC-bpltab.r01b04`; `P4.n1b5` (p. 301) `APPC-bpltab.r01b05`; `P4.n1b8` (p. 301) `APPC-bpltab.r01b08`; `P4.n1b9` (p. 301) `APPC-bpltab.r01b09`; `P4.n2b0` (p. 301) `APPC-bpltab.r02b00`; `P4.n2b1` (p. 301) `APPC-bpltab.r02b01`; `P4.n2b4` (p. 301) `APPC-bpltab.r02b04`; `P4.n2b5` (p. 301) `APPC-bpltab.r02b05`; `P4.n2b8` (p. 301) `APPC-bpltab.r02b08`; `P4.n2b9` (p. 301) `APPC-bpltab.r02b09`; `P4.n3b0` (p. 301) `APPC-bpltab.r03b00`; `P4.n3b1` (p. 301) `APPC-bpltab.r03b01`; `P4.n3b4` (p. 301) `APPC-bpltab.r03b04`; `P4.n3b5` (p. 301) `APPC-bpltab.r03b05`; `P4.n3b8` (p. 301) `APPC-bpltab.r03b08`; `P4.n3b9` (p. 301) `APPC-bpltab.r03b09`; `P5.L` (p. 301) `CH4-appC-context.c7`; `P7.2` (p. 301) `APPC-shr.c6`; `P7.3` (p. 301) `APPC-shr.c8`; `P4.n0b2` (p. 301) `APPC-bpltab.r00b02`; `P4.n0b3` (p. 301) `APPC-bpltab.r00b03`; `P4.n0b6` (p. 301) `APPC-bpltab.r00b06`; `P4.n0b7` (p. 301) `APPC-bpltab.r00b07`; `P4.n0b10` (p. 301) `APPC-bpltab.r00b10`; `P4.n0b11` (p. 301) `APPC-bpltab.r00b11`; `P4.n1b2` (p. 301) `APPC-bpltab.r01b02`; `P4.n1b3` (p. 301) `APPC-bpltab.r01b03`; `P4.n1b6` (p. 301) `APPC-bpltab.r01b06`; `P4.n1b7` (p. 301) `APPC-bpltab.r01b07`; `P4.n1b10` (p. 301) `APPC-bpltab.r01b10`; `P4.n1b11` (p. 301) `APPC-bpltab.r01b11`; `P4.n2b2` (p. 301) `APPC-bpltab.r02b02`; `P4.n2b3` (p. 301) `APPC-bpltab.r02b03`; `P4.n2b6` (p. 301) `APPC-bpltab.r02b06`; `P4.n2b7` (p. 301) `APPC-bpltab.r02b07`; `P4.n2b10` (p. 301) `APPC-bpltab.r02b10`; `P4.n2b11` (p. 301) `APPC-bpltab.r02b11`; `P4.n3b2` (p. 301) `APPC-bpltab.r03b02`; `P4.n3b3` (p. 301) `APPC-bpltab.r03b03`; `P4.n3b6` (p. 301) `APPC-bpltab.r03b06`; `P4.n3b7` (p. 301) `APPC-bpltab.r03b07`; `P4.n3b10` (p. 301) `APPC-bpltab.r03b10`; `P4.n3b11` (p. 301) `APPC-bpltab.r03b11` </details>. `CONTROL-KNOWN-VALUES-ARRIVE`, `FIXTURE-DATA-PATH-LIVE`, `SELECT-REGISTER-ADDRESSES-CORRECTLY`, the twelve `P1` lines and `P5.U` carry no claim id, and no assertion message cites p. 300 |
| **Module** | `denise_colortable.v`, `denise.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v); inherited (MiSTer and TC64 have the same colour-table code; their `denise.v` adds a `window_ena &&` term to the depth line) |
| **Test** | [`hrm/ch3-shres-bitplane-colour-encoding`](../../hrm/ch3-shres-bitplane-colour-encoding): `make -C hrm/ch3-shres-bitplane-colour-encoding test` — `15 passed, 9 failed`; the nine reds are expected and the run exits nonzero; [`integration/ch4-shres-bitplane`](../../integration/ch4-shres-bitplane): `make -k -C integration/ch4-shres-bitplane test`, the same behaviour at the system boundary on fx68k and on TG68K with an enhanced chip set; an expected red exits nonzero |
| **Assertions** | 9 red: `SHRES-REINTERPRETS-SAME-REGISTERS`; `SHRES-BITPLANE-TABLE-COLOR-01`, `SHRES-BITPLANE-TABLE-COLOR-02`, `SHRES-BITPLANE-TABLE-COLOR-03`, `SHRES-BITPLANE-TABLE-COLOR-05`, `SHRES-BITPLANE-TABLE-COLOR-0A`, `SHRES-BITPLANE-TABLE-COLOR-0F`; `SHRES-USES-SIXTEEN-LOWER-REGISTERS`; `SHRES-TWO-BITS-RESOLUTION`. Expected, for `SHRES-REINTERPRETS-SAME-REGISTERS`: SuperHires reads a solid colour-1 run differently from plain mode (COLOR05, `0x55AAFF` or `0x4488CC`); observed: `0x4499EE`, COLOR01 in full, in both. Each of the nine is green on a lookup that pairs adjacent pixels, and on one that pairs pixels 0/2 and 1/3, at every group alignment; the other eight are in the table under *RTL and measurement*. In `integration/ch4-shres-bitplane`, 24 red on each core: `P2.R`, `P2.G` and `P2.B`; the eighteen `P4` lines of bits 0, 1, 4, 5, 8 and 9 of COLOR01, COLOR02 and COLOR03 (`P4.n1b0`, `P4.n1b1`, `P4.n1b4`, `P4.n1b5`, `P4.n1b8`, `P4.n1b9`, `P4.n2b0`, `P4.n2b1`, `P4.n2b4`, `P4.n2b5`, `P4.n2b8`, `P4.n2b9`, `P4.n3b0`, `P4.n3b1`, `P4.n3b4`, `P4.n3b5`, `P4.n3b8`, `P4.n3b9`); the SuperHires half of `P5.L`; `P7.3`; and `P7.2`, which is red through [NM-HRM-016](NM-HRM-016.md)'s `P6.2` as well (86 passed, 49 failed on fx68k and on TG68K; `P6.2-SHR`, `P6.2` and `P7.4` are [NM-HRM-016](NM-HRM-016.md)'s and `C11` is [NM-HRM-071](NM-HRM-071.md)'s, and 21 further reds of the package are not this finding's). Expected, for `P2.R`: pixels of value c whose Colour c has the same two letters show the same 4-bit red; observed: they do not, over 272,160 graded pixels in 16 groups. The twelve `P1` lines, the 24 `P4` lines of bits 2, 3, 6, 7, 10 and 11 of COLOR00 to COLOR03, the Hires half of `P5.L` and `P5.U` are green in the same run |
| **Verdict of the four** | **two against two**: WinUAE and vAmiga pair SuperHires pixels into the sixteen lower registers and show two bits per component, ECS Denise only; Denise and Copperline show each pixel's own register in full; outside the emulators, the 8373R4 dissection pairs SuperHires pixels into the lower sixteen registers and shows two bits per component, as WinUAE and vAmiga do — see *What other Commodore documents say*. Hardware: not measured |
| **Effort** | `M` — `denise_colortable.v` needs a mode input it does not have, and `denise.v:369-370` must let the address reach COLOR04–COLOR0F at one or two bitplanes |
| **See also** | [NM-HRM-016](NM-HRM-016.md), the sprite half of the same mechanism |

---

## Source requirement

**Printed p. 301** limits a SuperHires pixel to two bits per component and
moves the four bitplane colours into sixteen registers:

> SuperHires mode has a coarser granularity of color control than either Hires
> or Lores modes. ... There are only two bits of red, green and blue color
> resolution per hires pixel.

> In order to decode sprite and bitplane color information in SuperHires mode,
> certain multiplexing occurs in the use of the registers. Instead of 4 bits of
> red, green, and blue for bitplane registers 0-3 stored as 0x0RGB in four color
> registers, SuperHires bitplane colors are specially encoded in the sixteen
> lower color registers:

The key names each colour's two bits per component, and the table places them
in the registers (the key's first two lines and three of the sixteen rows):

```
                              R    G    B
      Bitplane (Color 0) :   ab-- cd-- ef--
      Bitplane (Color 1) :   gh-- ij-- kl--

      BIT  15  14  13  12  11  10  09  08  07  06  05  04  03  02  01  00
   O  01   .   .   .   .   g   h   a   b   i   j   c   d   k   l   e   f
   R  04   .   .   .   .   a   b   g   h   c   d   i   j   e   f   k   l
      05   .   .   .   .   g   h   g   h   i   j   i   j   k   l   k   l
```

Row *n* carries Color (*n* mod 4) in bits 11–10, 7–6 and 3–2 and Color
(*n* div 4) in bits 9–8, 5–4 and 1–0. Row 05 is the one row that holds Color 1
alone; row 01 holds Color 1 in its upper bit pairs and Color 0 in its lower
ones.

**Printed p. 297** calls this a new reading of the same registers:

> There are new color register interpretations as part of the SuperHires mode.

and **printed p. 300** limits the depth it applies to:

> The maximum plane depth for a SuperHires viewport is 2 bitplanes

so the sixteen lower registers hold the colours of one or two bitplanes.

---

## RTL and measurement

### What NanoMig does

Same root cause as [NM-HRM-016](NM-HRM-016.md), reached down a different path:
`denise_colortable.v` has **no `shres` port**
([`denise_colortable.v:7-20`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v#L7-L20)),
so no colour lookup anywhere in the design can depend on the mode. In a single
playfield its address is the plane bits themselves
([`denise_playfields.v:100`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_playfields.v#L100),
`plfdata[7:0] = bpldata[8:1];`), and all twelve bits of the register it reads
reach the output
([`denise_colortable.v:56-58`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_colortable.v#L56-L58)).

At a SuperHires depth the address never reaches COLOR04–COLOR0F:
[`denise.v:369-370`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L369-L370)
hold planes 3 and 4 at zero whenever BPU is 2 or less,

```verilog
assign bpldata[3] = l_bpu > 2 ? bpldata_out[3] : 1'b0;
assign bpldata[4] = l_bpu > 3 ? bpldata_out[4] : 1'b0;
```

so with one or two bitplanes a pixel reads one of COLOR00–COLOR03, as in Hires.
Under `shres` the plane shifter moves on every 28 MHz clock
([`denise_bitplane_shifter.v:206-208`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise_bitplane_shifter.v#L206-L208))
and the colour outputs follow the table on that clock
([`denise.v:518`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/denise.v#L518)),
so one sample per clock is one 35 ns pixel.

### Measured

The package `hrm/ch3-shres-bitplane-colour-encoding` reports `15 passed, 9 failed`. Every stimulus but the four controls' writes the sixteen
lower registers from one palette by p. 301's row formula, two bits per
component for each colour (colour 0: red 0, green 1, blue 2; colour 1: 1, 2, 3;
colour 2: 2, 3, 0; colour 3: 3, 0, 1), sets BPU = 2, and drives *solid runs*:
both planes held at one value *v*. Two lookups show every pixel its own colour
when the registers are written by the table: *adjacent* pixels share one
(WinUAE's), or pixels 0 and 2 and pixels 1 and 3 of each group of four do
(*0/2–1/3*, the 8373R4 reading under *Independent evidence*). Under both, a
solid run of value *v* reads COLOR(5*v*), and each pixel shows two bits per
component. That is all the package grades (`:797-812`): each significant bit of
COLOR(5*v*) must move the value-*v* run and no other; no bit of any other
register, and no bit 15–12 of any register, may move a solid run. Solid runs
are compared tick by tick (`:577-585`). Two mixed runs, planes 1 and 2 loaded
with `0xAAAA`/`0xCCCC` and `0xF0F0`/`0xFF00` (`:571-572`), are driven on every
flip and reported, not graded (`:893-894`). The two lookup columns apply each
lookup, with the two `--` positions filled either by repeating the pair or by
zeros, to the pixel stream the fixture produces; they are derived, not run.

| assertion (`test_denise.cpp`) | NanoMig | adjacent | 0/2–1/3 | what it shows |
|---|---|---|---|---|
| `SHRES-REINTERPRETS-SAME-REGISTERS` (`:785`) | red: a solid run of value 1 shows COLOR01 (`0x49E`) in full, `0x4499EE`, with `shres` = 0 and 1 | green: the run reads COLOR05 (`0x5AF`), shown as `0x55AAFF` or `0x4488CC` depending on the fill, never `0x4499EE` | green, the same | the finding |
| `SHRES-BITPLANE-TABLE-COLOR-01` to `-03`, `-05`, `-0A`, `-0F` (`:897`) | red, 12 of 12 bits each: every significant bit of COLOR01, COLOR02 and COLOR03 moves the run of value 1, 2 or 3; none of COLOR05, COLOR0A or COLOR0F moves any run | green | green | the finding: the value-*v* run moves with COLOR0*v*, which neither lookup gives it, and COLOR(5*v*) does not reach the value-*v* run |
| `SHRES-BITPLANE-TABLE-COLOR-00`, `SHRES-BITPLANE-TABLE-COLOR-04`, `-06` to `-09`, `-0B` to `-0E` (`:897`) | green: COLOR00's bits move the value-0 run alone; no bit of the other nine registers, nor any of the 64 bits 15–12, moves a solid run | green | green | no difference: COLOR00 is COLOR(5·0), the register NanoMig reads too, and no solid run reads the other nine on any of the three |
| `SHRES-USES-SIXTEEN-LOWER-REGISTERS` (`:931`), and its Hires control (`:957`) | red: COLOR0A XORed with `$0FFF` changes none of the four solid runs; the control, the same XOR under plain Hires, changes none either (green) | green: COLOR0A is COLOR(5·2) and moves the value-2 run | green | the finding; the control: outside SuperHires a value-*v* run reads COLOR0*v*, so COLOR0A moves nothing |
| `SHRES-TWO-BITS-RESOLUTION` (`:1026`) | red, 6 of 18: of the one-bit changes to colours 0, 2 and 3, the six to colour 0 move a solid run of value 1 | green, 0 of 18 | green, 0 of 18 | the finding: NanoMig shows COLOR01 in full, and row 01 holds Color 0's `a b`, `c d`, `e f` in bits 9–8, 5–4 and 1–0; the key gives a colour-1 pixel Color 1's own `g h`, `i j`, `k l` |
| `CONTROL-KNOWN-VALUES-ARRIVE` (`:637`), `FIXTURE-DATA-PATH-LIVE` (`:664`), `OCS-BASELINE-0RGB-FORMAT` (`:693`), `SELECT-REGISTER-ADDRESSES-CORRECTLY` (`:731`) | green: run with `shres` = 0; `OCS-BASELINE-0RGB-FORMAT` checks only the top four bits of each 8-bit component, the part "stored as 0x0RGB" fixes | green | green | controls |

Each solid run is reloaded every 32 ticks and sampled on six consecutive ticks,
pixels 5 to 10 of the word's sixteen, with five of the word's pixels on either
side (`solid_run`, `:507-519`). Every adjacent pair and every group of four
that holds a sampled tick lies inside the word at every alignment, so under
both lookups each sampled tick reads COLOR(5*v*), and the six together show
both of its halves.

### At the system boundary

[`integration/ch4-shres-bitplane`](../../integration/ch4-shres-bitplane) runs the whole design on each CPU core with an enhanced chip set (VPOSR chip identifier $20, DENISEID low byte $FC) and a two-plane SuperHires display (BPLCON0 = $2040) of a pseudo-random pixel stream in which every pair of neighbouring values is present in every band. Frame F1 writes COLOR00–COLOR0F from p. 301's table by a series of palettes; in frames F2 to F4 each band of two lines inverts one bit of one lower register, all 192 bits in turn. One sample of the video output is one hires period, so each sample shows one of the period's two SuperHires pixels. fx68k and TG68K print the same lines.

- **The letters the key puts in bits 3–2 are there.** The twelve `P1` lines are green: a value-c pixel shows Colour c's two letters in bits 3–2 of R, G and B, over 64,392 to 78,289 graded pixels per line. Rows 00 to 03 of the table hold Colour c's own letters in their upper bit pairs, so a pixel that shows COLOR0c as 0x0RGB shows them.
- **The lower two bits of a component do not depend on its two letters alone.** `P2.R`, `P2.G` and `P2.B` grade p. 301's limit of two bits of red, green and blue per hires pixel: pixels of value c whose Colour c has the same two letters must show the same 4-bit component, over 272,160 graded pixels in 16 groups; they do not. NanoMig shows COLOR0c in full, and the table fills bits 9–8, 5–4 and 1–0 of COLOR0c with Colour 0's letters. In a mutation build, not run by `make test` (`NMH_MUTATE=P2G`, `test_ch4_shres_bitplane.cpp:79-81`), where F1's palettes write Colour 0 the same in every palette and so differ, for the registers of a value-c pixel, only in Colour c's letters, the same comparator prints PASS.
- **The table's other half of COLOR01–COLOR03 moves the wrong pixels.** Each of the eighteen bits that rows 01 to 03 give to Colour 0 changes none of the 1120 graded pixels at the place its letter names (`changed=0`). The 24 bits that rows 00 to 03 give to Colour c's own letters are green.
- **COLOR04–COLOR0F reach no pixel.** The SuperHires half of `P5.L` grades p. 301's statement that SuperHires bitplane colours are encoded in the sixteen lower registers: some register outside COLOR00–COLOR03 changes a playfield pixel. Over all 144 bands of COLOR04–COLOR0F, 161,280 graded pixels, none changes; the RTL's `l_bpu > 2` and `l_bpu > 3` hold planes 3 and 4 at zero, so a two-plane pixel never addresses them. `P7.3`, the same sentence's two clauses together, is red through it, while its other half, `P5.U` (no playfield pixel depends on COLOR16–COLOR31), is green.

```text
FAIL: P2.R APPC-shr.c3 CH4-appC-context.c3 p.301: R depends only on its two encoded letters, 272160 graded pixels, 16 groups
FAIL: P4.n1b0 APPC-bpltab.r01b00 p.301: bit 0 of COLOR01 changes only its own place, checked=1120 changed=0 excluded=0
MEASURED: P4: 150 of 192 bits not observable (L1), 0 not graded
FAIL: P5.L CH4-appC-context.c7 p.301/94 (SuperHires half): a register outside COLOR00-03 changes a visible playfield pixel, any bit, bands graded 144 of 144, bands whose rebuilt palette matches the plan 144, pixels checked 161280, changed=0 (allowed-bit-only count 0)
FAIL: P7.3 APPC-shr.c8 p.301: the bitplane colours are encoded in the sixteen lower registers (P5.L and P5.U together; P5.L / P5.U): fails / holds
```

The 150 bits that the package records as not observable (`changed=0` of 1120 graded pixels on each) are the 144 bits of COLOR04–COLOR0F and bits 0, 1, 4, 5, 8 and 9 of COLOR00.

---

## Risks and unknowns

- **Which register a pixel reads.** p. 301 does not say which register, or
  which half of it, a given pixel reads, nor what a pixel shows in the two
  positions the key marks `--`. The two lookups disagree on mixed pairs, which
  the package therefore drives but does not grade.
- **Depth.** Every SuperHires stimulus runs at BPU = 2, the ceiling p. 300 gives; one
  bitplane is not measured.
- **The lookup columns are derived**, not run.
- **A split witness.** WinUAE and vAmiga pair pixels, and differ from each other
  in how; Denise and Copperline do not pair them.
- **One third-party silicon revision.** The 8373R4 dissection is a work in
  progress, of one revision, not a Commodore document. Its PLA and read decoders
  are read from drawn crosspoints, not from a netlist; the output stage is
  traced in the netlist for red, the one component drawn in full, and green and
  blue draw only their bit 0. None of the evidence here is a hardware capture.
- **A neighbouring divergence.** MiSTer and TC64 add a `window_ena` term to the
  depth line; this finding does not measure it.
- **150 of the 192 table bits are not observable at the output.** `integration/ch4-shres-bitplane` records them (`MEASURED: P4: 150 of 192 bits not observable`): inverting one changes none of the 1120 graded pixels at the place its letter names (`changed=0`), so no pixel shows that bit where the key puts its letter.

---

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | [`drawing.cpp:2991-3000`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L2991-L3000) | <pre>uae_u8 off = ((p1 & 3) * 4) + (p0 & 3) + ((p0 \| p1) & 16);</pre> reinterprets, ECS Denise only ([`:6470`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/drawing.cpp#L6470)): two adjacent SuperHires pixels select one of the sixteen lower registers; the first shows its bits 11–10, 7–6, 3–2 (`& 0xccc`), the second its bits 9–8, 5–4, 1–0 (`& 0x333`). A solid colour-1 run reads COLOR05 |
| **vAmiga** | [`Core/Components/Denise/PixelEngine.h:301-305`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.h#L301-L305) | <pre>return ((mbuf[base + 2] & 3) * 4) + (mbuf[base] & 3);</pre> reinterprets, ECS Denise only ([`PixelEngine.cpp:215-222`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.cpp#L215-L222)): pixels 0 and 2, and pixels 1 and 3, of each group of four select one of the sixteen lower registers; even pixels show the high bit pair of each component, odd pixels the low pair ([`PixelEngine.cpp:620`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/PixelEngine.cpp#L620), [`Colors.cpp:127-148`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Denise/Colors.cpp#L127-L148)). A solid colour-1 run reads COLOR05; a mixed pattern can read a different register than under WinUAE's pairing |
| **Denise** | [`emulation/libami/video/denise.cpp:200`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L200) | <pre>colors[pos] = value;</pre> one table, no mode; a SuperHires pixel shows its own register in full ([`:797`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/video/denise.cpp#L797), `color = colors[colIndex];`) |
| **Copperline** | [`src/video/bitplane/tests.rs:7563`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/tests.rs#L7563) | *"each 35 ns half resolves palette[1], never a pair-encoded entry"*; the path under test looks up each 35 ns sample on its own ([`src/video/bitplane/output.rs:324-333`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/video/bitplane/output.rs#L324-L333)) |
| **MiSTer** | [`rtl/denise_colortable.v:32-35`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/denise_colortable.v#L32-L35) | descendant — same as [NM-HRM-016](NM-HRM-016.md): no `shres` port on `rtl/denise_colortable.v:32-35`; `rtl/denise.v:339` — `assign bpldata[3] = window_ena && (l_bpu > 2) ? bpldata_out[3] : 1'b0;`, the same `l_bpu > 2` depth force as NanoMig's `:369`, with an added `window_ena &&` term NanoMig's line lacks |
| **TC64** | [`rtl/minimig/denise_colortable.v:32-35`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/denise_colortable.v#L32-L35) | descendant — the same interface, mode-blind; its 11-line diff from NanoMig (a RAM primitive's word width) is identical to MiSTer's and unrelated to colour mode. `rtl/minimig/denise.v:354` is the identical depth line, `window_ena` term included |

**On `integration/ch4-shres-bitplane`'s display**, two planes of a pseudo-random pattern, WinUAE's and vAmiga's lookups above read COLOR(4q + p) for a pair whose first pixel has value p and whose second has value q (WinUAE pairs neighbours, vAmiga pixels 0 and 2 and pixels 1 and 3), so every pair with q not 0 reads one of COLOR04–COLOR0F, and each pixel shows two bits per component from one half of that register. Denise and Copperline look each pixel up in COLOR00–COLOR03 and show all twelve bits, as NanoMig does. That is read from the code cited in the table, not run.

### What other Commodore documents say

- **6502.org dissection of Denise 8373R4, undated — third-party, the silicon of one revision**, schematic sheets 9b, 8i, 7f to 7h and 9h — agrees with the manual:

  *Read from the drawing, not quoted:* the sixteen lower registers have two read decoders, rmCOLOR00 to rmCOLOR15 addressed by CMA0 to CMA3 and rsCOLOR00 to rsCOLOR15 by CSA0 to CSA3 (sheet 9b). In SuperHires the playfield PLA on sheet 8i (its rows on SH_RES#) sets C_SA1 and C_SA0 from planes 2 and 1 at shift-register position 15 and C_SA3 and C_SA2 from position 13, C_MA1 and C_MA0 from position 14 and C_MA3 and C_MA2 from position 12. Positions 15 to 12 are the four SuperHires pixels of a 140 ns cycle: sheet 7h gives planes 1 and 2 the outputs B1_Q12 to B2_Q15, while planes 3 and 4 have positions 15 and 14 (sheet 7g) and plane 5 position 15 (sheet 7f). In SuperHires the output stage of sheet 9h joins red outputs R0 with R2 and R1 with R3, drives them from register bits 11 and 10 in one 70 ns half and from bits 9 and 8 in the other, and alternates between the two read paths within each half.
  <!-- drawing: denise-8373r4-dissection/sheet_072.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_065.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_047.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_046.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_045.txt -->
  <!-- drawing: denise-8373r4-dissection/sheet_079.txt -->

  Each lower register is read for two pixels, the first and third or the second and fourth of a cycle, the grouping vAmiga uses where WinUAE pairs adjacent pixels, and each pixel shows two bits per component, the upper or the lower pair of that register, repeated to fill the component: p. 301's two bits and its table. A solid colour-1 run reads COLOR05, as in WinUAE and vAmiga, where NanoMig shows COLOR01 in full.

---

## Decision

**Suggested change:** as with [NM-HRM-016](NM-HRM-016.md), `denise_colortable` needs a mode input it does not have, and at a depth of one or two bitplanes its address must reach COLOR04–COLOR0F, which `denise.v:369-370` keep out of reach. The two findings are one mechanism and should be closed together or not at all.

It closes as stated in *What is asked of a reader*.
