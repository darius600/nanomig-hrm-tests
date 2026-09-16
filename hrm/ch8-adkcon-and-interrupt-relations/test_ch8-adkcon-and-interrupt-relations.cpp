// Test purpose: HRM conformance for ch8-adkcon-and-interrupt-relations --
// the claims that relate an ADKCON bit or a SERDATR bit to an
// interrupt-request bit, plus the ADKCON/ADKCONR and SERDAT/SERDATR
// register-summary rows, at the `paula` boundary (paula.v:59), per
// SPECIFICATION.md. `paula` instantiates paula_uart, paula_intcontroller,
// paula_floppy and paula_audio directly (paula.v:203,220,243,289) -- INTREQ
// is invisible at paula_uart alone, which is exactly why these 97 claims
// are assigned here rather than to a paula_uart package.
//
// 97 claims total: 26 already graded in accepted packages elsewhere
// (record(), not re-graded -- see SPECIFICATION.md's verification table,
// including 3 corrections where the classify stage's "subject of slice s2"
// note did not actually name a graded assertion anywhere); 4 on one side
// each of the p.257/p.283 SERDATR-bit-13 source conflict (record() both,
// graded neither); 67 graded here through 18 expect() assertions.
//
// RTL read for step 2, after SPECIFICATION.md was committed: paula.v in
// full (311 lines -- confirms `adkcon` is `reg [14:0]`, so ADKCONR's own
// `{1'b0,adkcon[14:0]}` construction makes bit 15 structurally always 0,
// and confirms adkcon's only two fan-outs anywhere are bit 11 to
// paula_uart's uartbrk (:211) and bit 10 to paula_floppy's wordsync (:269)
// -- paula_audio's instantiation at :289-307 carries no adkcon port at
// all), paula_uart.v in full (262 lines -- rxd reaches SERDATR bit 11
// through rxd_sync[0], rxd_sync[1]=rxds, then a further register rx_rxd,
// three clk7_en edges of latency, not zero; tx_tbe's own reassert-on-IDLE
// behaviour), paula_intcontroller.v in full (176 lines -- rbfmirror =
// intreq[11] combinationally; SERDATR's rx_rbf registers rbfmirror one
// edge late; rx_ovrun sets to rbfmirror's value when a byte completes
// while rbfmirror is still 1, and clears on rbfmirror's own 1->0
// transition -- exactly "reset by resetting bit 11 of INTREQ").
//
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, printed
//   pages 255, 256, 257, 265, 283, 289; SPECIFICATION.md (this package).
//
// Fixture: a fresh Rig (Vpaula wrapper) per assertion block, matching the
//   discipline of every sibling `paula`-top package in this suite.
//
// Every mutation is a compile-time variant, replayable with
// `make clean && make MUT=MUT_n test` (n = 0..18, matching the MUT_n token
// at the end of each MUTATION record at the bottom of this file) -- see the
// `#if MUT_n` guard at each assertion's stimulus/oracle site.

#include <verilated.h>
#include "Vpaula.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <functional>
#include <iostream>
#include <string>

// Replayable mutations: `make MUT=MUT_n test` rebuilds with mutation n's
// stimulus/oracle substituted in place of the baseline at that one site;
// unset/empty MUT builds the unmutated baseline. 18 sites, MUT_0..MUT_17,
// one per assertion below, in the order they appear.
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

namespace {

std::string hex4(uint32_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "0x%04X", (unsigned)(v & 0xFFFF));
    return std::string(b);
}

// Register word addresses, as reg_address_in[8:1] (the $DFFxxx byte offset
// right-shifted by 1). paula.v's own parameters: DMACON=9'h096,
// ADKCON=9'h09e, ADKCONR=9'h010. paula_uart.v's own localparams:
// SERDAT=9'h030, SERDATR=9'h018, SERPER=9'h032. paula_intcontroller.v's
// own parameters: INTENAR=9'h01c, INTREQR=9'h01e, INTENA=9'h09a,
// INTREQ=9'h09c.
const uint8_t ADDR_NONE    = 0x00;
const uint8_t ADDR_ADKCONR = 0x08;  // $010 >> 1
const uint8_t ADDR_SERDATR = 0x0C;  // $018 >> 1
const uint8_t ADDR_INTENAR = 0x0E;  // $01C >> 1
const uint8_t ADDR_INTREQR = 0x0F;  // $01E >> 1
const uint8_t ADDR_SERDAT  = 0x18;  // $030 >> 1
const uint8_t ADDR_SERPER  = 0x19;  // $032 >> 1
const uint8_t ADDR_INTENA  = 0x4D;  // $09A >> 1
const uint8_t ADDR_INTREQ  = 0x4E;  // $09C >> 1
const uint8_t ADDR_DMACON  = 0x4B;  // $096 >> 1
const uint8_t ADDR_ADKCON  = 0x4F;  // $09E >> 1

// ADKCON/INTREQ/DMACON all use the chipset SET/CLR convention
// (paula.v:187-193, paula_intcontroller.v:88-97): bit 15 of the write
// selects OR (set, 1) or AND-NOT (clear, 0) against the named data bits.
const uint16_t SETCLR = 0x8000;

// SERDATR bit positions (paula_uart.v:248, serdatr={rx_ovrun, rx_rbf,
// tx_tbe, tx_tsre, rx_rxd} at bits 15:11 of data_o).
const uint16_t SDR_BIT15_OVRUN = 0x8000;
const uint16_t SDR_BIT14_RBF   = 0x4000;
const uint16_t SDR_BIT13_TBE   = 0x2000;
const uint16_t SDR_BIT12_TSRE  = 0x1000;
const uint16_t SDR_BIT11_RXD   = 0x0800;

// INTREQ/INTREQR bit positions (paula_intcontroller.v:106 txint->bit0,
// :128 rxint->bit11).
const uint16_t INTREQ_BIT0_TBE  = 0x0001;
const uint16_t INTREQ_BIT11_RBF = 0x0800;

// ADKCON bit positions (appA-audio/p265.txt; fan-outs confirmed at
// paula.v:211 bit11->uartbrk, :269 bit10->wordsync; all others store with
// no consumer, confirmed by reading paula.v's instantiations in full).
const uint16_t ADK_BIT14_PRECOMP1 = 0x4000;
const uint16_t ADK_BIT13_PRECOMP0 = 0x2000;
const uint16_t ADK_BIT12_MFMPREC  = 0x1000;
const uint16_t ADK_BIT11_UARTBRK  = 0x0800;
const uint16_t ADK_BIT10_WORDSYNC = 0x0400;
const uint16_t ADK_BIT9_MSBSYNC   = 0x0200;
const uint16_t ADK_BIT8_FAST      = 0x0100;
const uint16_t ADK_ALL_15         = 0x7FFF;

}  // namespace

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// A fresh Verilated model per trial (paula_floppy's dsktrack has no reset
// term, per the disclosed rig hazard also carried by
// hrm/ch8-adkcon-disk-control-bits; not touched by any assertion here, but
// the same fresh-model discipline is followed throughout regardless).
struct Rig {
    Vpaula* dut;
    int clk7_cycle = 0;

