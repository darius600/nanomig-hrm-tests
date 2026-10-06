// test_ch8_controller_ports.cpp -- ch8-controller-ports. See
// SPECIFICATION.md for every assertion's proposition, program, observed
// value and expected value. This file only implements what
// SPECIFICATION.md already committed to.
//
// Connector wiring (stimulus information, not an expected value):
// Connector A (JOY0DAT, CIAAPRA bit 6, the manual's "left"/"port 1") is
// the harness's `joystick1` port. Connector B (JOY1DAT, CIAAPRA bit 7,
// "right"/"port 2") is the harness's `joystick0` port -- the opposite of
// the naive index reading ("joystick0" = "0=left"). The P2 anchor (below)
// checks this choice empirically at every run; it is not assumed to hold.
//
// Bit layout within the 8-bit joystick0/joystick1 field: bit3=up(pin1),
// bit2=down(pin2), bit1=left(pin3), bit0=right(pin4), bit4=fire(pin6),
// bit5=the pot-line gate used below. A harness field bit of 1 grounds the
// pin. SPECIFICATION.md's "level" is the electrical level (1=open,
// 0=shorted), so: harness bit = !level.
//
// Pot lines (P1's four lines, pins 5 and 9 of both connectors), as ports
// this test can drive from nanomig.v's boundary: LX (connector A pin 5)
// through mouse_buttons bit 2; LY (connector A pin 9) through
// mouse_buttons bit 1 and connector A's own joystick1 bit 5; RY
// (connector B pin 9) through connector B's own joystick0 bit 5; RX
// (connector B pin 5) through none of the ports this harness offers. See
// the K5 and VGREEN mutations below, and the RECORDED lines they print.
//
// MUTATION PC1 stimulus red the R step of every Fc waits for line 12 instead of line 10, via NMH_MUTATE=K3 (PAL+ECS run)
// MUTATION PC1 stimulus red the R step of every Fc waits for line 5 instead of line 10, so no record of any Fc counts and PC1's every-R-step clause fails, via NMH_MUTATE=K6 (PAL+ECS run)
// MUTATION PC2 stimulus red Phase P's F1 writes $AA00 instead of $FF00, via NMH_MUTATE=K4 (PAL+ECS run)
// MUTATION PC3 stimulus red the harness grounds LX (mouse_buttons bit 2), LY (mouse_buttons bit 1 and joystick1 bit 5) and RY (joystick0 bit 5) from each configuration's Fb W step through the end of its Fc R step, via NMH_MUTATE=K5 (PAL+ECS run)
// MUTATION PC4 stimulus red every W step stores its M1 marker after waiting for line 9 instead of right after the POTGO write at line 4; the POTGO write, the V reads and the R steps are unchanged, so PC1 to PC3 hold and t(M2)-t(M1) falls below 300us, via NMH_MUTATE=K7 (PAL+ECS run)
// MUTATION C0-Fc none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (PAL+ECS run)
// MUTATION C1-Fc none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (PAL+ECS run)
// MUTATION C2-Fc none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (PAL+ECS run)
// MUTATION C3-Fc stimulus green (proven-live input) the harness grounds each configuration's reachable input lines (excluding that configuration's own output line) from Fc's M1 marker to the end of its R step; C3-Fc's three input lines (LX, LY, RY) are all reachable this way, so it goes green through the same comparator, via NMH_MUTATE=VGREEN (PAL+ECS run)
// MUTATION C4-Fc none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (PAL+ECS run)
// MUTATION P1-START none impossible green is not reachable at this boundary: the line is the conjunction of C0-Fc to C4-Fc, four of which have POTGOR bit 12 (RX) among their input lines and no port of nanomig.v reaches that bit (userio.v derives it from the POTGO register alone); VGREEN turns C3-Fc green through the same comparator (PAL+ECS run)
// MUTATION C1-Fc-OUT stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (PAL+ECS run)
// MUTATION C1-Fc-OUT stimulus red the Fc write of C1 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (PAL+ECS run)
// MUTATION C2-Fc-OUT stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (PAL+ECS run)
// MUTATION C2-Fc-OUT stimulus red the Fc write of C2 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (PAL+ECS run)
// MUTATION C3-Fc-OUT stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (PAL+ECS run)
// MUTATION C3-Fc-OUT stimulus red the Fc write of C3 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (PAL+ECS run)
// MUTATION C4-Fc-OUT stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (PAL+ECS run)
// MUTATION C4-Fc-OUT stimulus red the Fc write of C4 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (PAL+ECS run)
// MUTATION PC1-NTSC+ECS stimulus red the R step of every Fc waits for line 12 instead of line 10, via NMH_MUTATE=K3 (NTSC+ECS run)
// MUTATION PC1-NTSC+ECS stimulus red the R step of every Fc waits for line 5 instead of line 10, so no record of any Fc counts and PC1's every-R-step clause fails, via NMH_MUTATE=K6 (NTSC+ECS run)
// MUTATION PC2-NTSC+ECS stimulus red Phase P's F1 writes $AA00 instead of $FF00, via NMH_MUTATE=K4 (NTSC+ECS run)
// MUTATION PC3-NTSC+ECS stimulus red the harness grounds LX (mouse_buttons bit 2), LY (mouse_buttons bit 1 and joystick1 bit 5) and RY (joystick0 bit 5) from each configuration's Fb W step through the end of its Fc R step, via NMH_MUTATE=K5 (NTSC+ECS run)
// MUTATION PC4-NTSC+ECS stimulus red every W step stores its M1 marker after waiting for line 9 instead of right after the POTGO write at line 4; the POTGO write, the V reads and the R steps are unchanged, so PC1 to PC3 hold and t(M2)-t(M1) falls below 300us, via NMH_MUTATE=K7 (NTSC+ECS run)
// MUTATION C0-Fc-NTSC+ECS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (NTSC+ECS run)
// MUTATION C1-Fc-NTSC+ECS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (NTSC+ECS run)
// MUTATION C2-Fc-NTSC+ECS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (NTSC+ECS run)
// MUTATION C3-Fc-NTSC+ECS stimulus green (proven-live input) the harness grounds each configuration's reachable input lines (excluding that configuration's own output line) from Fc's M1 marker to the end of its R step; C3-Fc's three input lines (LX, LY, RY) are all reachable this way, so it goes green through the same comparator, via NMH_MUTATE=VGREEN (NTSC+ECS run)
// MUTATION C4-Fc-NTSC+ECS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (NTSC+ECS run)
// MUTATION P1-START-NTSC+ECS none impossible green is not reachable at this boundary: the line is the conjunction of C0-Fc to C4-Fc, four of which have POTGOR bit 12 (RX) among their input lines and no port of nanomig.v reaches that bit (userio.v derives it from the POTGO register alone); VGREEN turns C3-Fc green through the same comparator (NTSC+ECS run)
// MUTATION C1-Fc-OUT-NTSC+ECS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (NTSC+ECS run)
// MUTATION C1-Fc-OUT-NTSC+ECS stimulus red the Fc write of C1 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (NTSC+ECS run)
// MUTATION C2-Fc-OUT-NTSC+ECS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (NTSC+ECS run)
// MUTATION C2-Fc-OUT-NTSC+ECS stimulus red the Fc write of C2 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (NTSC+ECS run)
// MUTATION C3-Fc-OUT-NTSC+ECS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (NTSC+ECS run)
// MUTATION C3-Fc-OUT-NTSC+ECS stimulus red the Fc write of C3 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (NTSC+ECS run)
// MUTATION C4-Fc-OUT-NTSC+ECS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (NTSC+ECS run)
// MUTATION C4-Fc-OUT-NTSC+ECS stimulus red the Fc write of C4 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (NTSC+ECS run)
// MUTATION PC1-PAL+OCS stimulus red the R step of every Fc waits for line 12 instead of line 10, via NMH_MUTATE=K3 (PAL+OCS run)
// MUTATION PC1-PAL+OCS stimulus red the R step of every Fc waits for line 5 instead of line 10, so no record of any Fc counts and PC1's every-R-step clause fails, via NMH_MUTATE=K6 (PAL+OCS run)
// MUTATION PC2-PAL+OCS stimulus red Phase P's F1 writes $AA00 instead of $FF00, via NMH_MUTATE=K4 (PAL+OCS run)
// MUTATION PC3-PAL+OCS stimulus red the harness grounds LX (mouse_buttons bit 2), LY (mouse_buttons bit 1 and joystick1 bit 5) and RY (joystick0 bit 5) from each configuration's Fb W step through the end of its Fc R step, via NMH_MUTATE=K5 (PAL+OCS run)
// MUTATION PC4-PAL+OCS stimulus red every W step stores its M1 marker after waiting for line 9 instead of right after the POTGO write at line 4; the POTGO write, the V reads and the R steps are unchanged, so PC1 to PC3 hold and t(M2)-t(M1) falls below 300us, via NMH_MUTATE=K7 (PAL+OCS run)
// MUTATION C0-Fc-PAL+OCS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (PAL+OCS run)
// MUTATION C1-Fc-PAL+OCS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (PAL+OCS run)
// MUTATION C2-Fc-PAL+OCS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (PAL+OCS run)
// MUTATION C3-Fc-PAL+OCS stimulus green (proven-live input) the harness grounds each configuration's reachable input lines (excluding that configuration's own output line) from Fc's M1 marker to the end of its R step; C3-Fc's three input lines (LX, LY, RY) are all reachable this way, so it goes green through the same comparator, via NMH_MUTATE=VGREEN (PAL+OCS run)
// MUTATION C4-Fc-PAL+OCS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (PAL+OCS run)
// MUTATION P1-START-PAL+OCS none impossible green is not reachable at this boundary: the line is the conjunction of C0-Fc to C4-Fc, four of which have POTGOR bit 12 (RX) among their input lines and no port of nanomig.v reaches that bit (userio.v derives it from the POTGO register alone); VGREEN turns C3-Fc green through the same comparator (PAL+OCS run)
// MUTATION C1-Fc-OUT-PAL+OCS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (PAL+OCS run)
// MUTATION C1-Fc-OUT-PAL+OCS stimulus red the Fc write of C1 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (PAL+OCS run)
// MUTATION C2-Fc-OUT-PAL+OCS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (PAL+OCS run)
// MUTATION C2-Fc-OUT-PAL+OCS stimulus red the Fc write of C2 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (PAL+OCS run)
// MUTATION C3-Fc-OUT-PAL+OCS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (PAL+OCS run)
// MUTATION C3-Fc-OUT-PAL+OCS stimulus red the Fc write of C3 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (PAL+OCS run)
// MUTATION C4-Fc-OUT-PAL+OCS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (PAL+OCS run)
// MUTATION C4-Fc-OUT-PAL+OCS stimulus red the Fc write of C4 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (PAL+OCS run)
// MUTATION PC1-NTSC+OCS stimulus red the R step of every Fc waits for line 12 instead of line 10, via NMH_MUTATE=K3 (NTSC+OCS run)
// MUTATION PC1-NTSC+OCS stimulus red the R step of every Fc waits for line 5 instead of line 10, so no record of any Fc counts and PC1's every-R-step clause fails, via NMH_MUTATE=K6 (NTSC+OCS run)
// MUTATION PC2-NTSC+OCS stimulus red Phase P's F1 writes $AA00 instead of $FF00, via NMH_MUTATE=K4 (NTSC+OCS run)
// MUTATION PC3-NTSC+OCS stimulus red the harness grounds LX (mouse_buttons bit 2), LY (mouse_buttons bit 1 and joystick1 bit 5) and RY (joystick0 bit 5) from each configuration's Fb W step through the end of its Fc R step, via NMH_MUTATE=K5 (NTSC+OCS run)
// MUTATION PC4-NTSC+OCS stimulus red every W step stores its M1 marker after waiting for line 9 instead of right after the POTGO write at line 4; the POTGO write, the V reads and the R steps are unchanged, so PC1 to PC3 hold and t(M2)-t(M1) falls below 300us, via NMH_MUTATE=K7 (NTSC+OCS run)
// MUTATION C0-Fc-NTSC+OCS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (NTSC+OCS run)
// MUTATION C1-Fc-NTSC+OCS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (NTSC+OCS run)
// MUTATION C2-Fc-NTSC+OCS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (NTSC+OCS run)
// MUTATION C3-Fc-NTSC+OCS stimulus green (proven-live input) the harness grounds each configuration's reachable input lines (excluding that configuration's own output line) from Fc's M1 marker to the end of its R step; C3-Fc's three input lines (LX, LY, RY) are all reachable this way, so it goes green through the same comparator, via NMH_MUTATE=VGREEN (NTSC+OCS run)
// MUTATION C4-Fc-NTSC+OCS none impossible green is not reachable at this boundary: this line is red on the RTL and its green needs POTGOR bit 12 (RX, connector B pin 5, an input line in this configuration) to read 0, and no port of nanomig.v reaches bit 12 (userio.v derives it from the POTGO register alone), so no stimulus turns the line green; VGREEN turns the same comparator green on C3-Fc, where bit 12 is the output line (NTSC+OCS run)
// MUTATION P1-START-NTSC+OCS none impossible green is not reachable at this boundary: the line is the conjunction of C0-Fc to C4-Fc, four of which have POTGOR bit 12 (RX) among their input lines and no port of nanomig.v reaches that bit (userio.v derives it from the POTGO register alone); VGREEN turns C3-Fc green through the same comparator (NTSC+OCS run)
// MUTATION C1-Fc-OUT-NTSC+OCS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (NTSC+OCS run)
// MUTATION C1-Fc-OUT-NTSC+OCS stimulus red the Fc write of C1 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (NTSC+OCS run)
// MUTATION C2-Fc-OUT-NTSC+OCS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (NTSC+OCS run)
// MUTATION C2-Fc-OUT-NTSC+OCS stimulus red the Fc write of C2 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (NTSC+OCS run)
// MUTATION C3-Fc-OUT-NTSC+OCS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (NTSC+OCS run)
// MUTATION C3-Fc-OUT-NTSC+OCS stimulus red the Fc write of C3 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (NTSC+OCS run)
// MUTATION C4-Fc-OUT-NTSC+OCS stimulus green (unchanged clause) every Fc write withholds bit 0 (START), changing nothing else; the output-driven line is not touched by START's absence, via NMH_MUTATE=K1 (NTSC+OCS run)
// MUTATION C4-Fc-OUT-NTSC+OCS stimulus red the Fc write of C4 sets the output-enable bit's data bit to 0 instead of 1, changing nothing else; p.240's output-drive sentence then predicts 0 on the output-driven line instead of 1, via NMH_MUTATE=OUTLOW (NTSC+OCS run)
// MUTATION FIXTURE stimulus red stored constant read back does not match the value the program wrote, via NMH_MUTATE=FIXTURE
// MUTATION P1-FIXTURE stimulus red the PAL+ECS run loads prog_p1_absent.bin, a file that does not exist, so the program is not loaded, via NMH_MUTATE=P1LOAD
// MUTATION P1-CONFIGS stimulus red the NTSC+ECS run loads prog_p1_absent.bin, a file that does not exist, so that configuration is not loaded, via NMH_MUTATE=CFGMUT
// MUTATION JOY-FIXTURE stimulus red the joystick run loads prog_joy_absent.bin, a file that does not exist, so the program is not loaded, via NMH_MUTATE=JOYLOAD
// MUTATION JOY-CONFIGS stimulus red the NTSC+ECS joystick run applies Q3's stimulus (the first Y-up(A) level at N1), so its records differ from the PAL+ECS run's, via NMH_MUTATE=JOYCFG
// MUTATION Q1 stimulus red the harness applies Yup(A)'s levels only after it sees U_k instead of at S_k, so the timing clause t_apply(k) < t(T_k) fails and the hold clause holds, via NMH_MUTATE=Q1
// MUTATION Q1 stimulus red the harness changes one level of connector B (joystick0 bit 7) after it sees T_k of step 5 and before U_k, so the drive trace shows a change inside (t_apply(k), t(U_k)]; the timing clause holds and the hold clause fails, via NMH_MUTATE=Q1HOLD
// MUTATION Q2 stimulus red the harness additionally grounds pin 9 of connector A throughout Yup(A), so POTGOR bit 10 or 14 reads 0, via NMH_MUTATE=Q2
// MUTATION Q2 stimulus red the harness holds mouse_buttons bit 0 for 64 cycles inside step 10's wait and releases it again before that step's reads; only the drive trace shows it, via NMH_MUTATE=Q2MOUSE
// MUTATION Q2 stimulus red the harness holds kbd_mouse_data at $55 for 64 cycles inside step 10's wait and releases it again before that step's reads; only the drive trace shows it, via NMH_MUTATE=Q2KEY
// MUTATION Q3 stimulus red the harness applies Yup(A) sub-step 0's levels at N1 instead of at N1's own (released) step, via NMH_MUTATE=Q3
// MUTATION Q4 stimulus red F0(A) is held at level 0 (pressed) instead of level 1 (released); F1(A) is unchanged, so the anchor (which reads only F1 records) is unaffected, and Q2 reads no own-connector bit of an F0 record, via NMH_MUTATE=Q4
// MUTATION P2-BLOCKA-JOY0 stimulus red block A's steps are applied to connector B and block B's to connector A (the F steps stay on their own connector, so the anchor is unaffected), via NMH_MUTATE=P2VAR
// MUTATION P2-BLOCKA-JOY1 stimulus red (same NMH_MUTATE=P2VAR) block A's own connector now carries block B's steps, so JOY1DAT (no longer released on that connector) does not stay at its N1 value throughout what is printed as block A
// MUTATION P2-BLOCKB-JOY0 stimulus red (same NMH_MUTATE=P2VAR) block B's own connector now carries block A's steps, so JOY0DAT does not stay at its end-of-block-A value throughout what is printed as block B
// MUTATION P2-BLOCKB-JOY1 stimulus red (same NMH_MUTATE=P2VAR) JOY1DAT no longer differs record-to-record in what is printed as block B, since that connector now carries the released steps
// MUTATION P2-FOUR-COUNTERS stimulus red (same NMH_MUTATE=P2VAR) each connector's Y/X steps now land on the other connector's register
// MUTATION P2-JOYDAT stimulus red (same NMH_MUTATE=P2VAR) block A's steps land on JOY1DAT and block B's on JOY0DAT, so every one of its five component predicates fails
// MUTATION P2-JOYDAT stimulus red (NMH_MUTATE=P3VAR) in Yup(A) pin 2 follows pin 1's pattern, so the X byte of JOY0DAT moves during the Y-up steps and the four-counters predicate fails while the four block predicates hold
// MUTATION P2-ANCHOR stimulus red F1(A) additionally grounds pin 6 of connector B; A's own F1 pattern (which Q4 reads) is unchanged and Q2 leaves the F1 records to the anchor, so Q2 and Q4 stay PASS, via NMH_MUTATE=P2ANCHOR
// MUTATION P3-ANCHOR stimulus red N0 and N1 both carry Yup(A) sub-step 0's pattern on connector A (pin 1 at level 0), so Q3 (N0 equals N1) stays PASS while the state-0 anchor fails, via NMH_MUTATE=P3ANCHOR
// MUTATION P3-Y stimulus red in Yup(A), the harness drives pin 2 where the table says pin 1 (pin 1 stays released), via NMH_MUTATE=P3VAR
// MUTATION P3-X stimulus red (same NMH_MUTATE=P3VAR) pin 2 follows pin 1's pattern during Yup(A), so the X byte of JOY0DAT moves in the Y-up steps where P3-X expects it unchanged
// MUTATION P4-PIN1 stimulus red in Buttons(A), the harness grounds pin 2 wherever the table says pin 1 (pin 1 stays released), via NMH_MUTATE=P4VAR
// MUTATION P4-PIN2 stimulus red (same NMH_MUTATE=P4VAR) pin 2 is grounded wherever the table says pin 1, so (bit1 xor bit0) reads 1 in steps where pin 2 is released
// MUTATION P4-PIN3 stimulus red (NMH_MUTATE=P2VAR) each connector's records now come from the other connector's steps, so pin 3's bit 9 no longer matches
// MUTATION P4-PIN4 stimulus red (NMH_MUTATE=P2VAR) each connector's records now come from the other connector's steps, so pin 4's bit 1 no longer matches
//
// K2 is not a separate mutation: every Phase-D config already carries its
// own "the other three lines" contrast in the same assertion (the "1"
// column next to the "0" column, SPECIFICATION.md section 4).
//
// A mutation selector (NMH_MUTATE) runs the part of the package whose lines
// it changes -- the POTGO program (P1) or the joystick program (P2-P4) -- and
// nothing else; the clean run, with no selector, runs both. Every P1 selector
// runs all four PAL/NTSC x ECS/OCS configurations, so the records of the
// mode-tagged labels name the same selector as the default label.

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;

