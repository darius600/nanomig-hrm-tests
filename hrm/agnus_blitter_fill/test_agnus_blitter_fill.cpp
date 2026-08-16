#include <Vagnus_blitter_fill.h>
#include <verilated.h>

#include <array>
#include <cstdint>
#include <cstdio>

namespace {

enum class FillMode {
    inclusive,
    exclusive,
};

struct ExpectedFill {
    std::uint16_t output;
    bool carry_out;
};

struct HrmExample {
    FillMode mode;
    bool carry_in;
    std::uint16_t input;
    std::uint16_t expected_output;
    bool expected_carry_out;
};

ExpectedFill hrm_oracle(
    FillMode mode,
    bool carry_in,
    std::uint16_t input) {
    bool fill_state = carry_in;
    std::uint16_t output = 0;

    // HRM: start at the rightmost edge and flip state at every source 1.
    for (unsigned bit = 0; bit < 16U; ++bit) {
        const bool input_bit = ((input >> bit) & 1U) != 0U;
        const bool output_bit = mode == FillMode::inclusive
            ? input_bit || fill_state
            : input_bit != fill_state;
        if (output_bit) {
            output = static_cast<std::uint16_t>(output | (1U << bit));
        }
        fill_state = fill_state != input_bit;
    }

    return ExpectedFill{output, fill_state};
}

bool check_fill_case(
    Vagnus_blitter_fill& dut,
    FillMode mode,
    bool carry_in,
    std::uint16_t input,
    ExpectedFill expected,
    std::size_t& checks) {
    dut.ife = mode == FillMode::inclusive;
    dut.efe = mode == FillMode::exclusive;
    dut.fci = carry_in;
    dut.in = input;
    dut.eval();
    ++checks;

    const auto actual_output = static_cast<std::uint16_t>(dut.out);
    const bool actual_carry_out = dut.fco != 0U;
    if (actual_output == expected.output &&
        actual_carry_out == expected.carry_out) {
        return true;
    }

    std::fprintf(
        stderr,
        "FAIL fill mode=%u FCI=%u IN=%04x expected OUT=%04x FCO=%u actual OUT=%04x FCO=%u\n",
        mode == FillMode::inclusive ? 0U : 1U,
        static_cast<unsigned>(carry_in),
        static_cast<unsigned>(input),
        static_cast<unsigned>(expected.output),
        static_cast<unsigned>(expected.carry_out),
        static_cast<unsigned>(actual_output),
        static_cast<unsigned>(actual_carry_out));
    return false;
}

bool check_bypass_case(
    Vagnus_blitter_fill& dut,
    bool carry_in,
    std::uint16_t input,
    std::size_t& checks) {
    dut.ife = 0;
    dut.efe = 0;
    dut.fci = carry_in;
    dut.in = input;
    dut.eval();
    ++checks;

    const auto actual_output = static_cast<std::uint16_t>(dut.out);
    if (actual_output == input) {
        return true;
    }
    std::fprintf(
        stderr,
        "FAIL bypass FCI=%u IN=%04x expected OUT=%04x actual OUT=%04x\n",
        static_cast<unsigned>(carry_in),
        static_cast<unsigned>(input),
        static_cast<unsigned>(input),
        static_cast<unsigned>(actual_output));
    return false;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_fill dut;
    std::size_t checks = 0;

    const std::array<HrmExample, 4> hrm_examples{{
        {FillMode::inclusive, false, 0x2418U, 0x3C18U, false},
        {FillMode::exclusive, false, 0x2418U, 0x1C08U, false},
        {FillMode::inclusive, true, 0x2418U, 0xE7FFU, true},
        {FillMode::exclusive, true, 0x2418U, 0xE3F7U, true},
    }};

    for (const auto& example : hrm_examples) {
        const ExpectedFill expected{
            example.expected_output,
            example.expected_carry_out};
        if (!check_fill_case(
                dut, example.mode, example.carry_in,
                example.input, expected, checks)) {
            return 1;
        }
    }

    const std::array<std::uint16_t, 6> boundary_patterns{{
        0x0000U,
        0xFFFFU,
        0x0001U,
        0x8000U,
        0x5555U,
        0xAAAAU,
    }};

    const std::array<FillMode, 2> modes{{
        FillMode::inclusive,
        FillMode::exclusive,
    }};

    for (const auto word : boundary_patterns) {
        for (unsigned carry_in = 0; carry_in < 2U; ++carry_in) {
            for (const auto mode : modes) {
                const auto expected =
                    hrm_oracle(mode, carry_in != 0U, word);
                if (!check_fill_case(
                        dut, mode, carry_in != 0U,
                        word, expected, checks)) {
                    return 1;
                }
            }
        }
    }

    for (unsigned input = 0; input <= 0xFFFFU; ++input) {
        for (unsigned carry_in = 0; carry_in < 2U; ++carry_in) {
            for (const auto mode : modes) {
                const auto word = static_cast<std::uint16_t>(input);
                const auto expected =
                    hrm_oracle(mode, carry_in != 0U, word);
                if (!check_fill_case(
                        dut, mode, carry_in != 0U,
                        word, expected, checks)) {
                    return 1;
                }
            }
        }
    }

    for (unsigned input = 0; input <= 0xFFFFU; ++input) {
        for (unsigned carry_in = 0; carry_in < 2U; ++carry_in) {
            if (!check_bypass_case(
                    dut, carry_in != 0U,
                    static_cast<std::uint16_t>(input), checks)) {
                return 1;
            }
        }
    }

    constexpr std::size_t expected_checks = 393244U;
    if (checks != expected_checks) {
        return 1;
    }

    dut.final();
    std::puts("PASS: 4 HRM examples, 24 boundary cases, 262144 fill cases, 131072 bypass cases");
    return 0;
}
