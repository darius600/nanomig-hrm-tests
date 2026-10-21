# NM-HRM-060 — the instruction cache monitors writes, and the manual says it does not [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Question: after a CPU data write to the address of a cached instruction, must `cpu_cache_new` still return the old instruction (`$AAAA`, as p.13's "do not monitor writes" says) instead of the written word (`$CCCC`, which NanoMig returns)? It closes when `CACHE-WRITE-VISIBILITY` turns green with `CACHE-HIT-RETAINS-INSTRUCTION` and `CACR-CLEAR-INVALIDATES-CACHE` still green, or when an Amiga specialist records that `cpu_cache_new` is a coherent cache of the board's own, which p.13 does not describe. |
| **Report to** | `upstream first` — MiSTer-devel/Minimig-AGA_MiSTer |
| **Scope** | NanoMig's `cpu_cache_new` at its own module boundary: a CPU data write to the address of a cached instruction line, read back as an instruction; processor model and revision unknown; real hardware not measured; the chip-bus path (`CHIPRAM_CACHE`) not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 1 — Introduction, printed p. 13, General Amiga Development Guidelines · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · claims `CACHE-HIT-RETAINS-INSTRUCTION` and `CACHE-WRITE-VISIBILITY`: `opus:1-guidelines.c70`, `sol:1-guidelines.c70`, p.13; `CACR-CLEAR-INVALIDATES-CACHE`: `opus:1-guidelines.c75`, p.13; the fixture control `CACHE-FIXTURE-DELIVERS-KNOWN-VALUE` carries no claim id |
| **Module** | `cpu_cache_new.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/minimig-aga/cpu_cache_new.v#L296-L309); inherited (MiSTer and TC64 carry the same statement) |
| **Test** | [`hrm/ch1-cpu-and-instruction-cache`](../../hrm/ch1-cpu-and-instruction-cache): `make -C hrm/ch1-cpu-and-instruction-cache test`; an expected red exits nonzero |
| **Assertions** | 1 red, `CACHE-WRITE-VISIBILITY`: expected `$AAAA`, observed `$CCCC`; the package reports `3 passed, 1 failed`. `CACHE-HIT-RETAINS-INSTRUCTION` and `CACR-CLEAR-INVALIDATES-CACHE` are green: the cache caches and the documented `$0008` flush works |
| **Verdict of the four** | **3–0 against, one silent.** WinUAE, vAmiga and Copperline all implement the sentence exactly; Denise models a 68000 and has no cache to disagree with. Hardware: not measured |
| **Effort** | none; if an Amiga specialist records that `cpu_cache_new` models the processor's instruction cache, S: the `cpu_sm_iram0_we` and `cpu_sm_iram1_we` enables of the CPU write state at `cpu_cache_new.v:298-309`, and the `sdr_` twin at `:511` |

---

## Source requirement

Printed page 13, under GENERAL AMIGA DEVELOPMENT GUIDELINES:

> The 68020, 68030, and 68040 processors all have instruction caches. These
> caches store recently used instructions, but do not monitor writes. After
> modifying or directly loading instructions, the cache must be flushed.

The page then gives the constant a program flushes with:

> `CACRF_ClearI  EQU     $0008 ;Bit for clear instruction cache`

The flush advice follows from the statement: if the cache did monitor writes
there would be nothing to flush.

## RTL and measurement

`cpu_cache_new.v:298-309` is the CPU write state. On a write whose address hits
a valid **instruction** tag it enables a write into the instruction RAM:

```verilog
CPU_SM_WRITE : begin
  // on hit update cache, on miss no update neccessary; tags don't get updated on writes
  cpu_sm_bs         <= cpu_bs;
  cpu_sm_mem_dat_w  <= cpu_dat_w;
  cpu_sm_iram0_we   <= itag0_match && itag0_valid /*&& !cc_fr*/;
  cpu_sm_iram1_we   <= itag1_match && itag1_valid /*&& !cc_fr*/;
