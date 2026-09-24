// nanomig_harness.h
//
// Header-only Verilator harness for nanomig.v (nanomig/src/nanomig.v), the
// NanoMig system top: clock generator, cpu_wrapper (fx68k or tg68k), the
// optional cpu cache, and the minimig-aga chipset. The harness only drives
// and observes the ports of nanomig.v -- it never reads or forces an
// internal signal, and it never places a real Kickstart image. A test
// supplies its own tiny 68000 program, preloaded at the physical RAM
// location the design's overlay maps Kickstart to, and observes the
// design purely through the RAM-port access log, the video/audio capture,
// and the other top-level outputs.
//
// Include this header against the Verilator-generated Vnanomig.h for
// whichever build (fx68k or tg68k) is being exercised; both builds
// instantiate the same nanomig.v top module under --top-module nanomig,
// so the generated class is always named Vnanomig and this header compiles
// unmodified against either object directory.

#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>
#include <algorithm>

#include "Vnanomig.h"
#include "verilated.h"

namespace nmh {

// ---------------------------------------------------------------------
// Address space notes (evidence for the constants used below)
//
// ram_address is declared [23:1] at the nanomig.v top (nanomig.v:82), but
// the sram bridge inside minimig.v only drives 22 bits: its own "address"
// output port is declared [22:1] (minimig_sram_bridge.v:29), and minimig.v
// connects it to its own 23-bit ram_address output (minimig.v:170,724)
// with the extra top bit left undriven by that connection. Neither build
// defines ENABLE_RAM32 (see nanomig/src/tang/nano20k/top.sv and
// top_020.sv), so nothing else drives ram_address[23] either: it reads 0
// on both builds, and the whole external RAM behaves as a 22-bit word
// space (4M words = 8MB), addressed by ram_address[22:1] directly.
//
// Kickstart / overlay mapping, derived from the bank decode:
//   - gary.v:166 sel_kick = (addr[23:19]==5'b11111 && ...) ||
//                            (cpu_rd && ovl && addr[23:19]==5'b00000)
//     so while the overlay is active (ovl==1, the reset default set at
//     minimig.v:861 "if(~_cpu_reset|~_cpu_reset_in) ovl<=1", and only
//     cleared by a CIA-A write at minimig.v:862) every CPU *read* in the
//     logical window $000000-$07FFFF is redirected to the kickstart
//     decode instead of chip RAM.
//   - minimig_bankmapper.v:29 packs sel_kick into bank[7].
//   - minimig_sram_bridge.v:71-75, the bank[7] branch:
//       address[22:18] = {4'b1111, address_in[18]}
//       address[17:1]  = address_in[17:1]                 (pass through)
//     which is exactly logical_address + 0x780000 (byte address), i.e. a
//     linear window: the whole $000000-$07FFFF overlay reads from
//     physical byte offset 0x780000-0x7FFFFF of the external RAM.
//   - Writes to that same $000000-$07FFFF window are NOT redirected: the
//     sel_kick overlay term is ANDed with cpu_rd (gary.v:166), and
//     sel_chip[0] is gated by "!ovl || cpu_hlt" (gary.v:159), so with
//     ovl==1 a write there hits no bank at all (enable==0 in
//     minimig_sram_bridge.v:63-68, so _we/_oe are never asserted). A test
//     program must not write inside the overlay window.
//   - Chip RAM blocks 1-3 (logical $080000-$1FFFFF) are never affected by
//     ovl (gary.v:160-162 sel_chip[1..3] have no ovl term) and, with
//     memory_config selecting 2.0 MB chip RAM (chipram_config==2'b11,
//     minimig_bankmapper.v:34 bank_r[3:0]={chip3,chip2,chip1,chip0}), map
//     physically 1:1 onto logical address (minimig_sram_bridge.v:73 with
//     one-hot bank[3:1] reproduces address_in[20:18] unchanged). That
//     makes $080000+ a safe, ovl-independent read/write data area.
constexpr uint32_t kKickShadowByteBase = 0x780000; // physical byte offset of the overlay window
constexpr uint32_t kKickShadowWordBase = kKickShadowByteBase / 2;
constexpr uint32_t kRamWordCount       = 1u << 22;  // 22-bit word space (8 MiB), see above
constexpr uint32_t kRamWordMask        = kRamWordCount - 1;

// Configuration inputs of nanomig.v (nanomig.v:31-41). Every field here is
// a real top-level port; there is no other way into the design.
struct Config {
    uint8_t memory_config  = 0x03; // [3:2] slow ram, [1:0] chip ram (11 = 2.0 MB, identity-mapped, see above)
    uint8_t fastram_config = 0x00; // 0 = no Zorro II fast RAM offered
    uint8_t cpu_config     = 0x00; // 00 = 68000 core behaviour (nanomig.v:198 comment)
    uint8_t chipset_config = 0x00; // 0 = OCS; COLOR00 stays a 12-bit RGB444 register in this mode
    uint8_t floppy_config  = 0x00; // 0 = no floppy drives; sdc_* stays idle
    uint8_t video_config   = 0x00; // filters/scanlines off
    uint8_t turbo_config   = 0x00; // 000 = no turbo chip/kick/data cache; every CPU access uses the
                                    // slow chip bus (cpu_wrapper.v:150-168 ramsel stays 0), so it is
                                    // always visible on ram_address/ram_data, never on fastram_*
    uint8_t ide_config     = 0x00; // 0 = no IDE drives
};

// One sample of the RAM port (ram_address/ram_data/_ram_oe/_ram_we), taken
// once per 7 MHz bus period at the same point nanomig/sim/nanomig_tb.cpp
// samples it (one clk_sys cycle after the falling 7 MHz edge, "ram_delay
// == 1" there). A bus cycle stretched by wait states produces one
// identical entry per 7 MHz period it spans; use coalesce() to collapse
// those into one logical access per address.
struct RamAccess {
    uint64_t cycle;       // clk_sys cycle count at the time of the sample
    uint32_t word_addr;   // ram_address value (word index, 22 bits significant)
    uint16_t data;        // ram_data (write) or the value served on ramdata_in (read)
    bool     is_write;
    bool     upper_byte;  // !_ram_bhe
    bool     lower_byte;  // !_ram_ble
};

// One clk_sys-cycle sample of the video output pins (nanomig.v:25-29).
struct VideoSample {
    uint8_t r, g, b; // 4 bits each, already blanked by the design during hbl/vbl (nanomig.v:1362-1364)
    bool hs, vs;
};

struct AudioSample {
    uint16_t left, right; // audio_left/audio_right, 15 bits significant
};

// A [begin,end) span of video samples, delimited by a hs or vs edge.
struct Span { size_t begin, end; };

// Splits a captured video trace into spans at every rising edge of `hs`.
// Because the Amber scan doubler is always enabled on this design
// (nanomig.v:1577 ".dblscan(1'b1)"), hs toggles at roughly twice the rate
// of the chipset's own horizontal sync (Amber.v:124,128 restarts its
// read-out pointer both at the doubler's own end-of-line and at the next
// incoming hsync), so a "line" here is one scan-doubler output line, not
// one Amiga scanline. Callers that want the Amiga's own line/frame rate
// should use find_frames() (vs is passed through undoubled, Amber.v:183).
inline std::vector<Span> find_lines(const std::vector<VideoSample>& v) {
    std::vector<Span> out;
    size_t start = 0;
    bool prev = v.empty() ? false : v[0].hs;
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i].hs && !prev) { out.push_back({start, i}); start = i; }
        prev = v[i].hs;
    }
    if (start < v.size()) out.push_back({start, v.size()});
    return out;
}

