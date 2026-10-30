# NM-HRM-094 — in 68020 mode NanoMig's TG68K reads no more instruction words ahead than in 68000 mode, where p. 13 says the more advanced processors prefetch more words [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page; the implementations split |
| **What is asked of a reader** | Question: with `cpu_config` = 11, the 68020 setting, must NanoMig's TG68K read more instruction words ahead of the one it executes than with 00, as p. 13's "The more advanced processors prefetch more words" says, where it reads one guard word past the stop instruction in both modes (D = 1 and D = 1)? Closes when the kernel reads further ahead with `cpu(1)` set and `P6` of `integration/ch1-cpu-system` turns green on TG68K, or when an Amiga specialist records that NanoMig's 68020 mode keeps the one-word prefetch of its 68000 mode. |
| **Report to** | `upstream first` — TobiFlex/TG68K.C |
| **Scope** | NanoMig's TG68K kernel with `cpu_config` = 11 against 00, one program image run in both modes and read at the Chip-memory port; `turbo_config` = 000, so every CPU access takes the chip bus and no cache is in the path, and CACR at its reset value; four kinds of stop instruction (`BRA.S`, `JMP (A0)`, `RTS`, `BEQ.S`), each at both long-word alignments; the 68010 setting is not compared, and the 68030 and 68040 have no setting; processor revision unknown; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 1 — Introduction, printed p. 13, General Amiga Development Guidelines · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · `P6` carries no claim id, only its page tag `[p13]`, and neither do its gates `P6 alignment` and `P6.e`; `opus:1-guidelines.c67` and `sol:1-guidelines.c67`, p. 13, stand on a RECORDED line of the package as withdrawn, because the sentence also covers the 68030 and 68040 |
| **Module** | `TG68KdotC_Kernel.vhd` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/tg68k/TG68KdotC_Kernel.vhd#L1132-L1133) · [Minimig-AGA `da7632e`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/tg68k/TG68KdotC_Kernel.vhd#L1138-L1139); inherited (MiSTer's copy has the same fetch path; TC64 builds the kernel from TobiFlex/TG68K.C, whose fetch path is the same lines) |
| **Test** | [`integration/ch1-cpu-system`](../../integration/ch1-cpu-system): `make -k -C integration/ch1-cpu-system test` (or `test-tg68k`; the fx68k build has no 68020 mode and grades no `P6` pair); an expected red exits nonzero |
| **Assertions** | 1 red of this finding's, on TG68K: `P6` — expected D(68020 mode) > D(68000 mode) for the same image, observed D = 1 in both modes, and the two runs' Chip-memory port logs identical entry for entry (244 entries). Its gates are green in both runs: the fixture, `C1` to `C5` (C5 reads `cpu_config` as 0x00 and 0x03), `P3.a` to `P3.d`, `P6 alignment` and `P6.e`. TG68K prints 170 passed, 3 failed; the other two reds, both about CLR, are not this finding's (*Risks and unknowns*). fx68k prints 67 passed, 0 failed and records its own D(68000 mode) = 1 |
| **Verdict of the four** | **split, read, not run**: WinUAE's 68020 fills a three-word pipeline from aligned long words, where its 68000 holds two words, so it reads further ahead; vAmiga keeps the same two-word queue in both models but reads a 68020's instruction words through a long-word latch, so on a 16-bit port it reads one word further when the next word opens a long word; Copperline's CPU core keeps its two-word queue for the 68000 and 68010 only and reads a 68020's instruction words at the PC as it needs them, so its 68020 reads less far ahead than its 68000; Denise models a 68000 only and casts no vote. Hardware: not measured |
| **Effort** | none; if an Amiga specialist records that NanoMig's 68020 mode should read further ahead than its 68000 mode, L: the kernel's instruction fetch, which holds one word in `last_opc_read` and moves the PC one word per fetch in every mode (`TG68KdotC_Kernel.vhd:1020-1021` and `:1132-1133`), made deeper while `cpu(1)`, the bit on which the kernel admits the 68020's MOVEC registers, CACR among them (`:3841`), is set |
| **See also** | [NM-HRM-060](NM-HRM-060.md) — the same p. 13 list, on the 68020's instruction cache |

---

## Source requirement

**Printed p. 13**, under General Amiga Development Guidelines:

> Self-modifying code is strongly discouraged. All 68000 family processors have a pre-fetch feature. This means the CPU loads instructions ahead of the current program counter. Hence, if your code modifies or decrypts itself just ahead of the program counter, the pre-fetched instructions may not match the modified instructions. The more advanced processors prefetch more words. If self-modifying code must be used, flushing the cache is the safest way to prevent troubles.

The page gives no depth for any processor and does not say which processors are the more advanced ones. It names the 68020 with the 68030 and 68040 as the processors that have instruction caches, so the 68020 is more advanced than the 68000 on every reading; only that pair is graded.

