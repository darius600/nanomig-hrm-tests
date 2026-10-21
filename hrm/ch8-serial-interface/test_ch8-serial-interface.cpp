// Test purpose: grade the 106 claims of
//   hrm-testpipeline/work/ch8-s4-build/ch8-serial-interface.jsonl at the
//   `paula_uart` module boundary, compiled alone. Subject: the chapter's own
//   prose on pp.255-259 -- programmable rate and length, overrun, the three
//   status-bit families, the receive path down to SERDATR, the transmit path
//   down to SERDAT, and the two shift-register figures -- rather than one
//   register's bit table in isolation.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 255-259; committed specification (SPECIFICATION.md, this package).
// Claims file: hrm-testpipeline/work/ch8-s4-build/ch8-serial-interface.jsonl
//   (106 claims; 97 uids graded via expect(), 9 uids declined via record()
//   -- see SPECIFICATION.md's "Claim inventory" for the full accounting).
// Non-goals: anything outside the `paula_uart` fixture this package builds,
//   and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s4-build/ch8-serial-interface.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// Order-of-work disclosure (see SPECIFICATION.md for the full statement):
// this package's own claims file embeds paula_uart.v port names and line
// numbers in every classify.evidence field, so they were visible before
// SPECIFICATION.md was written. SPECIFICATION.md was still written and
// committed from the printed pages alone, with no expected value taken from
// that evidence field. paula_uart.v (261 lines) and amiga_clk.v (lines
// 1-45) were then read in full for this file, after that commit.
//
// RTL facts established by reading paula_uart.v (none shaped
// SPECIFICATION.md, which was committed first):
//   - No read/write strobe on the register bus: rga_i (8 bits, = byte
//     address >> 1) selects a register on every clk7_en cycle regardless of
//     software's read/write intent. SERPER (:52) and SERDAT (:61) write
//     unconditionally on an rga_i match -- addressing either "to read" is a
//     write of whatever data_i currently holds. SERDATR has no write path
//     at all; addressing it never has a side effect, so it is always safe
//     to hold rga_i at SERDATR's address for continuous, side-effect-free
//     polling of data_o (used throughout below to watch TSRE without
//     disturbing SERPER/SERDAT).
//   - Bit-cell length: tx_cnt reloads to {serper[14:0],1'b1} = 2N+1 and
//     counts down once per clk7_en to 0 before the next bit's output
//     changes (:100-124) -- N+1 output-bit intervals of 2N+2 clk7_en beats
//     each. Measured directly below (measure_cell_beats()), never assumed.
//   - Shift-until-last-one: tx_shift <= serdat at the start of a frame
//     (:100); each clk7_en it right-shifts, zero-filling from the top
//     (:122), and outputs the OLD bit0 (the bit about to be dropped) as
//     txd for the upcoming interval (:123). The frame ends (TSRE, :113-116)
//     only once the shift register reads all-zero -- i.e. once the
//     *highest* set bit has been shifted out, not the first.
//   - Receive frame layout, traced by hand from paula_uart.v:156-251
//     (RX_IDLE/RX_START/RX_SHIFT) and confirmed against the printed table:
//     8-bit mode (LONG=serper[15]=0) needs 10 bit-cells (1 start + 8 data +
//     1 stop-slot) and data_o[7:0] carries the 8 data bits LSB-first,
//     data_o[8] carries the sampled stop-slot line level; 9-bit mode
//     (LONG=1) needs 11 cells and data_o[8:0] carries the 9 data bits
//     LSB-first (bit 8 = 9th/last), data_o[9] carries the sampled
//     stop-slot line level. This matches Table 8-9's STP/DB8 row exactly.
//   - SERDATR bit 11 (RXD) is `rx_rxd`, itself `rxds` (a 2-flop
//     synchroniser output, :36-43) registered one further cycle (:234) --
//     three clk7_en cycles of latency from a pin change to this bit
//     reflecting it, not zero.
//   - amiga_clk.v:34-38: `cck` toggles once per clk7_en pulse, so one full
//     colour-clock period is exactly two clk7_en beats -- a fact about a
//     different module, not a port of paula_uart, and not independently
//     confirmed by this package (see SPECIFICATION.md sections A16-A18).
//
// Every mutation below is a compile-time variant, replayable with
// `make clean && make MUT=MUT_n test` (n = 0..49, matching the MUT_n token
// at the end of each record line below) -- see the `#if MUT_n` guard at
// each assertion's stimulus/oracle site. `make clean` first is required:
// the object-directory target has no dependency on MUT, so a stale binary
// from a previous variant would otherwise be silently reused (the `test`
// recipe below also does an unconditional `rm -rf obj_dir` for the same
// reason). Every mutation was actually run this way; each was then reverted
// (rebuilt with MUT unset) and the baseline reconfirmed before the next.
// Three assertions (BIT11-RXD-DIRECT, RX-DEADLINE-8TO10, TX-ALLZERO) are RED
// at baseline -- their mutations are `oracle green` flips (the only
// demonstration meaningful at a red baseline, per the build brief), proving
// the assertion mechanism is sound and the red is not a broken test. Every
// other assertion is green at baseline and its mutation is a `stimulus red`
// or `oracle red` flip.
//
// Machine-readable mutation records, one per MUT_n above, in the form
// `MUTATION <assertion-id> <oracle|stimulus> <red|green> <what changed> <MUT_n>`.
// <assertion-id> is the leading token of the expect() label the mutation's
// assertion carries, not the letter-scheme name used in this file's own prose
// comments above and at each site -- the two differ for every lettered claim
// (e.g. B11's id is opus:8-serial.c54, C4's is opus:8-serial.c81, E6's is
// opus:8-serial.c140); only the four unlettered fixture/measurement-basis
// assertions (FIXTURE-LIVE, CONTROL, DIVIDER-SPAN, CELL-LENGTH-FORMULA) use
// their own name as the id.
// MUTATION FIXTURE-LIVE stimulus red probe_n changed from 10 to 5, the wrong SERPER divisor, so the fixture no longer measures the expected 22-beat cell MUT_0
// MUTATION CONTROL stimulus red cb3b reuses cb3's own SERPER=3 measurement instead of measuring the second, distinct divisor 371 MUT_1
// MUTATION DIVIDER-SPAN stimulus red cb_min measured at SERPER=1 instead of the true minimum SERPER=0 MUT_2
// MUTATION opus:8-serial.c10 stimulus red send8_val changed to 0x0055 (no bit 8 set), not a valid 8-length stop pattern MUT_3
// MUTATION opus:8-serial.c34 stimulus red rx8 driven with 0x5C instead of the known byte 0xA3 MUT_4
// MUTATION opus:8-serial.c35 stimulus red rx9 driven with 0x0D5 instead of the known 9-bit value 0x1A3 MUT_5
// MUTATION sol:8-serial.c10 stimulus red t8 transmitted with SERDAT=0x0055 (no bit 8 set) instead of 0x0155 MUT_6
// MUTATION sol:8-serial.c11 stimulus red t9 transmitted with SERDAT=0x0155 (8-length) instead of 0x0255 (9-length) MUT_7
// MUTATION opus:8-serial.c12 stimulus red f2 cleared empty, only one frame driven instead of a second to trigger overrun MUT_8
// MUTATION opus:8-serial.c13 stimulus red rbfmirror left clear (0) instead of set (1) before the second frame MUT_9
// MUTATION sol:8-serial.c14 oracle red rbf_ok compares (after_rx & BIT_RBF) == 0 instead of != 0, the wrong expected polarity MUT_10
// MUTATION opus:8-serial.c18 stimulus red cbB re-measured at the same divisor (3) as cbA instead of a second, distinct one (17) MUT_11
// MUTATION opus:8-serial.c19 stimulus red cb_hi measured at SERPER=6, a different low-14 value, instead of 5|0x8000 MUT_12
// MUTATION opus:8-serial.c25 oracle red shape_ok checks cb9 == 2*(9+1)+1, an off-by-one oracle, instead of 2*(9+1) MUT_13
// MUTATION CELL-LENGTH-FORMULA oracle red the per-N comparison adds +2 to the expected cell length only when n==100, a wrong formula for that divisor MUT_14
// MUTATION opus:8-serial.c37 oracle red expect_glitch_rxint hardcoded true instead of false, claiming a glitch should register MUT_15
// MUTATION opus:8-serial.c38 stimulus red rx9 built with only 8 data bits (lsb_first_bits(0x0BC, 8)) instead of 9, in 9-bit mode MUT_16
// MUTATION opus:8-serial.c39 oracle red expected_cells hardcoded to 9 instead of 10, omitting the stop-bit slot MUT_17
// MUTATION opus:8-serial.c40 oracle red polarity_ok compares got9 against the 8-bit mask/value (0x00FF/0x2D) instead of 0x01FF/0x1B4 MUT_18
// MUTATION opus:8-serial.c41 oracle red readback hardcoded to 44 instead of the actually probed value, as if SERPER's last write came back on read MUT_19
// MUTATION opus:8-serial.c45 oracle red status_distinct compares (got & 0xFF00) == 0 instead of != 0, asserting the status half is empty MUT_20
// MUTATION opus:8-serial.c48 stimulus red the low-rxd hold extended from cb*3 (well short of a frame) to cb*12 (a full frame's worth) MUT_21
// MUTATION opus:8-serial.c49 stimulus red saw_rxint hardcoded false and drive_receive(h, f, cb) is never called, so no receive frame is driven at all MUT_22
// MUTATION opus:8-serial.c52 oracle red expected_low_byte hardcoded to 0x12 (frame 1's byte) instead of 0x87 (frame 2's) MUT_23
// MUTATION opus:8-serial.c53 stimulus red the rbfmirror-clear servicing step before frame 2 is removed entirely, so rbfmirror is never cleared MUT_24
// MUTATION opus:8-serial.c54 oracle green ceiling widened from the printed 10 to 11, matching the measured 9-bit-mode reality; oracle-green flip at this assertion's red baseline MUT_25
// MUTATION opus:8-serial.c55 stimulus red f2 left empty, no second frame driven, so overrun cannot be observed to set MUT_26
// MUTATION opus:8-serial.c59 oracle red probe_bit set to BIT_TSRE instead of BIT_OVRUN, the wrong bit position MUT_27
// MUTATION opus:8-serial.c65 oracle red probe_bit set to BIT_TBE instead of BIT_RBF, the wrong bit position MUT_28
// MUTATION opus:8-serial.c70 oracle red idle_tbe compares (at_idle & BIT_TBE) == 0 instead of != 0, the wrong expected polarity MUT_29
// MUTATION opus:8-serial.c76 oracle red clear_check_ok negated to !saw_clear_mid_shift, expecting TSRE never clears mid-shift MUT_30
// MUTATION opus:8-serial.c79 oracle red watch_cells shortened from 9 to 3, far too short a window, stopping before word 1 alone would finish MUT_31
// MUTATION opus:8-serial.c81 oracle green direct_ok hardcoded true, reading the manual's "direct" loosely with latency ignored; oracle-green flip at this assertion's red baseline MUT_32
// MUTATION opus:8-serial.c83 oracle red mask set to BIT_9 instead of BIT_10, checking the wrong bit MUT_33
// MUTATION opus:8-serial.c84 oracle red bit8_9bit_ok compares got9's bit 9 against its own bit 8 instead of against the written 9th data bit MUT_34
// MUTATION opus:8-serial.c91 stimulus red f driven with the bitwise-inverted byte (~0x96 & 0xFF) instead of 0x96 MUT_35
// MUTATION opus:8-serial.c103 oracle red write_reaches_state ANDed with a literal false, forcing the result regardless of t1's actual measurement MUT_36
// MUTATION opus:8-serial.c104 stimulus red the actual h.read_reg(ADDR_SERDAT) bus read is skipped entirely; readback hardcoded to 0x0177 (the written value) instead MUT_37
// MUTATION opus:8-serial.c105 oracle red same_rate compares cb_rx == 9 instead of == 10, the wrong expected frame length MUT_38
// MUTATION opus:8-serial.c106 oracle red begins_promptly hardcoded false, ignoring the measured beats_to_start value MUT_39
// MUTATION opus:8-serial.c108 stimulus red the second word is written immediately (as soon as TBE reads 1, same timing as baseline) but as 0x0066 instead of 0x0266 -- its highest set bit is 6, so pp.258-259's own rule shifts 7 data bits (8 cells) instead of 10, giving 18 combined cells, not 21, with every beat still inside the counted window MUT_40
// MUTATION opus:8-serial.c110 stimulus red t transmitted with SERDAT=0x00AA (MSB-heavy) instead of 0x0155, checked against the unchanged LSB-first oracle MUT_41
// MUTATION opus:8-serial.c112 oracle red expected_cells hardcoded to 4 instead of 11, expecting the shift to stop early at the first 1-bit MUT_42
// MUTATION opus:8-serial.c133 oracle red expected_t2_cells hardcoded to 7 instead of 10 MUT_43
// MUTATION opus:8-serial.c113 oracle red shapes_differ compares ta.bits.size() == tb.bits.size() instead of !=, expecting the two differently-shaped frames to match MUT_44
// MUTATION opus:8-serial.c114 stimulus red t transmitted with SERDAT=0x0001 instead of 0x8001, bit 15 never exercised MUT_45
// MUTATION opus:8-serial.c119 oracle red idle_ok compares (idle_status & BIT_TSRE) == 0 instead of != 0, the wrong expected TSRE polarity MUT_46
// MUTATION opus:8-serial.c136 oracle red expected_diff hardcoded to 0 instead of 1, the fixture-hazard-3 trap of expecting the level pattern to look the same MUT_47
// MUTATION opus:8-serial.c137 oracle red start_always_zero expects the start cell to follow bit 0's own value instead of always being 0 MUT_48
// MUTATION opus:8-serial.c139 stimulus red t transmitted with SERDAT=0x0000 (all-zero, E6's case) instead of the positive-case 0x0188 MUT_49
// MUTATION opus:8-serial.c140 oracle green expect_no_transmission hardcoded false, matching the measured reality that txd does go low; oracle-green flip at this assertion's red baseline MUT_50
//
// Stimulus mutations added after the fact (25 assertions that previously
// carried only an oracle mutation, found by tools/check_gates.py's
// check_mutations gate): each changes what is driven into the fixture --
// never the expected-value computation -- and was built and run individually
// (`make clean && make MUT=MUT_n test`), confirmed to flip only its own
// assertion, then reverted before the next. 24 of the 25 had a writable
// stimulus; one (opus:8-serial.c83) does not -- see its `none impossible`
// record below.
// MUTATION CELL-LENGTH-FORMULA stimulus red for n=100, the divisor actually driven is 55 instead of 100, while the comparison stays literally against n=100 (2*(100+1)=202 != measured 2*(55+1)=112) MUT_51
// MUTATION opus:8-serial.c103 stimulus red t1 transmitted with SERDAT=0x0033 (highest bit at 5, a 7-cell shape) instead of 0x0133 (highest bit at 8, the 10-cell shape the oracle still requires) MUT_52
// MUTATION opus:8-serial.c105 stimulus red one extra write (SERPER=25|0x8000) issued before the sample, switching receive to 9-bit mode so the frame measures 11 cells, not the 10 the oracle still requires MUT_53
// MUTATION opus:8-serial.c106 stimulus red data_i written as 0x0188 instead of 0x0189 -- bit 0 of the written word is now 0, so the second sampled cell no longer reads 1 MUT_54
// MUTATION opus:8-serial.c112 stimulus red t transmitted with SERDAT=0x0005 (bit 9 dropped, highest set bit now at 2) instead of 0x0205, a genuinely 4-cell frame against the oracle's still-required 11 MUT_55
// MUTATION opus:8-serial.c113 stimulus red tb transmitted with SERDAT=0x0021 (highest bit at 5, same shape as ta) instead of 0x0709 (highest bit at 10) -- the two frames no longer differ MUT_56
// MUTATION opus:8-serial.c119 stimulus red one extra write (SERDAT=0x0155) issued before the idle sample, leaving a transmission in flight so TSRE is no longer set at the checkpoint MUT_57
// MUTATION opus:8-serial.c133 stimulus red t2 transmitted with SERDAT=0x0066 (highest bit at 6, an 8-cell shape) instead of 0x0166 (highest bit at 8, the 10-cell shape the oracle still requires) MUT_58
// MUTATION opus:8-serial.c136 stimulus red two_stop transmitted with SERDAT=0x0155 (one-stop shape, same as one_stop) instead of 0x0355 (two-stop shape) -- the two frames no longer differ by one cell MUT_59
// MUTATION opus:8-serial.c137 stimulus red bit0_zero transmitted with SERDAT=0x0157 (bit 0 = 1) instead of 0x0154 (bit 0 = 0) -- the second sampled cell no longer reads 0 as the oracle still requires MUT_60
// MUTATION opus:8-serial.c25 stimulus red cb9 measured by driving SERPER=8 instead of 9, while the comparison stays literally against 2*(9+1)=20 MUT_61
// MUTATION opus:8-serial.c37 stimulus red the genuine frame truncated to 4 data bits (lsb_first_bits(0x5A,4)) instead of 8 -- too short for the DUT to complete, so genuine_rxint never fires MUT_62
// MUTATION opus:8-serial.c39 stimulus red one extra write (SERPER=10|0x8000) issued before the sample, switching receive to 9-bit mode so f_slot1's frame measures 11 cells, not the 10 the oracle still requires MUT_63
// MUTATION opus:8-serial.c40 stimulus red rx9 driven with 0x1B5 instead of the known 9-bit value 0x1B4, while the comparison stays literally against 0x1B4 MUT_64
// MUTATION opus:8-serial.c41 stimulus red cb2 measured by driving SERPER=45 instead of 44, while the comparison stays literally against 90 (2*44+2) MUT_65
// MUTATION opus:8-serial.c45 stimulus red f driven with byte 0x7D instead of 0x7E, while the comparison stays literally against 0x7E MUT_66
// MUTATION opus:8-serial.c52 stimulus red f2 driven with byte 0x88 instead of 0x87, while the comparison stays literally against 0x87 MUT_67
// MUTATION opus:8-serial.c59 stimulus red rbfmirror left clear (0) instead of set (1) before frame 2, so the overrun condition this assertion probes never arms MUT_68
// MUTATION opus:8-serial.c65 stimulus red rbfmirror left clear (0) instead of set (1) -- RBF (rx_rbf) is a direct mirror of that pin (paula_uart.v:246), not a consequence of the frame completing, so RBF never reads set MUT_69
// MUTATION opus:8-serial.c70 stimulus red t transmitted with SERDAT=0x8001 (a 17-cell frame) instead of 0x0155, against the unchanged 16-cell capture window, so the frame never completes MUT_70
// MUTATION opus:8-serial.c76 stimulus red the SERDAT write omitted entirely -- no transmission is triggered, so TSRE cannot be observed to clear mid-shift MUT_71
// MUTATION opus:8-serial.c79 stimulus red the second write retargeted to SERDATR (no write path at all, paula_uart.v) instead of SERDAT -- no second word ever loads, so TSRE sets within the watch window on word 1 alone instead of staying clear through both MUT_72
// MUTATION opus:8-serial.c84 stimulus red rx9 driven with 0x1D4 instead of 0x0D4 -- the 9th bit actually presented is now 1, while the comparison stays literally against 0x0D4's own bit 8 (0) MUT_73
// MUTATION sol:8-serial.c14 stimulus red rbfmirror left clear (0) instead of set (1) before the frame -- RBF (rx_rbf) is a direct mirror of that pin (paula_uart.v:246), not a consequence of the frame completing, so RBF never reads set MUT_74
// MUTATION opus:8-serial.c83 none impossible: SERDATR bit 10 is `1'b0`, a hardcoded literal in paula_uart.v:272's `data_o` bus mux (`{serdatr, 1'b0, rx_data}`), not a register or wire fed by any port -- no combination of SERPER, SERDAT, rxd or rbfmirror can ever make it read nonzero, so no stimulus mutation can turn this assertion red. Same shape as hrm/ch8-keycode-table's CUTOUT-30.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include "Vpaula_uart.h"
#include "verilated.h"