// Splits a captured video trace into spans at every rising edge of `vs`.
// vs is passed straight through the scan doubler (Amber.v:183 "_vsync_out
// <= dblscan ? _vsync_in : ..." keeps the chipset's own vsync), so each
// span here is one real field/frame.
inline std::vector<Span> find_frames(const std::vector<VideoSample>& v) {
    std::vector<Span> out;
    size_t start = 0;
    bool prev = v.empty() ? false : v[0].vs;
    for (size_t i = 1; i < v.size(); ++i) {
        if (v[i].vs && !prev) { out.push_back({start, i}); start = i; }
        prev = v[i].vs;
    }
    if (start < v.size()) out.push_back({start, v.size()});
    return out;
}

// Returns the first sample within [span.begin,span.end) whose r/g/b are
// not all zero, i.e. the first sample outside hblank/vblank (the design
// forces r/g/b to 0 during blanking, nanomig.v:1362-1364, before the
// signal even reaches the scan doubler). With no bitplanes enabled the
// background colour (COLOR00) covers the whole visible span, so any such
// sample reflects COLOR00.
inline std::optional<VideoSample> find_visible_sample(const std::vector<VideoSample>& v, Span span) {
    for (size_t i = span.begin; i < span.end && i < v.size(); ++i) {
        if (v[i].r || v[i].g || v[i].b) return v[i];
    }
    return std::nullopt;
}