## RTL and measurement

**The kernel's fetch reads one word and moves the PC one word, whatever the CPU input says.** A fetch cycle, `state="00"`, latches the word it reads into `last_opc_read`, a 16-bit register ([`TG68KdotC_Kernel.vhd#L1132-L1133`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/tg68k/TG68KdotC_Kernel.vhd#L1132-L1133)), and adds one word to the PC ([`#L1020-L1021`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/tg68k/TG68KdotC_Kernel.vhd#L1020-L1021)):

```vhdl
		ELSIF state="00" THEN
			PC_datab(1) <= '1';
```

Neither reads the CPU input. The file tests `cpu(0)` or `cpu(1)` on 27 lines, and none of them is in the fetch path: they choose the exception frames and SR bits, MOVES, MOVE from SR and MOVE from CCR, the skipped operand read of CLR, Scc and MOVE from SR, the extended addressing modes, CAS, CMP2 and CHK2, EXTB.L and CHK.L, 32-bit MUL and DIV, TRAPcc, bit fields, the barrel shifter, cpSAVE and cpRESTORE, MOVEM, and MOVEC's control registers. The boards compile this file (`src/nanomig_020.gprj:99-102`); the integration build compiles its machine translation, `sim/TG68KdotC_Kernel.v` (`integration/harness/nanomig.mk`, `TG68K_FILE`), which reads `cpu[0]` 7 times and `cpu[1]` 19 times, as many as the VHDL once its barrel-shifter test, constant at the default, folds away.

**The setting reaches the kernel.** `nanomig.v:210` maps `wire [1:0] cpucfg = cpu_config; // CPU-Type: 00 = 68000, 01 = 68010, 11 = 68020`, `:309` passes `.cpucfg       (cpucfg          ),` to the wrapper, and the wrapper passes `.cpu(cpucfg),` to the kernel in this build (`cpu_wrapper.v:437`) and `.CPU(cpucfg),` on the boards (`:453`). In the same package `P5` (the exception frames differ between the three settings) and `P7` (MOVE SR in user mode) pass on TG68K, so the kernel acts on the setting.

[`integration/ch1-cpu-system`](../../integration/ch1-cpu-system) runs one image twice on TG68K, with `cpu_config` = 00 and 11. Each of its eight blocks ends in a stop instruction X_k followed by six guard words that are never executed; a long `DIVU.W` before X_k gives the CPU time to read ahead. D is the largest number of guard words read at the Chip-memory port in any block. `P6` grades D(68020 mode) > D(68000 mode) at `test_ch1_cpu_system.cpp:2193-2194`:

```text
RECORDED: P6 D(68000 mode, second core) = 1 (guard length 6)
RECORDED: P6 D(68020 mode, second core) = 1
FAIL: P6 [p13]: D(68020 mode) > D(68000 mode), the same image on the second core: the 68020 prefetches more words than the 68000 (D(68020 mode) = 1, D(68000 mode) = 1)
RECORDED: P6 measured: D(68020 mode) = 1, D(68000 mode) = 1 (same image, second core); the 68020 mode RAM-port log against the 68000-mode log of the same image: identical, entry for entry (time, address, data, direction), 244 entries
```

`P6.e`, D(68000 mode) below the six-word guard, gates it, so a larger count could show. P3's far-word calibration, green in `make test`, shows the count can report absence; P6.e's one-word-guard contrast, a mutation build not run by `make test`, shows it can report saturation.

## Risks and unknowns

- **This build drops one kernel output the boards use.** Its kernel instance leaves `skipFetch` unconnected (`cpu_wrapper.v:420-440`; Verilator reports "Instance has missing pin: 'skipFetch'"), where the boards connect `.skipFetch(skip_fetch_i),` (`:464`) and the wrapper withholds the CPU's bus request while it is set (`:101`), so this build reads words a board's TG68K skips. A copy of the RTL with that one pin added to the port list at `:437`, built and run outside `make test`, still gives D = 1 in both modes and identical logs (239 entries); the same copy turns the package's two CLR reds green.
- **The cache path is not measured.** With Fast RAM or a turbo mode, CPU reads take the direct RAM path, which in a build with `ENABLE_CACHE`, this one included, runs through `cpu_cache_new`: its misses fetch a whole 64-bit line (`nanomig.v:334-336`), and its control is the CACR (`:438`) that only the 68020 setting can write through MOVEC (`TG68KdotC_Kernel.vhd:3840-3841`). What that path reads ahead is not measured.
- **The page gives no number**, so `P6` grades only the order. The claim ids are withdrawn because the sentence covers the 68030 and 68040, which the system cannot run.
- **The four are read, not run**, each at one revision; Copperline's CPU core is the crate m68k 0.13.0, read at that version.
- **Hardware is not measured.**