// ---------------------------------------------------------------------
// MUT_n scaffolding. Pass MUT=MUT_n on the command line to build one
// mutation variant; unset/empty MUT builds the unmutated baseline.
// ---------------------------------------------------------------------
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
#ifndef MUT_5
#define MUT_5 0
#endif
#ifndef MUT_6
#define MUT_6 0
#endif
#ifndef MUT_7
#define MUT_7 0
#endif
#ifndef MUT_8
#define MUT_8 0
#endif
#ifndef MUT_9
#define MUT_9 0
#endif
#ifndef MUT_10
#define MUT_10 0
#endif
#ifndef MUT_11
#define MUT_11 0
#endif
#ifndef MUT_12
#define MUT_12 0
#endif
#ifndef MUT_13
#define MUT_13 0
#endif
#ifndef MUT_14
#define MUT_14 0
#endif
#ifndef MUT_15
#define MUT_15 0
#endif
#ifndef MUT_16
#define MUT_16 0
#endif
#ifndef MUT_17
#define MUT_17 0
#endif
#ifndef MUT_18
#define MUT_18 0
#endif
#ifndef MUT_19
#define MUT_19 0
#endif
#ifndef MUT_20
#define MUT_20 0
#endif
#ifndef MUT_21
#define MUT_21 0
#endif
#ifndef MUT_22
#define MUT_22 0
#endif
#ifndef MUT_23
#define MUT_23 0
#endif
#ifndef MUT_24
#define MUT_24 0
#endif
#ifndef MUT_25
#define MUT_25 0
#endif
#ifndef MUT_26
#define MUT_26 0
#endif
#ifndef MUT_27
#define MUT_27 0
#endif
#ifndef MUT_28
#define MUT_28 0
#endif
#ifndef MUT_29
#define MUT_29 0
#endif
#ifndef MUT_30
#define MUT_30 0
#endif
#ifndef MUT_31
#define MUT_31 0
#endif
#ifndef MUT_32
#define MUT_32 0
#endif
#ifndef MUT_33
#define MUT_33 0
#endif
#ifndef MUT_34
#define MUT_34 0
#endif
#ifndef MUT_35
#define MUT_35 0
#endif
#ifndef MUT_36
#define MUT_36 0
#endif
#ifndef MUT_37
#define MUT_37 0
#endif
#ifndef MUT_38
#define MUT_38 0
#endif
#ifndef MUT_39
#define MUT_39 0
#endif
#ifndef MUT_40
#define MUT_40 0
#endif
#ifndef MUT_41
#define MUT_41 0
#endif
#ifndef MUT_42
#define MUT_42 0
#endif
#ifndef MUT_43
#define MUT_43 0
#endif
#ifndef MUT_44
#define MUT_44 0
#endif
#ifndef MUT_45
#define MUT_45 0
#endif
#ifndef MUT_46
#define MUT_46 0
#endif
#ifndef MUT_47
#define MUT_47 0
#endif
#ifndef MUT_48
#define MUT_48 0
#endif
#ifndef MUT_49
#define MUT_49 0
#endif
#ifndef MUT_50
#define MUT_50 0
#endif
#ifndef MUT_51
#define MUT_51 0
#endif
#ifndef MUT_52
#define MUT_52 0
#endif
#ifndef MUT_53
#define MUT_53 0
#endif
#ifndef MUT_54
#define MUT_54 0
#endif
#ifndef MUT_55
#define MUT_55 0
#endif
#ifndef MUT_56
#define MUT_56 0
#endif
#ifndef MUT_57
#define MUT_57 0
#endif
#ifndef MUT_58
#define MUT_58 0
#endif
#ifndef MUT_59
#define MUT_59 0
#endif
#ifndef MUT_60
#define MUT_60 0
#endif
#ifndef MUT_61
#define MUT_61 0
#endif
#ifndef MUT_62
#define MUT_62 0
#endif
#ifndef MUT_63
#define MUT_63 0
#endif
#ifndef MUT_64
#define MUT_64 0
#endif
#ifndef MUT_65
#define MUT_65 0
#endif
#ifndef MUT_66
#define MUT_66 0
#endif
#ifndef MUT_67
#define MUT_67 0
#endif
#ifndef MUT_68
#define MUT_68 0
#endif
#ifndef MUT_69
#define MUT_69 0
#endif
#ifndef MUT_70
#define MUT_70 0
#endif
#ifndef MUT_71
#define MUT_71 0
#endif
#ifndef MUT_72
#define MUT_72 0
#endif
#ifndef MUT_73
#define MUT_73 0
#endif
#ifndef MUT_74
#define MUT_74 0
#endif

namespace {

std::string hex4(uint32_t v) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "0x%04X", static_cast<unsigned>(v) & 0xFFFFu);
    return std::string(buf);
}

constexpr int ADDR_SERDATR = 0x018;
constexpr int ADDR_SERDAT  = 0x030;
constexpr int ADDR_SERPER  = 0x032;

constexpr uint16_t BIT_OVRUN = 0x8000;
constexpr uint16_t BIT_RBF   = 0x4000;
constexpr uint16_t BIT_TBE   = 0x2000;
constexpr uint16_t BIT_TSRE  = 0x1000;
constexpr uint16_t BIT_RXD   = 0x0800;
constexpr uint16_t BIT_10    = 0x0400;
constexpr uint16_t BIT_9     = 0x0200;
constexpr uint16_t BIT_8     = 0x0100;

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

// ---------------------------------------------------------------------
// Bus-timing protocol for paula_uart.v, following the pattern already
// verified for this exact fixture by hrm/ch8-uart-frame-and-status (this slice's own
// calibration reference, per the package list): posedge-clk registers
// gated by clk7_en, so rga_i/data_i must be held stable across one full
// clk7_en-active cycle for a write to land; data_o is the combinational
// read mux and needs no extra settling beyond that same cycle.
// ---------------------------------------------------------------------
class Harness {
public:
    Harness() : dut(new Vpaula_uart) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->rga_i = 0;
        dut->data_i = 0;
        dut->uartbrk = 0;
        dut->rbfmirror = 0;
        dut->rxd = 1;  // idle high
        for (int i = 0; i < 32; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; ++i) tick();
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
        clk7_phase = (clk7_phase + 1) % 4;
        dut->clk7_en = (clk7_phase == 0);
    }

    void write_reg(int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->rga_i = (addr >> 1) & 0xFF;
        dut->data_i = data;
        tick(); tick(); tick(); tick();
        dut->rga_i = 0;
        dut->data_i = 0;
    }

    // Safe at any address: paula_uart has no write path triggered by a
    // SERDATR-address match, so this never has a side effect regardless of
    // what data_i currently holds. Used both for genuine SERDATR reads and
    // -- per hazard 1 -- for the deliberate, last-thing-done "does this
    // write-only address read back the value I wrote" probe at SERPER/
    // SERDAT, where the read itself is known to also clobber the register.
    uint16_t read_reg(int addr) {
        while (!dut->clk7_en) tick();
        dut->rga_i = (addr >> 1) & 0xFF;
        tick(); tick(); tick(); tick();
        uint16_t val = dut->data_o;
        dut->rga_i = 0;
        return val;
    }

    Vpaula_uart* dut;
    int clk7_phase = 0;
};

// Measures the bit-cell length in clk7_en beats for a given SERPER divisor,
// directly from the gap between the first two txd edges of a one-data-bit
// frame (idle->start is always a 1->0 edge; start->the single '1' data bit,
// which is also the frame's only nonzero bit and so ends it, is always a
// 0->1 edge one full cell later). Nothing here is assumed from the
// evidence field's own 2N+2 arithmetic -- it is measured fresh, every call.
int measure_cell_beats(Harness& h, uint16_t serper_n) {
    h.write_reg(ADDR_SERPER, serper_n);
    h.write_reg(ADDR_SERDAT, 0x0001);
    int last = h.dut->txd;
    int t = 0, t1 = -1, t2 = -1;
    for (int guard = 0; guard < 300000; ++guard) {
        h.tick();
        if (!h.dut->clk7_en) continue;
        ++t;
        if (h.dut->txd != last) {
            if (t1 < 0) { t1 = t; }
            else { t2 = t; break; }
        }
        last = h.dut->txd;
    }
    const int cell_beats = t2 - t1;
    // Drain this short (2-cell) calibration frame fully before returning,
    // by polling TSRE at the SERDATR address (side-effect-free), so no
    // caller ever measures anything "immediately after a write" against a
    // still-in-flight leftover transmission from this calibration itself.
    h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
    for (int guard = 0; guard < cell_beats * 8 + 400 && (h.dut->data_o & BIT_TSRE) == 0; ) {
        h.tick();
        if (h.dut->clk7_en) ++guard;
    }
    h.dut->rga_i = 0;
    return cell_beats;
}

struct TxResult {
    std::vector<int> bits;         // mid-cell samples, bits[0] = start bit
    int beats_to_tsre = -1;        // clk7_en beats from start-bit onset to TSRE rising
    bool completed = false;
};

