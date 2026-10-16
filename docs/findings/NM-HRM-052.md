# NM-HRM-052 — p.252 and p.254 disagree about the right Alt key

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the printed page |
| **Evidence** | none |
| **What is asked of a reader** | No RTL change follows from this. Is the right Alt key code `$65`, as p.252's keycode table and p.363's table of independently readable keys print, rather than `64`, as both keyboard drawings on p.254 print? NanoMig returns `$65` and so do all four implementations; the drawings are the likely defect. Closes when an Amiga specialist records the `64` on the right Alt in Figures 8-9 and 8-10 as a misprint, or confirms that a real keyboard sends `$64` for it, in which case this page is refiled against the RTL. |
| **Report to** | `no RTL action` — HRM documentation: whoever keeps errata for Chapter 8 and Appendix G of the *Amiga Hardware Reference Manual*, 3rd ed. |
| **Scope** | the right Alt key code in `amiga_keymap.v` (HID `$6E` to `$65`) against p.252, p.363 and the keyboard drawings of the A1000 (Figure 8-9) and the A500/2000/3000 (Figure 8-10); which code a real keyboard sends is unknown |
| **Software impact** | unknown; no named affected software |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed., 1991 · keycode table printed p.252, Figures 8-9 and 8-10 printed p.254, Appendix G printed p.363 · `RIGHTALT-CONFLICT` (a `record()` line, not an assertion) carries six claim ids: `opus:8-fig.c75`, `sol:8-layout.c77`, `opus:8-fig.c164`, `sol:8-layout.c168` (the p.254 drawings) and `opus:8-keycode.c42`, `sol:8-keycode.c67` (p.252); p.363 carries no claim id |
| **Module** | `amiga_keymap.v` — [NanoMig `4495f86`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/misc/amiga_keymap.v#L135-L139) · no counterpart in MiSTer; TC64 has a separate PS/2 map that takes the table too; NanoMig's own |
| **Test** | [`hrm/ch8-keycode-table`](../../hrm/ch8-keycode-table): `make -C hrm/ch8-keycode-table test`; an expected red exits nonzero. `RIGHTALT-CONFLICT` is recorded, **graded on neither side** |
| **Assertions** | none, deliberately. The package records that `$6E` returns `$65` and declines the six claims on one `record()` line; it reports `97 passed, 1 failed`, the red being NM-HRM-050's `CUTOUT-30` |
| **Verdict of the four** | **unanimous with NanoMig and with p.252 and p.363**: all four name `$65` for the right Alt, and none takes the `64` both p.254 drawings print — evidence about which reading practitioners took, not about which page is right. Hardware: not measured |
| **Effort** | none — no RTL change is asked |
| **See also** | [NM-HRM-044](NM-HRM-044.md), the same kind of disagreement between two printed pages; [NM-HRM-050](NM-HRM-050.md), which rests on the same two drawings |

---

## Source requirement

**p.252**, the keycode table, lists the qualifier keys one per line:

> 64      Left Alt
> 65      Right Alt

**Appendix G, printed p.363**, gives the same codes in its table of the keys that "are independently readable", one per bit: `LEFT ALT (64)` in bit 5, `RIGHT ALT (65)` in bit 1.

**p.254** draws two keyboards, Figure 8-9 the A1000 and Figure 8-10 the A500/2000/3000, with each key's code printed on its cap. Both figures print `64` on both ALT keys, the one left of the space bar and the one right of it: the bottom row of Figure 8-9 runs `ALT 64`, `A 66`, `40`, `A 67`, `ALT 64`, and Figure 8-10's runs the same.

## RTL and measurement

NanoMig follows the table, in [`amiga_keymap.v:135-139`](https://github.com/harbaum/NanoMig/blob/4495f86fbb0611e53649165faa36f1125e062f18/src/misc/amiga_keymap.v#L135-L139):

```verilog
  (code == 7'h6a)?7'h64:   // left alt        amiga_keymap.v:135
  (code == 7'h6e)?7'h65:   // right alt       amiga_keymap.v:139
```

Nothing is graded. `RIGHTALT-CONFLICT` in [`hrm/ch8-keycode-table`](../../hrm/ch8-keycode-table) records that the right-Alt input returns `$65`, matching p.252 and contradicting both p.254 figures. Its six claims, four of them flagged `source_defines: conflict` by the classification and two recorded against p.252 without the flag, are declined together on that one line, and none of them appears on a graded line.

## Risks and unknowns

- **No hardware settles it.** A real A1000 or A500 would; the manual itself gives the key two codes.
- **The agreement of four implementations is evidence about what implementers read**, not about what the hardware did, and it does not license grading a test against p.252.
- **p.363 and both p.254 figures were read from the images**, not from the text layer.
- **The drawings are the likely defect, not a settled one.** The same two drawings are the primary evidence for [NM-HRM-050](NM-HRM-050.md): both shade the international cutout keys, in different places, Figure 8-9 `30` alone and Figure 8-10 `30` and `2B`; there the figures and p.252 agree with each other and NanoMig disagrees with both.

## Independent evidence

| | code | |
|---|---|---|
| **WinUAE** | `include/keyboard.h:103-104` | `AK_LALT 0x64`, `AK_RALT 0x65` |
| **vAmiga** | `GUI/Input/AmigaKey.swift:136-137` | `leftAlt = 0x64`, `rightAlt = 0x65` |
| **Denise** | `emulation/libami/input/keyboard/keyboard.cpp:18` | `0x64, 0x65` adjacent in the modifier group of its key table |
| **Copperline** | `src/video/window/host_input.rs:185-186` | `AltLeft => 0x64`, `AltRight => 0x65` |
| **MiSTer** | none | descendant — not applicable: `amiga_keymap.v` is absent from the pinned `rtl/`, so there is no keymap to consult |
| **TC64** | `rtl/minimig/ciaa_ps2keyboard_map.v:430`, `:686` | descendant — no `amiga_keymap.v`, but the keymap its builds with `MINIMIG_PS2_KEYBOARD` use takes the table too: `$64` for the left Alt and `$65` for the right |

## Decision

No RTL change. A conformance test needs an oracle, and here the manual speaks with two voices on the same fact: grading p.252 would report a defect the figures deny, grading the figures one the table denies, the same reasoning as [NM-HRM-044](NM-HRM-044.md).
