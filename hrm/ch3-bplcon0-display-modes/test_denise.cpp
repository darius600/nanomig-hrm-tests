// ch3-bplcon0-display-modes -- HRM conformance harness for BPLCON0's mode-bit
// field (HIRES, BPU, HOMOD, DBLPF, COLOR_ON, LACE, ERSY, LPEN, and the bits
// chapter 3 calls unused) at the `denise` fixture boundary.
//
// Oracle: the printed Amiga Hardware Reference Manual quotations in
// claims.jsonl (see SPEC.md, written and committed before this file existed).
// If a measurement disagrees with the manual, the RTL is what is wrong: this
// file records that as a FAIL and does not adjust the expectation.
//
// Clock phases (trap 1 -- see SPEC.md and PACKAGE.md): c1 and c3 OVERLAP.
// phase0: clk7_en=1,c1=1,c3=0   phase1: c1=1,c3=1   phase2: c1=0,c3=1
// phase3: c1=0,c3=0
// denise_bitplanes.v:381 gates its BPLxDAT write strobe on c1&c3 (phase1);
// the shifter parallel-load fires on !c1&&!c3 (phase3). Driving c1/c3 as
// disjoint single-cycle pulses (as at least one older harness in this repo
// does) means that strobe never fires and every pixel silently reads colour
// 0. This harness was verified against PACKAGE.md's own MEASURED example
// (category 0 / A-bpc.intro) before any other claim was asserted.

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Vdenise.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

static VerilatedVcdC* g_tfp = nullptr;
static vluint64_t g_simtime = 0;

// ---------------------------------------------------------------------------
// bookkeeping
// ---------------------------------------------------------------------------

static int g_total = 0, g_pass = 0, g_fail = 0;

static void A(const std::string& id, int page, const std::string& reader,
              bool pass, const std::string& msg) {
  g_total++;
  if (pass) g_pass++; else g_fail++;
  std::printf("[%s] %s (p.%d, %s): %s\n", pass ? "PASS" : "FAIL",
              id.c_str(), page, reader.c_str(), msg.c_str());
}

// ---------------------------------------------------------------------------
// harness core
// ---------------------------------------------------------------------------

static Vdenise* dut;
static int g_phase = 0;  // 0..3
// hpos tracking: denise's own `hpos` register is internal (not a port), but
// it is fully deterministic from strhor/clk7_en, which this harness alone
// drives -- so a mirrored counter here always matches the RTL's hpos exactly.
static int g_hpos = 0;

static void applyPhase() {
  dut->clk7_en = (g_phase == 0);
  dut->c1      = (g_phase == 0 || g_phase == 1);
  dut->c3      = (g_phase == 1 || g_phase == 2);
}

// denise's own `hpos` register is internal (not a port of top-level denise),
// but it is entirely deterministic from strhor and clk7_en (denise.v:136-142,
// confirmed by reading denise.v): hpos<=2 on strhor, else hpos<=hpos+1, both
// gated by clk7_en (phase 0). Mirrored here, centrally, in tick() itself so
// every call site (writeReg included) keeps it exact -- this is bookkeeping
// for the harness's own window/border checks, not a claim oracle.
static void tick() {
  if (g_phase == 0) { if (dut->strhor) g_hpos = 2; else g_hpos++; }
  applyPhase();
  dut->eval();
  if (g_tfp) { g_tfp->dump(g_simtime); g_simtime++; }
  dut->clk = 1;
  dut->eval();
  if (g_tfp) { g_tfp->dump(g_simtime); g_simtime++; }
  dut->clk = 0;
  dut->eval();
  if (g_tfp) { g_tfp->dump(g_simtime); g_simtime++; }
  g_phase = (g_phase + 1) % 4;
}

static void ticks(int n) { for (int i = 0; i < n; i++) tick(); }

// align so that the *next* tick() call executes phase 0 (clk7_en pulse)
static void alignPhase0() { while (g_phase != 0) tick(); }

// register bus write: address is the 9-bit custom-chip offset (e.g. 0x100
// for BPLCON0); denise's reg_address_in port carries bits [8:1] of that
// offset (denise.v:39, confirmed by reading the module header), i.e. addr>>1.
static void writeReg(int addr, uint16_t data) {
  alignPhase0();
  dut->reg_address_in = (addr >> 1) & 0xFF;
  dut->data_in = data;
  tick();  // executes phase 0 with clk7_en=1 -> register captures data_in
  dut->reg_address_in = 0;
  dut->data_in = 0;
}

