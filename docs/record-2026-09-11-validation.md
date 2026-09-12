# Record: the validation round of 11.09.2026

A record of one day, kept out of [`findings.md`](findings.md) so that the
register stays a reference. Every finding it revised carries its own note in
that file; this is the round's own account of why.

Not an instruction, and not to be updated to match today.

---

*What follows is a record of one day, not an instruction. It is kept because
the reasoning that revised a finding is part of the result — and it is at the
end rather than the front because a reader looking up a finding should reach
the register first.*

## Record: the validation round of 11.09.2026

An independent validator (Codex Sol, which wrote none of these tests) audited the
built packages against the printed manual. **Seven of the thirteen findings did not
survive in the form they were written.** Every disposition was re-checked against
the manual page by the lead before being accepted; none was taken on the
validator's word, and each affected finding carries its own note below.

| | findings |
|---|---|
| **Hold as test-backed findings** | F01, F02, F03, F06, F10 |
| **Hold with a correction** | F11 (evidential count) |
| **Source observations, not test-backed** | F05, F09 |
| **Downgraded, then restored once the witness was fixed** | F12 |
| **Withdrawn** | F04, F07, F08, F13 |
| **Established while correcting one of them** | **F14** |

**What the four implementations do.** They decide nothing — the manual does.
A dash means nobody has read that source for this finding.

| WinUAE | vAmiga | Denise | Copperline |
|---|---|---|---|
| no border effect | no border effect | name not present anywhere in its Amiga core | **implements it**, with its own test |

One of four treats BRDNTRAN as real border behaviour.

**F02's evidence was sharpened on 11.09.2026.** Page 80 does not merely give the
number 49; it gives the reason — *"because the rightmost limit remains ($D8) and
only one word is fetched at this limit"*. The package measured only the total (50,
not 49). A second assertion now measures the mechanism the sentence names, and it
is red for a sharper reason: the hires word count is structurally even, so 49 is
not individually reachable at all — 48 and 50 are, 49 is not. The finding is the
same; the evidence now matches the clause the manual actually argues from.

Every withdrawal was also carried back into its package the same day, so no test
is left asserting something the manual does not say. Three of those corrections
answered the question the withdrawal raised: with an expectation the page
actually supports, **F04 and F07 turn green** — the implementation was right and
the oracle was not — and **F05's claim became a recorded decline**, because no
faithful assertion could be written without importing an RTL constant.

**F08 is the instructive one.** It asserted that SPRxCTL bits 3 *and* 4 are live.
Bit 3 the manual itself calls `(unimplemented)`; bit 4 it specifies. Split apart,
bit 3 is declined and bit 4 stays red on a claim that holds — recorded as F14.
The original finding pointed at something real and described it incorrectly, and
only reading the printed page again separated the two. That is the whole case for
this step.

Four of the withdrawals share one cause: **the assertion, not the RTL, was wrong.**
Each read a permission or a silence in the manual as a requirement — "not used" as
"reads zero", a bare mnemonic as a mandated effect, "ECS-only" as "must be live",
in one case even where the manual prints the word *unimplemented*. A red result is
a finding only where the manual actually says otherwise.

The two source-only downgrades share a different cause, and it is the one this
pipeline exists to prevent: **an expected value that came from the implementation.**
F05 computes `227.5 CCK × 2 hpos-ticks/CCK`, taking the factor 2 from the RTL's
internal representation. F12 is its mirror image — an assertion that could not have
passed against a perfect implementation, because it tested a bit *rearrangement*
using a value made of identical bits. **F12 is the one that came back:** given a
witness whose nibbles differ top-half from bottom-half, it is still red, and the
finding now rests on a measurement that could have gone the other way.