// Writes SERPER/SERDAT, then watches txd (content) and data_o's TSRE bit
// (timing) at the SERDATR address -- a side-effect-free continuous poll,
// per hazard 1's own resolution above. Frame length is read off the TSRE
// transition, never off counting trailing same-level cells on txd (hazard
// 3): a run of 1-cells on txd cannot be told apart from idle by level
// alone, but TSRE's own rising edge is a discrete, unambiguous event.
TxResult capture_transmit(Harness& h, uint16_t serper_n, uint16_t serdat_val,
                           int cell_beats, int max_cells = 24) {
    TxResult r;
    h.write_reg(ADDR_SERPER, serper_n);
    h.write_reg(ADDR_SERDAT, serdat_val);
    h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
    int last_txd = h.dut->txd;
    bool started = false;
    int beats = 0, since_cell = 0;
    for (int guard = 0; guard < max_cells * cell_beats * 4 + 2000; ++guard) {
        h.tick();
        if (!h.dut->clk7_en) continue;
        h.dut->eval();
        int cur_txd = h.dut->txd;
        if (!started && last_txd == 1 && cur_txd == 0) {
            started = true;
            beats = 0;
            since_cell = 0;
            r.bits.clear();
        }
        if (started) {
            if (since_cell == cell_beats / 2) r.bits.push_back(cur_txd);
            ++since_cell;
            if (since_cell >= cell_beats) since_cell = 0;
            ++beats;
            bool tsre = (h.dut->data_o & BIT_TSRE) != 0;
            if (tsre) { r.completed = true; r.beats_to_tsre = beats; break; }
            if ((int)r.bits.size() > max_cells) break;
        }
        last_txd = cur_txd;
    }
    h.dut->rga_i = 0;
    return r;
}

// Drives a receive frame bit-by-bit at the measured cell cadence -- the
// same recipe hrm/ch8-uart-frame-and-status (this slice's calibration reference) uses
// successfully for SERPER=10 -- then returns whether rxint pulsed.
bool drive_receive(Harness& h, const std::vector<int>& bits, int cell_beats) {
    bool saw_rxint = false;
    for (int b : bits) {
        h.dut->rxd = b;
        // hold for cell_beats clk7_en-ACTIVE pulses, not cell_beats raw
        // tick() calls (clk7_en is active on only 1 of every 4 ticks).
        for (int c = 0; c < cell_beats; ) {
            h.tick();
            if (h.dut->clk7_en) {
                ++c;
                if (h.dut->rxint) saw_rxint = true;
            }
        }
    }
    h.dut->rxd = 1;
    for (int c = 0; c < cell_beats; ) {
        h.tick();
        if (h.dut->clk7_en) {
            ++c;
            if (h.dut->rxint) saw_rxint = true;
        }
    }
    return saw_rxint;
}

// Drives a receive frame at the measured cadence while counting clk7_en
// beats from the moment rxd is first driven low (start bit onset) until
// rxint pulses, returning that count in whole bit-cells (rounded to the
// nearest cell via integer division against the independently measured
// cell_beats). Used for the character-receive-time claims (A21, B11) where
// the *duration* of the frame, not just its bit content, is the fact under
// test.
int measure_rx_frame_cells(Harness& h, const std::vector<int>& bits, int cell_beats) {
    int beats = 0;
    bool seen_rxint = false;
    for (int b : bits) {
        h.dut->rxd = b;
        for (int c = 0; c < cell_beats && !seen_rxint; ) {
            h.tick();
            if (h.dut->clk7_en) {
                ++beats;
                ++c;
                if (h.dut->rxint) seen_rxint = true;
            }
        }
        if (seen_rxint) break;
    }
    h.dut->rxd = 1;
    if (!seen_rxint) {
        for (int c = 0; c < cell_beats * 3 && !seen_rxint; ) {
            h.tick();
            if (h.dut->clk7_en) {
                ++beats;
                ++c;
                if (h.dut->rxint) seen_rxint = true;
            }
        }
    }
    if (!seen_rxint) return -1;
    // Round to the nearest whole bit-cell rather than truncating: the
    // state machine's own start-bit confirmation window (RX_IDLE's
    // edge-detect latency plus RX_START's N-cycle, not 2N+2-cycle, sample
    // delay -- paula_uart.v:180, :190) is shorter than a full steady-state
    // cell, so a straight floor-division under-counts by one cell on every
    // frame measured this way. Rounding attributes that short first
    // interval to the one bit-time it still occupies.
    return (beats + cell_beats / 2) / cell_beats;
}

// Waits for exactly n_pulses clk7_en-ACTIVE pulses (never n_pulses raw
// tick() calls -- clk7_en is active on only one of every four).
void wait_beats(Harness& h, int n_pulses) {
    for (int c = 0; c < n_pulses; ) {
        h.tick();
        if (h.dut->clk7_en) ++c;
    }
}

// Polls (side-effect-free, via the SERDATR address) until the transmit
// side reports genuinely idle -- TSRE set and txd high -- or gives up
// after max_cells bit-cells. Used before any "immediately after this
// write" measurement, so a still-in-flight prior transmission (e.g. the
// short calibration frame measure_cell_beats leaves behind) cannot
// contaminate the timing being measured.
bool wait_for_tx_idle(Harness& h, int cell_beats, int max_cells = 24) {
    h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
    bool idle = false;
    for (int cell = 0; cell < max_cells && !idle; ++cell) {
        wait_beats(h, cell_beats);
        if (h.dut->txd == 1 && (h.dut->data_o & BIT_TSRE) != 0) idle = true;
    }
    h.dut->rga_i = 0;
    return idle;
}

std::vector<int> lsb_first_bits(uint16_t value, int n) {
    std::vector<int> v;
    for (int i = 0; i < n; ++i) v.push_back((value >> i) & 1);
    return v;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // -------------------------------------------------------------
    { Harness h;
#if MUT_0
      const int probe_n = 5;     // wrong divisor -- 22 no longer the right answer
#else
      const int probe_n = 10;
#endif
      const int cell_beats = measure_cell_beats(h, probe_n);
      checks.expect(cell_beats == 22,
          "FIXTURE-LIVE: with SERPER=10 the fixture delivers a known, "
          "measured bit-cell length of exactly 22 clk7_en beats (2*10+2) "
          "before any other assertion below is trusted [no page -- "
          "infrastructure]");
      checks.note("FIXTURE-LIVE: measured cell_beats(N=" + std::to_string(probe_n) +
                   ") = " + std::to_string(cell_beats));
    }

    // -------------------------------------------------------------
    { Harness h;
      const int cb3 = measure_cell_beats(h, 3);
#if MUT_1
      const int cb3b = cb3;  // no second, distinct divisor measured
#else
      const int cb3b = measure_cell_beats(h, 371);
#endif
      checks.expect(cb3 == 8 && cb3b == 744 && cb3 != cb3b,
          "CONTROL: a hand-computable sanity point independent of the "
          "propositions below -- SERPER=3 must measure an 8-beat cell "
          "(2*3+2) and SERPER=371 a 744-beat cell (2*371+2), distinct from "
          "each other -- proving the measurement basis every timing "
          "assertion below depends on works before those are graded "
          "[no page -- measurement-basis control]");
      checks.note("CONTROL: cell_beats(3)=" + std::to_string(cb3) +
                   " cell_beats(371)=" + std::to_string(cb3b));
    }

    // ===============================================================
    // A. Programmable rate and length (p.255)
    // ===============================================================

    // A1 -- NOT CONSTRUCTIBLE (absolute bits/second needs the colour-clock
    // frequency; hazard 3). Declined and recorded, all four uids.
    // Backed by an untagged DIVIDER-SPAN measurement of the dimensionless
    // residue: the 15-bit divider's own range.
    { Harness h;
#if MUT_2
      const int cb_min = measure_cell_beats(h, 1);   // not the true minimum (N=0)
#else
      const int cb_min = measure_cell_beats(h, 0);
#endif
      const int cb_max = measure_cell_beats(h, 0x7FFF);
      checks.expect(cb_min == 2 && cb_max == 65536 && cb_max == 32768 * cb_min,
          "DIVIDER-SPAN: the 15-bit divisor's shortest (N=0) and longest "
          "(N=0x7FFF) bit cells measure 2 and 65536 clk7_en beats, a "
          "32768:1 span, backing the dimensionless half of the rate-range "
          "claims below [no page -- measurement basis for A1's record()]");
      checks.note("DIVIDER-SPAN: cell_beats(0)=" + std::to_string(cb_min) +
                   " cell_beats(0x7FFF)=" + std::to_string(cb_max));
      checks.record(
          "opus:8-serial.c8 (p.255, floor 110 bits/second), "
          "opus:8-serial.c9 (p.255, ceiling >1,000,000 bits/second), "
          "sol:8-serial.c6 (p.255, floor, sol's wording), "
          "sol:8-serial.c7 (p.255, ceiling, sol's wording): not "
          "constructible at this fixture. Both figures are absolute "
          "bits/second values; paula_uart has no colour-clock port, ntsc "
          "never reaches it, and this design's only colour-clock signal "
          "exists solely as a comment naming the PAL constant, in a "
          "different module (amiga_clk.v:11) -- not a reachable value here "
          "(build brief, fixture hazard 2). The dimensionless residue -- "
          "the divider's own 32768:1 range -- is graded by DIVIDER-SPAN "
          "above. Not graded here.");
    }

    // A2 -- compound (opus only): receive 8, receive 9, send 8-length,
    // send 9-length, all at once. Graded in full as a conjunction of the
    // four measurements A3-A6 use individually.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      // receive 8-bit: known byte 0xA3, LSB-first + start + stop
      std::vector<int> rx8 = {0};
      { auto d = lsb_first_bits(0xA3, 8); rx8.insert(rx8.end(), d.begin(), d.end()); }
      rx8.push_back(1);
      const bool rxint8 = drive_receive(h, rx8, cb);
      const uint16_t rd8 = h.read_reg(ADDR_SERDATR);
      // receive 9-bit: SERPER bit15 set, known 9-bit value 0x1A3
      const int cb9 = measure_cell_beats(h, 10);  // recalibrate after LONG change below
      h.write_reg(ADDR_SERPER, 10 | 0x8000);
      std::vector<int> rx9 = {0};
      { auto d = lsb_first_bits(0x1A3, 9); rx9.insert(rx9.end(), d.begin(), d.end()); }
      rx9.push_back(1);
      const bool rxint9 = drive_receive(h, rx9, cb9);
      const uint16_t rd9 = h.read_reg(ADDR_SERDATR);
      // send 8-length and 9-length: highest set bit at 8 vs 9
#if MUT_3
      const uint16_t send8_val = 0x0055;  // no bit 8 set -- not a valid 8-length stop pattern
#else
      const uint16_t send8_val = 0x0155;
#endif
      TxResult t8 = capture_transmit(h, 10, send8_val, cb, 16);
      TxResult t9 = capture_transmit(h, 10, 0x0255, cb, 16);
      const bool rx_ok = rxint8 && (rd8 & 0x00FF) == 0x00A3 &&
                          rxint9 && (rd9 & 0x01FF) == 0x01A3;
      const bool tx_ok = t8.completed && (int)t8.bits.size() == 10 &&
                          t9.completed && (int)t9.bits.size() == 11;
      checks.expect(rx_ok && tx_ok,
          "opus:8-serial.c10 A2: the UART's data length is programmable to "
          "either eight or nine bits, for both receive and send -- an "
          "eight-bit receive (0xA3), a nine-bit receive (0x1A3 with SERPER "
          "bit 15 set), an effective eight-bit send frame (10 cells) and an "
          "effective nine-bit send frame (11 cells) are all separately "
          "producible [p.255 \"It can receive or send data with a "
          "programmable length of eight or nine bits.\"]");
    }

    // A3/A4 -- receive length, eight and nine bits.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      std::vector<int> rx8 = {0};
#if MUT_4
      { auto d = lsb_first_bits(0x5C, 8); rx8.insert(rx8.end(), d.begin(), d.end()); }
#else
      { auto d = lsb_first_bits(0xA3, 8); rx8.insert(rx8.end(), d.begin(), d.end()); }
#endif
      rx8.push_back(1);
      const bool rxint8 = drive_receive(h, rx8, cb);
      const uint16_t rd8 = h.read_reg(ADDR_SERDATR);
      checks.expect(rxint8 && (rd8 & 0x00FF) == 0x00A3,
          "opus:8-serial.c34 sol:8-serial.c8 sol:8-serial.c19 "
          "sol:8-serial.c23 A3: the receive-register-full threshold can be "
          "set to eight received bits (SERPER bit 15 clear) -- a known "
          "8-bit value (0xA3) is received intact at the documented data "
          "position [p.255 \"...defined either as eight or nine...\", "
          "restated p.255 \"...eight or nine data bits...\"]");
    }
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      h.write_reg(ADDR_SERPER, 10 | 0x8000);
      std::vector<int> rx9 = {0};
#if MUT_5
      { auto d = lsb_first_bits(0x0D5, 9); rx9.insert(rx9.end(), d.begin(), d.end()); }
#else
      { auto d = lsb_first_bits(0x1A3, 9); rx9.insert(rx9.end(), d.begin(), d.end()); }
#endif
      rx9.push_back(1);
      const bool rxint9 = drive_receive(h, rx9, cb);
      const uint16_t rd9 = h.read_reg(ADDR_SERDATR);
      checks.expect(rxint9 && (rd9 & 0x01FF) == 0x01A3,
          "opus:8-serial.c35 sol:8-serial.c9 sol:8-serial.c20 "
          "sol:8-serial.c24 A4: the receive-register-full threshold can be "
          "set to nine received bits (SERPER bit 15 set) -- a known 9-bit "
          "value (0x1A3) is received intact [p.255, same citation as A3, "
          "the nine-bit half]");
    }

    // A5/A6 -- send length, eight and nine bits (no dedicated length-select
    // field on the transmit side -- length follows the position of the
    // highest set bit, per this claim's own split note in SPECIFICATION.md).
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
#if MUT_6
      TxResult t8 = capture_transmit(h, 10, 0x0055, cb, 16);  // no bit 8 -- wrong length
#else
      TxResult t8 = capture_transmit(h, 10, 0x0155, cb, 16);
#endif
      checks.expect(t8.completed && (int)t8.bits.size() == 10,
          "sol:8-serial.c10 A5: the UART can send eight-bit data words -- "
          "SERDAT=0x0155 (highest set bit at position 8) produces a "
          "10-cell frame (1 start + 8 data + 1 stop-slot) [p.255 \"...send "
          "data with a programmable length of eight or nine bits.\"]");
    }
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
#if MUT_7
      TxResult t9 = capture_transmit(h, 10, 0x0155, cb, 16);  // 8-length, not 9
#else
      TxResult t9 = capture_transmit(h, 10, 0x0255, cb, 16);
