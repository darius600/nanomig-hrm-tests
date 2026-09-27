// appF-cia-pin-bindings -- minimig conformance test
//
// Test purpose: grade page 339's "Each Amiga system contains two 8520 CIA
//   chips", page 340's CIAB address-map figure (the /DTR, /RTS, /CD, /CTS,
//   /DSR pin bindings on CIAB's PRA), and page 353's "CIAA communicates on
//   data bits 7-0, CIAB communicates on data bits 15-8" -- all three
//   propositions name a fact only visible at `minimig` (the join where the
//   two physical 8520 chips, generic at their own `ciaa`/`ciab` boundary,
//   get bound to named system-level signals).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 339, 340 and 353.
// Non-goals: anything outside the `minimig` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// CLAIM COVERAGE: 21 of 38 uids are declined on record() lines (duplicate
//   citations, see SPECIFICATION.md). The remaining 17 uids (across 10
//   propositions) are graded with expect() below: TWO-CIA-CHIPS-EXIST
//   (sol:F-overview.c2, opus:F-intro.c2), CIAA-LOW-CIAB-HIGH-BYTE
//   (sol:F-signals.c59, sol:F-signals.c60, opus:F-conn.c3),
//   DTR-IS-PRA-BIT7/RTS-IS-PRA-BIT6/CD-IS-PRA-BIT5/CTS-IS-PRA-BIT4/
//   DSR-IS-PRA-BIT3 (sol:F-ciab-map.c3/c4/c5/c6/c7, each also citing
//   opus:F-ciab-map.c2, whose own single claim covers all five bindings in
//   one row -- see SPECIFICATION.md), CIAA-IRQ-REACHES-INT2 and
//   CIAA-IRQ-REACHES-PORTS (sol:F-signals.c84, opus:F-signals.c15,
//   sol:F-ciaa-map.c55, opus:F-ciaa-map.c32 -- both assertions establish
//   "CIAA can generate interrupt INT2" whole), CIAB-IRQ-REACHES-EXTER
//   (sol:F-ciab-map.c62, opus:F-ciab-map.c32, establishing "CIAB can
//   generate interrupt INT6" whole).
//
// EXTENSION: Figure 7-4's naming of the PORTS/EXTER interrupt sources
//   (p.221), claims this package carries although classify.package does not
//   name it -- see
//   SPECIFICATION.md's own extension section. opus:7-fig7-4.c12 and
//   opus:7-fig7-4.c42 each name TWO sources joined by "&" ("external INT2
//   & CIAA" / "external INT6 & CIAB"); only the CIA leg is measured here
//   (CIAA-IRQ-REACHES-PORTS / CIAB-IRQ-REACHES-EXTER below), and the
//   expansion-connector leg of each row is absent from this RTL entirely,
//   so both ids are withdrawn to record() rather than carried on a
//   passing label with a caveat -- see the PORTS-DENOTATION-TWO-SOURCES
//   and EXTER-DENOTATION-TWO-SOURCES record() lines below.
//   CIAA-IRQ-REACHES-PORTS and CIAB-IRQ-REACHES-EXTER instead establish,
//   whole, the single-source claims sol:F-ciaa-map.c55/opus:F-ciaa-map.c32
//   ("CIAA can generate interrupt INT2") and
//   sol:F-ciab-map.c62/opus:F-ciab-map.c32 ("CIAB can generate interrupt
//   INT6").
//
// RTL read directly: minimig.v's CIAA1/CIAB1 instantiations (lines
//   607-650) confirm CIAA's data_in/data_out are cpu_data_out/cia_data_out
//   [7:0] and CIAB's are [15:8]; CIAB1's porta_in/porta_out concatenation
//   (`.porta_in({cd,cts,dsr,ri&_joy3[4],1'b1,_joy4[4]})`,
//   `.porta_out({dtr,rts})`) combined with ciab.v's own PRA read mux
//   (`pa_out = {porta_out[7:6],porta_in2[5:0]}`, ciab.v:118) place dtr at
//   PRA bit 7, rts at bit 6, cd at bit 5, cts at bit 4, dsr at bit 3 --
//   confirmed by direct reading, then measured independently below, not
//   assumed from the reading alone.
//
// Fixture plumbing (ClkGen, Harness::tick/bus_cycle/write_byte/read_byte,
//   the long reset-acquisition budget) is shared with
//   hrm/ch7-overlay-source/test_minimig.cpp and
//   hrm/ch8-cia-disk-signal-bindings/test_minimig.cpp -- this suite's own
//   validated infrastructure for this identical TOP_MODULE=minimig
//   boundary (both document why: minimig_m68k_bridge.v's `_ta_n`
//   zero-inits instead of X, and minimig_syscontrol.v's own reset counter
//   needs several hundred thousand real clk7_en-qualified `sof` pulses --
//   video frame boundaries -- before `_cpu_reset`/`rst_out` release at
//   all, regardless of anything this package's own claims touch). It is
//   plumbing, not a source of this package's own assertions; the
//   CIAA/CIAB register addresses, PRA bit assignments and byte-lane checks
//   are this package's own.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag. Every mutation was actually built and run (WSL Verilator,
//   `make MUT=<NAME> test`, full `rm -rf obj_dir` rebuild).

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vminimig.h"
#include "Vminimig___024root.h"
#include "Vminimig_minimig.h"
#include "Vminimig_paula.h"