// Reads the colour at a given pixel offset within a line span (as found by
// find_lines()), i.e. "the colour of a given pixel position on a given
// line". Returns nullopt if the offset falls outside the span or the
// trace. Unlike find_visible_sample() this does not look for a non-zero
// colour, so it also works when the colour under test is black -- e.g.
// the middle sample of a line is a position-based, colour-independent way
// to land inside the visible area (blanking only ever affects a portion
// near the edges of a line/frame, never the middle).
inline std::optional<VideoSample> sample_at(const std::vector<VideoSample>& v, Span line, size_t offset_in_line) {
    size_t idx = line.begin + offset_in_line;
    if (idx >= line.end || idx >= v.size()) return std::nullopt;
    return v[idx];
}

// Collapses consecutive RamAccess entries that describe the same still-in-
// -progress bus cycle (same word_addr/is_write/data, immediately adjacent
// in the log) into a single logical access. Bus cycles that take more
// than one 7 MHz period (wait states) otherwise appear once per period.
inline std::vector<RamAccess> coalesce(const std::vector<RamAccess>& log) {
    std::vector<RamAccess> out;
    for (const auto& a : log) {
        if (!out.empty() && out.back().word_addr == a.word_addr &&
            out.back().is_write == a.is_write && out.back().data == a.data) {
            continue;
        }
        out.push_back(a);
    }
    return out;
}

// Result of driving the design out of reset (see NanomigHarness::boot()).
struct BootResult {
    bool     cpu_released;      // cpu_nrst_out observed high before the cycle budget ran out
    uint64_t cycles_to_release; // clk_sys cycles from reset deassertion to cpu_nrst_out high
};

// Owns one Verilated nanomig instance plus everything a test needs on top
// of its ports: a flat RAM-port memory model with an access log, a
// minimal fast RAM model, and per-cycle video/audio capture.
class NanomigHarness {
public:
    explicit NanomigHarness(const Config& cfg)
        : top_(new Vnanomig), mem_(kRamWordCount, 0) {
        top_->memory_config  = cfg.memory_config;
        top_->fastram_config = cfg.fastram_config;
        top_->cpu_config     = cfg.cpu_config;
        top_->chipset_config = cfg.chipset_config;
        top_->floppy_config  = cfg.floppy_config;
        top_->video_config   = cfg.video_config;
        top_->turbo_config   = cfg.turbo_config;
        top_->ide_config     = cfg.ide_config;

        // sdc_* interface idle: no image mounted, no size, and the harness
        // never asserts sdc_busy/sdc_done/sdc_byte_in_strobe.
        top_->sdc_img_mounted = 0;
        top_->sdc_img_size    = 0;
        top_->sdc_busy        = 0;
        top_->sdc_done        = 0;
        top_->sdc_byte_in_strobe = 0;
        top_->sdc_byte_addr   = 0;
        top_->sdc_byte_in_data = 0;

        top_->mouse_buttons  = 0;
        top_->kbd_mouse_level = 0;
        top_->kbd_mouse_type  = 0;
        top_->kbd_mouse_data  = 0;
        top_->joystick0 = 0;
        top_->joystick1 = 0;
        top_->uart_rx = 1; // idle mark level

        top_->ramdata_in = 0;
        top_->chip48 = 0;
        top_->fastram_dout = 0;
        top_->fastram_dout48 = 0;
        top_->fastram_ready = 0;

        top_->clk_sys = 0;
        top_->por = 0;
        top_->reset = 0;
        top_->eval();
    }