static void resetPulse() {
  dut->reset = 1;
  ticks(16);
  dut->reset = 0;
  ticks(16);
}

// pulse strhor so hpos resets to 2, then run to a chosen hpos value
static void syncHpos() {
  alignPhase0();
  dut->strhor = 1;
  tick();
  dut->strhor = 0;
  ticks(3);
  g_hpos = 2;
}

// ---------------------------------------------------------------------------
// register addresses (denise.v parameters, confirmed by reading denise.v)
// ---------------------------------------------------------------------------

static const int A_DIWSTRT = 0x08e;
static const int A_DIWSTOP = 0x090;
static const int A_DIWHIGH = 0x1e4;
static const int A_BPLCON0 = 0x100;
static const int A_BPLCON1 = 0x102;
static const int A_BPLCON2 = 0x104;
static const int A_BPLCON3 = 0x106;
static const int A_BPLCON4 = 0x10c;
static const int A_BPL1DAT = 0x110;
static const int A_BPL2DAT = 0x112;
static const int A_BPL3DAT = 0x114;
static const int A_BPL4DAT = 0x116;
static const int A_BPL5DAT = 0x118;
static const int A_BPL6DAT = 0x11a;
static const int A_COLORBASE = 0x180;

// BPLCON0 field builder (denise.v:180-192)
static uint16_t mkBplcon0(bool hires, int bpu3, bool homod, bool dblpf,
                           bool colorOn = false, bool shres = false,
                           bool lace = false, bool lpen = false,
                           bool ersy = false, bool ecsena = false,
                           int bit7 = 0) {
  uint16_t v = 0;
  if (hires) v |= (1u << 15);
  v |= (uint16_t)((bpu3 & 0x7) << 12);
  if (homod) v |= (1u << 11);
  if (dblpf) v |= (1u << 10);
  if (colorOn) v |= (1u << 9);
  if (bit7) v |= (1u << 7);
  if (shres) v |= (1u << 6);
  if (lpen) v |= (1u << 3);
  if (lace) v |= (1u << 2);
  if (ersy) v |= (1u << 1);
  if (ecsena) v |= (1u << 0);
  return v;
}

// preload COLOR0..COLOR31 with COLORn = n (12-bit value). Recoverable
// exactly: the colour table replicates each 4-bit nibble into an 8-bit
// channel (denise_colortable.v:58, {hi[3:0],hi[3:0]} etc, confirmed by
// reading that file) when loct=0 (always true here, aga=0), so an observed
// channel byte V was written nibble v = V/17 (exact for v in 0..15, since
// 17*v <= 255 for v<=15 and the replication is literally v*17). COLORn=n for
// n=0..31 therefore reads back with red=0 always, green nibble=(n>>4),
// blue nibble=(n&0xF) -- i.e. the two low nibbles ARE n. This decode uses
// only the replicate-nibble fact (a documented, general Amiga OCS colour
// convention, not a value read out of this specific RTL) plus arithmetic;
// it never converts a raw tick/measurement using an RTL-sourced constant.
static void preloadColours() {
  for (int n = 0; n < 32; n++) writeReg(A_COLORBASE + 2 * n, (uint16_t)n);
}

static int decodeIndex(uint8_t r, uint8_t g, uint8_t b) {
  int gn = (int)std::lround(g / 17.0);
  int bn = (int)std::lround(b / 17.0);
  if (r != 0) return -1;  // outside the COLORn=n<32 encoding -> not decodable
  if (gn > 1) return -1;  // n<32 means green nibble is 0 or 1 only
  return gn * 16 + bn;
}

struct Px { uint8_t r, g, b; };

static Px sample() { return Px{dut->red, dut->green, dut->blue}; }

static Px advanceOneHpos() {
  alignPhase0();
  ticks(4);  // phase0..phase3 of this hpos tick (g_hpos updated inside tick())
  return sample();
}

// sample once per 7 MHz tick (once per hpos increment) for n hpos ticks,
// sampling at the end of each tick's phase 3 (settled)
static std::vector<Px> scanHpos(int n) {
  std::vector<Px> out;
  for (int i = 0; i < n; i++) out.push_back(advanceOneHpos());
  return out;
}

