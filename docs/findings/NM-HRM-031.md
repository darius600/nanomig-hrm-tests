# NM-HRM-031 — INTREQ bit 14 is a settable request bit where the appendix says enable-only

| | |
|---|---|
| **Status** | open · red |
| **Module** | `paula_intcontroller.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_intcontroller.v#L132-L134) · [Minimig-AGA](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_intcontroller.v#L132-L134) |
| **Report to** | **MiSTer-devel/Minimig-AGA_MiSTer** — the code is upstream's; a fix there must then be ported to NanoMig deliberately, as there is no automatic merge |
| **Test** | [`hrm/ch5-audio-interrupt`](../../hrm/ch5-audio-interrupt) |
| **Chapter** | 5 — Audio · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Verdict of the four** | two of four behave the same way; one masks bit 15 and nothing else, deliberately |

---

## What the HRM says

**Appendix A, printed p. 280**, in the `INTREQ` / `INTENA` bit table:

> `14  INTEN  Master interrupt (enable only, no request)`

Bit 14 is the master interrupt enable. The parenthesis is the whole of the claim:
writing it to `INTREQ` should set no request, because there is no request there
to set.

---

## What NanoMig does

It latches a request bit at 14 like any other, and the module says so in its own
comment:

[`src/minimig-aga/paula_intcontroller.v#L130-L134`](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/minimig-aga/paula_intcontroller.v#L130-L134)

```verilog
	  	intreq[12] <= tmp[12] | syncint;
	  	//external interrupt
	  	intreq[13] <= tmp[13] | int6;
	  	//undocumented interrupt
	  	intreq[14] <= tmp[14];
```

### Measured

| | |
|---|---|
| a `SET` write to `INTREQ` naming only bit 14 | bit 14 reads back **set** in `INTREQR` |
| the control — bit 14 read back when never written | returns `0`, so the fixture is live and the readback is not stuck high |

The enable half of the same appendix line is **green**: bit 14 does gate the
other enables, exactly as the manual says. Only "no request" fails.

---

## What the others say

Nobody decides this but the manual. Every line below was read in a checkout at
the pinned revision and the quotation verified there.

| | code | |
|---|---|---|
| **WinUAE** | — | not read for this finding |
| **vAmiga** | [`Core/Components/Paula/PaulaRegs.cpp:92-93`](https://github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Paula/PaulaRegs.cpp#L92-L93) | <pre>if (setclr) {<br>&nbsp;&nbsp;&nbsp;&nbsp;intreq \|= value;</pre> no mask at all — bit 14 is settable |
| **Denise** | [`emulation/libami/paula/irq.cpp:17-18`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/paula/irq.cpp#L17-L18) | <pre>if (value & 0x8000)<br>&nbsp;&nbsp;&nbsp;&nbsp;intreq \|= value & 0x7fff;</pre> masks **only** bit 15 |
| **Copperline** | — | not read for this finding |
| **MiSTer** | [`rtl/paula_intcontroller.v:132-134`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/paula_intcontroller.v#L132-L134) | descendant — byte-identical to NanoMig, same line numbers, comments included |
| **TC64** | [`rtl/minimig/paula_intcontroller.v:132-134`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/paula_intcontroller.v#L132-L134) | descendant — the same, byte-identical; `paula_intcontroller.v` is identical across all three trees throughout, confirmed by a full-file comparison. The settable bit-14 request is inherited unchanged by both lines |

**Two of the four let bit 14 be set as a request, and Denise's mask is the
precise statement**: `0x7fff` strips the SET/CLR bit and nothing else, so bit 14
survives on purpose rather than by omission. NanoMig is not alone, and the
printed "no request" is the outlier among the implementations that have been
read.

---

## Suggested change

If the appendix is to be followed literally, mask bit 14 out of the request
path the way Denise masks bit 15:

```verilog
	  	//undocumented interrupt -- appendix A p.280 says "enable only, no request"
	  	intreq[14] <= 1'b0;
```

**This is a suggestion, and it is the one least likely to be right.** The RTL's
own comment calls the bit undocumented, which is a claim that hardware has a
request there and the manual omitted it. Two emulators agree with the hardware
reading. Removing the bit to satisfy a 1991 parenthesis could break software
that depends on it — `paula.v`'s notes name *Unreal* as a title that does.

The defensible action may be no code change and a note in the register instead.
That is a decision for someone who knows what real Paula latches.

---

## What holds the finding up

- **The control is on the same fixture and the same measurement basis**: bit 14
  read back without ever being written returns `0`, so a set readback is the
  write arriving and not a stuck bit.
- **The other half of the same sentence passes.** `MASTER-ENABLE-BIT14-GATE` is
  green, so the assertion is not failing because the fixture cannot reach
  `INTENA`/`INTREQ` at all.
- Each half has its own assertion, its own control and its own mutation.

---

## For the specialists

The interesting possibility is that the manual is incomplete rather than NanoMig
wrong. If real Paula latches a request at bit 14, the appendix's parenthesis is
the defect, and two independent emulators behaving the same way is the evidence
pointing that direction. Settling it needs hardware, not another reading.