// clk_sys, the harness's own clock, runs at 28.375160 MHz (Appendix F,
// p.352: "PAL 28.37516 MHz"; that clk_sys is this clock comes from the
// implementation, since the harness contract defines no clock). Converts
// a cycle count to nanoseconds for PC4's 300 us bound (p.240).
constexpr double kClkHz = 28'375'160.0;
double ns_from_cycles(uint64_t c) { return (double)c * (1.0e9 / kClkHz); }

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

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

// nmh::NanomigHarness::run_until_write, restated to match ANY value at a
// fixed word address (used for U_k -- the timestamp of a store whose
// value is the data itself, not a known marker).
std::optional<uint64_t> run_until_write_any(nmh::NanomigHarness& h, uint32_t byte_addr, uint64_t max_cycles) {
    const uint32_t w = word_addr(byte_addr);
    size_t seen = h.ram_log().size();
    const uint64_t start = h.cycles();
    while (h.cycles() - start < max_cycles) {
        h.run_cycles(1);
        for (; seen < h.ram_log().size(); ++seen)
            if (h.ram_log()[seen].is_write && h.ram_log()[seen].word_addr == w)
                return h.ram_log()[seen].cycle;
    }
    return std::nullopt;
}

// ---- the drive trace --------------------------------------------------
// Every port this package could drive, sampled after every clk_sys cycle the
// test itself advances and stored as the list of changes. It records what
// the ports held, not what the test meant to apply.
struct PortState {
    uint8_t j0 = 0, j1 = 0, mbtn = 0, kbd_level = 0, kbd_type = 0, kbd_data = 0;
    bool operator==(const PortState& o) const {
        return j0 == o.j0 && j1 == o.j1 && mbtn == o.mbtn && kbd_level == o.kbd_level &&
               kbd_type == o.kbd_type && kbd_data == o.kbd_data;
    }
    bool operator!=(const PortState& o) const { return !(*this == o); }
};
struct PortEvent { uint64_t cycle; PortState s; };
struct PortTrace {
    std::vector<PortEvent> ev;
    void sample(nmh::NanomigHarness& h) {
        auto* p = h.ports();
        PortState s;
        s.j0 = p->joystick0; s.j1 = p->joystick1; s.mbtn = p->mouse_buttons;
        s.kbd_level = p->kbd_mouse_level; s.kbd_type = p->kbd_mouse_type; s.kbd_data = p->kbd_mouse_data;
        if (ev.empty() || ev.back().s != s) ev.push_back({h.cycles(), s});
    }
    // the state in force at `cycle`: the last change at or before it
    PortState at(uint64_t cycle) const {
        PortState s;
        for (auto& e : ev) { if (e.cycle > cycle) break; s = e.s; }
        return s;
    }
    // true when some change falls in (lo, hi]
    bool changed_in(uint64_t lo, uint64_t hi) const {
        for (auto& e : ev) if (e.cycle > lo && e.cycle <= hi) return true;
        return false;
    }
};
void traced_cycles(nmh::NanomigHarness& h, PortTrace& tr, uint64_t n) {
    for (uint64_t i = 0; i < n; ++i) { h.run_cycles(1); tr.sample(h); }
}
// nmh::NanomigHarness::run_until_write with the trace sampled after every
// cycle; a value of nullopt matches any value written to the address.
std::optional<uint64_t> traced_until_write(nmh::NanomigHarness& h, PortTrace& tr, uint32_t byte_addr,
                                           std::optional<uint16_t> value, uint64_t max_cycles) {
    const uint32_t w = word_addr(byte_addr);
    size_t seen = h.ram_log().size();
    const uint64_t start = h.cycles();
    while (h.cycles() - start < max_cycles) {
        h.run_cycles(1);
        tr.sample(h);
        for (; seen < h.ram_log().size(); ++seen)
            if (h.ram_log()[seen].is_write && h.ram_log()[seen].word_addr == w &&
                (!value || h.ram_log()[seen].data == *value))
                return h.ram_log()[seen].cycle;
    }
    return std::nullopt;
}