struct HPx { int hpos; Px px; };
static std::vector<HPx> scanHposTagged(int n) {
  std::vector<HPx> out;
  for (int i = 0; i < n; i++) { Px p = advanceOneHpos(); out.push_back(HPx{g_hpos, p}); }
  return out;
}

// fine-grained sampler: one sample per 28 MHz clk, for nclks clocks
static std::vector<Px> scanClk(int nclks) {
  std::vector<Px> out;
  for (int i = 0; i < nclks; i++) { tick(); out.push_back(sample()); }
  return out;
}

static bool eq(const Px& a, const Px& b) {
  return a.r == b.r && a.g == b.g && a.b == b.b;
}

// Trap 2 (given): denise_bitplanes.v:146-152 decodes hpos[3:2] into 0/4/8/12
// lores pixels of EXTRA SCROLL DELAY (extra_delay_f0), latched into
// extra_delay_r at the tick load becomes active and added into pf1h_del /
// pf2h_del, which select where in the 64-bit scroller history the displayed
// bit is read from. Measured directly: without this alignment, the loaded
// byte's visible window can land many hpos-ticks away from where a naive
// "scan right after the write" would look -- confirmed by a VCD trace during
// harness development, where the same load became visible at a completely
// different hpos depending solely on which hpos[3:2] phase the write landed
// on. loadPlanes() below aligns so the load lands on hpos[3:2]==0, giving
// exactly zero extra delay, rather than carrying/computing the offset.
static void alignForCleanLoad(int writesUntilBpl1dat) {
  while (((g_hpos + writesUntilBpl1dat + 1) & 0xC) != 0) advanceOneHpos();
}

// write BPL1..6DAT (2..6 optional via the vector; unspecified = 0). BPL1DAT
// is written LAST so the shared `load` strobe (denise_bitplanes.v) captures
// every plane's already-buffered word together (denise_bitplanes.v:467 and
// the per-plane bpl*dat buffer registers, confirmed by reading that file).
static void loadPlanes(uint16_t p1, uint16_t p2 = 0, uint16_t p3 = 0,
                        uint16_t p4 = 0, uint16_t p5 = 0, uint16_t p6 = 0) {
  alignForCleanLoad(6);  // 6 writes below, the 6th (BPL1DAT) triggers the load
  writeReg(A_BPL2DAT, p2);
  writeReg(A_BPL3DAT, p3);
  writeReg(A_BPL4DAT, p4);
  writeReg(A_BPL5DAT, p5);
  writeReg(A_BPL6DAT, p6);
  writeReg(A_BPL1DAT, p1);  // triggers l_bpu latch (denise.v:194-203) + load
  // Measured (VCD trace during harness development): with alignForCleanLoad's
  // zero extra delay, the shifter's parallel-load fires 3 ticks after the
  // BPL1DAT write and the first loaded bit is visible on the FOLLOWING
  // hpos-tick. Advance exactly 2 hpos-ticks (8 sub-phase ticks) here, so the
  // caller's own first scan sample (which itself advances one more hpos-tick
  // before sampling) lands squarely on that first visible pixel rather than
  // one hpos-tick past it.
  ticks(8);
}

// baseline: window H=$30..$50 (PACKAGE.md's own MEASURED example), BPLCON1/2/3
// cleared, COLOR0..31 preloaded with the recoverable n=index encoding.
static void baseline(bool ecs, bool aga = false, bool a1k = false) {
  dut->ecs = ecs;
  dut->aga = aga;
  dut->a1k = a1k;
  dut->blank = 0;
  writeReg(A_DIWSTRT, 0x0030);
  writeReg(A_DIWSTOP, 0x0050);
  writeReg(A_DIWHIGH, 0x0000);
  writeReg(A_BPLCON1, 0x0000);
  writeReg(A_BPLCON2, 0x0000);
  writeReg(A_BPLCON3, 0x0c00);
  writeReg(A_BPLCON4, 0x0011);
  preloadColours();
  syncHpos();
  // advance to just before the window opens (hpos must reach 0x30); g_hpos
  // is exact at this point (tick() maintains it through every writeReg call
  // above), so this margin is computed, never hand-tuned.
  int margin = 6;
  int target = 0x30 - margin;
  if (g_hpos < target) scanHpos(target - g_hpos);
}

