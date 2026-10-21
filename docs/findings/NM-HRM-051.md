# NM-HRM-051 — Caps Lock is delivered as an ordinary key [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should NanoMig's `hid.v` report no event when Caps Lock (HID `$39`, Amiga `$62`) is released, and give the second press bit 7 set (`$E2`), as p.359 prints? Closes when `hid.v` does both and `CAPS-LOCK-RELEASE-NOT-ACCEPTED` and `CAPS-LOCK-LED-OFF-PRESS-BIT-ONE` turn green, or when an Amiga specialist rejects the change. |
| **Report to** | `NanoMig direct` — harbaum/NanoMig |
| **Scope** | NanoMig's `hid.v` keyboard-event path for HID `$39`: the release after the first press, and the second press, after reset; real keyboard hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · Appendix G, printed p. 359 (Caps Lock rule) and p. 360 (LED off after start-up). Claim ids, each with the assertion that carries it: `CAPS-LOCK-DOWN-ACCEPTED` `opus:8-appG.c51`, `sol:8-appG.c63`; `CAPS-LOCK-RELEASE-NOT-ACCEPTED` `opus:8-appG.c52`, `sol:8-appG.c64`; `CAPS-LOCK-BIT7-CARRIES-LED-STATE` `opus:8-appG.c53`, `sol:8-appG.c65`; `CAPS-LOCK-LED-ON-PRESS-BIT-ZERO` `opus:8-appG.c54`, `sol:8-appG.c66`; `CAPS-LOCK-LED-OFF-PRESS-BIT-ONE` `opus:8-appG.c55`, `sol:8-appG.c67`. `CONTROL-FIXTURE-DELIVERS-KNOWN-VALUE` and `CONTROL-NO-EVENT-ON-UNMAPPED-CODE` carry no claim id |
| **Module** | `hid.v` — [NanoMig `4315acb`](https://github.com/harbaum/NanoMig/blob/4315acba5e9f1e15103037627aa78b3460cae8af/src/misc/hid.v#L100-L104) · no counterpart in MiSTer; TC64 has a Caps Lock rule of its own; NanoMig's own |
| **Test** | [`hrm/ch8-keyboard-events`](../../hrm/ch8-keyboard-events): `make -C hrm/ch8-keyboard-events test`; an expected red exits nonzero |
| **Assertions** | 2 red of 15: `CAPS-LOCK-RELEASE-NOT-ACCEPTED` expected no event on release, observed an event; `CAPS-LOCK-LED-OFF-PRESS-BIT-ONE` expected bit 7 set on the second press, observed clear; the package reports `13 passed, 2 failed`. `CAPS-LOCK-DOWN-ACCEPTED` and `CAPS-LOCK-LED-ON-PRESS-BIT-ZERO` are green controls on the same stimulus path |
| **Verdict of the four** | **3–1 against.** WinUAE, Denise and Copperline each implement the rule; vAmiga has no Caps Lock case at all and behaves as NanoMig does. Hardware: not measured |
| **Effort** | S — `hid.v` only: a per-key branch for `$62` and a register that remembers a press count or lock state |

---

## Source requirement

**p.359**, the whole paragraph, because both halves of it are load-bearing:

> *"This key is different from all the others in that it generates a keycode only
> when it is pushed down, never when it is released. However, the up/down bit is
> still used. When pushing the Caps Lock key turns on the Caps Lock LED, the
> up/down bit will be 0; when pushing Caps Lock shuts off the LED, the up/down
> bit will be 1."*

**p.360**: the keyboard shuts the Caps Lock LED off at the end of its start-up
sequence, so after power-on the first press turns the LED on and the second shuts
it off. The rule is two successive bytes, `$62` then `$E2`, and nothing in
between; on this key the up/down bit carries the lock state, not the key state.

## RTL and measurement

`hid.v` has no Caps Lock case, no `data_in[7]` branch and no register that remembers a press count or a lock state (`grep -rni caps` over
the pinned HDL sources finds only `minimig.v:139`, a changelog comment, and
`amiga_keymap.v:79`, the table entry):

```verilog
  if(state == 4'd0 && amiga_keycode != 7'h7f) begin      // hid.v:101
     kbd_mouse_level <= !kbd_mouse_level;
     kbd_mouse_type  <= 2'd2;
     kbd_mouse_data  <= { data_in[7], amiga_keycode };   // hid.v:104
```

What `hid.v:101-104` and `amiga_keymap.v:79` give for two press/release cycles of
HID `$39`; the package measures the first three rows:

| | reported? | `kbd_mouse_data` | bit 7 | p.359 requires |
|---|---|---|---|---|
| press #1 | yes | `$62` | 0 | reported, bit 0 ✔ |
| release #1 | **yes** | `$E2` | 1 | **not reported at all** |
| press #2 | yes | `$62` | **0** | reported, **bit 1** |
| release #2 | **yes** | `$E2` | 1 | **not reported at all** |

Two divergences, graded separately. The release fires
(`CAPS-LOCK-RELEASE-NOT-ACCEPTED`, red): software counting keystrokes would see
twice as many Caps Lock events as it should. The second press carries bit 7
clear (`CAPS-LOCK-LED-OFF-PRESS-BIT-ONE`, red): software reading the up/down bit
to learn the lock state would read "on" forever. The first press is correct
under the page's own rule and green as `CAPS-LOCK-LED-ON-PRESS-BIT-ZERO`, which
makes the second press's `0` a measurement: same path, same key, one press apart.

## Risks and unknowns

- Real keyboard behaviour is not measured, and no named software depends on it.
- The vote is split: vAmiga has no Caps Lock case at all, so this is not a case
  where everyone else agrees and NanoMig alone is out.
- TC64 was read from source, not run; it sends the second `$E2` on the second
  release rather than on the second press.
- WinUAE passes the key through unchanged in raw keyboard modes
  (`currprefs.keyboard_mode > 0`, `od-win32/keyboard_win32.cpp:578`, `od-unix/input.cpp:1407`).
- Release #2 is read from the code, not asserted.

## Independent evidence

| | |
|---|---|
| **WinUAE** | **Conformant, by mirroring the host.** `target_checkcapslock` drops the key's release (`if (*state == 0) return -1;`, which its caller skips with `continue`, [`inputdevice.cpp:8565-8567`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/inputdevice.cpp#L8565-L8567)) and replaces the press's up/down state with the host's caps-lock state before translation ([`od-win32/keyboard_win32.cpp:573-598`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-win32/keyboard_win32.cpp#L573-L598), [`od-unix/input.cpp:1402-1418`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/od-unix/input.cpp#L1402-L1418)) |
| **vAmiga** | **Not conformant — the same shape as NanoMig.** `Core/Peripherals/Keyboard/Keyboard.cpp` has no `0x62` case: `press()` queues the code and `release()` queues `keycode \| 0x80` for every key alike. Its one lock mechanism (`lock()` / `isLocked()`) serves the Cmd/Amiga key combination, not Caps Lock |
| **Denise** | **Conformant, explicitly.** The release returns before emitting anything — `if (stroke == 0x62) // Caps Lock release  return;` ([`emulation/libami/input/keyboard/keyboard.cpp:203-204`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/keyboard/keyboard.cpp#L203-L204)) — and the press toggles the modelled LED and sets bit 7 when that press turns it off ([`:230-234`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/keyboard/keyboard.cpp#L230-L234)) |
| **Copperline** | **Conformant, documented as the keyboard's own behaviour.** The `caps_lock_on` comment reads *"owned by the keyboard: the key toggles it, sending a press code on toggle-on and a release code on toggle-off; the physical key release sends nothing"* ([`src/chipset/keyboard.rs:272-275`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/keyboard.rs#L272-L275)) |
| **MiSTer** | descendant — **no counterpart.** No `hid.v` and no `keymap` in the pinned tree: [`rtl/minimig.v:207`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L207) takes `kbd_mouse_data` as a top-level input and the keyboard's behaviour is decided in software above the chipset |
| **TC64** | descendant — no `hid.v`, but a Caps Lock rule of its own in the builds with `MINIMIG_PS2_KEYBOARD` ([`rtl/minimig/amiga_keyboard.v:70`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/amiga_keyboard.v#L70)): `ciaa_ps2keyboard.v` toggles a `capslock` register on each press and suppresses every Caps Lock event while it is set ([`rtl/minimig/ciaa_ps2keyboard.v:351-366`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa_ps2keyboard.v#L351-L366)). The first press sends `$62`, the first release and second press send nothing, the second release sends `$E2`. Its MiST builds take `kbd_mouse_data` already decoded ([`rtl/minimig/amiga_keyboard.v:156-202`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/amiga_keyboard.v#L156-L202)) |

## Decision

No suggested change is filed on this page: the two reds are the observable difference. Expected red `CAPS-LOCK-RELEASE-NOT-ACCEPTED` and `CAPS-LOCK-LED-OFF-PRESS-BIT-ONE`, green `CAPS-LOCK-DOWN-ACCEPTED` and `CAPS-LOCK-LED-ON-PRESS-BIT-ZERO`.
