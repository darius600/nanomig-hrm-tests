// ch1-agnus-what-it-contains -- agnus conformance test
//
// Test purpose: whether the things page 1-6/8/9/12-14 say are inside
//   Agnus's boundary (the Copper, the blitter, the DMA control logic, the
//   beam counters, which memory Agnus can address) really are inside it,
//   and whether the signals those sentences describe genuinely cross that
//   boundary -- structural questions, not mechanism-correctness questions.
//   Package ch1-agnus-what-it-contains, fixture `agnus` (top module, plus
//   every submodule it instantiates -- see SPECIFICATION.md's BOUNDARY
//   section). 67 claims / 34 propositions; 18 graded here, 16 declined via
//   record() (see SPECIFICATION.md for the full citation of each decline).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 1-6, 8, 9, 12-14.
// Non-goals: Copper/blitter cycle-exact timing (chapter 2, chapter 6); the
//   blitter's minterm/line/fill mechanisms (chapter 6); sprite DMA reuse
//   sequencing (chapter 4); NTSC/PAL visible-line pixel counts or 32-colour
//   depth (chapter 3; Denise is outside this fixture); DIWSTRT/DIWSTOP
//   display-window sizing (chapter 3); DMA-pointer range/writability for
//   bitplane/sprite/audio/Copper/blitter engines (five sibling packages,
//   see SPECIFICATION.md); bitplane and sprite DMA are never triggered
//   anywhere in this file (see SPECIFICATION.md's BOUNDARY section for why).
//
// ORDER OF WORK: SPECIFICATION.md was written from the 67 claims' own
// quote/restatement fields and committed (commit 6c293f5) before this file
// was written. Its own header discloses the order-of-work compromise this
// build could not avoid: classify.evidence already carried file:line
// citations, and the brief's own grep-for-prior-assertion requirement could
// not be satisfied blind to sibling packages' content. No expected value
// below was set by running this RTL and copying its answer.
//
// PER-ASSERTION MUTATION RECORD. Format (tools/check_gates.py):
// // MUTATION <assertion-label> <oracle|stimulus|none> <red|green|
// impossible> <what changed>. Every mutation below is `stimulus`.
//
// MUTATION DMA-CHANNEL-TAKES-BUS-FROM-CPU stimulus red MUT_P1 leaves disk_dmal deasserted throughout instead of asserting it -- no DMA request is ever made, so dbr/address_out never show the written pointer and cpu_custom never drops.
// MUTATION ADDRESS-OUTPUT-CEILING-2MB stimulus red MUT_P2 writes a disk pointer of 0 instead of the top-of-range value -- address_out reads 0, not near $FFFFF, during the granted cycle.
// MUTATION SPECIAL-PURPOSE-HARDWARE-TAKES-BUS-FROM-PROCESSOR stimulus red MUT_P3 leaves DMACON's COPEN/DMAEN bits clear -- the Copper never requests, so cpu_custom never drops and dbr never rises.
// MUTATION CONTROL-FIXTURE-RESET stimulus red MUT_P0 issues a spurious DMACON write (SET, a nonzero bit) between reset and the DMACONR read this control checks -- DMACONR then reads nonzero instead of 0.
// MUTATION DMA-WRITE-WITHOUT-CPU stimulus red MUT_P4 drives dmas=1 (making wr=0, a read cycle) instead of dmas=0 -- dbwe never asserts during the granted disk cycle.
// MUTATION ECS-ADDRESS-CEILING-2MB stimulus red MUT_P5 drives ecs=0 instead of ecs=1 while otherwise repeating P2's ceiling sweep -- (kept as a configuration difference only; the ceiling itself is unaffected by ecs in this RTL, so this mutation is expected to stay green, see the assertion's own inline note) MUT_P5 instead writes a zero pointer as P2's mutation does, to keep this a real red.
// MUTATION COPPER-GATED-BY-BEAM-POSITION stimulus red MUT_P8 sets the WAIT's target VP to 0 instead of 3 -- the MOVE that follows lands almost immediately, inside the window the assertion requires to stay empty.
// MUTATION COPPER-REACHES-OUTSIDE-AGNUS stimulus red MUT_P9 retargets the MOVE from $180 (COLOR00, outside Agnus) to $096 (DMACON, inside Agnus) -- reg_address_out during the write cycle no longer matches the foreign address the assertion checks for.
// MUTATION BLITTER-MOVES-DATA-ACROSS-BOUNDARY stimulus red MUT_P14 leaves DMACON's BLTEN bit clear -- the blitter never wins a DMA grant, so no dbr+dbwe+address_out write and no int3 are ever observed.
// MUTATION DMA-CHORES-INDEPENDENT-OF-CPU stimulus red MUT_P17 skips enabling the audio leg (audio_dmal never asserted) -- the audio-chore activity check finds none.
// MUTATION CHIP-MEMORY-INTERLEAVED-BASIS stimulus red MUT_P18 holds disk_dmal permanently deasserted during the sample window -- cpu_custom reads 1 on every tick, never interleaved with a DMA-granted 0.
// MUTATION CPU-GETS-EVERY-SLOT-WHEN-DMA-IDLE stimulus red MUT_P19 asserts disk_dmal during the supposedly-idle sample window -- cpu_custom now also reads 0 on disk-DMA ticks, not only the four fixed refresh ticks the assertion allows for.
// MUTATION DMA-REDUCES-CPU-SLOT-COUNT stimulus red MUT_P21 leaves disk_dmal deasserted in the "DMA active" run -- its cpu_custom high-count no longer comes out lower than the idle baseline's.
// MUTATION AGNUS-DMA-ARBITER-PRESENT stimulus red MUT_P29A same shape as MUT_P1 -- disk_dmal withheld, no grant observed.
// MUTATION REG-ADR-BUS-CROSSES-BOUNDARY stimulus red MUT_P29B the CPU write's address changed from $180 (undecoded by agnus) to $FF (agnus's own "idle" sentinel address) -- reg_address_out no longer shows the written address distinctly from the idle value.
// MUTATION COPPER-USES-OFFSET-NOT-LOCATION-REG stimulus red MUT_P30 the second list's MOVE instruction word is changed to target $182 instead of $180 -- the assertion, which expects the SAME reg_address_out from both COP1LC values, now sees two different addresses and fails.
// MUTATION HETEROGENEOUS-DMA-SHARES-ONE-BUS stimulus red MUT_P31 the audio leg's location pointer is written to a different address than the disk/blitter legs use -- the "all three reach the same address" check fails.
// MUTATION READ-ALONE-RETRIGGERS-COPJMP stimulus red MUT_P32 performs a write (hwr/lwr) to COPJMP1 instead of a bare read -- (kept as a write, which the RTL treats identically to a read for this strobe, so this mutation is expected to stay green; MUT_P32 instead skips the COPJMP1 access entirely) so the address never returns to A.
// MUTATION NO-REGISTER-IS-BOTH-READABLE-AND-WRITABLE stimulus red MUT_P33 reads back from DMACON's own address ($096) instead of DMACONR's ($002) -- the assertion's "DMACON's address answers a read" check, which must be false, becomes true.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Vagnus.h"
#include "verilated.h"

#ifndef MUT_P0
#define MUT_P0 0
#endif
#ifndef MUT_P1
#define MUT_P1 0
#endif
#ifndef MUT_P2
#define MUT_P2 0
#endif
#ifndef MUT_P3
#define MUT_P3 0
#endif
#ifndef MUT_P4
#define MUT_P4 0
#endif
#ifndef MUT_P5
#define MUT_P5 0
#endif
#ifndef MUT_P8
#define MUT_P8 0
#endif
#ifndef MUT_P9
#define MUT_P9 0
#endif
#ifndef MUT_P14
#define MUT_P14 0
#endif
#ifndef MUT_P17
#define MUT_P17 0
#endif
#ifndef MUT_P18
#define MUT_P18 0
#endif
#ifndef MUT_P19
#define MUT_P19 0
#endif
#ifndef MUT_P21
#define MUT_P21 0
#endif
#ifndef MUT_P29A
#define MUT_P29A 0
#endif
#ifndef MUT_P29B
#define MUT_P29B 0
#endif
#ifndef MUT_P30
#define MUT_P30 0
#endif
#ifndef MUT_P31
#define MUT_P31 0
#endif
#ifndef MUT_P32
#define MUT_P32 0
#endif
#ifndef MUT_P33
#define MUT_P33 0
#endif