// ---------------------------------------------------------------------------
// measurement scenarios (run once; results consumed by many claim ids)
// ---------------------------------------------------------------------------

struct BpuResult { int measuredPlanes; int rawIndex; bool ok; };
static BpuResult g_bpu[8];              // index by 3-bit BPU code 0..7
static int g_msbIndex[6];               // plane 1..5 isolation index (1-based, [0] unused)
static Px  g_pf1[8], g_pf2[8];          // dual-playfield probe results (unused slots)
static bool g_pf1_ok = false, g_pf2_ok = false;
static bool g_ham_ok = false, g_nonham_ok = false;
static Px g_ham_pix1, g_ham_pix2, g_nonham_pix1, g_nonham_pix2;
static bool g_ehb_raw_ok = false, g_ehb_halved_ok = false, g_ehb_killehb_ok = false;
static Px g_ehb_raw, g_ehb_halved, g_ehb_killehb;
static bool g_bplcon3_effect = false;
static bool g_bplcon2_priority_effect = false;
static double g_span_lores = -1, g_span_hires = -1, g_span_shres = -1;
static bool g_shres_hires0_rule_observed = false;

// COLORxx RAM beyond entry 31 cannot be written in this non-AGA fixture
// (wr_adr's bank field is forced to 0 unless aga=1, denise.v:249, confirmed
// by reading denise.v) -- so a combined index of 32-63 (all 6 planes driven
// at once) reads back whatever this fixture's colour RAM defaults to, not a
// value this harness controls, and cannot be decoded via the COLORn=n
// scheme. For codes needing more than 5 simultaneous planes, this is worked
// around with a DIFFERENTIAL measurement: planes 1-5 driven (a decodable,
// known index) versus the same stimulus plus plane 6 also driven -- if
// plane 6 is gated through, the output must measurably change, even though
// its exact resulting index is not decoded.
static Px stimulateAllMsb(int code, bool includePlane6) {
  baseline(false);
  writeReg(A_BPLCON0, mkBplcon0(false, code, false, false));
  loadPlanes(0x8000, 0x8000, 0x8000, 0x8000, 0x8000, includePlane6 ? 0x8000 : 0);
  auto px = scanHpos(8);
  for (auto& p : px) if (!(p.r == 0 && p.g == 0 && p.b == 0)) return p;
  return Px{0, 0, 0};
}

static void measureBpuTable() {
  for (int code = 0; code < 6; code++) {
    baseline(/*ecs=*/false);
    writeReg(A_BPLCON0, mkBplcon0(false, code, false, false));
    // MSB of each of the (up to 5) plane words = 1: whichever planes l_bpu
    // gates through show up as a set bit in the decoded index (index stays
    // within 0..31, always decodable).
    loadPlanes(0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x0000);
    auto px = scanHpos(8);
    int idx = -1;
    for (auto& p : px) { int d = decodeIndex(p.r, p.g, p.b); if (d > 0) idx = d; }
    if (idx < 0) idx = 0;
    int planes = 0;
    while ((1 << planes) <= idx) planes++;
    bool clean = (idx == (1 << planes) - 1) || idx == 0;
    g_bpu[code] = BpuResult{planes, idx, clean};
  }
  // code 6 and 7 (undefined) both potentially gate a 6th plane through;
  // measured differentially per the note above.
  for (int code = 6; code <= 7; code++) {
    Px without6 = stimulateAllMsb(code, false);
    Px with6 = stimulateAllMsb(code, true);
    bool plane6Effect = !eq(without6, with6);
    int planes = plane6Effect ? 6 : 5;
    g_bpu[code] = BpuResult{planes, with6.r * 65536 + with6.g * 256 + with6.b, plane6Effect};
  }
}

static void measureMsbOrder() {
  for (int plane = 1; plane <= 5; plane++) {
    baseline(false);
    writeReg(A_BPLCON0, mkBplcon0(false, 5, false, false));  // BPU=101 -> 5 planes
    uint16_t w[7] = {0,0,0,0,0,0,0};
    w[plane] = 0x8000;
    loadPlanes(w[1], w[2], w[3], w[4], w[5], w[6]);
    auto px = scanHpos(8);
    int idx = 0;
    for (auto& p : px) { int d = decodeIndex(p.r, p.g, p.b); if (d > 0) idx = d; }
    g_msbIndex[plane] = idx;
  }
}