    ~NanomigHarness() {
        top_->final();
        delete top_;
    }

    NanomigHarness(const NanomigHarness&) = delete;
    NanomigHarness& operator=(const NanomigHarness&) = delete;

    // ---- direct memory preload (no bus cycles, not logged) ------------

    // Writes one 16-bit word at a physical word address, i.e. what will
    // appear on ram_address when the design reads/writes it. Use
    // kKickShadowWordBase + (logical_addr/2) to preload the overlay
    // window, or logical_addr/2 directly for the identity-mapped
    // $080000+ chip RAM area (see the address space notes above).
    void poke_word(uint32_t phys_word_addr, uint16_t value) {
        mem_[phys_word_addr & kRamWordMask] = value;
    }

    // Writes a 32-bit value as two big-endian words (68000 byte order),
    // matching how the CPU reads a long from two consecutive RAM words.
    void poke_long(uint32_t phys_word_addr, uint32_t value) {
        poke_word(phys_word_addr,     uint16_t(value >> 16));
        poke_word(phys_word_addr + 1, uint16_t(value & 0xFFFF));
    }

    uint16_t peek_word(uint32_t phys_word_addr) const {
        return mem_[phys_word_addr & kRamWordMask];
    }

    // ---- 68000 program builder -----------------------------------------
    //
    // Emits instruction words sequentially starting at a physical word
    // address (see poke_word above for what "physical" means here).
    // Absolute-address *operands* embedded in an instruction (the targets
    // of MOVE ...,abs.L or abs.L,...) are always logical CPU addresses --
    // that is what ends up on chip_addr and what gary.v decodes -- never
    // the physical placement address. Encodings are derived directly from
    // the 68000 instruction format (Motorola M68000 Programmer's
    // Reference, MOVE instruction layout: bits 15-14 00, 13-12 size,
    // 11-9 dest register, 8-6 dest mode, 5-3 src mode, 2-0 src register;
    // absolute long = mode 111 reg 001, immediate = mode 111 reg 100) and
    // are only trusted once a test's own RAM-port log shows the CPU
    // fetching the addresses the encoding implies -- see
    // apparatus-cpu-to-video/test_apparatus.cpp for that check.
    class Program {
    public:
        Program(NanomigHarness& h, uint32_t start_phys_word_addr)
            : h_(h), cursor_(start_phys_word_addr) {}

        uint32_t here() const { return cursor_; }

        void word(uint16_t w) { h_.poke_word(cursor_++, w); }
        void long_(uint32_t l) { word(uint16_t(l >> 16)); word(uint16_t(l & 0xFFFF)); }

        // MOVE.W #imm,abs.L -- opcode 0x33FC = 0011 001 111 111 100
        //   size=11 (word), dest mode/reg=111/001 (abs.L), src mode/reg=111/100 (#imm)
        // followed by the immediate word, then the 32-bit absolute address.
        void move_w_imm_abs(uint16_t imm, uint32_t abs_addr) {
            word(0x33FC);
            word(imm);
            long_(abs_addr);
        }

