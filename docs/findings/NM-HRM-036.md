# NM-HRM-036 — chip memory banks 1-3 stay reachable while the overlay is active

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. The page that was quoted allows what NanoMig does. |
| **Module** | `gary.v` |
| **Test** | [`hrm/ch7-reset-overlay`](../../hrm/ch7-reset-overlay) measures the bank sweep and declines to grade it |
| **Chapter** | 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Error class** | **a sentence graded without the paragraph that qualifies it** |
| **Verdict of the four** | **3–1**: vAmiga, Denise and Copperline each replace only the ROM's own 512K; only WinUAE makes "all memory" literally true |

---

## Why it was withdrawn

The assertion graded one sentence of printed p. 223, on the 68000 `RESET`
instruction:

> "All memory and AUTOCONFIG(TM) cards disappear"

and required every chip-memory bank to vanish while the overlay is active.
NanoMig keeps banks 1-3 mapped, so it was red, and this page called the sentence
a manual defect.

**The same page qualifies it, seven lines earlier**, for power-on and external
reset:

> "On some Amiga models, portions of the RAM still respond. On other models, no
> RAM responds. Software must assume that memory is not available."

and the `RESET` paragraph itself says the instruction "works much like external
reset or power on" and ends "Since RAM may not be available, special care is
needed". The page does not require all memory to vanish; it requires software
not to rely on it. Banks 1-3 responding is one of the two behaviours it names.
The manual was not loose -- the sentence was read without its paragraph.

The measurement stands and is still printed by the package, as a `MEASURED:`
line; the claim is declined on a `record()` line quoting the qualification.

---

## What NanoMig does

Only bank 0 is gated on `ovl`. Banks 1, 2 and 3 are decoded unconditionally:

[`src/minimig-aga/gary.v#L159-L162`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/gary.v#L159-L162)

```verilog
		sel_chip[0] = cpu_address_in[23:19]==5'b0000_0 && (!ovl || cpu_hlt);
		sel_chip[1] = cpu_address_in[23:19]==5'b0000_1;
		sel_chip[2] = cpu_address_in[23:19]==5'b0001_0;
		sel_chip[3] = cpu_address_in[23:19]==5'b0001_1;
```

### Measured

One address per 512K bank, `ovl=1`:

| address | `sel_chip` |
|---|---|
| `$000000` | **0** — bank 0 does disappear |
| `$080000` | 2 |
| `$100000` | 4 |
| `$180000` | 8 |

---

## What the others say

Nobody decides this but the manual. Every line below was read in a local checkout
at the pinned revision named in the link.

| | code | |
|---|---|---|
| **vAmiga** | [`Core/Components/Memory/Memory.cpp:940-943`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Memory/Memory.cpp#L940-L943) | <pre>// Blend in Rom in lower memory area if the overlay line (OVL) is high<br>if (ovl) {<br>&nbsp;&nbsp;&nbsp;&nbsp;for (isize i = 0; i &lt; 8 && ...; i++)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;cpuMemSrc[i] = cpuMemSrc[0xF8 + i];<br>}</pre> eight pages of $10000 — **512K** |
| **Denise** | [`emulation/libami/agnus/memory.cpp:762-766`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/agnus/memory.cpp#L762-L766) | <pre>if (state) {<br>&nbsp;&nbsp;&nbsp;&nbsp;for (int i = 0x0; i &lt; 0x8; i++)<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;mapper[i] = mapper[0xf8 + i];</pre> the same eight banks |
| **Copperline** | [`src/cpu.rs:4733-4738`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/cpu.rs#L4733-L4738) | <pre>if !self.bus.mem.overlay { return None; }<br>region_offset(self.bus.mem.rom.len(), CHIP_RAM_BASE, addr, size)</pre> exactly the ROM's own length from `$0` |
| **WinUAE** | [`memory.cpp:2923-2936`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/memory.cpp#L2923-L2936) | <pre>if (size &lt; 32 && bogomem_aliasing == 0)<br>&nbsp;&nbsp;&nbsp;&nbsp;size = 32;<br>...<br>map_banks(rb, 0, size, 0x80000);</pre> the ROM mirrored across **at least 2 MB** |
| **MiSTer** | [`rtl/gary.v:172-175`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L172-L175) | descendant — byte-identical: banks 1-3 decoded unconditionally, only bank 0 gated by `(!ovl \| cpu_hlt)` |
| **TC64** | [`rtl/minimig/gary.v:150-153`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/gary.v#L150-L153) | descendant — the same shape, banks 1-3 unconditional, but bank 0's gate is narrower: `!ovl` alone, without the `cpu_hlt` term NanoMig and MiSTer both carry. All three still leave banks 1-3 unconditional, which is what this finding is about; gating them on `ovl` would diverge from both lines equally |

**Three of the four independent implementations replace only the ROM's own
512K**, exactly as NanoMig does. Only WinUAE mirrors the ROM over the whole
low 2 MB and so makes the printed sentence literally true. MiSTer and TC64
have no ROM-mirroring question to answer here at all — what they carry is the
same unconditional decode of banks 1-3 that is this finding's actual subject.


---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore, *A500/A2000 Gary Specification***, PDF page 1 — agrees with the manual:

  > "000000-1FFFFF NROM if OVR asserted"
  <!-- corpus: gary-spec/p001.txt -->

  The address-decode table gives BANK0 as 000000-1FFFFF and, in this row, decodes the ROM select over that whole 2 MB range: all four 512K chip-memory banks, not just the first. On that reading chip RAM at $080000-$1FFFFF answers as ROM while the overlay is up. That is WinUAE's shape, and it makes p.223's 'All memory and AUTOCONFIG(TM) cards disappear' literally true for memory on an A500/A2000. Two caveats. (1) The row says 'OVR', and elsewhere in this spec that is the override pin (NOVR); the ROME conditions require '3. OVR is de-asserted', so read as override the row would contradict itself. It only makes sense as the overlay input OVL ('Overlay (ROM to address 0)'), which is an inference from the text, not something it states. (2) ROME is qualified by a read (PRW=1), and the spec does not say whether NRAME is suppressed there, so it is silent on what a write to $080000 does while the overlay is up.

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 228 — agrees with the manual:

  > "RE = DBR*/AS*DTACK*/A23*/A22* /AZ1*OVR*/OVL* ; SOOQ0000-1FFFFF IF /C1*C3 3; OVL=L, OVR=H !"
  <!-- corpus: a500-a2000-trm-1987/p228.txt -->

  In the A2000 memory decoder PAL (U26, headed preliminary), the RAM enable covers all of $000000-1FFFFF and needs OVL low. /BLS carries the same /OVL term. While OVL is high, the ROM select (p229) covers only $000000-07FFFF and $180000-1FFFFF. So on this board no chip RAM answers anywhere in the low 2 MB while the overlay is up. That matches the p.223 sentence that all memory disappears, not NanoMig's banks 1-3 staying mapped. The finding is withdrawn because p.223 also allows models where RAM still responds; this source describes one model, the A2000, where none does.

Silent on this point: US patent 4,777,621, Commodore-Amiga.

---

## Why this entry is kept

It was a documented red for ten days, first against the RTL and then against the
printed page, and three of four implementations agreeing with NanoMig made the
"manual defect" reading look settled. A reader of p. 223 who stops at the RESET
paragraph will find the same red again; this page is where they find the
paragraph above it.
