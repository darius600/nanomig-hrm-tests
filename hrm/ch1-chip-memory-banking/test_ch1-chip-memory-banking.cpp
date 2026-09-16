// ch1-chip-memory-banking -- minimig_bankmapper conformance test
//
// Test purpose: how much Chip memory the custom chips can reach, and how
//   that ceiling changes across the Agnus generations the manual names
//   (A1000 original Agnus, ECS Fat Agnus 8372A, A3000 ECS) -- the banking
//   behaviour, not the memory-map boundary (that is
//   ch1-memory-map-and-two-buses, top gary; gary.v:159's "first 512K of
//   memory is Chip memory" is a different claim in that package and is not
//   cited here).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 3 and 5; committed specification 658c8de (SPECIFICATION.md, this
//   package).
// Requirement IDs (8 claims, 4 sentences x 2 independent readers -- (reader,
//   id) is the uid, the bare id is not):
//     sol:1-components.c40   opus:1-components.c40   (p.3)
//     sol:1-components.c93   opus:1-components.c92   (p.5, A1000)
//     sol:1-components.c101  opus:1-components.c100  (p.5, ECS Fat Agnus)
//     sol:1-components.c104  opus:1-components.c103  (p.5, A3000 ECS)
//   Claims file: hrm-testpipeline/work/2026-09-16_ch1_packages.jsonl,
//   filtered on classify.package == "ch1-chip-memory-banking" (8 claims).
//
// BOUNDARY: top module `minimig_bankmapper` only (one Verilator build, one
//   source file, minimig_bankmapper.v -- 38 lines, purely combinational,
//   no clock). Ports: chip0..chip3, slow0..slow2, kick, kick256kmirror,
//   memory_config[1:0], bank[7:0] (minimig_bankmapper.v:10-23). This
//   package drives chip0..chip3 directly, one live at a time, standing in
//   for whatever DMA address gary's decode (gary.v:147-150, cited in the
//   claims file's own evidence, not built here) would have produced for an
//   access into that logical 512K block. slow0..slow2, kick and
//   kick256kmirror are held at 0 throughout -- held deliberately, not left
//   floating -- because none of this package's 8 claims are about
//   slow/pseudo-fast memory or Kickstart mirroring.
//
// Non-goal: memory_config == 2 ("1.5M CHIP" per the RTL's own comment,
//   minimig_bankmapper.v:33) is not exercised by any assertion below -- no
//   sentence on pages 3 or 5 names a 1.5 MB configuration, so there is no
//   claim to grade it against. See SPECIFICATION.md.
//
// Every mutation below was replayed as a `-D` build variant (MUT_n, see
// Makefile: `make MUT=MUT_n test`), confirmed red in isolation, then
// rebuilt with no flags to confirm the clean baseline result returned.
//
// MUTATION FIXTURE-LIVE stimulus red MUT_0: the second probe point was changed from block 1 to block 0 again (chip0 probed twice instead of chip0 then chip1), so the two bank[3:0] readings no longer differ.
// MUTATION sol:1-components.c40 stimulus red MUT_1: the middle sample's memory_config was changed from 1 to 0, so the distinct-bank-count sequence becomes 1, 1, 4 instead of 1, 2, 4 -- no longer strictly increasing. (Assertion MEMORY-CONFIG-MONOTONIC-REACH; label leads with the first claim uid, sol:1-components.c40, per the gate's own convention.)
// MUTATION sol:1-components.c93 stimulus red MUT_2: memory_config was changed from 0 to 1 for the whole probe, so the four one-hot readings become {1,2,1,2} instead of all equal to 1. (Assertion A1000-512K-MIRROR; label leads with sol:1-components.c93.)
// MUTATION sol:1-components.c101 stimulus red MUT_3: memory_config was changed from 1 to 0, so chip1's and chip3's reading collapses onto chip0's and chip2's (one distinct bank, not two). (Assertion ECS-FATAGNUS-1MB-TWO-BANKS; label leads with sol:1-components.c101.)
// MUTATION sol:1-components.c104 stimulus red MUT_4: memory_config was changed from 3 to 1, so chip2's reading collapses onto chip0's (value 1, not 4) and chip3's onto chip1's (value 2, not 8) -- two distinct banks, not four. (Assertion A3000-ECS-2MB-FOUR-BANKS; label leads with sol:1-components.c104.)