        // MOVE.W abs.L,abs.L -- opcode 0x33F9 = 0011 001 111 111 001
        //   size=11 (word), dest mode/reg=111/001 (abs.L), src mode/reg=111/001 (abs.L)
        // followed by the 32-bit source address, then the 32-bit dest address.
        void move_w_abs_abs(uint32_t src_abs, uint32_t dst_abs) {
            word(0x33F9);
            long_(src_abs);
            long_(dst_abs);
        }

        // NOP -- 0x4E71, fixed encoding.
        void nop() { word(0x4E71); }

        // BRA.S disp8 -- 0110 0000 dddddddd, target = pc(of opcode)+2+disp8.
        void bra_s(int8_t disp8) { word(uint16_t(0x6000 | (uint8_t)disp8)); }

        // Branch to self: disp8 = -2 makes target == the opcode's own address.
        void bra_s_self() { bra_s(-2); }

    private:
        NanomigHarness& h_;
        uint32_t cursor_;
    };

    // ---- reset / run ----------------------------------------------------

    // Drives por and reset (nanomig.v:12-13), releasing por first (the
    // clock-enable generator amiga_clk.v is a free-running counter with
    // an async reset, it locks within a handful of clk_sys cycles) and
    // reset afterwards, then runs until cpu_nrst_out (nanomig.v:18) goes
    // high or max_cycles clk_sys cycles have elapsed since reset was
    // released. The design's own internal reset (minimig.v:851-857
    // minimig_syscontrol, counting sof pulses, i.e. whole frames, before
    // releasing sys_reset, which gates the CPU reset too at
    // minimig.v:1050-1057) is not a small fixed number of cycles -- this
    // polls cpu_nrst_out rather than assuming a count.
    BootResult boot(uint64_t max_cycles) {
        top_->por = 1;
        top_->reset = 1;
        for (int i = 0; i < 32; ++i) tick_pair();
        top_->por = 0;
        for (int i = 0; i < 32; ++i) tick_pair();
        top_->reset = 0;

        uint64_t start = cycle_;
        while (cycle_ - start < max_cycles) {
            tick_pair();
            if (top_->cpu_nrst_out) return {true, cycle_ - start};
        }
        return {false, cycle_ - start};
    }

    void run_cycles(uint64_t n) {
        for (uint64_t i = 0; i < n; ++i) tick_pair();
    }

    uint64_t cycles() const { return cycle_; }

    const std::vector<RamAccess>&   ram_log() const { return log_; }
    const std::vector<VideoSample>& video()   const { return video_; }
    const std::vector<AudioSample>& audio()   const { return audio_; }

    // Escape hatch onto the top-level ports only (e.g. clk7_en,
    // cpu_nrst_out, pwr_led) -- never use ->rootp or -Wno-public to reach
    // inside the design.
    Vnanomig* ports() { return top_; }

private:
    void tick_pair() { half(1); half(0); }

    void half(int c) {
        top_->clk_sys = c;
        top_->eval();
        if (!c) return;

        // Sample the RAM port one clk_sys cycle after the falling 7 MHz
        // edge, exactly like nanomig/sim/nanomig_tb.cpp's tick() does
        // ("ram works on/after falling 7mhz edge", ram_delay==1 there).
        if (top_->clk7n_en) ram_delay_ = 0; else ++ram_delay_;
        if (ram_delay_ == 1) {
            uint32_t word_addr = top_->ram_address & kRamWordMask;
            if (!top_->_ram_oe) {
                uint16_t v = mem_[word_addr];
                top_->ramdata_in = v;
                uint32_t base = word_addr & ~3u;
                uint64_t c48 = (uint64_t(mem_[(base + 1) & kRamWordMask]) << 32) |
                               (uint64_t(mem_[(base + 2) & kRamWordMask]) << 16) |
                                uint64_t(mem_[(base + 3) & kRamWordMask]);
                top_->chip48 = c48;
                log_.push_back({cycle_, word_addr, v, false, !top_->_ram_bhe, !top_->_ram_ble});
            }
            if (!top_->_ram_we) {
                uint16_t cur = mem_[word_addr];
                uint16_t nv = cur;
                if (!top_->_ram_bhe) nv = uint16_t((nv & 0x00FF) | (top_->ram_data & 0xFF00));
                if (!top_->_ram_ble) nv = uint16_t((nv & 0xFF00) | (top_->ram_data & 0x00FF));
                mem_[word_addr] = nv;
                log_.push_back({cycle_, word_addr, nv, true, !top_->_ram_bhe, !top_->_ram_ble});
            }
        }

        service_fastram();

        video_.push_back({uint8_t(top_->r), uint8_t(top_->g), uint8_t(top_->b),
                           bool(top_->hs), bool(top_->vs)});
        audio_.push_back({uint16_t(top_->audio_left), uint16_t(top_->audio_right)});
        ++cycle_;
    }