#ifndef MUT_ALIAS_CIAB_INTO_CIAA
#define MUT_ALIAS_CIAB_INTO_CIAA 0
#endif
#ifndef MUT_READ_HIGH_HALF
#define MUT_READ_HIGH_HALF 0
#endif
#ifndef MUT_SWAP_DTR_RTS_CHECK
#define MUT_SWAP_DTR_RTS_CHECK 0
#endif
#ifndef MUT_SWAP_CD_CHECK
#define MUT_SWAP_CD_CHECK 0
#endif
#ifndef MUT_SWAP_CTS_CHECK
#define MUT_SWAP_CTS_CHECK 0
#endif
#ifndef MUT_SWAP_DSR_CHECK
#define MUT_SWAP_DSR_CHECK 0
#endif
#ifndef MUT_NO_TIMERA_START
#define MUT_NO_TIMERA_START 0
#endif
#ifndef MUT_NO_TIMERA_START_PORTS
#define MUT_NO_TIMERA_START_PORTS 0
#endif
#ifndef MUT_NO_TIMERB_START_EXTER
#define MUT_NO_TIMERB_START_EXTER 0
#endif

namespace {

// Reproduces amiga_clk.v's exact register update -- verbatim from
// hrm/ch7-overlay-source/test_minimig.cpp via
// hrm/ch8-cia-disk-signal-bindings/test_minimig.cpp (this suite's own
// validated plumbing for this fixture, not a source of this package's own
// assertions).
struct ClkGen {
    uint8_t clk7_cnt = 2;
    bool clk7_en = true, clk7n_en = true, cck = true;
    uint16_t shifter = 1;
    bool c1 = false, c3 = false;
    void step() {
        uint8_t old_cnt = clk7_cnt;
        bool old_c3 = c3;
        bool old_cck = cck;
        uint16_t old_shifter = shifter;
        bool old_clk7 = (old_cnt >> 1) & 1;
        clk7_cnt = (old_cnt + 1) & 0x3;
        clk7_en = (old_cnt == 0);
        clk7n_en = (old_cnt == 2);
        if (old_cnt == 1) {
            cck = !old_cck;
            uint16_t rotated = (uint16_t)(((old_shifter << 1) | (old_shifter >> 9)) & 0x3FF);
            shifter = rotated ? rotated : 1;
        } else {
            cck = old_cck;
            shifter = old_shifter;
        }
        c3 = old_clk7;
        c1 = !old_c3;
    }
};

std::string hex(unsigned v) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%X", v);
    return std::string(buf);
}

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

class Harness {
public:
    Vminimig* dut;
    ClkGen cg;

    Harness() : dut(new Vminimig) {
        dut->clk = 0;
        dut->clk7_en = cg.clk7_en; dut->clk7n_en = cg.clk7n_en;
        dut->c1 = cg.c1; dut->c3 = cg.c3; dut->cck = cg.cck; dut->eclk = cg.shifter;
        dut->rst_ext = 0;
        dut->_cpu_reset_in = 0;
        dut->cpu_address = 0; dut->cpu_r_w = 1;
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1;
        dut->cpudata_in = 0;
        dut->memory_config = 0; dut->chipset_config = 0; dut->floppy_config = 0;
        dut->nmi_addr = 0; dut->chip48 = 0; dut->ramdata_in = 0;
        dut->rxd = 1; dut->cts = 1; dut->dsr = 1; dut->cd = 1; dut->ri = 1;
        dut->_joy1 = 0xFFFF; dut->_joy2 = 0xFFFF; dut->_joy3 = 0xFFFF; dut->_joy4 = 0xFFFF;
        dut->joya1 = 0; dut->joya2 = 0; dut->mouse_btn = 0;
        dut->kms_level = 0; dut->kbd_mouse_type = 0; dut->kbd_mouse_data = 0;
        dut->sdc_img_mounted = 0; dut->sdc_img_size = 0; dut->sdc_busy = 0; dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0; dut->sdc_byte_addr = 0; dut->sdc_byte_in_data = 0;
        dut->cpucfg = 0;

        // Same harness-hygiene throwaway _as edge hrm/ch7-overlay-source's
        // own Harness uses: Verilator zero-inits minimig_m68k_bridge.v's
        // `_ta_n`, so _cpu_dtack reads a stale 0 until the first _as
        // low->high edge.
        dut->_cpu_as = 0; tick(); tick();
        dut->_cpu_as = 1; tick(); tick();

        for (int i = 0; i < 8; ++i) tick();
        dut->_cpu_reset_in = 1;

        const long long BUDGET = 3000000;
        bool reset_released = false;
        for (long long i = 0; i < BUDGET && !reset_released; ++i) {
            tick();
            if (dut->_cpu_reset == 1) reset_released = true;
        }
        for (int i = 0; i < 8; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        cg.step();
        dut->clk7_en = cg.clk7_en; dut->clk7n_en = cg.clk7n_en;
        dut->c1 = cg.c1; dut->c3 = cg.c3; dut->cck = cg.cck; dut->eclk = cg.shifter;
    }

    // Verbatim (data-carrying) shape from
    // hrm/ch8-cia-disk-signal-bindings/test_minimig.cpp's own bus_cycle():
    // same dtack-wait/settle sequence. `raw` leaves dut->cpu_data as the
    // bus multiplexer's full 16-bit word for the caller to inspect
    // directly -- CIAA-LOW-CIAB-HIGH-BYTE's own point is which HALF of
    // that word carries a chip's data, so this package reads the raw word
    // itself rather than only using a byte-extracting helper that would
    // presuppose the answer.
    bool bus_cycle(uint32_t byte_addr, bool is_write, uint16_t write_data = 0) {
        bool low_byte = (byte_addr & 1u) != 0;
        dut->cpu_address = (byte_addr >> 1) & 0x7FFFFFu;
        dut->cpu_r_w = is_write ? 0 : 1;
        dut->cpudata_in = is_write ? write_data : 0;
        if (low_byte) { dut->_cpu_uds = 1; dut->_cpu_lds = 0; }
        else          { dut->_cpu_uds = 0; dut->_cpu_lds = 1; }
        dut->_cpu_as = 0;
        bool dtack_seen = false;
        for (int i = 0; i < 4000 && !dtack_seen; ++i) {
            tick();
            if (dut->_cpu_dtack == 0) dtack_seen = true;
        }
        for (int i = 0; i < 16; ++i) tick();
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1; dut->cpu_r_w = 1;
        for (int i = 0; i < 8; ++i) tick();
        return dtack_seen;
    }

    bool write_byte(uint32_t byte_addr, uint8_t value) {
        bool low = (byte_addr & 1u) != 0;
        uint16_t wd = low ? (uint16_t)value : (uint16_t)((uint16_t)value << 8);
        return bus_cycle(byte_addr, true, wd);
    }
    uint8_t read_byte(uint32_t byte_addr) {
        bus_cycle(byte_addr, false, 0);
        bool low = (byte_addr & 1u) != 0;
        uint16_t v = dut->cpu_data;
        return low ? (uint8_t)(v & 0xFF) : (uint8_t)((v >> 8) & 0xFF);
    }
    // Raw 16-bit read, no byte extraction -- see bus_cycle's own comment.
    uint16_t read_word_raw(uint32_t byte_addr) {
        bus_cycle(byte_addr, false, 0);
        return dut->cpu_data;
    }
};

constexpr uint32_t CIAA_PRA  = 0xBFE001;
constexpr uint32_t CIAA_DDRA = 0xBFE201;
constexpr uint32_t CIAB_PRA  = 0xBFD000;
constexpr uint32_t CIAB_DDRA = 0xBFD200;

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =====================================================================
    // Declined claims (record() only -- 21 of 38 uids; see
    // SPECIFICATION.md).
    // =====================================================================
    // (record() lines are emitted after the fresh assertions below, so a
    // reader sees this package's own measurements before its citations --
    // see the final block of main().)