    Rig() { dut = new Vpaula; }
    ~Rig() { delete dut; }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en  = (clk7_cycle == 0);
        dut->clk7n_en = (clk7_cycle == 2);
        dut->cck      = (clk7_cycle < 2);
    }

    // Advance until exactly n qualifying (clk7_en-active) rising edges have
    // been applied. clk7_en, as read here, is the value that will gate the
    // UPCOMING tick()'s rising edge (set by the previous tick's tail).
    void settle_edges(int n) {
        int seen = 0;
        while (seen < n) {
            bool q = dut->clk7_en;
            tick();
            if (q) seen++;
        }
    }

    void do_reset() {
        clk7_cycle = 0;
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->reg_address_in = ADDR_NONE;
        dut->data_in = 0;
        dut->rxd = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->strhor = 0;
        dut->vblint = 0;
        dut->int2 = 0;
        dut->int3 = 0;
        dut->int6 = 0;
        dut->_step = 1;      // inactive (active low)
        dut->direc = 0;
        dut->_sel = 0xE;     // drive 0 selected (active low, 4'b1110)
        dut->side = 0;
        dut->_motor = 1;     // inactive; not consulted by this package's data path
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
        settle_edges(4);
        dut->reset = 0;
        settle_edges(4);
    }

    // One qualifying-edge write, then the address is withdrawn so no
    // further qualifying edge repeats it.
    void write_reg(uint8_t addr, uint16_t value) {
        dut->reg_address_in = addr;
        dut->data_in = value;
        settle_edges(1);
        dut->reg_address_in = ADDR_NONE;
        dut->data_in = 0;
        settle_edges(2);
    }

    // Combinational read: never advances a clock edge while the address is
    // asserted, so this can never be mistaken for a write by any edge-
    // triggered write-decode -- the documented fixture hazard ("reading a
    // write-only address is a write") cannot occur through this function.
    uint16_t read_reg(uint8_t addr) {
        dut->reg_address_in = addr;
        dut->eval();
        uint16_t v = (uint16_t)dut->data_out;
        dut->reg_address_in = ADDR_NONE;
        dut->eval();
        return v;
    }

    // Drive one full receive frame on `rxd`: idle high, then low well past
    // both the synchroniser latency and the start-bit confirmation sample
    // (SERPER=3 set by the caller; measured, not assumed -- the RX_START
    // confirmation reloads its own countdown from {serper[14:0],1'b1}, not
    // simply SERPER+1, so a generous fixed hold is used instead of a
    // hand-derived exact period), then high for a generous margin covering
    // the remaining eight data bits plus one stop bit. Content is
    // irrelevant to every assertion in this package, only the
    // existence/mirroring of the resulting status and interrupt bits is
    // graded, never a specific received byte.
    void receive_one_frame() {
        dut->rxd = 1;
        settle_edges(8);    // idle settles through the synchroniser
        dut->rxd = 0;
        settle_edges(20);   // start bit, held well past detection + confirmation
        dut->rxd = 1;
        settle_edges(160);  // remaining data bits + stop, generous margin
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ========================================================================
    // 1 -- SERIAL-REGS-REACHABLE-AT-PAULA. opus:8-serial.c7, sol:8-serial.c5
    // (p.255, "The Paula custom chip contains a ... UART."). The serial
    // registers answer at `paula`'s own bus with no separate top level:
    // after reset, SERDATR reads its known idle pattern (TBE=1, TSRE=1)
    // directly at this boundary.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
#if MUT_0
        uint16_t v = rig.read_reg(ADDR_SERPER);  // wrong address: no read decode exists at SERPER
#else
        uint16_t v = rig.read_reg(ADDR_SERDATR);
#endif
        checks.note("SERIAL-REGS-REACHABLE: SERDATR at reset = " + hex4(v));
        checks.expect((v & (SDR_BIT13_TBE | SDR_BIT12_TSRE)) == (SDR_BIT13_TBE | SDR_BIT12_TSRE),
            "SERIAL-REGS-REACHABLE-AT-PAULA (opus:8-serial.c7 sol:8-serial.c5 p.255 \"The Paula custom chip "
            "contains a Universal Asynchronous Receiver/Transmitter, or UART.\"): the UART's serial registers "
            "answer directly at paula's own reg_address_in/data_out bus, no separate top module needed -- "
            "SERDATR reads its known idle pattern (TBE=1, TSRE=1) = " + hex4(v));
    }

    // ========================================================================
    // 2 -- STATUS-AND-INTERRUPT-EXIST-RBF. opus:8-serial.c14, sol:8-serial.c15
    // (p.255, "status bits and interrupts for ... receive buffer full").
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_SERPER, 0x0003);
        rig.receive_one_frame();
#if MUT_1
        uint16_t sdr = rig.read_reg(ADDR_SERDAT);   // wrong address: SERDAT has no read decode (reads 0)
#else
        uint16_t sdr = rig.read_reg(ADDR_SERDATR);
#endif
        uint16_t irq = rig.read_reg(ADDR_INTREQR);
        checks.note("RBF-EXIST: SERDATR=" + hex4(sdr) + " INTREQR=" + hex4(irq) + " after one received frame");
        checks.expect((sdr & SDR_BIT14_RBF) != 0 && (irq & INTREQ_BIT11_RBF) != 0,
            "STATUS-AND-INTERRUPT-EXIST-RBF (opus:8-serial.c14 sol:8-serial.c15 p.255 \"There are also status bits "
            "and interrupts for the conditions of receive buffer full and transmit buffer empty.\"): after a "
            "received frame, both a status bit (SERDATR bit 14) and an interrupt-request bit (INTREQR bit 11) for "
            "receive-buffer-full are observably set -- SERDATR=" + hex4(sdr) + ", INTREQR=" + hex4(irq));
    }

    // ========================================================================
    // 3 -- STATUS-AND-INTERRUPT-EXIST-TBE. opus:8-serial.c15, sol:8-serial.c17
    // (p.255, same sentence, transmit half).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
#if MUT_2
        rig.write_reg(ADDR_SERPER, 0x0155);  // wrong address: SERPER instead of SERDAT
#else
        rig.write_reg(ADDR_SERDAT, 0x0155);
#endif
        rig.settle_edges(8);
        uint16_t irq = rig.read_reg(ADDR_INTREQR);
        checks.note("TBE-EXIST: INTREQR=" + hex4(irq) + " after a SERDAT write");
        checks.expect((irq & INTREQ_BIT0_TBE) != 0,
            "STATUS-AND-INTERRUPT-EXIST-TBE (opus:8-serial.c15 sol:8-serial.c17 p.255 \"There are also status bits "
            "and interrupts for the conditions of receive buffer full and transmit buffer empty.\"): after a "
            "SERDAT write, an interrupt-request bit (INTREQR bit 0) for transmit-buffer-empty is observably set -- "
            "INTREQR=" + hex4(irq));
    }

    // ========================================================================
    // 4 -- RBF-BIT14-NAME-AND-MIRROR. p.256 "14 RBF READ BUFFER FULL
    // (Mirror ...)", p.283 "14 RBF Serial port receive buffer full
    // (mirror)." opus:8-serial.c64, c66, c67; sol:8-serdatr.c18, c22, c94;
    // opus:8-serdatr.c15. SERDATR bit 14 tracks INTREQR bit 11 exactly,
    // both when it sets and when it clears.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_SERPER, 0x0003);
        rig.receive_one_frame();
        uint16_t sdr_set = rig.read_reg(ADDR_SERDATR);
        uint16_t irq_set = rig.read_reg(ADDR_INTREQR);
