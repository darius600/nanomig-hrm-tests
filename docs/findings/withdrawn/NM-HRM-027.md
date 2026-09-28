# NM-HRM-027 — in direct mode the interrupt is raised before a word is output

| | |
|---|---|
| **Lifecycle** | withdrawn |
| **Filed against** | — |
| **Evidence** | none |
| **What is asked of a reader** | Nothing. The entry is kept so the claim is not raised a second time. Figure 5-8 raises the interrupt at the write, where NanoMig raises it. |
| **Module** | `paula_audio_channel.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L283-L285) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L283-L285) |
| **Test** | [`hrm/ch5-audio-direct-output`](../../../hrm/ch5-audio-direct-output) — its `5-p157.c13` / `5-direct.c4` assertion requires an AUDxIR pulse at the word transition that ends word 1 (p. 157, p. 164, Figure 5-8's 011→010 arc) and no longer forbids the one Figure 5-8 draws at the write; measured at tick 176, green; `12 passed, 0 failed` |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Error class** | **a sentence graded without the figure that completes it** |
| **Verdict of the four** | **unanimous with NanoMig and with Figure 5-8**: WinUAE, vAmiga, Denise and Copperline all raise the interrupt at the write that starts direct output, and again at every word transition; Copperline asserts both in its own tests |
| **Boundary** | at the pins — `intreq` and `sample` are both top-level ports |

---

## Why it was withdrawn

The assertion grades one sentence of printed p. 157, "the interrupt occurs
after one data word has been output", as a rule that **no** interrupt may come
before the first word has been output. The page does not say that.

**Figure 5-8, two pages later, draws the interrupt request as an action of the
write itself**: the arc from the idle state 000 to 010 that a processor write
takes carries `AUDxIR`. It draws the request again on every return from 011 to
010 with DMA off, and p. 164 says the same in words: "Interrupts are generated
on every word transition". That second interrupt is the one p. 157 describes,
the one that comes after a data word has been output.

NanoMig raises both, at the moments the figure puts them: at the write
(measured at tick 80) and at each word transition, as the low byte's period
expires (tick 176, the low byte having reached the output at 136; then every 96
ticks). So do all four independent implementations. Read together with the
figure and p. 164, the manual predicts what NanoMig does.

---

## What the HRM says

**Printed p. 157**, on direct (non-DMA, CPU-driven) audio output:

> "To use direct audio output, do not enable the DMA for the audio channel you
> wish to use; this changes the timing of the interrupts. The normal interrupt
> occurs after a data address has been read; in direct audio output, the
> interrupt occurs after one data word has been output."

**Printed p. 164**, the same path in the state machine's terms:

> "In interrupt-driven operation, transfer to the main loop (states 010 and 011)
> occurs immediately after data is written by the processor. In the 010 state
> the upper byte is output, and in the 011 state the lower byte is output. ...
> Interrupts are generated on every word transition (011→010)."

**Figure 5-8, printed p. 166** (read from the drawing, not quoted; the legend
says brackets mark actions and parentheses causes):

- the arc from `000` to `010` has the cause `(AUDxDAT · AUDxON · AUDxIP)`, the
  last two barred (a write with DMA off and no interrupt pending), and the
  actions `[volcntrld, percntrld, pbufld1, AUDxIR]`: the request is raised on the
  write's own transition, as the machine enters `010`, where the upper byte is
  output;
- the arc from `011` back to `010` has the cause `(perfin · (AUDxON + AUDxIP))`,
  `AUDxIP` barred, and among its actions `AUDxIR if napnav & AUDxON`, `AUDxON`
  barred: with DMA off, every word transition raises the request again.

So in direct mode the manual names two moments, and they do not contradict each
other: one at the write (the figure), one at every word transition (p. 164, the
figure, and p. 157's "after one data word has been output"). p. 157 says nothing
about an interrupt at the write.

The other statements of the interrupt's moment describe DMA-driven operation,
which p. 157 itself sets apart ("this changes the timing of the interrupts"):

| printed page | mode | the moment |
|---|---|---|
| 147 | DMA | after the location and length are copied to the back-up registers |
| 164 | DMA | as the last word of the waveform starts its output |
| appendix A, 265 | DMA | when words output equals the length **and** the register's data has been used |
| **157** | **direct** | **after one data word has been output** |
| **164** | **direct** | **on every word transition, 011→010** |
| **Figure 5-8, 166** | **direct** | **on the write's own arc, 000→010, and on every 011→010** |

---

## What NanoMig does

The branch leaving the idle state on a CPU write to `AUDxDAT` raises the
interrupt request in the same step, as Figure 5-8's arc does:

[`src/minimig-aga/paula_audio_channel.v#L281-L285`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L281-L285)