// Port levels to apply at nanomig.v's boundary for grounding a subset of
// the four pot lines: LX through mouse_buttons bit 2, LY through
// mouse_buttons bit 1 and joystick1 bit 5, RY through joystick0 bit 5. RX
// has no bit here: no port this harness offers grounds it (see the K5 and
// VGREEN mutations, and their RECORDED lines).
struct PotGround { bool lx = false, ly = false, ry = false; };
void apply_pot_ground(nmh::NanomigHarness& h, const PotGround& g) {
    uint8_t mbtn = (g.lx ? (1u << 2) : 0) | (g.ly ? (1u << 1) : 0);
    uint8_t j1 = g.ly ? (1u << 5) : 0; // connector A pin 9 (LY)
    uint8_t j0 = g.ry ? (1u << 5) : 0; // connector B pin 9 (RY)
    h.ports()->mouse_buttons = mbtn;
    h.ports()->joystick1 = j1;
    h.ports()->joystick0 = j0;
}
void release_ports(nmh::NanomigHarness& h) {
    h.ports()->mouse_buttons = 0; h.ports()->joystick0 = 0; h.ports()->joystick1 = 0;
}

struct VidCfg { const char* name; uint8_t chipset_config; };
constexpr VidCfg kConfigs[4] = {
    {"PAL+ECS",  0x08}, {"NTSC+ECS", 0x0A}, {"PAL+OCS",  0x00}, {"NTSC+OCS", 0x02},
};
nmh::Config cfg_for(const VidCfg& v) {
    nmh::Config c;
    c.chipset_config = v.chipset_config;
    return c;
}

// =========================================================================
// Proposition 1: POTGO bit 0 dumps the pot-line capacitors (p.240).
// =========================================================================

constexpr uint32_t kP1Base = 0x0C1000;

struct WSlot { int vw0 = -1, vw1 = -1, m1 = -1, rpre = -1; bool has_rpre = false; };
struct RSlot { int vr0[4], m2[4], pg[4], vr1[4]; int target; };

struct Cur {
    int pos = 0;
    WSlot w(bool rpre) {
        WSlot s;
        s.vw0 = pos++;
        if (rpre) { s.has_rpre = true; s.rpre = pos++; }
        s.vw1 = pos++;
        s.m1 = pos++;
        return s;
    }
    RSlot r(int target) {
        RSlot s;
        s.target = target;
        for (int i = 0; i < 4; ++i) { s.vr0[i] = pos++; s.m2[i] = pos++; s.pg[i] = pos++; s.vr1[i] = pos++; }
        return s;
    }
};

struct P1Frame {
    std::string label;
    WSlot w;
    RSlot r;
    bool is_p2_high = false;  // Phase P, output driven high (F1, F3): PC2's Rhigh/Rhigh2
    bool is_p2_low = false;   // Phase P, output driven low (F2): PC2's Rlow
    bool is_fa = false, is_fb = false, is_fc = false;
    uint16_t out_mask = 0;    // Fc only: which of $5500 is the output-driven bit
};