#endif
      checks.expect(t9.completed && (int)t9.bits.size() == 11,
          "sol:8-serial.c11 A6: the UART can send nine-bit data words -- "
          "SERDAT=0x0255 (highest set bit at position 9) produces an "
          "11-cell frame (1 start + 9 data + 1 stop-slot) [p.255, same "
          "citation as A5, the nine-bit half]");
    }

    // A7/A8 -- overrun detection and its defining condition: a second
    // frame completing while the first is still marked unread (rbfmirror
    // held asserted throughout, mirroring the receiving CPU never clearing
    // INTF_RBF between the two).
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      // rbfmirror stays clear through frame 1: there is no previous
      // unread byte yet, so OVRUN must not set on this, the first
      // completion -- the genuine baseline, not an artificially
      // pre-armed one.
      std::vector<int> f1 = {0}; { auto d = lsb_first_bits(0x11, 8); f1.insert(f1.end(), d.begin(), d.end()); } f1.push_back(1);
#if MUT_8
      std::vector<int> f2 = f1;  // only one frame driven, not a second
      f2.clear();
#else
      std::vector<int> f2 = {0}; { auto d = lsb_first_bits(0x22, 8); f2.insert(f2.end(), d.begin(), d.end()); } f2.push_back(1);
#endif
      drive_receive(h, f1, cb);
      const uint16_t after1 = h.read_reg(ADDR_SERDATR);
      h.dut->rga_i = 0;
      h.dut->rbfmirror = 1;  // now mark it unread, ahead of frame 2
      if (!f2.empty()) drive_receive(h, f2, cb);
      const uint16_t after2 = h.read_reg(ADDR_SERDATR);
      checks.expect((after1 & BIT_OVRUN) == 0 && (after2 & BIT_OVRUN) != 0,
          "opus:8-serial.c12 sol:8-serial.c12 A7: the UART can detect "
          "overrun errors -- a distinct OVRUN indication reads clear after "
          "one frame and sets after a second frame completes while the "
          "first is still marked unread [p.255 \"The UART is capable of "
          "detecting overrun errors...\"]");
    }
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      std::vector<int> f1 = {0}; { auto d = lsb_first_bits(0x33, 8); f1.insert(f1.end(), d.begin(), d.end()); } f1.push_back(1);
      drive_receive(h, f1, cb);
      h.read_reg(ADDR_SERDATR);
      h.dut->rga_i = 0;
#if MUT_9
      h.dut->rbfmirror = 0;  // previous byte no longer marked unread -- not overrun
#else
      h.dut->rbfmirror = 1;
#endif
      std::vector<int> f2 = {0}; { auto d = lsb_first_bits(0x44, 8); f2.insert(f2.end(), d.begin(), d.end()); } f2.push_back(1);
      drive_receive(h, f2, cb);
      const uint16_t after2 = h.read_reg(ADDR_SERDATR);
      checks.expect((after2 & BIT_OVRUN) != 0,
          "opus:8-serial.c13 sol:8-serial.c13 A8: an overrun is exactly "
          "the condition \"another system sends data faster than you "
          "remove it from the data-receive register\" -- OVRUN sets when, "
          "and only demonstrated here when, the previous byte is still "
          "marked unread (rbfmirror held) at the next frame's completion "
          "[p.255, same sentence as A7]");
    }

    // A9/A10/A11 -- the three status bits named on p.255, each shown
    // distinctly observable.
    { Harness h;
      const uint16_t idle = h.read_reg(ADDR_SERDATR);
      const bool idle_ok = (idle & BIT_TBE) != 0 && (idle & BIT_TSRE) != 0 &&
                            (idle & BIT_RBF) == 0;
      const int cb = measure_cell_beats(h, 10);
#if MUT_74
      h.dut->rbfmirror = 0;  // RBF (rx_rbf) is a direct mirror of this pin, paula_uart.v:246 -- left clear, RBF never reads set
#else
      h.dut->rbfmirror = 1;
#endif
      std::vector<int> f = {0}; { auto d = lsb_first_bits(0x5A, 8); f.insert(f.end(), d.begin(), d.end()); } f.push_back(1);
      drive_receive(h, f, cb);
      const uint16_t after_rx = h.read_reg(ADDR_SERDATR);
      h.dut->rga_i = 0;
      h.dut->rbfmirror = 0;
#if MUT_10
      const bool rbf_ok = (after_rx & BIT_RBF) == 0;  // wrong expected polarity
#else
      const bool rbf_ok = (after_rx & BIT_RBF) != 0;
#endif
      TxResult t = capture_transmit(h, 10, 0x0155, cb, 16);
      checks.expect(idle_ok && rbf_ok && t.completed,
          "sol:8-serial.c14 opus:8-serial.c16 sol:8-serial.c16 "
          "sol:8-serial.c18 A9-A11: receive-buffer-full, "
          "transmit-buffer-empty and \"all bits have been shifted out\" "
          "(TSRE) are three distinct, independently observable status "
          "indications -- idle shows TBE and TSRE set and RBF clear, and a "
          "completed receive sets RBF [p.255 \"...status bits and "
          "interrupts for the conditions of receive buffer full and "
          "transmit buffer empty. An additional status bit is provided "
          "that indicates 'all bits have been shifted out'.\"]");
    }

    // A12 -- rate controlled by SERPER's contents (dimensionless: two
    // distinct SERPER values must produce two distinct, measurable
    // bit-cell lengths on the same transmitted data).
    { Harness h;
#if MUT_11
      const int cbA = measure_cell_beats(h, 3);
      const int cbB = measure_cell_beats(h, 3);  // same divisor twice, not two distinct ones
#else
      const int cbA = measure_cell_beats(h, 3);
      const int cbB = measure_cell_beats(h, 17);
#endif
      checks.expect(cbA != cbB,
          "opus:8-serial.c18 A12: the rate of transmission is controlled "
          "by the contents of SERPER -- two distinct SERPER values (3, "
          "17) produce two distinct, measured bit-cell lengths on "
          "otherwise identical stimulus [p.255 \"The rate of transmission "
          "(the baud rate) is controlled by the contents of the register "
          "named SERPER.\"]");
    }

    // A13 -- bits 14-0 are the divider; bit 15 is not (bit 15 is shown
    // elsewhere, A3/A4/B1, to select the receive-length mode instead).
    { Harness h;
      const int cb_lo = measure_cell_beats(h, 5);
#if MUT_12
      const int cb_hi = measure_cell_beats(h, 6);  // a different low-14 value, not just bit15 added
#else
      const int cb_hi = measure_cell_beats(h, 5 | 0x8000);
#endif
      checks.expect(cb_lo == cb_hi,
          "opus:8-serial.c19 A13: bits 14-0 of SERPER are the baud-rate "
          "divider bits -- setting bit 15 on top of an unchanged 14-0 "
          "value (5) does not change the measured bit-cell length, "
          "confirming bit 15 plays no part in the divisor (its role, "
          "selecting receive length, is confirmed separately in A3/A4/B1) "
          "[p.255 \"Bits 14-0 of SERPER are the baud-rate divider bits.\"]");
    }

    // A14 -- the divisor formula's shape (period proportional to N+1, the
    // "-1" rearranged), graded on the dimensionless relationship alone; no
    // frequency value appears in this assertion or its oracle.
    { Harness h;
      const int cb0 = measure_cell_beats(h, 0);
#if MUT_61
      const int cb9 = measure_cell_beats(h, 8);  // wrong divisor driven -- oracle still compares against literal 9
#else
      const int cb9 = measure_cell_beats(h, 9);
#endif
#if MUT_13
      const bool shape_ok = (cb9 == 2 * (9 + 1) + 1);  // off-by-one oracle
#else
      const bool shape_ok = (cb0 == 2 * (0 + 1)) && (cb9 == 2 * (9 + 1));
#endif
      checks.expect(shape_ok,
          "opus:8-serial.c25 A14: the divisor formula's shape -- bit-cell "
          "duration is directly proportional to (divisor+1), with no "
          "additive offset beyond that -- measured at N=0 (2 beats = "
          "2*(0+1)) and N=9 (20 beats = 2*(9+1)) [p.255 \"Thus "
          "SERPER=(3,579,545/baud)-1.\", graded on its dimensionless shape "
          "only -- the frequency itself is not constructible here]");
    }

    // A15 -- NOT CONSTRUCTIBLE. The worked example names an absolute baud
    // rate on ntsc, which this module never receives at all.
    { Harness h;
      const int cb371 = measure_cell_beats(h, 371);
      checks.note("A15: SERPER=371 measures a " + std::to_string(cb371) +
                   "-beat bit cell (= " + std::to_string(cb371 / 2) +
                   " colour-clock units by the N+1 formula) -- reported, "
                   "not compared against 9600 baud, which needs a "
                   "frequency this fixture cannot construct.");
      checks.record(
          "opus:8-serial.c28 (p.255, \"...the proper SERPER value for 9600 "
          "baud on an NTSC machine is (3,579,545/9600)-1=371\"): not "
          "constructible. Names an absolute baud rate on ntsc, a signal "
          "this module never receives at all (grep -nw ntsc paula_uart.v "
          "returns nothing) -- doubly unreachable, not only lacking a "
          "frequency but lacking the very clock domain the example names. "
          "Not graded here.");
    }

    // A16/A17/A18 -- partially constructible. The proportional,
    // whole-multiple-of-a-fixed-unit structure is graded on one shared,
    // untagged measurement; the identification of that unit with one
    // colour clock specifically needs a signal (cck) that exists only in
    // a different module (amiga_clk), not a port of paula_uart, and is not
    // independently confirmed by this package.
    { Harness h;
      bool all_even = true;
      bool all_match_formula = true;
      const int test_ns[] = {0, 3, 10, 100, 371};
      for (int n : test_ns) {
#if MUT_51
        const int cb = measure_cell_beats(h, static_cast<uint16_t>(n == 100 ? 55 : n));  // wrong divisor driven for n=100, oracle still compares against n itself
#else
        const int cb = measure_cell_beats(h, static_cast<uint16_t>(n));
#endif
        if (cb % 2 != 0) all_even = false;
#if MUT_14
        if (cb != 2 * (n + 1) + (n == 100 ? 2 : 0)) all_match_formula = false;
#else
        if (cb != 2 * (n + 1)) all_match_formula = false;
#endif
      }
      checks.expect(all_even && all_match_formula,
          "CELL-LENGTH-FORMULA: for every divisor tested (0, 3, 10, 100, "
          "371), the measured bit-cell length is exactly 2*(N+1) clk7_en "
          "beats -- always an even number, i.e. counted in matched pairs, "
          "consistent with (but not by itself proof of) each pair being "
          "one colour clock [no page -- measurement basis for A16/A17/A18's "
          "record() lines]");
      checks.record(
          "opus:8-serial.c20 A16 (p.255, \"All timing is done on the basis "
          "of a 'color clock'...\"): partially graded. paula_uart has no "
          "colour-clock port (amiga_clk.v owns `cck`, a different module); "
          "the whole-multiple-of-a-fixed-unit structure is graded above, "
          "the colour-clock identification is not. Not graded here.");
      checks.record(
          "opus:8-serial.c23 A17 (p.255, \"...N+1 color clocks occur "
          "between samples of the state of the input pin (for "
          "receive)...\"): partially graded, same basis as A16 -- the "
          "proportional (N+1) relationship is graded above; which fixed "
          "unit it is measured in is not confirmable at this boundary. "
          "Not graded here.");
      checks.record(
          "opus:8-serial.c24 A18 (p.255, \"...or between transmissions of "
          "output bits (for transmit)\"): partially graded, same basis as "
          "A16/A17. Not graded here.");
    }

    // A19 -- receive expects exactly one start bit: a glitch that does not
    // persist through the sampling point is rejected; a genuine, held
    // start bit is accepted.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      h.dut->rxd = 0;
      h.tick();  // one-beat glitch
      h.dut->rxd = 1;
      bool glitch_rxint = false;
      for (int c = 0; c < cb * 3; ) {
        h.tick();
        if (h.dut->clk7_en) { ++c; if (h.dut->rxint) glitch_rxint = true; }
      }
#if MUT_62
      std::vector<int> genuine = {0}; { auto d = lsb_first_bits(0x5A, 4); genuine.insert(genuine.end(), d.begin(), d.end()); } genuine.push_back(1);  // truncated to 4 data bits -- too short to complete, no rxint
#else
      std::vector<int> genuine = {0}; { auto d = lsb_first_bits(0x5A, 8); genuine.insert(genuine.end(), d.begin(), d.end()); } genuine.push_back(1);
#endif
      const bool genuine_rxint = drive_receive(h, genuine, cb);
#if MUT_15
      const bool expect_glitch_rxint = true;  // wrong -- a glitch should not register
#else
      const bool expect_glitch_rxint = false;
#endif
      checks.expect(glitch_rxint == expect_glitch_rxint && genuine_rxint,
          "opus:8-serial.c37 sol:8-serial.c22 A19: the receive circuitry "
          "expects exactly one start bit -- a one-beat low glitch on rxd "
          "(not held through the mid-cell sampling point) produces no "
          "receive interrupt, while a genuine, held start bit followed by "
          "a full frame does [p.255 \"...the receive circuitry expects to "
          "see one start bit...\"]");
    }

    // A20 -- compound (opus only): data-bit count matches the selected
    // mode, both modes graded together as this claim states them.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      std::vector<int> rx8 = {0}; { auto d = lsb_first_bits(0x3C, 8); rx8.insert(rx8.end(), d.begin(), d.end()); } rx8.push_back(1);
      const bool ok8 = drive_receive(h, rx8, cb);
      const uint16_t rd8 = h.read_reg(ADDR_SERDATR);
      h.write_reg(ADDR_SERPER, 10 | 0x8000);
#if MUT_16
      std::vector<int> rx9 = {0}; { auto d = lsb_first_bits(0x0BC, 8); rx9.insert(rx9.end(), d.begin(), d.end()); } rx9.push_back(1);  // only 8 data bits driven in 9-bit mode
#else
      std::vector<int> rx9 = {0}; { auto d = lsb_first_bits(0x0BC, 9); rx9.insert(rx9.end(), d.begin(), d.end()); } rx9.push_back(1);
#endif
      const bool ok9 = drive_receive(h, rx9, cb);
      const uint16_t rd9 = h.read_reg(ADDR_SERDATR);
      checks.expect(ok8 && (rd8 & 0x00FF) == 0x3C && ok9 && (rd9 & 0x01FF) == 0x0BC,
          "opus:8-serial.c38 A20: in both modes the receive circuitry "
          "expects eight or nine data bits matching the selected mode -- "
          "8-bit mode captures exactly 8 data bits (0x3C) and 9-bit mode "
          "captures exactly 9 (0x0BC), each at its documented position "
          "[p.255 \"...eight or nine data bits...\", both modes together]");
    }

    // A21 -- at least one stop bit: the frame's fixed timing structure
    // includes one bit-cell's worth of time after the last data bit before
    // completion is signalled -- a genuine stop-bit slot, distinct from
    // the data bits. What the slot's value does is reported, not judged:
    // this fixture does not flag a framing error either way (no framing-
    // error bit exists anywhere in this chapter, unlike overrun).
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
#if MUT_63
      h.write_reg(ADDR_SERPER, 10 | 0x8000);  // one extra write before the sample -- switches to 9-bit mode, frame now one cell short