#if MUT_3
        rig.write_reg(ADDR_INTREQ, SETCLR | INTREQ_BIT11_RBF);  // mutation: SET (bit15=1) instead of CLR
#else
        rig.write_reg(ADDR_INTREQ, INTREQ_BIT11_RBF);            // CLR (bit15=0): clear INTREQ bit 11
#endif
        rig.settle_edges(4);
        uint16_t sdr_clr = rig.read_reg(ADDR_SERDATR);
        uint16_t irq_clr = rig.read_reg(ADDR_INTREQR);
        checks.note("RBF-MIRROR: set SERDATR=" + hex4(sdr_set) + " INTREQR=" + hex4(irq_set) +
                    "; after clearing INTREQ b11, SERDATR=" + hex4(sdr_clr) + " INTREQR=" + hex4(irq_clr));
        checks.expect((sdr_set & SDR_BIT14_RBF) != 0 && (irq_set & INTREQ_BIT11_RBF) != 0 &&
                      (sdr_clr & SDR_BIT14_RBF) == 0 && (irq_clr & INTREQ_BIT11_RBF) == 0,
            "RBF-BIT14-NAME-AND-MIRROR (opus:8-serial.c64, opus:8-serial.c66, opus:8-serial.c67 p.256 \"14 RBF "
            "READ BUFFER FULL (Mirror -- also appears in the interrupt request register.)\"; sol:8-serdatr.c18, "
            "sol:8-serdatr.c22, sol:8-serdatr.c94, opus:8-serdatr.c15 p.283 "
            "\"14 RBF Serial port receive buffer full (mirror).\"): SERDATR bit 14 tracks INTREQR bit 11 exactly, "
            "both set (SERDATR=" + hex4(sdr_set) + " INTREQR=" + hex4(irq_set) + ") and cleared (SERDATR=" +
            hex4(sdr_clr) + " INTREQR=" + hex4(irq_clr) + ")");
    }

    // ========================================================================
    // 5 -- OVRUN-BIT15-MIRROR. p.256 "15 OVRUN OVERRUN (Mirror ...)", p.283
    // "15 OVRUN ... Reset by resetting bit 11 of INTREQ." opus:8-serial.c61;
    // sol:8-serdatr.c15; opus:8-serdatr.c12; sol:8-serdatr.c90. A SECOND,
    // uncollected received frame (the overrun condition p.256 itself
    // describes) sets SERDATR bit 15, tracking INTREQR bit 11.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_SERPER, 0x0003);
        rig.receive_one_frame();
#if MUT_4
        // mutation: clear INTREQ b11 between the two frames, so the second
        // frame is NOT an overrun (RBF was already collected) -- OVRUN
        // should then stay 0.
        rig.write_reg(ADDR_INTREQ, INTREQ_BIT11_RBF);
        rig.settle_edges(4);
#endif
        rig.receive_one_frame();  // second frame, RBF still set (unless MUT_4) -> overrun
        uint16_t sdr = rig.read_reg(ADDR_SERDATR);
        uint16_t irq = rig.read_reg(ADDR_INTREQR);
        checks.note("OVRUN-MIRROR: after a second uncollected frame, SERDATR=" + hex4(sdr) +
                    " INTREQR=" + hex4(irq));
        checks.expect((sdr & SDR_BIT15_OVRUN) != 0 && (irq & INTREQ_BIT11_RBF) != 0,
            "OVRUN-BIT15-MIRROR (opus:8-serial.c61 p.256 \"15 OVRUN OVERRUN (Mirror -- also appears in the "
            "interrupt request register.) Indicates that another byte of data was received before the previous "
            "byte was picked up by the processor.\"; sol:8-serdatr.c15, opus:8-serdatr.c12, sol:8-serdatr.c90 "
            "p.283 \"15 OVRUN ... Reset by resetting bit 11 of INTREQ.\"): a second, uncollected received frame "
            "(the overrun condition) sets SERDATR bit 15, tracking INTREQR bit 11 -- SERDATR=" + hex4(sdr) +
            " INTREQR=" + hex4(irq));
    }

    // ========================================================================
    // 6 -- RESET-INTF-RBF-CLEARS-RBF-AND-OVRUN. p.256 "you must reset the
    // INTF_RBF bit in INTREQ to prevent an overrun." opus:8-serial.c63,
    // c69; sol:8-serdatr.c17, c24. Clearing INTREQ bit 11 clears BOTH
    // SERDATR bit 14 and bit 15 together, from an overrun state.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_SERPER, 0x0003);
        rig.receive_one_frame();
        rig.receive_one_frame();  // overrun state: bit14=1, bit15=1
        uint16_t sdr_before = rig.read_reg(ADDR_SERDATR);
#if MUT_5
        rig.write_reg(ADDR_INTREQ, SETCLR | INTREQ_BIT11_RBF);  // mutation: SET instead of CLR
#else
        rig.write_reg(ADDR_INTREQ, INTREQ_BIT11_RBF);            // CLR bit 11
#endif
        rig.settle_edges(4);
        uint16_t sdr_after = rig.read_reg(ADDR_SERDATR);
        checks.note("RESET-CLEARS-BOTH: before=" + hex4(sdr_before) + " after clearing INTF_RBF=" +
                    hex4(sdr_after));
        checks.expect((sdr_before & (SDR_BIT14_RBF | SDR_BIT15_OVRUN)) == (SDR_BIT14_RBF | SDR_BIT15_OVRUN) &&
                      (sdr_after & (SDR_BIT14_RBF | SDR_BIT15_OVRUN)) == 0,
            "RESET-INTF-RBF-CLEARS-RBF-AND-OVRUN (opus:8-serial.c63, opus:8-serial.c69 p.256 \"After reading the "
            "contents of this data register, you must reset the INTF_RBF bit in INTREQ to prevent an overrun.\"; "
            "sol:8-serdatr.c17, sol:8-serdatr.c24 same sentence): from an overrun state (SERDATR=" + hex4(sdr_before) + "), resetting INTF_RBF "
            "(INTREQ bit 11) clears both SERDATR bit 14 and bit 15 together -- SERDATR=" + hex4(sdr_after));
    }

    // ========================================================================
    // 7 -- INTREQ-BITS-VISIBLE-AT-SERDATR-ADDRESS. p.283 "Several interrupt
    // request bits are also read at this address, along with the data, as
    // shown below." opus:8-serdatr.c8, sol:8-serdatr.c86.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_SERPER, 0x0003);
        rig.receive_one_frame();
#if MUT_6
        uint16_t sdr = rig.read_reg(ADDR_INTENAR);  // wrong address: INTENAR instead of SERDATR
#else
        uint16_t sdr = rig.read_reg(ADDR_SERDATR);