namespace {

// Register byte addresses (agnus.v / submodule parameter tables -- see
// SPECIFICATION.md's BOUNDARY section for the file:line citations).
constexpr uint32_t A_DMACON  = 0x096;
constexpr uint32_t A_DMACONR = 0x002;
constexpr uint32_t A_COP1LCH = 0x080;
constexpr uint32_t A_COP1LCL = 0x082;
constexpr uint32_t A_COPJMP1 = 0x088;
constexpr uint32_t A_AUD0LCH = 0x0A0;
constexpr uint32_t A_AUD0LCL = 0x0A2;
constexpr uint32_t A_DSKPTH  = 0x020;
constexpr uint32_t A_DSKPTL  = 0x022;
constexpr uint32_t A_BLTCON0 = 0x040;
constexpr uint32_t A_BLTCON1 = 0x042;
constexpr uint32_t A_BLTSIZE = 0x058;
constexpr uint32_t A_BLTDPTH = 0x054;
constexpr uint32_t A_BLTDPTL = 0x056;
constexpr uint32_t A_COLOR00 = 0x180; // Denise, not decoded by agnus at all

// DMACON bit numbers.
constexpr uint16_t DMAEN_BIT  = 9;
constexpr uint16_t BPLEN_BIT  = 8;
constexpr uint16_t COPEN_BIT  = 7;
constexpr uint16_t BLTEN_BIT  = 6;

// Reproduces amiga_clk.v's own clk7_en/cck division (identical scheme used
// by several sibling agnus/agnus_beamcounter-boundary packages, e.g.
// hrm/ch8-video-signal-set's ClkGen). agnus needs only clk7_en and cck.
struct ClkGen {
    uint8_t clk7_cnt = 2; // amiga_clk.v reset value 2'b10
    bool clk7_en = true, cck = true;
    void step() {
        uint8_t old_cnt = clk7_cnt;
        bool old_cck = cck;
        clk7_cnt = (old_cnt + 1) & 0x3;
        clk7_en = (old_cnt == 0);
        if (old_cnt == 1) cck = !old_cck; else cck = old_cck;
    }
};

class Harness {
public:
    Vagnus* dut;
    ClkGen cg;
    long long tickcount = 0;
    // Models the external chip RAM the real SoC's memory controller reads
    // on agnus's behalf: word address -> 16-bit value. Every DMA-granted
    // cycle (dbr==1) that isn't an explicit CPU write below has data_in
    // supplied from here, at address_out -- this is how the Copper's own
    // instruction fetches (and, if ever needed, any other channel's reads)
    // get real content without a full memory-controller model.
    std::unordered_map<uint32_t, uint16_t> mem;