#endif
      std::vector<int> f_slot1 = {0}; { auto d = lsb_first_bits(0x66, 8); f_slot1.insert(f_slot1.end(), d.begin(), d.end()); } f_slot1.push_back(1);
      const int cells = measure_rx_frame_cells(h, f_slot1, cb);
      std::vector<int> f_slot0 = {0}; { auto d = lsb_first_bits(0x66, 8); f_slot0.insert(f_slot0.end(), d.begin(), d.end()); } f_slot0.push_back(0);
      const bool slot0_completes = drive_receive(h, f_slot0, cb);
      const uint16_t slot0_data = h.read_reg(ADDR_SERDATR);
#if MUT_17
      const int expected_cells = 9;  // wrong -- omits the stop-bit slot
#else
      const int expected_cells = 10;
#endif
      checks.expect(cells == expected_cells,
          "opus:8-serial.c39 sol:8-serial.c25 A21: the receive circuitry "
          "expects at least one stop bit -- the frame's fixed timing "
          "includes one bit-cell's worth of time (a genuine stop-bit slot) "
          "after the 8th data bit before completion, measured at 10 cells "
          "total (1 start + 8 data + 1 slot), not 9 [p.255 \"...and at "
          "least one stop bit.\"]");
      checks.note("A21: a frame whose stop-slot cell reads 0 instead of 1 "
                   "still completes (rxint=" + std::to_string(slot0_completes) +
                   ") and stores that 0 at the documented position (SERDATR=" +
                   hex4(slot0_data) + ") -- the slot's value is sampled, not "
                   "validated; no framing-error indication exists anywhere "
                   "in this chapter to contradict, so this is reported, not "
                   "graded against the page.");
    }

    // ===============================================================
    // B. SERPER's own behaviour (p.256)
    // ===============================================================

    // B1 -- SERPER bit 15 selects receive mode, with its stated polarity.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      std::vector<int> rx8 = {0}; { auto d = lsb_first_bits(0x2D, 8); rx8.insert(rx8.end(), d.begin(), d.end()); } rx8.push_back(1);
      drive_receive(h, rx8, cb);
      const uint16_t got8 = h.read_reg(ADDR_SERDATR);
      h.write_reg(ADDR_SERPER, 10 | 0x8000);
#if MUT_64
      std::vector<int> rx9 = {0}; { auto d = lsb_first_bits(0x1B5, 9); rx9.insert(rx9.end(), d.begin(), d.end()); } rx9.push_back(1);  // wrong 9-bit value driven, oracle still compares against literal 0x1B4
#else
      std::vector<int> rx9 = {0}; { auto d = lsb_first_bits(0x1B4, 9); rx9.insert(rx9.end(), d.begin(), d.end()); } rx9.push_back(1);
#endif
      drive_receive(h, rx9, cb);
      const uint16_t got9 = h.read_reg(ADDR_SERDATR);
#if MUT_18
      const bool polarity_ok = ((got8 & 0x00FF) == 0x2D) && ((got9 & 0x00FF) == 0x2D);  // ignoring the 9th bit entirely
#else
      const bool polarity_ok = ((got8 & 0x00FF) == 0x2D) && ((got9 & 0x01FF) == 0x1B4);
#endif
      checks.expect(polarity_ok,
          "opus:8-serial.c40 opus:8-serial.c42 opus:8-serial.c43 B1: "
          "receive mode is set by bit 15 of SERPER -- 0 selects eight data "
          "bits (0x2D received intact in bits 7-0) and 1 selects nine "
          "(0x1B4 received intact in bits 8-0) [p.256 \"Receive mode is "
          "set by bit 15 of the write-only SERPER register. Bit 15 is a 1 "
          "if you chose nine data bits..., and a 0 if you chose eight...\"]");
    }

    // B2 -- SERPER is write-only, per hazard 1: a write reaches state with
    // an observable effect, and -- measured last, in a harness nothing
    // downstream depends on -- addressing SERPER for a "read" does not
    // return the value most recently written there.
    { Harness h;
      const int cb1 = measure_cell_beats(h, 3);
#if MUT_65
      const int cb2 = measure_cell_beats(h, 45);  // wrong divisor driven, oracle still compares against literal 90
#else
      const int cb2 = measure_cell_beats(h, 44);
#endif
      const bool write_reaches_state = (cb1 == 8) && (cb2 == 90) && (cb1 != cb2);
      // Last thing done with this harness: addressing SERPER "to read" it.
      // This is itself a write of whatever data_i currently holds (hazard
      // 1) -- deliberately absorbed here since nothing after depends on
      // SERPER's value any more.
      const uint16_t probed = h.read_reg(ADDR_SERPER);
#if MUT_19
      const uint16_t readback = 44;  // wrong -- as if the last written value came back
#else
      const uint16_t readback = probed;
#endif
      checks.expect(write_reaches_state && readback == 0x0000 && readback != 44,
          "opus:8-serial.c41 B2: SERPER is write-only -- writes reach "
          "state with a measured, distinguishing effect (cell lengths of "
          "8 and 90 beats for N=3 and N=44), and addressing SERPER for a "
          "\"read\" returns the fixed value every non-SERDATR address "
          "returns (0x0000), never the value most recently written "
          "[p.256 \"...the write-only SERPER register.\"]");
    }

    // B3/B4/B5 -- SERDATR is 16 bits wide, holds the received data, and
    // holds status bits alongside it in the same read word.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
#if MUT_66
      std::vector<int> f = {0}; { auto d = lsb_first_bits(0x7D, 8); f.insert(f.end(), d.begin(), d.end()); } f.push_back(1);  // wrong byte driven, oracle still compares against literal 0x7E
#else
      std::vector<int> f = {0}; { auto d = lsb_first_bits(0x7E, 8); f.insert(f.end(), d.begin(), d.end()); } f.push_back(1);
#endif
      h.dut->rbfmirror = 1;
      drive_receive(h, f, cb);
      const uint16_t got = h.read_reg(ADDR_SERDATR);
      h.dut->rga_i = 0;
      h.dut->rbfmirror = 0;
#if MUT_20
      const bool status_distinct = (got & 0xFF00) == 0;  // status half asserted to be empty
#else
      const bool status_distinct = (got & 0xFF00) != 0;  // RBF/TBE/TSRE bits, at minimum, are set
#endif
      checks.expect((got & 0x00FF) == 0x7E && status_distinct,
          "opus:8-serial.c45 opus:8-serial.c46 opus:8-serial.c47 B3-B5: "
          "the serial input data-receive register is 16 bits wide and "
          "contains both the received data and status bits in the same "
          "word -- a read after reception carries the data byte in bits "
          "7-0 and a nonzero, distinguishable status pattern above it "
          "[p.256 \"The serial input data-receive register is 16 bits "
          "wide. It contains the 8 or 9 bit input data and status bits.\"]");
    }

    // B6 -- data is clocked in one bit at a time; a partial frame produces
    // no premature result (no rxint, and no change to the last completed
    // read word).
    { Harness h;
      const int cb = measure_cell_beats(h, 10);  // rxd left idle (1) throughout this calibration
      const uint16_t before = h.read_reg(ADDR_SERDATR);
      bool saw_rxint = false;
      h.dut->rxd = 0;  // start bit only, begins the deliberate partial-frame stimulus
#if MUT_21
      for (int c = 0; c < cb * 12; ) { h.tick(); if (h.dut->clk7_en) { ++c; if (h.dut->rxint) saw_rxint = true; } }  // long enough for a full frame's worth of time
#else
      for (int c = 0; c < cb * 3; ) { h.tick(); if (h.dut->clk7_en) { ++c; if (h.dut->rxint) saw_rxint = true; } }  // well short of a full frame
#endif
      h.dut->rxd = 1;
      const uint16_t after_partial = h.read_reg(ADDR_SERDATR);
      // Bit 11 (RXD) is excluded from the comparison: it is a registered
      // mirror of the raw pin (C4), and legitimately follows the pin level
      // this stimulus itself drove, regardless of whether any full frame
      // completed. What must NOT move is everything a completed frame
      // would touch: the data bits and the status bits other than RXD.
      checks.expect(!saw_rxint && (after_partial & ~BIT_RXD) == (before & ~BIT_RXD),
          "opus:8-serial.c48 B6: incoming data is clocked one bit at a "
          "time into an internal serial-to-parallel shift register -- a "
          "frame interrupted well short of completion raises no receive "
          "interrupt and leaves the last completed read word unchanged "
          "[p.256 \"The data is received, one bit at a time, into an "
          "internal serial-to-parallel shift register.\"]");
    }

    // B7/B8 -- completion both transfers the shift register's contents to
    // SERDATR and raises a distinct ready signal (rxint) at the same event.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      const uint16_t before = h.read_reg(ADDR_SERDATR);
      std::vector<int> f = {0}; { auto d = lsb_first_bits(0x39, 8); f.insert(f.end(), d.begin(), d.end()); } f.push_back(1);
#if MUT_22
      const bool saw_rxint = false;  // pretend the signal never fired
#else
      const bool saw_rxint = drive_receive(h, f, cb);
#endif
      const uint16_t after = h.read_reg(ADDR_SERDATR);
      checks.expect(saw_rxint && (after & 0x00FF) == 0x39 && after != before,
          "opus:8-serial.c49 opus:8-serial.c50 B7-B8: once the proper "
          "number of bit times have elapsed, the shift register's "
          "contents transfer to SERDATR and a ready signal (rxint) is "
          "raised at that same completion [p.256 \"When the proper number "
          "of bit times have elapsed, the contents of this register are "
          "transferred to the serial data read register (SERDATR)..., and "
          "you are signaled that there is data ready for you.\"]");
    }

    // B9 -- the receive shift register is ready again immediately: two
    // frames sent back-to-back -- frame 1's own stop cell (1) is
    // immediately followed by frame 2's start cell (0), with no extra
    // idle cells between them -- are both received correctly.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      std::vector<int> f1 = {0}; { auto d = lsb_first_bits(0x12, 8); f1.insert(f1.end(), d.begin(), d.end()); } f1.push_back(1);
#if MUT_67
      std::vector<int> f2 = {0}; { auto d = lsb_first_bits(0x88, 8); f2.insert(f2.end(), d.begin(), d.end()); } f2.push_back(1);  // wrong byte driven, oracle still compares against literal 0x87
#else
      std::vector<int> f2 = {0}; { auto d = lsb_first_bits(0x87, 8); f2.insert(f2.end(), d.begin(), d.end()); } f2.push_back(1);
#endif
      std::vector<int> both = f1; both.insert(both.end(), f2.begin(), f2.end());
      drive_receive(h, both, cb);
      const uint16_t final_word = h.read_reg(ADDR_SERDATR);
#if MUT_23
      const uint16_t expected_low_byte = 0x12;  // wrong -- the first frame's byte, not the second's
#else
      const uint16_t expected_low_byte = 0x87;
#endif
      checks.expect((final_word & 0x00FF) == expected_low_byte,
          "opus:8-serial.c52 B9: the receive shift register becomes ready "
          "to accept new data immediately after a transfer -- a second "
          "frame (0x87) sent with no idle gap after the first (0x12) is "
          "still received correctly, landing as the final read word "
          "[p.256 \"Immediately after the transfer of data takes place, "
          "the receive shift register again becomes ready to accept new "
          "data.\"]");
    }

    // B10 -- the receiver-full deadline exists and can be met: reading and
    // clearing (dropping rbfmirror, simulating the CPU clearing INTF_RBF)
    // between two frames avoids overrun.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      // rbfmirror stays clear through frame 1 (no previous-unread byte
      // yet). After it completes, mark RBF set (as the interrupt
      // controller would) -- then, in the baseline, service it (clear
      // rbfmirror) well within the deadline, before frame 2 completes.
      std::vector<int> f1 = {0}; { auto d = lsb_first_bits(0x50, 8); f1.insert(f1.end(), d.begin(), d.end()); } f1.push_back(1);
      drive_receive(h, f1, cb);
      h.read_reg(ADDR_SERDATR);
      h.dut->rga_i = 0;
      h.dut->rbfmirror = 1;
#if MUT_24
      // not serviced -- rbfmirror is never cleared before frame 2 completes
#else
      wait_beats(h, cb / 2);
      h.dut->rbfmirror = 0;
      wait_beats(h, 2);
#endif
      std::vector<int> f2 = {0}; { auto d = lsb_first_bits(0x51, 8); f2.insert(f2.end(), d.begin(), d.end()); } f2.push_back(1);
      drive_receive(h, f2, cb);
      const uint16_t after2 = h.read_reg(ADDR_SERDATR);
      checks.expect((after2 & BIT_OVRUN) == 0,
          "opus:8-serial.c53 B10: after the receiver-full interrupt you "
          "have up to one full character-receive time to accept the data "
          "and clear it -- a deadline that can be met: dropping rbfmirror "
          "(clearing INTF_RBF) between two received frames avoids overrun "
          "on the second [p.256 \"...you will have up to one full "
          "character-receive time...to accept the data and clear the "
          "interrupt.\"]");
    }

    // B11 -- the deadline quantified as 8 to 10 bit times. Measured
    // directly in bit cells for both receive modes; needs no absolute
    // frequency. RED AT BASELINE: 8-bit mode measures 10 (in range), but
    // 9-bit mode measures 11 -- one bit-cell beyond the printed ceiling.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      std::vector<int> f8 = {0}; { auto d = lsb_first_bits(0x5B, 8); f8.insert(f8.end(), d.begin(), d.end()); } f8.push_back(1);
      const int cells8 = measure_rx_frame_cells(h, f8, cb);
      h.write_reg(ADDR_SERPER, 10 | 0x8000);
      std::vector<int> f9 = {0}; { auto d = lsb_first_bits(0x0B5, 9); f9.insert(f9.end(), d.begin(), d.end()); } f9.push_back(1);
      const int cells9 = measure_rx_frame_cells(h, f9, cb);
#if MUT_25
      const int ceiling = 11;  // widened to match measured reality -- oracle-green flip
#else
      const int ceiling = 10;  // the printed ceiling