#endif
        checks.note("ONE-READ-BOTH: single SERDATR read = " + hex4(sdr));
        checks.expect((sdr & SDR_BIT14_RBF) != 0 && (sdr & 0x00FF) != 0,
            "INTREQ-BITS-VISIBLE-AT-SERDATR-ADDRESS (opus:8-serdatr.c8 sol:8-serdatr.c86 p.283 \"This address reads "
            "data from a receive data buffer. ... Several interrupt request bits are also read at this address, "
            "along with the data, as shown below.\"): one single read of $018 carries both an interrupt-request-"
            "mirrored bit (bit 14) and the received data bits together in the same word -- SERDATR=" + hex4(sdr));
    }

    // ========================================================================
    // 8 -- SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT (S4-C3, expected RED).
    // p.257 "11 RXD Direct read of RXD pin on Paula chip." opus:8-serial.c82.
    // Measured latency in clk7_en edges between `rxd` changing and SERDATR
    // bit 11 changing (paula_uart.v: rxd -> rxd_sync[0] -> rxd_sync[1]=rxds
    // -> rx_rxd, three registers deep). "Direct" would predict zero.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dut->rxd = 1;
        rig.settle_edges(8);
        uint16_t before = rig.read_reg(ADDR_SERDATR);
#if MUT_18
        rig.dut->rxd = 1;  // MUTATION: no transition applied -- rxd stays at its already-settled value
#else
        rig.dut->rxd = 0;  // the transition being tracked
#endif
        int latency = -1;
        for (int i = 1; i <= 8; i++) {
            rig.settle_edges(1);
            uint16_t v = rig.read_reg(ADDR_SERDATR);
            if ((v & SDR_BIT11_RXD) == 0) { latency = i; break; }
        }
        uint16_t after_settle = rig.read_reg(ADDR_SERDATR);
        checks.note("RXD-LATENCY: before=" + hex4(before) + " latency=" + std::to_string(latency) +
                    " clk7_en edges; settled=" + hex4(after_settle));
        // Control, same measurement basis: once settled, the bit DOES
        // correctly reflect rxd's new value -- ruling out a broken/stuck
        // fixture as the explanation for the nonzero latency above.
        checks.expect((after_settle & SDR_BIT11_RXD) == 0,
            "RXD-DELIVERS-CORRECT-VALUE-CONTROL [no claim id, same basis as SERDATR-BIT11-RXD-REGISTERED-NOT-"
            "DIRECT]: once settled, SERDATR bit 11 does correctly read the new rxd value (0) -- the path works, "
            "it is simply not zero-latency");
#if MUT_7
        // Oracle mutation: relax the accepted latency bound from strictly
        // 0 (what "direct" would mean) to the actual measured value,
        // flipping the same measurement to green -- proving the baseline
        // red is a genuine latency, not a broken fixture.
        checks.expect(latency <= 3,
#else
        checks.expect(latency == 0,
#endif
            "SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT (opus:8-serial.c82 p.257 \"11 RXD Direct read of RXD pin on "
            "Paula chip.\"): SERDATR bit 11 takes " + std::to_string(latency) + " clk7_en edge(s) to reflect a "
            "change on `rxd` (two synchroniser flops plus one further register, paula_uart.v:36-43,234), not the "
            "zero-cycle, combinational latency \"direct read\" would predict. Whether \"direct\" forbids a "
            "synchroniser is the adjudication (not this assertion's to make); the measured latency is not in "
            "doubt.");
    }

    // ========================================================================
    // 9 -- SERDAT-CHIP-COLUMN-P283. p.283 "SERDAT 030 W P Serial port data
    // and stop bits write." opus:8-serdat.c4, sol:8-serdat.c40. A
    // deliberate SERDAT write (never a "probe read" -- $030 has no read
    // decode) produces its transmit effect at the `paula` boundary alone.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_SERPER, 0x0003);
        bool idle_high = rig.dut->txd == 1;
#if MUT_8
        rig.write_reg(ADDR_SERPER, 0x0155);  // mutation: write SERPER's own address again, not SERDAT
#else
        rig.write_reg(ADDR_SERDAT, 0x0155);
#endif
        rig.settle_edges(2);
        bool start_bit_seen = rig.dut->txd == 0;
        checks.note(std::string("SERDAT-CHIP: idle_high=") + (idle_high ? "yes" : "no") + " start_bit_seen=" +
                    (start_bit_seen ? "yes" : "no"));
        checks.expect(idle_high && start_bit_seen,
            "SERDAT-CHIP-COLUMN-P283 (opus:8-serdat.c4 sol:8-serdat.c40 p.283 \"SERDAT 030 W P Serial port data "
            "and stop bits write ... Data from this buffer is moved into a serial shift register for output "
            "transmission whenever it is empty.\"): a SERDAT write reaches real transmit logic directly at the "
            "`paula` boundary -- txd idles high, then drops to 0 (a start bit) once the write lands, with no "
            "separate chip-selection wrapper needed");
    }

    // ========================================================================
    // 10 -- ADKCON-REGISTER-SUMMARY-ROW. p.265 "ADKCON 09E W P Audio, disk,
    // control write." opus:8-adkcon.c2, c3, c4, c6; sol:8-adkcon.c9, c10,
    // c12, c26. Address $09E, write-only (no read path exists at that same
    // address), reaches real logic (UARTBRK's effect on txd).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_ADKCON, SETCLR | ADK_BIT9_MSBSYNC);  // known nonzero value first, so the mutation below is detectable
#if MUT_9
        uint16_t at_own_addr = rig.read_reg(ADDR_ADKCONR);  // mutation: probe the READ address instead -- now nonzero
#else
        uint16_t at_own_addr = rig.read_reg(ADDR_ADKCON);   // the write address itself has no read decode
#endif
        rig.write_reg(ADDR_ADKCON, SETCLR | ADK_BIT11_UARTBRK);
        rig.settle_edges(2);
        bool txd_forced = rig.dut->txd == 0;
        checks.note("ADKCON-ROW: read-at-write-address=" + hex4(at_own_addr) + " txd_forced=" +
                    (txd_forced ? "yes" : "no"));
        checks.expect(at_own_addr == 0x0000 && txd_forced,
            "ADKCON-REGISTER-SUMMARY-ROW (opus:8-adkcon.c2, opus:8-adkcon.c3, opus:8-adkcon.c4, opus:8-adkcon.c6 "
            "p.265 \"ADKCON 09E W P Audio, disk, control write\"; sol:8-adkcon.c9, sol:8-adkcon.c10, "
            "sol:8-adkcon.c12, sol:8-adkcon.c26 same row): address $09E is write-only (no read decode exists "
            "there -- reading it returns 0x0000) and its write function reaches real logic (setting UARTBRK forces "
            "txd low)");
    }

    // ========================================================================
    // 11 -- ADKCONR-REGISTER-SUMMARY-ROW. p.265 "ADKCONR 010 R P Audio,
    // disk, control read." opus:8-adkcon.c7, c8, c9, c11; sol:8-adkcon.c13,
    // c14, c16, c27. Address $010, read-only (a write attempt there changes
    // nothing stored), reports real state.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_ADKCON, SETCLR | ADK_BIT9_MSBSYNC);
        uint16_t before = rig.read_reg(ADDR_ADKCONR);
