// listing_scan.h -- the assembler listing of a program and the static check
// of its data accesses (SPECIFICATION.md Section 6): the CPU program executes
// no data access to a region another channel uses, beyond the accesses a
// proposition names as its own.
#pragma once

#include <cstdint>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace lscan {

// Reads a whole file (a package .bin) into bytes; empty if it cannot be opened.
inline std::vector<uint8_t> read_file_bytes(const std::string& path) {
    std::vector<uint8_t> out;
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return out;
    std::fseek(f, 0, SEEK_END);
    long n = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    if (n > 0) { out.resize(size_t(n)); if (std::fread(out.data(), 1, out.size(), f) != out.size()) out.clear(); }
    std::fclose(f);
    return out;
}

// ---- assembler listings and the static checks built on them --------------
//
// vasm's listing (-L) gives, for every source line that emits code, the
// address and the emitted bytes, and at its end the symbol table. The test
// uses the listing of the very program it loaded: after the listing has been
// checked against the image byte for byte, the listing's instruction
// boundaries and symbols are what the image holds.
struct ListItem { uint32_t addr = 0; std::vector<uint8_t> bytes; };
struct Listing {
    bool ok = false;
    std::vector<ListItem> items;            // in file order
    std::map<std::string, uint32_t> sym;    // labels (A:) and equates (E:)
    std::set<uint32_t> labels;              // addresses of the labels
};

int hex_value(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

std::string listing_path(const std::string& bin_path) {
    const std::string ext = ".bin";
    if (bin_path.size() > ext.size() && bin_path.compare(bin_path.size() - ext.size(), ext.size(), ext) == 0)
        return bin_path.substr(0, bin_path.size() - ext.size()) + ".lst";
    return bin_path + ".lst";
}

Listing read_listing(const std::string& path) {
    Listing L;
    std::FILE* f = std::fopen(path.c_str(), "rb");
    if (!f) return L;
    std::string text;
    char buf[4096];
    size_t n;
    while ((n = std::fread(buf, 1, sizeof(buf), f)) > 0) text.append(buf, n);
    std::fclose(f);
    bool in_names = false;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t eol = text.find('\n', pos);
        if (eol == std::string::npos) eol = text.size();
        std::string line = text.substr(pos, eol - pos);
        pos = eol + 1;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.rfind("Symbols by name:", 0) == 0) { in_names = true; continue; }
        if (line.rfind("Symbols by value:", 0) == 0) { in_names = false; continue; }
        if (in_names) {
            size_t sp = line.find_first_of(" \t");
            if (sp == std::string::npos || sp == 0) continue;
            size_t k = line.find_first_not_of(" \t", sp);
            if (k == std::string::npos || k + 10 > line.size()) continue;
            char kind = line[k];
            if ((kind != 'A' && kind != 'E') || line[k + 1] != ':') continue;
            uint32_t v = 0;
            bool good = true;
            for (size_t i = 0; i < 8; ++i) {
                int h = hex_value(line[k + 2 + i]);
                if (h < 0) { good = false; break; }
                v = (v << 4) | uint32_t(h);
            }
            if (!good) continue;
            L.sym[line.substr(0, sp)] = v;
            if (kind == 'A') L.labels.insert(v);
            continue;
        }
        // code line: "SS:AAAAAAAA HEXBYTES<spaces><tab> ..."
        if (line.size() < 14 || line[2] != ':' || line[11] != ' ') continue;
        if (hex_value(line[0]) < 0 || hex_value(line[1]) < 0) continue;
        uint32_t addr = 0;
        bool good = true;
        for (size_t i = 3; i < 11; ++i) {
            int h = hex_value(line[i]);
            if (h < 0) { good = false; break; }
            addr = (addr << 4) | uint32_t(h);
        }
        if (!good) continue;
        ListItem it;
        it.addr = addr;
        size_t i = 12;
        while (i + 1 < line.size() && hex_value(line[i]) >= 0 && hex_value(line[i + 1]) >= 0) {
            it.bytes.push_back(uint8_t(hex_value(line[i]) * 16 + hex_value(line[i + 1])));
            i += 2;
        }
        // An instruction longer than eight bytes continues on a line that has
        // bytes but no source text: it extends the item before it.
        const bool continuation = i >= line.size() && !L.items.empty() &&
                                  L.items.back().addr + L.items.back().bytes.size() == addr;
        if (continuation) {
            for (uint8_t b : it.bytes) L.items.back().bytes.push_back(b);
        } else if (!it.bytes.empty()) {
            L.items.push_back(std::move(it));
        }
    }
    L.ok = !L.items.empty();
    return L;
}