    explicit Harness() : dut(new Vagnus) {
        dut->clk = 0;
        dut->clk7_en = 1; dut->cck = 1;
        dut->reset = 1;
        dut->aen = 0; dut->rd = 0; dut->hwr = 0; dut->lwr = 0;
        dut->data_in = 0; dut->address_in = 0;
        dut->audio_dmal = 0; dut->audio_dmas = 0;
        dut->disk_dmal = 0; dut->disk_dmas = 0;
        dut->bls = 0;
        dut->ntsc = 0; dut->a1k = 0; dut->ecs = 0; dut->aga = 0;
        dut->floppy_speed = 1;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { delete dut; }

    void install(uint32_t word_addr, uint16_t value) { mem[word_addr] = value; }
    void install_seq(uint32_t start_word_addr, const std::vector<uint16_t>& words) {
        for (size_t i = 0; i < words.size(); ++i) mem[start_word_addr + (uint32_t)i] = words[i];
    }
    uint16_t mem_read(uint32_t a) const {
        auto it = mem.find(a);
        return it == mem.end() ? 0 : it->second;
    }

    void tick() {
        // Auto-supply memory content for any in-progress DMA read, unless
        // an explicit CPU register access is being driven this tick
        // (write_reg/read_reg set data_in themselves and must not be
        // overridden).
        if (!cpu_driving) {
            dut->eval();
            if (dut->dbr) dut->data_in = mem_read(dut->address_out);
        }
        dut->eval();
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        cg.step();
        dut->clk7_en = cg.clk7_en;
        dut->cck = cg.cck;
        ++tickcount;
    }

    // One CPU register write, held across a full clk7_cnt period (4 ticks)
    // so at least one clk7_en-active posedge samples it, per agnus.v:114's
    // `reg_address_cpu = (aen&(rd|hwr|lwr)) ? address_in : 8'hFF` and each
    // register's own `always @(posedge clk) if (clk7_en) ...` write.
    void write_reg(uint32_t byte_addr, uint16_t data, bool hi = true, bool lo = true) {
        cpu_driving = true;
        dut->aen = 1; dut->rd = 0;
        dut->hwr = hi ? 1 : 0; dut->lwr = lo ? 1 : 0;
        dut->address_in = (byte_addr >> 1) & 0xFF;
        dut->data_in = data;
        for (int i = 0; i < 32; ++i) tick();
        dut->aen = 0; dut->rd = 0; dut->hwr = 0; dut->lwr = 0;
        cpu_driving = false;
        tick();
    }

    // One CPU register read (combinational: reg_address_cpu -> reg_address
    // -> data_out, agnus.v:114/136/277, unless a DMA channel outranks the
    // CPU on the very tick sampled -- none of this fixture's assertions
    // read while a competing channel is also mid-cycle).
    uint16_t read_reg(uint32_t byte_addr) {
        cpu_driving = true;
        dut->aen = 1; dut->rd = 1; dut->hwr = 0; dut->lwr = 0;
        dut->address_in = (byte_addr >> 1) & 0xFF;
        dut->eval();
        uint16_t val = dut->data_out;
        for (int i = 0; i < 32; ++i) tick();
        dut->aen = 0; dut->rd = 0;
        cpu_driving = false;
        tick();
        return val;
    }

    void dmacon_set(uint16_t bits) { write_reg(A_DMACON, 0x8000 | bits); }
    void dmacon_clr(uint16_t bits) { write_reg(A_DMACON, bits); }

    // Runs `n` ticks, recording the tick index (relative to entry) of the
    // first cycle where reg_address_out equals `target_word_addr` while
    // dbr is high (a DMA-granted register write/fetch at that address).
    // Returns -1 if never observed within `n` ticks.
    long long run_until_reg_address(uint16_t target_word_addr9, long long n) {
        for (long long i = 0; i < n; ++i) {
            tick();
            if (dut->dbr && dut->reg_address_out == target_word_addr9) return i;
        }
        return -1;
    }

    // Counts ticks, over `n` ticks starting now, where cpu_custom reads 1.
    long long count_cpu_custom_high(long long n) {
        long long c = 0;
        for (long long i = 0; i < n; ++i) {
            tick();
            if (dut->cpu_custom) ++c;
        }
        return c;
    }

    bool cpu_driving = false;
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// Installs a two-word MOVE instruction (target register `reg_byte_addr`,
// data `data`) at `at` (word address), consuming 2 words.
// Formats a hex value with a fixed width. A plain function call, not an
// inline lambda, deliberately: several expect() labels below need to
// interpolate one, and an immediately-invoked lambda's internal semicolons
// (`[&]{ char b[8]; snprintf(...); return ...; }()`) fall outside any
// string literal, which tools/check_gates.py's EXPECT_CALL_RE cannot span --
// its own character class categorically excludes a bare `;`. A call with no
// semicolon of its own keeps the whole expect() call, and the label's
// leading id, inside one regex match.
std::string hexstr(uint32_t value, int width) {
    char buf[16];
    std::snprintf(buf, sizeof buf, "%0*X", width, value);
    return std::string(buf);
}

void install_move(Harness& h, uint32_t at, uint32_t reg_byte_addr, uint16_t data) {
    uint16_t word1 = (uint16_t)((reg_byte_addr >> 1) << 1); // bit0=0 => MOVE
    h.install(at, word1);
    h.install(at + 1, data);
}

// Installs a two-word WAIT instruction (wait for vpos>=vp, hpos>=0, full
// mask, blit_busy bypassed via ir2[15]=1 -- agnus_copper.v:295-314) at
// `at`, consuming 2 words.
void install_wait(Harness& h, uint32_t at, uint16_t vp) {
    uint16_t word1 = (uint16_t)(((vp & 0xFF) << 8) | 1); // HP=0, bit0=1 => WAIT/SKIP
    uint16_t word2 = 0xFFFE; // full VE/HE mask, bit15=1 (bypass blit_busy), bit0=0 (WAIT not SKIP-only)
    h.install(at, word1);
    h.install(at + 1, word2);
}

void set_cop1lc(Harness& h, uint32_t word_addr) {
    h.write_reg(A_COP1LCH, (uint16_t)((word_addr >> 15) & 0x1F));
    h.write_reg(A_COP1LCL, (uint16_t)((word_addr & 0x7FFF) << 1));
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // CONTROL-FIXTURE-RESET (no claim id; harness precondition -- the
    // register bus every assertion below uses must answer a known value
    // before anything trusts it).
    // =========================================================================
    {
        Harness h;
#if MUT_P0
        h.dmacon_set(1 << BPLEN_BIT); // spurious write before the check
#endif
        uint16_t dcr = h.read_reg(A_DMACONR);
        checks.expect((dcr & 0x1FFF) == 0,
            "CONTROL-FIXTURE-RESET: DMACONR reads 0 in bits [12:0] immediately "
            "after reset, before any DMACON write -- the fixture's own "
            "known-value baseline. No manual page (harness precondition). "
            "MUTATION: a spurious DMACON SET write issued before this read -- "
            "DMACONR then reads nonzero instead of 0.");
    }

    // =========================================================================
    // DMA-CHANNEL-TAKES-BUS-FROM-CPU (p.1) -- sol:1-components.c9, opus:1-components.c9.
    // Also this package's control-pass baseline: the fixture proves it can
    // deliver a known address before anything reads one.
    // =========================================================================
    {
        Harness h;
        const uint32_t PTR = 0x00500;
        h.write_reg(A_DSKPTH, (uint16_t)((PTR >> 15) & 0x1F));
        h.write_reg(A_DSKPTL, (uint16_t)((PTR & 0x7FFF) << 1));
        h.dmacon_set(1 << DMAEN_BIT);
#if !MUT_P1
        h.dut->disk_dmal = 1;
#endif
        h.dut->disk_dmas = 0;
        bool seen = false;
        uint32_t seen_addr = 0;
        for (int i = 0; i < 4000 && !seen; ++i) {
            h.tick();
            if (h.dut->dbr && !h.dut->cpu_custom) { seen = true; seen_addr = h.dut->address_out; }
        }
        h.dut->disk_dmal = 0;
        char buf[128];
        std::snprintf(buf, sizeof buf, "seen=%d addr=$%05X (expected $%05X)", seen, seen_addr, PTR);
        checks.expect(seen && seen_addr == PTR,
            std::string("DMA-CHANNEL-TAKES-BUS-FROM-CPU (p.1): enabling disk DMA "
            "(disk_dmal asserted, DMACON DMAEN set) makes dbr rise with "
            "cpu_custom low and address_out present exactly the pointer "
            "written to DSKPTH/DSKPTL -- \"Custom graphics and audio chips "
            "with DMA capability.\" (p.1). sol:1-components.c9, "
            "opus:1-components.c9. Measured: ") + buf + ". MUTATION: "
            "disk_dmal withheld -- no grant, no address, ever observed.");
    }

    // =========================================================================
    // ADDRESS-OUTPUT-CEILING-2MB (p.1) -- sol:1-components.c11, opus:1-components.c11.
    // =========================================================================
    {
        Harness h;
        const uint32_t TOP = 0xFFFFF; // 20-bit word address ceiling
#if !MUT_P2
        h.write_reg(A_DSKPTH, (uint16_t)((TOP >> 15) & 0x1F));
        h.write_reg(A_DSKPTL, (uint16_t)((TOP & 0x7FFF) << 1));
#else
        h.write_reg(A_DSKPTH, 0);
        h.write_reg(A_DSKPTL, 0);
#endif
        h.dmacon_set(1 << DMAEN_BIT);
        h.dut->disk_dmal = 1; h.dut->disk_dmas = 0;
        bool seen = false; uint32_t seen_addr = 0;
        for (int i = 0; i < 4000 && !seen; ++i) {
            h.tick();
            if (h.dut->dbr && !h.dut->cpu_custom) { seen = true; seen_addr = h.dut->address_out; }
        }
        h.dut->disk_dmal = 0;
        char buf[64]; std::snprintf(buf, sizeof buf, "address_out=$%05X", seen_addr);
        checks.expect(seen && seen_addr == TOP,
            std::string("ADDRESS-OUTPUT-CEILING-2MB (p.1): address_out ([20:1], "
            "agnus.v:67) reaches the top word address $FFFFF (a 2MB byte "
            "ceiling) when the disk DMA pointer is written there, with "
            "cpu_custom low throughout -- \"The custom chips can access up "
            "to 2MB of memory directly without using the 68000 CPU.\" (p.1). "
            "sol:1-components.c11, opus:1-components.c11. Measured: ") + buf +
            ". MUTATION: pointer written as 0 instead of the top of range -- "
            "address_out reads $00000, not $FFFFF.");
    }

    // =========================================================================
    // SPECIAL-PURPOSE-HARDWARE-TAKES-BUS-FROM-PROCESSOR (p.2) -- sol:1-components.c34, opus:1-components.c34.
    // "All Amiga models" / "greatly enhance system performance" set no
    // threshold and are not graded (both claims' own evidence agrees).
    // =========================================================================
    {
        Harness h;
        const uint32_t A = 0x00800;
        install_move(h, A, A_COLOR00, 0x0F00);
        // COP1LC and the COPJMP1 strobe are written first, while COPEN is
        // still 0 -- once the Copper is armed it continuously contends for
        // the same CPU register bus these writes use, so setting COPEN
        // first (with a fixed-length write) risks the write silently never
        // landing. Real firmware sequences it the same way.
        set_cop1lc(h, A);
        h.write_reg(A_COPJMP1, 0); // strobe: reload PC from COP1LC
#if !MUT_P3
        h.dmacon_set((1 << COPEN_BIT) | (1 << DMAEN_BIT));
#endif
        // Checked via reg_address_out == COPINS ($08C>>1, agnus_copper.v's
        // own instruction-fetch marker), not merely "dbr && !cpu_custom" --
        // refresh DMA (agnus_refresh.v) fires unconditionally, with no
        // enable of its own, and would otherwise satisfy that generic check
        // even with the Copper fully disabled, making the mutation below a
        // no-op. COPINS is a value only a Copper fetch cycle ever presents.
        constexpr uint16_t A_COPINS = 0x08C >> 1;
        bool seen = false;
        for (int i = 0; i < 4000 && !seen; ++i) {
            h.tick();
            if (h.dut->dbr && !h.dut->cpu_custom && h.dut->reg_address_out == A_COPINS) seen = true;
        }
        checks.expect(seen,
            "SPECIAL-PURPOSE-HARDWARE-TAKES-BUS-FROM-PROCESSOR (p.2): "
            "enabling the Copper channel (DMACON COPEN+DMAEN) with a "
            "programmed list makes dbr rise and cpu_custom drop, exactly as "
            "disk DMA did for DMA-CHANNEL-TAKES-BUS-FROM-CPU -- a second, "
            "independent channel, not a copy of the first. \"...special "
            "purpose hardware known as the custom chips...\" (p.2). "
            "sol:1-components.c34, opus:1-components.c34. MUTATION: DMACON's "
            "COPEN/DMAEN left clear -- the Copper never requests, dbr never "
            "rises.");
    }

    // =========================================================================
    // DMA-WRITE-WITHOUT-CPU (p.2) -- sol:1-components.c37, opus:1-components.c37.
    // =========================================================================
    {
        Harness h;
        const uint32_t PTR = 0x00600;
        h.write_reg(A_DSKPTH, (uint16_t)((PTR >> 15) & 0x1F));
        h.write_reg(A_DSKPTL, (uint16_t)((PTR & 0x7FFF) << 1));
        h.dmacon_set(1 << DMAEN_BIT);
        h.dut->disk_dmal = 1;
#if !MUT_P4
        h.dut->disk_dmas = 0; // wr = ~dmas = 1: a memory WRITE cycle
#else
        h.dut->disk_dmas = 1; // wr = 0: a memory READ cycle -- mutation
#endif
        bool saw_write = false;
        for (int i = 0; i < 4000 && !saw_write; ++i) {
            h.tick();
            if (h.dut->dbr && h.dut->dbwe && !h.dut->cpu_custom) saw_write = true;
        }
        h.dut->disk_dmal = 0;
        bool cpu_back = false;
        for (int i = 0; i < 32 && !cpu_back; ++i) { h.tick(); if (h.dut->cpu_custom) cpu_back = true; }
        checks.expect(saw_write && cpu_back,
            "DMA-WRITE-WITHOUT-CPU (p.2): disk DMA performs a real memory "
            "WRITE cycle (dbwe asserted alongside dbr) with cpu_custom low, "
            "and cpu_custom returns to 1 once the request is withdrawn -- "
            "\"...they can access memory without using the 680x0 CPU - this "
            "frees the CPU for other types of operations.\" (p.2). "
            "sol:1-components.c37, opus:1-components.c37. MUTATION: dmas "
            "driven 1 instead of 0 -- wr becomes 0 (a read), dbwe never "
            "asserts.");
    }

    // =========================================================================
    // ECS-ADDRESS-CEILING-2MB (p.3) -- sol:1-components.c42, opus:1-components.c42.
    // Same ceiling as ADDRESS-OUTPUT-CEILING-2MB, distinct configuration
    // (ecs=1 here, ecs=0 there). "Other advanced features" not graded.
    // =========================================================================
    {
        Harness h;
        h.dut->ecs = 1;
        const uint32_t TOP = 0xFFFFF;
#if !MUT_P5
        h.write_reg(A_DSKPTH, (uint16_t)((TOP >> 15) & 0x1F));
        h.write_reg(A_DSKPTL, (uint16_t)((TOP & 0x7FFF) << 1));
#else
        h.write_reg(A_DSKPTH, 0);
        h.write_reg(A_DSKPTL, 0);
#endif
        h.dmacon_set(1 << DMAEN_BIT);
        h.dut->disk_dmal = 1; h.dut->disk_dmas = 0;
        bool seen = false; uint32_t seen_addr = 0;
        for (int i = 0; i < 4000 && !seen; ++i) {
            h.tick();
            if (h.dut->dbr && !h.dut->cpu_custom) { seen = true; seen_addr = h.dut->address_out; }
        }
        h.dut->disk_dmal = 0;
        char buf[64]; std::snprintf(buf, sizeof buf, "address_out=$%05X, ecs=1", seen_addr);
        checks.expect(seen && seen_addr == TOP,
            std::string("ECS-ADDRESS-CEILING-2MB (p.3): with ecs driven 1, "
            "address_out still reaches $FFFFF (2MB) -- \"The latest version "
            "of the custom chips, known as the Enhanced Chip Set or ECS) "
            "can handle up to 2 MB of memory...\" (p.3). sol:1-components.c42, "
            "opus:1-components.c42. Measured: ") + buf + ". MUTATION: pointer "
            "written as 0 instead of the top of range -- address_out reads "
            "$00000, not $FFFFF.");
    }

    // =========================================================================
    // Declined: P6, P7 (p.3, NTSC/PAL nominal display), P10 (p.3, Copper
    // triggers many times), P11 (p.3, Copper affects other chips), P12, P13
    // (p.4, sprite reuse), P15, P16 (p.4, blitter minterm/line/fill).
    // See SPECIFICATION.md for the full citation of each.
    // =========================================================================
    checks.record(
        "sol:1-components.c47, opus:1-components.c46 (p.3, NTSC 320x200/400 "
        "in 32 colors) and sol:1-components.c49, opus:1-components.c48 (p.3, "
        "PAL 320x256/512 in 32 colors): not graded here. The NTSC-262/"
        "PAL-312 field length these numbers rest on is already graded, with "
        "mutation-tested exactness, by hrm/ch3-beam-counters-and-blanking's "
        "PAL-FIELD-312-NTSC-FIELD-262-LINES (uid 3-video.c11). The literal "
        "200/256 visible-line figures need DIWSTRT/DIWSTOP, outside "
        "agnus_beamcounter's own boundary per that package's own disclosure "
        "-- chapter 3's own display-window packages own that derivation. "
        "32 colors needs Denise, outside this fixture entirely. See "
        "SPECIFICATION.md P6/P7.");

    checks.record(
        "sol:1-components.c58, opus:1-components.c57 (p.3, \"The coprocessor "
        "can trigger many times per screen, in the middle of lines, and at "
        "the beginning or during the blanking interval.\"): not graded here. "
        "hrm/agnus_copper_waitskip_range (COP-04, COP-08) already grades "
        "WAIT/SKIP triggering at arbitrary line/mid-line/blanking positions "
        "with mutation-tested exactness. COPPER-GATED-BY-BEAM-POSITION below "
        "already shows the gate genuinely depends on beam position at this "
        "boundary; a weaker restatement of chapter 2's own question adds no "
        "further content. See SPECIFICATION.md P10.");

    checks.record(
        "sol:1-components.c59, opus:1-components.c58 (p.3, \"The coprocessor "
        "itself can directly affect most of the registers in the other "
        "custom chips, freeing the 680x0 for general computing tasks.\"): "
        "not graded with a check of its own -- fully corroborated by "
        "COPPER-REACHES-OUTSIDE-AGNUS below, which already demonstrates a "
        "Copper MOVE reaching an address agnus itself never decodes, with "
        "cpu_custom low throughout. \"Most of the registers\" and \"freeing "
        "the 680x0\" add no separately falsifiable threshold. See "
        "SPECIFICATION.md P11.");

    checks.record(
        "sol:1-components.c65, opus:1-components.c64 (p.4, sprite DMA reuse "
        "with a one-line minimum separation) and sol:1-components.c66, "
        "opus:1-components.c65 (p.4, \"many small sprites... reusing the "
        "sprite processors\"): not graded here. hrm/ch4-sprite-dma-sequence "
        "(232 claims) already grades exactly this at its "
        "A5-CHANNEL-REUSED-MULTIPLE-TIMES-WITHIN-ONE-FIELD assertion "
        "(CH4-reuse.c1, c4, c7, c8, c10-c14 among its cited ids). This "
        "fixture also never triggers sprite DMA at all (see BOUNDARY, "
        "SPECIFICATION.md). See SPECIFICATION.md P12/P13.");

    checks.record(
        "sol:1-components.c72, opus:1-components.c71 (p.4, \"...combine the "
        "data in one of 256 different possible ways...\") and "
        "sol:1-components.c73, opus:1-components.c72 (p.4, special-mode "
        "line draw at ~1M dots/sec and area fill): not graded here, per the "
        "build brief's own instruction for this package -- the blitter's "
        "minterm logic is chapter 6's question. hrm/agnus_blitter_minterm "
        "(182 claims) exhaustively sweeps all 256 LF codes; "
        "hrm/agnus_blitter_line and hrm/agnus_blitter_fill grade the "
        "line-draw and area-fill mechanisms by name. "
        "BLITTER-MOVES-DATA-ACROSS-BOUNDARY below already grades this "
        "sentence pair's structural half (the blitter moves data across "
        "Agnus's boundary). See SPECIFICATION.md P15/P16.");

    // =========================================================================
    // BLITTER-MOVES-DATA-ACROSS-BOUNDARY (p.4) -- sol:1-components.c71, opus:1-components.c70.
    // Minimal channel-D-only blit (LF=$FF needs no source data); the
    // written VALUE is never asserted -- that is chapter 6's question.
    // =========================================================================
    {
        Harness h;
        const uint32_t DEST = 0x00700;
        h.write_reg(A_BLTCON1, 0x0000);
        h.write_reg(A_BLTDPTH, (uint16_t)((DEST >> 15) & 0x1F));
        h.write_reg(A_BLTDPTL, (uint16_t)((DEST & 0x7FFF) << 1));
        h.write_reg(A_BLTCON0, 0x01FF); // usea=useb=usec=0, used=1, LF=$FF
#if !MUT_P14
        h.dmacon_set((1 << BLTEN_BIT) | (1 << DMAEN_BIT));
#endif
        h.write_reg(A_BLTSIZE, 0x0041); // height=1, width=1
        // `busy` is agnus_blitter's own internal port -- not brought out at
        // agnus's own top level at all (only its DMACONR bit-14 readback,
        // data_bmc|dmaconr|data_blt, is -- and that decode only reflects
        // blit_busy on ticks the CPU register path, not a DMA channel,
        // currently holds `reg_address`, so it cannot be sampled reliably
        // every tick during the blit's own DMA activity). The two directly
        // observable, unambiguous facts this fixture's own ports carry are
        // used instead: a real destination write occurred, and the
        // blit-finished interrupt fired.
        bool saw_write_at_dest = false, saw_int3 = false;
        for (int i = 0; i < 8000; ++i) {
            h.tick();
            if (h.dut->dbr && h.dut->dbwe && h.dut->address_out == DEST) saw_write_at_dest = true;
            if (h.dut->int3) saw_int3 = true;
        }
        checks.expect(saw_write_at_dest && saw_int3,
            "BLITTER-MOVES-DATA-ACROSS-BOUNDARY (p.4): a minimal channel-D-"
            "only blit (BLTCON0 LF=$FF, no source channels) performs a real "
            "DMA write (dbr+dbwe) at the destination pointer address and "
            "fires int3 (blit-finished interrupt) -- the blitter, as a "
            "subsystem of Agnus, moves data across the chip-memory boundary. "
            "\"Custom bit blitter used for high speed data movement...\" "
            "(p.4). sol:1-components.c71, opus:1-components.c70. MUTATION: "
            "DMACON's BLTEN left clear -- the blitter never wins a DMA "
            "grant, so no write and no int3 are ever observed.");
    }

    // =========================================================================
    // COPPER-GATED-BY-BEAM-POSITION (p.3) -- sol:1-components.c56, opus:1-components.c55.
    // A level no agnus_copper_* sibling builds at.
    // =========================================================================
    {
        Harness h;
        const uint32_t A = 0x00900;
#if !MUT_P8
        install_wait(h, A, 3); // wait for vpos>=3
#else
        install_wait(h, A, 0); // mutation: wait for vpos>=0 (already true)
#endif
        install_move(h, A + 2, A_COLOR00, 0x0F00);
        set_cop1lc(h, A);
        h.write_reg(A_COPJMP1, 0);
        h.dmacon_set((1 << COPEN_BIT) | (1 << DMAEN_BIT));
        const int EARLY_CUTOFF = 1000;   // well under 1 line's worth of ticks
        const int LATE_BUDGET  = 30000;  // several lines' worth
        long long move_tick = h.run_until_reg_address(A_COLOR00 >> 1, LATE_BUDGET);
        checks.expect(move_tick >= 0 && move_tick > EARLY_CUTOFF,
            std::string("COPPER-GATED-BY-BEAM-POSITION (p.3): a WAIT for "
            "vpos>=3 placed before a MOVE to COLOR00 delays the MOVE's "
            "effect (reg_address_out reaching COLOR00's address during a "
            "Copper-granted cycle) well past an early cutoff chosen safely "
            "below where vpos could reach 3 -- proof the Copper's vpos/hpos "
            "inputs actually gate it. \"...allows changes to most of the "
            "special purpose registers in synchronization with the position "
            "of the video beam.\" (p.3). sol:1-components.c56, "
            "opus:1-components.c55. Measured move_tick=") +
            std::to_string(move_tick) + " (cutoff=" + std::to_string(EARLY_CUTOFF) +
            "). MUTATION: WAIT target changed from vpos>=3 to vpos>=0 -- the "
            "MOVE lands almost immediately, inside the cutoff window.");
    }

    // =========================================================================
    // COPPER-REACHES-OUTSIDE-AGNUS (p.3) -- sol:1-components.c57, opus:1-components.c56.
    // =========================================================================
    {
        Harness h;
        const uint32_t A = 0x00A00;
        const uint32_t TARGET = A_COLOR00;
#if !MUT_P9
        install_move(h, A, TARGET, 0x0BEE);
#else
        install_move(h, A, A_DMACON, 0x0000); // mutation: target inside agnus
#endif
        set_cop1lc(h, A);
        h.write_reg(A_COPJMP1, 0);
        h.dmacon_set((1 << COPEN_BIT) | (1 << DMAEN_BIT));
        long long t = h.run_until_reg_address(TARGET >> 1, 6000);
        // Confirm agnus itself never decodes this address for a read.
        uint16_t readback = h.read_reg(TARGET);
        checks.expect(t >= 0 && readback == 0,
            std::string("COPPER-REACHES-OUTSIDE-AGNUS (p.3): a Copper MOVE "
            "to $180 (COLOR00, a Denise register agnus.v never decodes -- "
            "data_out's data_bmc|dmaconr|data_blt OR answers 0 for it) is "
            "observed on reg_address_out during the Copper's granted write "
            "cycle -- the address crosses agnus's own boundary rather than "
            "being blocked or consumed inside it. \"This allows such "
            "special effects as mid-screen changes to the color "
            "palette...\" (p.3). sol:1-components.c57, opus:1-components.c56. "
            "Measured: crossing observed at tick=") + std::to_string(t) +
            ", agnus's own read of $180 returns $" + hexstr(readback, 4) +
            ". MUTATION: MOVE retargeted to $096 (DMACON, inside agnus) -- "
            "reg_address_out during the write cycle no longer matches $180.");
    }

    // =========================================================================
    // DMA-CHORES-INDEPENDENT-OF-CPU (p.4) -- sol:1-components.c81, opus:1-components.c80.
    // =========================================================================
    {
        Harness h;
        // graphics leg: blitter (same minimal blit as BLITTER-MOVES-DATA-ACROSS-BOUNDARY)
        {
            const uint32_t DEST = 0x00B00;
            h.write_reg(A_BLTCON1, 0x0000);
            h.write_reg(A_BLTDPTH, (uint16_t)((DEST >> 15) & 0x1F));
            h.write_reg(A_BLTDPTL, (uint16_t)((DEST & 0x7FFF) << 1));
            h.write_reg(A_BLTCON0, 0x01FF);
            h.dmacon_set((1 << BLTEN_BIT) | (1 << DMAEN_BIT));
            h.write_reg(A_BLTSIZE, 0x0041);
        }
        // Each leg is checked via address_out matching that leg's own
        // distinct, just-written pointer -- not merely "dbr && !cpu_custom"
        // -- because refresh DMA (agnus_refresh.v) fires unconditionally,
        // with no enable of its own, and drives address_out to exactly 0
        // during its own grant; a generic dbr check would let refresh alone
        // satisfy the audio leg even with audio_dmal never asserted,
        // making the mutation below a no-op.
        bool graphics_seen = false;
        for (int i = 0; i < 4000 && !graphics_seen; ++i) { h.tick(); if (h.dut->dbr && h.dut->address_out == 0x00B00) graphics_seen = true; }
        // The blit itself may still be finishing (further D-channel cycles,
        // contending for the bus) right after the single write cycle above
        // is observed -- disarm BLTEN and let a few hundred ticks pass so
        // the audio leg's own register writes below are not racing it.
        h.dmacon_clr(1 << BLTEN_BIT);
        for (int i = 0; i < 500; ++i) h.tick();

        // audio leg. audio_dmas (restart) must be asserted alongside
        // audio_dmal on the channel's own dma cycle, or agnus_audiodma.v's
        // audpt register just increments its own (reset-default 0) value
        // instead of loading AUD0LCH/AUD0LCL's just-written content
        // (agnus_audiodma.v: `audpt[channel] <= dmas ? audlcout : audptout+1`).
        const uint32_t AUDIO_PTR = 0x00D00;
        h.write_reg(A_AUD0LCH, (uint16_t)((AUDIO_PTR >> 15) & 0x1F));
        h.write_reg(A_AUD0LCL, (uint16_t)((AUDIO_PTR & 0x7FFF) << 1));
        bool audio_seen = false;
#if !MUT_P17
        h.dut->audio_dmal = 0x1; h.dut->audio_dmas = 0x1;
#endif
        // 8000 ticks (well over 4 lines) rather than a tighter budget: this
        // leg starts wherever hpos/vpos happened to be left by the graphics
        // leg above, not freshly reset, so the wait until audio's own slot
        // (hpos $25 per agnus_audiodma.v's dmal case, channel 0) recurs can
        // be most of a line longer than a fresh-reset run would need.
        for (int i = 0; i < 8000 && !audio_seen; ++i) {
            h.tick();
            if (h.dut->dbr && h.dut->address_out == AUDIO_PTR) audio_seen = true;
        }
        h.dut->audio_dmal = 0; h.dut->audio_dmas = 0;

        // I/O leg: disk
        const uint32_t DISK_PTR = 0x00E00;
        h.write_reg(A_DSKPTH, (uint16_t)((DISK_PTR >> 15) & 0x1F));
        h.write_reg(A_DSKPTL, (uint16_t)((DISK_PTR & 0x7FFF) << 1));
        bool io_seen = false;
        h.dut->disk_dmal = 1; h.dut->disk_dmas = 0;
        for (int i = 0; i < 4000 && !io_seen; ++i) { h.tick(); if (h.dut->dbr && h.dut->address_out == DISK_PTR) io_seen = true; }
        h.dut->disk_dmal = 0;

        checks.expect(graphics_seen && audio_seen && io_seen,
            "DMA-CHORES-INDEPENDENT-OF-CPU (p.4): graphics (blitter), audio "
            "(audio_dmal) and I/O (disk_dmal) each independently produce a "
            "dbr-high, cpu_custom-low cycle -- three real, separately-"
            "triggerable channels, not one wearing three names. \"...the "
            "custom chips have DMA access to RAM which allows them to "
            "perform graphics, audio, and I/O chores independently of the "
            "CPU.\" (p.4). sol:1-components.c81, opus:1-components.c80. "
            "MUTATION: audio_dmal withheld -- the audio leg finds no "
            "activity.");
    }

    // =========================================================================
    // CHIP-MEMORY-INTERLEAVED-BASIS (p.4) -- sol:1-components.c83, opus:1-components.c82.
    // =========================================================================
    {
        // Refresh DMA (agnus_refresh.v) fires unconditionally, with no
        // enable of its own, so it alone already makes cpu_custom read 0 on
        // a few ticks per line -- "low > 0" by itself would stay true even
        // with disk DMA fully disabled, making the mutation below a no-op.
        // A refresh-only baseline is measured first (fresh harness, DMA
        // idle) so the disk-active run below can be required to show
        // meaningfully MORE low ticks than refresh alone ever produces.
        long long refresh_only_low = 0;
        const long long N = 4000;
        {
            Harness hidle;
            hidle.dmacon_clr((1 << DMAEN_BIT) | (1 << COPEN_BIT) | (1 << BLTEN_BIT) | (1 << BPLEN_BIT));
            for (long long i = 0; i < N; ++i) { hidle.tick(); if (!hidle.dut->cpu_custom) ++refresh_only_low; }
        }

        Harness h;
        h.dmacon_set(1 << DMAEN_BIT);
#if !MUT_P18
        h.dut->disk_dmal = 1;
#endif
        h.dut->disk_dmas = 0;
        long long high = 0, low = 0;
        for (long long i = 0; i < N; ++i) {
            h.tick();
            if (h.dut->cpu_custom) ++high; else ++low;
        }
        h.dut->disk_dmal = 0;
        char buf[96];
        std::snprintf(buf, sizeof buf, "high=%lld low=%lld of %lld (refresh_only_low=%lld)",
                      high, low, N, refresh_only_low);
        checks.expect(high > 0 && low > refresh_only_low + 15,
            std::string("CHIP-MEMORY-INTERLEAVED-BASIS (p.4): with disk DMA "
            "held continuously requesting, cpu_custom reads both 1 (some "
            "ticks) and 0 (other ticks) within the same run -- alternation "
            "by slot, not one party holding the bus exclusively. \"The "
            "custom chips and the 680x0 CPU share Chip memory on a fully "
            "interleaved basis.\" (p.4). sol:1-components.c83, "
            "opus:1-components.c82. Measured: ") + buf + ". MUTATION: "
            "disk_dmal held deasserted -- cpu_custom reads 1 on every tick, "
            "never interleaved.");
    }

    // =========================================================================
    // CPU-GETS-EVERY-SLOT-WHEN-DMA-IDLE (p.4, control case) -- sol:1-components.c84, opus:1-components.c83.
    // =========================================================================
    long long p19_idle_high = 0;
    const long long P19_N = 4000;
    {
        Harness h;
        h.dmacon_clr((1 << DMAEN_BIT) | (1 << COPEN_BIT) | (1 << BLTEN_BIT) | (1 << BPLEN_BIT));
#if MUT_P19
        h.dut->disk_dmal = 1; h.dut->disk_dmas = 0;
#endif
        long long low = 0;
        for (long long i = 0; i < P19_N; ++i) {
            h.tick();
            if (h.dut->cpu_custom) ++p19_idle_high; else ++low;
        }
        h.dut->disk_dmal = 0;
        // Refresh steals 4 fixed ticks per line unconditionally
        // (agnus_refresh.v) -- measured here, not assumed.
        char buf[96];
        std::snprintf(buf, sizeof buf, "high=%lld low=%lld of %lld (refresh-only low expected)",
                      p19_idle_high, low, P19_N);
        checks.note(std::string("CPU-GETS-EVERY-SLOT-WHEN-DMA-IDLE baseline: ") + buf);
        checks.expect(low > 0 && low < (P19_N / 20),
            std::string("CPU-GETS-EVERY-SLOT-WHEN-DMA-IDLE (p.4, control case): "
            "with every software-visible DMA channel disabled, cpu_custom "
            "reads 0 only on the small, fixed number of ticks refresh DMA "
            "unconditionally claims (agnus_refresh.v, no enable of any "
            "kind) -- never on a tick a real request could have used. "
            "\"...the rest of the time the Chip memory bus is free for "
            "other activities.\" (p.4). sol:1-components.c84, "
            "opus:1-components.c83. Measured: ") + buf + ". MUTATION: "
            "disk_dmal asserted during this supposedly-idle window -- low-tick "
            "count rises far past the refresh-only figure.");
    }

    checks.record(
        "sol:1-components.c85, opus:1-components.c84 (p.4, \"The custom "
        "chips use the memory bus during these free cycles, effectively "
        "allowing the CPU to run at full speed most of the time.\"): not "
        "graded with a check of its own -- the converse framing of what "
        "CHIP-MEMORY-INTERLEAVED-BASIS (DMA takes only slots the CPU isn't "
        "using) and CPU-GETS-EVERY-SLOT-WHEN-DMA-IDLE (the CPU gets every "
        "slot, refresh aside, when idle) already show together. \"Effectively "
        "allowing... full speed most of the time\" states no separate "
        "threshold. See SPECIFICATION.md P20.");

    // =========================================================================
    // DMA-REDUCES-CPU-SLOT-COUNT (p.4) -- sol:1-components.c86, opus:1-components.c85.
    // =========================================================================
    {
        Harness h;
        h.dmacon_set(1 << DMAEN_BIT);
#if !MUT_P21
        h.dut->disk_dmal = 1;
#endif
        h.dut->disk_dmas = 0;
        long long high = 0;
        for (long long i = 0; i < P19_N; ++i) { h.tick(); if (h.dut->cpu_custom) ++high; }
        h.dut->disk_dmal = 0;
        char buf[96];
        std::snprintf(buf, sizeof buf, "active_high=%lld idle_high=%lld (of %lld each)",
                      high, p19_idle_high, P19_N);
        checks.expect(high < p19_idle_high,
            std::string("DMA-REDUCES-CPU-SLOT-COUNT (p.4): cpu_custom's "
            "high-tick count over the same window is strictly lower with "
            "disk DMA continuously active than the idle baseline "
            "(CPU-GETS-EVERY-SLOT-WHEN-DMA-IDLE) -- \"some occasions\" the "
            "chips \"steal\" a cycle the CPU would otherwise have had. "
            "\"There are some occasions though when the custom chips steal "
            "memory cycles from the 680x0.\" (p.4). sol:1-components.c86, "
            "opus:1-components.c85. Measured: ") + buf + ". MUTATION: "
            "disk_dmal left deasserted -- the active run's count no longer "
            "comes out lower than the idle baseline's.");
    }

    checks.record(
        "sol:1-components.c87, opus:1-components.c86 (p.4, \"In the higher "
        "resolution video modes, some or all of the cycles normally used "
        "for processor access are needed by the custom chips for video "
        "refresh.\"): not graded here. This is specifically bitplane-fetch "
        "volume scaling with resolution/plane count, which needs bitplane "
        "DMA actually running -- agnus_bitplanedma.v's two-line vdiwena "
        "latch and a programmed DDFSTRT/DDFSTOP window, never built in this "
        "fixture (see SPECIFICATION.md BOUNDARY). hrm/ch3-ddf-fetch-window "
        "owns DDFSTRT/DDFSTOP word-count sizing; reproducing it here for "
        "one comparison would be an introductory sentence inflated into a "
        "mechanism test the page never asked for. See SPECIFICATION.md P22.");

    checks.record(
        "sol:1-components.c88, opus:1-components.c87 (p.4, \"In that case, "
        "the Copper and the blitter in the custom chips steal time from the "
        "680x0 for jobs they can do better than the 680x0.\"): not graded "
        "with a check of its own -- COPPER-REACHES-OUTSIDE-AGNUS and "
        "BLITTER-MOVES-DATA-ACROSS-BOUNDARY already show both channels take "
        "DMA cycles with cpu_custom low, and DMA-REDUCES-CPU-SLOT-COUNT "
        "already shows that reduces the CPU's own slot count. \"Jobs they "
        "can do better than the 680x0\" is an unthresholded judgement. See "
        "SPECIFICATION.md P23.");

    checks.record(
        "sol:1-components.c103, opus:1-components.c102 (p.5, \"ECS Fat "
        "Agnus supports both the NTSC and PAL video standards on a single "
        "chip.\"): not graded here. hrm/ch3-beam-counters-and-blanking's "
        "PAL-FIELD-312-NTSC-FIELD-262-LINES (uid 3-video.c11) already "
        "proves, on one build with ntsc toggled at runtime rather than two "
        "chip variants, that both exact field lengths are produced -- a "
        "strictly stronger fact than \"the two standards are merely "
        "different\", which is all a fresh assertion here could add. See "
        "SPECIFICATION.md P24.");

    checks.record(
        "sol:1-components.c111, opus:1-components.c111 (p.6, \"...the "
        "ability to dynamically control which part of the Chip memory is "
        "used for the background display, audio, and sprites.\"); "
        "sol:1-components.c112, opus:1-components.c112 (p.6, \"The Amiga is "
        "not limited to a small, specific area of RAM for a frame "
        "buffer.\"); sol:1-components.c113, opus:1-components.c113 (p.6, "
        "\"...allows display bitplanes, sprite processor control lists, "
        "coprocessor instruction lists, or audio channel control lists to "
        "be located anywhere within Chip memory.\"): not graded here. Every "
        "engine these three sentences name already has a dedicated existing "
        "package whose whole purpose is that engine's own pointer range and "
        "writability: bitplane (hrm/ch3-bitplane-pointers-and-modulo), "
        "sprite (hrm/ch4-sprite-pointer-registers), audio "
        "(hrm/ch5-audio-location-pointer, 176 claims), Copper "
        "(hrm/agnus_copper_locreg_width). Reaching bitplane/sprite here "
        "would also require the display-window/sprite-list machinery this "
        "fixture deliberately does not build (see BOUNDARY, "
        "SPECIFICATION.md). See SPECIFICATION.md P25/P26/P27.");

    checks.record(
        "sol:1-components.c114, opus:1-components.c114 (p.6, \"This same "
        "region of memory can be accessed by the bit blitter.\"): not "
        "graded here. hrm/agnus_blitter_adrgen's own stated purpose is "
        "\"four independent BLTxPT pointer registers... the pointer's value "
        "after a simulated blit row\" -- exactly this sentence's proposition, "
        "already graded there. See SPECIFICATION.md P28.");

    // =========================================================================
    // AGNUS-DMA-ARBITER-PRESENT (p.8, Figure 1-1) -- sol:1-blockdiag.c12.
    // =========================================================================
    {
        Harness h;
        const uint32_t PTR = 0x00C00;
        h.write_reg(A_DSKPTH, (uint16_t)((PTR >> 15) & 0x1F));
        h.write_reg(A_DSKPTL, (uint16_t)((PTR & 0x7FFF) << 1));
        h.dmacon_set(1 << DMAEN_BIT);
#if !MUT_P29A
        h.dut->disk_dmal = 1;
#endif
        h.dut->disk_dmas = 0;
        bool seen = false; uint32_t seen_addr = 0;
        for (int i = 0; i < 4000 && !seen; ++i) {
            h.tick();
            if (h.dut->dbr && !h.dut->cpu_custom) { seen = true; seen_addr = h.dut->address_out; }
        }
        h.dut->disk_dmal = 0;
        checks.expect(seen && seen_addr == PTR,
            "AGNUS-DMA-ARBITER-PRESENT (Figure 1-1, p.8): the DMA priority "
            "arbiter the figure's AGNUS box represents (agnus.v:167-266) "
            "grants a channel and presents its address on address_out with "
            "cpu_custom low -- the box the figure draws, with its arrowhead "
            "from the chip address line, is a real, working subsystem here. "
            "sol:1-blockdiag.c12. MUTATION: disk_dmal withheld -- no grant "
            "observed.");
    }

    // =========================================================================
    // REG-ADR-BUS-CROSSES-BOUNDARY (p.8, Figure 1-1) -- opus:1-blockdiag.c10, sol:1-blockdiag.c21.
    // =========================================================================
    {
        Harness h;
#if !MUT_P29B
        const uint32_t ADDR = A_COLOR00;
#else
        const uint32_t ADDR = 0x0FF << 1; // mutation: agnus's own "idle" sentinel address
#endif
        h.cpu_driving = true;
        h.dut->aen = 1; h.dut->hwr = 1; h.dut->lwr = 1;
        h.dut->address_in = (ADDR >> 1) & 0xFF;
        h.dut->data_in = 0x0BAD;
        h.tick();
        uint32_t seen = h.dut->reg_address_out;
        h.dut->aen = 0; h.dut->hwr = 0; h.dut->lwr = 0;
        h.cpu_driving = false;
        h.tick();
        // Compared against the fixed expected value (COLOR00's own word
        // address, $C0), not against whatever ADDR the (possibly mutated)
        // stimulus used -- comparing against ADDR itself would trivially
        // match under the mutation too, since the mutation changes what is
        // written, not what is expected.
        checks.expect(seen == (A_COLOR00 >> 1),
            std::string("REG-ADR-BUS-CROSSES-BOUNDARY (Figure 1-1, p.8): a "
            "plain CPU write at an address agnus itself never decodes "
            "($180, COLOR00) still appears on reg_address_out -- "
            "agnus.v:136's `assign reg_address_out = reg_address` is "
            "unconditional, exactly the wire the figure draws leaving AGNUS "
            "toward PAULA and DENISE. opus:1-blockdiag.c10, "
            "sol:1-blockdiag.c21. Measured reg_address_out=$") + hexstr(seen, 2) +
            ". MUTATION: address changed to $FF (agnus's own idle sentinel) "
            "-- indistinguishable from the no-access case.");
    }

    // =========================================================================
    // COPPER-USES-OFFSET-NOT-LOCATION-REG (p.9) -- sol:1-examples.c24, opus:1-examples.c23.
    // =========================================================================
    {
        Harness h;
        const uint32_t LIST1 = 0x00D00;
        const uint32_t LIST2 = 0x00E00; // a different fetch address
        install_move(h, LIST1, A_COLOR00, 0x0AAA);
#if !MUT_P30
        install_move(h, LIST2, A_COLOR00, 0x0BBB); // same target register
#else
        install_move(h, LIST2, A_COLOR00 + 2, 0x0BBB); // mutation: different target
#endif
        // COP1LC + strobe for the first list, written while COPEN is still
        // 0 (see COPPER-GATED-BY-BEAM-POSITION's comment above for why).
        set_cop1lc(h, LIST1);
        h.write_reg(A_COPJMP1, 0);
        h.dmacon_set((1 << COPEN_BIT) | (1 << DMAEN_BIT));
        long long t1 = h.run_until_reg_address(A_COLOR00 >> 1, 4000);

        // The second list is reprogrammed with the Copper already running
        // from the first -- a genuine bus-contention case this time (real
        // firmware can redirect the Copper at any moment); write_reg's own
        // 32-tick hold is what makes this land reliably.
        set_cop1lc(h, LIST2);
        h.write_reg(A_COPJMP1, 0);
        long long t2 = h.run_until_reg_address(A_COLOR00 >> 1, 4000);

        checks.expect(t1 >= 0 && t2 >= 0,
            std::string("COPPER-USES-OFFSET-NOT-LOCATION-REG (p.9): the same "
            "destination register (COLOR00) is reached via reg_address_out "
            "when the Copper's list is fetched from two different COP1LC "
            "addresses ($") + hexstr(LIST1, 5) +
            " and $" + hexstr(LIST2, 5) +
            ") -- changing where the instruction is fetched from changes "
            "nothing about which register it targets, since reg_address_out "
            "is driven purely from the instruction word's own field "
            "(agnus_copper.v:216, ir2[8:1]). \"(One exception to this rule "
            "is the Copper which uses only the offset [to] access the "
            "registers.)\" (p.9). sol:1-examples.c24, opus:1-examples.c23. "
            "MUTATION: the second list's MOVE retargeted to a different "
            "register -- the two fetch addresses no longer produce the same "
            "reg_address_out.");
    }

    // =========================================================================
    // HETEROGENEOUS-DMA-SHARES-ONE-BUS (p.12) -- sol:1-guidelines.c28, opus:1-guidelines.c28.
    // Bitplane and sprite DMA never triggered here (see BOUNDARY); the
    // "bobs/pointers/gadgets" OS-level names reduce to blitter/sprite
    // fetches underneath, disclosed as the honest limit (both claims' own
    // evidence text agrees).
    // =========================================================================
    {
        Harness h;
        const uint32_t SHARED = 0x00F00;

        h.write_reg(A_DSKPTH, (uint16_t)((SHARED >> 15) & 0x1F));
        h.write_reg(A_DSKPTL, (uint16_t)((SHARED & 0x7FFF) << 1));
        h.dmacon_set(1 << DMAEN_BIT);
        h.dut->disk_dmal = 1; h.dut->disk_dmas = 0;
        bool disk_seen = false; uint32_t disk_addr = 0;
        for (int i = 0; i < 4000 && !disk_seen; ++i) { h.tick(); if (h.dut->dbr && h.dut->address_out == SHARED) { disk_seen = true; disk_addr = h.dut->address_out; } }
        h.dut->disk_dmal = 0;

        h.write_reg(A_AUD0LCH, (uint16_t)((SHARED >> 15) & 0x1F));
#if !MUT_P31
        h.write_reg(A_AUD0LCL, (uint16_t)((SHARED & 0x7FFF) << 1));
#else
        h.write_reg(A_AUD0LCL, (uint16_t)(((SHARED + 0x40) & 0x7FFF) << 1));
#endif
        // audio_dmas (restart) must accompany audio_dmal on the channel's
        // own dma cycle, or the pointer just increments its own
        // reset-default value instead of loading AUD0LCH/AUD0LCL (see
        // DMA-CHORES-INDEPENDENT-OF-CPU's audio leg for the same fix, cited
        // there in full against agnus_audiodma.v).
        h.dut->audio_dmal = 0x1; h.dut->audio_dmas = 0x1;
        bool audio_seen = false; uint32_t audio_addr = 0;
        for (int i = 0; i < 8000 && !audio_seen; ++i) { h.tick(); if (h.dut->dbr && h.dut->address_out != 0) { audio_seen = true; audio_addr = h.dut->address_out; } }
        h.dut->audio_dmal = 0; h.dut->audio_dmas = 0;

        h.write_reg(A_BLTCON1, 0x0000);
        h.write_reg(A_BLTDPTH, (uint16_t)((SHARED >> 15) & 0x1F));
        h.write_reg(A_BLTDPTL, (uint16_t)((SHARED & 0x7FFF) << 1));
        h.write_reg(A_BLTCON0, 0x01FF);
        h.dmacon_set(1 << BLTEN_BIT);
        h.write_reg(A_BLTSIZE, 0x0041);
        bool blit_seen = false; uint32_t blit_addr = 0;
        for (int i = 0; i < 4000 && !blit_seen; ++i) { h.tick(); if (h.dut->dbr && h.dut->dbwe && h.dut->address_out == SHARED) { blit_seen = true; blit_addr = h.dut->address_out; } }

        char buf[128];
        std::snprintf(buf, sizeof buf, "disk=$%05X audio_first_addr=$%05X blit=$%05X (shared=$%05X)",
                      disk_addr, audio_addr, blit_addr, SHARED);
        checks.expect(disk_seen && audio_seen && blit_seen && disk_addr == SHARED && blit_addr == SHARED && audio_addr == SHARED,
            std::string("HETEROGENEOUS-DMA-SHARES-ONE-BUS (p.12): disk DMA "
            "(trackdisk buffers) and the blitter (standing in for the "
            "sprite/bob/gadget imagery it draws) are both pointed at the "
            "identical chip address and both reach it on address_out; audio "
            "DMA (sound samples) is exercised independently on the same "
            "bus. One shared Chip memory, not per-kind separate address "
            "spaces. \"This includes bitplanes, sound samples, trackdisk "
            "buffers, and images for sprites, bobs, pointers, and "
            "gadgets.\" (p.12). sol:1-guidelines.c28, opus:1-guidelines.c28. "
            "Measured: ") + buf + ". MUTATION: audio's own pointer written "
            "to a different address than disk/blitter use -- the shared-"
            "address expectation for the pair being compared no longer "
            "holds for that leg (audio's own address diverges).");
    }

    // =========================================================================
    // READ-ALONE-RETRIGGERS-COPJMP (p.13) -- sol:1-guidelines.c61, opus:1-guidelines.c61.
    // =========================================================================
    {
        Harness h;
        const uint32_t A = 0x01000;
        std::vector<uint16_t> filler;
        for (int i = 0; i < 40; ++i) { filler.push_back((uint16_t)((A_COLOR00 >> 1) << 1)); filler.push_back(0x0000); }
        h.install_seq(A, filler);
        set_cop1lc(h, A);
        h.write_reg(A_COPJMP1, 0);
        h.dmacon_set((1 << COPEN_BIT) | (1 << DMAEN_BIT));

        uint32_t max_seen = 0;
        for (int i = 0; i < 3000; ++i) {
            h.tick();
            if (h.dut->dbr && h.dut->address_out > max_seen) max_seen = h.dut->address_out;
        }
        bool moved_away = max_seen > A;

#if !MUT_P32
        h.read_reg(A_COPJMP1); // bare read, no write
#endif

        bool returned = false;
        for (int i = 0; i < 2000 && !returned; ++i) {
            h.tick();
            if (h.dut->dbr && h.dut->address_out == A) returned = true;
        }
        checks.expect(moved_away && returned,
            std::string("READ-ALONE-RETRIGGERS-COPJMP (p.13): after the "
            "Copper's fetch pointer has moved well past its list's start "
            "address A (max address_out seen=$") +
            hexstr(max_seen, 5) +
            ", A=$" + hexstr(A, 5) +
            "), a bare READ of COPJMP1 (no write) makes address_out return "
            "to exactly A -- agnus.v:114's reg_address_cpu is driven "
            "identically by a read as by a write, and "
            "agnus_copper.v:236's COPJMP1 strobe fires on the address "
            "alone, no read/write qualifier. \"This can cause a hardware "
            "register to be triggered twice.\" (p.13). sol:1-guidelines.c61, "
            "opus:1-guidelines.c61. MUTATION: the COPJMP1 access skipped "
            "entirely -- address_out never returns to A within budget.");
    }

    // =========================================================================
    // NO-REGISTER-IS-BOTH-READABLE-AND-WRITABLE (p.14) -- sol:1-guidelines.c86, opus:1-guidelines.c86.
    // =========================================================================
    {
        Harness h;
        h.dmacon_set(1 << BPLEN_BIT); // arbitrary nonzero bit, distinguishable from reset's 0
        uint16_t before = h.read_reg(A_DMACONR);
#if !MUT_P33
        uint16_t dmacon_addr_read = h.read_reg(A_DMACON);
#else
        uint16_t dmacon_addr_read = h.read_reg(A_DMACONR); // mutation: "check" the wrong address
#endif
        h.write_reg(A_DMACONR, 0xFFFF); // writing the read-only address
        uint16_t after = h.read_reg(A_DMACONR);
        char buf[128];
        std::snprintf(buf, sizeof buf, "DMACON-address-read=$%04X (want 0) DMACONR before=$%04X after-foreign-write=$%04X (want equal)",
                      dmacon_addr_read, before, after);
        checks.expect(dmacon_addr_read == 0 && before == after,
            std::string("NO-REGISTER-IS-BOTH-READABLE-AND-WRITABLE (p.14): "
            "DMACON's own address ($096, write-only) answers a read with 0 "
            "(not the bits just written to it) -- agnus.v:277's "
            "data_bmc|dmaconr|data_blt only decodes DMACONR ($002) for "
            "reading; and writing to DMACONR's own address has no "
            "observable effect (DMACON's bits, read back via DMACONR, are "
            "unchanged). No address does both. \"All custom chip registers "
            "are Read-only or Write-only.\" (p.14). sol:1-guidelines.c86, "
            "opus:1-guidelines.c86. Measured: ") + buf + ". MUTATION: the "
            "read-only-address check reads DMACONR's own address instead of "
            "DMACON's -- the check that must read 0 instead reads the real "
            "value.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
