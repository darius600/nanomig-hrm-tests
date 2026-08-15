#include "Vagnus_blitter_minterm.h"
#include "verilated.h"

#include <array>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string_view>

namespace {

std::uint16_t hrm_oracle(
    std::uint8_t lf,
    std::uint16_t a,
    std::uint16_t b,
    std::uint16_t c) {
    std::uint16_t expected = 0;

    for (unsigned bit = 0; bit < 16; ++bit) {
        const unsigned selector =
            (((a >> bit) & 1U) << 2U) |
            (((b >> bit) & 1U) << 1U) |
            ((c >> bit) & 1U);
        const std::uint16_t result_bit =
            static_cast<std::uint16_t>((lf >> selector) & 1U);
        expected |= static_cast<std::uint16_t>(result_bit << bit);
    }

    return expected;
}

bool check_case(
    Vagnus_blitter_minterm& dut,
    std::string_view label,
    std::uint8_t lf,
    std::uint16_t a,
    std::uint16_t b,
    std::uint16_t c,
    std::uint16_t expected,
    std::size_t& checks) {
    dut.lf = lf;
    dut.ain = a;
    dut.bin = b;
    dut.cin = c;
    dut.eval();
    ++checks;

    const auto actual = static_cast<std::uint16_t>(dut.out);
    if (actual == expected) {
        return true;
    }

    std::cerr
        << "FAIL " << label
        << ": LF=$" << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<unsigned>(lf)
        << " A=$" << std::setw(4) << static_cast<unsigned>(a)
        << " B=$" << std::setw(4) << static_cast<unsigned>(b)
        << " C=$" << std::setw(4) << static_cast<unsigned>(c)
        << " expected D=$" << std::setw(4) << static_cast<unsigned>(expected)
        << " actual D=$" << std::setw(4) << static_cast<unsigned>(actual)
        << std::dec << std::endl;
    return false;
}

struct WordPattern {
    std::uint16_t a;
    std::uint16_t b;
    std::uint16_t c;
};

bool run_hrm_examples(
    Vagnus_blitter_minterm& dut,
    const WordPattern& pattern,
    std::size_t& checks) {
    const auto a = pattern.a;
    const auto b = pattern.b;
    const auto c = pattern.c;

    const std::array<std::pair<std::uint8_t, std::uint16_t>, 10> examples{{
        {0x80U, static_cast<std::uint16_t>(a & b & c)},
        {0xF0U, a},
        {0x0FU, static_cast<std::uint16_t>(~a)},
        {0xCCU, b},
        {0xAAU, c},
        {0xFCU, static_cast<std::uint16_t>(a | b)},
        {0xC0U, static_cast<std::uint16_t>(a & b)},
        {0xCAU, static_cast<std::uint16_t>((a & b) | ((~a) & c))},
        {0x00U, 0x0000U},
        {0xFFU, 0xFFFFU},
    }};

    for (const auto& [lf, expected] : examples) {
        if (!check_case(
                dut, "HRM example", lf, a, b, c, expected, checks)) {
            return false;
        }
    }

    return true;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_minterm dut;
    std::size_t checks = 0;

    // Every LF byte against every ABC selector. Each source bit is
    // replicated across the word, so the expected word is all zero or all one.
    for (unsigned lf = 0; lf <= 0xFFU; ++lf) {
        for (unsigned selector = 0; selector < 8U; ++selector) {
            const std::uint16_t a =
                (selector & 0x4U) != 0U ? 0xFFFFU : 0x0000U;
            const std::uint16_t b =
                (selector & 0x2U) != 0U ? 0xFFFFU : 0x0000U;
            const std::uint16_t c =
                (selector & 0x1U) != 0U ? 0xFFFFU : 0x0000U;
            const std::uint16_t expected =
                ((lf >> selector) & 1U) != 0U ? 0xFFFFU : 0x0000U;

            if (!check_case(
                    dut,
                    "exhaustive LF/ABC",
                    static_cast<std::uint8_t>(lf),
                    a,
                    b,
                    c,
                    expected,
                    checks)) {
                return 1;
            }
        }
    }

    // Bits 0..7 and 8..15 each encode selectors ABC=000..111.
    for (unsigned lf = 0; lf <= 0xFFU; ++lf) {
        const auto expected = static_cast<std::uint16_t>(lf | (lf << 8U));
        if (!check_case(
                dut,
                "packed selector order",
                static_cast<std::uint8_t>(lf),
                0xF0F0U,
                0xCCCCU,
                0xAAAAU,
                expected,
                checks)) {
            return 1;
        }
    }

    const std::array<WordPattern, 3> patterns{{
        {0x5AA5U, 0x3CC3U, 0x0FF0U},
        {0x1357U, 0x2468U, 0xAAAAU},
        {0xF00FU, 0x55AAU, 0xC33CU},
    }};

    // Independent bitwise oracle for mixed source words and every LF byte.
    for (const auto& pattern : patterns) {
        for (unsigned lf = 0; lf <= 0xFFU; ++lf) {
            const auto expected = hrm_oracle(
                static_cast<std::uint8_t>(lf),
                pattern.a,
                pattern.b,
                pattern.c);
            if (!check_case(
                    dut,
                    "mixed word",
                    static_cast<std::uint8_t>(lf),
                    pattern.a,
                    pattern.b,
                    pattern.c,
                    expected,
                    checks)) {
                return 1;
            }
        }
    }

    for (const auto& pattern : patterns) {
        if (!run_hrm_examples(dut, pattern, checks)) {
            return 1;
        }
    }

    dut.final();
    std::cout << "PASS: " << checks
              << " HRM-derived blitter minterm cases" << std::endl;
    return 0;
}