#include <cstdio>
#include <iostream>
#include <set>
#include <string>
#include "Vminimig_bankmapper.h"
#include "verilated.h"

#ifndef MUT_0
#define MUT_0 0
#endif
#ifndef MUT_1
#define MUT_1 0
#endif
#ifndef MUT_2
#define MUT_2 0
#endif
#ifndef MUT_3
#define MUT_3 0
#endif
#ifndef MUT_4
#define MUT_4 0
#endif

namespace {

std::string hex2(uint8_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "$%01X", static_cast<unsigned>(v & 0x0F));
    return std::string(buf);
}

class Harness {
public:
    Harness() : dut(new Vminimig_bankmapper) {
        dut->chip0 = 0;
        dut->chip1 = 0;
        dut->chip2 = 0;
        dut->chip3 = 0;
        dut->slow0 = 0;
        dut->slow1 = 0;
        dut->slow2 = 0;
        dut->kick = 0;
        dut->kick256kmirror = 0;
        dut->memory_config = 0;
        dut->eval();
    }
    ~Harness() { delete dut; }

    // Drives the one-hot Chip-memory 512KB-block select that gary's DMA
    // address decode (gary.v:147-150) would produce for an access whose top
    // address bits select logical block `which` (0..3), at the given
    // memory_config, and returns bank[3:0] -- the nibble
    // (minimig_bankmapper.v:31-34) that determines which physical 512KB
    // bank that logical block is routed onto. slow0..slow2, kick and
    // kick256kmirror stay at 0 (set once in the constructor, never
    // touched here) -- this package's claims are about Chip memory only.
    uint8_t probe_bank(int which, uint8_t cfg) {
        dut->chip0 = (which == 0) ? 1 : 0;
        dut->chip1 = (which == 1) ? 1 : 0;
        dut->chip2 = (which == 2) ? 1 : 0;
        dut->chip3 = (which == 3) ? 1 : 0;
        dut->memory_config = cfg;
        dut->eval();
        return static_cast<uint8_t>(dut->bank & 0x0F);
    }

    Vminimig_bankmapper* dut;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // FIXTURE-LIVE -- minimig_bankmapper has never been measured by this
    // package before. At memory_config = 3 (the configuration with the
    // least aliasing, per minimig_bankmapper.v:34), chip0 alone and chip1
    // alone must read back two distinct bank[3:0] values before anything
    // below trusts this path. This is also this package's control case: it
    // uses the identical probe_bank() measurement basis as every graded
    // assertion below and is expected to, and does, PASS.
    // =========================================================================
    {
        Harness h;
        uint8_t b_chip0 = h.probe_bank(0, /*cfg=*/3);
#if MUT_0
        uint8_t b_chip1 = h.probe_bank(0, /*cfg=*/3);  // MUTATION: block 0 probed twice
#else
        uint8_t b_chip1 = h.probe_bank(1, /*cfg=*/3);
#endif
        checks.expect(
            b_chip0 != b_chip1,
            "FIXTURE-LIVE: at memory_config=3, probing chip0 alone and "
            "chip1 alone reads back two distinct bank[3:0] values before "
            "any assertion below trusts this path [no page -- "
            "infrastructure / control case]");
        checks.note("FIXTURE-LIVE: chip0->" + hex2(b_chip0) + " chip1->" + hex2(b_chip1));
    }

