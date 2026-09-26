# NM-HRM-051 — Caps Lock is delivered as an ordinary key [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `hid.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/misc/hid.v#L100-L104) · no counterpart upstream |
| **Report to** | **NanoMig.** There is no ancestor line to file this against, and the finding does not generalise to the Minimig family |
| **Test** | [`hrm/ch8-keyboard-events`](../../hrm/ch8-keyboard-events) — `CAPS-LOCK-RELEASE-NOT-ACCEPTED` and `CAPS-LOCK-LED-OFF-PRESS-BIT-ONE`, both red |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 |
| **Assertions** | 2 red of 15. `CAPS-LOCK-DOWN-ACCEPTED` and `CAPS-LOCK-LED-ON-PRESS-BIT-ZERO` are green controls on the same stimulus path |
| **Verdict of the four** | **3–1 against.** WinUAE, Denise and Copperline each implement the rule; vAmiga has no Caps Lock case at all and behaves as NanoMig does |

---

## What the HRM says

**p.359**, the whole paragraph, because both halves of it are load-bearing:

> *"This key is different from all the others in that it generates a keycode only
> when it is pushed down, never when it is released. However, the up/down bit is
> still used. When pushing the Caps Lock key turns on the Caps Lock LED, the
> up/down bit will be 0; when pushing Caps Lock shuts off the LED, the up/down
> bit will be 1."*

**p.360** supplies the initial condition that makes this determinate: the
keyboard shuts the Caps Lock LED off at the end of its start-up sequence. So
after power-on the first press turns the LED on and the second shuts it off.

The rule is therefore a statement about two successive bytes: `$62`, then `$E2`,
and nothing at all in between. The up/down bit on this one key carries the
**lock state**, not the key state.

## What NanoMig does

`hid.v` has no Caps Lock case anywhere. The acceptance condition carries no
per-key branch and no `data_in[7]` branch:

```verilog
  if(state == 4'd0 && amiga_keycode != 7'h7f) begin      // hid.v:101
     kbd_mouse_level <= !kbd_mouse_level;
     kbd_mouse_type  <= 2'd2;
     kbd_mouse_data  <= { data_in[7], amiga_keycode };   // hid.v:104
```

and there is no register anywhere in the module that remembers a press count or
a lock state — `grep -rni caps` over the whole pinned submodule returns exactly
two lines, `minimig.v:139` (a changelog comment about autofire) and
`amiga_keymap.v:79` (the key's table entry), neither of them a branch.

MEASURED over `hid.v` + `amiga_keymap.v`, two press/release cycles of HID `$39`:

| | reported? | `kbd_mouse_data` | bit 7 | p.359 requires |
|---|---|---|---|---|
| press #1 | yes | `$62` | 0 | reported, bit 0 ✔ |
| release #1 | **yes** | `$E2` | 1 | **not reported at all** |
| press #2 | yes | `$62` | **0** | reported, **bit 1** |
| release #2 | **yes** | `$E2` | 1 | **not reported at all** |

Four events where the page specifies two, and the one bit that was supposed to
distinguish the two presses never changes.

### Measured

[`hrm/ch8-keyboard-events`](../../hrm/ch8-keyboard-events) presses and
releases HID `$39` (Caps Lock) twice: every release reports an event —
`CAPS-LOCK-RELEASE-NOT-ACCEPTED`, red — where p.359 says none should fire, and
the second press still carries bit 7 clear — `CAPS-LOCK-LED-OFF-PRESS-BIT-ONE`,
red — where p.359's LED rule needs it set. Two of the package's `13 passed, 2
failed`; `CAPS-LOCK-DOWN-ACCEPTED` and `CAPS-LOCK-LED-ON-PRESS-BIT-ZERO` stay
green as controls on the same stimulus path.

## Two divergences, not one

They fail differently and are graded separately:

- **The release fires.** Software counting keystrokes sees twice as many Caps
  Lock events as it should.
- **The value on the second press is wrong.** Software reading the up/down bit
  to learn the lock state — which is the only thing the bit means on this key —
  reads "on" forever.

The first press is correct under the page's own rule and is graded green as
`CAPS-LOCK-LED-ON-PRESS-BIT-ZERO`. That is what makes the second press's `0`
a measurement rather than an assumption: the same path, the same key, one press
apart.

## What the others say

| | |
|---|---|
| **WinUAE** | **Conformant, by mirroring the host.** `target_checkcapslock` replaces the physical up/down state with the host machine's own caps-lock state before the event is translated (`od-win32/keyboard_win32.cpp:573`, `od-unix/input.cpp:1402-1418`), so the byte's up/down bit follows the lock and not the key. In raw keyboard modes (`currprefs.keyboard_mode > 0`) it returns early and passes the key through unchanged |
| **vAmiga** | **Not conformant — the same shape as NanoMig.** `Core/Peripherals/Keyboard/Keyboard.cpp` has no `0x62` case: `press()` queues the code and `release()` queues `keycode \| 0x80` for every key alike. Its one lock mechanism (`lock()` / `isLocked()`) exists for the Cmd/Amiga key combination, not for Caps Lock |
| **Denise** | **Conformant, both halves, explicitly.** The release returns before emitting anything — `if (stroke == 0x62) // Caps Lock release  return;` (`emulation/libami/input/keyboard/keyboard.cpp:203-204`) — and the press toggles the modelled LED and sets bit 7 when that press turns it off (`:230-234`) |
| **Copperline** | **Conformant, and documented as the keyboard's own behaviour.** The `caps_lock_on` field's comment reads *"owned by the keyboard: the key toggles it, sending a press code on toggle-on and a release code on toggle-off; the physical key release sends nothing"* (`src/chipset/keyboard.rs:272-275`) |
| **MiSTer** | descendant — **no counterpart.** No `hid.v` and no `keymap` in the pinned tree: `rtl/minimig.v:207` takes `kbd_mouse_data` as a top-level input and the keyboard's own behaviour is decided in software above the chipset |
| **TC64** | descendant — **no counterpart either**: no `hid.v` in its `rtl/minimig/` |

Three of the four independent implementations arrive at the page's byte
sequence by three different routes — mirroring the host's lock, modelling the
LED in the keyboard, or owning the toggle in a keyboard MCU. vAmiga does not
implement the rule, which is worth saying plainly: this is not a case where
everyone else agrees and NanoMig alone is out.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

None of them speaks to this point.
Silent on this point: Commodore, *A500/A2000 Technical Reference Manual*, 1987; Commodore, *A500/A2000 Gary Specification*, undated; US patent 4,829,473, Commodore-Amiga, filed 1986, granted 1989.

