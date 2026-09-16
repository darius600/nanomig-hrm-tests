# Specification — `ch8-keycode-table`

Package: `hrm/ch8-keycode-table`
Fixture / boundary: `keymap`

    module keymap (input [6:0] code, output [6:0] amiga);   amiga_keymap.v:10

Two ports, purely combinational. Claims file:
`hrm-testpipeline/work/ch8-s3-build/ch8-keycode-table.jsonl`, 446 records.
Manual pages 251, 252, 254.

This file is written from the `extract.quote` / `extract.restatement` fields
of the 446 claims alone (plus the two port names and the boundary module
name, which the task sheet states directly — no RTL file has been opened to
find them). `amiga_keymap.v` and `hid.v` have not been opened. Concrete HID
stimulus values, the Makefile, and the `expect()`/`note()`/`record()` calls
themselves are added only after this file is committed, per
`2026-09-16_ch8_s3_build_brief.md`'s order of work.

## Scope: this finding-family is NanoMig-specific

**Every red this package can produce is a statement about NanoMig, not about
the Minimig family.** In the upstream this project derives from,
`MiSTer-devel/Minimig-AGA_MiSTer` at `da7632e`, there is no
`amiga_keymap.v`, no module `keymap` and no `hid.v`: `rtl/minimig.v:207`
takes `kbd_mouse_data` as a top-level input and the HID-to-Amiga translation
happens above the chipset, in software, on MiSTer's ARM side. `src/misc`
(where `keymap` and `hid` live) is not shared lineage the way `src/minimig-
aga` is; other Minimig boards solve keyboard translation differently again.
This sentence is required in this file by the build brief and is repeated
in every finding this package produces.

## Why one boundary answers both pages 252 and 254