#endif
      checks.expect(cells8 >= 8 && cells8 <= ceiling && cells9 >= 8 && cells9 <= ceiling,
          "opus:8-serial.c54 B11: that deadline is quantified as 8 to 10 "
          "bit times -- measured directly at 10 bit-cells for an 8-bit "
          "frame and 11 for a 9-bit frame (1 start + N data + 1 stop-slot "
          "each) [p.256 \"...(8 to 10 bit times)...\"]");
      checks.note("B11: measured character-receive time = " +
                   std::to_string(cells8) + " bit-cells (8-bit mode), " +
                   std::to_string(cells9) + " bit-cells (9-bit mode).");
    }

    // B12 -- OVERRUN sets if the interrupt is not cleared in time. Same
    // measurement basis as A7/A8, its own uid and citation.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      std::vector<int> f1 = {0}; { auto d = lsb_first_bits(0x61, 8); f1.insert(f1.end(), d.begin(), d.end()); } f1.push_back(1);
      drive_receive(h, f1, cb);
      h.read_reg(ADDR_SERDATR);
      h.dut->rga_i = 0;
      h.dut->rbfmirror = 1;  // left uncleared
#if MUT_26
      std::vector<int> f2;  // no second frame -- overrun cannot be observed to set
#else
      std::vector<int> f2 = {0}; { auto d = lsb_first_bits(0x62, 8); f2.insert(f2.end(), d.begin(), d.end()); } f2.push_back(1);
#endif
      if (!f2.empty()) drive_receive(h, f2, cb);
      const uint16_t after2 = h.read_reg(ADDR_SERDATR);
      checks.expect((after2 & BIT_OVRUN) != 0,
          "opus:8-serial.c55 B12: if the interrupt is not cleared in "
          "time, the OVERRUN bit is set -- left uncleared (rbfmirror held) "
          "across a second completed frame, OVRUN reads set [p.256 \"If "
          "the interrupt is not cleared in time, the OVERRUN bit is "
          "set.\"]");
    }

    // B13 -- SERDATR bit 15, name OVRUN, function OVERRUN, and its
    // semantics, graded together: this bit position, and no other, carries
    // the overrun condition already established in A7/A8/B12.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      std::vector<int> f1 = {0}; { auto d = lsb_first_bits(0x70, 8); f1.insert(f1.end(), d.begin(), d.end()); } f1.push_back(1);
      drive_receive(h, f1, cb);
      const uint16_t clean = h.read_reg(ADDR_SERDATR);
      h.dut->rga_i = 0;
#if MUT_68
      h.dut->rbfmirror = 0;  // previous byte no longer marked unread -- overrun condition never arms
#else
      h.dut->rbfmirror = 1;
#endif
      std::vector<int> f2 = {0}; { auto d = lsb_first_bits(0x71, 8); f2.insert(f2.end(), d.begin(), d.end()); } f2.push_back(1);
      drive_receive(h, f2, cb);
      const uint16_t overrun = h.read_reg(ADDR_SERDATR);
#if MUT_27
      const uint16_t probe_bit = BIT_TSRE;  // the wrong bit position -- set at idle regardless, never tracks this transition
#else
      const uint16_t probe_bit = BIT_OVRUN;
#endif
      checks.expect((clean & probe_bit) == 0 && (overrun & probe_bit) != 0,
          "opus:8-serial.c59 opus:8-serial.c60 opus:8-serial.c62 B13: "
          "SERDATR bit 15, named OVRUN, function OVERRUN -- indicating "
          "another byte arrived before the previous one was picked up -- "
          "and no other bit position tracks that condition [p.256 \"15 "
          "OVRUN OVERRUN...Indicates that another byte of data was "
          "received before the previous byte was picked up...\"]");
    }

    // B14 -- SERDATR bit 14, name RBF, and its semantics ("data ready"),
    // graded together against the ready-signal fact established in B7/A9.
    { Harness h;
      const uint16_t idle = h.read_reg(ADDR_SERDATR);
      const int cb = measure_cell_beats(h, 10);
#if MUT_69
      h.dut->rbfmirror = 0;  // RBF (rx_rbf) is a direct mirror of this pin, paula_uart.v:246 -- left clear, RBF never reads set
#else
      h.dut->rbfmirror = 1;
#endif
      std::vector<int> f = {0}; { auto d = lsb_first_bits(0x2A, 8); f.insert(f.end(), d.begin(), d.end()); } f.push_back(1);
      drive_receive(h, f, cb);
      const uint16_t got = h.read_reg(ADDR_SERDATR);
#if MUT_28
      const uint16_t probe_bit = BIT_TBE;  // the wrong bit position
#else
      const uint16_t probe_bit = BIT_RBF;
#endif
      checks.expect((idle & probe_bit) == 0 && (got & probe_bit) != 0,
          "opus:8-serial.c65 opus:8-serial.c68 B14: SERDATR bit 14, named "
          "RBF -- when 1, there is data ready to be picked up -- reads "
          "clear at idle and set after a completed receive [p.256 \"14 "
          "RBF READ BUFFER FULL...When this bit is 1, there is data ready "
          "to be picked up by the processor.\"]");
    }

    // ===============================================================
    // C. SERDATR's remaining bits (p.257)
    // ===============================================================

    // C1 -- SERDATR bit 13, name TBE, function TRANSMIT BUFFER EMPTY, and
    // "also true when the buffer is empty" -- reads set both after a
    // completed transfer and at idle, before any SERDAT write has ever
    // occurred (an empty buffer, not just an emptied one).
    { Harness h;
      const uint16_t at_idle = h.read_reg(ADDR_SERDATR);
      const int cb = measure_cell_beats(h, 10);
#if MUT_70
      TxResult t = capture_transmit(h, 10, 0x8001, cb, 16);  // needs 17 cells but the window only watches 16 -- never completes
#else
      TxResult t = capture_transmit(h, 10, 0x0155, cb, 16);
#endif
      const uint16_t after_tx = h.read_reg(ADDR_SERDATR);
#if MUT_29
      const bool idle_tbe = (at_idle & BIT_TBE) == 0;  // wrong expected polarity
#else
      const bool idle_tbe = (at_idle & BIT_TBE) != 0;
#endif
      checks.expect(idle_tbe && t.completed && (after_tx & BIT_TBE) != 0,
          "opus:8-serial.c70 opus:8-serial.c71 opus:8-serial.c74 C1: "
          "SERDATR bit 13, named TBE, function TRANSMIT BUFFER EMPTY -- "
          "this indication reads set both before any SERDAT write has "
          "ever occurred (an empty buffer) and again once a transmission "
          "completes [p.257 \"13 TBE TRANSMIT BUFFER EMPTY...This is also "
          "true when the buffer is empty.\"]");
    }

    // C2 -- SERDATR bit 12, name TSRE, function TRANSMIT SHIFT REGISTER
    // EMPTY: clear from the moment a transmission is triggered until the
    // last bit has left the shift register, set before and after.
    { Harness h;
      const uint16_t at_idle = h.read_reg(ADDR_SERDATR);
      const int cb = measure_cell_beats(h, 10);
      h.write_reg(ADDR_SERPER, 10);
#if MUT_71
      // SERDAT write omitted entirely -- no transmission is ever triggered, so TSRE cannot clear mid-shift
#else
      h.write_reg(ADDR_SERDAT, 0x0155);
#endif
      h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
      bool saw_clear_mid_shift = false;
      for (int c = 0; c < cb * 4; ) {
        h.tick();
        if (h.dut->clk7_en) { ++c; if ((h.dut->data_o & BIT_TSRE) == 0) saw_clear_mid_shift = true; }
      }
      wait_beats(h, cb * 8);  // let the frame finish
      const uint16_t after = h.dut->data_o;
      h.dut->rga_i = 0;
#if MUT_30
      const bool clear_check_ok = !saw_clear_mid_shift;  // wrong -- expects TSRE never clears mid-shift
#else
      const bool clear_check_ok = saw_clear_mid_shift;
#endif
      checks.expect((at_idle & BIT_TSRE) != 0 && clear_check_ok && (after & BIT_TSRE) != 0,
          "opus:8-serial.c76 opus:8-serial.c77 opus:8-serial.c78 C2: "
          "SERDATR bit 12, named TSRE, function TRANSMIT SHIFT REGISTER "
          "EMPTY -- reads set at idle, clear while a frame is shifting "
          "out, and set again once it completes [p.257 \"12 TSRE TRANSMIT "
          "SHIFT REGISTER EMPTY...the output shift register has completed "
          "its task, all data has been transmitted, and the register is "
          "now idle.\"]");
    }

    // C3 -- across two back-to-back writes, TSRE stays clear through both
    // words and only sets once the SECOND finishes.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      h.write_reg(ADDR_SERPER, 10);
      h.write_reg(ADDR_SERDAT, 0x0155);
      h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
      // wait roughly one bit cell, then land the second word while the
      // first is still shifting
      wait_beats(h, cb);
      h.dut->rga_i = 0;
#if MUT_72
      h.write_reg(ADDR_SERDATR, 0x0166);  // retargeted to SERDATR, which has no write path at all -- no second word ever loads, but the same number of ticks elapses
#else
      h.write_reg(ADDR_SERDAT, 0x0166);
#endif
      h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
      bool clear_through_both = true;
#if MUT_31
      const int watch_cells = 3;  // far too short a window -- stops watching before word 1 alone would finish
#else
      const int watch_cells = 9;  // short of both words' combined length, long enough to span word 1 alone
#endif
      for (int c = 0; c < cb * watch_cells; ) {
        h.tick();
        if (h.dut->clk7_en) { ++c; if ((h.dut->data_o & BIT_TSRE) != 0) clear_through_both = false; }
      }
      wait_beats(h, cb * 15);
      const bool set_after_both = (h.dut->data_o & BIT_TSRE) != 0;
      h.dut->rga_i = 0;
      checks.expect(clear_through_both && set_after_both,
          "opus:8-serial.c79 C3: if you stop writing into SERDAT, TSRE "
          "becomes 1 only after both the word already in the shift "
          "register and the word placed into SERDAT have been transmitted "
          "-- writing a second word one bit-cell after the first keeps "
          "TSRE clear through both words' worth of cells, setting only "
          "once both are done [p.257 \"...this bit will become a 1 after "
          "both the word currently in the shift register and the word "
          "placed into SERDAT have been transmitted.\"]");
    }

    // C4 -- SERDATR bit 11, "Direct read of RXD pin". Measured against the
    // ordinary reading of "direct": an unsynchronised, same-cycle
    // reflection of the pin. Control, same measurement basis (count
    // clk7_en beats from a stimulus to the observable bit position that
    // reports it): the bus read mux itself is purely combinational --
    // changing rga_i to an address whose content is already stable and
    // reading it back costs zero clk7_en beats, so the read PATH is not
    // what is slow. RED AT BASELINE: bit 11 still lags a genuine pin
    // change by several clk7_en cycles (a two-flop synchroniser plus one
    // more register, paula_uart.v:46-53, :244) -- "direct" does not hold
    // on its ordinary reading.
    { Harness h;
      // Control: DB7-0 already holds a known, stable value (0 at reset,
      // before any receive). Address SERDATR and confirm it reads
      // correctly with no clk7_en beats elapsed at all -- the read mux
      // has zero latency of its own.
      const uint16_t immediate = h.read_reg(ADDR_SERDATR);
      const bool read_path_instant = (immediate & 0x00FF) == 0x0000;

      // RXD latency: change the pin, count clk7_en beats until bit 11
      // follows.
      h.dut->rxd = 1;
      while (!h.dut->clk7_en) h.tick();
      h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
      h.dut->rxd = 0;
      int rxd_latency = -1;
      for (int c = 0; c < 20 && rxd_latency < 0; ) {
        h.tick();
        if (h.dut->clk7_en) { ++c; if ((h.dut->data_o & BIT_RXD) == 0) rxd_latency = c - 1; }
      }
      h.dut->rga_i = 0;
      h.dut->rxd = 1;

#if MUT_32
      const bool direct_ok = true;  // oracle-green: the manual's "direct" is read loosely, latency ignored
#else
      const bool direct_ok = read_path_instant && (rxd_latency == 0);
#endif
      checks.expect(direct_ok,
          "opus:8-serial.c81 C4: SERDATR bit 11 is a direct read of the "
          "RXD pin -- measured, on the ordinary reading of \"direct\" "
          "(unsynchronised, same-cycle), against a same-boundary control: "
          "the bus read path itself is purely combinational (an "
          "already-stable value reads back with zero clk7_en beats "
          "elapsed), so any latency on bit 11 comes from the pin's own "
          "path, not from reading it [p.257 \"11 RXD Direct read of RXD "
          "pin on Paula chip.\"]");
      checks.note("C4: measured latency -- RXD-bit-after-pin-change = " +
                   std::to_string(rxd_latency) + " clk7_en beats (0 would "
                   "match \"direct\"). The pin passes a two-flop "
                   "synchroniser plus one more register (paula_uart.v:46-53, "
                   ":234) before reaching bit 11.");
    }

    // C5 -- SERDATR bit 10 is unused, and reads a fixed value regardless
    // of any stimulus applied elsewhere in the register.
    { Harness h;
      const uint16_t before = h.read_reg(ADDR_SERDATR);
      const int cb = measure_cell_beats(h, 10);
      h.dut->rbfmirror = 1;
      std::vector<int> f = {0}; { auto d = lsb_first_bits(0xFF, 8); f.insert(f.end(), d.begin(), d.end()); } f.push_back(1);
      drive_receive(h, f, cb);
      const uint16_t after_rx = h.read_reg(ADDR_SERDATR);
      h.dut->rga_i = 0;
      h.dut->rbfmirror = 0;
      TxResult t = capture_transmit(h, 10, 0xFFFF, cb, 20);
      const uint16_t after_tx = h.read_reg(ADDR_SERDATR);
#if MUT_33
      const uint16_t mask = BIT_9;  // checking the wrong bit
#else
      const uint16_t mask = BIT_10;
#endif
      checks.expect((before & mask) == 0 && (after_rx & mask) == 0 && (after_tx & mask) == 0,
          "opus:8-serial.c83 C5: SERDATR bit 10 is not used at this time "
          "-- reads 0 at idle, after an all-ones receive, and after an "
          "all-ones transmit [p.257 \"10 Not used at this time.\"]");
    }

    // C6-C10 -- bit 9 and bit 8's two mode-dependent meanings, graded
    // together as one composed fact: in 8-bit mode bit 8 carries the
    // stop-slot sample and bit 9 is not the meaningful stop bit for that
    // mode; in 9-bit mode bit 8 carries the 9th data bit and bit 9 carries
    // the stop-slot sample. The two meanings are shown exclusive because
    // they are gated by the same mode bit (B1). Also backs
    // opus:8-serial.c135 (E1, p.259: stop bits are 1s placed above the
    // data bits) -- the same stop-slot mechanism, restated.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      std::vector<int> rx8 = {0}; { auto d = lsb_first_bits(0x4D, 8); rx8.insert(rx8.end(), d.begin(), d.end()); } rx8.push_back(1);
      drive_receive(h, rx8, cb);
      const uint16_t got8 = h.read_reg(ADDR_SERDATR);
      h.write_reg(ADDR_SERPER, 10 | 0x8000);
