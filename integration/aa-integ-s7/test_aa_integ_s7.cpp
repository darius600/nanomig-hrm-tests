// test_aa_integ_s7.cpp -- aa-integ-s7: see SPECIFICATION.md.

#include "s7_f6.h"

using namespace s7;

namespace {

const char* kP6 = "AA p.6 '~=Address not writable by the Coprocessor unless COPCON bit 1 is set true.'; AA p.19 COPCON bit 01 CDANG '(if 0, access to RGA>7E)'";

// The proposition a selector belongs to ("C7-PC1" -> "C7"; "FIXTURE" belongs to every one).
std::string sel_prop() {
    if (!g_mutate) return "";
    std::string s = g_mutate;
    size_t i = s.find('-');
    return i == std::string::npos ? s : s.substr(0, i);
}

void do_c(Checks& ck, const Prop& p, const std::string& claims, const std::string& what, const std::string& pc5, const std::string& pc6 = "") {
    CRun r = run_c(p, g_mutate ? g_mutate : "");
    CEval e = eval_c(ck, p, r);
    print_c(ck, p, e, "(" + std::string(kP6) + ") claims " + claims, what, pc5, pc6);
}

} // namespace

int main(int argc, char** argv) {
    g_dir = argc > 1 ? argv[1] : ".";
    g_mutate = std::getenv("NMH_MUTATE");
    const char* only = std::getenv("NMH_ONLY");   // NMH_ONLY=C1,C3 restricts a run to the named propositions
    std::string core = NMH_XSTR(NANOMIG_CORE_NAME);
    std::printf("CORE: %s\n", core.c_str());
    if (g_mutate) std::printf("NMH_MUTATE=%s\n", g_mutate);
    Checks ck;
    const std::string sp = sel_prop();
    auto want = [&](const char* p) {
        if (only) {
            std::string l = std::string(",") + only + ",";
            if (l.find(std::string(",") + p + ",") == std::string::npos) return false;
        }
        return sp.empty() || sp == "FIXTURE" || sp == p;
    };
    const std::string pc5blt = "DMACONR reads DMAEN and BLTEN set, and no blitter access lies outside the windows of the cases, so every blit finished inside its case (AA p.22)";
    if (want("C1")) do_c(ck, prop_c1(), "opus:AA-dsklen.c4 sol:AA-dsklen.c4",
        "a Copper MOVE to DSKLEN ($4000) changes DSKBYTR bit 13, the mirror of DSKLEN bit 14 WRITE, only while COPCON bit 1 is set: 0 with COPCON $0000 and $FFFD, 1 with $0002 (AA p.23)", "");
    if (want("C3")) do_c(ck, prop_c3(), "opus:AA-vposw.c4 sol:AA-vposw.c4",
        "a Copper MOVE to VPOSW ($8000) changes VPOSR bit 15, the frame flop LOF, only while COPCON bit 1 is set: 0 with COPCON $0000 and $FFFD, 1 with $0002 (AA p.29)", "");
    if (want("C5")) do_c(ck, prop_c5(), "opus:AA-potgo.c4 sol:AA-potgo.c4",
        "a Copper MOVE to POTGO ($AA00) changes the pot-pin drive read in POTINP & $5500 only while COPCON bit 1 is set: $5500 with COPCON $0000 and $FFFD, $0000 with $0002 (AA p.26-27)", "");
    if (want("C7")) do_c(ck, prop_c7(), "opus:AA-bltcon0.c4 sol:AA-bltcon0.c4 opus:AA-bltcon0l.c5 sol:AA-bltcon0l.c5",
        "a Copper MOVE to BLTCON0 ($01FF) or to BLTCON0L ($00FF) changes the logic function of the next blit only while COPCON bit 1 is set: four writes of $0000 with COPCON $0000 and $FFFD, four of $FFFF with $0002 (AA p.14)", pc5blt,
        "every observed blit made exactly four blitter writes");
    if (want("C8")) do_c(ck, prop_c8(), "opus:AA-bltcon1.c4 sol:AA-bltcon1.c4",
        "a Copper MOVE to BLTCON1 ($0002, DESC) changes the address direction of the next blit only while COPCON bit 1 is set: writes at DST, DST+2 with COPCON $0000 and $FFFD, at DST, DST-2 with $0002 (AA p.14)", pc5blt,
        "every observed blit made exactly two blitter writes");
    if (want("C9")) do_c(ck, prop_c9(), "opus:AA-bltafwm.c4 sol:AA-bltafwm.c4 opus:AA-bltalwm.c4 sol:AA-bltalwm.c4",
        "a Copper MOVE of $0F0F to BLTAFWM or to BLTALWM changes the first-word or last-word mask of the next blit only while COPCON bit 1 is set (AA p.13)", pc5blt,
        "every observed blit made exactly three blitter reads of ASRC and three blitter writes");
    if (want("C10")) do_c(ck, prop_c10(), "opus:AA-bltsize.c4 sol:AA-bltsize.c4 opus:AA-bltsizh.c5 sol:AA-bltsizh.c5",
        "a Copper MOVE to BLTSIZE ($0044) or to BLTSIZH ($0004) starts the blitter only while COPCON bit 1 is set: no blitter write with COPCON $0000 and $FFFD, four writes of $FFFF with $0002 (AA p.14)", pc5blt);
    if (want("C11")) do_c(ck, prop_c11(), "opus:AA-bltsizv.c5 sol:AA-bltsizv.c5",
        "a Copper MOVE of 5 to BLTSIZV changes the height of the next blit only while COPCON bit 1 is set: 2 writes with COPCON $0000 and $FFFD, 5 with $0002 (AA p.14)", pc5blt);
    if (want("C2")) do_c(ck, prop_c2(), "opus:AA-dskpth.c5 sol:AA-dskpth.c5 opus:AA-dskptl.c5 sol:AA-dskptl.c5",
        "a Copper MOVE to DSKPTH or to DSKPTL changes the address from which disk DMA reads only while COPCON bit 1 is set: the first disk read is at Rd with COPCON $0000 and $FFFD and at Re (DSKPTH) or Re' (DSKPTL) with $0002, and disk DMA with DMAON set reads RAM in every run (AA p.23 'This pair of registers contains the 20 bit address of Disk DMA data.')",
        "DMACONR reads DMAEN and DSKEN set, and no disk read lies outside the windows of the cases, so every disk DMA finished inside its case (AA p.22, p.23)",
        "DSKBYTR bit 14 DMAON reads 1 in every run of the disk DMA sequence (AA p.23 'DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)')");
    if (want("C4")) do_c(ck, prop_c4(), "opus:AA-vhposw.c4 sol:AA-vhposw.c4",
        "a Copper MOVE to VHPOSW ($C000) moves the beam counter only while COPCON bit 1 is set: VHPOSR bits 15-8 read $41 or $42 at the first poll past $40 with COPCON $0000 and $FFFD, $C0 or $C1 with $0002 (AA p.29)", "");
    if (want("C6")) do_c(ck, prop_c6(), "opus:AA-joytest.c4 sol:AA-joytest.c4",
        "a Copper MOVE to JOYTEST ($FCFC) changes the mouse counters of JOY0DAT and JOY1DAT (read & $FCFC) only while COPCON bit 1 is set: $0000 with COPCON $0000 and $FFFD, $FCFC with $0002 (AA p.26)", "",
        "the two reads of each of JOY0DAT and JOY1DAT in a case are equal, so nothing but the write moved the counters");
    if (want("C12")) { C12Run r = run_c12(g_mutate ? g_mutate : ""); eval_print_c12(ck, r, ""); }
    const std::string sel = g_mutate ? g_mutate : "";
    if (want("F1")) run_f1_f2(ck, false, sel);
    if (want("F2")) run_f1_f2(ck, true, sel);
    if (want("F3")) run_f3(ck, sel);
    if (want("F4")) run_f4(ck);
    if (want("F5")) run_f5(ck, sel);
    if (want("F6")) run_f6(ck, sel);
    if (want("F7")) run_f7(ck);
    if (want("F8")) run_f8(ck, sel);
    if (want("F9")) run_f9(ck, sel);
    if (!g_mutate && !only) {
        // claims the package declines (SPECIFICATION.md section 0)
        ck.record("DECLINED the ~ on a read address or a dummy address (SPECIFICATION.md 0.1): no page states an effect of a write to it, by the CPU or by the Copper, so COPCON bit 1 set and clear predict the same outcome at every output: opus:AA-adkconr.c4 sol:AA-adkconr.c4 opus:AA-dskbytr.c4 sol:AA-dskbytr.c4 opus:AA-intenar.c4 sol:AA-intenar.c4 opus:AA-intreqr.c4 sol:AA-intreqr.c4 opus:AA-joy0dat.c4 sol:AA-joy0dat.c4 opus:AA-joy1dat.c4 sol:AA-joy1dat.c4 opus:AA-pot0dat.c4 sol:AA-pot0dat.c4 opus:AA-pot1dat.c4 sol:AA-pot1dat.c4 opus:AA-potinp.c4 sol:AA-potinp.c4 opus:AA-vhposr.c4 sol:AA-vhposr.c4 opus:AA-vposr.c4 sol:AA-vposr.c4 opus:AA-lisaid.c5 sol:AA-lisaid.c5 opus:AA-dskdatr.c5 sol:AA-dskdatr.c5 opus:AA-bltddat.c5 sol:AA-bltddat.c5");
        ck.record("DECLINED the disk-side effects (SPECIFICATION.md 0.2): each needs data read from or written to a disk surface, and the harness serves no disk image: opus:AA-dskdat.c5 sol:AA-dskdat.c5 opus:AA-dsksync.c4 sol:AA-dsksync.c4 opus:AA-dskdatr.c4 sol:AA-dskdatr.c4 opus:AA-dskdatr.c2 sol:AA-dskdatr.c2");
        ck.record("DECLINED the ~ of BLTADAT, BLTBDAT and BLTCDAT (SPECIFICATION.md 0.3, D1): with COPCON bit 1 set the pages disagree on what a Copper write does; C12 grades the bit-1-clear half and carries no claim id: opus:AA-bltadat.c5 sol:AA-bltadat.c5 opus:AA-bltbdat.c5 sol:AA-bltbdat.c5 opus:AA-bltcdat.c5 sol:AA-bltcdat.c5");
        ck.record("DECLINED the & of BPL2DAT-BPL8DAT (SPECIFICATION.md 0.3, D1): 'only' is contradicted by They may also be written by either micro; F6 grades the DMA part and carries no claim id: opus:AA-bpl2dat.c4 sol:AA-bpl2dat.c4 opus:AA-bpl3dat.c4 sol:AA-bpl3dat.c4 opus:AA-bpl4dat.c4 sol:AA-bpl4dat.c4 opus:AA-bpl5dat.c4 sol:AA-bpl5dat.c4 opus:AA-bpl6dat.c4 sol:AA-bpl6dat.c4 opus:AA-bpl7dat.c4 sol:AA-bpl7dat.c4 opus:AA-bpl8dat.c4 sol:AA-bpl8dat.c4");
        ck.record("DECLINED the & of AUD0DAT-AUD3DAT (SPECIFICATION.md 0.3, D1): 'only' is contradicted by The Processor can also write directly to this register; F7 grades the DMA part through the W claims: opus:AA-aud0dat.c4 sol:AA-aud0dat.c4 opus:AA-aud1dat.c4 sol:AA-aud1dat.c4 opus:AA-aud2dat.c4 sol:AA-aud2dat.c4 opus:AA-aud3dat.c4 sol:AA-aud3dat.c4");
        ck.record("DECLINED the & of BLTADAT, BLTBDAT, BLTCDAT, BLTDDAT and DSKDAT (SPECIFICATION.md 0.3, D1): 'only' is contradicted by it may also be preloaded by the microprocessor or is not observable; F8 and F9 grade the DMA part and carry no claim id: opus:AA-bltadat.c4 sol:AA-bltadat.c4 opus:AA-bltbdat.c4 sol:AA-bltbdat.c4 opus:AA-bltcdat.c4 sol:AA-bltcdat.c4 opus:AA-bltddat.c4 sol:AA-bltddat.c4 opus:AA-dskdat.c4 sol:AA-dskdat.c4");
    }
    std::printf("%d passed, %d failed\n", ck.passed, ck.failed);
    return ck.failed == 0 ? 0 : 1;
}