    // =========================================================================
    // sol:1-components.c40 opus:1-components.c40 --
    // MEMORY-CONFIG-MONOTONIC-REACH (p.3): "The more Chip memory the Amiga
    // has, the more graphics, audio, and I/O data it can operate on without
    // the CPU being involved." Graded at this boundary as the structural,
    // monotonic claim the sentence actually makes: the number of
    // pairwise-distinct physical 512K banks the four logical chip0..chip3
    // blocks are routed onto strictly grows as memory_config increases
    // across the three points pages 3 and 5 name (0 = A1000, "0.5M CHIP";
    // 1 = ECS Fat Agnus, "1.0M CHIP"; 3 = A3000 ECS, "2.0M CHIP"). The
    // sentence names no quantity of "data", so what is graded is the
    // addressable region, not a throughput.
    // =========================================================================
    {
        Harness h;
        auto distinct_count = [&](uint8_t cfg) {
            std::set<uint8_t> s;
            for (int which = 0; which < 4; ++which) s.insert(h.probe_bank(which, cfg));
            return s.size();
        };
        size_t n0 = distinct_count(/*cfg=*/0);
#if MUT_1
        size_t n1 = distinct_count(/*cfg=*/0);  // MUTATION: middle sample re-uses cfg=0, not 1
#else
        size_t n1 = distinct_count(/*cfg=*/1);
#endif
        size_t n3 = distinct_count(/*cfg=*/3);
        checks.expect(
            n0 < n1 && n1 < n3,
            "sol:1-components.c40 opus:1-components.c40 "
            "MEMORY-CONFIG-MONOTONIC-REACH: the count of pairwise-distinct "
            "bank[3:0] readings across chip0..chip3 strictly increases as "
            "memory_config goes 0 -> 1 -> 3 (1 -> 2 -> 4 distinct physical "
            "512K banks), the addressable Chip-memory region growing "
            "exactly as page 3 states, measured by probing and counting "
            "distinct banks rather than asserted from the field's declared "
            "width [p.3 \"The more Chip memory the Amiga has, the more "
            "graphics, audio, and I/O data it can operate on without the "
            "CPU being involved.\"]");
        checks.note("MEMORY-CONFIG-MONOTONIC-REACH: distinct banks at cfg=0,1,3 -> " +
                     std::to_string(n0) + ", " + std::to_string(n1) + ", " + std::to_string(n3));
    }

    // =========================================================================
    // sol:1-components.c93 opus:1-components.c92 -- A1000-512K-MIRROR (p.5):
    // "The original custom chip set found in the A1000 could access the
    // first 512K of RAM." At memory_config = 0 ("0.5M CHIP",
    // minimig_bankmapper.v:31), every one of chip0..chip3 ORs onto the same
    // single bank bit -- all four logical 512K blocks collapse onto one
    // physical 512K bank, and it is the SAME bank chip0 alone reaches at
    // memory_config = 3 below (bank[3:0] == 1, i.e. bit 0, "the first"
    // 512K in this module's own bit ordering) -- not merely some undefined
    // bank.
    // =========================================================================
    {
        Harness h;
        uint8_t cfg = 0;
#if MUT_2
        cfg = 1;  // MUTATION: memory_config raised from 0 to 1 for the whole probe
#endif
        uint8_t b0 = h.probe_bank(0, cfg);
        uint8_t b1 = h.probe_bank(1, cfg);
        uint8_t b2 = h.probe_bank(2, cfg);
        uint8_t b3 = h.probe_bank(3, cfg);
        const bool all_equal = (b0 == b1) && (b1 == b2) && (b2 == b3);
        checks.expect(
            all_equal && b0 == 1,
            "sol:1-components.c93 opus:1-components.c92 A1000-512K-MIRROR: "
            "at memory_config=0, all four one-hot chip0..chip3 probes read "
            "back the identical bank[3:0]=$1 -- the whole 2 MB logical "
            "Chip-memory window collapses onto the single, first physical "
            "512K bank, exactly the ceiling the A1000's original custom "
            "chip set had [p.5 \"The original custom chip set found in the "
            "A1000 could access the first 512K of RAM.\"]");
        checks.note("A1000-512K-MIRROR: chip0->" + hex2(b0) + " chip1->" + hex2(b1) +
                     " chip2->" + hex2(b2) + " chip3->" + hex2(b3));
    }