#if MUT_73
      std::vector<int> rx9 = {0}; { auto d = lsb_first_bits(0x1D4, 9); rx9.insert(rx9.end(), d.begin(), d.end()); } rx9.push_back(1);  // wrong 9th-bit value driven, oracle still compares against literal 0x0D4's bit 8
#else
      std::vector<int> rx9 = {0}; { auto d = lsb_first_bits(0x0D4, 9); rx9.insert(rx9.end(), d.begin(), d.end()); } rx9.push_back(1);
#endif
      drive_receive(h, rx9, cb);
      const uint16_t got9 = h.read_reg(ADDR_SERDATR);
#if MUT_34
      const bool bit8_9bit_ok = ((got9 & BIT_9) != 0) == ((got9 & BIT_8) != 0);  // treats bit8 as if it mirrored bit9 in 9-bit mode
#else
      const bool bit8_9bit_ok = ((got9 & BIT_8) != 0) == (((0x0D4 >> 8) & 1) != 0);
#endif
      checks.expect(bit8_9bit_ok,
          "opus:8-serial.c84 opus:8-serial.c85 opus:8-serial.c86 "
          "opus:8-serial.c87 opus:8-serial.c88 opus:8-serial.c89 "
          "opus:8-serial.c90 opus:8-serial.c135 C6-C10/E1: bit 9 is STP "
          "(stop bit) in 9-bit mode; bit 8 is STP in 8-bit mode and DB8 "
          "(9th data bit) in 9-bit mode -- the two meanings are exclusive "
          "alternatives selected by the same mode bit (B1): in 9-bit mode "
          "bit 8 tracks the presented 9th data bit, not a stop sample "
          "[p.257 \"9 STP Stop bit if 9 data bits...8 STP Stop bit if 8 "
          "data bits...OR DB8 9th data bit if 9 bits...\", p.259 \"Above "
          "the data bits, 1 bits must be added as stop bits.\"]");
    }

    // C11/C12 -- SERDATR bits 7-0, named DB7-DB0, carry the low 8 data
    // bits, TRUE polarity (not inverted).
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
#if MUT_35
      std::vector<int> f = {0}; { auto d = lsb_first_bits(static_cast<uint16_t>(~0x96 & 0xFF), 8); f.insert(f.end(), d.begin(), d.end()); } f.push_back(1);  // drive the inverted byte
#else
      std::vector<int> f = {0}; { auto d = lsb_first_bits(0x96, 8); f.insert(f.end(), d.begin(), d.end()); } f.push_back(1);
#endif
      drive_receive(h, f, cb);
      const uint16_t got = h.read_reg(ADDR_SERDATR);
      checks.expect((got & 0x00FF) == 0x96,
          "opus:8-serial.c91 opus:8-serial.c92 opus:8-serial.c93 C11-C12: "
          "SERDATR bits 7-0, named DB7-DB0, carry the low 8 data bits, "
          "TRUE polarity -- a known byte (0x96) presented on rxd reads "
          "back bit-for-bit identical, not inverted [p.257 \"7-0 DB7-DB0 "
          "Low 8 data bits of received data. Data is TRUE...\"]");
    }

    // ===============================================================
    // D. How output data is transmitted (p.258)
    // ===============================================================

    // D1 -- data is sent by writing SERDAT; SERDAT is write-only.
    { Harness h;
      const int cb1 = measure_cell_beats(h, 3);
#if MUT_52
      TxResult t1 = capture_transmit(h, 3, 0x0033, cb1, 16);  // wrong shape driven (highest bit at 5, not 8) -- oracle still expects a 10-cell frame
#else
      TxResult t1 = capture_transmit(h, 3, 0x0133, cb1, 16);
#endif
#if MUT_36
      const bool write_reaches_state = t1.completed && (int)t1.bits.size() == 10 && false;
#else
      const bool write_reaches_state = t1.completed && (int)t1.bits.size() == 10;
#endif
      checks.expect(write_reaches_state,
          "opus:8-serial.c103 opus:8-serial.c134 D1: data is sent by "
          "writing into SERDAT, and the data to be transmitted is placed "
          "there -- a written word produces a correctly-shaped transmitted "
          "frame [p.258 \"You send data out on the transmit lines by "
          "writing into the serial data output register (SERDAT).\", "
          "p.259 \"The data to be transmitted is placed in the output "
          "register (SERDAT).\"]");
    }
    { Harness h;
      const int cb = measure_cell_beats(h, 3);
      TxResult t = capture_transmit(h, 3, 0x0177, cb, 16);
      // Last thing done with this harness: addressing SERDAT "to read" it
      // (hazard 1) -- deliberately absorbed since nothing after depends on
      // SERDAT's stored value any more.
#if MUT_37
      const uint16_t readback = 0x0177;  // not actually probed
#else
      const uint16_t readback = h.read_reg(ADDR_SERDAT);
#endif
      checks.expect(t.completed && readback == 0x0000,
          "opus:8-serial.c104 D1b: SERDAT is write-only -- addressing it "
          "for a \"read\" returns the fixed value every non-SERDATR "
          "address returns (0x0000), never the value most recently "
          "written [p.258 \"This register is write-only.\"]");
    }

    // D2 -- transmit rate is the same rate established for receive; one
    // divisor register governs both directions.
    { Harness h;
      const int cb_tx = measure_cell_beats(h, 25);
#if MUT_53
      h.write_reg(ADDR_SERPER, 25 | 0x8000);  // one extra write before the sample -- switches to 9-bit mode, frame now one cell longer
#endif
      std::vector<int> rxf = {0};
      { auto d = lsb_first_bits(0x63, 8); rxf.insert(rxf.end(), d.begin(), d.end()); }
      rxf.push_back(1);
      const int cb_rx = measure_rx_frame_cells(h, rxf, cb_tx);
#if MUT_38
      const bool same_rate = (cb_rx == 9);  // wrong expected frame length
#else
      const bool same_rate = (cb_rx == 10);
#endif
      checks.expect(same_rate,
          "opus:8-serial.c105 D2: data is sent out at the same rate "
          "established for the read -- the one SERPER divisor that sets "
          "the measured transmit cell length also governs the receive "
          "side, which completes an 8-bit frame in the expected 10 cells "
          "under that same divisor [p.258 \"Data will be sent out at the "
          "same rate as you have established for the read.\"]");
      checks.note("D2: cell_beats(N=25, tx side)=" + std::to_string(cb_tx) +
                   "; rx frame cells under the same N=" + std::to_string(cb_rx));
    }

    // D3 -- transmission begins immediately after the write, at the
    // selected rate; the data is transferred into the shift register at
    // the start.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      while (!h.dut->clk7_en) h.tick();
      h.dut->rga_i = (ADDR_SERDAT >> 1) & 0xFF;
#if MUT_54
      h.dut->data_i = 0x0188;  // bit 0 now 0 -- the LSB the assertion checks no longer reads 1
#else
      h.dut->data_i = 0x0189;
#endif
      for (int c = 0; c < 4; ++c) h.tick();
      h.dut->rga_i = 0; h.dut->data_i = 0;
      // Watch the SAME single write's own frame through to completion --
      // never a second, uncoordinated write -- collecting both the
      // write-to-start latency and the frame's content in one pass.
      h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
      int beats_to_start = -1;
      bool started = false;
      bool last1 = (h.dut->txd == 1);
      std::vector<int> bits;
      int since_cell = 0, beats_since_start = 0;
      bool completed = false;
      for (int guard = 0; guard < cb * 4 * 15 + 500 && !completed; ++guard) {
        h.tick();
        if (!h.dut->clk7_en) continue;
        if (!started) {
          ++beats_to_start;
          if (last1 && h.dut->txd == 0) { started = true; since_cell = 0; beats_since_start = 0; }
        } else {
          if (since_cell == cb / 2) bits.push_back(h.dut->txd);
          ++since_cell; if (since_cell >= cb) since_cell = 0;
          ++beats_since_start;
          if ((h.dut->data_o & BIT_TSRE) != 0) completed = true;
        }
        last1 = (h.dut->txd == 1);
      }
      h.dut->rga_i = 0;
#if MUT_39
      const bool begins_promptly = false;  // wrong -- treated as never prompt
#else
      const bool begins_promptly = (beats_to_start >= 0 && beats_to_start <= cb);
#endif
      checks.expect(begins_promptly && completed && !bits.empty() && bits[1] == 1,
          "opus:8-serial.c106 opus:8-serial.c107 D3: immediately after "
          "the write the system begins transmission at the selected rate, "
          "and the data is transferred into the internal shift register "
          "at the start -- the start bit appears within one bit-cell of "
          "the write, and the bits that follow match the written word "
          "[p.258 \"Immediately after you write the data into this "
          "register, the system will begin the transmission...\", \"At "
          "the start of the operation, this data is transferred from "
          "SERDAT into an internal serial shift register.\"]");
      checks.note("D3: beats from write to start-bit onset = " + std::to_string(beats_to_start) +
                   " (one bit-cell = " + std::to_string(cb) + " beats).");
    }

    // D4 -- SERDAT can accept new data once the transfer into the shift
    // register completes; TBE signals exactly that moment. A second write
    // issued as soon as TBE reads 1 (not at some arbitrary later time) is
    // accepted and correctly transmitted after the first word finishes.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      h.write_reg(ADDR_SERDAT, 0x0155);
      h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
      // The combined frame's window opens at the first 1->0 TxD edge after
      // THIS, the first, write -- word 1's own start bit, which D3 above
      // measures lands within one bit-cell of the write, well before TBE
      // itself fires. Opening it only after the SECOND write below (as
      // this block used to) finds the first 1->0 edge somewhere inside
      // word 1's own data bits instead, understating the combined frame by
      // however many cells that edge is already into word 1. So the same
      // edge-tracking state (last_txd/started/beats) starts here and
      // carries straight through the TBE-polling loop and the second
      // write into the completion loop below -- one continuous window.
      int last_txd = h.dut->txd;
      bool started = false;
      int beats = 0;
      bool completed = false;
      int beats_to_tbe = -1;
      for (int c = 0; c < cb * 2 && beats_to_tbe < 0; ) {
        h.tick();
        if (!h.dut->clk7_en) continue;
        ++c;
        int cur = h.dut->txd;
        if (!started && last_txd == 1 && cur == 0) { started = true; beats = 0; }
        if (started) ++beats;
        last_txd = cur;
        if ((h.dut->data_o & BIT_TBE) != 0) beats_to_tbe = c - 1;
      }
      h.dut->rga_i = 0;
#if MUT_40
      // second word the page grades DIFFERENTLY, written immediately (as
      // soon as TBE reads 1, same as the baseline) so every beat stays
      // inside the counted window: 0x0066's highest set bit is bit 6, so
      // by pp.258-259's "shifting continues until all 1 bits have been
      // shifted out" it shifts 7 data bits, 8 cells (1 start + 7 data) --
      // combined with word 1's 10, 18 cells, not the baseline's 21. A
      // delay inserted here instead (an earlier version of this mutation)
      // spent its own beats inside wait_beats(), OUTSIDE this window's own
      // counting loops -- red for an uncounted-beats artefact, not because
      // the page grades the stimulus differently.
      h.write_reg(ADDR_SERDAT, 0x0066);
#else
      h.write_reg(ADDR_SERDAT, 0x0266);
#endif
      // Continue the SAME window through to the frame's single completion
      // (TSRE does not rise between the two words -- C3 establishes that
      // it only sets once every pending word is done). Content is not
      // decoded bit-by-bit here -- the state machine's own extra
      // idle-to-shift bounce between the two words (traced in
      // paula_uart.v's TX_IDLE/TX_SHIFT cases) does not divide evenly into
      // fixed-length cell slots, so timing, not a rigid per-cell sample
      // grid, is what this measures.
      h.dut->rga_i = (ADDR_SERDATR >> 1) & 0xFF;
      for (int guard = 0; guard < cb * 4 * 30 + 1000 && !completed; ++guard) {
        h.tick();
        if (!h.dut->clk7_en) continue;
        int cur = h.dut->txd;
        if (!started && last_txd == 1 && cur == 0) { started = true; beats = 0; }
        if (started) {
          ++beats;
          if ((h.dut->data_o & BIT_TSRE) != 0) completed = true;
        }
        last_txd = cur;
      }
      h.dut->rga_i = 0;
      const int cells = (beats + cb / 2) / cb;
      checks.expect(beats_to_tbe >= 0 && completed && cells == 21,
          "opus:8-serial.c108 opus:8-serial.c109 D4: SERDAT can accept "
          "new data once the transfer to the shift register completes, "
          "and the TBE interrupt signals this fact -- a second word "
          "written as soon as TBE reads 1 is accepted and transmitted "
          "after the first word finishes: the combined frame measures "
          "exactly 21 cells (10 + 11), not 10 or 11 alone. 21 is derived, "
          "not read off one sentence: each word is one start bit (p.259 "
          "\"One start bit is automatically generated before the first "
          "data bit (bit 0) is sent.\") plus data bits shifted out one per "
          "interval until every 1 bit has gone (p.258 \"Data will be moved "
          "out of the shift register, one bit during each time interval "
          "... The shifting continues until all 1 bits have been shifted "
          "out.\") -- 0x0155's highest set bit is bit 8, so 1+9=10 cells; "
          "0x0266's is bit 9, so 1+10=11 cells -- and the frame's end "
          "instant is TSRE (p.257 \"...the output shift register has "
          "completed its task, all data has been transmitted...\"), which "
          "fires once, after both words, not between them [p.258 \"When "
          "the transfer to the serial shift register has been completed, "
          "SERDAT can accept new data; the TBE interrupt signals this "
          "fact.\"]");
      checks.note("D4: combined frame measured " + std::to_string(beats) +
                   " clk7_en beats (" + std::to_string(cells) + " cells at " +
                   std::to_string(cb) + " beats/cell); word 1 alone is 10 "
                   "cells, word 2 alone is 11.");
    }

    // D5 -- shift-out timing: LSB first, one bit per interval, including
    // the two figure readings of the same fact (bit leaving = the one
    // shifted out; data leaves at the LSB end).
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
#if MUT_41
      TxResult t = capture_transmit(h, 10, 0x00AA, cb, 16);  // MSB-heavy pattern check against an LSB-first oracle mismatched on purpose below
#else
      TxResult t = capture_transmit(h, 10, 0x0155, cb, 16);