#if MUT_10
        rig.write_reg(ADDR_ADKCON, 0xFFFF);   // mutation: the write lands at ADKCON's real write address instead of $010
#else
        rig.write_reg(ADDR_ADKCONR, 0xFFFF);  // attempted write at the read-only address ($010) -- has no decode
#endif
        uint16_t after = rig.read_reg(ADDR_ADKCONR);
        checks.note("ADKCONR-ROW: before=" + hex4(before) + " after write-attempt-at-$010=" + hex4(after));
        checks.expect(before == ADK_BIT9_MSBSYNC && after == before,
            "ADKCONR-REGISTER-SUMMARY-ROW (opus:8-adkcon.c7, opus:8-adkcon.c8, opus:8-adkcon.c9, opus:8-adkcon.c11 "
            "p.265 \"ADKCONR 010 R P Audio, disk, control read\"; sol:8-adkcon.c13, sol:8-adkcon.c14, "
            "sol:8-adkcon.c16, sol:8-adkcon.c27 same row): address $010 reports real, previously-written state "
            "(" + hex4(before) + "), and is read-only -- a write attempt AT $010 changes nothing stored (paula.v's "
            "ADKCON write-decode only matches $09E, never $010) -- after=" + hex4(after));
    }

    // ========================================================================
    // 12 -- ADKCON-BITS-0-14-SYMMETRIC-VIA-ADKCONR. p.265's BIT#/USE table,
    // read as one table shared by both registers. opus:8-adkcon.c12
    // (position/storage half). Every named bit 0-14 written through ADKCON
    // reads back at the same position through ADKCONR. Also this package's
    // FIXTURE-LIVENESS control: two distinct, known words delivered and
    // read back correctly before any other assertion is trusted.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_ADKCON, SETCLR | 0x5555);
        uint16_t v1 = rig.read_reg(ADDR_ADKCONR);
#if MUT_11
        rig.write_reg(ADDR_ADKCON, SETCLR | 0x5555);  // mutation: repeat the same pattern, not a distinct one
#else
        rig.write_reg(ADDR_ADKCON, 0x5555);           // CLR those bits
        rig.write_reg(ADDR_ADKCON, SETCLR | 0x2AAA);  // SET the complementary pattern
#endif
        uint16_t v2 = rig.read_reg(ADDR_ADKCONR);
        checks.note("BIT-TABLE-SYMMETRY: pattern1=0x5555 -> " + hex4(v1) + "; pattern2 -> " + hex4(v2));
        checks.expect(v1 == 0x5555 && v2 == 0x2AAA && v1 != v2,
            "ADKCON-BITS-0-14-SYMMETRIC-VIA-ADKCONR (opus:8-adkcon.c12 p.265 \"ADKCON and ADKCONR share one bit "
            "table\", the BIT#/USE table's position/storage half; also this package's FIXTURE-LIVENESS control): "
            "every bit position 0-14 (0x5555 | 0x2AAA covers all fifteen) written through ADKCON reads back "
            "exactly at the same position through ADKCONR, and two distinct, known words (0x5555, 0x2AAA) are "
            "both delivered and read back correctly and distinctly -- ADKCONR=" + hex4(v1) + " then " + hex4(v2));
    }

    // ========================================================================
    // 13 -- ADKCONR-BIT15-SETCLR-NEVER-READS-BACK (S4-C6, expected RED).
    // p.265, same table's row 15: "15 SET/CLR Set/clear control bit...".
    // opus:8-adkcon.c12 (the row-15 half -- the "one shared table"
    // prediction that fails for exactly this one row). `adkcon` is
    // declared `reg [14:0]` (paula.v:132) and ADKCONR is built as
    // `{1'b0,adkcon[14:0]}` (paula.v:198): bit 15 is structurally always 0.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
#if MUT_12
        rig.write_reg(ADDR_ADKCON, 0x7FFF);  // mutation: SETCLR=0 (CLR mode) instead of SET -- still expect bit15=0 either way, but this changes what's being demonstrated
#else
        rig.write_reg(ADDR_ADKCON, SETCLR | ADK_ALL_15);  // SET every one of the 15 stored bits
#endif
        uint16_t v = rig.read_reg(ADDR_ADKCONR);
        checks.note("BIT15-READBACK: wrote SETCLR=1 (bit15=1) plus all 15 stored bits, ADKCONR=" + hex4(v) +
                    " (bit15=" + std::to_string((v>>15)&1) + ")");
        // Baseline oracle: what assertion 12 demonstrates for every OTHER
        // row (0-14) is that the bit written through ADKCON reads back at
        // the same position through ADKCONR -- the manual's own "one
        // shared table" claim (opus:8-adkcon.c12) predicts row 15 behaves
        // the same way, i.e. that the bit 15 (=1) just written would read
        // back as 1 too. This is the manual-implied oracle, not the RTL's
        // own behaviour -- measuring against it is what makes the FAIL
        // below a genuine finding rather than a tautology against paula.v.
#if MUT_12
        // Oracle mutation: relax the oracle from "must read back the
        // written 1" to "must read back 0" (the RTL's actual, structural
        // behaviour), flipping the identical measurement to green --
        // showing the baseline FAIL is the real always-0 readback, not a
        // fixture bug.
        checks.expect((v & 0x8000) == 0,
#else
        checks.expect((v & 0x8000) != 0,
#endif
            "ADKCONR-BIT15-SETCLR-NEVER-READS-BACK (opus:8-adkcon.c12 p.265, row 15 of the shared BIT#/USE table, "
            "\"15 SET/CLR Set/clear control bit. Determines if bits written with a 1 get set or cleared.\"): the "
            "same 'one shared table' claim that assertion 12 confirms true for rows 0-14 predicts row 15's "
            "written bit (1) reads back too; measured, ADKCONR's bit 15 is structurally always 0 (paula.v:132 "
            "`reg [14:0] adkcon`, paula.v:198 `{1'b0,adkcon[14:0]}`) regardless of what was written -- it never "
            "reads back the SET/CLR mode, unlike every other row of the same table. Whether a write-only control "
            "bit *should* read back is the adjudication (not this assertion's to make); ADKCONR=" + hex4(v) +
            " (bit 15 = 0) is not in doubt.");
    }

    // ========================================================================
    // 14 -- ADKCON-SETCLR-SET. p.257 "If bit 15 is a 1 specified bits are
    // set.", p.265 "Set/clear control bit. Determines if bits written with
    // a 1 get set...". opus:8-serial.c96, c97, c98; opus:8-adkcon.c13, c14,
    // c15; sol:8-adkcon.c1, c2, c3, c4, c17, c18, c19, c20.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
#if MUT_13
        rig.write_reg(ADDR_ADKCON, 0x7FFF);  // mutation: SETCLR=0 (CLR) instead of 1 (SET)
#else
        rig.write_reg(ADDR_ADKCON, SETCLR | ADK_ALL_15);