    // =====================================================================
    // TWO-CIA-CHIPS-EXIST -- also this package's control/liveness case:
    // writing and reading back two distinct, independently-addressed
    // registers, on a path (full CPU bus cycle through minimig_m68k_bridge
    // + gary's address decode + both CIAs) every other assertion below
    // also depends on.
    // =====================================================================
    {
        Harness h;
        constexpr uint8_t CIAA_V = 0x37;
        constexpr uint8_t CIAB_V_BASE = 0xC8;
        const uint8_t CIAB_V = MUT_ALIAS_CIAB_INTO_CIAA ? CIAA_V : CIAB_V_BASE;

        h.write_byte(CIAA_DDRA, CIAA_V);
        const uint8_t ciaa_read1 = h.read_byte(CIAA_DDRA);
        h.write_byte(CIAB_DDRA, CIAB_V);
        const uint8_t ciab_read1 = h.read_byte(CIAB_DDRA);
        const uint8_t ciaa_read2 = h.read_byte(CIAA_DDRA);  // re-read after the CIAB write

        checks.note("TWO-CIA-CHIPS-EXIST: wrote CIAA DDRA ($BFE201)=0x37 -> read 0x" +
                    hex(ciaa_read1) + "; wrote CIAB DDRA ($BFD200)=0x" +
                    hex(CIAB_V) + " -> read 0x" + hex(ciab_read1) +
                    "; re-read CIAA after the CIAB write -> 0x" + hex(ciaa_read2) + ".");
        checks.expect(
            ciaa_read1 == CIAA_V && ciab_read1 == CIAB_V &&
            ciaa_read2 == CIAA_V && ciaa_read1 != ciab_read1,
            "TWO-CIA-CHIPS-EXIST (p.339, sol:F-overview.c2, opus:F-intro.c2): "
            "\"Each Amiga system contains two 8520 Complex Interface "
            "Adapter (CIA) chips.\" -- CIAA ($BFE201) and CIAB ($BFD200) "
            "each store and read back their own distinct written byte "
            "through a real CPU bus cycle, and writing CIAB does not "
            "disturb CIAA's already-written value -- two independently "
            "addressable chips, not one aliased twice.");
    }
    // MUTATION TWO-CIA-CHIPS-EXIST stimulus red MUT_ALIAS_CIAB_INTO_CIAA: write the identical byte to CIAB that was just written to CIAA instead of a different one -- a genuinely shared/aliased single chip could no longer be told apart from two independent ones by this measurement, so the ciaa_read1 != ciab_read1 conjunct goes red.

