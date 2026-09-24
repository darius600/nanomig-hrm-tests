# NM-HRM-052 — p.252 and p.254 disagree about the right Alt key

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a source observation, not test-backed |
| **What is asked of a reader** | Nothing yet. This was read from the source rather than measured by a graded assertion, and needs a test before it can ask for anything. |
| **Module** | `amiga_keymap.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/misc/amiga_keymap.v#L135-L139) |
| **Report to** | **nobody, yet.** Two printed pages contradict each other; which is right is an Amiga-history question, not a conformance question |
| **Test** | [`hrm/ch8-keycode-table`](../../hrm/ch8-keycode-table) — `RIGHTALT-CONFLICT`, six claims declined on a `record()` line, **graded on neither side** |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | none. Deliberately |
| **Verdict of the four** | **unanimous for p.252, and it settles nothing.** All four name `$65` for the right Alt — evidence about which reading practitioners took, not about which page is right |

---

## The two sources

**p.252**, the keycode table, lists the qualifier keys one per line:

       64      Left Alt
       65      Right Alt

**p.254** draws two keyboards, Figure 8-9 the A1000 and Figure 8-10 the
A500/2000/3000, with each key's code printed on its cap. **Both figures print
`64` on both ALT keys** — the one left of the space bar and the one right of it.

Read from the images, not from the text layer: the bottom row of Figure 8-9 runs
`ALT 64`, `A 66`, `40`, `A 67`, `ALT 64`, and Figure 8-10's runs the same.

## What NanoMig does

It follows the table:

```verilog
  (code == 7'h6a)?7'h64:   // left alt        amiga_keymap.v:135
  (code == 7'h6e)?7'h65:   // right alt       amiga_keymap.v:139
```

## Why this is not graded

A conformance test needs an oracle, and here the manual speaks with two voices
on the same fact. Grading the table would report a defect the figures deny;
grading the figures would report one the table denies. Either way the test would
be asserting a choice between two printed pages, which is not its job — the same
reasoning as [NM-HRM-044](NM-HRM-044.md).

The six claims that carry this — four of them flagged `source_defines:
conflict` by the classification, two recorded against p.252 without the flag —
are declined together on one `record()` line naming the conflict, and none of
them appears on a graded line.

## What the others say

| | |
|---|---|
| **WinUAE** | `AK_LALT 0x64`, `AK_RALT 0x65` (`include/keyboard.h:103-104`) |
| **vAmiga** | `leftAlt = 0x64`, `rightAlt = 0x65` (`GUI/Input/AmigaKey.swift:136-137`) |
| **Denise** | `0x64, 0x65` adjacent in the modifier group of its key table (`emulation/libami/input/keyboard/keyboard.cpp:18`) |
| **Copperline** | `AltLeft => 0x64`, `AltRight => 0x65` (`src/video/window/host_input.rs:185-186`) |
| **MiSTer** | descendant — **moot, and not applicable**: this finding is about the manual, not about an implementation. `amiga_keymap.v` is absent from the pinned `rtl/` in any case, so there is no keymap here to consult |
| **TC64** | descendant — **the same**: no `amiga_keymap.v` in its `rtl/minimig/` either |

Four independent implementations all took the table and none took the figures.
That is worth recording — but it is evidence about what implementers read, not
about what the hardware did, and it does not license grading a test against
p.252. A real A1000 or A500 is what would settle it.

## What the figures do settle

The same two drawings are the primary evidence for
[NM-HRM-050](NM-HRM-050.md): both shade the international cutout keys, and they
shade them in different places — Figure 8-9 shades `30` alone, beside the left
shift; Figure 8-10 shades `30` there and `2B` beside Return. On that point the
figures and p.252 agree with each other, and NanoMig disagrees with both.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore-Amiga, *Amiga Hardware Manual*, 1985 — the oracle's first edition**, PDF page 218 — agrees with the manual:

  > "64 Left Alt 65 Right Alt"
  <!-- corpus: hw-manual-1985/p218.txt -->

  This sides with the 3rd edition's p.252 table (Right Alt = $65), which NanoMig also follows, and against Figures 8-9/8-10 (64 on both ALT keys). Two more 1985 places agree. The Appendix H keyboard hardware table (p.260) gives RIGHT ALT its own 6500/1 port-B input with code (65), next to LEFT ALT (64). Figure 8-3 (p.220), the A1000 keyboard, prints 65 on the right ALT key and 64 on the left. I read that from the page image; the p.220 text layer does not carry the 65, so it is not quoted. The 1985 figure therefore does not have the 3rd edition figures' 64/64.

Silent on this point: Commodore, *A500/A2000 Gary Specification*; Commodore-Amiga, *Amiga Hardware Manual* errata, October 1985; US patent 4,829,473, Commodore-Amiga.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.