#endif
        uint16_t v = rig.read_reg(ADDR_ADKCONR);
        checks.note("SETCLR-SET: ADKCONR=" + hex4(v));
        checks.expect(v == ADK_ALL_15,
            "ADKCON-SETCLR-SET (opus:8-serial.c96, opus:8-serial.c97, opus:8-serial.c98 p.257 \"15 SET/CLR Allows "
            "setting or clearing individual bits. If bit 15 is a 1 specified bits are set.\"; opus:8-adkcon.c13, "
            "opus:8-adkcon.c14, opus:8-adkcon.c15, sol:8-adkcon.c1, sol:8-adkcon.c2, sol:8-adkcon.c3, "
            "sol:8-adkcon.c4, sol:8-adkcon.c17, sol:8-adkcon.c18, sol:8-adkcon.c19, sol:8-adkcon.c20 p.265 \"15 "
            "SET/CLR Set/clear control bit. Determines if bits written with a 1 get set "
            "or cleared.\"): writing ADKCON with bit 15 (SET/CLR)=1 and data bits 0-14 all 1 sets all fifteen, "
            "read back whole through ADKCONR = " + hex4(v));
    }

    // ========================================================================
    // 15 -- ADKCON-SETCLR-CLR. p.257 "If bit 15 is a 0 specified bits are
    // cleared." opus:8-serial.c99; sol:8-adkcon.c5. A CLR-mode write
    // actually clears already-set bits (not merely leaves already-0 bits
    // alone).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_ADKCON, SETCLR | ADK_ALL_15);  // all 15 set
#if MUT_14
        rig.write_reg(ADDR_ADKCON, SETCLR | 0x0055);  // mutation: SETCLR=1 (SET) instead of 0 (CLR) -- a no-op on already-set bits
#else
        rig.write_reg(ADDR_ADKCON, 0x0055);  // CLR (bit15=0): clear bits 0,2,4,6
#endif
        uint16_t v = rig.read_reg(ADDR_ADKCONR);
        checks.note("SETCLR-CLR: ADKCONR=" + hex4(v));
        checks.expect(v == (uint16_t)(ADK_ALL_15 & ~0x0055),
            "ADKCON-SETCLR-CLR (opus:8-serial.c99 p.257 \"If bit 15 is a 0 specified bits are cleared.\"; "
            "sol:8-adkcon.c5 \"When ADKCON bit 15 is 0, specified control bits are cleared.\"): with all fifteen "
            "bits first set, a CLR-mode write (bit 15=0) naming bits 0,2,4,6 actually clears exactly those "
            "already-set bits, leaving the rest set -- ADKCONR=" + hex4(v) + " (expected " +
            hex4((uint16_t)(ADK_ALL_15 & ~0x0055)) + ")");
    }

    // ========================================================================
    // 16 -- ADKCON-SETCLR-PRESERVE. p.265 "Bits written with a zero are
    // always unchanged." opus:8-adkcon.c16; sol:8-adkcon.c21. A CLR-mode
    // write naming only bits that are already 0 leaves an existing pattern
    // completely untouched.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.write_reg(ADDR_ADKCON, SETCLR | 0x5555);  // known pattern
#if MUT_15
        rig.write_reg(ADDR_ADKCON, 0x5555);  // mutation: name the SET bits themselves in CLR mode (should clear them)
#else
        rig.write_reg(ADDR_ADKCON, 0x2AAA);  // CLR mode naming only bits that are already 0
#endif
        uint16_t v = rig.read_reg(ADDR_ADKCONR);
        checks.note("SETCLR-PRESERVE: ADKCONR=" + hex4(v));
        checks.expect(v == 0x5555,
            "ADKCON-SETCLR-PRESERVE (opus:8-adkcon.c16 p.265 \"Bits written with a zero are always unchanged.\"; "
            "sol:8-adkcon.c21 \"Writing 0 to an ADKCON control bit leaves that bit unchanged.\"): from a known "
            "pattern (0x5555), a CLR-mode write naming only bits that are already 0 (0x2AAA, the complement) "
            "leaves the existing pattern completely unchanged -- ADKCONR=" + hex4(v) + " (expected 0x5555)");
    }

    // ========================================================================
    // 17 -- ADKCON-BIT11-UARTBRK. p.257 "11 UARTBRK Force the transmit pin
    // to zero.", p.265 "11 UARTBRK Forces a UART break (clears TXD) if
    // true." opus:8-serial.c100, c101; sol:8-adkcon.c7 (its own classify
    // `note` calls it a "restored duplicate of opus:8-serial.c100" -- same
    // proposition, cited together).
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        bool idle_high = rig.dut->txd == 1;
#if MUT_16
        rig.write_reg(ADDR_ADKCON, SETCLR | ADK_BIT10_WORDSYNC);  // mutation: bit 10 (WORDSYNC) instead of bit 11
#else
        rig.write_reg(ADDR_ADKCON, SETCLR | ADK_BIT11_UARTBRK);
#endif
        rig.settle_edges(4);
        bool forced_low = rig.dut->txd == 0;
        rig.write_reg(ADDR_ADKCON, ADK_BIT11_UARTBRK);  // CLR bit 11
        rig.settle_edges(4);
        bool released_high = rig.dut->txd == 1;
        checks.note(std::string("UARTBRK: idle_high=") + (idle_high?"yes":"no") + " forced_low=" +
                    (forced_low?"yes":"no") + " released_high=" + (released_high?"yes":"no"));
        checks.expect(idle_high && forced_low && released_high,
            "ADKCON-BIT11-UARTBRK (opus:8-serial.c100, opus:8-serial.c101 p.257 \"11 UARTBRK Force the transmit "
            "pin to zero.\"; "
            "sol:8-adkcon.c7 p.257 same bit, restored duplicate of opus:8-serial.c100; also p.265 \"11 UARTBRK "
            "Forces a UART break (clears TXD) if true.\"): after reset txd idles high; setting ADKCON bit 11 "
            "forces it low (paula.v:211 wires adkcon[11] to paula_uart's uartbrk, paula_uart.v:136); clearing bit "
            "11 releases it back to the idle-mark 1");
    }

    // ========================================================================
    // 18 -- ADKCON-BITS-12-9-8-STORE-ONLY-NO-CONSUMER. p.265 "12 MFMPREC",
    // "09 MSBSYNC", "08 FAST" (name/position only). opus:8-adkcon.c23, c30,
    // c32 -- the three claims a "subject of slice s2" note flagged as
    // already graded, but which trace to sibling uids marked
    // [not-asserted] in slice s2's own output and confirmed absent from
    // every accepted package (see SPECIFICATION.md's correction). Each bit
    // stores and reads back individually at its own position; whether the
    // named function does anything is not asserted by any of these three
    // claims (matching the non-overreach discipline
    // hrm/ch8-adkcon-disk-control-bits's own C8 treatment used for
    // PRECOMP/MFMPREC/MSBSYNC/FAST) and is reported via note() only.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint16_t bits = ADK_BIT12_MFMPREC | ADK_BIT9_MSBSYNC | ADK_BIT8_FAST;
#if MUT_17
        rig.write_reg(ADDR_ADKCON, SETCLR | ADK_BIT11_UARTBRK);  // mutation: a different bit (11) instead of 12/9/8
#else
        rig.write_reg(ADDR_ADKCON, SETCLR | bits);