    // =====================================================================
    // CIAA-LOW-CIAB-HIGH-BYTE
    // =====================================================================
    {
        Harness h;
        h.write_byte(CIAA_DDRA, 0xA5);
        const uint16_t ciaa_word = h.read_word_raw(CIAA_DDRA);
        h.write_byte(CIAB_DDRA, 0x3C);
        const uint16_t ciab_word = MUT_READ_HIGH_HALF
            ? h.read_word_raw(CIAA_DDRA)   // mutated: re-inspect CIAA's own address instead of CIAB's
            : h.read_word_raw(CIAB_DDRA);

        checks.note("CIAA-LOW-CIAB-HIGH-BYTE: raw cpu_data after writing CIAA DDRA=0xA5 and "
                    "reading it back = 0x" + hex(ciaa_word) + "; after writing "
                    "CIAB DDRA=0x3C and reading it back = 0x" + hex(ciab_word) + ".");
        checks.expect(
            (ciaa_word & 0xFF) == 0xA5 && (ciaa_word >> 8) == 0x00 &&
            (ciab_word >> 8) == 0x3C && (ciab_word & 0xFF) == 0x00,
            "CIAA-LOW-CIAB-HIGH-BYTE (p.353, sol:F-signals.c59, "
            "sol:F-signals.c60, opus:F-conn.c3): \"CIAA communicates on "
            "data bits 7-0, CIAB communicates on data bits 15-8.\" -- a "
            "CIAA register's value appears only in cpu_data's low byte "
            "(high byte reads 0, from CIAB's own idle/unselected "
            "contribution), and a CIAB register's value appears only in "
            "the high byte (low byte reads 0) -- read directly off the "
            "16-bit bus word, not extracted by an address-parity-aware "
            "helper that would presuppose the split.");
    }
    // MUTATION CIAA-LOW-CIAB-HIGH-BYTE stimulus red MUT_READ_HIGH_HALF: re-read CIAA's own address instead of CIAB's for the second half of the check -- ciab_word becomes CIAA's word again (low byte 0xA5, high byte 0x00), so the (ciab_word >> 8) == 0x3C conjunct goes red.

    // =====================================================================
    // CIAB-PRA-PIN-BINDINGS: DTR (bit7), RTS (bit6), CD (bit5), CTS (bit4),
    // DSR (bit3) -- one Harness, two states per direction, so five
    // per-bit assertions share the same two measurements rather than each
    // paying for its own full reset-acquisition budget.
    // =====================================================================
    bool dtr1, rts1, dtr2, rts2;
    bool bit5_1, bit4_1, bit3_1, bit5_2, bit4_2, bit3_2;
    {
        Harness h;

        // Output half: DDRA bits 7,6 = output (1), rest = input (0).
        h.write_byte(CIAB_DDRA, 0xC0);
        h.write_byte(CIAB_PRA, MUT_SWAP_DTR_RTS_CHECK ? 0x40 : 0x80);  // bit7=1,bit6=0 (or swapped)
        h.dut->eval();
        dtr1 = h.dut->dtr; rts1 = h.dut->rts;
        h.write_byte(CIAB_PRA, MUT_SWAP_DTR_RTS_CHECK ? 0x80 : 0x40);  // bit7=0,bit6=1 (or swapped)
        h.dut->eval();
        dtr2 = h.dut->dtr; rts2 = h.dut->rts;

        // Input half: drive cd/cts/dsr to a distinguishable pattern, read PRA.
        h.dut->cd = MUT_SWAP_CD_CHECK ? 0 : 1;
        h.dut->cts = MUT_SWAP_CTS_CHECK ? 1 : 0;
        h.dut->dsr = MUT_SWAP_DSR_CHECK ? 0 : 1;
        for (int i = 0; i < 8; ++i) h.tick();
        uint8_t pra1 = h.read_byte(CIAB_PRA);
        bit5_1 = (pra1 >> 5) & 1; bit4_1 = (pra1 >> 4) & 1; bit3_1 = (pra1 >> 3) & 1;

        h.dut->cd = MUT_SWAP_CD_CHECK ? 1 : 0;
        h.dut->cts = MUT_SWAP_CTS_CHECK ? 0 : 1;
        h.dut->dsr = MUT_SWAP_DSR_CHECK ? 1 : 0;
        for (int i = 0; i < 8; ++i) h.tick();
        uint8_t pra2 = h.read_byte(CIAB_PRA);
        bit5_2 = (pra2 >> 5) & 1; bit4_2 = (pra2 >> 4) & 1; bit3_2 = (pra2 >> 3) & 1;

        checks.note("CIAB-PRA-PIN-BINDINGS: state1 dtr=" + std::to_string(dtr1) + " rts=" +
                    std::to_string(rts1) + " PRA=0x" + hex(pra1) + " (bit5/4/3=" +
                    std::to_string(bit5_1) + "/" + std::to_string(bit4_1) + "/" + std::to_string(bit3_1) +
                    "); state2 dtr=" + std::to_string(dtr2) + " rts=" + std::to_string(rts2) +
                    " PRA=0x" + hex(pra2) + " (bit5/4/3=" + std::to_string(bit5_2) + "/" +
                    std::to_string(bit4_2) + "/" + std::to_string(bit3_2) + ").");
    }

    checks.expect(dtr1 == 1 && dtr2 == 0,
        "DTR-IS-PRA-BIT7 (p.340, sol:F-ciab-map.c3, opus:F-ciab-map.c2): "
        "\"In the CIAB PRA image row, data bit 7 is /DTR.\" -- writing "
        "CIAB PRA with bit7=1,bit6=0 then bit7=0,bit6=1 moves minimig's "
        "own top-level `dtr` output pin to 1 then 0, following PRA bit 7 "
        "specifically.");
    // MUTATION DTR-IS-PRA-BIT7 stimulus red MUT_SWAP_DTR_RTS_CHECK: write PRA with bit7=0,bit6=1 first and bit7=1,bit6=0 second (the two states swapped) -- dtr1 reads 0 and dtr2 reads 1, so the dtr1==1 && dtr2==0 conjunct goes red.