```

It is an update, not a snoop-invalidate: `cpu_sm_mem_dat_w` carries the written
word into the line. `cpu_cache_new.v:511` does the same from the SDRAM side,
`sdr_sm_iram0_we <= sdr_itag0_match && sdr_itag0_valid`, where `CHIPRAM_CACHE`
is defined (`nanomig.v:345`; without it `snoop_act` is tied to 0, `:406`). The
path is live: `cpu_wrapper.v:96/258` exports CACR and `nanomig.v:328/426`
carries it into `cpu_cache_new` inside the `ENABLE_CACHE` block at `:320`;
`src/tang/nano20k/top_020.sv:14/15/16` and `src/mistle/gw3a_20/top.sv:9/12/13`
define `ENABLE_TG68K`, `ENABLE_CACHE` and `CHIPRAM_CACHE`.

[`hrm/ch1-cpu-and-instruction-cache`](../../hrm/ch1-cpu-and-instruction-cache)
fills an instruction address with `$AAAA`, writes `$CCCC` to it as a CPU data
write, and reads the instruction address again: `$CCCC`, where p.13 predicts the
stale `$AAAA` — `CACHE-WRITE-VISIBILITY`, red.

## Risks and unknowns

- The implementation is safer than the manual, not more permissive: software
  written to p.13 flushes and works on both. Only a program that relies on the
  documented staleness, or a test of that behaviour, sees a difference.
- A deliberate coherency feature and a conformance gap look identical from
  here; `nanomig.v`'s comment near the instantiation calls chip-bus writes
  "snooped to keep cached … data coherent". Whether `cpu_cache_new` is meant to
  model the processor's instruction cache or to be the board's own is for the
  people who wrote it.
- The processor model and real hardware are not measured, and the test
  measures the CPU write path only.

## Independent evidence

| | | |
|---|---|---|
| **WinUAE** | does not monitor writes | `caches020` is invalidated in exactly two places, both inside `flush_cpu_caches` ([`newcpu.cpp:1573-1582`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/newcpu.cpp#L1573-L1582)): the whole cache when CACR bit 3 is set, one line when bit 2 is. No write path touches it |
| **vAmiga** | does not monitor writes | Moira's `flushInstructionCache` / `invalidateCacheEntry` are called from a CPU-model change ([`Moira.cpp:105`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/Moira/Moira.cpp#L105)), from reset ([`:230`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/Moira/Moira.cpp#L230)) and from the CACR write handler alone — [`:573`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/Moira/Moira.cpp#L573) for bit 2, [`:576`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/CPU/Moira/Moira.cpp#L576) for bit 3 |
| **Denise** | cannot disagree | its CPU is `libami/cpu/m68000`, a 68000; zero occurrences of `CACR` or an instruction cache in it |
| **Copperline** | does not monitor writes, and says so | [`src/cache.rs:25-28`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cache.rs#L25-L28): *"Like the real silicon, the instruction cache does NOT snoop writes - self-modifying code must clear it (through CACR on the 020/030, through CINV/CPUSH on the 040), and DMA (blitter/copper/disk) never invalidates it. The data cache is write-through; a write that hits invalidates the entry"*. It separates the two caches; NanoMig's write state enables `iram` and `dram` writes from the same condition |
| **MiSTer** | descendant — same statement | [`rtl/cpu_cache_new.v:306-307`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/cpu_cache_new.v#L306-L307) carries the same `cpu_sm_iram0_we <= itag0_match && itag0_valid /*&& !cc_fr*/;` and its `iram1` twin, commented-out guard included |
| **TC64** | descendant — same statement, relocated | no `cpu_cache_new.v` under `rtl/minimig/`, but `rtl/sdram/cpu_cache_new.v` carries the same two lines and guard, differing only by a `#1` delay; instantiated with `cache_en` hardwired to `1'b1` in [`rtl/sdram/sdram_ctrl.v:274`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/sdram/sdram_ctrl.v#L274), part of the board target `rtl/soc/minimig_mist_top.v` |

## Decision

Must a cached instruction stay stale after a CPU write to its address? No change
is proposed now. If an Amiga specialist records that `cpu_cache_new` models the
processor's instruction cache, the instruction-RAM write enables of the CPU write
state and of the chip-bus snoop path stop following writes, and the page closes
when `CACHE-WRITE-VISIBILITY` turns green with the two controls still green; if
the specialist records that it is a coherent cache of the board's own, which p.13
does not describe, the page closes on that record.
