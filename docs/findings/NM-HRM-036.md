# NM-HRM-036 — chip memory banks 1-3 stay reachable while the overlay is active

| | |
|---|---|
| **Status** | open · red — **the manual is the likely defect**, see the first section |
| **Module** | `gary.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/gary.v#L159-L162) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/gary.v#L172-L175) |
| **Upstream** | inherited — the four lines are byte-identical to upstream |
| **Report to** | **nobody.** Three of the four implementations do the same thing |
| **Test** | [`hrm/ch7-reset-overlay`](../../hrm/ch7-reset-overlay) |
| **Chapter** | 7 — System Control · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | `RESET-ALL-MEMORY-DISAPPEARS-SWEEP` — red. `OVERLAY-ROM-AT-000000` is green in the same package: the overlay works, the question is how far it reaches |
| **Verdict of the four** | **3–1**: vAmiga, Denise and Copperline each replace only the ROM's own 512K; only WinUAE makes "all memory" literally true |

---

## This is a manual defect, not an RTL defect

The test is red because the manual is this project's oracle, and that is the
correct outcome of the process. But **the finding to carry forward is that
p.223's "all memory" is loose**, not that NanoMig is wrong.

A 512K Kickstart overlaid at address zero covers 512K. What happens to chip RAM
above that is not a consequence the sentence can decide, and three of the four
independent implementations read it the way NanoMig does.

Everything below is evidence for that reading.

---

## What the HRM says

**Printed p. 223**, on the state after a 68000 `RESET` instruction:

> "All memory and AUTOCONFIG(TM) cards disappear"

Read as a universal, no chip-memory address may respond while the overlay is up.

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

**Three of the four replace only the ROM's own 512K**, exactly as NanoMig does.
Only WinUAE mirrors the ROM over the whole low 2 MB and so makes the printed
sentence literally true.

---

## Suggested change

**None is recommended.** Gating banks 1-3 on `ovl` to satisfy p.223 would put
NanoMig alone against three of four implementations, and would make chip RAM
above 512K unreachable during boot on a machine that has it.

If a reader wants to see what the change would be:

```verilog
		sel_chip[1] = cpu_address_in[23:19]==5'b0000_1 && (!ovl || cpu_hlt);
```

and the same for banks 2 and 3. It is written here so that the option is legible,
not because it should be taken.

---

## What holds the finding up

- **The control passes on the same measurement basis**: bank 0 *does* read
  `sel_chip == 0` under the overlay, through the same probe, so the harness can
  observe a bank disappearing.
- **The sweep is one address per bank**, not a single sample, so the result is
  not an artefact of one address.
- **A second assertion in the same package**, `OVERLAY-ROM-AT-000000`, is green:
  the ROM does answer at `$000000` while the overlay is up. The overlay is
  working; the question is only how far it reaches.

### One limit, stated

The test forces `ovl=1` rather than executing a 68000 `RESET` instruction. So it
measures the post-reset **mapping** and not reset propagation; a composed test
that does both is not written. The independent validation raised this, and it
does not change the measured result.

---

## For the specialists

The reading this evidence supports: "all memory disappears" is describing the
programmer's experience on a 512K machine — the only configuration that existed
when the sentence was written — and it does not survive being read as a universal
on a 2 MB machine.

If that is right, the entry to carry forward is a note against p.223 rather than
a change to anyone's RTL. Settling it needs someone who can say what a real Gary
does at `$080000` with `/OVL` asserted.