    checks.expect(rts1 == 0 && rts2 == 1,
        "RTS-IS-PRA-BIT6 (p.340, sol:F-ciab-map.c4, opus:F-ciab-map.c2): "
        "\"In the CIAB PRA image row, data bit 6 is /RTS.\" -- the same "
        "two-state PRA write moves `rts` to 0 then 1, following PRA bit 6 "
        "specifically (the complementary pattern from DTR-IS-PRA-BIT7, "
        "not a value that would pass if the two bits were swapped).");
    // MUTATION RTS-IS-PRA-BIT6 stimulus red MUT_SWAP_DTR_RTS_CHECK: (shared flag, see DTR-IS-PRA-BIT7) -- rts1 reads 1 and rts2 reads 0 under the swapped states, so the rts1==0 && rts2==1 conjunct goes red.

    checks.expect(bit5_1 == 1 && bit5_2 == 0,
        "CD-IS-PRA-BIT5 (p.340, sol:F-ciab-map.c5, opus:F-ciab-map.c2): "
        "\"In the CIAB PRA image row, data bit 5 is /CD.\" -- driving "
        "minimig's top-level `cd` input pin 1 then 0 (cts/dsr held at the "
        "complementary pattern) moves PRA bit 5 to 1 then 0 specifically, "
        "read back through a real CPU bus cycle.");
    // MUTATION CD-IS-PRA-BIT5 stimulus red MUT_SWAP_CD_CHECK: drive `cd` 0 then 1 (swapped) while cts/dsr keep their original pattern -- bit5_1 reads 0 and bit5_2 reads 1, so the bit5_1==1 && bit5_2==0 conjunct goes red.

    checks.expect(bit4_1 == 0 && bit4_2 == 1,
        "CTS-IS-PRA-BIT4 (p.340, sol:F-ciab-map.c6, opus:F-ciab-map.c2): "
        "\"In the CIAB PRA image row, data bit 4 is /CTS.\" -- driving "
        "`cts` 0 then 1 moves PRA bit 4 to 0 then 1 specifically.");
    // MUTATION CTS-IS-PRA-BIT4 stimulus red MUT_SWAP_CTS_CHECK: drive `cts` 1 then 0 (swapped) while cd/dsr keep their original pattern -- bit4_1 reads 1 and bit4_2 reads 0, so the bit4_1==0 && bit4_2==1 conjunct goes red.

    checks.expect(bit3_1 == 1 && bit3_2 == 0,
        "DSR-IS-PRA-BIT3 (p.340, sol:F-ciab-map.c7, opus:F-ciab-map.c2): "
        "\"In the CIAB PRA image row, data bit 3 is /DSR.\" -- driving "
        "`dsr` 1 then 0 moves PRA bit 3 to 1 then 0 specifically.");
    // MUTATION DSR-IS-PRA-BIT3 stimulus red MUT_SWAP_DSR_CHECK: drive `dsr` 0 then 1 (swapped) while cd/cts keep their original pattern -- bit3_1 reads 0 and bit3_2 reads 1, so the bit3_1==1 && bit3_2==0 conjunct goes red.

    // =====================================================================
    // CIAA-IRQ-REACHES-INT2 -- p.354's general IRQ-pin description
    // (sol:F-signals.c84, opus:F-signals.c15), CIAA leg only (minimig.v:621,
    // irq -> int2): the claims' classify.note records the CIAB leg as
    // graded by hrm/ch8-disk-index-interrupt-path PROP-FLAG-PIN
    // (irq -> int6). ch1-cia-
    // timers already shows ciaa's own `irq` pin asserting in isolation;
    // what is graded here is whether that pin actually REACHES `int2` at the
    // `minimig` join -- a fact hrm/ch5-audio-interrupt's own note
    // independently says is not graded anywhere else either. This
    // assertion, together with CIAA-IRQ-REACHES-PORTS below, also
    // establishes sol:F-ciaa-map.c55/opus:F-ciaa-map.c32 ("CIAA can
    // generate interrupt INT2") whole -- see both assertions' own labels.
    // =====================================================================
    {
        Harness h;
        // Load Timer A with a small count, unmask ICR's TA bit (set-mode),
        // start the timer -- the same idiom hrm/ch1-cia-timers's own
        // ciaa-alone fixture already uses, now observed one level up at
        // int2 instead of at ciaa's own irq pin.
        constexpr uint32_t CIAA_TALO = 0xBFE401, CIAA_TAHI = 0xBFE501,
                            CIAA_ICR = 0xBFED01, CIAA_CRA = 0xBFEE01;
        h.write_byte(CIAA_TALO, 0x08);
        h.write_byte(CIAA_TAHI, 0x00);
        h.write_byte(CIAA_ICR, 0x81);  // set-mode, unmask TA (bit0)
        bool int2_before = h.dut->rootp->minimig->int2;
        if (!MUT_NO_TIMERA_START)
            h.write_byte(CIAA_CRA, 0x01);  // start, continuous mode
        bool int2_after = false;
        for (int i = 0; i < 4000 && !int2_after; ++i) {
            h.tick();
            if (h.dut->rootp->minimig->int2) int2_after = true;
        }
        checks.note("CIAA-IRQ-REACHES-INT2: int2 before starting Timer A = " +
                    std::to_string(int2_before) + "; after loading a small count, unmasking ICR's "
                    "TA bit and starting the timer, int2 became true = " + std::to_string(int2_after) + ".");
        checks.expect(!int2_before && int2_after,
            "CIAA-IRQ-REACHES-INT2 (p.354, sol:F-signals.c84, "
            "opus:F-signals.c15, sol:F-ciaa-map.c55, opus:F-ciaa-map.c32 "
            "-- graded for the CIAA leg only, per "
            "these uids' own classify.note: the CIAB leg is already "
            "graded by ch8-disk-index-interrupt-path PROP-FLAG-PIN "
            "(irq -> int6), so only the CIAA leg (minimig.v:621, "
            "irq -> int2) is fresh here): \"IRQ is an open drain output "
            "normally connected to the processor interrupt input.\" -- "
            "ciaa's own "
            "`irq` pin, driven true by an unmasked Timer A underflow (the "
            "same mechanism ch1-cia-timers already grades at ciaa alone), "
            "is not merely present at the chip boundary but actually "
            "reaches `int2` at the minimig.v:621 join -- int2 is false "
            "beforehand and becomes true once the timer underflows.");
    }
    // MUTATION CIAA-IRQ-REACHES-INT2 stimulus red MUT_NO_TIMERA_START: never write CRA's start bit at all -- Timer A never starts, so int2_after stays false and the int2_after conjunct goes red.

