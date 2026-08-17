#include <verilated.h>
#include "Vagnus_blitter_line_octant.h"
#include <iostream>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_line_octant* top = new Vagnus_blitter_line_octant;

    int fail_count = 0;
    
    auto test_case = [&](uint8_t sud, uint8_t sul, uint8_t aul, uint8_t sign_del, 
                         uint8_t ash, uint8_t exp_inc, uint8_t exp_dec, uint8_t exp_add, uint8_t exp_sub, const char* name) {
        
        uint8_t bltcon1 = (sud << 2) | (sul << 1) | aul;
        top->bltcon1 = bltcon1;
        top->sign_del = sign_del;
        top->ash = ash;
        top->eval();
        
        if (top->lineinc != exp_inc || top->linedec != exp_dec || 
            top->lineadd != exp_add || top->linesub != exp_sub) {
            std::cout << "FAIL: " << name << " - SUD:" << (int)sud << " SUL:" << (int)sul << " AUL:" << (int)aul 
                      << " Sign:" << (int)sign_del << " Ash:" << (int)ash << "\n";
            std::cout << "  Expected: inc=" << (int)exp_inc << " dec=" << (int)exp_dec 
                      << " add=" << (int)exp_add << " sub=" << (int)exp_sub << "\n";
            std::cout << "  Got:      inc=" << (int)top->lineinc << " dec=" << (int)top->linedec 
                      << " add=" << (int)top->lineadd << " sub=" << (int)top->linesub << "\n";
            fail_count++;
        }
    };

    // Helper for testing octant
    auto test_octant = [&](uint8_t sud, uint8_t sul, uint8_t aul, 
                           uint8_t expected_major_x, uint8_t expected_major_y,
                           uint8_t expected_minor_x, uint8_t expected_minor_y, const char* name) {
        // Sign=1 (Negative Error) -> Step ONLY on Major Axis
        // For X step, test with ash=15 (Right/Inc) or ash=0 (Left/Dec) depending on direction
        uint8_t ash_major = (expected_major_x == 1) ? 15 : ((expected_major_x == 2) ? 0 : 8);
        
        test_case(sud, sul, aul, 1, ash_major, 
                  (expected_major_x == 1 ? 1 : 0), // inc
                  (expected_major_x == 2 ? 1 : 0), // dec
                  (expected_major_y == 1 ? 1 : 0), // add
                  (expected_major_y == 2 ? 1 : 0), // sub
                  "Sign=1 (Major Only)");

        // Sign=0 (Positive Error) -> Step on Major AND Minor Axis
        // We set ash to match the X step direction (if any X step occurs)
        uint8_t ash_both = 8;
        if (expected_major_x == 1 || expected_minor_x == 1) ash_both = 15;
        if (expected_major_x == 2 || expected_minor_x == 2) ash_both = 0;

        test_case(sud, sul, aul, 0, ash_both, 
                  ((expected_major_x == 1 || expected_minor_x == 1) ? 1 : 0), // inc
                  ((expected_major_x == 2 || expected_minor_x == 2) ? 1 : 0), // dec
                  ((expected_major_y == 1 || expected_minor_y == 1) ? 1 : 0), // add
                  ((expected_major_y == 2 || expected_minor_y == 2) ? 1 : 0), // sub
                  "Sign=0 (Major+Minor)");
    };

    // Octant 1: SUD=1 (Minor Y, Major X), SUL=0 (Minor Down), AUL=0 (Major Right)
    test_octant(1, 0, 0,  1, 0,  0, 1, "Octant 1 (X Right, Y Down)");
    
    // Octant 2: SUD=0 (Minor X, Major Y), SUL=0 (Minor Right), AUL=0 (Major Down)
    test_octant(0, 0, 0,  0, 1,  1, 0, "Octant 2 (Y Down, X Right)");

    // Octant 3: SUD=0 (Minor X, Major Y), SUL=1 (Minor Left), AUL=0 (Major Down)
    test_octant(0, 1, 0,  0, 1,  2, 0, "Octant 3 (Y Down, X Left)");
    
    // Octant 4: SUD=1 (Minor Y, Major X), SUL=0 (Minor Down), AUL=1 (Major Left)
    test_octant(1, 0, 1,  2, 0,  0, 1, "Octant 4 (X Left, Y Down)");

    // Octant 5: SUD=1 (Minor Y, Major X), SUL=1 (Minor Up), AUL=1 (Major Left)
    test_octant(1, 1, 1,  2, 0,  0, 2, "Octant 5 (X Left, Y Up)");
    
    // Octant 6: SUD=0 (Minor X, Major Y), SUL=1 (Minor Left), AUL=1 (Major Up)
    test_octant(0, 1, 1,  0, 2,  2, 0, "Octant 6 (Y Up, X Left)");

    // Octant 7: SUD=0 (Minor X, Major Y), SUL=0 (Minor Right), AUL=1 (Major Up)
    test_octant(0, 0, 1,  0, 2,  1, 0, "Octant 7 (Y Up, X Right)");
    
    // Octant 8: SUD=1 (Minor Y, Major X), SUL=1 (Minor Up), AUL=0 (Major Right)
    test_octant(1, 1, 0,  1, 0,  0, 2, "Octant 8 (X Right, Y Up)");

    if (fail_count == 0) {
        std::cout << "PASS: All octant cases tested successfully.\n";
    }
    
    top->final();
    delete top;
    return fail_count == 0 ? 0 : 1;
}