static void measureDualPlayfield() {
  baseline(false);
  writeReg(A_BPLCON0, mkBplcon0(false, 6, false, true));  // BPU=110, DBLPF=1
  // odd planes (1,3,5) driven, even (2,4,6) held at 0
  loadPlanes(0x8000, 0x0000, 0x8000, 0x0000, 0x8000, 0x0000);
  auto px1 = scanHpos(8);
  g_pf1_ok = false;
  for (auto& p : px1) {
    int d = decodeIndex(p.r, p.g, p.b);
    if (d >= 1 && d <= 7) { g_pf1[0] = p; g_pf1_ok = true; }
  }

  baseline(false);
  writeReg(A_BPLCON0, mkBplcon0(false, 6, false, true));
  // even planes (2,4,6) driven, odd (1,3,5) held at 0
  loadPlanes(0x0000, 0x8000, 0x0000, 0x8000, 0x0000, 0x8000);
  auto px2 = scanHpos(8);
  g_pf2_ok = false;
  for (auto& p : px2) {
    int d = decodeIndex(p.r, p.g, p.b);
    if (d >= 8 && d <= 15) { g_pf2[0] = p; g_pf2_ok = true; }
  }
}

static void measureBplcon2Priority() {
  // both odd and even planes active; toggle bplcon2[6] (pf2pri) and confirm
  // the winning group flips (BPLCON2's own documented function, appendix A
  // c3/c12/A-bplcon2.fn -- "video priority control").
  baseline(false);
  writeReg(A_BPLCON0, mkBplcon0(false, 6, false, true));
  writeReg(A_BPLCON2, 0x0000);  // pf2pri=0 -> PF1 wins when both valid
  loadPlanes(0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000);
  auto pxA = scanHpos(8);
  int dA = -1; for (auto& p : pxA) { int d=decodeIndex(p.r,p.g,p.b); if (d>0) dA=d; }

  baseline(false);
  writeReg(A_BPLCON0, mkBplcon0(false, 6, false, true));
  writeReg(A_BPLCON2, 0x0040);  // bit6 = pf2pri=1 -> PF2 wins
  loadPlanes(0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000);
  auto pxB = scanHpos(8);
  int dB = -1; for (auto& p : pxB) { int d=decodeIndex(p.r,p.g,p.b); if (d>0) dB=d; }

  // PF1-wins index should be in 1..7, PF2-wins index in 8..15, and they differ
  g_bplcon2_priority_effect = (dA >= 1 && dA <= 7) && (dB >= 8 && dB <= 15) && dA != dB;
}

// HAM6: control bits are planes 6,5 (select bits 5,4); data planes 1-4 are
// the payload (denise_hamgenerator.v:80-92, confirmed by reading that file).
// pixel1: control=00 (planes5,6=0,0), index = plane1*1+plane3*4 = 5 -> loads
// COLOR5 (preloaded = 5 -> rgb (0,0,0x55)).
// pixel2: control=01 (plane5=1,plane6=0) = "modify blue"; payload
// plane2=1,plane4=1 -> new blue nibble = 0b1010=0xA -> expect blue=0xAA,
// red/green held from pixel1 (0,0).
// Under HOMOD=0 the same 6-bit pattern is a plain table lookup each pixel:
// pixel2's 6-bit index = plane6*32+plane5*16+plane4*8+plane2*2 = 16+8+2=26
// -> COLOR26 (preloaded = 26 -> green nibble 1, blue nibble 0xA -> rgb
// (0,0x11,0xAA)) -- green differs from the HAM prediction, a clean
// discriminator computed from the manual-external, well documented Amiga
// HAM6 control-bit convention and this RTL's own colour encoding, not a
// constant borrowed from the RTL to manufacture agreement.
static uint16_t hamWord(int pix1bit, int pix2bit) {
  uint16_t w = 0;
  if (pix1bit) w |= 0x8000;
  if (pix2bit) w |= 0x4000;
  return w;
}