    // =====================================================================
    // CIAA-IRQ-REACHES-PORTS -- Figure 7-4's PORTS row (p.221) names the
    // interrupt raised by "external INT2 & CIAA", two sources joined by
    // "&". Only the CIA leg is graded here (the expansion-connector INT2*
    // input is absent from this RTL entirely -- SPECIFICATION.md's
    // extension section -- so opus:7-fig7-4.c12 itself is withdrawn to a
    // record() line below rather than carried on this passing label with
    // a caveat): does ciaa's own irq pin, already shown above to reach
    // int2, go on to actually set INTREQ bit 3? p.219 ("Bit 3, PORTS,
    // becomes a 1 when the system line called INT2* becomes a logic 0")
    // and p.280's register table ("03 PORTS 2 I/O ports and timers") name
    // bit 3 as PORTS; paula_intcontroller.v:112 (`intreq[3] <= tmp[3] |
    // int2;`) confirms which wire is inspected, not which bit PORTS is --
    // Figure 7-4 itself prints no bit numbers. Established whole here,
    // instead, is the single-source claim "CIAA can generate interrupt
    // INT2" (sol:F-ciaa-map.c55, opus:F-ciaa-map.c32). Same
    // Timer A/ICR/CRA idiom as CIAA-IRQ-REACHES-INT2, its own fresh
    // Harness, now read one register further downstream at
    // dut->rootp->minimig->PAULA1->pi1__DOT__intreq -- the same
    // --public-flat-rw path hrm/ch8-disk-interrupt-path already reads at
    // this identical minimig fixture.
    // =====================================================================
    {
        Harness h;
        constexpr uint32_t CIAA_TALO = 0xBFE401, CIAA_TAHI = 0xBFE501,
                            CIAA_ICR = 0xBFED01, CIAA_CRA = 0xBFEE01;
        h.write_byte(CIAA_TALO, 0x08);
        h.write_byte(CIAA_TAHI, 0x00);
        h.write_byte(CIAA_ICR, 0x81);  // set-mode, unmask TA (bit0)
        bool ports_before = ((h.dut->rootp->minimig->PAULA1->pi1__DOT__intreq >> 3) & 1) != 0;
        if (!MUT_NO_TIMERA_START_PORTS)
            h.write_byte(CIAA_CRA, 0x01);  // start, continuous mode
        bool ports_after = false;
        for (int i = 0; i < 4000 && !ports_after; ++i) {
            h.tick();
            if ((h.dut->rootp->minimig->PAULA1->pi1__DOT__intreq >> 3) & 1) ports_after = true;
        }
        checks.note("CIAA-IRQ-REACHES-PORTS: INTREQ bit 3 before starting Timer A = " +
                    std::to_string(ports_before) + "; after loading a small count, unmasking ICR's "
                    "TA bit and starting the timer, INTREQ bit 3 became true = " + std::to_string(ports_after) + ".");
        checks.expect(!ports_before && ports_after,
            "CIAA-IRQ-REACHES-PORTS (p.221, sol:F-ciaa-map.c55, "
            "opus:F-ciaa-map.c32): Figure 7-4's PORTS row names the CIA leg of "
            "\"external INT2 & CIAA\"; \"CIAA can generate interrupt INT2\" "
            "(p.339) -- CIAA's own `irq` pin, driven true by an unmasked Timer A "
            "underflow (the same mechanism CIAA-IRQ-REACHES-INT2 above grades at "
            "the int2 net), reaches all the way to INTREQ bit 3, the bit p.219 "
            "(\"Bit 3, PORTS, becomes a 1 when the system line called INT2* "
            "becomes a logic 0\") and p.280's register table (\"03 PORTS 2 I/O "
            "ports and timers\") both name PORTS -- paula_intcontroller.v:112 "
            "(`intreq[3] <= tmp[3] | int2;`) confirms which wire is inspected, "
            "not which bit PORTS is -- the expansion-connector half of Figure "
            "7-4's own condition (\"external INT2\") is absent from this RTL "
            "and out of scope; INTREQ bit 3 is false beforehand and becomes "
            "true once the timer underflows.");
    }
    // MUTATION CIAA-IRQ-REACHES-PORTS stimulus red MUT_NO_TIMERA_START_PORTS: never write CRA's start bit at all -- Timer A never starts, so INTREQ bit 3 never sets and the ports_after conjunct goes red.

