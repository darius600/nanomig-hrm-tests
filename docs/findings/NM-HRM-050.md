# NM-HRM-050 — one of the two international cutout keys reaches no code [RTL review]

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a test fails against the page |
| **What is asked of a reader** | Should the `keymap` entry for HID `$64` (`amiga_keymap.v:130`, the key cut out of the left Shift) emit `$30`, as p.252 prints, instead of `$2B`? Closes when NanoMig makes that change and `CUTOUT-30` turns green, or when an Amiga specialist rejects it. |
| **Report to** | `NanoMig direct` — harbaum/NanoMig |
| **Scope** | `keymap` entries for HID `$64` and `$32`, the two international cutout keys; a combinational translation table, so no chip revision applies; hardware not measured |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware, printed p.252 (text) and p.254 (Figures 8-9 and 8-10) · *Amiga Hardware Reference Manual*, 3rd ed., 1991. Claim ids: `CUTOUT-30`: p. 254: `opus:8-fig.c59`, `sol:8-layout.c58`, `opus:8-fig.c148`, `sol:8-layout.c152`; p. 252: `opus:8-keycode.c14`, `sol:8-keycode.c13`; `KEY-$2B`: p. 254: `opus:8-fig.c146`, `sol:8-layout.c150`; p. 252: `opus:8-keycode.c15`, `sol:8-keycode.c14`. Both assertions this page names carry claim ids |
| **Module** | `amiga_keymap.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/misc/amiga_keymap.v#L70-L130) · no counterpart in MiSTer; TC64 has a separate PS/2 map that emits both codes; NanoMig's own |
| **Test** | [`hrm/ch8-keycode-table`](../../hrm/ch8-keycode-table): `make -C hrm/ch8-keycode-table test`; an expected red exits nonzero |
| **Assertions** | 1 red, `CUTOUT-30`: expected `$30` for HID `$64`, observed `$2B`; the package reports `97 passed, 1 failed` (98 assertions, among them a 94-row exhaustive sweep of the table). Control `KEY-$2B`, green: HID `$32` gives `$2B` |
| **Verdict of the four** | **unanimous against.** All four carry both cutout keys as distinct codes, `$30` and `$2B`, in their own key tables. Hardware: not measured |
| **Effort** | S — the output of one table entry, `amiga_keymap.v:130` for HID `$64` |

---

## Source requirement

**p.252**:

> "The international keyboards have two more keys that are "cut out" of
> larger keys on the USA version. These are $30, cut out from the the left shift,
> and $2B, cut out from the return key."

Figure 8-9 (A1000) shades one key, `30`, beside the left shift. Figure 8-10
(A500/2000/3000) shades two, `30` beside the left shift and `2B` beside Return.

## RTL and measurement

`keymap` is a single combinational table, `code` in and `amiga` out; its 98
entries and its `7'h7f` default, read in full, show that **no input produces
`$30`**. Both international keys produce `$2B`:

```verilog
  (code == 7'h32)?7'h2b:   // 32: EUR-1      amiga_keymap.v:72
  (code == 7'h64)?7'h2b:   // 64: EUR-2      amiga_keymap.v:130
```