```verilog
else if (AUDxDAT && !AUDxON && !AUDxIP) begin
  audio_next = AUDIO_STATE_3;
  AUDxDR = 1'b0;
  AUDxIR = 1'b1;
```

The return from the low-byte state to the high-byte state raises it again,
unconditionally while DMA is off:

[`src/minimig-aga/paula_audio_channel.v#L421-L427`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L421-L427)

```verilog
if (perfin && (AUDxON || !AUDxIP)) //period counter expired and audio DMA active
begin
  audio_next = AUDIO_STATE_3;
  AUDxDR = AUDxON;
  AUDxIR = (intreq2 & AUDxON) | ~AUDxON;
  intreq2_clr = intreq2;
  pbufld1 = 1'b1;
```

NanoMig numbers that low-byte state `110`, not the figure's `011`
([NM-HRM-026](../NM-HRM-026.md)); the transition is the same one. `AUDxIR` is the
interrupt port itself:

[`src/minimig-aga/paula_audio_channel.v#L133`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/minimig-aga/paula_audio_channel.v#L133)

```verilog
assign intreq = AUDxIR;
```

### Measured

The package's baseline run up to its second write: `0xA5C3` written with DMA
off and `intpen` low. Ticks are the harness's, eight to a colour clock.

| event | tick |
|---|---|
| the write reaches the channel (`AUDxDAT`, state 000) and `intreq` pulses | **80** |
| the word's high byte reaches `sample` (state 010) | 88 |
| its low byte reaches `sample` | 136 |
| the low byte's period expires and `intreq` pulses | **176** |
| the high byte reaches `sample` again (no second word written yet) | 184 |

The pulse at 80 is Figure 5-8's `000→010` action. The pulse at 176 is the word
transition of p. 164 and comes after one data word has been output, as p. 157
says; it recurs every 96 ticks (272, 368, ...) while `intpen` stays low.

---

## What the others say

Every line below was read in a local checkout at the pinned revision named in
the link.

| | code | |
|---|---|---|
| **WinUAE** | [`audio.cpp:1833-1835`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1833-L1835) | <pre>} else if (cdp-&gt;dat_written && !isirq (nr)) {<br>&nbsp;&nbsp;&nbsp;&nbsp;cdp-&gt;state = 2;<br>&nbsp;&nbsp;&nbsp;&nbsp;setirq(nr, 1);</pre> a CPU write with DMA off reaches the state machine at once ([`:2562-2567`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L2562-L2567)), and leaving idle raises the interrupt as it enters state 2, WinUAE's `010`; `setirq` posts it one colour clock later, `INTREQ_INT(nr + 7, CYCLE_UNIT);` ([`:1573-1574`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1573-L1574)); leaving state 3 with DMA off raises it again, `if (napnav) { setirq(nr, 32);` ([`:2005-2006`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L2005-L2006)) |
| **vAmiga** | [`Core/Components/Paula/Audio/StateMachineRegs.cpp:84-89`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachineRegs.cpp#L84-L89) | `if (!AUDxIP()) move_000_010();` in IRQ mode, and `move_000_010` performs the figure's four actions ([`StateMachine.cpp:198-212`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L198-L212)): <pre>volcntrld();<br>percntrld();<br>pbufld1();<br>AUDxIR();</pre> `AUDxIR()` schedules the interrupt one DMA cycle later, `paula.scheduleIrqRel(IrqSource::AUD0, DMA_CYCLES(1));` ([`:96-104`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L96-L104)); `move_011_010` calls it again in IRQ mode ([`:344-345`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/Audio/StateMachine.cpp#L344-L345)) |
| **Denise** | [`emulation/libami/paula/audio.cpp:120-127`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L120-L127) | <pre>if (cha.state == 0) {<br>&nbsp;&nbsp;&nbsp;&nbsp;if (!AUDxIP(nr)) {<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;percntrld&lt;nr, true&gt;();<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;volcntrld()<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;pbufld1&lt;nr&gt;();<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;AUDxIR(nr)</pre> the DMA-off write from idle, with the figure's actions; the return from state 3 with DMA off raises it again, `} else AUDxIR(nr)` ([`:208-209`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/audio.cpp#L208-L209)); the request is posted one clock later, `int64_t nextClock = agnus.clock + 1;` ([`irq.cpp:54-55`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/irq.cpp#L54-L55)) |
| **Copperline** | [`src/chipset/paula.rs:1856-1864`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1856-L1864) | `/// 000 -> 010 (IRQ mode): a CPU AUDxDAT write starts direct output.` and the transition ends in `self.latch_interrupt_sources(INT_AUDX[ch]);`, reached from an idle channel with no interrupt pending ([`:1977-1978`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1977-L1978)); `// IRQ mode: every word start raises the interrupt` on the 011→010 transition ([`:1930-1933`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L1930-L1933)); its own test asserts the first right after the write, `assert_eq!(paula.intreq & INT_AUD0, INT_AUD0);` ([`:2806-2809`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2806-L2809)), and the second at the word boundary ([`:2842-2845`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/paula.rs#L2842-L2845)) |
| **MiSTer** | [`rtl/paula_audio_channel.v:281-285`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L281-L285), [`:421-425`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_audio_channel.v#L421-L425) | descendant — byte-identical to NanoMig, same line numbers: `AUDxIR = 1'b1;` on the idle exit and `AUDxIR = (intreq2 & AUDxON) \| ~AUDxON;` on the word transition |
| **TC64** | [`rtl/minimig/paula_audio_channel.v:258-262`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L258-L262), [`:398-402`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_audio_channel.v#L398-L402) | descendant — the same two branches: `AUDxIR = 1'b1;` on the idle exit and `AUDxIR = (intreq2 & AUDxON) \| ~AUDxON;` on the word transition |

**All four independent implementations raise the interrupt at the write that
starts direct output**, WinUAE, vAmiga and Denise one clock after it and
Copperline at once, and **all four raise it again at every word transition**
while DMA is off. vAmiga, Denise and Copperline carry the figure's own action
list on the idle exit. MiSTer and TC64 share NanoMig's code and confirm the
ancestry, not the reading.

---

## What the test measures

The failing assertion in `hrm/ch5-audio-direct-output` takes the **first**
`AUDxIR` after the write and requires it no earlier than the word's low byte
(`test_ch5_audio_direct_output.cpp:713`):

```cpp
bool per_manual = (firstAudxir >= 0 && tLo1 >= 0 && firstAudxir >= tLo1);
```

An implementation that follows Figure 5-8 raises its first `AUDxIR` on the
`000→010` arc, at the write, and cannot pass it. The interrupt p. 157 names is
the one at tick 176, after the low byte at 136.

`hrm/ch5-audio-channel-state` grades the figure's arc as `F58.t000_010` and
passes it, but its predicate (`test_ch5_audio_channel_state.cpp:880`) checks
the arc's cause and destination only, not the `AUDxIR` action its text names.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](../README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *AmigaMail* vol. 1, audio cutoff filter, 1989; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989; 6502.org dissection of Paula 8364R7, 2023.

---

## Why this entry is kept

p. 157's sentence, read on its own, sounds like a rule about the first word,
and the measured interrupt at the write looks like a violation of it. The state
diagram two pages later draws that interrupt as an action of the write, and all
four implementations raise it there. A reader who grades p. 157 without Figure
5-8 will find the same red again; this page is where they find the figure.