#endif
      bool lsb_first_ok = t.completed && (int)t.bits.size() == 10;
      if (lsb_first_ok) {
        auto want = lsb_first_bits(0x55, 8);
        if (t.bits[0] != 0) lsb_first_ok = false;
        for (int i = 0; i < 8 && lsb_first_ok; ++i)
          if (t.bits[i + 1] != want[i]) lsb_first_ok = false;
      }
      checks.expect(lsb_first_ok,
          "opus:8-serial.c110 opus:8-serial.c111 opus:8-serial.c120 "
          "opus:8-serial.c128 D5: data moves out of the shift register "
          "one bit per interval, starting with the least significant bit "
          "-- SERDAT=0x0155 transmits start=0 then 0x55's bits in order "
          "b0,b1,...,b7 [p.258 \"Data will be moved out of the shift "
          "register, one bit during each time interval, starting with the "
          "least significant bit.\"]");
    }

    // D6 -- shifting continues until all 1 bits are shifted out; TSRE
    // signals the stop condition (a 1 in the shifted-out position AND the
    // rest all 0s, both conditions together -- a middle 1 followed by more
    // nonzero content does not stop the shift early).
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      // 0x0205: bit0=1, bit2=1, bit9=1 -- a "1" at bit2 is followed by
      // more nonzero content (bit9) above it, so shifting must continue
      // past bit2 (which a defect stopping at the *first* 1-with-zero-
      // below would reach after only 4 cells) rather than stopping there;
      // correct behaviour shifts the full 11 cells out to bit9.
#if MUT_55
      TxResult t = capture_transmit(h, 10, 0x0005, cb, 16);  // bit 9 dropped -- highest set bit now at 2, a genuinely shorter frame
#else
      TxResult t = capture_transmit(h, 10, 0x0205, cb, 16);
#endif
#if MUT_42
      const int expected_cells = 4;   // wrong -- stopping early at bit 2
#else
      const int expected_cells = 11;  // correct -- continuing to bit 9
#endif
      checks.expect(t.completed && (int)t.bits.size() == expected_cells,
          "opus:8-serial.c112 opus:8-serial.c130 opus:8-serial.c131 "
          "opus:8-serial.c132 D6: shifting continues until all 1 bits "
          "have been shifted out -- TSRE (shift-register-empty) fires "
          "only once a 1 is in the shifted-out position AND everything "
          "above it is 0 -- SERDAT=0x0205 (a 1 at bit 2 with more nonzero "
          "content at bit 9 above it) shifts the full 11 cells to bit 9, "
          "not stopping early at bit 2 [p.258 \"The shifting continues "
          "until all 1 bits have been shifted out.\", p.259 \"...stops "
          "shifting and signals...(TSRE) when there is a 1 bit present in "
          "the bit-shifted-out position and the rest of the contents...are "
          "0s.\"]");
    }

    // D7 -- shifting resumes when new nonzero contents are loaded, once
    // the shift register is idle.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      TxResult t1 = capture_transmit(h, 10, 0x0155, cb, 16);
      wait_beats(h, cb * 2);  // settle at idle
#if MUT_58
      TxResult t2 = capture_transmit(h, 10, 0x0066, cb, 16);  // wrong shape driven (highest bit at 6, not 8) -- oracle still expects 10 cells
#else
      TxResult t2 = capture_transmit(h, 10, 0x0166, cb, 16);
#endif
#if MUT_43
      const int expected_t2_cells = 7;  // wrong -- some other pattern's length
#else
      const int expected_t2_cells = 10;
#endif
      checks.expect(t1.completed && t2.completed && (int)t2.bits.size() == expected_t2_cells,
          "opus:8-serial.c133 D7: when new nonzero contents are loaded "
          "into SERDAT once the shift register is idle, shifting begins "
          "again -- a second, later write after a completed transmission "
          "starts a fresh, complete frame [p.259 \"When new nonzero "
          "contents are loaded into this register, shifting begins "
          "again.\"]");
    }

    // D8 -- any combination of data and stop bits may be specified; SERDAT
    // controls the transmitted format. Several distinct bit patterns, each
    // producing its own correctly-shaped frame -- including the figure's
    // own 8-bit-data-field-plus-stop-bit-at-8 depiction.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      TxResult ta = capture_transmit(h, 10, 0x0031, cb, 16);  // highest set bit at 5 -- 7-cell frame
#if MUT_56
      TxResult tb = capture_transmit(h, 10, 0x0021, cb, 16);  // same shape as ta (highest bit at 5) -- the two frames no longer differ
#else
      TxResult tb = capture_transmit(h, 10, 0x0709, cb, 16);  // highest set bit at 10 -- 12-cell frame
#endif
#if MUT_44
      const bool shapes_differ = (ta.bits.size() == tb.bits.size());  // wrong -- should differ
#else
      const bool shapes_differ = (ta.bits.size() != tb.bits.size());
#endif
      checks.expect(ta.completed && tb.completed && shapes_differ &&
                     (int)ta.bits.size() == 7 && (int)tb.bits.size() == 12,
          "opus:8-serial.c113 opus:8-serial.c115 opus:8-serial.c125 "
          "opus:8-serial.c126 opus:8-serial.c127 D8: any number or "
          "combination of data and stop bits may be specified, and SERDAT "
          "controls the transmitted format -- two patterns differing in "
          "where their highest set bit sits (0x0031, 0x0709) each produce "
          "their own, differently-shaped, correct frame [p.258 \"Any "
          "number or combination of data and stop bits may be specified "
          "this way.\", \"...allows you to control the format (appearance) "
          "of the transmitted data.\"]");
    }

    // D9 -- SERDAT is 16 bits wide (and the figures' own ruler spans all
    // 16, MSB left): a word using the top bit shifts out in full.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
#if MUT_45
      TxResult t = capture_transmit(h, 10, 0x0001, cb, 20);  // bit 15 never exercised
#else
      TxResult t = capture_transmit(h, 10, 0x8001, cb, 20);
#endif
      checks.expect(t.completed && (int)t.bits.size() == 17 && t.bits[16] == 1,
          "opus:8-serial.c114 opus:8-serial.c118 opus:8-serial.c124 D9: "
          "SERDAT is a 16-bit register, spanning bit 15 down to bit 0 "
          "(the figures' own ruler) -- a word using the top bit (0x8001) "
          "shifts out in full, 17 cells (start + 16 data positions) with "
          "bit 15 arriving last, proving all sixteen bits are live "
          "[p.258 \"SERDAT is a 16-bit register...\"]");
    }

    // D11 -- the typical frame the figures depict, partially constructible.
    // The frame half reuses A5's own measurement (SERDAT=0x0155, 10
    // cells); the figure-numbering mismatch itself is a fact about the
    // manual's 1999 re-edit, not this hardware, and is recorded, not
    // graded.
    checks.record(
        "opus:8-serial.c116 D11 (p.258, \"To form a typical data sequence, "
        "such as one start bit, eight data bits, and one stop bit, you "
        "write into SERDAT the contents shown in Figures 8-12 and "
        "8-13.\"): partially graded. The frame half -- one start bit, "
        "eight data bits, one stop bit -- is the same case the A5 "
        "proposition already measures and grades elsewhere in this file "
        "(SERDAT=0x0155, 10 cells). The figure-numbering half is not: the "
        "printed page "
        "numbers the figures 8-11 and 8-12 (figures/p258.png's own "
        "caption, and the bracketed marker at ch8/p258.txt:57, both agree "
        "with the print) while this claim's text layer reads \"8-12 and "
        "8-13\" -- a one-off shift from the manual's 1999 re-edit. No "
        "port of paula_uart carries a figure number. Not graded here.");

    // D12 -- the figure's starting appearance: all-zero register, before
    // any nonzero word has been written. Graded together with E6's
    // all-zero-write case below (same idle-state measurement).
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
#if MUT_57
      h.write_reg(ADDR_SERDAT, 0x0155);  // one extra write before the sample -- leaves a transmission in flight, not idle
#endif
      // still-idle probe on txd/TSRE, before this harness ever writes a
      // nonzero word to SERDAT.
      const bool idle_txd_high = (h.dut->txd == 1);
      const uint16_t idle_status = h.read_reg(ADDR_SERDATR);
#if MUT_46
      const bool idle_ok = idle_txd_high && (idle_status & BIT_TSRE) == 0;  // wrong expected TSRE polarity
#else
      const bool idle_ok = idle_txd_high && (idle_status & BIT_TSRE) != 0;
#endif
      (void)cb;
      checks.expect(idle_ok,
          "opus:8-serial.c119 D12: the figure's starting appearance -- "
          "\"all zeros from last shift\" -- before any nonzero word has "
          "ever been written: txd idle high and TSRE already set, "
          "consistent with a genuinely idle, all-zero internal state "
          "[p.258, Figure 8-11 caption, \"Starting Appearance of SERDAT "
          "and Shift Register\"]");
    }

    // ===============================================================
    // E. Specifying the register contents (p.259)
    // ===============================================================

    // E2 -- THE FIXTURE-HAZARD-3 CLAIM. "Normally, either one or two stop
    // bits are sent." A fixture reading the level pattern on txd cannot
    // tell one stop bit from two -- both end in a run of 1-cells
    // indistinguishable from idle. Graded instead by total frame length in
    // bit cells, through the TSRE completion event: SERDAT=0x0155 (highest
    // set bit at 8, one-stop-bit shape) and SERDAT=0x0355 (highest set bit
    // at 9, two-stop-bit shape) must differ by exactly one bit cell,
    // measured on the completion-event timing, never by parsing the level
    // pattern by itself.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      TxResult one_stop = capture_transmit(h, 10, 0x0155, cb, 16);
#if MUT_59
      TxResult two_stop = capture_transmit(h, 10, 0x0155, cb, 16);  // same shape as one_stop -- the two frames no longer differ
#else
      TxResult two_stop = capture_transmit(h, 10, 0x0355, cb, 16);
#endif
#if MUT_47
      const int expected_diff = 0;  // wrong -- the fixture-hazard-3 trap: "the level pattern looks the same"
#else
      const int expected_diff = 1;
#endif
      checks.expect(one_stop.completed && two_stop.completed &&
                     (int)one_stop.bits.size() == 10 && (int)two_stop.bits.size() == 11 &&
                     ((int)two_stop.bits.size() - (int)one_stop.bits.size()) == expected_diff,
          "opus:8-serial.c136 E2: normally either one or two stop bits are "
          "sent -- SERDAT=0x0155 (one stop bit's shape) and SERDAT=0x0355 "
          "(two stop bits' shape) each complete correctly and differ by "
          "exactly one bit cell, measured through the transmit-shift-"
          "register-empty completion event (TSRE), not by reading the "
          "trailing 1-cells on txd -- both end in a run of 1s "
          "indistinguishable from idle by level alone [p.259 \"Normally, "
          "either one or two stop bits are sent.\"]");
      checks.note("E2: frame lengths -- one-stop shape = " +
                   std::to_string(one_stop.bits.size()) + " cells, two-stop "
                   "shape = " + std::to_string(two_stop.bits.size()) +
                   " cells (measured via TSRE, never via level-pattern "
                   "parsing, per this slice's own fixture hazard 3).");
    }

    // E3/E4 -- the start bit is transmitted independently of SERDAT's own
    // contents, and exactly one is generated, regardless of what bit 0 of
    // the written word holds -- including when bit 0 is itself 1.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      TxResult bit0_one = capture_transmit(h, 10, 0x0155, cb, 16);   // bit0 of written word = 1
#if MUT_60
      TxResult bit0_zero = capture_transmit(h, 10, 0x0157, cb, 16);  // bit0 of written word now 1, not 0 -- oracle still expects the second cell to read 0
#else
      TxResult bit0_zero = capture_transmit(h, 10, 0x0154, cb, 16);  // bit0 of written word = 0
#endif
#if MUT_48
      const bool start_always_zero = (bit0_one.bits[0] == 1) && (bit0_zero.bits[0] == 0);  // wrong -- expects start to follow bit0
#else
      const bool start_always_zero = (bit0_one.bits[0] == 0) && (bit0_zero.bits[0] == 0);
#endif
      checks.expect(bit0_one.completed && bit0_zero.completed && start_always_zero &&
                     bit0_one.bits[1] == 1 && bit0_zero.bits[1] == 0,
          "opus:8-serial.c137 opus:8-serial.c138 E3-E4: the start bit is "
          "transmitted independently of SERDAT's contents, and exactly "
          "one is generated before the first data bit -- the first "
          "transmitted cell is 0 whether the written word's bit 0 is 1 "
          "(0x0155) or 0 (0x0154), while the SECOND cell does follow bit "
          "0's actual value in each case [p.259 \"The transmission of the "
          "start bit is independent of the contents of this register. One "
          "start bit is automatically generated before the first data bit "
          "(bit 0) is sent.\"]");
    }

    // E5 -- writing SERDAT starts the data transmission (the ordinary
    // positive case).
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
#if MUT_49
      TxResult t = capture_transmit(h, 10, 0x0000, cb, 4);  // all-zero -- see E6, not the positive case
#else
      TxResult t = capture_transmit(h, 10, 0x0188, cb, 16);
#endif
      checks.expect(t.completed && (int)t.bits.size() == 10,
          "opus:8-serial.c139 E5: writing this register starts the data "
          "transmission -- a nonzero write (0x0188) produces a start bit "
          "followed by the written pattern [p.259 \"Writing this register "
          "starts the data transmission.\"]");
    }

    // E6 -- writing SERDAT with all zeros initiates no transmission.
    // RED AT BASELINE: this is the same printed sentence the slice's own
    // candidates file (S4-C1) names for the RTL's most-anticipated red in
    // this chapter -- measured independently here, on this package's own
    // uid, not imported from that candidate. A red here is a statement
    // about the Minimig line generally (src/minimig-aga is shared
    // lineage), not only this board -- see SPECIFICATION.md.
    { Harness h;
      const int cb = measure_cell_beats(h, 10);
      while (!h.dut->clk7_en) h.tick();
      h.dut->rga_i = (ADDR_SERDAT >> 1) & 0xFF;
      h.dut->data_i = 0x0000;
      for (int c = 0; c < 4; ++c) h.tick();
      h.dut->rga_i = 0; h.dut->data_i = 0;
      bool any_low_cell = false;
      for (int c = 0; c < cb * 4; ) {
        h.tick();
        if (h.dut->clk7_en) {
          ++c;
          if (h.dut->txd == 0) any_low_cell = true;
        }
      }
#if MUT_50
      const bool expect_no_transmission = false;  // oracle-green: matches the measured reality
#else
      const bool expect_no_transmission = true;  // the printed sentence
#endif
      const bool observed_no_transmission = !any_low_cell;
      checks.expect(observed_no_transmission == expect_no_transmission,
          "opus:8-serial.c140 E6: if this register is written with all "
          "zeros, no data transmission is initiated [p.259 \"If this "
          "register is written with all zeros, no data transmission is "
          "initiated.\"]");
      checks.note(std::string("E6: after SERDAT=0x0000, txd was observed to ") +
                   (any_low_cell ? "go low at least once" : "stay high throughout") +
                   " over 4 bit-cells' worth of clk7_en beats.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
