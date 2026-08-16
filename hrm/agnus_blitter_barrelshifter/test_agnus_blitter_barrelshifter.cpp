#include <Vagnus_blitter_barrelshifter.h>
#include <verilated.h>

#include <array>
#include <cstdio>
#include <cstdint>

#define HRM_STRINGIFY_IMPL(...) #__VA_ARGS__
#define HRM_STRINGIFY(...) HRM_STRINGIFY_IMPL(__VA_ARGS__)

namespace {

std::uint16_t hrm_oracle(
    bool descending,
    std::uint8_t shift,
    std::uint16_t previous,
    std::uint16_t current) {
    if (shift == 0U) {
        return current;
    }
    if (descending) {
        return static_cast<std::uint16_t>(
            (static_cast<std::uint32_t>(current) << shift) |
            (static_cast<std::uint32_t>(previous) >> (16U - shift)));
    }
    return static_cast<std::uint16_t>(
        (static_cast<std::uint32_t>(previous) << (16U - shift)) |
        (static_cast<std::uint32_t>(current) >> shift));
}

bool check_case(
    Vagnus_blitter_barrelshifter& dut,
    bool descending,
    std::uint8_t shift,
    std::uint16_t previous,
    std::uint16_t current,
    std::size_t& checks) {
    const auto expected =
        hrm_oracle(descending, shift, previous, current);
    dut.desc = descending;
    dut.shift = shift;
    dut.old_val = previous;
    dut.new_val = current;
    dut.eval();
    ++checks;
    const auto actual = static_cast<std::uint16_t>(dut.out);
    if (actual == expected) {
        return true;
    }
    std::fprintf(
        stderr,
        HRM_STRINGIFY(
            FAIL desc=%u shift=%u P=%04x C=%04x expected=%04x actual=%04x),
        static_cast<unsigned>(descending),
        static_cast<unsigned>(shift),
        static_cast<unsigned>(previous),
        static_cast<unsigned>(current),
        static_cast<unsigned>(expected),
        static_cast<unsigned>(actual));
    std::fputc(10, stderr);
    return false;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_barrelshifter dut;
    std::size_t checks = 0;

    const std::array<std::uint16_t, 2> zero_shift_previous{{
        0x0000U,
        0xFFFFU,
    }};

    // Shift zero selects the current word and ignores the previous word.
    for (unsigned current = 0; current <= 0xFFFFU; ++current) {
        for (const auto previous : zero_shift_previous) {
            for (unsigned desc = 0; desc < 2U; ++desc) {
                const bool ok = check_case(
                    dut, desc != 0U, 0U, previous,
                    static_cast<std::uint16_t>(current), checks);
                if (ok == false) {
                    return 1;
                }
            }
        }
    }

    // First word: no predecessor, so zeros are shifted in.
    for (unsigned current = 0; current <= 0xFFFFU; ++current) {
        for (unsigned shift = 1; shift < 16U; ++shift) {
            for (unsigned desc = 0; desc < 2U; ++desc) {
                const bool ok = check_case(
                    dut, desc != 0U, static_cast<std::uint8_t>(shift),
                    0x0000U, static_cast<std::uint16_t>(current), checks);
                if (ok == false) {
                    return 1;
                }
            }
        }
    }

    // Zero current word isolates bits crossing from the previous word.
    for (unsigned previous = 0; previous <= 0xFFFFU; ++previous) {
        for (unsigned shift = 1; shift < 16U; ++shift) {
            for (unsigned desc = 0; desc < 2U; ++desc) {
                const bool ok = check_case(
                    dut, desc != 0U, static_cast<std::uint8_t>(shift),
                    static_cast<std::uint16_t>(previous), 0x0000U, checks);
                if (ok == false) {
                    return 1;
                }
            }
        }
    }

    // Route every individual bit in the abstract 32-bit P:C input.
    for (unsigned bit = 0; bit < 32U; ++bit) {
        const auto previous = static_cast<std::uint16_t>(
            bit >= 16U ? 1U << (bit - 16U) : 0U);
        const auto current = static_cast<std::uint16_t>(
            bit < 16U ? 1U << bit : 0U);
        for (unsigned shift = 0; shift < 16U; ++shift) {
            for (unsigned desc = 0; desc < 2U; ++desc) {
                const bool ok = check_case(
                    dut, desc != 0U, static_cast<std::uint8_t>(shift),
                    previous, current, checks);
                if (ok == false) {
                    return 1;
                }
            }
        }
    }

    const std::array<std::uint16_t, 8> patterns{{
        0x0000U,
        0xFFFFU,
        0xAAAAU,
        0x5555U,
        0x8001U,
        0x7FFEU,
        0x00FFU,
        0xFF00U,
    }};
    for (const auto previous : patterns) {
        for (const auto current : patterns) {
            for (unsigned shift = 0; shift < 16U; ++shift) {
                for (unsigned desc = 0; desc < 2U; ++desc) {
                    const bool ok = check_case(
                        dut, desc != 0U, static_cast<std::uint8_t>(shift),
                        previous, current, checks);
                    if (ok == false) {
                        return 1;
                    }
                }
            }
        }
    }

    constexpr std::size_t expected_checks = 4197376U;
    if (checks != expected_checks) {
        return 1;
    }
    dut.final();

    std::puts(HRM_STRINGIFY(
        PASS: 4197376 HRM-derived blitter barrel shifter cases));
    return 0;
}
