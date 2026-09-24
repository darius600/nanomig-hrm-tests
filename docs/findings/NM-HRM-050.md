# NM-HRM-050 — one of the two international cutout keys reaches no code

| | |
|---|---|
| **Lifecycle** | open |
| **Filed against** | the RTL |
| **Evidence** | a graded red |
| **What is asked of a reader** | Decide whether the RTL should change. *Suggested change* states one option; the decision belongs to an Amiga specialist, not to this repository. |
| **Module** | `amiga_keymap.v` — [NanoMig](https://github.com/harbaum/NanoMig/blob/d6f7df957fad655bc07f6a381e5094a9edff0be0/src/misc/amiga_keymap.v#L70-L130) · no counterpart upstream |
| **Report to** | **NanoMig.** There is no ancestor line to file this against, and the finding does not generalise to the Minimig family |
| **Test** | [`hrm/ch8-keycode-table`](../../hrm/ch8-keycode-table) — `CUTOUT-30` red at HID `$64`, `KEY-$2B` green at HID `$32`, same measurement basis |
| **Chapter** | 8 — Interface Hardware · *Amiga Hardware Reference Manual*, 3rd ed. |
| **Assertions** | 1 red of 98. The other 97 include a 94-row exhaustive sweep of the table, all green |
| **Verdict of the four** | **unanimous against.** All four carry both cutout keys as distinct codes, `$30` and `$2B`, in their own key tables |

---

## What the HRM says

**p.252**: *"The international keyboards have two more keys that are "cut out" of
larger keys on the USA version. These are $30, cut out from the the left shift,
and $2B, cut out from the return key."*

Two keys, two codes, and the sentence names which larger key each was taken
from. On a German or UK machine these are the two that a USA keyboard does not
have.

## What NanoMig does

`keymap` is a single combinational table, `code` in and `amiga` out, and a
128-value sweep of `code` is an exhaustive statement about all of it. **No input
produces `$30`.** Both international keys produce `$2B`:

```verilog
  (code == 7'h32)?7'h2b:   // 32: EUR-1      amiga_keymap.v:72
  (code == 7'h64)?7'h2b:   // 64: EUR-2      amiga_keymap.v:130
```

The file's only other `7'h30` is on the **input** side, where it is the HID
source code for `]` and maps to `$1b`:

```verilog
  (code == 7'h30)?7'h1b:   // 30: ]          amiga_keymap.v:70
```

So the key cut out of the left shift is not merely mapped to the wrong value —
it is unreachable. Nothing a user can press makes the Amiga side see `$30`, and
the two keys the page distinguishes are collapsed onto one code.

## Which entry is wrong, and what it should say

The two entries are not equally wrong. `keymap`'s input side is the FPGA
Companion code space, which its own header describes as *"mainly the USB HID key
codes with the modifier keys mapped into the 0x68+ range"* — and FPGA
Companion's `core_map_key` confirms it, returning the host's usage code
unchanged for a non-legacy core (`src/core.c:60-82`). So the input side has an
oracle outside this tree, and the USB HID usage names settle it:

| HID usage | USB HID name | where the key sits | Amiga code | NanoMig gives |
|---|---|---|---|---|
| `$32` | Keyboard Non-US `#` and `~` | beside Return | `$2B` | `$2B` ✔ |
| `$64` | Keyboard Non-US `\` and `\|` | beside left Shift | `$30` | `$2B` ✘ |

(The usage names are read from Denise's own HID-indexed table,
`driver/input/iokit/keyNames.cpp:149` and `:199`, and agree with
`amiga_keymap.v`'s own input-side comments for every neighbouring code.)

So `amiga_keymap.v:130` is the single wrong line, and `$30` is the value it
should carry. The entry at `:72` is right.

**The two figures show the same split.** Figure 8-9 (A1000) shades one key,
`30`, beside the left shift. Figure 8-10 (A500/2000/3000) shades two, `30`
beside the left shift and `2B` beside Return. The shading is the page drawing
p.252's sentence, and it places the two keys at opposite ends of the board.

## Why this is not a near-miss

`$2B` is correct for the Return cutout, and the test grades that separately as
`KEY-$2B`, green, on exactly the same stimulus path — so a units error or a
broken fixture is ruled out by the package's own control. The defect is that
the *other* key is given the same answer.

The two stimuli were bound the wrong way round in the first build, by
`amiga_keymap.v`'s own naming order rather than against any oracle for what
those inputs physically are. Corrected and rebuilt: the red stays red and the
green stays green, because both RTL entries emit `$2B` whichever is driven —
but the red is now against the key p.252 names, and the green is a control on
the key that is right, instead of a pass standing on the defect.

## What the others say

| | |
|---|---|
| **WinUAE** | Both, named and distinct: `AK_LTGT 0x30` and `AK_NUMBERSIGN 0x2B` (`include/keyboard.h:119-120`) |
| **vAmiga** | Both, named and distinct: `laceBrace = 0x30` and `hashtag = 0x2B` (`GUI/Input/AmigaKey.swift:97-98`) |
| **Denise** | Both, adjacent in its key table: `0x2b, 0x30` (`emulation/libami/input/keyboard/keyboard.cpp:19`) |
| **Copperline** | `0x30` sits in the A500 key matrix taken from the keyboard's own wiring (`src/chipset/keyboard.rs:133`) |
| **MiSTer** | descendant — **no counterpart.** The pinned Minimig-AGA tree has no `amiga_keymap.v`, no `keymap` and no `hid.v`: `rtl/minimig.v:207` takes the already-decoded `kbd_mouse_data` as a top-level input, and the translation happens in software above the chipset |
| **TC64** | descendant — **no counterpart either**: no `amiga_keymap.v` in its `rtl/minimig/`. Other Minimig boards solve it differently again |

None of the four independent implementations reaches `$30` by accident and none
of them omits it. The page's two cutout keys are two keys everywhere else.

---

## What other Commodore documents say

Second documents, not oracles: each can agree or disagree with the manual,
and none decides this finding. What they are and how they were read is in
[the findings README](README.md#second-documents).

- **Commodore-Amiga, *Amiga Hardware Manual*, 1985 — the oracle's first edition**, PDF page 260 — agrees with the manual:

  > "I (spare) I (LEFT I CAPS I TAB I ,ESC I (p0.6) I I SHIFT) I LOCK I , , I I (50) I (30) I (62)"
  <!-- corpus: hw-manual-1985/p260.txt -->

  This is the keyboard-matrix table in Appendix H (August 1985); the text layer is fragmented, so I read it against the page image. Column 14 (PD.6), row 4 is '(LEFT SHIFT)' with keycode (30), beside CAPS LOCK (62); the OCR's '(50)' is '(5D)' on the image. In the same table, column 2 (PC.2), row 3 is '(RET)' with keycode (2B). These are the two parenthesised matrix positions under the left Shift and the Return key: two distinct codes, $30 and $2B, as p.252 says. Figure 8-3 (p.220 image) draws key 30 between SHIFT 60 and Z 31, and a key 2B under the RETURN label.

Silent on this point: Commodore, *A500/A2000 Gary Specification*; Commodore-Amiga, *Amiga Hardware Manual* errata, October 1985; US patent 4,829,473, Commodore-Amiga.

Not read, having no text layer: the A500/A2000 technical reference, the 8371 Agnus specification and the 6526 CIA specification.