// PC1 and every other check below always grade against line 10 (p.238's
// own reference frame, L0+6): a mutant assembly (K3, K6) may itself wait
// for a different line, but the grader must not adopt that line as its
// own expectation, or it stops checking anything -- it would just agree
// with whatever the mutant did. Holding "10" fixed here is what makes
// PC1 fail when the assembly's own wait target moves.
std::vector<P1Frame> build_p1_frames() {
    constexpr int fc_target = 10;
    Cur c;
    std::vector<P1Frame> f;
    { P1Frame x; x.label = "F1"; x.w = c.w(false); x.r = c.r(10); x.is_p2_high = true; f.push_back(x); }
    { P1Frame x; x.label = "F2"; x.w = c.w(false); x.r = c.r(10); x.is_p2_low  = true; f.push_back(x); }
    { P1Frame x; x.label = "F3"; x.w = c.w(false); x.r = c.r(10); x.is_p2_high = true; f.push_back(x); }
    struct CfgDef { const char* name; uint16_t out_mask; };
    constexpr CfgDef cfgs[5] = {
        {"C0", 0x0000}, {"C1", 0x0100}, {"C2", 0x0400}, {"C3", 0x1000}, {"C4", 0x4000},
    };
    for (auto& cd : cfgs) {
        { P1Frame x; x.label = std::string(cd.name) + "-Fa"; x.w = c.w(false); x.r = c.r(10); x.is_fa = true; f.push_back(x); }
        { P1Frame x; x.label = std::string(cd.name) + "-Fb"; x.w = c.w(false); x.r = c.r(10); x.is_fb = true; f.push_back(x); }
        { P1Frame x; x.label = std::string(cd.name) + "-Fc"; x.w = c.w(true);  x.r = c.r(fc_target); x.is_fc = true; x.out_mask = cd.out_mask; f.push_back(x); }
    }
    return f;
}

struct P1Run {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::unordered_map<uint32_t, uint64_t> first_write; // word_addr -> cycle
    std::vector<std::optional<uint16_t>> word;          // word[i] = value at kP1Base+2*i
    int nwords = 0;
};

uint16_t wv(const P1Run& r, int idx) { return (idx >= 0 && idx < (int)r.word.size() && r.word[idx]) ? *r.word[idx] : 0xFFFF; }
uint64_t cyc(const P1Run& r, int word_index) {
    uint32_t addr = word_addr(kP1Base + 2 * word_index);
    auto it = r.first_write.find(addr);
    return it == r.first_write.end() ? 0 : it->second;
}

// Advances h until the P1 log word at index `word_idx` (kP1Base + 2*word_idx)
// is written, with any value; used to step live on frame boundaries (the W
// step's M1 marker, the R step's last word) so a grounding window can be
// applied at the ports exactly between two such writes.
std::optional<uint64_t> run_until_p1_slot(nmh::NanomigHarness& h, int word_idx, uint64_t max_cycles) {
    return run_until_write_any(h, kP1Base + 2 * word_idx, max_cycles);
}

// A window during which `ground` is applied at the ports, opened right
// after the write at `lo_word` and closed right after the write at
// `hi_word` (inclusive of both writes, i.e. the window covers everything
// timestamped strictly between them plus the moment of `hi_word` itself).
struct GroundWindow { int lo_word, hi_word; PotGround ground; };

// Runs prog_p1 (or a compiled variant) with a sequence of grounding
// windows applied live at the ports (ports are released outside every
// window), collecting the same P1Run a plain run would. Windows must be
// given in the order their lo_word occurs in the program.
P1Run run_p1_windowed(const std::string& bin, nmh::Config cfg, int nwords,
                       const std::vector<GroundWindow>& windows) {
    nmh::NanomigHarness h(cfg);
    P1Run r; r.nwords = nwords;
    bool loaded = h.load_program(bin) > 0;
    if (!loaded) return r;
    r.loaded = true;
    release_ports(h);
    r.released = h.boot(kBootBudget).cpu_released;
    for (auto& win : windows) {
        run_until_p1_slot(h, win.lo_word, 20'000'000);
        apply_pot_ground(h, win.ground);
        run_until_p1_slot(h, win.hi_word, 20'000'000);
        release_ports(h);
    }
    r.done = h.run_until_write(kDoneAddr, kDoneValue, 20'000'000);
    for (auto& a : h.ram_log())
        if (a.is_write) { auto it = r.first_write.find(a.word_addr); if (it == r.first_write.end()) r.first_write[a.word_addr] = a.cycle; }
    for (int i = 0; i < nwords; ++i) r.word.push_back(h.peek_logical(kP1Base + 2 * i));
    return r;
}

// =========================================================================
// The joystick program (shared by P2, P3, P4).
// =========================================================================

constexpr uint32_t kJoyMark    = 0x0C1C00;
constexpr uint32_t kJoyResults = 0x0C2000;
constexpr int kNSteps = 54;
constexpr uint64_t kStepBudget = 200'000; // >> one PAL/NTSC line (~1816 cycles)
// Measured directly: prog_fixture's own DONE, a three-instruction program,
// needed ~2.27M post-boot cycles here -- well past what boot() alone
// reaching cpu_nrst_out's first edge would suggest. Budget for the gap
// once, before step 0's own wait.
constexpr uint64_t kFirstStepBudget = 6'000'000;

enum class Conn { NONE, A, B };

struct Levels { bool up = true, down = true, left = true, right = true, fire = true; };

uint8_t encode(const Levels& L) {
    uint8_t v = 0;
    if (!L.up)    v |= (1u << 3);
    if (!L.down)  v |= (1u << 2);
    if (!L.left)  v |= (1u << 1);
    if (!L.right) v |= (1u << 0);
    if (!L.fire)  v |= (1u << 4);
    return v;
}

struct StepDef { Conn conn; std::string group; Levels lv; };

std::vector<StepDef> baseline_steps() {
    std::vector<StepDef> s;
    s.push_back({Conn::NONE, "N", {}}); // N0
    s.push_back({Conn::NONE, "N", {}}); // N1
    for (Conn c : {Conn::A, Conn::B}) {
        s.push_back({c, "F1", {true, true, true, true, false}});
        s.push_back({c, "F0", {true, true, true, true, true}});
        const bool yu_up[4] = {false, false, true, true}, yu_lf[4] = {true, false, false, true};
        for (int i = 0; i < 4; ++i) s.push_back({c, "Yup", {yu_up[i], true, yu_lf[i], true, true}});
        const bool yd_up[4] = {true, false, false, true}, yd_lf[4] = {false, false, true, true};
        for (int i = 0; i < 4; ++i) s.push_back({c, "Ydown", {yd_up[i], true, yd_lf[i], true, true}});
        const bool xu_dn[4] = {false, false, true, true}, xu_rt[4] = {true, false, false, true};
        for (int i = 0; i < 4; ++i) s.push_back({c, "Xup", {true, xu_dn[i], true, xu_rt[i], true}});
        const bool xd_dn[4] = {true, false, false, true}, xd_rt[4] = {false, false, true, true};
        for (int i = 0; i < 4; ++i) s.push_back({c, "Xdown", {true, xd_dn[i], true, xd_rt[i], true}});
        struct BS { bool p1, p3, p4; };
        constexpr BS combos[8] = {
            {true, false, false}, {true, true, false}, {false, true, false}, {false, true, true},
            {true, true, true},   {true, false, true}, {false, false, true}, {false, false, false},
        };
        for (auto& b : combos) s.push_back({c, "Buttons", {!b.p1, true, !b.p3, !b.p4, true}});
    }
    return s;
}

Levels levels_of(const StepDef& s, Conn which) { return s.conn == which ? s.lv : Levels{}; }

int find_step(const std::vector<StepDef>& steps, Conn conn, const std::string& group, int occurrence = 0) {
    int seen = 0;
    for (int i = 0; i < (int)steps.size(); ++i)
        if (steps[i].conn == conn && steps[i].group == group) { if (seen == occurrence) return i; ++seen; }
    return -1;
}
std::vector<int> find_steps(const std::vector<StepDef>& steps, Conn conn, const std::string& group) {
    std::vector<int> out;
    for (int i = 0; i < (int)steps.size(); ++i) if (steps[i].conn == conn && steps[i].group == group) out.push_back(i);
    return out;
}

struct Applied { uint8_t j0 = 0, j1 = 0, mbtn = 0; bool delay_until_uk = false; };

Applied compute_applied(const std::vector<StepDef>& steps, int k, const std::string& mv) {
    StepDef s = steps[k];
    bool moving = (s.group == "Yup" || s.group == "Ydown" || s.group == "Xup" || s.group == "Xdown" || s.group == "Buttons");

    if (mv == "P2VAR" && moving && s.conn != Conn::NONE) s.conn = (s.conn == Conn::A) ? Conn::B : Conn::A;

    Levels a = levels_of(s, Conn::A);
    Levels b = levels_of(s, Conn::B);
    bool extra_a5 = false; // connector A pin9 (LY) forced to level 0

    if (mv == "Q2" && steps[k].conn == Conn::A && steps[k].group == "Yup") extra_a5 = true;
    if (mv == "Q3" && k == 1) a = Levels{false, true, true, true, true}; // Yup(A) sub-step 0's pattern, at N1
    if (mv == "Q4" && steps[k].conn == Conn::A && steps[k].group == "F0") a.fire = false; // held pressed instead of released; F1(A) is untouched, so the anchor is unaffected
    if (mv == "P2ANCHOR" && steps[k].conn == Conn::A && steps[k].group == "F1") b.fire = false; // additionally grounds pin 6 of B; A's own pattern (which Q4 reads) is unchanged
    if (mv == "P3ANCHOR" && (k == 0 || k == 1)) a.up = false; // N0 and N1 both carry Yup(A) sub-step 0's pattern on connector A instead of staying released
    if (mv == "P3VAR" && steps[k].conn == Conn::A && steps[k].group == "Yup") { a.down = a.up; a.up = true; }
    if (mv == "P4VAR" && steps[k].conn == Conn::A && steps[k].group == "Buttons") { a.down = a.up; a.up = true; }

    Applied ap;
    ap.j1 = encode(a);
    ap.j0 = encode(b);
    if (extra_a5) ap.j1 |= (1u << 5);
    if (mv == "Q1" && steps[k].conn == Conn::A && steps[k].group == "Yup") ap.delay_until_uk = true;
    return ap;
}