    // =====================================================================
    // CIAB-IRQ-REACHES-EXTER -- the mirror of CIAA-IRQ-REACHES-PORTS for
    // CIAB/int6/EXTER: Figure 7-4's EXTER row (p.221) reads "external INT6
    // & CIAB", two sources joined by "&". Only the CIA leg is graded here
    // (the expansion-connector INT6* input is absent from this RTL
    // entirely, so opus:7-fig7-4.c42 itself is withdrawn to a record()
    // line below rather than carried on this passing label with a
    // caveat). p.219 ("Bit 13, EXTER, becomes a 1 when the system line
    // called INT6* becomes a logic 0") and p.280's register table ("13
    // EXTER 6 External interrupt") name bit 13 as EXTER; Figure 7-4 itself
    // prints no bit numbers. Established whole here, instead, is the
    // single-source claim "CIAB can generate interrupt INT6"
    // (sol:F-ciab-map.c62, opus:F-ciab-map.c32), through this RTL's own
    // minimig.v join rather than a testbench wiring of ciab.irq to int6.
    // CIAB's Timer A/ICR/CRA are the same generic cia_int/
    // cia_timera cores ciaa.v instantiates (ciab.v:175/195), addressed at
    // the even (high) byte the way CIAB_PRA/CIAB_DDRA above already are
    // (rs==4'h4/5/D/E confirmed directly in ciab.v, same encoding as
    // ciaa.v). Observes INTREQ bit 13 (paula_intcontroller.v:132,
    // `intreq[13] <= tmp[13] | int6;`).
    // =====================================================================
    {
        Harness h;
        constexpr uint32_t CIAB_TALO = 0xBFD400, CIAB_TAHI = 0xBFD500,
                            CIAB_ICR = 0xBFDD00, CIAB_CRA = 0xBFDE00;
        h.write_byte(CIAB_TALO, 0x08);
        h.write_byte(CIAB_TAHI, 0x00);
        h.write_byte(CIAB_ICR, 0x81);  // set-mode, unmask TA (bit0)
        bool exter_before = ((h.dut->rootp->minimig->PAULA1->pi1__DOT__intreq >> 13) & 1) != 0;
        if (!MUT_NO_TIMERB_START_EXTER)
            h.write_byte(CIAB_CRA, 0x01);  // start, continuous mode
        bool exter_after = false;
        for (int i = 0; i < 4000 && !exter_after; ++i) {
            h.tick();
            if ((h.dut->rootp->minimig->PAULA1->pi1__DOT__intreq >> 13) & 1) exter_after = true;
        }
        checks.note("CIAB-IRQ-REACHES-EXTER: INTREQ bit 13 before starting Timer A = " +
                    std::to_string(exter_before) + "; after loading a small count, unmasking ICR's "
                    "TA bit and starting the timer, INTREQ bit 13 became true = " + std::to_string(exter_after) + ".");
        checks.expect(!exter_before && exter_after,
            "CIAB-IRQ-REACHES-EXTER (p.221, sol:F-ciab-map.c62, "
            "opus:F-ciab-map.c32): Figure 7-4's EXTER row names the CIA leg of "
            "\"external INT6 & CIAB\"; \"CIAB can generate interrupt INT6\" "
            "(p.340) -- CIAB's own `irq` pin, driven true by an unmasked Timer A "
            "underflow (the same idiom as CIAA-IRQ-REACHES-PORTS above, at "
            "CIAB's own register addresses), reaches all the way to INTREQ bit "
            "13, the bit p.219 (\"Bit 13, EXTER, becomes a 1 when the system "
            "line called INT6* becomes a logic 0\") and p.280's register table "
            "(\"13 EXTER 6 External interrupt\") both name EXTER -- "
            "paula_intcontroller.v:132 (`intreq[13] <= tmp[13] | int6;`) "
            "confirms which wire is inspected, not which bit EXTER is -- the "
            "expansion-connector half of Figure 7-4's own condition "
            "(\"external INT6\") is absent from this RTL and out of scope; "
            "INTREQ bit 13 is false beforehand and becomes true once the "
            "timer underflows.");
    }
    // MUTATION CIAB-IRQ-REACHES-EXTER stimulus red MUT_NO_TIMERB_START_EXTER: never write CIAB's CRA start bit at all -- Timer A never starts, so INTREQ bit 13 never sets and the exter_after conjunct goes red.

    // =====================================================================
    // Declined claims (record() only -- 21 of 38 uids, all verified
    // against this package's own filtered claim data; see
    // SPECIFICATION.md's duplicate-grading check for each citation).
    // =====================================================================

    checks.record(
        "CIAA-MAP-PASSTHROUGH (p.339, ch8-ciaapra-fire-buttons "
        "PROP-BIT7-PASSTHROUGH / PROP-BIT6-PASSTHROUGH, uid-free in that "
        "package): the CIAA address-map figure's PRA bits 7 "
        "and 6 (fire-button pass-through pins) are already graded there. "
        "opus:F-ciaa-map.c2's own single claim spans this whole figure "
        "(PA7..PA1, cited here and on the two neighbouring record() lines "
        "below for the same reason). Declined in full, not re-asserted. "
        "[claims: sol:F-ciaa-map.c3, sol:F-ciaa-map.c4, opus:F-ciaa-map.c2]");

    checks.record(
        "CIAA-MAP-PA5-PA2 (p.339, ch8-cia-disk-signal-bindings "
        "PA5/PA4/PA3/PA2-BINDING, opus:8-cia.c6/sol:8-cia.c5, "
        "opus:8-cia.c11/sol:8-cia.c9, opus:8-cia.c16/sol:8-cia.c16, "
        "opus:8-cia.c18/sol:8-cia.c17): the CIAA address-map figure's PRA "
        "bits 5 through 2 (disk-sense pin bindings) are already graded "
        "there, individually. opus:F-ciaa-map.c2's own single claim spans "
        "this whole figure (PA7..PA1, cited here and on the two "
        "neighbouring record() lines below for the same reason). Declined "
        "in full, not re-asserted. [claims: sol:F-ciaa-map.c5, "
        "sol:F-ciaa-map.c6, sol:F-ciaa-map.c7, sol:F-ciaa-map.c8, "
        "opus:F-ciaa-map.c2]");