`code` is a single 7-bit stimulus and `amiga` a single 7-bit response;
nothing else crosses the boundary. A 128-value sweep of `code` is therefore
an exhaustive enumeration of the entire table, and one such sweep answers
every claim of the form "key X carries code Y" regardless of which printed
locus asserted it: page 252's two text tables, or the keys drawn in page
254's Figure 8-9 (Amiga 1000) and Figure 8-10 (Amiga 500/2000/3000).
Because there is only one table at this boundary, a code that page 252
assigns to a key and a code that a figure prints on that same key are, at
the RTL boundary, the same testable stimulus/expected-value pair — so where
both loci name the same key with the same code, this specification treats
that as one proposition with multiple citations (the same pattern
`ch7-overlay-source`'s `SPECIFICATION.md` used for reader twins), not as two
separate assertions.

**The two figures are two different keyboards and are never merged.**
Figure 8-9 draws the Amiga 1000; Figure 8-10 draws the Amiga 500/2000/3000.
They do not carry the same keys (the numeric-pad parenthesis/slash/asterisk/
plus keys and the international Return cutout exist only in Figure 8-10; the
A1000 numeric-pad Enter key is drawn differently but at the same code). A
figure-sourced claim names one keyboard, and this specification keeps the
two keyboards as separate proposition sets throughout — see the two tables
below.

## Claim inventory — all 446 accounted for

| source | page | raw claims | reader split | distinct propositions |
|---|---|---:|---|---:|
| general prose | 251 | 4 | opus 2, sol 2 | 2 |
| "$00-$3F is main-body" range statement | 252 | 2 | opus 1, sol 1 | 1 |
| international cutout keys ($30, $2B) | 252 | 4 | opus 2, sol 2 | 2 |
| "qualifier keys" section heading | 252 | 1 | opus 1 | 1 (declined, see below) |
| qualifier-table individual rows | 252 | 62 | opus 26, sol 36 | 36 |
| "$50-59 is the ten function keys" range statement | 252 | 1 | opus 1 | 1 |
| Figure 8-9 (Amiga 1000) key/code pairs | 254 | 180 | opus 90, sol 90 | 90 |
| Figure 8-10 (Amiga 500/2000/3000) key/code pairs | 254 | 192 | opus 96, sol 96 | 96 |
| **total** | | **446** | opus 219, sol 227 | **229** |

The opus/sol reader counts (219/227) match the task sheet's package-list
entry for this package exactly. Every one of the 446 `classify.uid` values
in the claims file appears in exactly one row of the tables in this
document; none is left unassigned.

## PROP-GEN — general behaviour (page 251)

> The keyboard data is not received in the form of ASCII characters.
— `opus:8-keycode.c1`, `sol:8-keycode.c1`

> Instead, for maximum versatility, it is received in the form of keycodes.
— `opus:8-keycode.c2`, `sol:8-keycode.c2`

**Proposition to test:** for at least a sample of keys with a printable
ASCII interpretation (the letter and digit rows), the code `amiga` carries
is not that character's ASCII value. This is a general statement, not tied
to any one key; which sample keys make the comparison is a step-3 decision
(the letters/digits are already fully enumerated in the tables below, so no
new stimulus needs to be invented for it). Status: testable, as a derived
check over rows already in this document.

## PROP-RANGE-MAIN — the $00-$3F range (page 252)

> These are key codes assigned to specific positions on the main body of
> the keyboard.
— `opus:8-keycode.c9`, `sol:8-keycode.c9`

**Proposition to test:** this is a block-level statement about the $00-$3F
range as a whole, not a per-key claim (no individual main-body key is named
here — that is what the figures on page 254 do, per-key, and is covered by
the two keyboard tables below). Status: testable only as an aggregate over
the main-body rows already carried by PROP-A1000 / PROP-A500 (below); not
given its own `expect()`.

## PROP-CUTOUT — the two international cutout keys (page 252) — candidate S3-C1

> These are $30, cut out from the the left shift, and $2B, cut out from the
> return key.
— `opus:8-keycode.c14`+`opus:8-keycode.c15` (split across two sentences by
opus), `sol:8-keycode.c13`+`sol:8-keycode.c14` (same two-sentence split,
sol's own numbering). Page reads "from the the left shift" verbatim (typo
in the original).

Two keys, two distinct codes, named without reference to either keyboard.
Figure 8-9 (A1000) draws only the Shift-cutout key (code $30); Figure 8-10
(A500/2000/3000) draws both the Shift-cutout ($30) and the Return-cutout
($2B) — see the two keyboard tables for the figure citations.

**PROP-CUTOUT-30:** the key cut out beside left Shift carries code $30.
Citations: `opus:8-keycode.c14`, `sol:8-keycode.c13`, plus the two figure
citations at code $30 in both keyboard tables below.

**PROP-CUTOUT-2B:** the key cut out of Return carries code $2B. Citations:
`opus:8-keycode.c15`, `sol:8-keycode.c14`, plus the Figure 8-10 citation at
code $2B (Figure 8-9 does not draw this key at all — see the A1000 table).

**This is the pre-measured candidate S3-C1** from
`2026-09-16_ch8_s3_finding_candidates.md`: the classify stage already
established that `amiga_keymap.v`'s only two EUR/cutout table entries both
emit `$2B`, and that `$30` is emitted by no entry in the table at all. If
that measurement holds at build time, PROP-CUTOUT-2B is expected to PASS
(control, same measurement basis as the suspect case) and PROP-CUTOUT-30 is
expected to FAIL (the finding) — both on a stimulus this package still has
to identify (the HID source code for the physical cutout key), which is a
step-3, RTL-reading task, not decided here.

## PROP-HEADING — declined (page 252)

> Key codes for qualifier keys
— `opus:8-keycode.c17`

A section heading, not a numeric proposition; no key or code is named. This
claim is **declined** with a `record()` line, not asserted — the heading
introduces the table in PROP-QUAL below, but states nothing itself that is
observable at `code`/`amiga`. No sol twin exists for this claim (sol's own
reading did not extract the heading as a separate proposition).

## PROP-QUAL — the qualifier-key table, $40-$67 (page 252)

Individual table rows, one code per row, keyboard-agnostic (the table names
no keyboard model). 26 of the 36 codes are read as individual rows by both
readers (opus and sol agree on the printed key name in every one of the 26);
the ten codes $50-$59 (the function-key row) are read by opus as a single
range claim (PROP-FUNCTION-RANGE, below) and individually by sol.

| code | key (p.252 qualifier table) | opus uid | sol uid |
|---|---|---|---|
| $40 | Space | opus:8-keycode.c18 | sol:8-keycode.c34 |
| $41 | Backspace | opus:8-keycode.c19 | sol:8-keycode.c35 |
| $42 | Tab | opus:8-keycode.c20 | sol:8-keycode.c36 |
| $43 | Numeric Pad "ENTER" | opus:8-keycode.c21 | sol:8-keycode.c37 |
| $44 | Return | opus:8-keycode.c22 | sol:8-keycode.c38 |
| $45 | Escape | opus:8-keycode.c23 | sol:8-keycode.c39 |
| $46 | Delete | opus:8-keycode.c24 | sol:8-keycode.c40 |
| $4A | Numeric pad minus | opus:8-keycode.c25 | sol:8-keycode.c41 |
| $4C | Cursor up | opus:8-keycode.c26 | sol:8-keycode.c42 |
| $4D | Cursor down | opus:8-keycode.c27 | sol:8-keycode.c43 |
| $4E | Cursor right | opus:8-keycode.c28 | sol:8-keycode.c44 |
| $4F | Cursor left | opus:8-keycode.c29 | sol:8-keycode.c45 |
| $50 | F1 | (range only, see PROP-FUNCTION-RANGE) | sol:8-keycode.c46 |
| $51 | F2 | (range only) | sol:8-keycode.c47 |
| $52 | F3 | (range only) | sol:8-keycode.c48 |
| $53 | F4 | (range only) | sol:8-keycode.c49 |
| $54 | F5 | (range only) | sol:8-keycode.c50 |
| $55 | F6 | (range only) | sol:8-keycode.c51 |
| $56 | F7 | (range only) | sol:8-keycode.c52 |
| $57 | F8 | (range only) | sol:8-keycode.c53 |
| $58 | F9 | (range only) | sol:8-keycode.c54 |
| $59 | F10 | (range only) | sol:8-keycode.c55 |
| $5A | Numeric pad left parenthesis | opus:8-keycode.c31 | sol:8-keycode.c56 |
| $5B | Numeric pad right parenthesis | opus:8-keycode.c32 | sol:8-keycode.c57 |
| $5C | Numeric pad slash "/" | opus:8-keycode.c33 | sol:8-keycode.c58 |
| $5D | Numeric pad asterisk | opus:8-keycode.c34 | sol:8-keycode.c59 |
| $5E | Numeric pad plus | opus:8-keycode.c35 | sol:8-keycode.c60 |
| $5F | Help | opus:8-keycode.c36 | sol:8-keycode.c61 |
| $60 | Left Shift | opus:8-keycode.c37 | sol:8-keycode.c62 |
| $61 | Right Shift | opus:8-keycode.c38 | sol:8-keycode.c63 |
| $62 | Caps Lock | opus:8-keycode.c39 | sol:8-keycode.c64 |
| $63 | Control | opus:8-keycode.c40 | sol:8-keycode.c65 |
| $64 | Left Alt | opus:8-keycode.c41 | sol:8-keycode.c66 |
| $65 | Right Alt | opus:8-keycode.c42 | sol:8-keycode.c67 |
| $66 | Left Amiga (or Commodore key) | opus:8-keycode.c43 | sol:8-keycode.c68 |
| $67 | Right Amiga | opus:8-keycode.c44 | sol:8-keycode.c69 |

**Cross-reference to the figures:** every code in this table except
$5A-$5E also appears, for the same key, in one or both keyboard tables
below (PROP-A1000 / PROP-A500) — $5A-$5E (the numeric-pad parenthesis/
slash/asterisk/plus keys) exist only on the Amiga 500/2000/3000 keypad, so
those five rows are corroborated only by the A500/2000/3000 table, not by
A1000. Where a code from this table matches a code in a keyboard table for
the same key, they are the same proposition (one `expect()`, cited from
both loci) — **except $65 Right Alt, which matches no entry in either
keyboard table.** That is candidate S3-C2, below.

## PROP-FUNCTION-RANGE — $50-$59 as a block (page 252)

>    50-59   Function keys F1-F10
— `opus:8-keycode.c30`. The page states the range and the ten-key group
name, not (on this row) which of the ten codes belongs to which numbered
key.

**Proposition to test:** the block $50-$59 is exactly the ten function
keys, in order F1 (lowest code) through F10 (highest). This is a distinct,
weaker proposition from the ten individual PROP-QUAL rows above (which
sol's independent reading supports one code at a time): a red on this one
and green on all ten individual rows would mean the ten codes are right but
not contiguous/ordered as the manual's row implies, which is worth
distinguishing. Citation for the block claim: `opus:8-keycode.c30`. The ten
individual assignments are `sol:8-keycode.c46` through `c55` (already in
the PROP-QUAL table above) and are corroborated again by both keyboards'
$50-$59 rows below.

## PROP-A1000 — Figure 8-9, the Amiga 1000 keyboard (page 254)

90 keys. Every row cites one `opus:8-fig.cN` claim (row/legend-style
reading) and one `sol:8-layout.cM` claim (plain-key-name-style reading);
both are independent full readings of the same figure and agree on every
code in this table. `$64 ALT` appears twice — see the S3-C2 note below the
table.

| code | key (p.254 legend) | citations |
|---|---|---|
| $00 | grave/tilde | opus:8-fig.c15, sol:8-layout.c13 |
| $01 | 1/exclamation | opus:8-fig.c16, sol:8-layout.c14 |
| $02 | 2/at | opus:8-fig.c17, sol:8-layout.c15 |
| $03 | 3/hash | opus:8-fig.c18, sol:8-layout.c16 |
| $04 | 4/dollar | opus:8-fig.c19, sol:8-layout.c17 |
| $05 | 5/percent | opus:8-fig.c20, sol:8-layout.c18 |
| $06 | 6/caret | opus:8-fig.c21, sol:8-layout.c19 |
| $07 | 7/ampersand | opus:8-fig.c22, sol:8-layout.c20 |
| $08 | 8/asterisk | opus:8-fig.c23, sol:8-layout.c21 |
| $09 | 9/left-parenthesis | opus:8-fig.c24, sol:8-layout.c22 |
| $0A | 0/right-parenthesis | opus:8-fig.c25, sol:8-layout.c23 |
| $0B | minus/underscore | opus:8-fig.c26, sol:8-layout.c24 |
| $0C | equals/plus | opus:8-fig.c27, sol:8-layout.c25 |
| $0D | backslash/vertical-bar | opus:8-fig.c28, sol:8-layout.c26 |
| $0F | numeric-pad 0 | opus:8-fig.c89, sol:8-layout.c87 |
| $10 | Q | opus:8-fig.c31, sol:8-layout.c29 |
| $11 | W | opus:8-fig.c32, sol:8-layout.c30 |
| $12 | E | opus:8-fig.c33, sol:8-layout.c31 |
| $13 | R | opus:8-fig.c34, sol:8-layout.c32 |
| $14 | T | opus:8-fig.c35, sol:8-layout.c33 |
| $15 | Y | opus:8-fig.c36, sol:8-layout.c34 |
| $16 | U | opus:8-fig.c37, sol:8-layout.c35 |
| $17 | I | opus:8-fig.c38, sol:8-layout.c36 |
| $18 | O | opus:8-fig.c39, sol:8-layout.c37 |
| $19 | P | opus:8-fig.c40, sol:8-layout.c38 |
| $1A | left-bracket/left-brace | opus:8-fig.c41, sol:8-layout.c39 |
| $1B | right-bracket/right-brace | opus:8-fig.c42, sol:8-layout.c40 |
| $1D | numeric-pad 1 | opus:8-fig.c86, sol:8-layout.c84 |
| $1E | numeric-pad 2 | opus:8-fig.c87, sol:8-layout.c85 |
| $1F | numeric-pad 3 | opus:8-fig.c88, sol:8-layout.c86 |
| $20 | A | opus:8-fig.c46, sol:8-layout.c44 |
| $21 | S | opus:8-fig.c47, sol:8-layout.c45 |
| $22 | D | opus:8-fig.c48, sol:8-layout.c46 |
| $23 | F | opus:8-fig.c49, sol:8-layout.c47 |
| $24 | G | opus:8-fig.c50, sol:8-layout.c48 |
| $25 | H | opus:8-fig.c51, sol:8-layout.c49 |
| $26 | J | opus:8-fig.c52, sol:8-layout.c50 |
| $27 | K | opus:8-fig.c53, sol:8-layout.c51 |
| $28 | L | opus:8-fig.c54, sol:8-layout.c52 |
| $29 | semicolon/colon | opus:8-fig.c55, sol:8-layout.c53 |
| $2A | apostrophe/quote | opus:8-fig.c56, sol:8-layout.c54 |
| $2D | numeric-pad 4 | opus:8-fig.c83, sol:8-layout.c81 |
| $2E | numeric-pad 5 | opus:8-fig.c84, sol:8-layout.c82 |
| $2F | numeric-pad 6 | opus:8-fig.c85, sol:8-layout.c83 |
| $30 | international cutout beside left Shift | opus:8-fig.c59, sol:8-layout.c58 |
| $31 | Z | opus:8-fig.c60, sol:8-layout.c59 |
| $32 | X | opus:8-fig.c61, sol:8-layout.c60 |
| $33 | C | opus:8-fig.c62, sol:8-layout.c61 |
| $34 | V | opus:8-fig.c63, sol:8-layout.c62 |
| $35 | B | opus:8-fig.c64, sol:8-layout.c63 |
| $36 | N | opus:8-fig.c65, sol:8-layout.c64 |
| $37 | M | opus:8-fig.c66, sol:8-layout.c65 |
| $38 | comma/less-than | opus:8-fig.c67, sol:8-layout.c66 |
| $39 | period/greater-than | opus:8-fig.c68, sol:8-layout.c67 |
| $3A | slash/question | opus:8-fig.c69, sol:8-layout.c68 |
| $3C | numeric-pad period | opus:8-fig.c90, sol:8-layout.c88 |
| $3D | numeric-pad 7 | opus:8-fig.c80, sol:8-layout.c78 |
| $3E | numeric-pad 8 | opus:8-fig.c81, sol:8-layout.c79 |
| $3F | numeric-pad 9 | opus:8-fig.c82, sol:8-layout.c80 |
| $40 | Space | opus:8-fig.c73, sol:8-layout.c75 |
| $41 | Backspace | opus:8-fig.c29, sol:8-layout.c27 |
| $42 | Tab | opus:8-fig.c30, sol:8-layout.c28 |
| $43 | numeric-pad Enter | opus:8-fig.c92, sol:8-layout.c90 |
| $44 | Return | opus:8-fig.c57, sol:8-layout.c55 |
| $45 | Escape | opus:8-fig.c3, sol:8-layout.c1 |
| $46 | Delete | opus:8-fig.c14, sol:8-layout.c12 |
| $4A | numeric-pad minus | opus:8-fig.c91, sol:8-layout.c89 |
| $4C | cursor up | opus:8-fig.c76, sol:8-layout.c56 |
| $4D | cursor down | opus:8-fig.c79, sol:8-layout.c72 |
| $4E | cursor right | opus:8-fig.c78, sol:8-layout.c71 |
| $4F | cursor left | opus:8-fig.c77, sol:8-layout.c70 |
| $50 | F1 | opus:8-fig.c4, sol:8-layout.c2 |
| $51 | F2 | opus:8-fig.c5, sol:8-layout.c3 |
| $52 | F3 | opus:8-fig.c6, sol:8-layout.c4 |
| $53 | F4 | opus:8-fig.c7, sol:8-layout.c5 |
| $54 | F5 | opus:8-fig.c8, sol:8-layout.c6 |
| $55 | F6 | opus:8-fig.c9, sol:8-layout.c7 |
| $56 | F7 | opus:8-fig.c10, sol:8-layout.c8 |
| $57 | F8 | opus:8-fig.c11, sol:8-layout.c9 |
| $58 | F9 | opus:8-fig.c12, sol:8-layout.c10 |
| $59 | F10 | opus:8-fig.c13, sol:8-layout.c11 |
| $5F | Help | opus:8-fig.c43, sol:8-layout.c41 |
| $60 | left Shift | opus:8-fig.c58, sol:8-layout.c57 |
| $61 | right Shift | opus:8-fig.c70, sol:8-layout.c69 |
| $62 | Caps Lock | opus:8-fig.c45, sol:8-layout.c43 |
| $63 | Control | opus:8-fig.c44, sol:8-layout.c42 |
| $64 | left Alt | opus:8-fig.c71, sol:8-layout.c73 |
| $64 | right Alt | opus:8-fig.c75, sol:8-layout.c77 |
| $66 | left Amiga | opus:8-fig.c72, sol:8-layout.c74 |
| $67 | right Amiga | opus:8-fig.c74, sol:8-layout.c76 |

Note: this figure draws no key at code $2B (no international Return
cutout key appears in Figure 8-9 at all — confirmed by checking both
independent readings, neither has an entry between $2A and $2D for this
keyboard). PROP-CUTOUT-2B is therefore corroborated by page 252 and by
Figure 8-10 only, not by this table.

## PROP-A500 — Figure 8-10, the Amiga 500/2000/3000 keyboard (page 254)

96 keys, same twin-citation convention as PROP-A1000.

| code | key (p.254 legend) | citations |
|---|---|---|
| $00 | grave/tilde | opus:8-fig.c104, sol:8-layout.c102 |
| $01 | 1/exclamation | opus:8-fig.c105, sol:8-layout.c103 |
| $02 | 2/at | opus:8-fig.c106, sol:8-layout.c104 |
| $03 | 3/hash | opus:8-fig.c107, sol:8-layout.c105 |
| $04 | 4/dollar | opus:8-fig.c108, sol:8-layout.c106 |
| $05 | 5/percent | opus:8-fig.c109, sol:8-layout.c107 |
| $06 | 6/caret | opus:8-fig.c110, sol:8-layout.c108 |
| $07 | 7/ampersand | opus:8-fig.c111, sol:8-layout.c109 |
| $08 | 8/asterisk | opus:8-fig.c112, sol:8-layout.c110 |
| $09 | 9/left-parenthesis | opus:8-fig.c113, sol:8-layout.c111 |
| $0A | 0/right-parenthesis | opus:8-fig.c114, sol:8-layout.c112 |
| $0B | minus/underscore | opus:8-fig.c115, sol:8-layout.c113 |
| $0C | equals/plus | opus:8-fig.c116, sol:8-layout.c114 |
| $0D | backslash/vertical-bar | opus:8-fig.c117, sol:8-layout.c115 |
| $0F | numeric-pad 0 | opus:8-fig.c187, sol:8-layout.c185 |
| $10 | Q | opus:8-fig.c120, sol:8-layout.c124 |
| $11 | W | opus:8-fig.c121, sol:8-layout.c125 |
| $12 | E | opus:8-fig.c122, sol:8-layout.c126 |
| $13 | R | opus:8-fig.c123, sol:8-layout.c127 |
| $14 | T | opus:8-fig.c124, sol:8-layout.c128 |
| $15 | Y | opus:8-fig.c125, sol:8-layout.c129 |
| $16 | U | opus:8-fig.c126, sol:8-layout.c130 |
| $17 | I | opus:8-fig.c127, sol:8-layout.c131 |
| $18 | O | opus:8-fig.c128, sol:8-layout.c132 |
| $19 | P | opus:8-fig.c129, sol:8-layout.c133 |
| $1A | left-bracket/left-brace | opus:8-fig.c130, sol:8-layout.c134 |
| $1B | right-bracket/right-brace | opus:8-fig.c131, sol:8-layout.c135 |
| $1D | numeric-pad 1 | opus:8-fig.c183, sol:8-layout.c181 |
| $1E | numeric-pad 2 | opus:8-fig.c184, sol:8-layout.c182 |
| $1F | numeric-pad 3 | opus:8-fig.c185, sol:8-layout.c183 |
| $20 | A | opus:8-fig.c135, sol:8-layout.c139 |
| $21 | S | opus:8-fig.c136, sol:8-layout.c140 |
| $22 | D | opus:8-fig.c137, sol:8-layout.c141 |
| $23 | F | opus:8-fig.c138, sol:8-layout.c142 |
| $24 | G | opus:8-fig.c139, sol:8-layout.c143 |
| $25 | H | opus:8-fig.c140, sol:8-layout.c144 |
| $26 | J | opus:8-fig.c141, sol:8-layout.c145 |
| $27 | K | opus:8-fig.c142, sol:8-layout.c146 |
| $28 | L | opus:8-fig.c143, sol:8-layout.c147 |
| $29 | semicolon/colon | opus:8-fig.c144, sol:8-layout.c148 |
| $2A | apostrophe/quote | opus:8-fig.c145, sol:8-layout.c149 |
| $2B | international Return cutout | opus:8-fig.c146, sol:8-layout.c150 |
| $2D | numeric-pad 4 | opus:8-fig.c179, sol:8-layout.c177 |
| $2E | numeric-pad 5 | opus:8-fig.c180, sol:8-layout.c178 |
| $2F | numeric-pad 6 | opus:8-fig.c181, sol:8-layout.c179 |
| $30 | international cutout beside left Shift | opus:8-fig.c148, sol:8-layout.c152 |
| $31 | Z | opus:8-fig.c149, sol:8-layout.c153 |
| $32 | X | opus:8-fig.c150, sol:8-layout.c154 |
| $33 | C | opus:8-fig.c151, sol:8-layout.c155 |
| $34 | V | opus:8-fig.c152, sol:8-layout.c156 |
| $35 | B | opus:8-fig.c153, sol:8-layout.c157 |
| $36 | N | opus:8-fig.c154, sol:8-layout.c158 |
| $37 | M | opus:8-fig.c155, sol:8-layout.c159 |
| $38 | comma/less-than | opus:8-fig.c156, sol:8-layout.c160 |
| $39 | period/greater-than | opus:8-fig.c157, sol:8-layout.c161 |
| $3A | slash/question | opus:8-fig.c158, sol:8-layout.c162 |
| $3C | numeric-pad period | opus:8-fig.c188, sol:8-layout.c186 |
| $3D | numeric-pad 7 | opus:8-fig.c175, sol:8-layout.c173 |
| $3E | numeric-pad 8 | opus:8-fig.c176, sol:8-layout.c174 |
| $3F | numeric-pad 9 | opus:8-fig.c177, sol:8-layout.c175 |
| $40 | Space | opus:8-fig.c162, sol:8-layout.c166 |
| $41 | Backspace | opus:8-fig.c118, sol:8-layout.c116 |
| $42 | Tab | opus:8-fig.c119, sol:8-layout.c123 |
| $43 | numeric-pad Enter | opus:8-fig.c186, sol:8-layout.c184 |
| $44 | Return | opus:8-fig.c132, sol:8-layout.c136 |
| $45 | Escape | opus:8-fig.c93, sol:8-layout.c91 |
| $46 | Delete | opus:8-fig.c165, sol:8-layout.c117 |
| $4A | numeric-pad minus | opus:8-fig.c178, sol:8-layout.c176 |
| $4C | cursor up | opus:8-fig.c167, sol:8-layout.c169 |
| $4D | cursor down | opus:8-fig.c169, sol:8-layout.c171 |
| $4E | cursor right | opus:8-fig.c170, sol:8-layout.c172 |
| $4F | cursor left | opus:8-fig.c168, sol:8-layout.c170 |
| $50 | F1 | opus:8-fig.c94, sol:8-layout.c92 |
| $51 | F2 | opus:8-fig.c95, sol:8-layout.c93 |
| $52 | F3 | opus:8-fig.c96, sol:8-layout.c94 |
| $53 | F4 | opus:8-fig.c97, sol:8-layout.c95 |
| $54 | F5 | opus:8-fig.c98, sol:8-layout.c96 |
| $55 | F6 | opus:8-fig.c99, sol:8-layout.c97 |
| $56 | F7 | opus:8-fig.c100, sol:8-layout.c98 |
| $57 | F8 | opus:8-fig.c101, sol:8-layout.c99 |
| $58 | F9 | opus:8-fig.c102, sol:8-layout.c100 |
| $59 | F10 | opus:8-fig.c103, sol:8-layout.c101 |
| $5A | numeric-pad left parenthesis | opus:8-fig.c171, sol:8-layout.c119 |
| $5B | numeric-pad right parenthesis | opus:8-fig.c172, sol:8-layout.c120 |
| $5C | numeric-pad slash | opus:8-fig.c173, sol:8-layout.c121 |
| $5D | numeric-pad asterisk | opus:8-fig.c174, sol:8-layout.c122 |
| $5E | numeric-pad plus | opus:8-fig.c182, sol:8-layout.c180 |
| $5F | Help | opus:8-fig.c166, sol:8-layout.c118 |
| $60 | left Shift | opus:8-fig.c147, sol:8-layout.c151 |
| $61 | right Shift | opus:8-fig.c159, sol:8-layout.c163 |
| $62 | Caps Lock | opus:8-fig.c134, sol:8-layout.c138 |
| $63 | Control | opus:8-fig.c133, sol:8-layout.c137 |
| $64 | left Alt | opus:8-fig.c160, sol:8-layout.c164 |
| $64 | right Alt | opus:8-fig.c164, sol:8-layout.c168 |
| $66 | left Amiga | opus:8-fig.c161, sol:8-layout.c165 |
| $67 | right Amiga | opus:8-fig.c163, sol:8-layout.c167 |

## Candidate S3-C2 — right Alt: page 252 disagrees with page 254 — DECLINE, grade neither side

p.252's qualifier table (PROP-QUAL above) reads **$64 Left Alt / $65 Right
Alt**. Both figures print **$64 on both ALT keys** in both keyboards (see
the `$64 right Alt` rows in PROP-A1000 and PROP-A500 above) — code $65
appears on no key in either figure. This is a genuine disagreement between
two printed loci about the same physical key, not an RTL divergence, and
per the build brief and the finding-candidates file it must not be graded
either way: no `expect()` compares an RTL value against $65, and no
`expect()` compares the same RTL value against $64 for right Alt. Both
readings are recorded with `record()`, citing the conflict, and the RTL
value read at that stimulus is reported with `note()` only.

**Claim records carrying this conflict, checked against the classify data
in the claims file itself (not against the RTL):**

- `opus:8-fig.c75` (Figure 8-9, right Alt, prints $64)
- `sol:8-layout.c77` (Figure 8-9, right Alt, prints $64)
- `opus:8-fig.c164` (Figure 8-10, right Alt, prints $64)
- `sol:8-layout.c168` (Figure 8-10, right Alt, prints $64)

That is **four** claim records, not five: `2026-09-16_ch8_s3_finding_
candidates.md` states "Five claim records carry `source_defines: conflict:
page 252 versus page 254`," but `opus:8-keycode.c42` and `sol:8-keycode.c67`
(the two page-252 "$65 Right Alt" claims) are each marked plainly `source_
defines: yes, page 252` in the claims file, not `conflict` — the conflict
flag was applied only to the four page-254 figure claims above. This
discrepancy is reported at the end per the build brief's request; it does
not change the handling (all six of these claims — the four figure claims
and the two page-252 claims — are declined, not graded, for right Alt).

## What this package does not raise (checked, not re-derived)

Three things the finding-candidates file names as looking like divergences
and are not, none of which corresponds to any claim in this package's 446
(every claim here is of the form "this key's own printed code produces this
output"; none is of the form "this output is reachable from more than one
input," which is the shape all three of these have):

- **PageDown also produces $67** (alongside right Amiga) — a fact about the
  RTL's *input* side (an extra HID source reaching the same output), not
  about any key this package's claims name.
- **$5F is reachable from both Insert and End** — same shape.
- **There is no right Control key** — the manual (p.252, singular
  "Control"; the independently-readable Matrix Table entry) names one
  Control key, and this package's claims agree: no claim in the 446 asserts
  a second, right-hand Control code.

None of these three appears as a citation anywhere in this document, and
none will be asserted against in the eventual test.

## Order of work

This file was written from `extract.quote` / `extract.restatement` and the
two port names given directly by the task sheet. `amiga_keymap.v` and
`hid.v` have not been opened. It is committed before either file is read,
per `2026-09-16_ch8_s3_build_brief.md`. HID stimulus values, the reverse-
map mechanics, the Makefile pattern (following `ch7-overlay-source`, the
nearest precedent for a non-`minimig-aga` top), and every `expect()`/
`note()`/`record()` call are step-3 work that starts only after this
commit.