## Independent evidence

The question put to each: does its 68020 read more instruction words ahead of the one it executes than its 68000?

| | code | |
|---|---|---|
| **WinUAE** | [`newcpu.cpp:10340-10348`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L10340-L10348), [`:10264-10293`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L10264-L10293), [`:10104-10111`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L10104-L10111) | **yes.** For the 68000 and 68010 the refill reads two words, `regs.ir = x_get_word (pc);` and `regs.irc = x_get_word (pc + 2);`. For the 68020 it reads the aligned long words at `pc &= ~3;` and `fill_icache020 (pc + 4, true);` into a three-word pipeline, `regs.prefetch020[2] = w;` |
| **vAmiga** | [`Core/Components/CPU/Moira/MoiraDataflow_cpp.h:865-878`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/Moira/MoiraDataflow_cpp.h#L865-L878), [`:673-677`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/Moira/MoiraDataflow_cpp.h#L673-L677), [`MoiraCache_cpp.h:44-46`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/Moira/MoiraCache_cpp.h#L44-L46), [`:83`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/Moira/MoiraCache_cpp.h#L83) | **by one word, at one alignment.** Every model keeps two words, `queue.ird = queue.irc;` and `queue.irc = readInstr<C, F>(reg.pc + 2);`. On the 68020 each instruction read goes through `result = readInstructionCache(addr & addrMask<C>(), busAccess);`, which loads the aligned long word, `auto base = addr & addrMask() & ~3;`, on a 16-bit port as `case 2:  data = u32(read16(base)) << 16 \| read16(base + 2); break;`. When the next word opens a long word, the word after it is read too |
| **Denise** | [`emulation/libami/cpu/m68000/m68000.h:42`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/cpu/m68000/m68000.h#L42) | **no vote**: its CPU is `class M68000 {`, and no file under `emulation/` names a 68010 or a 68020 |
| **Copperline** | [`Cargo.lock:3497-3500`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/Cargo.lock#L3497-L3500); m68k 0.13.0, benletchford/m68k-rs `3d00fb8` [`src/core/cpu.rs:790-797`](https://github.com/benletchford/m68k-rs/blob/3d00fb8f51bfb7970e7128d0d1202edf3633170e/src/core/cpu.rs#L790-L797), [`src/core/ea.rs:284-299`](https://github.com/benletchford/m68k-rs/blob/3d00fb8f51bfb7970e7128d0d1202edf3633170e/src/core/ea.rs#L284-L299) | **no, fewer.** Its CPU core is the crate m68k, pinned at `version = "0.13.0"`. The crate keeps its two-word queue only for "matches!(self.cpu_type, CpuType::M68000 \| CpuType::M68010)", and `read_imm_16` reads every other CPU type's instruction words at the PC when they are needed, "Other CPU types read directly at PC": its 68020 reads no word ahead |
| **MiSTer** | [`rtl/tg68k/TG68KdotC_Kernel.vhd:1026-1027`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/tg68k/TG68KdotC_Kernel.vhd#L1026-L1027), [`:1138-1139`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/tg68k/TG68KdotC_Kernel.vhd#L1138-L1139) | descendant — the same fetch path, `PC_datab(1) <= '1';` and `last_opc_read <= data_read(15 downto 0);`. Its file differs from NanoMig's in 22 lines, a data-cache bit of CACR and the reset synchroniser, none in the fetch path |
| **TC64** | [`rtl/soc/tg68.qip:2`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/soc/tg68.qip#L2), [`.gitmodules:1-3`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/.gitmodules#L1-L3); TobiFlex/TG68K.C `ade33e3` [`TG68KdotC_Kernel.vhd:1023-1024`](https://github.com/TobiFlex/TG68K.C/blob/ade33e396a1e647c2de9daf71ff9d5b3979639b2/TG68KdotC_Kernel.vhd#L1023-L1024), [`:1135-1136`](https://github.com/TobiFlex/TG68K.C/blob/ade33e396a1e647c2de9daf71ff9d5b3979639b2/TG68KdotC_Kernel.vhd#L1135-L1136) | descendant — it compiles `../tg68k/TG68KdotC_Kernel.vhd` from the submodule `rtl/tg68k`, `url = https://github.com/TobiFlex/TG68K.C.git`, at `ade33e3`, with the same fetch path; that file differs from NanoMig's in 7 lines, all in the reset synchroniser |

## Decision

**One option: let the kernel read further ahead while `cpu(1)` is set**, beyond the one word `last_opc_read` holds now, as WinUAE's 68020 does by filling its pipeline from aligned long words. No change is proposed now: p. 13 gives no depth, and the kernel holds one word in every mode. If the change is made, `P6` turns green with `P6.e` and `P6 alignment` still green on the 68000-mode run.
