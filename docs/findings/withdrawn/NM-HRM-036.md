# NM-HRM-036 — chip memory banks 1-3 stay reachable while the overlay is active [withdrawn]

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | p. 223 says of power-on and external reset "On some Amiga models, portions of the RAM still respond", and with `ovl=1` NanoMig's `sel_chip` is 0 at `$000000` but 2, 4 and 8 at `$080000`, `$100000` and `$180000`: banks 1-3 respond, one of the two behaviours the page names |
| **What is asked of a reader** | Nothing. p. 223 requires software not to rely on RAM after RESET, not every chip bank to vanish, so a red graded on "All memory ... disappear" cannot be raised again. |
| **Withdrawal reason** | The sentence graded was qualified seven lines earlier on the same page; banks 1-3 responding is one of the two behaviours that page allows |
| **Error class** | **a sentence graded without the paragraph that qualifies it** |
| **Replacement** | none for the bank sweep, which the package prints as a `MEASURED:` line and declines on a `record()` line quoting the qualification; green: `RESET-ALL-MEMORY-DISAPPEARS-CONTROL-BANK0-ONLY` in [`hrm/ch7-reset-overlay`](../../../hrm/ch7-reset-overlay) (four points inside bank 0 all read `sel_chip` gone) |
| **Module** | `gary.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L159-L162); `inherited` (MiSTer has the same code; TC64's bank 0 gate is `!ovl` alone, banks 1-3 unchanged) |
| **Test** | `make -C hrm/ch7-reset-overlay test`; the package prints `6 passed, 0 failed` and has no expected red |
| **Chapter** | 7 — System Control, printed p. 223 (the RESET instruction) · *Amiga Hardware Reference Manual*, 3rd ed., 1991; `RESET-ALL-MEMORY-DISAPPEARS-SWEEP`: `sol:7-reset.c13`, p. 223; `AUTOCONFIG-HALF-NOT-ASSERTED`: `sol:7-reset.c13`, p. 223; `RESET-ALL-MEMORY-DISAPPEARS-CONTROL-BANK0-ONLY`: no claim id |
| **Assertions** | `RESET-ALL-MEMORY-DISAPPEARS-SWEEP`, the withdrawn check (every chip bank gone with `ovl=1`: expected `sel_chip` 0 at all four addresses, observed 0, 2, 4, 8), is a `MEASURED:` line and a `record()` line, not graded. `RESET-ALL-MEMORY-DISAPPEARS-CONTROL-BANK0-ONLY` green |
| **Verdict of the four** | **3–1**: vAmiga, Denise and Copperline each replace only the ROM's own 512K; only WinUAE, outside its aliased 0.5 MB + 0.5 MB configuration, makes "all memory" literally true for chip RAM. Hardware: p. 223 says models differ; the 1987 A500/A2000 TRM's A2000 decoder leaves no chip RAM answering in `$000000-1FFFFF` while OVL is high; no hardware was measured |

---

## Why it was withdrawn

The assertion graded one sentence of printed p. 223, on the 68000 `RESET`
instruction:

> All memory and AUTOCONFIG(TM) cards disappear

and required every chip-memory bank to vanish while the overlay is active.
NanoMig keeps banks 1-3 mapped, so it was red.

**The same page qualifies it, seven lines earlier**, for power-on and external
reset:

> On some Amiga models, portions of the RAM still respond. On other models, no RAM responds. Software must assume that memory is not available.

and the `RESET` paragraph itself says:

> The 68000 RESET instruction works much like external reset or power on.

> Since RAM may not be available, special care is needed to write reboot code that will reliably reboot all Amiga models.

The page does not require all memory to vanish; it requires software not to
rely on it.

---

## What NanoMig does

Only bank 0 is gated on `ovl`. Banks 1, 2 and 3 are decoded unconditionally:

[`src/minimig-aga/gary.v#L159-L162`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/gary.v#L159-L162)

```verilog
		sel_chip[0] = cpu_address_in[23:19]==5'b0000_0 && (!ovl || cpu_hlt);
		sel_chip[1] = cpu_address_in[23:19]==5'b0000_1;
		sel_chip[2] = cpu_address_in[23:19]==5'b0001_0;
		sel_chip[3] = cpu_address_in[23:19]==5'b0001_1;
```