// The listing reproduces the image: its items are contiguous from org and
// every byte equals the image's byte.
bool listing_matches_image(const Listing& L, const std::vector<uint8_t>& img, uint32_t org) {
    if (!L.ok || img.empty()) return false;
    uint32_t next = org;
    for (const auto& it : L.items) {
        if (it.addr != next) return false;
        if (size_t(it.addr - org) + it.bytes.size() > img.size()) return false;
        for (size_t i = 0; i < it.bytes.size(); ++i)
            if (img[it.addr - org + i] != it.bytes[i]) return false;
        next += uint32_t(it.bytes.size());
    }
    return next - org == img.size();
}




// A byte range [lo,hi] (inclusive) of Chip memory and the CPU accesses the
// program may make to it: at most max_reads data reads and max_writes data
// writes. The scan fills in how many the listing holds.
struct Guard {
    uint32_t lo, hi;
    unsigned max_reads = 0, max_writes = 0;
    unsigned reads = 0, writes = 0;
};

struct ScanResult { bool ok = true; std::string why; };

// Decodes every instruction of the listing (the 68000 instructions the
// package's programs use; any other opcode is not analysable and fails) and
// follows every effective-address path that reads or writes memory: absolute
// short/long, PC-relative, and register-indirect with the address register's
// value tracked from the instructions that set it. A data access whose
// address cannot be resolved fails. LEA and PEA compute an address without
// accessing it; the branches fetch from their target. A MOVE reads its source
// and writes its destination; CLR, NEG, NOT and NEGX read and then write; TST,
// CMP and BTST read.
inline ScanResult scan_data_accesses(const Listing& L, std::vector<Guard>& guards) {
    for (auto& g : guards) { g.reads = 0; g.writes = 0; }
    ScanResult res;
    auto fail = [&](uint32_t pc, const std::string& why) {
        if (res.ok) {
            char b[96];
            std::snprintf(b, sizeof(b), "at $%06X: ", pc);
            res.ok = false;
            res.why = std::string(b) + why;
        }
    };
    struct AReg { bool known = false; uint32_t v = 0; };
    AReg A[8];
    std::set<uint32_t> targets = L.labels;
    for (int pass = 0; pass < 2; ++pass) {
        for (auto& r : A) r = AReg();
        bool after_uncond = false;
        for (const auto& it : L.items) {
            const auto& b = it.bytes;
            const uint32_t pc = it.addr;
            if (after_uncond || targets.count(pc)) for (int i = 0; i < 7; ++i) A[i] = AReg();
            after_uncond = false;
            if (b.size() < 2 || (b.size() & 1)) { fail(pc, "item is not a whole number of words"); continue; }
            auto be16 = [&](size_t off) { return uint16_t((b[off] << 8) | b[off + 1]); };
            auto be32 = [&](size_t off) { return (uint32_t(be16(off)) << 16) | be16(off + 2); };
            const uint16_t op = be16(0);
            size_t ext = 2;

            struct EA { enum K { Dn, An, Mem, Imm } k = Dn; bool known = false; uint32_t addr = 0; };
            auto take = [&](int mode, int reg, int size) -> EA {
                EA e;
                auto step = [&](int sz) { return (sz == 1 && reg == 7) ? 2 : sz; };
                switch (mode) {
                case 0: e.k = EA::Dn; break;
                case 1: e.k = EA::An; break;
                case 2: e.k = EA::Mem; e.known = A[reg].known; e.addr = A[reg].v; break;
                case 3: e.k = EA::Mem; e.known = A[reg].known; e.addr = A[reg].v; A[reg].v += uint32_t(step(size)); break;
                case 4: A[reg].v -= uint32_t(step(size)); e.k = EA::Mem; e.known = A[reg].known; e.addr = A[reg].v; break;
                case 5:
                    if (ext + 2 > b.size()) { fail(pc, "truncated d16(An)"); break; }
                    e.k = EA::Mem; e.known = A[reg].known;
                    e.addr = A[reg].v + uint32_t(int32_t(int16_t(be16(ext)))); ext += 2; break;
                case 6:
                    e.k = EA::Mem; e.known = false; ext += 2; break; // the index register's value is not tracked
                case 7:
                    if (reg == 0) {
                        if (ext + 2 > b.size()) { fail(pc, "truncated abs.W"); break; }
                        e.k = EA::Mem; e.known = true; e.addr = uint32_t(int32_t(int16_t(be16(ext)))); ext += 2;
                    } else if (reg == 1) {
                        if (ext + 4 > b.size()) { fail(pc, "truncated abs.L"); break; }
                        e.k = EA::Mem; e.known = true; e.addr = be32(ext); ext += 4;
                    } else if (reg == 2) {
                        if (ext + 2 > b.size()) { fail(pc, "truncated d16(PC)"); break; }
                        e.k = EA::Mem; e.known = true;
                        e.addr = pc + uint32_t(ext) + uint32_t(int32_t(int16_t(be16(ext)))); ext += 2;
                    } else if (reg == 3) {
                        e.k = EA::Mem; e.known = false; ext += 2; // the index register's value is not tracked
                    } else if (reg == 4) {
                        e.k = EA::Imm; ext += (size == 4) ? 4 : 2;
                    } else fail(pc, "undefined effective address");
                    break;
                }
                return e;
            };
            auto touch = [&](const EA& e, int size, bool is_write) {
                if (e.k != EA::Mem) return;
                if (!e.known) { fail(pc, "a data access whose address is not resolvable"); return; }
                for (auto& g : guards)
                    if (e.addr <= g.hi && e.addr + uint32_t(size) - 1 >= g.lo) {
                        if (pass != 1) continue;
                        if (is_write) ++g.writes; else ++g.reads;
                    }
            };
            auto set_a = [&](int reg, bool known, uint32_t v) { A[reg].known = known; A[reg].v = v; };
            auto stack_push = [&](int size) {
                if (!A[7].known) { fail(pc, "stack pointer not tracked"); return; }
                A[7].v -= uint32_t(size);
                EA e; e.k = EA::Mem; e.known = true; e.addr = A[7].v; touch(e, size, true);
            };
            auto stack_pop = [&](int size) {
                if (!A[7].known) { fail(pc, "stack pointer not tracked"); return; }
                EA e; e.k = EA::Mem; e.known = true; e.addr = A[7].v; touch(e, size, false);
                A[7].v += uint32_t(size);
            };

            const int nib = op >> 12;
            if (nib == 1 || nib == 2 || nib == 3) {                              // MOVE, MOVEA
                const int size = (nib == 1) ? 1 : (nib == 3) ? 2 : 4;
                EA src = take((op >> 3) & 7, op & 7, size);
                EA dst = take((op >> 6) & 7, (op >> 9) & 7, size);
                touch(src, size, false);
                if (((op >> 6) & 7) == 1) {                                         // MOVEA
                    const int an = (op >> 9) & 7;
                    if (src.k == EA::Imm && ext >= 4) {
                        uint32_t v = (size == 2) ? uint32_t(int32_t(int16_t(be16(ext - 2)))) : be32(ext - 4);
                        set_a(an, true, v);
                    } else if (src.k == EA::An && A[op & 7].known) {
                        set_a(an, true, A[op & 7].v);
                    } else set_a(an, false, 0);
                } else touch(dst, size, true);
            } else if ((op & 0xF1C0) == 0x41C0) {                                // LEA
                EA e = take((op >> 3) & 7, op & 7, 4);
                const int an = (op >> 9) & 7;
                if (e.k == EA::Mem && e.known) set_a(an, true, e.addr); else set_a(an, false, 0);
            } else if ((op & 0xFFC0) == 0x4840 && ((op >> 3) & 7) >= 2) {        // PEA
                take((op >> 3) & 7, op & 7, 4);
                stack_push(4);
            } else if ((op & 0xFFC0) == 0x4EC0) {                                // JMP
                EA e = take((op >> 3) & 7, op & 7, 4);
                if (e.k == EA::Mem && e.known && pass == 0) targets.insert(e.addr);
                after_uncond = true;
            } else if ((op & 0xFFC0) == 0x4E80) {                                // JSR
                EA e = take((op >> 3) & 7, op & 7, 4);
                if (e.k == EA::Mem && e.known && pass == 0) targets.insert(e.addr);
                stack_push(4);
                for (int i = 0; i < 7; ++i) A[i] = AReg();
            } else if (op == 0x4E75) {                                           // RTS
                stack_pop(4);
                after_uncond = true;
            } else if (op == 0x4E71 || op == 0x4AFC) {                           // NOP, ILLEGAL
            } else if (nib == 6) {                                               // BRA, Bcc
                const int cond = (op >> 8) & 0xF;
                const int d8 = op & 0xFF;
                if (cond == 1) { fail(pc, "BSR is not analysed"); }
                else if (d8 == 0xFF) { fail(pc, "32-bit branch displacement"); }
                else {
                    int32_t disp = 0;
                    if (d8 == 0) {
                        if (b.size() < 4) fail(pc, "truncated branch");
                        else { disp = int16_t(be16(2)); ext += 2; }
                    } else disp = int8_t(uint8_t(d8));
                    if (pass == 0) targets.insert(pc + 2 + uint32_t(disp));
                    if (cond == 0) after_uncond = true;
                }
            } else if (nib == 7 && (op & 0x0100) == 0) {                         // MOVEQ
            } else if (nib == 0xB && (((op >> 6) & 7) <= 3 || ((op >> 6) & 7) == 7)) { // CMP, CMPA
                const int om = (op >> 6) & 7;
                const int size = (om == 0) ? 1 : (om == 1 || om == 3) ? 2 : 4;
                touch(take((op >> 3) & 7, op & 7, size), size, false);
            } else if ((op & 0xF1C0) == 0x80C0 || (op & 0xF1C0) == 0x81C0) {     // DIVU.W, DIVS.W
                touch(take((op >> 3) & 7, op & 7, 2), 2, false);
            } else if (((op & 0xFF00) == 0x4200 || (op & 0xFF00) == 0x4A00 || (op & 0xFF00) == 0x4400 ||
                        (op & 0xFF00) == 0x4600 || (op & 0xFF00) == 0x4000) && ((op >> 6) & 3) != 3) { // CLR TST NEG NOT NEGX
                const int size = 1 << ((op >> 6) & 3);
                const EA e = take((op >> 3) & 7, op & 7, size);
                touch(e, size, false);                                           // CLR, NEG, NOT, NEGX and TST read first
                if ((op & 0xFF00) != 0x4A00) touch(e, size, true);               // TST does not write
            } else if ((op & 0xFFC0) == 0x44C0 || (op & 0xFFC0) == 0x46C0) {     // MOVE to CCR/SR
                touch(take((op >> 3) & 7, op & 7, 2), 2, false);
            } else if ((op & 0xFFC0) == 0x40C0) {                                // MOVE from SR
                touch(take((op >> 3) & 7, op & 7, 2), 2, true);
            } else if ((op & 0xFFC0) == 0x0800) {                                // BTST #n,<ea>
                ext = 4;
                touch(take((op >> 3) & 7, op & 7, 1), 1, false);
            } else if ((op & 0xFFF0) == 0x4E60) {                                // MOVE USP
                if (op & 8) set_a(op & 7, false, 0);
            } else {
                char t[64];
                std::snprintf(t, sizeof(t), "opcode $%04X is not analysable", op);
                fail(pc, t);
            }
            if (res.ok && ext != b.size()) {
                char t[112];
                std::snprintf(t, sizeof(t), "decoded length %d differs from the listing's %d bytes (opcode $%04X)",
                              int(ext), int(b.size()), op);
                fail(pc, t);
            }
        }
    }
    return res;
}

// The program's listing reproduces its image (checked byte for byte) and no
// guard's access count exceeds what the guard allows.
inline bool guards_hold(const std::string& bin, std::vector<Guard>& guards, uint32_t org, std::string& why) {
    auto img = read_file_bytes(bin);
    Listing L = read_listing(listing_path(bin));
    if (!listing_matches_image(L, img, org)) { why = "the listing does not reproduce the image"; return false; }
    ScanResult r = scan_data_accesses(L, guards);
    if (!r.ok) { why = r.why; return false; }
    for (const auto& g : guards) {
        if (g.reads > g.max_reads || g.writes > g.max_writes) {
            char b[160];
            std::snprintf(b, sizeof b, "guard $%06X-$%06X: %u reads (allowed %u), %u writes (allowed %u)",
                          g.lo, g.hi, g.reads, g.max_reads, g.writes, g.max_writes);
            why = b;
            return false;
        }
    }
    return true;
}

} // namespace lscan
