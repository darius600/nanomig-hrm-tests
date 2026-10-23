// s3_withdrawn.h -- the runtime record of every claim id declined in
// SPECIFICATION.md section 1: one RECORDED line per id, with its reason.
// No assertion carries one of these ids.

#pragma once

#include "s3_check.h"

namespace s3 {

struct Withdrawn { const char* ids; const char* reason; };

inline void record_withdrawn() {
    static const Withdrawn rows[] = {
        {"opus:AA-sprhstop.c6 sol:AA-sprhstop.c7 sol:AA-sprhstop.c8 sol:AA-sprhstop.c9 sol:AA-sprhstop.c10 "
         "sol:AA-sprhstrt.c6 sol:AA-sprhstrt.c7 sol:AA-sprhstrt.c8 sol:AA-sprhstrt.c9 sol:AA-sprhstrt.c10",
         "p.28 prints x for these bits and does not say what it means; where other pages define x it is a don't-care bit to be written 0, "
         "and a don't-care bit has no expected effect, so the stimulus writes them as 0 and no test reads or grades them"},
        {"opus:AA-clxcon2.c8 opus:AA-clxcon2.c11 sol:AA-clxcon2.c8 sol:AA-clxcon2.c14",
         "CLXCON2 bits 15-08 and 05-02 are unused; an unused bit gives no expected value, the stimulus writes them as 0"},
        {"sol:AA-clxcon2.c9 sol:AA-clxcon2.c15",
         "the unused bits are initialized by reset, but a bit with no function in a write-only register cannot reach any output, so its reset value cannot be seen"},
        {"sol:AA-clxcon2.c17",
         "MVBP8 matters only while ENBP8 is 1, and ENBP8 becomes 1 only through a CLXCON2 write, which rewrites MVBP8 in the same write; "
         "the reset value of MVBP8 can never reach an output"},
        {"opus:AA-clxcon2.c13 sol:AA-clxcon2.c18",
         "the printed page does not carry the line RESET BY ECSENA after CLXCON2's table; even read that way it does not say which bits it governs, so there is no expected value"},
        {"opus:AA-sprxdatb.c12 sol:AA-sprxdata.c19",
         "the records state only that the sentence In lowres mode, breaks across a page; its continuation on p.29 is not a claim of this package"},
        {"sol:AA-bplxpth.c11",
         "the record is a misprint (x=1,2,3,45,6,7,8) that changes nothing a test can see; B1 uses the eight addresses the entry prints"},
        {"opus:AA-sprxctl.c18 sol:AA-sprxctl.c32",
         "POS and CTL are usually loaded by the Sprite DMA channel during horizontal blank: usually admits exceptions, "
         "and the horizontal blanking interval is not visible at the video output; S13 checks the loading by the Sprite DMA channel only"},
        {"opus:AA-sprhstop.c9",
         "SPRHWRM (ECS and later chips only): S16 checks the positive part only, because with OCS behaviour there are no UHRES pointers at all (p.16), "
         "so there is no UHRES cycle whose direction could be seen"},
        {"opus:AA-sprhstop.c8",
         "SPRHWRM swaps ARW* so that external devices can detect the RGA and put things into memory: S16 checks the swapped ARW* polarity only, a part that another claim states alone; "
         "the register address bus and the SPRHDAT identifier on it are not visible to a test"},
        {"opus:AA-clxcon2.c7 sol:AA-clxcon2.c7",
         "CLXCON2's bits are initialized by reset: C5 checks the reset value of ENBP8 and ENBP7 only; "
         "the reset values of the unused bits and of MVBP7 and MVBP8 cannot reach any output"},
        {"opus:AA-clxcon.c6 sol:AA-clxcon.c6 opus:AA-clxcon2.c6 sol:AA-clxcon2.c6",
         "a write to CLXCON resets the bits of CLXCON2: C4 checks that it clears ENBP7 and ENBP8 only; that it clears MVBP7, MVBP8 and the unused bits cannot be seen, "
         "because a match value acts only while its enable is 1 and the enable can only become 1 again through a CLXCON2 write, which rewrites the match value"},
        {"opus:AA-bplxptl.c7 sol:AA-bplxptl.c12",
         "p.15 says the bit-plane pointer must be reinitalized every vertical blank time; B2 performs two writes of plane 1's pointer, in two vertical blanks, and grades their address effect, "
         "which establishes neither the universal obligation nor a consequence of omitting a re-initialisation, and covers plane 1 only; B2 remains as a check of the pointer effect"},
        {"opus:AA-sprxptl.c7",
         "p.28 says the sprite pointer registers must be initialized every vertical blank time; S2 performs writes in two vertical blanks and grades the address effect for sprite 0, "
         "which establishes neither the universal obligation nor a consequence of omitting an initialisation; S2 remains as a check of the pointer effect"},
        {"opus:AA-sprxdatb.c6 sol:AA-sprxdata.c13",
         "the image data registers are usually loaded by the Sprite DMA channel: one DMA load of sprite 0 shows that the load is possible and that the data flow, not usually, "
         "for which the page defines no population or threshold; S13 remains as a mechanism check"},
        {"opus:AA-sprxctl.c19 sol:AA-sprxctl.c33 opus:AA-sprxdatb.c7 sol:AA-sprxdata.c14",
         "either processor may load the registers at any time: S14 samples a finite set of line-start, mid-line and vertical-blank writes and does not establish every instant, "
         "and the page does not state the latency from the next line on, which S14 MEASURES; S14 grades the value in effect on the last line before the next event"},
    };
    for (const auto& row : rows) {
        std::string ids = row.ids, tok;
        for (size_t i = 0; i <= ids.size(); ++i) {
            if (i == ids.size() || ids[i] == ' ') {
                if (!tok.empty()) checks.record("WITHDRAWN " + tok + ": not asserted: " + row.reason);
                tok.clear();
            } else tok += ids[i];
        }
    }
}

}  // namespace s3