    // =========================================================================
    // sol:1-components.c101 opus:1-components.c100 --
    // ECS-FATAGNUS-1MB-TWO-BANKS (p.5): "ECS Fat Agnus (8372A) can access up
    // to one megabyte of Chip memory." At memory_config = 1 ("1.0M CHIP",
    // minimig_bankmapper.v:32), chip0/chip2 share one bank bit and
    // chip1/chip3 share a second, distinct bank bit -- exactly two physical
    // 512K banks, 1 MB, twice the A1000's reach and half the A3000 ECS's.
    // =========================================================================
    {
        Harness h;
        uint8_t cfg = 1;
#if MUT_3
        cfg = 0;  // MUTATION: memory_config lowered from 1 to 0
#endif
        uint8_t b0 = h.probe_bank(0, cfg);
        uint8_t b1 = h.probe_bank(1, cfg);
        uint8_t b2 = h.probe_bank(2, cfg);
        uint8_t b3 = h.probe_bank(3, cfg);
        checks.expect(
            b0 == b2 && b1 == b3 && b0 != b1,
            "sol:1-components.c101 opus:1-components.c100 "
            "ECS-FATAGNUS-1MB-TWO-BANKS: at memory_config=1, chip0 and "
            "chip2 read back one bank[3:0] value and chip1 and chip3 read "
            "back a second, distinct value -- exactly two physical 512K "
            "banks reachable, 1 MB [p.5 \"ECS Fat Agnus (8372A) can access "
            "up to one megabyte of Chip memory.\"]");
        checks.note("ECS-FATAGNUS-1MB-TWO-BANKS: chip0->" + hex2(b0) + " chip1->" + hex2(b1) +
                     " chip2->" + hex2(b2) + " chip3->" + hex2(b3));
    }

    // =========================================================================
    // sol:1-components.c104 opus:1-components.c103 --
    // A3000-ECS-2MB-FOUR-BANKS (p.5): "In the A3000, the Enhanced Chip Set
    // can access up to two megabytes of Chip memory." At memory_config = 3
    // ("2.0M CHIP", minimig_bankmapper.v:34), chip0..chip3 each read back
    // their own distinct bank[3:0] value -- no aliasing at all, the full
    // 2 MB.
    // =========================================================================
    {
        Harness h;
        uint8_t cfg = 3;
#if MUT_4
        cfg = 1;  // MUTATION: memory_config lowered from 3 to 1
#endif
        uint8_t b0 = h.probe_bank(0, cfg);
        uint8_t b1 = h.probe_bank(1, cfg);
        uint8_t b2 = h.probe_bank(2, cfg);
        uint8_t b3 = h.probe_bank(3, cfg);
        checks.expect(
            b0 == 1 && b1 == 2 && b2 == 4 && b3 == 8,
            "sol:1-components.c104 opus:1-components.c103 "
            "A3000-ECS-2MB-FOUR-BANKS: at memory_config=3, chip0..chip3 "
            "each read back their own distinct bank[3:0] value ($1,$2,$4,$8) "
            "-- four physical 512K banks with no aliasing, the full 2 MB "
            "[p.5 \"In the A3000, the Enhanced Chip Set can access up to "
            "two megabytes of Chip memory.\"]");
        checks.note("A3000-ECS-2MB-FOUR-BANKS: chip0->" + hex2(b0) + " chip1->" + hex2(b1) +
                     " chip2->" + hex2(b2) + " chip3->" + hex2(b3));
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