One address per 512K bank, `ovl=1`:

| address | `sel_chip` |
|---|---|
| `$000000` | **0** — bank 0 does disappear |
| `$080000` | 2 |
| `$100000` | 4 |
| `$180000` | 8 |

---

## Risks and unknowns

- **Condition for filing again.** A printed page that requires chip RAM beyond
  bank 0 to stop responding while the overlay is active. p. 223 does not: it
  names both behaviours.
- **No hardware was measured.**
- **WinUAE** mirrors the ROM over at least `$000000-1FFFFF` ([`memory.cpp:2923-2936`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2923-L2936)), and makes the sentence true
  for chip memory, except in its aliased 0.5 MB + 0.5 MB configuration.
- **Gary specification:** the row says `OVR`, which elsewhere in that spec is
  the override pin; read as override it would contradict the ROME condition
  "3. OVR is de-asserted", so reading it as the overlay input OVL is an
  inference from the text, not a statement. ROME is qualified by a read
  (PRW=1), and the spec does not say whether NRAME is suppressed there, so it is
  silent on what a write to `$080000` does while the overlay is up.
- **A500/A2000 TRM:** the A2000 memory decoder PAL (U26) is headed
  preliminary and describes one model.

---

## What the others say

| | |
|---|---|
| **WinUAE** | [`memory.cpp:2923-2936`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2923-L2936): the ROM mirrored across **at least 2 MB**; where `if (bogomem_aliasing) size = 8;` ([`memory.cpp:2889-2890`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2889-L2890)) applies, 0.5 MB chip plus 0.5 MB slow RAM emulated as aliased 1 MB chip RAM, over 512K only |
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:940-943`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L940-L943): eight pages of $10000 — **512K** |
| **Denise** | [`emulation/libami/agnus/memory.cpp:762-766`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L762-L766): the same eight banks |
| **Copperline** | [`src/cpu.rs:4733-4738`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L4733-L4738): exactly the ROM's own length from `$0` |
| **MiSTer** | descendant — [`rtl/gary.v:172-175`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L172-L175) is byte-identical: banks 1-3 decoded unconditionally, only bank 0 gated by `(!ovl \|\| cpu_hlt)` |
| **TC64** | descendant — [`rtl/minimig/gary.v:150-153`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/gary.v#L150-L153) has the same shape, banks 1-3 unconditional, but bank 0's gate is `!ovl` alone, without the `cpu_hlt` term NanoMig and MiSTer carry |

### What other Commodore documents say

- **Commodore, *A500/A2000 Gary Specification*, undated**, PDF page 1 — agrees with the manual:

  > "000000-1FFFFF NROM if OVR asserted"
  <!-- corpus: gary-spec/p001.txt -->

  The address-decode table gives BANK0 as 000000-1FFFFF and, in this row, decodes the ROM select over that whole 2 MB range: all four 512K chip-memory banks, not just the first. On that reading chip RAM at $080000-$1FFFFF answers as ROM while the overlay is up. That is WinUAE's shape, and it makes p.223's 'All memory and AUTOCONFIG(TM) cards disappear' literally true for memory on an A500/A2000.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 228 — agrees with the manual:

  > "RE = DBR*/AS*DTACK*/A23*/A22* /AZ1*OVR*/OVL* ; SOOQ0000-1FFFFF IF /C1*C3 3; OVL=L, OVR=H !"
  <!-- corpus: a500-a2000-trm-1987/p228.txt -->

  In the A2000 memory decoder PAL (U26), the RAM enable covers all of $000000-1FFFFF and needs OVL low. /BLS carries the same /OVL term. While OVL is high, the ROM select (p229) covers only $000000-07FFFF and $180000-1FFFFF. So on this board no chip RAM answers anywhere in the low 2 MB while the overlay is up. That matches the p.223 sentence that all memory disappears, not NanoMig's banks 1-3 staying mapped; it describes one model, the A2000, where none responds.
