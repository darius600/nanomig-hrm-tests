// test_aa_integ_s7_rest.cpp -- aa-integ-s7-rest: see SPECIFICATION.md.

#include "s7r_p1.h"
#include "s7r_p3.h"

using namespace s7r;

int main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    g_mutate = std::getenv("NMH_MUTATE");
    const char* only = std::getenv("NMH_ONLY");   // NMH_ONLY=HV,P8 restricts a run to the named parts
    std::printf("CORE: %s\n", cap_core().c_str());
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);
    Checks ck;
    auto want = [&](const char* p) {
        if (!only) return true;
        std::string l = std::string(",") + only + ",";
        return l.find(std::string(",") + p + ",") != std::string::npos;
    };
    Shared sh;
    if (want("HV") || want("P8") || want("REF") || want("U")) run_shared(ck, sh);
    if (want("REF")) run_refptr(ck, sh);
    if (want("U")) run_uhres(ck, sh);
    if (want("P8")) run_p8(ck, sh.hv);
    std::printf("%d passed, %d failed\n", ck.passed, ck.failed);
    return ck.failed == 0 ? 0 : 1;
}