static void measureHam(bool homod, Px& pix1, Px& pix2, bool& ok) {
  baseline(false);
  writeReg(A_BPLCON0, mkBplcon0(false, 6, homod, false));
  uint16_t p1 = hamWord(1, 0);  // plane1: pix1=1,pix2=0
  uint16_t p2 = hamWord(0, 1);  // plane2: pix1=0,pix2=1
  uint16_t p3 = hamWord(1, 0);  // plane3: pix1=1,pix2=0
  uint16_t p4 = hamWord(0, 1);  // plane4: pix1=0,pix2=1
  uint16_t p5 = hamWord(0, 1);  // plane5(control lo): pix1=0,pix2=1
  uint16_t p6 = hamWord(0, 0);  // plane6(control hi): pix1=0,pix2=0
  loadPlanes(p1, p2, p3, p4, p5, p6);
  auto px = scanHpos(10);
  // find the first pixel whose colour matches "index 5 raw" (0,0,0x55);
  // the following sampled pixel is the pix2 candidate.
  ok = false;
  for (size_t i = 0; i + 1 < px.size(); i++) {
    if (px[i].r == 0 && px[i].g == 0 && px[i].b == 0x55) {
      pix1 = px[i];
      pix2 = px[i + 1];
      ok = true;
      break;
    }
  }
}

static void measureEhb() {
  // raw (plane6=0, EHB flag off) vs halved (plane6=1) vs killehb override
  baseline(false);
  writeReg(A_BPLCON0, mkBplcon0(false, 6, false, false));
  loadPlanes(0x8000, 0x0000, 0x8000, 0x0000, 0x0000, 0x0000);  // index = 1+4=5, plane6=0
  auto pxRaw = scanHpos(8);
  for (auto& p : pxRaw) if (p.r==0 && (p.g==0) && p.b==0x55) { g_ehb_raw = p; g_ehb_raw_ok = true; }

  baseline(false);
  writeReg(A_BPLCON0, mkBplcon0(false, 6, false, false));
  loadPlanes(0x8000, 0x0000, 0x8000, 0x0000, 0x0000, 0x8000);  // index=5, plane6=1 (EHB flag)
  auto pxHalf = scanHpos(8);
  for (auto& p : pxHalf) if (p.r==0 && p.b>0 && p.b<0x55) { g_ehb_halved = p; g_ehb_halved_ok = (p.b == (uint8_t)(0x55>>1)); }

  // killehb: with ehb_en forced 0 (denise.v:431, ehb_en=!killehb&&...), plane
  // 6's bit stops being masked OUT of the colour-table address (it is only
  // excluded from the address -- denise_colortable.v:34,
  // rd_adr=ehb_en?{3'b0,select[4:0]}:select_xored -- when ehb_en is true) and
  // instead becomes part of it, landing on COLORxx entry 32+, which this
  // non-AGA fixture can never write (bank forced 0 unless aga=1,
  // denise.v:249) and so reads back as whatever this fixture's colour RAM
  // defaults to -- NOT COLOR5's own raw value. The oracle actually available
  // here is therefore negative, not positive: killing EHB must stop the
  // half-brite SHIFT from being applied, i.e. the output must differ from
  // the halved value already measured above, regardless of which address it
  // ends up reading.
  baseline(true);
  writeReg(A_BPLCON2, 0x0200);  // bit9 = KILLEHB
  writeReg(A_BPLCON0, mkBplcon0(false, 6, false, false));
  loadPlanes(0x8000, 0x0000, 0x8000, 0x0000, 0x0000, 0x8000);
  auto pxKill = scanHpos(8);
  g_ehb_killehb_ok = true;
  for (auto& p : pxKill) {
    g_ehb_killehb = p;
    if (eq(p, g_ehb_halved)) g_ehb_killehb_ok = false;  // halving still applied -> killehb failed
  }
}