The file's only other `7'h30` is on the input side, the HID code for `]`, which
maps to `$1b` (`:70`). The key cut out of the left shift is unreachable, and the
two keys the page distinguishes are collapsed onto one code.

`keymap`'s input side is the FPGA Companion code space, *"mainly the USB HID key
codes with the modifier keys mapped into the 0x68+ range"*; FPGA Companion's
`core_map_key` returns the usage code unchanged for a non-legacy core
(`src/core.c:60-82`), which NanoMig is: core id 0, *"Generic core"*
(`src/misc/sysctrl.v:186`). The USB HID usage names, read from Denise's own
HID-indexed table ([`driver/input/iokit/keyNames.cpp:149`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/driver/input/iokit/keyNames.cpp#L149) and [`:199`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/driver/input/iokit/keyNames.cpp#L199)), bind the
two stimuli to the two keys:

| HID usage | USB HID name | where the key sits | Amiga code | NanoMig gives |
|---|---|---|---|---|
| `$32` | Keyboard Non-US `#` and `~` | beside Return | `$2B` | `$2B` ✔ |
| `$64` | Keyboard Non-US `\` and `\|` | beside left Shift | `$30` | `$2B` ✘ |

[`hrm/ch8-keycode-table`](../../hrm/ch8-keycode-table) drives `code` with each
key's input and reads `amiga`: HID `$64` returns `$2B` instead of `$30`
(`CUTOUT-30`, red), the same value HID `$32` returns (`KEY-$2B`, green, same
measurement basis). The green control on the same stimulus path rules out a
units error or a broken fixture. So `amiga_keymap.v:130` is the single wrong
line, and `$30` is the value it should carry; the entry at `:72` is right.

## Risks and unknowns

- The binding of the two stimuli to the two keys rests on the USB HID usage
  names above, not on `amiga_keymap.v`'s own `EUR-1`/`EUR-2` labels. Both
  entries emit `$2B` whichever is driven, so the binding does not change the
  colours; it decides that the red is against the key p.252 names.
- The finding does not generalise to the Minimig family: the module has no
  counterpart in MiSTer.
- Real keyboard hardware is not measured; no named software depends on it.
- The 1987 *Technical Reference Manual* table stops at `5F`, so it does not
  itself name key 60 as the left shift.

## Independent evidence

| | |
|---|---|
| **WinUAE** | Both, named and distinct: `AK_LTGT 0x30` and `AK_NUMBERSIGN 0x2B` ([`include/keyboard.h:119-120`](https://github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/include/keyboard.h#L119-L120)) |
| **vAmiga** | Both, named and distinct: `laceBrace = 0x30` and `hashtag = 0x2B` (`GUI/Input/AmigaKey.swift:97-98`) |
| **Denise** | Both, adjacent in its key table: `0x2b, 0x30` ([`emulation/libami/input/keyboard/keyboard.cpp:19`](https://github.com/piciji/denise/blob/1cb7d45117893c76025d32260edb6008001344f9/emulation/libami/input/keyboard/keyboard.cpp#L19)) |
| **Copperline** | `0x30` sits in the A500 key matrix taken from the keyboard's own wiring ([`src/chipset/keyboard.rs:133`](https://github.com/CopperlineHQ/Copperline/blob/726bed00c407727c1f5bd32c7193587986638941/src/chipset/keyboard.rs#L133)) |
| **MiSTer** | descendant — **no counterpart.** The pinned Minimig-AGA tree has no `amiga_keymap.v`, no `keymap` and no `hid.v`: [`rtl/minimig.v:207`](https://github.com/MiSTer-devel/Minimig-AGA_MiSTer/blob/da7632e44498840850e78966947f8770166e6e77/rtl/minimig.v#L207) takes the already-decoded `kbd_mouse_data` as a top-level input, and the translation happens in software above the chipset |
| **TC64** | descendant — no `amiga_keymap.v`, but a keymap of its own: builds with `MINIMIG_PS2_KEYBOARD` ([`rtl/minimig/amiga_keyboard.v:70`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/amiga_keyboard.v#L70)), among them the Chameleon boards, translate PS/2 scan codes in `rtl/minimig/ciaa_ps2keyboard_map.v`, which gives the two cutout keys two codes, `$2B` ([`:506`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa_ps2keyboard_map.v#L506)) and `$30` ([`:510`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/ciaa_ps2keyboard_map.v#L510)), as the page does. Its MiST builds take `kbd_mouse_data` already decoded ([`rtl/minimig/amiga_keyboard.v:156-202`](https://github.com/minimig-dev/MinimigAGA-MiST-TC64/blob/9e8e30844fe134dca54cea65f1bd38539dfc15ad/rtl/minimig/amiga_keyboard.v#L156-L202)) |

### What other Commodore documents say

- **Commodore, *A500/A2000 Technical Reference Manual*, 1987**, PDF page 5 — agrees with the manual:

  > "Note: On the U.S. keyboard, the keys with codes 44 and 60 are extended to include the European keys with codes 2B and 30, respectively."
  <!-- corpus: a500-a2000-trm-1987/p005.txt -->

  Two European keys with two distinct codes: $2B cut from key 44 and $30 from key 60. Table 1-1 (p014) gives 44 as RETURN and lists 2B and 30 as separate reserved entries. That is the manual's pair of codes, with $2B from Return. It contradicts NanoMig, which maps both international keys to $2B and never produces $30.

## Decision

**Suggested change:** `amiga_keymap.v:130`, `7'h2b` to `7'h30`; leave `:72` as it is. `CUTOUT-30` is the assertion that would turn green.