    checks.record(
        "CIAA-MAP-PA1 (p.339, ch5-audio-filter-bypass, 5-p157.c8/"
        "5-filt.c9): the CIAA address-map figure's PRA bit 1 (LED-control "
        "register half) is already graded there; that package's own "
        "finding is that the downstream (physical LED) half is not "
        "constructible for the audio filter, but the register half is, at "
        "`minimig`. Declined in full, not re-asserted. [claims: "
        "sol:F-ciaa-map.c9, opus:F-ciaa-map.c2]");

    checks.record(
        "CIAA-MAP-PA0-OVL-REGISTER (p.339/p.351, ch7-overlay-source "
        "OVL-BIT-WRITE-CLEARS-OVERLAY, sol:7-reset.c8/opus:7-reset.c4): "
        "sol:F-ciaa-map.c10 (this package's own citation of ch7-overlay-"
        "source, declined above) already establishes -- read directly, "
        "confirmed again here at minimig.v:860-862 -- that `ovl` clears "
        "on ANY write to ANY CIAA register (`sel_cia_a & (cpu_lwr|"
        "cpu_hwr)`), with no dependence on register address or bit "
        "pattern at all. That finding resolves both the register-level "
        "half p.351's PA0/OVL row names (already graded by ch7-overlay-"
        "source) AND the bit-level half sol:F-signals.c19/opus:F-portsig."
        "c15's own note called 'new here': there is no PA0-specific "
        "gating in this RTL to test bit-level, so no fresh assertion is "
        "built for it -- inventing one would test a mechanism this page's "
        "own RTL does not implement. Declined in full, not re-asserted. "
        "[claims: sol:F-ciaa-map.c10, sol:F-signals.c19, "
        "opus:F-portsig.c15]");

    checks.record(
        "PORTS-DENOTATION-TWO-SOURCES (p.221, opus:7-fig7-4.c12): Figure "
        "7-4's own restatement, \"the name PORTS denotes the interrupt "
        "raised by the condition 'external INT2 & CIAA'\", names TWO "
        "sources joined by \"&\". CIAA-IRQ-REACHES-PORTS above grades the "
        "CIA leg only (INTREQ bit 3 follows CIAA's own irq pin); the "
        "expansion-connector INT2* input this row's other half names is "
        "absent from this RTL entirely (SPECIFICATION.md's own extension "
        "section), so the two-source claim cannot be established whole "
        "here. Declined in full, not carried on a passing label with a "
        "caveat. [claims: opus:7-fig7-4.c12]");

    checks.record(
        "CIAB-MAP-PB7-PB0 (p.340, ch8-cia-disk-signal-bindings "
        "PB7..PB0-BINDING, opus:8-cia.c21/sol:8-cia.c20 through "
        "opus:8-cia.c49/sol:8-cia.c44): the CIAB address-map figure's PRB "
        "bits 7 through 0 (disk-control pin bindings) are already graded "
        "there, individually. opus:F-ciab-map.c4's own single claim spans "
        "all eight bits in one row. Declined in full, not re-asserted. "
        "[claims: sol:F-ciab-map.c13, sol:F-ciab-map.c14, "
        "sol:F-ciab-map.c15, sol:F-ciab-map.c16, sol:F-ciab-map.c17, "
        "sol:F-ciab-map.c18, sol:F-ciab-map.c19, sol:F-ciab-map.c20, "
        "opus:F-ciab-map.c4]");

    checks.record(
        "EXTER-DENOTATION-TWO-SOURCES (p.221, opus:7-fig7-4.c42): the "
        "mirror of PORTS-DENOTATION-TWO-SOURCES above, for CIAB/int6/EXTER. "
        "Figure 7-4's own restatement, \"the name EXTER denotes the "
        "interrupt raised by the condition 'external INT6 & CIAB'\", names "
        "TWO sources joined by \"&\". CIAB-IRQ-REACHES-EXTER above grades "
        "the CIA leg only; the expansion-connector INT6* input this row's "
        "other half names is absent from this RTL entirely, so the "
        "two-source claim cannot be established whole here. Declined in "
        "full, not carried on a passing label with a caveat. [claims: "
        "opus:7-fig7-4.c42]");

    checks.record(
        "INTRO-DUTY-ENUMERATION (p.339, ch1-cia-timers, declining the "
        "same duty enumeration from printed p.6, sol:1-components.c125/"
        "opus:1-components.c125, at the `ciaa`/`ciab` boundary): p.339's "
        "own opening sentence (\"handle the serial, parallel, keyboard and "
        "other Amiga I/O activities\") restates the same six-duty-style "
        "enumeration ch1-cia-timers already found to be a `minimig.v`-"
        "level wiring fact one level above its own two tops -- the same "
        "boundary this package's own fresh TWO-CIA-CHIPS-EXIST assertion "
        "sits at, but this particular sentence names no specific pin or "
        "bit this package's own fresh work does not already cover more "
        "precisely (the CIAB-PRA-PIN-BINDINGS assertions above). Declined "
        "in full, not re-asserted. [claims: opus:F-intro.c1]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