static void measureBplcon3Effect() {
  // brdrblnk (BPLCON3 bit5) forces the border area to black when ecs=1 and
  // ecsena(BPLCON0 bit0)=1 (denise.v:253,515, confirmed by reading denise.v).
  // Compare the border colour (outside the window, where COLOR00 would
  // otherwise show) with brdrblnk=0 vs brdrblnk=1.
  // no loadPlanes() here: the border-area check depends only on
  // window_del/ecsena/brdrblnk, not on bitplane data, and baseline() already
  // leaves hpos comfortably before DIWSTRT -- calling loadPlanes would push
  // hpos past the window edge (measured during harness development) and
  // sample the wrong (inside-window) region instead.
  // COLOR0 is temporarily overridden to a non-zero value here (this
  // package's COLORn=n encoding otherwise makes COLOR0 itself (0,0,0),
  // indistinguishable from brdrblnk's forced black).
  baseline(true);
  writeReg(A_COLORBASE, 0x0f0f);  // COLOR0 = a distinctive non-zero colour
  writeReg(A_BPLCON0, mkBplcon0(false, 1, false, false, false, false, false, false, false, true));  // ecsena=1
  writeReg(A_BPLCON3, 0x0c00);  // brdrblnk=0 (bit5=0)
  auto pxOff = scanHpos(4);  // still before window (border area)

  baseline(true);
  writeReg(A_COLORBASE, 0x0f0f);
  writeReg(A_BPLCON0, mkBplcon0(false, 1, false, false, false, false, false, false, false, true));
  writeReg(A_BPLCON3, 0x0c20);  // brdrblnk=1 (bit5=1)
  auto pxOn = scanHpos(4);

  bool anyDiff = false;
  for (size_t i = 0; i < pxOff.size() && i < pxOn.size(); i++)
    if (!eq(pxOff[i], pxOn[i])) anyDiff = true;
  g_bplcon3_effect = anyDiff;
}

// span-based rate measurement (category HIRES/SHRES): load a 16-bit
// alternating pattern on plane 1 only, and measure how many 28MHz clocks
// elapse between the first and last observed colour-index transition. No
// RTL-derived pixel-width constant is used -- only observed transition
// positions in the sampled sequence.
static double measureSpan(bool hires, bool shres, bool ecs) {
  baseline(ecs);
  writeReg(A_BPLCON0, mkBplcon0(hires, 1, false, false, false, shres));
  loadPlanes(0xAAAA);
  auto px = scanClk(400);
  int firstT = -1, lastT = -1;
  int prev = decodeIndex(px[0].r, px[0].g, px[0].b);
  for (size_t i = 1; i < px.size(); i++) {
    int cur = decodeIndex(px[i].r, px[i].g, px[i].b);
    if (cur != prev && cur >= 0 && prev >= 0) {
      if (firstT < 0) firstT = (int)i;
      lastT = (int)i;
    }
    if (cur >= 0) prev = cur;
  }
  if (firstT < 0) return -1;
  return (double)(lastT - firstT);
}

// the mandatory control case / smoke test: reproduces PACKAGE.md's own
// MEASURED walk-through verbatim (DIWSTRT H=$30, DIWSTOP H=$50, BPU=1,
// BPL1DAT=$FF00 -> COLOR01 inside the window, COLOR00 outside it). This is
// the first thing this harness checks, and every other category's "known
// value" method rests on it passing.
static bool g_control_ok = false;
static std::string g_control_msg;

static void measureControl() {
  dut->ecs = false; dut->aga = false; dut->a1k = false; dut->blank = 0;
  writeReg(A_DIWSTRT, 0x0030);
  writeReg(A_DIWSTOP, 0x0050);
  writeReg(A_DIWHIGH, 0x0000);
  writeReg(A_BPLCON1, 0x0000);
  writeReg(A_BPLCON2, 0x0000);
  writeReg(A_BPLCON3, 0x0c00);
  writeReg(A_BPLCON4, 0x0011);
  preloadColours();
  writeReg(A_BPLCON0, mkBplcon0(false, 1, false, false));  // BPU=1
  syncHpos();
  // advance close to the window edge first, so the loaded byte's visible
  // pixels land inside DIWSTRT..DIWSTOP rather than shifting out beforehand
  // (measured during harness development: without this, the load can
  // complete and finish shifting before hpos ever reaches DIWSTRT at all).
  int target = 0x30 - 10;
  if (g_hpos < target) scanHpos(target - g_hpos);
  loadPlanes(0xFF00);  // planes 2-6 = 0

  auto seq = scanHposTagged(0x70 - 2);
  bool sawInside = false;
  bool cleanBefore = true, cleanAfter = true;
  for (auto& hp : seq) {
    int idx = decodeIndex(hp.px.r, hp.px.g, hp.px.b);
    if (hp.hpos < 0x30) { if (!(idx == 0)) cleanBefore = false; }
    else if (hp.hpos < 0x50) { if (idx == 1) sawInside = true; }
    else { if (!(idx == 0)) cleanAfter = false; }
  }
  g_control_ok = sawInside && cleanBefore && cleanAfter;
  g_control_msg = std::string("border-before=") + (cleanBefore ? "COLOR00" : "WRONG") +
                  ", window=" + (sawInside ? "saw COLOR01" : "never saw COLOR01") +
                  ", border-after=" + (cleanAfter ? "COLOR00" : "WRONG");
}