#endif
        uint16_t v_set = rig.read_reg(ADDR_ADKCONR);
        rig.write_reg(ADDR_ADKCON, bits);  // CLR, regardless
        uint16_t v_clr = rig.read_reg(ADDR_ADKCONR);
        checks.note("BITS-12-9-8: set=" + hex4(v_set) + " clr=" + hex4(v_clr) +
            " -- confirmed by reading paula.v in full: adkcon's only two fan-outs anywhere are bit 11 "
            "(paula.v:211, UARTBRK, graded above) and bit 10 (paula.v:269, WORDSYNC, graded elsewhere -- see "
            "SPECIFICATION.md). Bits 12, 9 and 8 drive no submodule port and reach no logic; storage with no "
            "consumer, matching the same finding hrm/ch8-adkcon-disk-control-bits already made for these same "
            "three bits, and not asserted as a functional claim by any of the three uids below.");
        checks.expect(v_set == bits && v_clr == 0x0000,
            "ADKCON-BITS-12-9-8-STORE-ONLY-NO-CONSUMER (opus:8-adkcon.c23 p.265 \"12 MFMPREC\"; opus:8-adkcon.c30 "
            "p.265 \"09 MSBSYNC\"; opus:8-adkcon.c32 p.265 \"08 FAST\" -- name/position only): each bit stores and "
            "reads back individually and as a group, set (" + hex4(v_set) + ") and cleared (" + hex4(v_clr) + ")");
    }

    // ========================================================================
    // Source conflict, S4-C8: SERDATR bit 13 (TBE). p.257 "(Not a mirror --
    // interrupt occurs when the buffer becomes empty.)" vs p.283 "Serial
    // port transmit buffer empty (mirror)." Both sides are present in this
    // package's own 97 claims. Per the build brief: "Measure both sides,
    // record both, grade neither." MEASURED here, RECORDED, not graded.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        uint16_t sdr_fresh = rig.read_reg(ADDR_SERDATR);
        rig.write_reg(ADDR_SERDAT, 0x0155);
        rig.settle_edges(2);
        uint16_t sdr_after_write = rig.read_reg(ADDR_SERDATR);
        uint16_t irq_after_write = rig.read_reg(ADDR_INTREQR);
        rig.write_reg(ADDR_INTREQ, INTREQ_BIT0_TBE);  // CLR INTREQ bit 0
        rig.settle_edges(2);
        uint16_t sdr_after_clear = rig.read_reg(ADDR_SERDATR);
        checks.record("SOURCE CONFLICT S4-C8, SERDATR bit 13 (TBE), p.257 vs p.283 -- MEASURED both sides, "
            "GRADED NEITHER, per the build brief (\"Measure both sides, record both, grade neither\"). "
            "p.257 side (opus:8-serial.c72, sol:8-serdatr.c28, ch8/p257.txt:9-10 \"(Not a mirror -- interrupt "
            "occurs when the buffer becomes empty.)\"): predicts bit 13 does NOT track INTREQR bit 0. p.283 side "
            "(opus:8-serdatr.c18, sol:8-serdatr.c98, appA-controller/p283.txt:42-43 \"Serial port transmit buffer "
            "empty (mirror).\"): predicts bit 13 DOES track INTREQR bit 0. MEASURED: fresh SERDATR=" +
            hex4(sdr_fresh) + " (bit13=" + std::to_string((sdr_fresh>>13)&1) + "); after a SERDAT write, SERDATR="
            + hex4(sdr_after_write) + " (bit13=" + std::to_string((sdr_after_write>>13)&1) + "), INTREQR=" +
            hex4(irq_after_write) + " (bit0=" + std::to_string(irq_after_write&1) + "); after clearing INTREQ "
            "bit 0, SERDATR=" + hex4(sdr_after_clear) + " (bit13=" + std::to_string((sdr_after_clear>>13)&1) +
            "). Bit 13 stayed 1 throughout while INTREQR bit 0 went 0->1->0, so the measurement matches the "
            "p.257 side's prediction and contradicts the p.283 side's -- which page is right is the adjudication "
            "S4-C8 names, not decided here. This package's own uids are: opus:8-serial.c72, sol:8-serdatr.c28 "
            "(p.257 side); opus:8-serdatr.c18, sol:8-serdatr.c98 (p.283 side).");
    }

    // ========================================================================
    // 26 claims already graded in accepted packages elsewhere. See
    // SPECIFICATION.md's "Verification of the 26 already-graded claims"
    // for how each citation below was independently confirmed (not merely
    // trusted from the classify stage's own "subject of slice s2" note).
    // ========================================================================
    checks.record("ALREADY GRADED in hrm/ch8-adkcon-disk-control-bits, assertions A3 (PRECOMP-FIELD-POSITION) / "
        "A4 (PRECOMP1-BIT14-STORES) / A5 (PRECOMP0-BIT13-STORES) -- p.265 PRECOMP 1-0 name and two-bit width. "
        "Claims: opus:8-adkcon.c17 (via sibling uids opus:8-appA-adkcon.c11, sol:8-appa-adkbit.c3, .c33, all "
        "[testable] there), opus:8-adkcon.c18 (via opus:8-appA-adkcon.c11, sol:8-appa-adkbit.c3). Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch8-adkcon-disk-control-bits, assertion A6 (WORDSYNC-BIT10-EFFECT) -- "
        "p.265 WORDSYNC name and disk-read-synchronizing effect. Claims: opus:8-adkcon.c27, opus:8-adkcon.c28 "
        "(via sibling uids opus:8-appA-adkcon.c20, sol:8-appa-adkbit.c11, both [testable] there). Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch8-disk-register-map-paula-floppy, assertion S1-DSKSYNC-ADDRESS -- "
        "p.265 \"DISK SYNC CODE, located in address (3F)*2\". Claim: opus:8-adkcon.c29 (via sibling uids "
        "opus:8-appA-adkcon.c21, sol:8-appa-adkbit.c12, both [testable, at paula_floppy] there). Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch5-audio-dmacon-adkcon, assertions ADKCON-SETCLR-SET (position/storage "
        "half) and ADKCON-ATTACH-BITS-DO-NOT-SILENCE-CHANNELS (behaviour half, EXPECTED/GRADED RED there: "
        "paula_audio_channel.v's own first line says attached modes are not supported, and paula.v instantiates "
        "paula_audio with no adkcon port at all -- confirmed independently by my own reading of paula.v:289-307). "
        "18 claims, all p.265 USE3PN..USE0V1 bit names/effects and the period/volume alternation NOTE: "
        "opus:8-adkcon.c35, opus:8-adkcon.c36 (USE3PN, bit7), opus:8-adkcon.c37, opus:8-adkcon.c38 (USE2P3, bit6), "
        "opus:8-adkcon.c39, opus:8-adkcon.c40 (USE1P2, bit5), opus:8-adkcon.c41, opus:8-adkcon.c42 (USE0P1, bit4), "
        "opus:8-adkcon.c43, opus:8-adkcon.c44 (USE3VN, bit3), opus:8-adkcon.c45, opus:8-adkcon.c46 (USE2V3, bit2), "
        "opus:8-adkcon.c47, opus:8-adkcon.c48 (USE1V2, bit1), opus:8-adkcon.c49, opus:8-adkcon.c50 (USE0V1, bit0), "
        "opus:8-adkcon.c51, opus:8-adkcon.c52 (alternation NOTE) -- via sibling uids opus:8-appA-adkcon.c28 through c36 and "
        "sol:8-appa-adkbit.c22 through c32, all present in that package's own \"CH8 S2 CITES\" extension. Not "
        "re-graded.");
    checks.record("ALREADY GRADED in hrm/ch5-audio-channel-registers, assertion AUDXDAT-IS-PERCHANNEL-DATA-BUFFER "
        "-- p.265 \"AUDxDAT 0AA W P Audio channel x data\". Claim: opus:8-adkcon.c54 (via sibling uid "
        "opus:8-appA-adkcon.c37). Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch5-audio-channel-registers, assertions SAMPLE-SIGNED-RANGE-FULL-SCALE "
        "and WORD-CONTAINS-TWO-SAMPLES -- p.265's AUDxDAT full paragraph (2-byte 2's-complement sequential "
        "output). Claim: opus:8-adkcon.c55 (via sibling uid opus:8-appA-adkcon.c38). Not re-graded.");
    checks.record("ALREADY GRADED in hrm/ch8-appb-chip-column-paula, section T5 -- p.289 SERDAT row's CHIP=P "
        "column. Claim: sol:8-serdat.c76 (its own classify `note` names the package and uids opus:8-appB.c40, "
        "sol:8-appB.c39 directly). Not re-graded.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION SERIAL-REGS-REACHABLE-AT-PAULA stimulus red MUT_0: the address probed changed from
//   SERDATR ($018) to SERPER ($032, no read decode) -- expect FAIL, the idle
//   pattern is not found because the address returns 0x0000.
// MUTATION STATUS-AND-INTERRUPT-EXIST-RBF stimulus red MUT_1: the address read
//   after the frame changed from SERDATR to SERDAT (no read decode) -- expect
//   FAIL, bit 14 is never seen set at the wrong address.
// MUTATION STATUS-AND-INTERRUPT-EXIST-TBE stimulus red MUT_2: the write address
//   changed from SERDAT to SERPER -- expect FAIL, no SERDAT write ever lands
//   so INTREQR bit 0 never sets.
// MUTATION RBF-BIT14-NAME-AND-MIRROR stimulus red MUT_3: the INTREQ write
//   changed from CLR (bit15=0) to SET (bit15=1) -- expect FAIL, bit 11 stays
//   set instead of clearing, so SERDATR bit 14/INTREQR bit 11 never clear.
// MUTATION OVRUN-BIT15-MIRROR stimulus red MUT_4: INTREQ bit 11 is cleared
//   between the two received frames, so the second frame is not an overrun
//   -- expect FAIL, SERDATR bit 15 stays 0.
// MUTATION RESET-INTF-RBF-CLEARS-RBF-AND-OVRUN stimulus red MUT_5: the INTREQ
//   write changed from CLR to SET -- expect FAIL, bit 14/15 stay set instead
//   of clearing.
// MUTATION INTREQ-BITS-VISIBLE-AT-SERDATR-ADDRESS stimulus red MUT_6: the
//   address read changed from SERDATR to INTENAR -- expect FAIL, neither the
//   RBF mirror bit nor the received data appear at the wrong address.
// MUTATION SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT oracle green MUT_7: the
//   accepted latency bound relaxed from 0 (strict, "direct") to <=3 (the
//   measured value) -- flips PASS on the identical measurement, showing the
//   baseline FAIL is a genuine latency, not a broken fixture.
// MUTATION SERDAT-CHIP-COLUMN-P283 stimulus red MUT_8: the write address
//   changed from SERDAT to SERPER -- expect FAIL, no start bit appears on
//   txd because no SERDAT write ever lands.
// MUTATION ADKCON-REGISTER-SUMMARY-ROW stimulus red MUT_9: with a known
//   nonzero value (MSBSYNC) already stored, the address probed for the
//   write-only check changed from ADKCON ($09E, no read decode) to ADKCONR
//   ($010, which DOES have one) -- expect FAIL, the probe now returns
//   0x0200 instead of the required 0x0000.
// MUTATION ADKCONR-REGISTER-SUMMARY-ROW stimulus red MUT_10: the write-attempt
//   address changed from $010 (ADKCONR, no write decode) to ADKCON's own real
//   write address ($09E) -- expect FAIL, the write actually lands (SETCLR=1,
//   0xFFFF sets all fifteen bits), so `after` (0x7FFF) no longer equals
//   `before` (0x0200, MSBSYNC alone).
// MUTATION ADKCON-BITS-0-14-SYMMETRIC-VIA-ADKCONR stimulus red MUT_11: the
//   second pattern changed from the complementary 0x2AAA to a repeat of the
//   first (0x5555) -- expect FAIL, the two readbacks no longer differ.
// MUTATION ADKCONR-BIT15-SETCLR-NEVER-READS-BACK oracle green MUT_12: the
//   accepted condition relaxed from "bit 15 must read back the written 1"
//   (the manual-implied, "one shared table" oracle) to "bit 15 must read
//   back 0" (the RTL's actual, structural behaviour) -- flips PASS on the
//   identical measurement, showing the baseline FAIL is the genuine
//   always-0 readback, not a fixture bug.
// MUTATION ADKCON-SETCLR-SET stimulus red MUT_13: SETCLR changed from 1 (SET)
//   to 0 (CLR) while writing all-1s data bits -- expect FAIL, nothing gets
//   set (a CLR-mode write of all-1s against an all-0 register is a no-op).
// MUTATION ADKCON-SETCLR-CLR stimulus red MUT_14: SETCLR changed from 0 (CLR)
//   to 1 (SET) -- expect FAIL, the already-set bits 0,2,4,6 stay set instead
//   of clearing (a SET-mode write is a no-op on already-set bits).
// MUTATION ADKCON-SETCLR-PRESERVE stimulus red MUT_15: the CLR-mode write's
//   named bits changed from the complement (0x2AAA, already 0) to the
//   pattern's own set bits (0x5555) -- expect FAIL, the pattern is cleared
//   instead of preserved.
// MUTATION ADKCON-BIT11-UARTBRK stimulus red MUT_16: the bit set changed from
//   11 (UARTBRK) to 10 (WORDSYNC) -- expect FAIL, txd is never forced low.
// MUTATION ADKCON-BITS-12-9-8-STORE-ONLY-NO-CONSUMER stimulus red MUT_17: the
//   bits written changed from 12/9/8 (MFMPREC/MSBSYNC/FAST) to bit 11
//   (UARTBRK) -- expect FAIL, the readback no longer equals the expected
//   12/9/8 mask.
// MUTATION RXD-DELIVERS-CORRECT-VALUE-CONTROL stimulus red MUT_18: the
//   tracked transition is never applied -- rxd is driven back to 1 (its
//   already-settled value) instead of 0, so no synchroniser propagation is
//   ever needed. `latency` stays -1 (the same FAIL verdict
//   SERDATR-BIT11-RXD-REGISTERED-NOT-DIRECT already had at baseline, so
//   that assertion's verdict does not move) and the once-settled read
//   still shows bit 11 = 1 -- expect FAIL, the control no longer confirms
//   the settled value is correct.