struct JoyRun {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<std::array<uint16_t, 4>> rec; // [JOY0DAT, JOY1DAT, CIAAPRA(byte), POTGOR]
    std::vector<uint64_t> t_apply, t_t, t_u;
    std::vector<uint8_t> ap_j0, ap_j1;        // the levels the test applied at t_apply(k)
    PortTrace trace;                          // every port, from the end of boot to DONE
    bool timed_out = false;
    int steps_completed = 0;
};

JoyRun run_joy(const std::string& bin, const std::string& mv, nmh::Config cfg) {
    JoyRun r;
    static const std::vector<StepDef> steps = baseline_steps();
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    h.ports()->joystick0 = 0;
    h.ports()->joystick1 = 0;
    h.ports()->mouse_buttons = 0;
    r.trace.sample(h);

    for (int k = 0; k < kNSteps; ++k) {
        auto ts = traced_until_write(h, r.trace, kJoyMark, (uint16_t)k, k == 0 ? kFirstStepBudget : kStepBudget); // S_k
        if (!ts) { r.timed_out = true; break; }
        // Q2 contrasts: a mouse button or a keyboard-mouse data byte held for a
        // few cycles inside step 10's wait, then released again, well before
        // that step's reads.
        if (k == 10 && mv == "Q2MOUSE") {
            h.ports()->mouse_buttons = 1; r.trace.sample(h);
            traced_cycles(h, r.trace, 64);
            h.ports()->mouse_buttons = 0; r.trace.sample(h);
        }
        if (k == 10 && mv == "Q2KEY") {
            h.ports()->kbd_mouse_data = 0x55; r.trace.sample(h);
            traced_cycles(h, r.trace, 64);
            h.ports()->kbd_mouse_data = 0; r.trace.sample(h);
        }
        Applied ap = compute_applied(steps, k, mv);
        if (!ap.delay_until_uk) {
            h.ports()->joystick0 = ap.j0; h.ports()->joystick1 = ap.j1;
            r.trace.sample(h);
            r.t_apply.push_back(h.cycles()); // the real application time: now
            r.ap_j0.push_back(ap.j0); r.ap_j1.push_back(ap.j1);
        }
        auto tt = traced_until_write(h, r.trace, kJoyMark, (uint16_t)k, kStepBudget); // T_k
        if (!tt) { r.timed_out = true; break; }
        r.t_t.push_back(*tt);
        // Q1 hold contrast: one level of connector B changes after T_k, before U_k.
        if (k == 5 && mv == "Q1HOLD") { h.ports()->joystick0 ^= 0x80; r.trace.sample(h); }
        auto tu = traced_until_write(h, r.trace, kJoyResults + 8 * k, std::nullopt, kStepBudget); // U_k (JOY0DAT store)
        if (!tu) { r.timed_out = true; break; }
        r.t_u.push_back(*tu);
        if (ap.delay_until_uk) {
            h.ports()->joystick0 = ap.j0; h.ports()->joystick1 = ap.j1;
            r.trace.sample(h);
            r.t_apply.push_back(h.cycles()); // the real application time: only now, after U_k
            r.ap_j0.push_back(ap.j0); r.ap_j1.push_back(ap.j1);
        }
        r.steps_completed = k + 1;
    }
    r.done = traced_until_write(h, r.trace, kDoneAddr, kDoneValue, kStepBudget);
    for (int k = 0; k < kNSteps; ++k) {
        std::array<uint16_t, 4> rr{0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
        for (int w = 0; w < 4; ++w) { auto v = h.peek_logical(kJoyResults + 8 * k + 2 * w); if (v) rr[w] = *v; }
        r.rec.push_back(rr);
    }
    return r;
}

// Figure 8-2 (p.230): (level1,level2) -> (D1,D0) -> state s.
int state_of(bool p1, bool p2) {
    if (p1 && p2) return 0;   // 1 1 : 0 0
    if (!p1 && p2) return 1;  // 0 1 : 0 1
    if (!p1 && !p2) return 2; // 0 0 : 1 0
    return 3;                 // 1 0 : 1 1
}
int d1d0_of(int s) {
    switch (s) { case 0: return 0b00; case 1: return 0b01; case 2: return 0b10; default: return 0b11; }
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    const nmh::Config default_cfg = cfg_for(kConfigs[0]);

    // A mutation selector names the part of the package whose lines it
    // changes; only that part runs under it. The clean run (no selector)
    // runs everything.
    auto in_list = [&](std::initializer_list<const char*> l) {
        for (const char* x : l) if (mv == x) return true;
        return false;
    };
    const bool p1_selector = in_list({"K1", "K3", "K4", "K5", "K6", "K7", "OUTLOW", "VGREEN", "P1LOAD", "CFGMUT"});
    const bool joy_selector = in_list({"Q1", "Q1HOLD", "Q2", "Q2MOUSE", "Q2KEY", "Q3", "Q4", "P2VAR", "P2ANCHOR",
                                       "P3ANCHOR", "P3VAR", "P4VAR", "JOYLOAD", "JOYCFG"});
    const bool part_p1 = mv.empty() || p1_selector || (!joy_selector && mv != "FIXTURE");
    const bool part_joy = mv.empty() || joy_selector || (!p1_selector && mv != "FIXTURE");

    // =====================================================================
    // FIXTURE
    // =====================================================================
    {
        std::string bin = dir + (mv == "FIXTURE" ? "/prog_fixture_mut.bin" : "/prog_fixture.bin");
        nmh::NanomigHarness h(default_cfg);
        bool loaded = h.load_program(bin) > 0;
        bool released = loaded && h.boot(kBootBudget).cpu_released;
        auto done = released ? h.run_until_write(kDoneAddr, kDoneValue, 9'000'000) : std::nullopt;
        auto v = done ? h.peek_logical(0x0C0100) : std::nullopt;
        checks.expect(loaded && released && done.has_value() && v && *v == 0xFEED,
            "FIXTURE: a known constant stored by the program reads back correctly "
            "[not graded -- apparatus check, no claim uid]");
    }

    // =====================================================================
    // Proposition 1 (p.240): opus:8-dio.c21, sol:8-dio.c19
    // =====================================================================
    if (part_p1) {
        auto frames = build_p1_frames();
        int nwords = 347;

        std::string bin = dir + "/" + (mv == "K1" ? "prog_p1_k1" : mv == "K3" ? "prog_p1_k3" :
                                        mv == "K4" ? "prog_p1_k4" : mv == "K6" ? "prog_p1_k6" :
                                        mv == "K7" ? "prog_p1_k7" :
                                        mv == "OUTLOW" ? "prog_p1_outlow" : "prog_p1") + ".bin";
        const std::string absent_bin = dir + "/prog_p1_absent.bin";

        // Baseline stimulus: all four pot lines released (level 1) throughout,
        // i.e. mouse_buttons=0 and joystick0/1 bits released.
        auto run_variant = [&](nmh::Config c, const std::string& b) {
            nmh::NanomigHarness h(c);
            bool loaded = h.load_program(b) > 0;
            P1Run r; r.nwords = nwords;
            if (!loaded) return r;
            r.loaded = true;
            release_ports(h);
            r.released = h.boot(kBootBudget).cpu_released;
            r.done = h.run_until_write(kDoneAddr, kDoneValue, 20'000'000);
            for (auto& a : h.ram_log())
                if (a.is_write) { auto it = r.first_write.find(a.word_addr); if (it == r.first_write.end()) r.first_write[a.word_addr] = a.cycle; }
            for (int i = 0; i < nwords; ++i) r.word.push_back(h.peek_logical(kP1Base + 2 * i));
            return r;
        };

        // Grades one run's PC1-PC4, the five "0"/"1" columns and the START
        // aggregate, with every printed label carrying `tag` (empty for the
        // default PAL+ECS run, "-<config>" for the three others
        // SPECIFICATION.md's "Configurations" section also asks every program
        // to run under).
        auto grade_p1 = [&](const P1Run& r, const std::string& tag) {
            auto mask_ok = [&](uint16_t pg, uint16_t want) { return (pg & 0x5500) == want; };

            bool pc1 = true; int pc1_r_records = 0;
            for (auto& fr : frames) {
                if (wv(r, fr.w.vw0) != 4 || wv(r, fr.w.vw1) != 4) pc1 = false;
                bool any = false;
                for (int i = 0; i < 4; ++i)
                    if (wv(r, fr.r.vr0[i]) == fr.r.target && wv(r, fr.r.vr1[i]) == fr.r.target) { any = true; ++pc1_r_records; }
                if (!any) pc1 = false;
            }
            checks.expect(pc1, "PC1" + tag + " [p.238,p.239]: every W's Vw0=Vw1=4, and every R step "
                "has at least one record with Vr0=Vr1=10");
            checks.note("PC1" + tag + ": " + std::to_string(pc1_r_records) + " qualifying R records "
                        "across " + std::to_string(frames.size()) + " R steps");

            bool pc2 = true;
            for (auto& fr : frames) {
                if (!(fr.is_p2_high || fr.is_p2_low || fr.is_fa)) continue;
                uint16_t want = fr.is_p2_low ? 0x0000 : 0x5500;
                for (int i = 0; i < 4; ++i)
                    if (wv(r, fr.r.vr0[i]) == fr.r.target && wv(r, fr.r.vr1[i]) == fr.r.target)
                        if (!mask_ok(wv(r, fr.r.pg[i]), want)) pc2 = false;
            }
            checks.expect(pc2, "PC2" + tag + " [p.240 \"The state of the data bit controls the logic "
                "level on the output pin.\"; p.241]: Rhigh, Rhigh2 and every Ra read $5500; Rlow reads $0000");

            bool pc3 = true;
            for (auto& fr : frames) {
                if (fr.is_fb)
                    for (int i = 0; i < 4; ++i)
                        if (wv(r, fr.r.vr0[i]) == fr.r.target && wv(r, fr.r.vr1[i]) == fr.r.target)
                            if (!mask_ok(wv(r, fr.r.pg[i]), 0x5500)) pc3 = false;
                if (fr.is_fc && fr.w.has_rpre)
                    if (!mask_ok(wv(r, fr.w.rpre), 0x5500)) pc3 = false;
            }
            checks.expect(pc3, "PC3" + tag + " [p.240]: Rb and Rpre read $5500 in every configuration");

            bool pc4 = true; int pc4_checked = 0;
            for (auto& fr : frames) {
                uint64_t t_m1 = cyc(r, fr.w.m1);
                for (int i = 0; i < 4; ++i)
                    if (wv(r, fr.r.vr0[i]) == fr.r.target && wv(r, fr.r.vr1[i]) == fr.r.target) {
                        uint64_t t_m2 = cyc(r, fr.r.m2[i]);
                        ++pc4_checked;
                        if (!(t_m2 > t_m1 && ns_from_cycles(t_m2 - t_m1) >= 300000.0)) pc4 = false;
                    }
            }
            checks.expect(pc4, "PC4" + tag + " [p.240 \"it can take up to 300 microseconds for the "
                "line to change state\"]: every counted R record has t(M2)-t(M1) >= 300us");
            checks.note("PC4" + tag + ": checked " + std::to_string(pc4_checked) + " records against "
                        "the 300us bound (clk_sys=28.375160MHz)");

            bool pre_ok = pc1 && pc2 && pc3 && pc4;
            bool all_zero = true, all_counted = true;
            for (int ci = 0; ci < 5; ++ci) {
                auto& fc = frames[3 + 3 * ci + 2]; // C{ci}-Fc
                bool zero_ok = true, one_ok = true; int counted = 0;
                std::vector<uint16_t> counted_pg;
                for (int i = 0; i < 4; ++i)
                    if (wv(r, fc.r.vr0[i]) == fc.r.target && wv(r, fc.r.vr1[i]) == fc.r.target) {
                        ++counted;
                        uint16_t pg = wv(r, fc.r.pg[i]) & 0x5500;
                        counted_pg.push_back(pg);
                        if ((pg & ~fc.out_mask) != 0) zero_ok = false;  // any dumped-line bit set = fail
                        if (fc.out_mask && (pg & fc.out_mask) != fc.out_mask) one_ok = false;
                    }
                if (counted == 0) all_counted = false;
                if (!zero_ok) all_zero = false;
                bool established = pre_ok && counted > 0;
                checks.expect(established && zero_ok,
                    fc.label + tag + " [p.240 Table 8-4 \"00 START Start pots (dump capacitors, start "
                    "counters)\"]: every input line of " + fc.label + " reads 0 in POTGOR after START "
                    "(one configuration's lines; the START claims are asserted by P1-START" + tag + ")" +
                    (established ? "" : " (preconditions not established: not graded as pass)"));
                if (fc.out_mask)
                    checks.expect(established && one_ok,
                        fc.label + "-OUT" + tag + " [p.240 \"the Amiga disconnects the potentiometer "
                        "control circuitry...and configures the pin for output\"]: the output-driven "
                        "line of " + fc.label + " still reads 1 (not this package's dump claim)" +
                        (established ? "" : " (preconditions not established: not graded as pass)"));
                if (counted > 0 && !zero_ok) {
                    char buf[16], mbuf[16];
                    std::string hexlist;
                    for (size_t i = 0; i < counted_pg.size(); ++i) {
                        std::snprintf(buf, sizeof buf, "$%04X", counted_pg[i]);
                        if (i) hexlist += ",";
                        hexlist += buf;
                    }
                    std::snprintf(mbuf, sizeof mbuf, "$%04X", fc.out_mask);
                    checks.record(fc.label + tag + ": measured POTGOR&$5500 in every counted record: " +
                        hexlist + " where the page predicts " + mbuf);
                }
            }
            // The START claims say every one of the four lines: that needs every
            // configuration C0-C4, in each of which a different set of lines is
            // an input, so this line alone carries the ids.
            bool agg_established = pre_ok && all_counted;
            checks.expect(agg_established && all_zero,
                "P1-START" + tag + " [p.240 Table 8-4 \"00 START Start pots (dump capacitors, start "
                "counters)\"; p.282] opus:8-dio.c21 sol:8-dio.c19: after START every pot line that is an "
                "input reads 0 in POTGOR, in each of C0-C4 (together all four lines: LX, LY, RX, RY)" +
                (agg_established ? "" : " (preconditions not established: not graded as pass)"));
        };

        // One run per configuration, all four graded on their own. `runner`
        // produces the run of configuration ci.
        auto grade_all = [&](const std::function<P1Run(nmh::Config, int)>& runner) {
            std::vector<P1Run> runs;
            for (int ci = 0; ci < 4; ++ci) runs.push_back(runner(cfg_for(kConfigs[ci]), ci));
            checks.expect(runs[0].loaded && runs[0].released && runs[0].done.has_value(),
                "P1-FIXTURE: prog_p1 loads, releases reset and reaches DONE "
                "[not graded -- apparatus check, no claim uid]");
            bool all_ran = true;
            for (int ci = 1; ci < 4; ++ci)
                if (!(runs[ci].loaded && runs[ci].released && runs[ci].done.has_value())) all_ran = false;
            checks.expect(all_ran,
                "P1-CONFIGS: PAL/NTSC x ECS/OCS all load, release and reach DONE "
                "[not graded -- apparatus check, no claim uid]");
            for (int ci = 0; ci < 4; ++ci) {
                const std::string tag = ci == 0 ? std::string() : std::string("-") + kConfigs[ci].name;
                if (runs[ci].loaded && runs[ci].done) grade_p1(runs[ci], tag);
                else checks.record(std::string("NOT GRADED: PC1-PC4, C0-Fc..C4-Fc, C1-Fc-OUT..C4-Fc-OUT and P1-START in ") +
                    kConfigs[ci].name + " -- the run did not load and reach DONE");
            }
            return runs;
        };

        if (mv == "K5") {
            // Grounds every pot line reachable at the ports (LX, LY, RY) only
            // from each configuration's Fb W step through the end of its Fc R
            // step (stepped on the M1 marker writes), not for the whole run --
            // so Phase P (PC2's Rhigh/Rlow/Ra) is untouched and only PC3 (Rb,
            // Rpre) is hit.
            auto runs = grade_all([&](nmh::Config c, int) {
                std::vector<GroundWindow> windows;
                for (int ci = 0; ci < 5; ++ci) {
                    int fb = 3 + 3 * ci + 1, fc = 3 + 3 * ci + 2;
                    windows.push_back({frames[fb].w.m1, frames[fc].r.vr1[3], PotGround{true, true, true}});
                }
                return run_p1_windowed(bin, c, nwords, windows);
            });
            for (int ci = 0; ci < 4; ++ci) {
                auto& r = runs[ci];
                if (!(r.loaded && r.done)) continue;
                auto& fb0 = frames[4]; // C0-Fb
                auto& fc0 = frames[5]; // C0-Fc
                uint16_t rb_val = 0xFFFF;
                for (int i = 0; i < 4; ++i)
                    if (wv(r, fb0.r.vr0[i]) == fb0.r.target && wv(r, fb0.r.vr1[i]) == fb0.r.target) { rb_val = wv(r, fb0.r.pg[i]) & 0x5500; break; }
                uint16_t rpre_val = fc0.w.has_rpre ? (wv(r, fc0.w.rpre) & 0x5500) : 0xFFFF;
                char buf[240];
                std::snprintf(buf, sizeof buf, "K5 %s: measured Rb=$%04X Rpre=$%04X (configuration C0) with "
                    "LX, LY and RY grounded from each configuration's Fb W step through the end of its "
                    "Fc R step", kConfigs[ci].name, rb_val, rpre_val);
                checks.record(buf);
            }
        } else if (mv == "VGREEN") {
            // Proven-live input through the same comparator: ground each
            // configuration's reachable input lines (excluding that
            // configuration's own output line) from Fc's M1 marker to the
            // end of its R step.
            const bool lx_g[5] = {true, false, true, true, true};
            const bool ly_g[5] = {true, true, false, true, true};
            const bool ry_g[5] = {true, true, true, true, false};
            grade_all([&](nmh::Config c, int) {
                std::vector<GroundWindow> windows;
                for (int ci = 0; ci < 5; ++ci) {
                    int fc = 3 + 3 * ci + 2;
                    windows.push_back({frames[fc].w.m1, frames[fc].r.vr1[3], PotGround{lx_g[ci], ly_g[ci], ry_g[ci]}});
                }
                return run_p1_windowed(bin, c, nwords, windows);
            });
            checks.record("VGREEN: C0-Fc, C1-Fc, C2-Fc and C4-Fc each still have bit 12 (RX, "
                "connector B pin 5) among their input lines; no port this harness offers grounds "
                "RX, so they cannot be shown green through this comparator.");
        } else {
            grade_all([&](nmh::Config c, int ci) {
                const bool absent = (mv == "P1LOAD" && ci == 0) || (mv == "CFGMUT" && ci == 1);
                return run_variant(c, absent ? absent_bin : bin);
            });
        }
    }

    // =====================================================================
    // The joystick program: P2, P3, P4.
    // =====================================================================
    if (part_joy) {
        static const std::vector<StepDef> steps = baseline_steps();
        std::string bin = dir + (mv == "JOYLOAD" ? "/prog_joy_absent.bin" : "/prog_joy.bin");

        auto run_cfg = [&](const std::string& mvv, nmh::Config c) { return run_joy(bin, mvv, c); };

        JoyRun r = run_cfg(mv, default_cfg);
        checks.expect(r.loaded && r.released && !r.timed_out && r.steps_completed == kNSteps && r.done.has_value(),
            "JOY-FIXTURE: prog_joy loads, releases reset, completes all 54 steps and reaches DONE "
            "[not graded -- apparatus check, no claim uid]");

        if (r.loaded && !r.timed_out && r.steps_completed == kNSteps && r.done.has_value()) {
            bool cross_agree = true; std::string disagree;
            if (mv.empty() || mv == "JOYCFG") {
                for (int ci = 1; ci < 4; ++ci) {
                    JoyRun r2 = run_cfg((mv == "JOYCFG" && ci == 1) ? "Q3" : "", cfg_for(kConfigs[ci]));
                    bool same = r2.loaded && !r2.timed_out && r2.steps_completed == kNSteps && r2.done.has_value();
                    for (int k = 0; same && k < kNSteps; ++k) same = (r2.rec[k] == r.rec[k]);
                    if (!same) { cross_agree = false; disagree += std::string(kConfigs[ci].name) + " "; }
                }
                checks.expect(cross_agree,
                    "JOY-CONFIGS: PAL/NTSC x ECS/OCS agree word-for-word with the PAL+ECS run "
                    "[not graded -- apparatus check, no claim uid]" +
                    (cross_agree ? std::string() : (" -- disagreed: " + disagree)));
            }

            // opus:8-appa-joy.c51, sol:8-appa-joy.c25: which chip samples a
            // pin, through which of its pins, and at which clock phase
            // (CCK/CCK*) never appears on any output this test can read,
            // and p.281 says "This table is for reference only and should
            // not be needed by the programmer" -- not asserted here.
            checks.record("opus:8-appa-joy.c51 sol:8-appa-joy.c25 declined: which chip samples "
                "a pin and at which clock phase is not visible at nanomig.v's ports (p.281 \"This "
                "table is for reference only and should not be needed by the programmer\")");

            // ---- Preconditions Q1-Q4 ----------------------------------
            // Q1's two clauses, each over all 54 steps: the levels were applied
            // before T_k, and the drive trace shows the ports holding exactly
            // those levels from t_apply(k) until t(U_k) (no port changes in
            // between).
            bool q1_timing = true, q1_hold = true;
            uint64_t min_tt = UINT64_MAX, max_tt = 0, min_ut = UINT64_MAX, max_ut = 0;
            for (int k = 0; k < kNSteps; ++k) {
                if (!(r.t_apply[k] < r.t_t[k])) q1_timing = false;
                PortState held = r.trace.at(r.t_apply[k]);
                if (held.j0 != r.ap_j0[k] || held.j1 != r.ap_j1[k]) q1_hold = false;
                if (r.t_apply[k] < r.t_u[k] && r.trace.changed_in(r.t_apply[k], r.t_u[k])) q1_hold = false;
                if (r.t_apply[k] < r.t_t[k]) {
                    uint64_t d = r.t_t[k] - r.t_apply[k];
                    min_tt = std::min(min_tt, d); max_tt = std::max(max_tt, d);
                }
                if (r.t_t[k] < r.t_u[k]) {
                    uint64_t d = r.t_u[k] - r.t_t[k];
                    min_ut = std::min(min_ut, d); max_ut = std::max(max_ut, d);
                }
            }
            bool q1 = q1_timing && q1_hold;
            checks.expect(q1, "Q1 [protocol]: for every step k=0..53, t_apply(k) < t(T_k), and the "
                "levels of step k are unchanged (the drive trace shows no port change) from t_apply(k) "
                "until t(U_k)");
            {
                char buf[240];
                std::snprintf(buf, sizeof buf,
                    "Q1: over all 54 steps, t(T_k)-t_apply(k) = %llu-%llu clk_sys; "
                    "t(U_k)-t(T_k) = %llu-%llu clk_sys; timing clause %s, hold clause %s "
                    "(%zu port changes in the trace)",
                    (unsigned long long)min_tt, (unsigned long long)max_tt,
                    (unsigned long long)min_ut, (unsigned long long)max_ut,
                    q1_timing ? "holds" : "fails", q1_hold ? "holds" : "fails", r.trace.ev.size());
                checks.note(buf);
            }

            int f1a = find_step(steps, Conn::A, "F1"), f0a = find_step(steps, Conn::A, "F0");
            int f1b = find_step(steps, Conn::B, "F1"), f0b = find_step(steps, Conn::B, "F0");

            bool q2_potgor = true;
            for (int k = 0; k < kNSteps; ++k) if ((r.rec[k][3] & 0x5500) != 0x5500) q2_potgor = false;
            // The F steps belong to the level convention (Q4) and the anchor:
            // the F1 records are read by the anchor, and the own-connector bit
            // of an F0 record by Q4. Q2 reads every other CIAAPRA bit.
            bool q2_ciaapra = true;
            for (int k = 0; k < kNSteps; ++k) {
                bool bit6 = (r.rec[k][2] & 0x40) != 0, bit7 = (r.rec[k][2] & 0x80) != 0;
                if (steps[k].group == "F1") continue;
                if (steps[k].group == "F0") {
                    if (steps[k].conn == Conn::A && !bit7) q2_ciaapra = false;
                    if (steps[k].conn == Conn::B && !bit6) q2_ciaapra = false;
                    continue;
                }
                if (!bit6 || !bit7) q2_ciaapra = false;
            }
            // The drive trace over the whole run, boot to DONE: no mouse button
            // and no keyboard-mouse data or level at any cycle.
            bool q2_quiet = true;
            for (auto& e : r.trace.ev)
                if (e.s.mbtn != 0 || e.s.kbd_level != 0 || e.s.kbd_type != 0 || e.s.kbd_data != 0) q2_quiet = false;
            bool q2 = q2_potgor && q2_ciaapra && q2_quiet;
            checks.expect(q2, "Q2 [p.241 \"set the proper pin to output, and drive the line high (set "
                "both OUT... and DAT... to 1). Reading POTGOR will produce a 0 if the button is "
                "pressed, a 1 if it is not.\"; p.232 \"A logic state of 1 means 'switch open.'\"]: in "
                "every record outside the F steps CIAAPRA bits 6 and 7 read 1 (in F0(A) bit 7 and in "
                "F0(B) bit 6); POTGOR bits 14,12,10,8 read 1 in every record; the drive trace shows no "
                "mouse button and no keyboard-mouse input at any cycle of the run");

            int n0 = 0, n1 = 1;
            bool q3 = (r.rec[n0][0] == r.rec[n1][0]) && (r.rec[n0][1] == r.rec[n1][1]);
            checks.expect(q3, "Q3 [p.281 \"Each counter is clocked by signals from two controller "
                "pins.\"]: JOY0DAT and JOY1DAT are equal in N0 and N1");

            bool q4 = ((r.rec[f1a][2] & 0x40) == 0) && ((r.rec[f0a][2] & 0x40) != 0) &&
                      ((r.rec[f1b][2] & 0x80) == 0) && ((r.rec[f0b][2] & 0x80) != 0);
            checks.expect(q4, "Q4 [p.232 \"A logic state of 1 means 'switch open.' A logic state of 0 "
                "means 'switch closed.'\"]: level 0 on pin 6 of A reads 0 in CIAAPRA bit 6 in F1(A), "
                "and level 1 reads 1 in F0(A) (and symmetrically for B, bit 7)");

            // ---- P2 anchor (section 7) ----------------------------------
            bool anchor = ((r.rec[f1a][2] & 0x40) == 0) && ((r.rec[f1a][2] & 0x80) != 0) &&
                          ((r.rec[f1b][2] & 0x80) == 0) && ((r.rec[f1b][2] & 0x40) != 0);
            checks.expect(anchor, "P2-ANCHOR [p.232 \"Port 1 uses bit 6 and port 2 uses bit 7\"; p.233]: "
                "F1(A) reads CIAAPRA bit6=0,bit7=1; F1(B) reads bit7=0,bit6=1 -- connector A is the "
                "one whose fire button is bit 6");

            // Gate (SPECIFICATION.md: "A record that fails one of these is
            // not graded, and the proposition is 'not established'"; P2 §7:
            // "If the anchor fails ... P2 is 'not established'"). P2-P4 are
            // graded only when the anchor and Q1-Q4 all hold.
            bool established = anchor && q1 && q2 && q3 && q4;
            auto gtag = [&](bool ok) { return ok ? std::string() : std::string(" (preconditions not established: not graded as pass)"); };

            // ---- P2: which register serves which connector --------------
            auto block_range = [&](Conn c) {
                std::vector<int> idx; for (int i = 0; i < kNSteps; ++i) if (steps[i].conn == c) idx.push_back(i);
                return idx;
            };
            auto blockA = block_range(Conn::A), blockB = block_range(Conn::B);
            int ref_b_for_a = r.rec[n1][1];               // JOY1DAT at N1
            int ref_a_for_b = r.rec[blockA.back()][0];     // JOY0DAT at end of block A

            bool a_unchanged_other = true, a_moves = true;
            for (int k : blockA) {
                if (r.rec[k][1] != ref_b_for_a) a_unchanged_other = false;
                if (steps[k].group != "F1" && steps[k].group != "F0" && k > 0)
                    if (r.rec[k][0] == r.rec[k - 1][0]) a_moves = false;
            }
            checks.expect(established && a_unchanged_other,
                "P2-BLOCKA-JOY1 [p.281]: JOY1DAT stays at its N1 value throughout block A" + gtag(established));
            checks.expect(established && a_moves,
                "P2-BLOCKA-JOY0 [p.281]: JOY0DAT differs from "
                "the record before it in every Yup/Ydown/Xup/Xdown/Buttons step of block A" + gtag(established));

            bool b_unchanged_other = true, b_moves = true;
            for (int k : blockB) {
                if (r.rec[k][0] != ref_a_for_b) b_unchanged_other = false;
                bool first_of_block = (k == blockB.front());
                if (steps[k].group != "F1" && steps[k].group != "F0" && !first_of_block)
                    if (r.rec[k][1] == r.rec[k - 1][1]) b_moves = false;
            }
            checks.expect(established && b_unchanged_other,
                "P2-BLOCKB-JOY0 [p.281]: JOY0DAT stays at its "
                "end-of-block-A value throughout block B" + gtag(established));
            checks.expect(established && b_moves,
                "P2-BLOCKB-JOY1 [p.281]: JOY1DAT differs from "
                "the record before it in every Yup/Ydown/Xup/Xdown/Buttons step of block B" + gtag(established));

            // "four counters total": Y steps move only the high byte, X only the low.
            auto byte_moves = [&](int prev, int cur, int reg, bool high) -> bool {
                uint8_t a = high ? (r.rec[prev][reg] >> 8) : (r.rec[prev][reg] & 0xFF);
                uint8_t b = high ? (r.rec[cur][reg] >> 8)  : (r.rec[cur][reg] & 0xFF);
                return a != b;
            };
            bool four_counters = true;
            for (Conn c : {Conn::A, Conn::B}) {
                int reg = (c == Conn::A) ? 0 : 1;
                for (auto& grp : {std::string("Yup"), std::string("Ydown")})
                    for (int k : find_steps(steps, c, grp))
                        if (!byte_moves(k - 1, k, reg, true) || byte_moves(k - 1, k, reg, false)) four_counters = false;
                for (auto& grp : {std::string("Xup"), std::string("Xdown")})
                    for (int k : find_steps(steps, c, grp))
                        if (!byte_moves(k - 1, k, reg, false) || byte_moves(k - 1, k, reg, true)) four_counters = false;
            }
            checks.expect(established && four_counters,
                "P2-FOUR-COUNTERS [p.281 \"(four counters total)\"]: the Y steps of each connector "
                "move only its register's high byte, the X steps only its low byte" + gtag(established));
            // The P2 claims say 0=left pair, 1=right pair, four counters total:
            // one line holds all of it, and carries the ids.
            checks.expect(established && a_unchanged_other && a_moves && b_unchanged_other && b_moves &&
                              four_counters,
                "P2-JOYDAT [p.281 \"0=left controller pair, 1=right controller pair (four counters "
                "total).\"] opus:8-appa-joy.c11 opus:8-appa-joy.c12 sol:8-appa-joy.c13 sol:8-appa-joy.c14: "
                "JOY0DAT serves the left connector and JOY1DAT the right one: in block A JOY1DAT stays at "
                "its N1 value and JOY0DAT moves on every Y/X/Buttons step, in block B JOY0DAT stays at its "
                "end-of-block-A value and JOY1DAT moves on every Y/X/Buttons step, and the four counters "
                "(Y and X of each connector) each move only on their own connector's steps" + gtag(established));

            // ---- P3 reference frame: the state-0 anchor (section 7) ------
            // "at N0 (all open, s=0), bits 9, 8, 1 and 0 of both registers
            // read 0" -- asserted on its own, before P3 uses the state
            // numbering, and gating P3/P4.
            bool p3_anchor = ((r.rec[n0][0] & 0x0303) == 0) && ((r.rec[n0][1] & 0x0303) == 0);
            checks.expect(p3_anchor,
                "P3-ANCHOR [p.230 Figure 8-2 (state s=0, D1D0=00); p.281]: at N0 (all open, s=0), "
                "bits 9, 8, 1 and 0 of both JOY0DAT and JOY1DAT read 0");
            bool p3established = established && p3_anchor;

            // ---- P3: quadrature (pins1,3=Y; pins2,4=X) -------------------
            bool p3_y = true, p3_x = true;
            for (Conn c : {Conn::A, Conn::B}) {
                int reg = (c == Conn::A) ? 0 : 1;
                for (int k = 1; k < kNSteps; ++k) {
                    if (steps[k].conn == Conn::NONE && steps[k - 1].conn == Conn::NONE) continue; // N0/N1: no transition graded here
                    Levels lo = levels_of(steps[k - 1], c), ln = levels_of(steps[k], c);
                    int sy_o = state_of(lo.up, lo.left), sy_n = state_of(ln.up, ln.left);
                    int sx_o = state_of(lo.down, lo.right), sx_n = state_of(ln.down, ln.right);
                    uint8_t yb_o = r.rec[k - 1][reg] >> 8, yb_n = r.rec[k][reg] >> 8;
                    uint8_t xb_o = r.rec[k - 1][reg] & 0xFF, xb_n = r.rec[k][reg] & 0xFF;
                    int dy = (sy_n == (sy_o + 1) % 4) ? 1 : (sy_n == (sy_o + 3) % 4) ? -1 : 0;
                    int dx = (sx_n == (sx_o + 1) % 4) ? 1 : (sx_n == (sx_o + 3) % 4) ? -1 : 0;
                    if (yb_n != (uint8_t)(yb_o + dy)) p3_y = false;
                    if (xb_n != (uint8_t)(xb_o + dx)) p3_x = false;
                    if ((r.rec[k][reg] & 0x0300) >> 8 != d1d0_of(sy_n)) p3_y = false;
                    if ((r.rec[k][reg] & 0x0003) != d1d0_of(sx_n)) p3_x = false;
                }
            }
            checks.expect(p3established && p3_y,
                "P3-Y [p.281 \"Mouse counter usage: (pins 1,3=Yclock, pins 2,4=Xclock)\"; Figure 8-2] "
                "opus:8-appa-joy.c17 sol:8-appa-joy.c19: every step's Y-counter byte and low bits "
                "(9,8) follow pins 1 and 3 through Figure 8-2's state table, on both connectors and "
                "both directions" + gtag(p3established));
            checks.expect(p3established && p3_x,
                "P3-X [p.281] opus:8-appa-joy.c18 sol:8-appa-joy.c20: every step's X-counter byte and "
                "low bits (1,0) follow pins 2 and 4 through Figure 8-2's state table, on both "
                "connectors and both directions" + gtag(p3established));

            // ---- P4: Table 8-1 buttons ------------------------------------
            bool p4_pin3 = true, p4_pin4 = true, p4_pin1 = true, p4_pin2 = true;
            for (Conn c : {Conn::A, Conn::B}) {
                int reg = (c == Conn::A) ? 0 : 1;
                for (int k = 0; k < kNSteps; ++k) {
                    Levels lv = levels_of(steps[k], c);
                    uint16_t v = r.rec[k][reg];
                    bool bit9 = (v >> 9) & 1, bit8 = (v >> 8) & 1, bit1 = (v >> 1) & 1, bit0 = v & 1;
                    if (bit9 != !lv.left) p4_pin3 = false;
                    if (bit1 != !lv.right) p4_pin4 = false;
                    if ((bit9 ^ bit8) != (uint16_t)(!lv.up)) p4_pin1 = false;
                    if ((bit1 ^ bit0) != (uint16_t)(!lv.down)) p4_pin2 = false;
                }
            }
            checks.expect(p3established && p4_pin3,
                "P4-PIN3 [p.281 \"Left: Y1\"]: bit 9 reads 1 exactly "
                "when pin 3 is at level 0, on both connectors, in every record" + gtag(p3established));
            checks.expect(p3established && p4_pin4,
                "P4-PIN4 [p.281 \"Right: X1\"]: bit 1 reads 1 "
                "exactly when pin 4 is at level 0, on both connectors, in every record" + gtag(p3established));
            checks.expect(p3established && p4_pin1,
                "P4-PIN1 [p.281 \"Forward: Y1 xor Y0\"]: (bit9 xor bit8) reads 1 exactly when pin 1 is "
                "at level 0, on both connectors, in every record" + gtag(p3established));
            checks.expect(p3established && p4_pin2,
                "P4-PIN2 [p.281 \"Back: X1 xor X0\"]: (bit1 xor bit0) reads 1 exactly when pin 2 is "
                "at level 0, on both connectors, in every record (no step closes anything on pin 2 "
                "except where the table drives it, so this also checks that nothing reads there)" + gtag(p3established));
            // Table 8-1 (p.228) says which physical controller button is wired to
            // which connector pin; that is a property of the controller, and no
            // port of nanomig.v shows it. P4-PIN1..4 grade only the chip set's
            // own decode of a pin pulled to ground (p.281).
            checks.record("opus:8-tbl8-1.c3 opus:8-tbl8-1.c8 opus:8-tbl8-1.c9 opus:8-tbl8-1.c12 "
                "opus:8-tbl8-1.c13 sol:8-pin.c3 sol:8-pin.c9 sol:8-pin.c10 sol:8-pin.c13 sol:8-pin.c14 "
                "declined: which controller button is connected to which connector pin (p.228 "
                "Table 8-1) is controller-side wiring and is not observable at nanomig.v's ports; "
                "P4-PIN1, P4-PIN2, P4-PIN3 and P4-PIN4 grade the chip set's p.281 pin decode only");
        } else {
            checks.record("NOT GRADED: Q1-Q4, P2-ANCHOR, P2-*, P3-*, P4-* -- JOY-FIXTURE failed, so the "
                "joystick program's records were not collected");
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
