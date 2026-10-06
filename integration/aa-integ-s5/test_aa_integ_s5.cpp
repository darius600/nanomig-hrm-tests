// test_aa_integ_s5.cpp -- aa-integ-s5: see SPECIFICATION.md.

#include "s5_serial.h"

using namespace s5;

namespace {

// The proposition a selector belongs to ("P2-SWAP" -> "P2"); the shared selectors apply to every proposition.
std::string sel_prop() {
    if (!g_mutate) return "";
    std::string s = g_mutate;
    if (s[0] != 'P') return "";
    size_t i = 1;
    while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;
    return s.substr(0, i);
}

// The propositions a shared selector applies to (none of the others runs).
bool shared_scope(const std::string& sel, const char* p) {
    auto in = [&](std::initializer_list<const char*> l) { for (auto* q : l) if (std::strcmp(q, p) == 0) return true; return false; };
    if (sel == "GT" || sel == "GBRK") return in({"P16", "P17", "P18"}) || (sel == "GBRK" && in({"P20"}));
    if (sel == "GRXD") return in({"P19", "P20"});
    return true;   // G0, GC, GD, GF, FIXTURE
}

} // namespace

int main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    g_mutate = std::getenv("NMH_MUTATE");
    const char* only = std::getenv("NMH_ONLY");   // NMH_ONLY=P2,P3 restricts a run to the named propositions
    std::string core = NMH_XSTR(NANOMIG_CORE_NAME);
    std::printf("CORE: %s\n", core.c_str());
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);
    Checks ck;
    const std::string sp = sel_prop();
    auto want = [&](const char* p) {
        if (only) {   // NMH_ONLY is a comma-separated list of proposition names
            std::string l = std::string(",") + only + ",";
            if (l.find(std::string(",") + p + ",") == std::string::npos && !(std::string(p) == "P6" && l.find(",P7,") != std::string::npos) &&
                !(std::string(p) == "P7" && l.find(",P6,") != std::string::npos)) return false;
        }
        if (sp.empty() && g_mutate) return shared_scope(g_mutate, p);
        return sp.empty() || sp == p;
    };
    if (want("P1")) run_p1(ck);
    if (want("P2")) run_p2(ck);
    if (want("P3")) run_p3(ck);
    if (want("P4")) run_p4(ck);
    if (want("P5")) run_p5(ck);
    if (want("P6") || want("P7")) run_p67(ck);
    if (want("P8")) run_p8(ck);
    if (want("P9")) run_vol_att(ck, "P9", "sol:AA-adkcon.c30", 0, 1, "00 USE0V1 Use audio channel 0 to modulate volume of channel 1");
    if (want("P10")) run_vol_att(ck, "P10", "sol:AA-adkcon.c29", 1, 2, "01 USE1V2 Use audio channel 1 to modulate volume of channel 2");
    if (want("P11")) run_vol_att(ck, "P11", "sol:AA-adkcon.c28", 2, 3, "02 USE2V3 Use audio channel 2 to modulate volume of channel 3");
    if (want("P12")) run_per_att(ck, "P12", "sol:AA-adkcon.c26", 0, 1, "04 USE0P1 Use audio channel 0 to modulate period of channel 1");
    if (want("P13")) run_per_att(ck, "P13", "sol:AA-adkcon.c25", 1, 2, "05 USE1P2 Use audio channel 1 to modulate period of channel 2");
    if (want("P14")) run_per_att(ck, "P14", "sol:AA-adkcon.c24", 2, 3, "06 USE2P3 Use audio channel 2 to modulate period of channel 3");
    if (want("P15")) run_p15(ck);
    if (want("P16") || want("P17")) run_p16_17(ck);
    if (want("P18")) run_p18(ck);
    if (want("P19")) run_p19(ck);
    if (want("P20")) run_p20(ck);
    if (!only && !g_mutate) {
        // claims the package declines (SPECIFICATION.md section 1) and clauses it leaves open
        ck.record("DECLINED sol:AA-audxdat.c8: '0.8 volts (peak to peak, typ)' is an analog voltage; the ports show digital samples, and 'typ' is no limit");
        ck.record("DECLINED sol:AA-audxvol.c5: '15-07 Not used' states no value; every stimulus writes these bits as 0");
        ck.record("DECLINED sol:AA-serdat.c9: 'A stop bit must be provided as part of the data word' is an obligation on the program; every stimulus meets it");
        ck.record("DECLINED sol:AA-audxlen.c2: 'AUDxLEN 0A4 W' read half: the page gives no value for a read of a write-only address; the write half is exercised by P1");
        ck.record("DECLINED sol:AA-audxvol.c2: 'AUDxVOL 0A8 W' read half: no value for a read of a write-only address; the write half is graded in P6");
        ck.record("DECLINED sol:AA-audxdat.c2: 'AUDxDAT 0AA W' read half: no value for a read of a write-only address; the write half is graded in P5");
        ck.record("NOT GRADED: which byte of an AUDxDAT word is output first (recorded in P4), what the output does after both bytes of a direct write (P5), when an attached modulator word takes effect (P9-P14), the meaning of AUDxLEN = 0, SERDATR bits 9 and 8 and OVRUN, SERDATR bit 13 while TBE is cleared with the buffer empty or set by software with the buffer full");
    }
    std::printf("%d passed, %d failed\n", ck.passed, ck.failed);
    return ck.failed == 0 ? 0 : 1;
}
