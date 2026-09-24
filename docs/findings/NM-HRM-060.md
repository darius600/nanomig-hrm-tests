# NM-HRM-060 — the instruction cache monitors writes, and the manual says it does not [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `cpu_cache_new.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/cpu_cache_new.v#L296-L309) |
| **Report to** | **the Minimig line**, not NanoMig alone |
| **Test** | [`hrm/ch1-cpu-and-instruction-cache`](../../hrm/ch1-cpu-and-instruction-cache) `CACHE-WRITE-VISIBILITY` |
| **Chapter** | 1 — Introduction · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 1 red. Its sibling `CACHE-HIT-RETAINS-INSTRUCTION` and `CACR-CLEAR-INVALIDATES-CACHE` are both green, so the cache caches and the documented $0008 flush works |
| **Verdict of the four** | **3–0 against, one silent.** WinUAE, vAmiga and Copperline all implement the sentence exactly; Denise models a 68000 and has no cache to disagree with |

---

## What the HRM says

Printed page 13, under GENERAL AMIGA DEVELOPMENT GUIDELINES:

> The 68020, 68030, and 68040 processors all have instruction caches. These
> caches store recently used instructions, but do not monitor writes. After
> modifying or directly loading instructions, the cache must be flushed.

The page then gives the constant a program uses to do it:

> `CACRF_ClearI  EQU     $0008 ;Bit for clear instruction cache`

The sentence is not advice. It is a statement of fact about the processor, and
the advice — flush after modifying instructions — follows *from* it. If the
cache did monitor writes there would be nothing to flush.

## What NanoMig does

`cpu_cache_new.v:298-309` is the CPU write state. On a write whose address hits
a valid **instruction** tag, it enables a write into the instruction RAM:

```verilog
CPU_SM_WRITE : begin
  // on hit update cache, on miss no update neccessary; tags don't get updated on writes
  cpu_sm_bs         <= cpu_bs;
  cpu_sm_mem_dat_w  <= cpu_dat_w;
  cpu_sm_iram0_we   <= itag0_match && itag0_valid /*&& !cc_fr*/;
  cpu_sm_iram1_we   <= itag1_match && itag1_valid /*&& !cc_fr*/;
```

The design's own comment says it: *on hit update cache*. It is not a
snoop-invalidate, which would merely discard the line — `cpu_sm_mem_dat_w`
carries the newly written word into it, so the cached instruction **becomes**
what was just written.

There is a second path. `cpu_cache_new.v:511` does the same from the SDRAM side,
`sdr_sm_iram0_we <= sdr_itag0_match && sdr_itag0_valid`, so a write arriving over
the chip bus updates the instruction cache too.

Measured by the test, at the module's own boundary:

```
fill an instruction address with $AAAA, read it back        -> $AAAA   (hit, PASS)
write $CCCC to that address as a CPU data write
read the instruction address again                          -> $CCCC
```

Page 13 predicts `$AAAA` — the stale instruction that makes a flush necessary.

### Measured

[`hrm/ch1-cpu-and-instruction-cache`](../../hrm/ch1-cpu-and-instruction-cache)
fills an instruction address with `$AAAA`, writes `$CCCC` to it as a CPU data
write, and reads the instruction address again: `$CCCC`, where p.13's "do not
monitor writes" predicts the stale `$AAAA` — `CACHE-WRITE-VISIBILITY`, red,
one of the package's `3 passed, 1 failed`. `CACHE-HIT-RETAINS-INSTRUCTION`
and `CACR-CLEAR-INVALIDATES-CACHE` stay green as controls on the same
fixture.

## What the others say

| | | |
|---|---|---|
| **WinUAE** | does not monitor writes | `caches020` is invalidated in exactly two places, both inside `flush_cpu_caches` (`newcpu.cpp:1573-1582`): the whole cache when CACR bit 3 is set, one line when bit 2 is. No write path touches it |
| **vAmiga** | does not monitor writes | Moira's `flushInstructionCache` / `invalidateCacheEntry` are called from reset (`Moira.cpp:105`, `:230`) and from the CACR write handler alone — `:573` for bit 2, `:576` for bit 3 |
| **Copperline** | does not monitor writes, and says so | `src/cache.rs:25-28`: *"Like the real silicon, the instruction cache does NOT snoop writes — it is invalidated only by explicit cache control (CACR on the 020/030 …), and DMA (blitter/copper/disk) never invalidates it. The data cache is write-through; a write that hits invalidates the entry"* |
| **Denise** | cannot disagree | its CPU is `libami/cpu/m68000`, a 68000. Zero occurrences of `CACR` or an instruction cache anywhere in it. The 68000 has no instruction cache, so the sentence has no counterpart to implement |
| **MiSTer** | descendant — identical statement | `rtl/cpu_cache_new.v:306-307` carries the same `cpu_sm_iram0_we <= itag0_match && itag0_valid /*&& !cc_fr*/;` (and its `iram1` twin), commented-out guard included. It carries the same commented-out `/*&& !cc_fr*/` guard. |
| **TC64** | descendant — identical statement, different location | **Not absent, only relocated.** There is no `cpu_cache_new.v` under `rtl/minimig/`, but `rtl/sdram/cpu_cache_new.v` carries the same two lines verbatim, guard included — the same module under the same 2015 `rok.krajnc@gmail.com` authorship, instantiated with `cache_en` hardwired to `1'b1` in `rtl/sdram/sdram_ctrl.v:274`, itself part of the real board target `rtl/soc/minimig_mist_top.v`. Its `CPU_SM_WRITE` state carries the identical statement and the identical commented-out guard. A fix here diverges from neither line. |

Copperline is the sharpest witness, because it draws the distinction NanoMig does
not: **instruction cache no snoop, data cache invalidate on write hit.**
NanoMig's write state enables `iram` and `dram` writes from the same condition,
so it treats both alike.

## What holds this finding up

**The control cases are green.** `CACHE-HIT-RETAINS-INSTRUCTION` shows the cache
really caches — an address filled once and read again returns the cached word
after the memory model beneath it changed. `CACR-CLEAR-INVALIDATES-CACHE` shows
the manual's own $0008 bit works exactly as printed. So this is not a fixture
that cannot tell a cache from a wire, and it is not a cache that is simply
absent.

**The path is live, not dead code.** `cpu_wrapper.v:96/258` exports CACR from the
TG68K core and `nanomig.v:312/422` carries it into `cpu_cache_new` as
`cpu_cache_ctrl`; `src/tang/nano20k/top_020.sv:12` and `src/mistle/gw3a_20/top.sv:9`
define `ENABLE_TG68K`.

**The implementation is SAFER than the manual, not more permissive.** Software
written to page 13 flushes the cache and works on both. What differs is a
program that relies on the documented staleness, and any test of the documented
behaviour itself. That is why this is filed and not adjudicated: a deliberate
coherency feature and a conformance gap look identical from here, and
`nanomig.v`'s own comment near the instantiation calls chip-bus writes
"snooped to keep cached … data coherent".

## What is left for specialists

Whether the Minimig line intends `cpu_cache_new` to model the 68020's on-chip
instruction cache — in which case the snoop is a divergence from the processor
it stands in for — or to be an accelerator cache of the board's own, for which
coherency is a feature and page 13 simply does not describe it. The suite cannot
settle that: it measures what the boundary does against what the page says, and
those two differ. The question of intent belongs to the people who wrote it.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