static void measureRates() {
  g_span_lores = measureSpan(false, false, true);
  g_span_hires = measureSpan(true, false, true);
  g_span_shres = measureSpan(false, true, true);
}

// bit-sweep "no observable effect" helper, used by UNUSED_OCS, ECS_REASSIGN
// (bit4 under ecs=1) and BPLCON2_UNUSED categories.
static bool bplcon0BitHasEffect(int bit, bool ecs) {
  // Two planes (1 and 2, one odd/one even) with different data, not one:
  // a single-plane baseline cannot reveal a DBLPF flip (the odd-only PF1
  // index and the plain single-playfield index coincide when only plane 1
  // carries data), which would make this check vacuously green regardless
  // of what a bit actually does. BPU=2 exercises the single-vs-dual
  // playfield split, priority and BPU field together.
  baseline(ecs);
  writeReg(A_BPLCON0, mkBplcon0(false, 2, false, false));
  loadPlanes(0xAAAA, 0x5555);
  auto base = scanHpos(20);

  baseline(ecs);
  writeReg(A_BPLCON0, (uint16_t)(mkBplcon0(false, 2, false, false) | (1u << bit)));
  loadPlanes(0xAAAA, 0x5555);
  auto withBit = scanHpos(20);

  for (size_t i = 0; i < base.size() && i < withBit.size(); i++)
    if (!eq(base[i], withBit[i])) return true;
  return false;
}

static bool bplcon2BitHasEffect(int bit) {
  // Single-playfield/BPU=1 (as used by bplcon0BitHasEffect) barely consults
  // BPLCON2 at all -- pf2pri and pf2of only matter in dual-playfield mode,
  // and the one single-playfield BPLCON2 read (the undocumented pf2p>5 SWIV
  // quirk in denise_playfields.v) needs plane 5 driven. A BPU=1 baseline
  // would make this check vacuously green regardless of what a bit does, so
  // this uses DBLPF=1/BPU=6 with both playfields driven instead, matching
  // the DBLPF category's own stimulus (denise.v's actual consumers of
  // bplcon2 are exercised this way).
  baseline(false);
  writeReg(A_BPLCON0, mkBplcon0(false, 6, false, true));
  loadPlanes(0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000);
  auto base = scanHpos(20);

  baseline(false);
  writeReg(A_BPLCON2, (uint16_t)(1u << bit));
  writeReg(A_BPLCON0, mkBplcon0(false, 6, false, true));
  loadPlanes(0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000);
  auto withBit = scanHpos(20);

  for (size_t i = 0; i < base.size() && i < withBit.size(); i++)
    if (!eq(base[i], withBit[i])) return true;
  return false;
}

// ---------------------------------------------------------------------------
#include "claims_gen.inc"
// ---------------------------------------------------------------------------

int main(int argc, char** argv) {
  Verilated::commandArgs(argc, argv);
  dut = new Vdenise;
  dut->clk = 0; dut->reset = 0; dut->strhor = 0;
  dut->reg_address_in = 0; dut->data_in = 0; dut->chip48 = 0;
  dut->blank = 0; dut->a1k = 0; dut->ecs = 0; dut->aga = 0;
  g_phase = 0;
  resetPulse();

  std::printf("=== ch3-bplcon0-display-modes: running measurement scenarios ===\n");
  measureControl();
  std::printf("control case: %s\n", g_control_msg.c_str());
  measureBpuTable();
  measureMsbOrder();
  measureDualPlayfield();
  measureBplcon2Priority();
  measureHam(true, g_ham_pix1, g_ham_pix2, g_ham_ok);
  measureHam(false, g_nonham_pix1, g_nonham_pix2, g_nonham_ok);
  measureEhb();
  measureBplcon3Effect();
  measureRates();

  std::printf("=== ch3-bplcon0-display-modes: assertions ===\n");
  runAllClaims();

  std::printf("=== ch3-bplcon0-display-modes: %d/%d PASS, %d FAIL ===\n",
              g_pass, g_total, g_fail);

  delete dut;
  return 0;
}