    // Minimal model of the fastram_* port (nanomig.v:88-96): a flat 4 MiB
    // word memory with a fixed two clk_sys cycle latency, acknowledged by
    // toggling fastram_ready the way nanomig.v expects (it compares
    // against the previous level, e.g. nanomig.v:252/465/486). With
    // turbo_config==0 the design never asserts fastram_sel (see the
    // Config comment above), so this path is not exercised by the
    // cpu-to-video apparatus check; it is provided for tests that do
    // enable turbo/fastram.
    void service_fastram() {
        if (fastram_busy_) {
            if (--fastram_delay_ == 0) {
                uint32_t a = fastram_addr_ & (kFastWordCount - 1);
                if (fastram_wr_) {
                    uint16_t cur = fastmem_[a];
                    uint16_t nv = cur;
                    if (fastram_uds_) nv = uint16_t((nv & 0x00FF) | (fastram_din_ & 0xFF00));
                    if (fastram_lds_) nv = uint16_t((nv & 0xFF00) | (fastram_din_ & 0x00FF));
                    fastmem_[a] = nv;
                } else {
                    top_->fastram_dout = fastmem_[a];
                    uint32_t base = a & ~3u;
                    top_->fastram_dout48 =
                        (uint64_t(fastmem_[(base + 1) & (kFastWordCount - 1)]) << 32) |
                        (uint64_t(fastmem_[(base + 2) & (kFastWordCount - 1)]) << 16) |
                         uint64_t(fastmem_[(base + 3) & (kFastWordCount - 1)]);
                }
                fastram_ready_level_ = !fastram_ready_level_;
                top_->fastram_ready = fastram_ready_level_;
                fastram_busy_ = false;
            }
            return;
        }
        if (top_->fastram_sel) {
            fastram_addr_ = top_->fastram_addr;
            fastram_wr_   = top_->fastram_wr;
            fastram_din_  = top_->fastram_din;
            fastram_uds_  = top_->fastram_uds;
            fastram_lds_  = top_->fastram_lds;
            fastram_delay_ = 2;
            fastram_busy_ = true;
        }
    }

    static constexpr uint32_t kFastWordCount = 1u << 22;

    Vnanomig* top_;
    std::vector<uint16_t> mem_;
    std::vector<uint16_t> fastmem_ = std::vector<uint16_t>(kFastWordCount, 0);
    std::vector<RamAccess>   log_;
    std::vector<VideoSample> video_;
    std::vector<AudioSample> audio_;
    uint64_t cycle_ = 0;
    int ram_delay_ = 0;

    bool     fastram_busy_ = false;
    uint8_t  fastram_delay_ = 0;
    bool     fastram_ready_level_ = false;
    uint32_t fastram_addr_ = 0;
    bool     fastram_wr_ = false;
    uint16_t fastram_din_ = 0;
    bool     fastram_uds_ = false;
    bool     fastram_lds_ = false;
};

} // namespace nmh
