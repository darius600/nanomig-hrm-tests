// test_ch8_disk_serial.cpp -- ch8-disk-serial. See SPECIFICATION.md for
// every proposition, program, observed value and expected value.
//
// Test purpose: grade the serial transmitter's bit time (P1, P2, P3) and the nine-bit receive setting (P5) through the top-level ports, with the preconditions of the disk write DMA rate (P4), on fx68k and on TG68K.
// Test class: HRM_CONFORMANCE for P1, P2, P3, (a), (b), (c) and their controls; INTEGRATION_SMOKE for the apparatus and precondition lines (FIXTURE, the R1 and R2 anchors, the G3/G4 completion lines, 4a-4f, 5a-5f).
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, printed pp. 255-259 and 283-284 (serial port), 194-195 and 23-24 (colour clock, fields), 246-250 and 265 (disk DMA), as read in SPECIFICATION.md.
// Requirement IDs: none; the claims sol:8-serper.c8, sol:8-serper.c9, sol:8-serper.c10, opus:8-serial.c36 and sol:8-serial.c21 are withdrawn from the graded lines and the other 29 declined, every id printed on a RECORDED line.
// Non-goals: every 15-bit divisor beyond N=0, 371 and 32396; the disk bit-cell time; RESULT bit 8 under a ninth bit of 0; an absolute duration; the long/short alternation of NTSC lines (measured, not graded; NM-HRM-010).
//
// NMH_MUTATE selects a stimulus substitution for every capture built from
// it, one at a time; a plain `make test` runs none. The comparator and the
// expected value of every line are the same under every selector, so the
// line that prints PASS in the baseline is the line that prints FAIL (or
// NOT GRADED, when one of its preconditions fails) under a mutation. A
// graded line is NOT GRADED, never PASS or FAIL, when a precondition it
// names was not graded and passed (SPECIFICATION.md 4.5).
//
// Every record below names the id a graded line begins with, the selector,
// what changed, and the result on each core; one record per graded line,
// even where one selector moves several lines. The three red lines P1, P2
// and P3 are red at baseline: their records name the live inputs of the same
// comparator (the controls) that go from PASS to FAIL under the selector.
// "Unfalsifiable at this boundary" marks a line that no stimulus the pages
// describe can make fail; it is not the same as a line the package chose not
// to drive.
//
// MUTATION FIXTURE stimulus red NMH_MUTATE=FIXTURE: the N=371 program (both standards) never writes the marker for k=3 (prog_tx_n371_nomark3.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: FIXTURE-PAL. Also NOT GRADED: 1c, 1e, 1d, P1-CTRL-WITHIN, 2c, 2e, 2d, P1, P2, P3.
// MUTATION FIXTURE-PAL stimulus red NMH_MUTATE=FIXTURE: the N=371 program (both standards) never writes the marker for k=3 (prog_tx_n371_nomark3.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: FIXTURE. Also NOT GRADED: 1c, 1e, 1d, P1-CTRL-WITHIN, 2c, 2e, 2d, P1, P2, P3.
// MUTATION 1a stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N0-R1, P1N32396-R1, 2a, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 1b stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N0-R2, P1N32396-R2, 2b, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 1c stimulus red NMH_MUTATE=P1-1C: ADKCON UARTBRK is forced low then high between the lead-in and MARK=0 (prog_tx_n371_1c.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 2c. Also NOT GRADED: P1-CTRL-WITHIN, P1, P2, P3.
// MUTATION 1d stimulus red NMH_MUTATE=P1-1D: the next word is polled on TSRE instead of TBE before it is written (prog_tx_n371_1d.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 2d. Also NOT GRADED: P1-CTRL-WITHIN, P1, P2, P3.
// MUTATION 1e stimulus red NMH_MUTATE=P1-1E: word 2's SERDAT write is left out while its transfer is still polled (prog_tx_n371_1e.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 2e. Also NOT GRADED: 1d, P1-CTRL-WITHIN, 2d, P1, P2, P3.
// MUTATION P1N0-R1 stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N32396-R1, 2a, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION P1N0-R2 stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N32396-R2, 2b, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION P1N0-DONE stimulus red NMH_MUTATE=P1-SINGLE-STALL: the N=0 and N=32396 programs follow the transmission with a bounded poll for a received character that never comes, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_tx_n0_stall.bin, prog_tx_n32396_stall.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N32396-DONE, P2N0-DONE, P2N32396-DONE. Also NOT GRADED: P1N0-IDLE, P1N32396-IDLE, P1-CTRL-N0, P1-CTRL-N32396, P2N0-IDLE, P2N32396-IDLE, P2-CTRL-PAIR, P1, P2. P3 prints FAIL, as at baseline.
// MUTATION P1N0-IDLE stimulus red NMH_MUTATE=P1-SINGLE-BRK: the N=0 and N=32396 programs force UARTBRK low then high between the lead-in and MARK=0 (prog_tx_n0_brk.bin, prog_tx_n32396_brk.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N32396-IDLE, P2N0-IDLE, P2N32396-IDLE. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P2-CTRL-PAIR, P1, P2. P3 prints FAIL, as at baseline.
// MUTATION P1N32396-R1 stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, 2a, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION P1N32396-R2 stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, 2b, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION P1N32396-DONE stimulus red NMH_MUTATE=P1-SINGLE-STALL: the N=0 and N=32396 programs follow the transmission with a bounded poll for a received character that never comes, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_tx_n0_stall.bin, prog_tx_n32396_stall.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N0-DONE, P2N0-DONE, P2N32396-DONE. Also NOT GRADED: P1N0-IDLE, P1N32396-IDLE, P1-CTRL-N0, P1-CTRL-N32396, P2N0-IDLE, P2N32396-IDLE, P2-CTRL-PAIR, P1, P2. P3 prints FAIL, as at baseline.
// MUTATION P1N32396-IDLE stimulus red NMH_MUTATE=P1-SINGLE-BRK: the N=0 and N=32396 programs force UARTBRK low then high between the lead-in and MARK=0 (prog_tx_n0_brk.bin, prog_tx_n32396_brk.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N0-IDLE, P2N0-IDLE, P2N32396-IDLE. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P2-CTRL-PAIR, P1, P2. P3 prints FAIL, as at baseline.
// MUTATION P1 stimulus red NMH_MUTATE=P1-NPLUS1: SERPER is written with N+1 (1, 372, 32397) by the three _plus1 programs, on both standards, and the runs are graded against N. P1 is red at baseline on both cores and stays FAIL, so it cannot be turned red; its comparator (check_grid on the N=371 NTSC run) runs on live inputs in P1-CTRL-N0 and P1-CTRL-N32396 (check_grid) and P1-CTRL-WITHIN (check_within_word), which print PASS at baseline and FAIL under this selector; TG68K FAIL, fx68k FAIL.
// MUTATION P1-CTRL-N0 stimulus red NMH_MUTATE=P1-NPLUS1: SERPER is written with N+1 (1, 372, 32397) by the three _plus1 programs, on both standards, and the runs are graded against N; TG68K FAIL, fx68k FAIL. Also printing FAIL: P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR. P1, P2, P3 print FAIL, as at baseline.
// MUTATION P1-CTRL-N32396 stimulus red NMH_MUTATE=P1-NPLUS1: SERPER is written with N+1 (1, 372, 32397) by the three _plus1 programs, on both standards, and the runs are graded against N; TG68K FAIL, fx68k FAIL. Also printing FAIL: P1-CTRL-N0, P1-CTRL-WITHIN, P2-CTRL-PAIR. P1, P2, P3 print FAIL, as at baseline.
// MUTATION P1-CTRL-WITHIN stimulus red NMH_MUTATE=P1-NPLUS1: SERPER is written with N+1 (1, 372, 32397) by the three _plus1 programs, on both standards, and the runs are graded against N; TG68K FAIL, fx68k FAIL. Also printing FAIL: P1-CTRL-N0, P1-CTRL-N32396, P2-CTRL-PAIR. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 2a stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, P1N32396-R1, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 2b stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, P1N32396-R2, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 2c stimulus red NMH_MUTATE=P1-1C: ADKCON UARTBRK is forced low then high between the lead-in and MARK=0 (prog_tx_n371_1c.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1c. Also NOT GRADED: P1-CTRL-WITHIN, P1, P2, P3.
// MUTATION 2d stimulus red NMH_MUTATE=P1-1D: the next word is polled on TSRE instead of TBE before it is written (prog_tx_n371_1d.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1d. Also NOT GRADED: P1-CTRL-WITHIN, P1, P2, P3.
// MUTATION 2e stimulus red NMH_MUTATE=P1-1E: word 2's SERDAT write is left out while its transfer is still polled (prog_tx_n371_1e.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1e. Also NOT GRADED: 1d, P1-CTRL-WITHIN, 2d, P1, P2, P3.
// MUTATION P2N0-R1 stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, P1N32396-R1, 2a, P2N32396-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION P2N0-R2 stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, P1N32396-R2, 2b, P2N32396-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION P2N0-DONE stimulus red NMH_MUTATE=P1-SINGLE-STALL: the N=0 and N=32396 programs follow the transmission with a bounded poll for a received character that never comes, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_tx_n0_stall.bin, prog_tx_n32396_stall.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N0-DONE, P1N32396-DONE, P2N32396-DONE. Also NOT GRADED: P1N0-IDLE, P1N32396-IDLE, P1-CTRL-N0, P1-CTRL-N32396, P2N0-IDLE, P2N32396-IDLE, P2-CTRL-PAIR, P1, P2. P3 prints FAIL, as at baseline.
// MUTATION P2N0-IDLE stimulus red NMH_MUTATE=P1-SINGLE-BRK: the N=0 and N=32396 programs force UARTBRK low then high between the lead-in and MARK=0 (prog_tx_n0_brk.bin, prog_tx_n32396_brk.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N0-IDLE, P1N32396-IDLE, P2N32396-IDLE. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P2-CTRL-PAIR, P1, P2. P3 prints FAIL, as at baseline.
// MUTATION P2N32396-R1 stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, P1N32396-R1, 2a, P2N0-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION P2N32396-R2 stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, P1N32396-R2, 2b, P2N0-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION P2N32396-DONE stimulus red NMH_MUTATE=P1-SINGLE-STALL: the N=0 and N=32396 programs follow the transmission with a bounded poll for a received character that never comes, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_tx_n0_stall.bin, prog_tx_n32396_stall.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N0-DONE, P1N32396-DONE, P2N0-DONE. Also NOT GRADED: P1N0-IDLE, P1N32396-IDLE, P1-CTRL-N0, P1-CTRL-N32396, P2N0-IDLE, P2N32396-IDLE, P2-CTRL-PAIR, P1, P2. P3 prints FAIL, as at baseline.
// MUTATION P2N32396-IDLE stimulus red NMH_MUTATE=P1-SINGLE-BRK: the N=0 and N=32396 programs force UARTBRK low then high between the lead-in and MARK=0 (prog_tx_n0_brk.bin, prog_tx_n32396_brk.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P1N0-IDLE, P1N32396-IDLE, P2N0-IDLE. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P2-CTRL-PAIR, P1, P2. P3 prints FAIL, as at baseline.
// MUTATION P2-CTRL-PAIR stimulus red NMH_MUTATE=P1-NPLUS1: SERPER is written with N+1 (1, 372, 32397) by the three _plus1 programs, on both standards, and the runs are graded against N; TG68K FAIL, fx68k FAIL. Also printing FAIL: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN. P1, P2, P3 print FAIL, as at baseline.
// MUTATION P2 stimulus red NMH_MUTATE=P1-NPLUS1: SERPER is written with N+1 (1, 372, 32397) by the three _plus1 programs, on both standards, and the runs are graded against N. P2 is red at baseline on both cores and stays FAIL, so it cannot be turned red; its comparator (check_uniform and the pairwise U comparison on the PAL runs) runs on live inputs in P2-CTRL-PAIR (the same pairwise U comparison on the N=0 and N=32396 pair), which print PASS at baseline and FAIL under this selector; TG68K FAIL, fx68k FAIL.
// MUTATION P3 stimulus red NMH_MUTATE=P1-NPLUS1: SERPER is written with N+1 (1, 372, 32397) by the three _plus1 programs, on both standards, and the runs are graded against N. P3 is red at baseline on both cores and stays FAIL, so it cannot be turned red; its comparator (check_grid on the N=371 NTSC run) runs on live inputs in P1-CTRL-N0 and P1-CTRL-N32396 (the same check_grid), which print PASS at baseline and FAIL under this selector; TG68K FAIL, fx68k FAIL.
// MUTATION 4a-NTSC-FAST1 stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, P1N32396-R1, 2a, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 4b-NTSC-FAST1 stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, P1N32396-R2, 2b, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST0, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 4c-NTSC-FAST1 stimulus red NMH_MUTATE=P4-4C: the FAST=1 programs write ADKCON <- $0100 (bit 15 clear clears the bit) and the FAST=0 programs write ADKCON <- $8100 (sets it); TG68K FAIL, fx68k FAIL. Also printing FAIL: 4c-NTSC-FAST0, 4c-PAL-FAST1, 4c-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4f-NTSC-FAST1 stimulus red NMH_MUTATE=P4-4F: after the fill, word 100 of the buffer is overwritten with $5555 (prog_fast1_4f.bin, prog_fast0_4f.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 4f-NTSC-FAST0, 4f-PAL-FAST1, 4f-PAL-FAST0. Also NOT GRADED: 4e-NTSC-FAST1, 4d-NTSC-FAST1, 4e-NTSC-FAST0, 4d-NTSC-FAST0, 4e-PAL-FAST1, 4d-PAL-FAST1, 4e-PAL-FAST0, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4e-NTSC-FAST1 stimulus red NMH_MUTATE=P4-4E: all four P4 programs use LENGTH=128 instead of 256; TG68K FAIL, fx68k FAIL. Also printing FAIL: 4e-NTSC-FAST0, 4e-PAL-FAST1, 4e-PAL-FAST0. Also NOT GRADED: 4d-NTSC-FAST1, 4d-NTSC-FAST0, 4d-PAL-FAST1, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline. Also NMH_MUTATE=P4-4E-ONCE (the program runs the transfer a second time over the same buffer, so every buffer word is fetched twice): 4e-NTSC-FAST1 FAIL through its exactly-once clause with every index seen and in rising order, 4d-NTSC-FAST1 PASS; TG68K FAIL, fx68k FAIL.
// MUTATION 4d-NTSC-FAST1 stimulus red NMH_MUTATE=P4-4D: in all four P4 programs DSKEN (DMACON bit 4) is cleared partway through the transfer, DSKBYTR is sampled while it is cleared, then DMAEN+DSKEN are set again; TG68K FAIL, fx68k FAIL. Also printing FAIL: 4d-NTSC-FAST0, 4d-PAL-FAST1, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline. The DISKWRITE clause is withdrawn (RECORDED) and has no contrast.
// MUTATION P4-DONE-NTSC-FAST1 stimulus red NMH_MUTATE=P4-STALL: the P4 programs poll for DSKBLK with a bound of 50 iterations, far below the length of the transfer, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_fast1_stall.bin, prog_fast0_stall.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P4-DONE-NTSC-FAST0, P4-DONE-PAL-FAST1, P4-DONE-PAL-FAST0. Also NOT GRADED: 4c-NTSC-FAST1, 4f-NTSC-FAST1, 4e-NTSC-FAST1, 4d-NTSC-FAST1, 4c-NTSC-FAST0, 4f-NTSC-FAST0, 4e-NTSC-FAST0, 4d-NTSC-FAST0, 4c-PAL-FAST1, 4f-PAL-FAST1, 4e-PAL-FAST1, 4d-PAL-FAST1, 4c-PAL-FAST0, 4f-PAL-FAST0, 4e-PAL-FAST0, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4a-NTSC-FAST0 stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, P1N32396-R1, 2a, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST1, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 4b-NTSC-FAST0 stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, P1N32396-R2, 2b, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST1, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 4c-NTSC-FAST0 stimulus red NMH_MUTATE=P4-4C: the FAST=1 programs write ADKCON <- $0100 (bit 15 clear clears the bit) and the FAST=0 programs write ADKCON <- $8100 (sets it); TG68K FAIL, fx68k FAIL. Also printing FAIL: 4c-NTSC-FAST1, 4c-PAL-FAST1, 4c-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4f-NTSC-FAST0 stimulus red NMH_MUTATE=P4-4F: after the fill, word 100 of the buffer is overwritten with $5555 (prog_fast1_4f.bin, prog_fast0_4f.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 4f-NTSC-FAST1, 4f-PAL-FAST1, 4f-PAL-FAST0. Also NOT GRADED: 4e-NTSC-FAST1, 4d-NTSC-FAST1, 4e-NTSC-FAST0, 4d-NTSC-FAST0, 4e-PAL-FAST1, 4d-PAL-FAST1, 4e-PAL-FAST0, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4e-NTSC-FAST0 stimulus red NMH_MUTATE=P4-4E: all four P4 programs use LENGTH=128 instead of 256; TG68K FAIL, fx68k FAIL. Also printing FAIL: 4e-NTSC-FAST1, 4e-PAL-FAST1, 4e-PAL-FAST0. Also NOT GRADED: 4d-NTSC-FAST1, 4d-NTSC-FAST0, 4d-PAL-FAST1, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline. Also NMH_MUTATE=P4-4E-ONCE (the program runs the transfer a second time over the same buffer, so every buffer word is fetched twice): 4e-NTSC-FAST0 FAIL through its exactly-once clause with every index seen and in rising order, 4d-NTSC-FAST0 PASS; TG68K FAIL, fx68k FAIL.
// MUTATION 4d-NTSC-FAST0 stimulus red NMH_MUTATE=P4-4D: in all four P4 programs DSKEN (DMACON bit 4) is cleared partway through the transfer, DSKBYTR is sampled while it is cleared, then DMAEN+DSKEN are set again; TG68K FAIL, fx68k FAIL. Also printing FAIL: 4d-NTSC-FAST1, 4d-PAL-FAST1, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline. The DISKWRITE clause is withdrawn (RECORDED) and has no contrast.
// MUTATION P4-DONE-NTSC-FAST0 stimulus red NMH_MUTATE=P4-STALL: the P4 programs poll for DSKBLK with a bound of 50 iterations, far below the length of the transfer, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_fast1_stall.bin, prog_fast0_stall.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P4-DONE-NTSC-FAST1, P4-DONE-PAL-FAST1, P4-DONE-PAL-FAST0. Also NOT GRADED: 4c-NTSC-FAST1, 4f-NTSC-FAST1, 4e-NTSC-FAST1, 4d-NTSC-FAST1, 4c-NTSC-FAST0, 4f-NTSC-FAST0, 4e-NTSC-FAST0, 4d-NTSC-FAST0, 4c-PAL-FAST1, 4f-PAL-FAST1, 4e-PAL-FAST1, 4d-PAL-FAST1, 4c-PAL-FAST0, 4f-PAL-FAST0, 4e-PAL-FAST0, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4a-PAL-FAST1 stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, P1N32396-R1, 2a, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST0, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 4b-PAL-FAST1 stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, P1N32396-R2, 2b, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST0, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 4c-PAL-FAST1 stimulus red NMH_MUTATE=P4-4C: the FAST=1 programs write ADKCON <- $0100 (bit 15 clear clears the bit) and the FAST=0 programs write ADKCON <- $8100 (sets it); TG68K FAIL, fx68k FAIL. Also printing FAIL: 4c-NTSC-FAST1, 4c-NTSC-FAST0, 4c-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4f-PAL-FAST1 stimulus red NMH_MUTATE=P4-4F: after the fill, word 100 of the buffer is overwritten with $5555 (prog_fast1_4f.bin, prog_fast0_4f.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 4f-NTSC-FAST1, 4f-NTSC-FAST0, 4f-PAL-FAST0. Also NOT GRADED: 4e-NTSC-FAST1, 4d-NTSC-FAST1, 4e-NTSC-FAST0, 4d-NTSC-FAST0, 4e-PAL-FAST1, 4d-PAL-FAST1, 4e-PAL-FAST0, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4e-PAL-FAST1 stimulus red NMH_MUTATE=P4-4E: all four P4 programs use LENGTH=128 instead of 256; TG68K FAIL, fx68k FAIL. Also printing FAIL: 4e-NTSC-FAST1, 4e-NTSC-FAST0, 4e-PAL-FAST0. Also NOT GRADED: 4d-NTSC-FAST1, 4d-NTSC-FAST0, 4d-PAL-FAST1, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline. Also NMH_MUTATE=P4-4E-ONCE (the program runs the transfer a second time over the same buffer, so every buffer word is fetched twice): 4e-PAL-FAST1 FAIL through its exactly-once clause with every index seen and in rising order, 4d-PAL-FAST1 PASS; TG68K FAIL, fx68k FAIL.
// MUTATION 4d-PAL-FAST1 stimulus red NMH_MUTATE=P4-4D: in all four P4 programs DSKEN (DMACON bit 4) is cleared partway through the transfer, DSKBYTR is sampled while it is cleared, then DMAEN+DSKEN are set again; TG68K FAIL, fx68k FAIL. Also printing FAIL: 4d-NTSC-FAST1, 4d-NTSC-FAST0, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline. The DISKWRITE clause is withdrawn (RECORDED) and has no contrast.
// MUTATION P4-DONE-PAL-FAST1 stimulus red NMH_MUTATE=P4-STALL: the P4 programs poll for DSKBLK with a bound of 50 iterations, far below the length of the transfer, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_fast1_stall.bin, prog_fast0_stall.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P4-DONE-NTSC-FAST1, P4-DONE-NTSC-FAST0, P4-DONE-PAL-FAST0. Also NOT GRADED: 4c-NTSC-FAST1, 4f-NTSC-FAST1, 4e-NTSC-FAST1, 4d-NTSC-FAST1, 4c-NTSC-FAST0, 4f-NTSC-FAST0, 4e-NTSC-FAST0, 4d-NTSC-FAST0, 4c-PAL-FAST1, 4f-PAL-FAST1, 4e-PAL-FAST1, 4d-PAL-FAST1, 4c-PAL-FAST0, 4f-PAL-FAST0, 4e-PAL-FAST0, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4a-PAL-FAST0 stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, P1N32396-R1, 2a, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 5a-R1-A, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 4b-PAL-FAST0 stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, P1N32396-R2, 2b, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST1, 5a-R2-A, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 4c-PAL-FAST0 stimulus red NMH_MUTATE=P4-4C: the FAST=1 programs write ADKCON <- $0100 (bit 15 clear clears the bit) and the FAST=0 programs write ADKCON <- $8100 (sets it); TG68K FAIL, fx68k FAIL. Also printing FAIL: 4c-NTSC-FAST1, 4c-NTSC-FAST0, 4c-PAL-FAST1. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4f-PAL-FAST0 stimulus red NMH_MUTATE=P4-4F: after the fill, word 100 of the buffer is overwritten with $5555 (prog_fast1_4f.bin, prog_fast0_4f.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 4f-NTSC-FAST1, 4f-NTSC-FAST0, 4f-PAL-FAST1. Also NOT GRADED: 4e-NTSC-FAST1, 4d-NTSC-FAST1, 4e-NTSC-FAST0, 4d-NTSC-FAST0, 4e-PAL-FAST1, 4d-PAL-FAST1, 4e-PAL-FAST0, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 4e-PAL-FAST0 stimulus red NMH_MUTATE=P4-4E: all four P4 programs use LENGTH=128 instead of 256; TG68K FAIL, fx68k FAIL. Also printing FAIL: 4e-NTSC-FAST1, 4e-NTSC-FAST0, 4e-PAL-FAST1. Also NOT GRADED: 4d-NTSC-FAST1, 4d-NTSC-FAST0, 4d-PAL-FAST1, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline. Also NMH_MUTATE=P4-4E-ONCE (the program runs the transfer a second time over the same buffer, so every buffer word is fetched twice): 4e-PAL-FAST0 FAIL through its exactly-once clause with every index seen and in rising order, 4d-PAL-FAST0 PASS; TG68K FAIL, fx68k FAIL.
// MUTATION 4d-PAL-FAST0 stimulus red NMH_MUTATE=P4-4D: in all four P4 programs DSKEN (DMACON bit 4) is cleared partway through the transfer, DSKBYTR is sampled while it is cleared, then DMAEN+DSKEN are set again; TG68K FAIL, fx68k FAIL. Also printing FAIL: 4d-NTSC-FAST1, 4d-NTSC-FAST0, 4d-PAL-FAST1. P1, P2, P3 print FAIL, as at baseline. The DISKWRITE clause is withdrawn (RECORDED) and has no contrast.
// MUTATION P4-DONE-PAL-FAST0 stimulus red NMH_MUTATE=P4-STALL: the P4 programs poll for DSKBLK with a bound of 50 iterations, far below the length of the transfer, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_fast1_stall.bin, prog_fast0_stall.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: P4-DONE-NTSC-FAST1, P4-DONE-NTSC-FAST0, P4-DONE-PAL-FAST1. Also NOT GRADED: 4c-NTSC-FAST1, 4f-NTSC-FAST1, 4e-NTSC-FAST1, 4d-NTSC-FAST1, 4c-NTSC-FAST0, 4f-NTSC-FAST0, 4e-NTSC-FAST0, 4d-NTSC-FAST0, 4c-PAL-FAST1, 4f-PAL-FAST1, 4e-PAL-FAST1, 4d-PAL-FAST1, 4c-PAL-FAST0, 4f-PAL-FAST0, 4e-PAL-FAST0, 4d-PAL-FAST0. P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5a-R1-A stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, P1N32396-R1, 2a, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-B. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 5a-R2-A stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, P1N32396-R2, 2b, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-B. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 5a-C8-A none impossible: unfalsifiable at this boundary -- no stimulus the pages describe changes the number of cycles per colour clock, so the measured C cannot differ from the 8.0 the driver assumes
// MUTATION 5a-DONE-A stimulus red NMH_MUTATE=P5-STALL-A: run A follows the eighth character with a bounded poll for a ninth that is never sent, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_rx_a_stall.bin); TG68K FAIL, fx68k FAIL. Also NOT GRADED: 5b, (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5a-R1-B stimulus red NMH_MUTATE=R1-READ: one CPU read of Chip word $000100 (word address $80, below $1000) with the ROM overlay cleared is added to every program: after the lead-in in the TX and P5 programs, after the transfer in the P4 programs (prog_*_r1.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 1a, P1N0-R1, P1N32396-R1, 2a, P2N0-R1, P2N32396-R1, 4a-NTSC-FAST1, 4a-NTSC-FAST0, 4a-PAL-FAST1, 4a-PAL-FAST0, 5a-R1-A. Also NOT GRADED: 1d, P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, 2d, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 5a-R2-B stimulus red NMH_MUTATE=STD-SWAP: every capture is made with the other video standard selected: the NTSC runs on PAL and the PAL runs on NTSC; TG68K FAIL, fx68k FAIL. Also printing FAIL: 1b, P1N0-R2, P1N32396-R2, 2b, P2N0-R2, P2N32396-R2, 4b-NTSC-FAST1, 4b-NTSC-FAST0, 4b-PAL-FAST1, 4b-PAL-FAST0, 5a-R2-A. Also NOT GRADED: P1-CTRL-N0, P1-CTRL-N32396, P1-CTRL-WITHIN, P2-CTRL-PAIR, 5f, (a), (b), (c), P1, P2, P3.
// MUTATION 5a-C8-B none impossible: unfalsifiable at this boundary -- no stimulus the pages describe changes the number of cycles per colour clock, so the measured C cannot differ from the 8.0 the driver assumes
// MUTATION 5a-DONE-B stimulus red NMH_MUTATE=P5-STALL-B: run B follows the eighth character with a bounded poll for a ninth that is never sent, so the poll runs to its own bound (POLL_TIMEOUT is set by the poll itself) and the program still reaches DONE (prog_rx_b_stall.bin); TG68K FAIL, fx68k FAIL. Also NOT GRADED: 5f, (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5b stimulus red NMH_MUTATE=P5-5B: the ADKCON write that sets UARTBRK is left out; MARK=BRK is still written; TG68K FAIL, fx68k FAIL. Also NOT GRADED: (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5c-A stimulus red NMH_MUTATE=P5-5C: the driver sends a ninth character in run A and in run B, and the read of character 7 is delayed until after it (prog_rx_a_5c.bin, prog_rx_b_5c.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 5c-B. Also NOT GRADED: 5f, (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5c-B stimulus red NMH_MUTATE=P5-5C: the driver sends a ninth character in run A and in run B, and the read of character 7 is delayed until after it (prog_rx_a_5c.bin, prog_rx_b_5c.bin); TG68K FAIL, fx68k FAIL. Also printing FAIL: 5c-A. Also NOT GRADED: 5f, (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5d-A stimulus red NMH_MUTATE=P5-5D: the INTREQ clear is left out in run A and in run B, so the next poll sees RBF=1 at once; TG68K FAIL, fx68k FAIL. Also printing FAIL: 5d-B. Also NOT GRADED: 5e-A, 5e-B, 5f, (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5d-B stimulus red NMH_MUTATE=P5-5D: the INTREQ clear is left out in run A and in run B, so the next poll sees RBF=1 at once; TG68K FAIL, fx68k FAIL. Also printing FAIL: 5d-A. Also NOT GRADED: 5e-A, 5e-B, 5f, (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5e-A stimulus red NMH_MUTATE=P5-5E: every poll iteration is padded by T/4 or more in run A and in run B; TG68K FAIL, fx68k FAIL. Also printing FAIL: 5e-B. Also NOT GRADED: (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5e-B stimulus red NMH_MUTATE=P5-5E: every poll iteration is padded by T/4 or more in run A and in run B; TG68K FAIL, fx68k FAIL. Also printing FAIL: 5e-A. Also NOT GRADED: (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION 5f stimulus red NMH_MUTATE=P5-5F: run B itself is driven at a bit time of 2T; TG68K FAIL, fx68k FAIL. Also NOT GRADED: (a), (b), (c). P1, P2, P3 print FAIL, as at baseline.
// MUTATION (a) stimulus red NMH_MUTATE=P5-A-DATA: run A is driven with every byte complemented, at the same timing; TG68K FAIL, fx68k FAIL. P1, P2, P3 print FAIL, as at baseline. Also NMH_MUTATE=P5-A (run A itself driven at a bit time of 2T): (a) FAIL, (b) FAIL, (c) FAIL.
// MUTATION (b) stimulus red NMH_MUTATE=P5-B: run A loads prog_rx_a_altper.bin (SERPER <- $80B9, N=185, driver unchanged); TG68K FAIL, fx68k FAIL. Also printing FAIL: (a), (c). P1, P2, P3 print FAIL, as at baseline. Also NMH_MUTATE=P5-A (run A itself driven at a bit time of 2T): (b) FAIL, the 5a-5e lines of run A stay PASS. (b) is decided per character and passes only when all 16 are decided and pass: of the 16 characters 16 reach their threshold at baseline, 8 stay below it under P5-B and 2 under P5-A; 0 straddle it at baseline, 0 under P5-B and 0 under P5-A.
// MUTATION (c) stimulus red NMH_MUTATE=P5-C: run B loads prog_rx_a.bin (SERPER <- $8173 in run B too); TG68K FAIL, fx68k FAIL. P1, P2, P3 print FAIL, as at baseline. Also NMH_MUTATE=P5-A (run A itself driven at a bit time of 2T): (c) FAIL. (c) is decided per character and passes only when all 8 are decided and pass: of the 8 characters 8 lie inside the band at baseline; under P5-C 0 lie inside it, 8 outside and 0 straddle an edge; under P5-A 3 inside, 5 outside, 0 straddle; under P5-B 0 inside, 8 outside, 0 straddle.

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

    // A graded line with its own leading id. It is graded only when every
    // precondition named in `deps` was graded and passed; otherwise it prints
    // NOT GRADED (a RECORDED line), never PASS or FAIL. A dependency that was
    // never evaluated counts as not met.
    std::string unmet_of(const std::vector<std::string>& deps) const {
        std::string unmet;
        for (const auto& d : deps) {
            auto it = verdict.find(d);
            if (it == verdict.end() || !it->second) unmet += (unmet.empty() ? "" : ", ") + d;
        }
        return unmet;
    }
    void line(const std::string& id, const std::vector<std::string>& deps, bool cond, const std::string& text) {
        const std::string unmet = unmet_of(deps);
        if (!unmet.empty()) {
            record("NOT GRADED " + id + ": precondition(s) not met: " + unmet);
            verdict[id] = false;
            return;
        }
        expect(cond, id + " " + text);
        verdict[id] = cond;
    }
    int passed = 0, failed = 0;
    std::map<std::string, bool> verdict; // id -> graded and PASS
};

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

constexpr uint32_t kMarkAddr  = 0x0C1000;
constexpr uint16_t kMarkDone  = 0x00F5;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint32_t kPollTimeoutAddr = 0x0C0020; // G4: POLLBIT ORs 1 in here if any poll reaches its bound

// NMH_MUTATE=STD-SWAP: every capture of the package is made on the other
// video standard than the one its checks expect.
bool g_std_swap = false;

// chipset_config bit1 selects the video standard (1=NTSC, 0=PAL); R2 (below)
// confirms which one a given capture actually shows. Bit3 stays set (ECS),
// matching the harness default.
nmh::Config cfg_for(bool ntsc) {
    nmh::Config c;
    c.chipset_config = (ntsc != g_std_swap) ? 0x0A : 0x08;
    return c;
}

// ---- R1: the colour clock, from the four refresh accesses per line -------
//
// Apparatus assumption the R1 anchors rest on: RamAccess carries no
// attribution, and no page fixes the address of a memory-refresh access, so
// a refresh access is taken to be any Chip-port read at a word address below
// 0x1000, and the accesses of one line are the reads of that kind that lie
// no more than 100 cycles apart.

struct RefreshGroup { uint64_t r1, r2, r3, r4; };

// The Chip-port log with its held continuations removed: a bus cycle
// stretched by wait states logs one identical entry per 7 MHz period it
// spans (nanomig_harness.h), and every count below is of accesses, not of
// samples. The period is the smallest spacing of two consecutive samples.
std::vector<nmh::RamAccess> logical_log(const std::vector<nmh::RamAccess>& log) {
    uint64_t period = 0;
    for (size_t i = 1; i < log.size(); ++i) {
        uint64_t d = log[i].cycle - log[i - 1].cycle;
        if (d > 0 && (period == 0 || d < period)) period = d;
    }
    return period ? nmh::coalesce_adjacent(log, period) : log;
}

// The candidate groups of low-address reads, split where two reads lie more
// than 100 cycles apart. The last group is dropped when the capture ended
// within 100 cycles of its last read: the gap rule has not closed it, so it
// may lack reads the capture never saw, and it does not lie wholly inside the
// capture.
std::vector<std::vector<uint64_t>> candidate_groups(const std::vector<nmh::RamAccess>& logical, uint64_t end_cycle) {
    std::vector<uint64_t> low;
    for (const auto& a : logical) if (!a.is_write && a.word_addr < 0x1000) low.push_back(a.cycle);
    std::vector<std::vector<uint64_t>> groups;
    if (low.empty()) return groups;
    groups.push_back({low[0]});
    for (size_t i = 1; i < low.size(); ++i) {
        if (low[i] - low[i - 1] > 100) groups.push_back({});
        groups.back().push_back(low[i]);
    }
    if (end_cycle - groups.back().back() <= 100) groups.pop_back();
    return groups;
}

// What a capture shows of R1. C is (r4-r1)/6 of the first four-read group.
// `closed` counts every candidate group wholly inside the capture,
// `malformed` those that are not four reads each 2C after the one before.
// (b), NTSC: the first read of a line and that of the next line are 227*C
// or 228*C apart (p.23: "Every other line is a long line (228 color clocks)
// ..., with the others being 227 color clocks long"); `lines` spacings,
// `line_bad` outside that range. Measured, not graded: the first read of
// line L and that of line L+2 against 455*C (`pairs`, `pair_bad`).
struct R1Scan {
    std::vector<RefreshGroup> groups; // the four-read groups
    double C = 0.0;
    int closed = 0, malformed = 0;
    int lines = 0, line_bad = 0;
    int pairs = 0, pair_bad = 0;
    double d1min = 1e18, d1max = 0, d2min = 1e18, d2max = 0; // spacing of first reads: line to line, line L to L+2
};

R1Scan scan_refresh(const std::vector<nmh::RamAccess>& log, uint64_t end_cycle) {
    R1Scan s;
    auto groups = candidate_groups(logical_log(log), end_cycle);
    for (const auto& g : groups)
        if (g.size() == 4) s.groups.push_back({g[0], g[1], g[2], g[3]});
    if (!s.groups.empty()) s.C = double(s.groups[0].r4 - s.groups[0].r1) / 6.0;
    s.closed = int(groups.size());
    if (s.C <= 0) return s;
    for (const auto& g : groups) {
        bool shape_ok = g.size() == 4;
        for (size_t i = 1; shape_ok && i < g.size(); ++i)
            if (std::abs(double(g[i] - g[i - 1]) - 2.0 * s.C) > 0.01) shape_ok = false;
        if (!shape_ok) ++s.malformed;
    }
    for (size_t i = 0; i + 1 < groups.size(); ++i) {
        double d = double(groups[i + 1][0] - groups[i][0]);
        s.d1min = std::min(s.d1min, d); s.d1max = std::max(s.d1max, d);
        ++s.lines;
        if (d < 227.0 * s.C - 0.01 || d > 228.0 * s.C + 0.01) ++s.line_bad;
    }
    for (size_t i = 0; i + 2 < groups.size(); ++i) {
        double d = double(groups[i + 2][0] - groups[i][0]);
        s.d2min = std::min(s.d2min, d); s.d2max = std::max(s.d2max, d);
        ++s.pairs;
        if (std::abs(double(groups[i + 2][0] - groups[i][0]) - 455.0 * s.C) > 0.01) ++s.pair_bad;
    }
    return s;
}

// Check (a): every group's three internal gaps equal 2*C, in every line.
bool check_r1_anchor(const std::vector<RefreshGroup>& groups, double candidate_C) {
    if (groups.empty() || candidate_C <= 0) return false;
    double expected = 2.0 * candidate_C;
    for (const auto& g : groups) {
        double g1 = double(g.r2 - g.r1), g2 = double(g.r3 - g.r2), g3 = double(g.r4 - g.r3);
        if (std::abs(g1 - expected) > 0.01 || std::abs(g2 - expected) > 0.01 || std::abs(g3 - expected) > 0.01)
            return false;
    }
    return true;
}

// The R1 anchor of one capture: (a) on the four-read groups, no malformed
// candidate group among those wholly inside the capture, and on NTSC (b).
bool r1_ok(const R1Scan& s, bool ntsc) {
    bool ok = !s.groups.empty() && check_r1_anchor(s.groups, s.C) && s.closed > 0 && s.malformed == 0;
    if (ntsc) ok = ok && s.lines > 0 && s.line_bad == 0;
    return ok;
}

std::string r1_note(const R1Scan& s, bool ntsc) {
    return "R1 population groups=" + std::to_string(s.closed) + " malformed=" + std::to_string(s.malformed) +
           (ntsc ? " (b) line spacings=" + std::to_string(s.lines) + " outside 227C..228C=" + std::to_string(s.line_bad) + "; (b') measured, not graded (NM-HRM-010): L to L+2 pairs=" + std::to_string(s.pairs) + " not 455C=" + std::to_string(s.pair_bad) + ", line spacing " + std::to_string(s.d1min) + ".." + std::to_string(s.d1max) + " L..L+2 spacing " + std::to_string(s.d2min) + ".." + std::to_string(s.d2max) : std::string(" (b) not applied on PAL"));
}

// ---- shared TX capture (P1/P2/P3) -----------------------------------------

struct TxRun {
    bool loaded = false, released = false;
    std::vector<std::pair<uint64_t, uint16_t>> marks; // (cycle, value), chronological
    std::vector<uint64_t> tx_changes;                 // cycle of every uart_tx transition
    std::vector<RefreshGroup> refresh;
    double C = 0.0;
    int field_lines = 0; // R2: refresh-groups in one representative field
    bool poll_timeout = false; // G4: a POLLBIT in this run reached its own bound
    R1Scan r1;
};

std::optional<uint64_t> mark_cycle(const TxRun& r, uint16_t value) {
    for (const auto& m : r.marks) if (m.second == value) return m.first;
    return std::nullopt;
}

uint64_t budget_for(int nval, int wcount) {
    return 20'000'000ull + 10ull * uint64_t(wcount) * uint64_t(nval + 1) * 8ull + 500'000ull;
}

TxRun run_tx(const std::string& bin, bool ntsc, uint64_t post_boot_budget) {
    TxRun r;
    nmh::NanomigHarness h(cfg_for(ntsc));
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;

    bool level = true; // TxD idle level is 1 (R4; precondition 1c/5b anchor this)
    bool done = false;
    const uint32_t mark_word = word_addr(kMarkAddr);
    size_t seen = 0;
    for (uint64_t i = 0; i < post_boot_budget && !done; ++i) {
        h.run_cycles(1);
        bool cur = h.ports()->uart_tx;
        if (cur != level) { r.tx_changes.push_back(h.cycles() - 1); level = cur; }
        const auto& log = h.ram_log();
        for (; seen < log.size(); ++seen) {
            if (log[seen].is_write && log[seen].word_addr == mark_word) {
                r.marks.push_back({log[seen].cycle, log[seen].data});
                if (log[seen].data == kMarkDone) done = true;
            }
        }
    }

    r.r1 = scan_refresh(h.ram_log(), h.cycles());
    r.refresh = r.r1.groups;
    r.C = r.r1.C;

    auto frames = nmh::find_frames(h.video());
    if (frames.size() >= 3) {
        int count = 0;
        for (const auto& g : r.refresh) if (g.r1 >= frames[1].begin && g.r1 < frames[1].end) ++count;
        r.field_lines = count;
    }
    auto to = h.peek_logical(kPollTimeoutAddr);
    r.poll_timeout = to.has_value() && *to != 0;
    return r;
}

// ---- P1/P2/P3 grid check ---------------------------------------------------

struct GridResult {
    bool ok = false;
    int matched = 0, total_expected = 0;
    size_t total_changes = 0;
    uint64_t t0 = 0;
};

// The observation resolution. TxD is sampled once per clk_sys cycle, so the
// time of a change is known to within one cycle, and the difference of two
// change times (every comparison below is relative to the first change, which
// removes the unknown phase) to within less than one cycle. This is the only
// uncertainty any comparison of a change time with the page's N+1-colour-clock
// spacing allows; no fraction of the colour clock enters.
constexpr double kObsRes = 1.0;

GridResult check_grid(const std::vector<uint64_t>& changes, double C, int N, int W) {
    GridResult g;
    g.total_expected = 10 * W;
    if (changes.empty() || C <= 0) return g;
    g.t0 = changes[0];
    double T = double(N + 1) * C;
    std::vector<bool> used(changes.size(), false);
    int matched = 0;
    for (int k = 0; k < 10 * W; ++k) {
        double target = double(g.t0) + double(k) * T;
        for (size_t i = 0; i < changes.size(); ++i) {
            if (used[i]) continue;
            if (std::abs(double(changes[i]) - target) < kObsRes) { used[i] = true; ++matched; break; }
        }
    }
    g.matched = matched;
    g.total_changes = changes.size();
    g.ok = (matched == 10 * W) && (changes.size() == size_t(10 * W));
    return g;
}

// Within-word grid (control): for a WMULTI run, the changes come in groups
// of 10 per word (start, 8 data bits, stop). The 9 gaps WITHIN each word
// (not the 10th, which crosses into the next word's own start bit) are
// graded in isolation from any inter-word drift.
bool check_within_word(const std::vector<uint64_t>& changes, double C, int N, int W) {
    if ((int)changes.size() != 10 * W || C <= 0) return false;
    double T = double(N + 1) * C;
    for (int w = 0; w < W; ++w)
        for (int i = 0; i < 9; ++i) {
            double gap = double(changes[w * 10 + i + 1]) - double(changes[w * 10 + i]);
            if (!(std::abs(gap - T) < kObsRes)) return false;
        }
    return true;
}

// Measured, not graded: how far each change of a run lies from t0+k*(N+1)*C,
// in cycles, when the run has exactly 10W changes (change k is then the one
// the page puts at k). Returns "max |deviation| = D at k=K; first change of
// each word: d0 d1 ..." or a note that the count differs.
std::string grid_deviation_note(const std::vector<uint64_t>& changes, double C, int N, int W) {
    if (C <= 0 || changes.size() != size_t(10 * W)) return "change count differs from 10W, no per-change deviation";
    double T = double(N + 1) * C, worst = 0; int worst_k = 0;
    std::string firsts;
    for (int k = 0; k < 10 * W; ++k) {
        double dev = double(changes[k]) - (double(changes[0]) + double(k) * T);
        if (std::abs(dev) > std::abs(worst)) { worst = dev; worst_k = k; }
        if (k % 10 == 0) firsts += " " + std::to_string(dev);
    }
    return "largest deviation " + std::to_string(worst) + " cycles at k=" + std::to_string(worst_k) +
           "; first change of each word:" + firsts;
}

// ---- P2's own checks: uniform grid within a run, and cross-run period ----

struct UniformResult { bool ok = false; double T_N = 0; size_t total_changes = 0; int matched = 0; };

UniformResult check_uniform(const std::vector<uint64_t>& changes, double C, int W) {
    UniformResult u;
    int total = 10 * W;
    if ((int)changes.size() < 2 || C <= 0) return u;
    u.total_changes = changes.size();
    u.T_N = double(changes.back() - changes.front()) / double(total - 1);
    uint64_t t0 = changes.front();
    std::vector<bool> used(changes.size(), false);
    int matched = 0;
    // Change k lies on the straight line through the first and the last
    // change; a change time is known to within kObsRes, so the line's own
    // interior points are known to within the same kObsRes.
    for (int k = 0; k < total; ++k) {
        double target = double(t0) + double(k) * u.T_N;
        for (size_t i = 0; i < changes.size(); ++i) {
            if (used[i]) continue;
            if (std::abs(double(changes[i]) - target) < kObsRes) { used[i] = true; ++matched; break; }
        }
    }
    u.matched = matched;
    u.ok = (matched == total) && (changes.size() == size_t(total));
    return u;
}

// ==== P5: nine-bit receive capture and analysis =============================

constexpr uint32_t kResultAddr = 0x0C0000; // shared with P4 (never run concurrently)
constexpr uint16_t kMarkBrk   = 0x00F1;
constexpr uint16_t kMarkUnbrk = 0x00F2;
constexpr uint16_t kMarkReady = 0x00F3;
constexpr uint32_t kProbeIAddr = 0x0C0012; // kept clear of RESULT..RESULT+$0E (ch8_defs.i)
const std::vector<uint8_t> kRxBytes = {0xA5, 0x5A, 0x81, 0x7E, 0x3C, 0xC3, 0x66, 0x99};

struct RxRun {
    bool loaded = false, released = false;
    std::vector<std::pair<uint64_t, uint16_t>> marks;
    std::vector<std::pair<uint64_t, uint16_t>> probe_i;
    std::vector<uint64_t> tx_changes;
    std::vector<std::optional<uint16_t>> results;
    std::vector<RefreshGroup> refresh;
    double C = 0.0;
    int field_lines = 0;
    uint64_t s0 = 0;
    bool scheduled = false;
    bool poll_timeout = false; // G4: any of the custom RBF poll loops reached its own bound
    R1Scan r1;
};

std::optional<uint64_t> mark_cycle_rx(const RxRun& r, uint16_t value) {
    for (const auto& m : r.marks) if (m.second == value) return m.first;
    return std::nullopt;
}

// RxD level at cycle `cyc`: idle (1) before s0 and after the last
// character; within a character, bit 0 is the start bit (0), bits 1-8 are
// byte_j's own bits 0-7 (LSB first, harmless either way since every byte_j
// is a bit-palindrome), bit 9 (the ninth bit) is 1 unless `ninth_zero`
// selects the recorded, not-graded variant (SPECIFICATION.md P5), and bit
// 10 (stop) and anything past bit 10 (the idle gap before the next
// character) are always 1.
bool rxd_level_at(uint64_t cyc, uint64_t s0, double T, double step, const std::vector<uint8_t>& bytes,
                   bool ninth_zero = false) {
    if (cyc < s0) return true;
    int j = int(double(cyc - s0) / step);
    if (j < 0 || j >= (int)bytes.size()) return true;
    double sj = double(s0) + double(j) * step;
    double pos = double(cyc) - sj;
    if (pos < 0) return true;
    int bit = int(pos / T);
    if (bit == 0) return false;
    if (bit >= 1 && bit <= 8) return (bytes[j] >> (bit - 1)) & 1;
    if (bit == 9 && ninth_zero) return false;
    return true;
}

// `t_scale` multiplies the driver's own bit time relative to T (1.0 =
// correct N=371 rate; 2.0 = the (a)/5f contrasting cases' wrong rate).
// `ntsc` is true for every graded P5 run (P5 is NTSC only); the MUTATION
// 5a-STANDARD contrast passes false to capture the same binary on PAL.
// `ninth_zero` selects the recorded run-A variant (SPECIFICATION.md P5)
// whose every ninth bit is driven 0 instead of 1. `ninth_char` appends a
// ninth driven character (the 5c contrast); `data_swap` drives every byte
// complemented, at the same timing (the (a) contrast).
RxRun run_rx(const std::string& bin, uint64_t budget, double T, double t_scale, bool ntsc = true,
             bool ninth_zero = false, bool ninth_char = false, bool data_swap = false) {
    RxRun r;
    nmh::NanomigHarness h(cfg_for(ntsc));
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;

    const double Tdrv = T * t_scale;
    const double step = 16.125 * Tdrv; // 16T + T/8
    std::vector<uint8_t> drv_bytes = kRxBytes;
    if (data_swap) for (auto& b : drv_bytes) b = uint8_t(~b);
    if (ninth_char) drv_bytes.push_back(0xE7);
    const uint64_t lead_margin = uint64_t(3.0 * T); // >= 2T (G2's own margin note)
    bool tx_level = true;
    bool done = false;
    const uint32_t mark_word = word_addr(kMarkAddr);
    const uint32_t probe_i_word = word_addr(kProbeIAddr);
    size_t seen = 0;
    for (uint64_t i = 0; i < budget && !done; ++i) {
        bool level = true;
        if (r.scheduled) level = rxd_level_at(h.cycles(), r.s0, Tdrv, step, drv_bytes, ninth_zero);
        h.ports()->uart_rx = level;
        h.run_cycles(1);
        bool cur_tx = h.ports()->uart_tx;
        if (cur_tx != tx_level) { r.tx_changes.push_back(h.cycles() - 1); tx_level = cur_tx; }
        const auto& log = h.ram_log();
        for (; seen < log.size(); ++seen) {
            const auto& a = log[seen];
            if (a.is_write && a.word_addr == mark_word) {
                r.marks.push_back({a.cycle, a.data});
                if (a.data == kMarkDone) done = true;
                if (a.data == kMarkReady && !r.scheduled) { r.s0 = a.cycle + lead_margin; r.scheduled = true; }
            }
            if (a.is_write && a.word_addr == probe_i_word) r.probe_i.push_back({a.cycle, a.data});
        }
    }
    for (int j = 0; j < 8; ++j) r.results.push_back(h.peek_logical(kResultAddr + 2 * j));
    r.r1 = scan_refresh(h.ram_log(), h.cycles());
    r.refresh = r.r1.groups;
    r.C = r.r1.C;
    auto frames = nmh::find_frames(h.video());
    if (frames.size() >= 3) {
        int count = 0;
        for (const auto& g : r.refresh) if (g.r1 >= frames[1].begin && g.r1 < frames[1].end) ++count;
        r.field_lines = count;
    }
    auto to = h.peek_logical(kPollTimeoutAddr);
    r.poll_timeout = to.has_value() && *to != 0;
    return r;
}

bool level_before(const std::vector<uint64_t>& changes, uint64_t cycle) {
    bool level = true;
    for (auto c : changes) { if (c > cycle) break; level = !level; }
    return level;
}

struct Bracket { uint64_t t0 = 0, t1 = 0, S = 0; bool ok = false; };

// G5's own bracket for character j: the probe_i samples strictly after
// `lo` (the previous MARK) and up to and including `hi` (this character's
// own MARK) are exactly that character's poll; the loop always stores
// INTREQR before testing it, so the last sample is the first one showing
// RBF=1 and the one before it is the last showing RBF=0 (t1, t0), and S is
// the widest gap between consecutive stores in that same poll.
Bracket g5_bracket(const std::vector<std::pair<uint64_t, uint16_t>>& probe_i, uint64_t lo, uint64_t hi) {
    Bracket b;
    std::vector<uint64_t> cyc;
    for (const auto& p : probe_i) if (p.first > lo && p.first <= hi) cyc.push_back(p.first);
    if (cyc.size() < 2) return b;
    std::sort(cyc.begin(), cyc.end());
    for (size_t i = 1; i < cyc.size(); ++i) b.S = std::max(b.S, cyc[i] - cyc[i - 1]);
    b.t1 = cyc.back();
    b.t0 = cyc[cyc.size() - 2];
    b.ok = true;
    return b;
}

double bracket_midpoint(const Bracket& b) {
    return (double(b.t0) - double(b.S) + double(b.t1)) / 2.0;
}

std::pair<uint64_t, uint64_t> rx_window(const RxRun& r, int j) {
    uint64_t lo = (j == 0) ? mark_cycle_rx(r, kMarkReady).value_or(0) : mark_cycle_rx(r, uint16_t(j)).value_or(0);
    uint64_t hi = mark_cycle_rx(r, uint16_t(j + 1)).value_or(0);
    return {lo, hi};
}

// Every marker one run of the P5 program writes, in order.
bool rx_sequence_present(const RxRun& r) {
    std::vector<uint16_t> want = {kMarkBrk, kMarkUnbrk, kMarkReady, 1, 2, 3, 4, 5, 6, 7, 8, kMarkDone};
    uint64_t prev = 0;
    for (uint16_t v : want) {
        auto c = mark_cycle_rx(r, v);
        if (!c || *c < prev) return false;
        prev = *c;
    }
    return true;
}

void run_p5(const std::string& dir, const std::string& mv, Checks& checks) {
    auto bin_a = dir + (mv == "R1-READ" ? "/prog_rx_a_r1.bin" :
                         mv == "P5-5B" ? "/prog_rx_a_5b.bin" :
                         mv == "P5-5C" ? "/prog_rx_a_5c.bin" :
                         mv == "P5-5D" ? "/prog_rx_a_5d.bin" :
                         mv == "P5-5E" ? "/prog_rx_a_5e.bin" :
                         mv == "P5-B"  ? "/prog_rx_a_altper.bin" :
                         mv == "P5-STALL-A" ? "/prog_rx_a_stall.bin" : "/prog_rx_a.bin");
    auto bin_b = dir + (mv == "R1-READ" ? "/prog_rx_b_r1.bin" :
                         mv == "P5-C" ? "/prog_rx_a.bin" :
                         mv == "P5-5C" ? "/prog_rx_b_5c.bin" :
                         mv == "P5-5D" ? "/prog_rx_b_5d.bin" :
                         mv == "P5-5E" ? "/prog_rx_b_5e.bin" :
                         mv == "P5-STALL-B" ? "/prog_rx_b_stall.bin" : "/prog_rx_b.bin");
    constexpr uint64_t kP5Budget = 25'000'000;
    const double T = 372.0 * 8.0; // N=371, C confirmed 8.0 by 5a below and every other proposition's own R1
    const double step = 16.125 * T;

    // NMH_MUTATE=P5-A drives run A itself at a bit time of 2T; NMH_MUTATE=
    // P5-5F drives run B itself at 2T; NMH_MUTATE=P5-C already swapped
    // bin_b to prog_rx_a.bin above. Every check below reads a/b as they
    // come out of these two calls, so a mutation's contrasting capture
    // reaches every check that uses it, through the check's own unchanged
    // comparator.
    // NMH_MUTATE=P5-A-DATA drives run A with every byte complemented at the
    // same timing; NMH_MUTATE=P5-5C drives a ninth character in both runs.
    RxRun a = run_rx(bin_a, kP5Budget, T, mv == "P5-A" ? 2.0 : 1.0, true, false, mv == "P5-5C", mv == "P5-A-DATA");
    RxRun b = run_rx(bin_b, kP5Budget, T, mv == "P5-5F" ? 2.0 : 1.0, true, false, mv == "P5-5C");

    checks.note("P5 capture health: a loaded=" + std::to_string(a.loaded) + " released=" + std::to_string(a.released) +
                " marks=" + std::to_string(a.marks.size()) + " probe_i=" + std::to_string(a.probe_i.size()) +
                " s0=" + std::to_string(a.s0) + " C=" + std::to_string(a.C) + " field_lines=" + std::to_string(a.field_lines) +
                " poll_timeout=" + std::to_string(a.poll_timeout));
    checks.note("P5 capture health: b loaded=" + std::to_string(b.loaded) + " released=" + std::to_string(b.released) +
                " marks=" + std::to_string(b.marks.size()) + " probe_i=" + std::to_string(b.probe_i.size()) +
                " s0=" + std::to_string(b.s0) + " C=" + std::to_string(b.C) + " field_lines=" + std::to_string(b.field_lines) +
                " poll_timeout=" + std::to_string(b.poll_timeout));

    // ---- 5a: R1 anchor, R2 shows NTSC, the driver's own C=8 assumption,
    // and completion (G4), each on run A and on run B. ----
    auto grade_5a = [&](const RxRun& r, const std::string& run) {
        checks.note("5a run " + run + ": " + r1_note(r.r1, true));
        bool r1ok = r1_ok(r.r1, true);
        checks.line("5a-R1-" + run, {}, r1ok,
            "[p194,p195] R1 anchor, run " + run + ": every candidate refresh group in the run is 2C, 2C "
            "and 2C apart (Figure 6-9's own shape), none discarded before grading");
        bool ntsc_ok = r.field_lines == 262 || r.field_lines == 263;
        checks.line("5a-R2-" + run, {}, ntsc_ok,
            "[p24] R2 shows NTSC, run " + run + " (measured field_lines=" + std::to_string(r.field_lines) + ")");
        // The P5 driver's own T=372*8.0 assumes C=8.0 (above); 5a's own
        // run must confirm that, not just recite it (see SPECIFICATION.md).
        bool c_ok = std::abs(r.C - 8.0) < 0.001;
        checks.line("5a-C8-" + run, {}, c_ok,
            "[p194,p195] the run's own R1 anchor C equals the driver's own C=8.0 assumption, "
            "run " + run + " (measured C=" + std::to_string(r.C) + ")");
        checks.line("5a-DONE-" + run, {}, rx_sequence_present(r) && !r.poll_timeout,
            "[G3,G4] every marker (BRK, UNBRK, READY, 1-8, DONE) is present in order and no poll (any of "
            "them) ended at its own bound, run " + run);
    };
    grade_5a(a, "A");
    grade_5a(b, "B");

    // ---- 5b: level anchor ----
    // NMH_MUTATE=P5-5B swaps bin_a to the no-UARTBRK-set variant above: the
    // level between MARK=BRK and MARK=UNBRK then also reads 1, and this same check flips.
    {
        auto mbrk = mark_cycle_rx(a, kMarkBrk), munbrk = mark_cycle_rx(a, kMarkUnbrk);
        bool brk_level = mbrk.has_value() && level_before(a.tx_changes, *mbrk + 50);
        bool unbrk_level = munbrk.has_value() && level_before(a.tx_changes, *munbrk + 50);
        bool ok = mbrk.has_value() && munbrk.has_value() && !brk_level && unbrk_level;
        if (mbrk && munbrk) {
            std::string ch;
            for (auto c : a.tx_changes) {
                if (c < *mbrk) continue;
                ch += " " + std::to_string(int64_t(c) - int64_t(*mbrk));
                if (c > *munbrk + 50) break;
            }
            checks.note("5b: MARK=UNBRK at MARK=BRK+" + std::to_string(*munbrk - *mbrk) +
                        "; TxD changes from MARK=BRK on (relative to MARK=BRK):" + ch);
        }
        checks.line("5b", {"5a-DONE-A"}, ok,
            "[p257] TxD sampled 50 cycles after the MARK=BRK write is 0 (UARTBRK forces the "
            "transmit pin to zero) and sampled 50 cycles after the MARK=UNBRK write is 1 (idle); "
            "two sampled points");
    }

    // ---- 5c: no overrun, on run A and on run B ----
    // NMH_MUTATE=P5-5C swaps both runs to the delayed-read-of-character-0
    // variant: p.256 predicts OVRUN sets, and this same check flips.
    {
        bool ok = true;
        for (const auto& res : a.results) if (!res || (*res & 0x8000)) ok = false;
        checks.line("5c-A", {}, ok, "[p256] OVRUN (bit 15) is 0 in every RESULT value, run A");

        bool ok_b = true;
        for (const auto& res : b.results) if (!res || (*res & 0x8000)) ok_b = false;
        checks.line("5c-B", {}, ok_b, "[p256] OVRUN (bit 15) is 0 in every RESULT value, run B");
    }

    // ---- 5d: every RBF event is observed, on run A and on run B ----
    // NMH_MUTATE=P5-5D swaps both runs to the missing-INTREQ-clear variant.
    auto check_5d = [](const RxRun& r) {
        auto ready = mark_cycle_rx(r, kMarkReady);
        bool ok = ready.has_value() && *ready < r.s0;
        for (const auto& p : r.probe_i) if (p.first < r.s0 && (p.second & 0x0800)) ok = false;
        for (int j = 0; j < 8; ++j) {
            auto [lo, hi] = rx_window(r, j);
            bool seen_zero = false, jok = false;
            for (const auto& p : r.probe_i) {
                if (p.first < lo || p.first > hi) continue;
                bool bit11 = (p.second & 0x0800) != 0;
                if (!bit11) seen_zero = true; else { jok = seen_zero; break; }
            }
            if (!jok) ok = false;
        }
        return ok;
    };
    checks.line("5d-A", {}, check_5d(a),
        "[p256] MARK=READY precedes s_0, no PROBE_I value before s_0 shows RBF, and every "
        "character's own poll sees RBF=0 before its first RBF=1, run A");
    checks.line("5d-B", {}, check_5d(b),
        "[p256] MARK=READY precedes s_0, no PROBE_I value before s_0 shows RBF, and every "
        "character's own poll sees RBF=0 before its first RBF=1, run B");

    // ---- 5e: resolution -- every G5 bracket narrower than T/4, on run A
    // and on run B ----
    // NMH_MUTATE=P5-5E swaps both runs to the padded-poll-iteration variant.
    std::vector<Bracket> brackets_a(8), brackets_b(8);
    for (int j = 0; j < 8; ++j) {
        auto [lo, hi] = rx_window(a, j);
        brackets_a[j] = g5_bracket(a.probe_i, lo, hi);
    }
    for (int j = 0; j < 8; ++j) {
        auto [lo, hi] = rx_window(b, j);
        brackets_b[j] = g5_bracket(b.probe_i, lo, hi);
    }
    {
        bool ok = true;
        for (int j = 0; j < 8; ++j) {
            double width = brackets_a[j].ok ? double(brackets_a[j].t1 - brackets_a[j].t0 + brackets_a[j].S) : 1e18;
            if (!brackets_a[j].ok || !(width < T / 4.0)) ok = false;
        }
        checks.line("5e-A", {"5d-A"}, ok, "[p255] every G5 bracket (run A) is narrower than T/4");

        bool ok_b = true;
        for (int j = 0; j < 8; ++j) {
            double width = brackets_b[j].ok ? double(brackets_b[j].t1 - brackets_b[j].t0 + brackets_b[j].S) : 1e18;
            if (!brackets_b[j].ok || !(width < T / 4.0)) ok_b = false;
        }
        checks.line("5e-B", {"5d-B"}, ok_b, "[p255] every G5 bracket (run B) is narrower than T/4");
    }

    // ---- 5f: run B received the same characters ----
    // NMH_MUTATE=P5-5F drives run B itself at 2T.
    {
        bool ok = true;
        std::string dump;
        for (int j = 0; j < 8; ++j) {
            char buf[48];
            uint32_t got = b.results[j] ? uint32_t(*b.results[j]) : 0xFFFFFFFFu;
            std::snprintf(buf, sizeof buf, " j%d(exp %02X)=%04X", j, kRxBytes[j], got);
            dump += buf;
            if (!b.results[j] || (*b.results[j] & 0x00FF) != kRxBytes[j]) ok = false;
        }
        checks.note("5f run B RESULT dump:" + dump);
        checks.line("5f", {"5a-R1-B", "5a-R2-B", "5a-C8-B", "5a-DONE-B", "5c-B", "5d-B"}, ok,
            "[p255] RESULT bits 7-0 = byte_j for every j, run B");
    }

    // The preconditions every P5 proposition reads its schedule from.
    const std::vector<std::string> p5_pre = {"5a-R1-A", "5a-R2-A", "5a-C8-A", "5a-DONE-A",
                                             "5a-R1-B", "5a-R2-B", "5a-C8-B", "5a-DONE-B", "5b"};

    // The preconditions every one of (a), (b) and (c) reads.
    std::vector<std::string> p5_deps = p5_pre;
    for (const char* d : {"5c-A", "5c-B", "5d-A", "5d-B", "5e-A", "5e-B", "5f"}) p5_deps.push_back(d);

    // ---- (a) Data: run A recovers the byte, bit 9 and bit 8 for every j.
    // NMH_MUTATE=P5-A-DATA drives run A with every byte complemented at the
    // same timing; the comparator and the expected bytes are unchanged. ----
    {
        bool data_ok = true;
        std::string dump;
        for (int j = 0; j < 8; ++j) {
            char buf[48];
            uint32_t got = a.results[j] ? uint32_t(*a.results[j]) : 0xFFFFFFFFu;
            std::snprintf(buf, sizeof buf, " j%d(exp %02X)=%04X", j, kRxBytes[j], got);
            dump += buf;
            if (!a.results[j] || (*a.results[j] & 0x03FF) != (0x0300 | kRxBytes[j])) data_ok = false;
        }
        checks.note("(a) run A RESULT dump:" + dump);
        checks.line("(a)", p5_deps, data_ok,
            "[p255,p256,p257,p283] run A recovers byte_j (RESULT bits 7-0), bit 9 and "
            "bit 8 for every j");
        checks.record(
            "opus:8-serial.c36 sol:8-serial.c21 [p255,p256,p257,p283]: withdrawn from the graded "
            "line above -- establishing that bit 8 specifically carries the ninth (parity) bit "
            "needs a run where the ninth bit is driven 0, and p.257 and p.283 disagree on what "
            "RESULT bit 8 should then read (0 or 1; see the run-A ninth-bit-zero variant below), "
            "so that discriminating case cannot be graded at this boundary");
    }

    // ---- (b) No early signal ----
    // G5 places the event in (t0-S, t1]. Per character, with the threshold
    // s_j+9T (run A) or s_j+8T (run B): the character PASSES when the
    // bracket's lower end t0-S reaches the threshold (the event lies after
    // it), FAILS when t1 stays below it (the event lies before it), and
    // otherwise is undecided: the bracket straddles the threshold, so no
    // sampling phase the pages allow is excluded. The line passes only when
    // all 16 characters are decided and pass; it fails when any character
    // fails; otherwise (no failure, some undecided) it is NOT GRADED.
    {
        int pass = 0, fail = 0, undecided = 0;
        auto grade_char = [&](const Bracket& br, double sj, double thr_bits) {
            if (!br.ok) { ++undecided; return; }
            double thr = sj + thr_bits * T;
            double lo = double(br.t0) - double(br.S);
            if (lo >= thr) ++pass;
            else if (double(br.t1) < thr) ++fail;
            else ++undecided;
        };
        for (int j = 0; j < 8; ++j) grade_char(brackets_a[j], double(a.s0) + double(j) * step, 9.0);
        for (int j = 0; j < 8; ++j) grade_char(brackets_b[j], double(b.s0) + double(j) * step, 8.0);
        checks.note("(b) characters (of 16): reach the threshold=" + std::to_string(pass) +
                    ", stay below it=" + std::to_string(fail) + ", straddle it=" + std::to_string(undecided));
        const std::string text =
            "[p256] no character's RBF event lies before s_j+9T (run A) or s_j+8T (run B): all 16 "
            "G5 brackets (t0-S, t1] decide the threshold, and each has its lower end t0-S at or past it";
        if (checks.unmet_of(p5_deps).empty() && fail == 0 && undecided > 0) {
            checks.record("NOT GRADED (b): " + std::to_string(undecided) +
                          " of 16 characters' brackets straddle the threshold and none lies below it");
            checks.verdict["(b)"] = false;
        } else {
            checks.line("(b)", p5_deps, fail == 0 && undecided == 0 && pass == 16, text);
        }
    }

    // ---- (c) One bit later: for every character j, the RBF event of run A
    // comes one bit time later than run B's own. ----
    // G5 places each event in (t0-S, t1], so D_j(A)-D_j(B), with D_j the
    // event time minus s_j, lies in (loA-sA-hiB+sB, hiA-sA-loB+sB]. A
    // character PASSES when that whole interval lies in [T/2,3T/2] (the
    // half-bit grading of p.256's "proper number of bit times", because the
    // page does not say where the receiver samples), FAILS when the interval
    // lies wholly outside it, and otherwise is undecided. The line passes
    // only when all 8 characters pass, fails when any fails, and is NOT
    // GRADED otherwise.
    {
        int pass = 0, fail = 0, undecided = 0;
        std::string per;
        double sumA = 0, sumB = 0;
        for (int j = 0; j < 8; ++j) {
            const double sA = double(a.s0) + j * step, sB = double(b.s0) + j * step;
            sumA += bracket_midpoint(brackets_a[j]) - sA;
            sumB += bracket_midpoint(brackets_b[j]) - sB;
            if (!brackets_a[j].ok || !brackets_b[j].ok) { ++undecided; per += " j" + std::to_string(j) + "=no bracket"; continue; }
            const double loA = double(brackets_a[j].t0) - double(brackets_a[j].S), hiA = double(brackets_a[j].t1);
            const double loB = double(brackets_b[j].t0) - double(brackets_b[j].S), hiB = double(brackets_b[j].t1);
            const double dlo = (loA - sA) - (hiB - sB), dhi = (hiA - sA) - (loB - sB);
            per += " j" + std::to_string(j) + "=[" + std::to_string(dlo) + "," + std::to_string(dhi) + "]";
            if (dlo >= T / 2.0 && dhi <= 1.5 * T) ++pass;
            else if (dhi < T / 2.0 || dlo > 1.5 * T) ++fail;
            else ++undecided;
        }
        checks.note("(c): D_j(A)-D_j(B) bracket per character:" + per + "; T=" + std::to_string(T) +
                    "; mean_j D_j(A) - mean_j D_j(B) = " + std::to_string((sumA - sumB) / 8.0));
        checks.note("(c) characters (of 8): inside [T/2,3T/2]=" + std::to_string(pass) +
                    ", outside it=" + std::to_string(fail) + ", straddle it=" + std::to_string(undecided));
        const std::string text =
            "[p255,p256] for every character j, run A's RBF event comes one bit time later than "
            "run B's own: the G5 interval of D_j(A)-D_j(B) lies in [T/2,3T/2] for all 8 characters";
        if (checks.unmet_of(p5_deps).empty() && fail == 0 && undecided > 0) {
            checks.record("NOT GRADED (c): " + std::to_string(undecided) +
                          " of 8 characters' intervals straddle the band edge and none lies outside it");
            checks.verdict["(c)"] = false;
        } else {
            checks.line("(c)", p5_deps, fail == 0 && undecided == 0 && pass == 8, text);
        }
    }

    // ---- P5 run-A ninth-bit-zero variant: recorded, not graded (p.257
    // predicts RESULT bit 8 = 0, p.283 predicts 1; SPECIFICATION.md P5).
    // Always the pristine run-A binary, regardless of NMH_MUTATE, since it
    // exercises the RxD driver's own ninth-bit level, not a stimulus
    // mutation. ----
    {
        RxRun a0 = run_rx(dir + "/prog_rx_a.bin", kP5Budget, T, 1.0, true, true);
        std::string dump;
        int bytes_ok = 0;
        for (int j = 0; j < 8; ++j) {
            char buf[24];
            uint32_t got = a0.results[j] ? uint32_t(*a0.results[j]) : 0xFFFFFFFFu;
            std::snprintf(buf, sizeof buf, " j%d=%04X", j, got);
            dump += buf;
            if (a0.results[j] && (*a0.results[j] & 0x00FF) == kRxBytes[j]) ++bytes_ok;
        }
        checks.note("P5 run-A ninth-bit-zero variant: loaded=" + std::to_string(a0.loaded) +
                    " released=" + std::to_string(a0.released) + " DONE=" + std::to_string(rx_sequence_present(a0)) +
                    " marks=" + std::to_string(a0.marks.size()) + " POLL_TIMEOUT=" + std::to_string(a0.poll_timeout) +
                    " bits 7-0 = byte_j for " + std::to_string(bytes_ok) + " of 8; RESULT words (every ninth bit driven 0):" + dump);
        checks.record("P5 run-A ninth-bit-zero variant [p257,p283]: RESULT bit 8 is not graded -- "
                       "p.257 predicts 0, p.283 predicts 1, so the pages disagree");
    }
}

// ==== P4: disk write DMA capture and analysis ==============================

constexpr uint32_t kDbufAddr   = 0x020000; // the program's own DBUF (ch8_defs.i)
constexpr uint32_t kProbeDAddr = 0x0C0010; // kept clear of RESULT..RESULT+$0E (ch8_defs.i)
constexpr int       kDbufLen   = 256;

struct FastRun {
    bool loaded = false, released = false;
    std::optional<uint64_t> done_cycle;
    std::vector<nmh::RamAccess> log;
    std::optional<uint16_t> result;
    std::vector<RefreshGroup> refresh;
    double C = 0.0;
    int field_lines = 0;
    bool poll_timeout = false; // G4: the custom disk poll loop reached its own bound
    R1Scan r1;
};

FastRun run_fast(const std::string& bin, bool ntsc, uint64_t budget) {
    FastRun r;
    nmh::NanomigHarness h(cfg_for(ntsc));
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done_cycle = h.run_until_write(kMarkAddr, kMarkDone, budget);
    r.log = logical_log(h.ram_log());
    r.result = h.peek_logical(kResultAddr);
    r.r1 = scan_refresh(h.ram_log(), h.cycles());
    r.refresh = r.r1.groups;
    r.C = r.r1.C;
    auto frames = nmh::find_frames(h.video());
    if (frames.size() >= 3) {
        int count = 0;
        for (const auto& g : r.refresh) if (g.r1 >= frames[1].begin && g.r1 < frames[1].end) ++count;
        r.field_lines = count;
    }
    auto to = h.peek_logical(kPollTimeoutAddr);
    r.poll_timeout = to.has_value() && *to != 0;
    return r;
}

// First read cycle of each word B+2i, i=0..n-1.
std::vector<std::optional<uint64_t>> extract_ti(const FastRun& r, int n = kDbufLen) {
    std::vector<std::optional<uint64_t>> t(n);
    uint32_t base = word_addr(kDbufAddr);
    for (const auto& a : r.log) {
        if (a.is_write || a.word_addr < base || a.word_addr >= base + uint32_t(n)) continue;
        int i = int(a.word_addr - base);
        if (!t[i]) t[i] = a.cycle;
    }
    return t;
}

// Total read count of each word B+2i, i=0..n-1 -- unlike extract_ti(), this
// counts every read, not just the first, so a duplicate read of an
// already-seen index is visible.
std::vector<int> count_reads(const FastRun& r, int n = kDbufLen) {
    std::vector<int> c(n, 0);
    uint32_t base = word_addr(kDbufAddr);
    for (const auto& a : r.log) {
        if (a.is_write || a.word_addr < base || a.word_addr >= base + uint32_t(n)) continue;
        ++c[int(a.word_addr - base)];
    }
    return c;
}

bool result_fast_bit_matches(const FastRun& r, bool expect_fast1) {
    if (!r.result) return false;
    return ((*r.result & 0x0100) != 0) == expect_fast1;
}

// 4d: every PROBE_D value the CPU stored inside [lo,hi] has DMAON (bit14)
// set. DISKWRITE (bit13) is counted, not graded (see 4d below).
bool check_4d(const FastRun& r, uint64_t lo, uint64_t hi, int* n_checked, int* n_diskwrite = nullptr) {
    int n = 0, nw = 0; bool ok = true;
    uint32_t pd = word_addr(kProbeDAddr);
    for (const auto& a : r.log) {
        if (a.is_write && a.word_addr == pd && a.cycle >= lo && a.cycle <= hi) {
            ++n;
            if ((a.data & 0x4000) == 0) ok = false;
            if (a.data & 0x2000) ++nw;
        }
    }
    if (n_checked) *n_checked = n;
    if (n_diskwrite) *n_diskwrite = nw;
    return ok && n > 0;
}

// 4f: the buffer holds the known value. Returns (a) the number of words
// B+2i, i=0..255, whose last Chip-port write before MARK=START is $AAAA, and
// (b) the number of Chip-port reads of buffer words and how many of them
// delivered $AAAA.
struct FillScan { int fill_words = 0; int reads = 0, reads_aaaa = 0; bool have_start = false; };
FillScan scan_fill(const FastRun& r) {
    FillScan f;
    const uint32_t base = word_addr(kDbufAddr), mk = word_addr(kMarkAddr);
    uint64_t start = 0;
    for (const auto& a : r.log)
        if (a.is_write && a.word_addr == mk && a.data == 0x00F4) { start = a.cycle; f.have_start = true; break; }
    if (!f.have_start) return f;
    std::vector<int> last(kDbufLen, -1);
    for (const auto& a : r.log) {
        if (a.word_addr < base || a.word_addr >= base + uint32_t(kDbufLen)) continue;
        const int i = int(a.word_addr - base);
        if (a.is_write) { if (a.cycle < start) last[i] = a.data; }
        else { ++f.reads; if (a.data == 0xAAAA) ++f.reads_aaaa; }
    }
    for (int i = 0; i < kDbufLen; ++i) if (last[i] == 0xAAAA) ++f.fill_words;
    return f;
}

double T_cell_us(uint64_t t_lo, uint64_t t_hi, double C, bool ntsc) {
    if (C <= 0) return -1;
    double cc = double(t_hi - t_lo) / C;
    double cc_per_cell = cc / (128.0 * 16.0);
    double us_per_cc = ntsc ? 0.27936 : 0.28194;
    return cc_per_cell * us_per_cc;
}

void run_p4(const std::string& dir, const std::string& mv, Checks& checks) {
    auto bin_fast1 = dir + (mv == "R1-READ" ? "/prog_fast1_r1.bin" :
                             mv == "P4-4C" ? "/prog_fast1_4c.bin" :
                             mv == "P4-4D" ? "/prog_fast1_4d.bin" :
                             mv == "P4-4E" ? "/prog_fast1_4e.bin" :
                             mv == "P4-4F" ? "/prog_fast1_4f.bin" :
                             mv == "P4-4E-ONCE" ? "/prog_fast1_once.bin" :
                             mv == "P4-STALL" ? "/prog_fast1_stall.bin" : "/prog_fast1.bin");
    auto bin_fast0 = dir + (mv == "R1-READ" ? "/prog_fast0_r1.bin" :
                             mv == "P4-4C" ? "/prog_fast0_4c.bin" :
                             mv == "P4-4D" ? "/prog_fast0_4d.bin" :
                             mv == "P4-4E" ? "/prog_fast0_4e.bin" :
                             mv == "P4-4F" ? "/prog_fast0_4f.bin" :
                             mv == "P4-4E-ONCE" ? "/prog_fast0_once.bin" :
                             mv == "P4-STALL" ? "/prog_fast0_stall.bin" : "/prog_fast0.bin");
    constexpr uint64_t kP4Budget = 21'000'000;

    FastRun ntsc_f1 = run_fast(bin_fast1, true, kP4Budget);
    FastRun ntsc_f0 = run_fast(bin_fast0, true, kP4Budget);
    FastRun pal_f1  = run_fast(bin_fast1, false, kP4Budget);
    FastRun pal_f0  = run_fast(bin_fast0, false, kP4Budget);

    checks.note("P4 capture health: ntsc_f1 loaded=" + std::to_string(ntsc_f1.loaded) +
                " done=" + std::to_string(ntsc_f1.done_cycle.has_value()) +
                " C=" + std::to_string(ntsc_f1.C) + " field_lines=" + std::to_string(ntsc_f1.field_lines) +
                " result=" + std::to_string(ntsc_f1.result.value_or(0xFFFF)));
    checks.note("P4 capture health: ntsc_f0 loaded=" + std::to_string(ntsc_f0.loaded) +
                " done=" + std::to_string(ntsc_f0.done_cycle.has_value()) +
                " C=" + std::to_string(ntsc_f0.C) + " field_lines=" + std::to_string(ntsc_f0.field_lines) +
                " result=" + std::to_string(ntsc_f0.result.value_or(0xFFFF)));
    checks.note("P4 capture health: pal_f1 loaded=" + std::to_string(pal_f1.loaded) +
                " done=" + std::to_string(pal_f1.done_cycle.has_value()) +
                " C=" + std::to_string(pal_f1.C) + " field_lines=" + std::to_string(pal_f1.field_lines));
    checks.note("P4 capture health: pal_f0 loaded=" + std::to_string(pal_f0.loaded) +
                " done=" + std::to_string(pal_f0.done_cycle.has_value()) +
                " C=" + std::to_string(pal_f0.C) + " field_lines=" + std::to_string(pal_f0.field_lines));

    auto grade_run = [&](const FastRun& r, bool ntsc, bool fast1, const std::string& tag) {
        const std::string pt = "P4-" + tag;
        auto t = extract_ti(r);
        bool has_window = t[64].has_value() && t[128].has_value() && t[192].has_value();
        double tcell = has_window ? T_cell_us(*t[64], *t[192], r.C, ntsc) : -1;
        checks.note(pt + ": T_cell=" + std::to_string(tcell) + " us, t64=" +
                    std::to_string(t[64].value_or(0)) + " t128=" + std::to_string(t[128].value_or(0)) +
                    " t192=" + std::to_string(t[192].value_or(0)));

        checks.record(pt +
            " [p265,p249]: cannot be checked at this boundary -- the disk data clock is not "
            "among the top-level observable ports, and no page ties a Chip-memory read rate to "
            "it (opus:8-adkcon.c33 declined, SPECIFICATION.md 1.4)");

        // 4a: R1 anchor, and the run's own refresh-group population.
        checks.note(pt + " " + r1_note(r.r1, ntsc));
        bool r1ok = r1_ok(r.r1, ntsc);
        checks.line("4a-" + tag, {}, r1ok,
            "[p194,p195] R1 anchor holds on this run, and every candidate refresh group is "
            "the same four-read, 2C-apart shape (Figure 6-9), none discarded before grading");

        // 4b: R2 shows the standard selected.
        bool std_ok = ntsc ? (r.field_lines == 262 || r.field_lines == 263)
                            : (r.field_lines == 312 || r.field_lines == 313);
        checks.line("4b-" + tag, {}, std_ok,
            "[p24] R2's own field-line count (" + std::to_string(r.field_lines) +
            ") matches the standard selected");

        // Completion (G4): the run reached DONE and no poll ended at its own
        // bound. 4c, 4d and 4e are graded only when it holds.
        checks.line("P4-DONE-" + tag, {}, r.done_cycle.has_value() && !r.poll_timeout,
            "[G4] MARK=DONE is present and no poll (any of them) ended at its own bound");

        // 4c: the FAST bit took effect (ADKCONR readback).
        bool c_ok = result_fast_bit_matches(r, fast1);
        checks.line("4c-" + tag, {"P4-DONE-" + tag}, c_ok,
            "[p249] ADKCONR bit 8 read back as " + std::string(fast1 ? "1" : "0") +
            " (RESULT=" + std::to_string(r.result.value_or(0xFFFF)) + ")");

        // 4f: the buffer holds the known valid raw-MFM value ($AAAA) when the
        // disk DMA reads it: every word B+2i, i=0..255, was last written
        // with $AAAA before MARK=START, and every Chip-port read of a buffer
        // word (at least one) delivered $AAAA.
        FillScan fs = scan_fill(r);
        checks.note(pt + " 4f: words whose last write before MARK=START is $AAAA=" + std::to_string(fs.fill_words) +
                    " of " + std::to_string(kDbufLen) + ", buffer reads seen=" + std::to_string(fs.reads) +
                    ", delivering $AAAA=" + std::to_string(fs.reads_aaaa));
        checks.line("4f-" + tag, {"P4-DONE-" + tag},
            fs.have_start && fs.fill_words == kDbufLen && fs.reads > 0 && fs.reads_aaaa == fs.reads,
            "[p250,p249,p247] the buffer holds the known valid raw-MFM value: the last write to each of "
            "B+2i, i=0..255, before MARK=START is $AAAA, and every Chip-port read of a buffer word "
            "(" + std::to_string(fs.reads) + " seen) delivers $AAAA");

        // 4e: fetches cover the window (i=0..192 seen, each exactly once, rising order).
        // Its coverage clause is also the precondition of 4d (the window t_64..t_192
        // exists), so it is printed first.
        bool cov_ok = true, e_ok = true;
        int unseen = 0, order_bad = 0;
        uint64_t prev = 0; bool have_prev = false;
        for (int i = 0; i <= 192; ++i) {
            if (!t[i]) { e_ok = false; cov_ok = false; ++unseen; continue; }
            if (have_prev && !(*t[i] > prev)) { e_ok = false; ++order_bad; }
            prev = *t[i]; have_prev = true;
        }
        auto counts = count_reads(r);
        int not_once = 0;
        for (int i = 0; i <= 192; ++i) if (counts[i] != 1) { e_ok = false; ++not_once; }
        checks.note(pt + " 4e: of the 193 indices i=0..192, never read=" + std::to_string(unseen) +
                    ", not read exactly once=" + std::to_string(not_once) + ", out of rising order=" +
                    std::to_string(order_bad) + "; reads counted after coalescing held bus cycles");
        checks.line("4e-" + tag, {"P4-DONE-" + tag, "4f-" + tag}, e_ok,
            "[p247] reads of B+2i are seen for i=0..192, each exactly "
            "once (by full read count, not just first-seen), in rising order");
        checks.verdict["4e-window-" + tag] = cov_ok && has_window;

        // 4d: DMAON is set at every instant the CPU sampled DSKBYTR inside the
        // window; the window must exist. The samples are the CPU's own, one
        // per poll iteration: the line says nothing about the instants
        // between them.
        int n4d = 0, n4d_write = 0;
        bool d_ok = has_window && check_4d(r, *t[64], *t[192], &n4d, &n4d_write);
        checks.note(pt + " 4d: " + std::to_string(n4d) + " PROBE_D samples in window, DMAON set in " +
                    (d_ok ? std::to_string(n4d) : std::string("not all")) + " of them; DISKWRITE (bit 13) set in " +
                    std::to_string(n4d_write) + " (measured, not graded)");
        checks.line("4d-" + tag, {"4e-window-" + tag, "4f-" + tag, "P4-DONE-" + tag}, d_ok,
            "[p248] DMAON is set in every PROBE_D value the CPU sampled between t_64 and t_192 (" +
            std::to_string(n4d) + " sampled instants)");

        // Also measured, not graded: no page sentence bounds the fetch cadence.
        std::string span = has_window ?
            (" (measured " + std::to_string(std::abs(int64_t(*t[128] - *t[64]) - int64_t(*t[192] - *t[128]))) + ")") :
            std::string(" (no window observed)");
        checks.note(pt + " |(t128-t64)-(t192-t128)|" + span);
    };

    grade_run(ntsc_f1, true, true, "NTSC-FAST1");
    grade_run(ntsc_f0, true, false, "NTSC-FAST0");
    grade_run(pal_f1, false, true, "PAL-FAST1");
    grade_run(pal_f0, false, false, "PAL-FAST0");

    // 4c's own mutation needs no separate code: NMH_MUTATE=P4-4C already
    // swapped both binaries above, so every "4c-..." check reads the
    // mutant. NMH_MUTATE=P4-4D/4E likewise swap both binaries.
}

// The claims of the package that no line grades, each printed with its reason
// (SPECIFICATION.md section 1), one RECORDED line per id.
struct DeclinedRow { std::vector<const char*> ids; const char* page; const char* reason; };

void record_declined(Checks& checks) {
    const std::vector<DeclinedRow> rows = {
        {{"opus:8-dsk.c10", "opus:8-dsk.c11", "opus:8-dsk.c12", "opus:8-dsk.c13", "opus:8-dsk.c14",
          "opus:8-dsk.c15", "opus:8-dsk.c16", "sol:8-dsk.c10", "sol:8-dsk.c11", "sol:8-dsk.c12",
          "sol:8-dsk.c13", "sol:8-dsk.c14", "sol:8-dsk.c15", "sol:8-dsk.c16", "sol:8-dsk.c17",
          "sol:8-dsk.c18"}, "p241",
         "needs data read from or written to a disk surface; the harness's fixture mounts no image "
         "(sdc_img_mounted and sdc_img_size stay 0) and answers no sector request, although nanomig.v "
         "exposes those ports and the sector and byte ports; once an image is served each named format "
         "can be checked one at a time, while \"virtually any\" and \"most\" stay declined because "
         "no sample of formats establishes a quantifier (SPECIFICATION.md 1.1)"},
        {{"opus:8-mfm.c7", "sol:8-gcr.c2"}, "p250",
         "MSBSYNC acts on disk read and needs bytes whose most significant bit is set, read from a disk "
         "surface; the harness's fixture mounts no image and answers no sector request, so the controller "
         "is given no disk bytes, although nanomig.v exposes the ports that would carry them "
         "(SPECIFICATION.md 1.1)"},
        {{"opus:8-adkcon.c31", "sol:8-appa-adkbit.c13", "opus:8-appA-adkcon.c22"}, "p265",
         "ADKCON bit 9 (MSBSYNC) acts on disk read, so the reason of the claims on p250 applies: the "
         "harness's fixture mounts no image and answers no sector request, although nanomig.v exposes "
         "the ports (SPECIFICATION.md 1.1)"},
        {{"opus:8-cia.c9", "sol:8-cia.c7", "opus:8-cia.c10", "sol:8-cia.c8"}, "p244",
         "the page gives DSKRDY* a meaning only while the motor is on (\"only valid when the motor is "
         "ON\") and says that at other times \"configuration information may obscure the meaning of this "
         "input\": both clauses say when the input may be trusted and neither prints a value, so the "
         "motor-off case has no falsifiable expected value; DF0 is configured (floppy_config=0), only "
         "the image interface is idle (SPECIFICATION.md 1.2)"},
        {{"sol:8-serper.c4", "sol:8-serper.c5"}, "p255",
         "an absolute duration (279.36 ns NTSC, 281.94 ns PAL); a test measures cycles and no page ties "
         "the clock to nanoseconds, so it is used only to convert (SPECIFICATION.md 1.3)"},
        {{"opus:8-serper.c10"}, "p284",
         "a constant (.2794 microseconds) for no named machine; it cannot be observed in cycles and on "
         "PAL disagrees with p255's colour clock (SPECIFICATION.md 1.3)"},
    };
    for (const auto& row : rows)
        for (const char* id : row.ids)
            checks.record(std::string(id) + " [" + row.page + "]: not graded -- " + row.reason);
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";
    g_std_swap = (mv == "STD-SWAP");

    record_declined(checks);

    // ==== P1/P2/P3: shared captures =====================================
    //
    // Each base binary is captured once per standard; NMH_MUTATE selects a
    // whole different binary (or nothing) uniformly for every capture that
    // reads it, so a single env var exercises the same substitution on
    // both NTSC and PAL alike, and on every proposition that shares it.

    auto bin_n0        = dir + (mv == "R1-READ" ? "/prog_tx_n0_r1.bin" :
                                  mv == "P1-NPLUS1" ? "/prog_tx_n0_plus1.bin" :
                                  mv == "P1-SINGLE-BRK" ? "/prog_tx_n0_brk.bin" :
                                  mv == "P1-SINGLE-STALL" ? "/prog_tx_n0_stall.bin" : "/prog_tx_n0.bin");
    auto bin_n371       = dir + (mv == "R1-READ" ? "/prog_tx_n371_r1.bin" :
                                  mv == "P1-1C" ? "/prog_tx_n371_1c.bin" :
                                  mv == "P1-1D" ? "/prog_tx_n371_1d.bin" :
                                  mv == "P1-1E" ? "/prog_tx_n371_1e.bin" :
                                  mv == "FIXTURE" ? "/prog_tx_n371_nomark3.bin" :
                                  mv == "P1-NPLUS1" ? "/prog_tx_n371_plus1.bin" : "/prog_tx_n371.bin");
    auto bin_n32396     = dir + (mv == "R1-READ" ? "/prog_tx_n32396_r1.bin" :
                                  mv == "P1-NPLUS1" ? "/prog_tx_n32396_plus1.bin" :
                                  mv == "P1-SINGLE-BRK" ? "/prog_tx_n32396_brk.bin" :
                                  mv == "P1-SINGLE-STALL" ? "/prog_tx_n32396_stall.bin" : "/prog_tx_n32396.bin");

    TxRun ntsc_n0    = run_tx(bin_n0, true, budget_for(0, 1));
    TxRun ntsc_n371  = run_tx(bin_n371, true, budget_for(371, 4));
    TxRun ntsc_n32396 = run_tx(bin_n32396, true, budget_for(32396, 1));
    TxRun pal_n0     = run_tx(bin_n0, false, budget_for(0, 1));
    TxRun pal_n371   = run_tx(bin_n371, false, budget_for(371, 4));
    TxRun pal_n32396 = run_tx(bin_n32396, false, budget_for(32396, 1));

    checks.note("capture health: ntsc_n0 loaded=" + std::to_string(ntsc_n0.loaded) +
                " released=" + std::to_string(ntsc_n0.released) +
                " changes=" + std::to_string(ntsc_n0.tx_changes.size()) +
                " marks=" + std::to_string(ntsc_n0.marks.size()) +
                " mark0=" + std::to_string(mark_cycle(ntsc_n0, 0).value_or(999999999)) +
                " C=" + std::to_string(ntsc_n0.C) +
                " field_lines=" + std::to_string(ntsc_n0.field_lines));
    checks.note("capture health: ntsc_n371 loaded=" + std::to_string(ntsc_n371.loaded) +
                " released=" + std::to_string(ntsc_n371.released) +
                " changes=" + std::to_string(ntsc_n371.tx_changes.size()) +
                " marks=" + std::to_string(ntsc_n371.marks.size()) +
                " C=" + std::to_string(ntsc_n371.C) +
                " field_lines=" + std::to_string(ntsc_n371.field_lines));
    checks.note("capture health: ntsc_n32396 loaded=" + std::to_string(ntsc_n32396.loaded) +
                " released=" + std::to_string(ntsc_n32396.released) +
                " changes=" + std::to_string(ntsc_n32396.tx_changes.size()) +
                " marks=" + std::to_string(ntsc_n32396.marks.size()) +
                " C=" + std::to_string(ntsc_n32396.C) +
                " field_lines=" + std::to_string(ntsc_n32396.field_lines));
    checks.note("capture health: pal_n0 loaded=" + std::to_string(pal_n0.loaded) +
                " changes=" + std::to_string(pal_n0.tx_changes.size()) +
                " marks=" + std::to_string(pal_n0.marks.size()) +
                " mark0=" + std::to_string(mark_cycle(pal_n0, 0).value_or(999999999)) +
                " C=" + std::to_string(pal_n0.C) +
                " field_lines=" + std::to_string(pal_n0.field_lines));
    checks.note("capture health: pal_n371 loaded=" + std::to_string(pal_n371.loaded) +
                " changes=" + std::to_string(pal_n371.tx_changes.size()) +
                " marks=" + std::to_string(pal_n371.marks.size()) +
                " mark0=" + std::to_string(mark_cycle(pal_n371, 0).value_or(999999999)) +
                " C=" + std::to_string(pal_n371.C) +
                " field_lines=" + std::to_string(pal_n371.field_lines));
    checks.note("capture health: pal_n32396 loaded=" + std::to_string(pal_n32396.loaded) +
                " changes=" + std::to_string(pal_n32396.tx_changes.size()) +
                " C=" + std::to_string(pal_n32396.C) +
                " field_lines=" + std::to_string(pal_n32396.field_lines));

    // ---- FIXTURE: the fixture proves it can deliver a known value before
    // anything reads one -- the N=371 run's own marker sequence 0,2,3,4,DONE
    // in rising cycle order, on each standard. The lines that read a marker
    // of that run (1c-1e, 2c-2e) are graded only when it holds. ----
    auto grade_fixture = [&](const TxRun& r, const std::string& id, const char* what) {
        auto m0 = mark_cycle(r, 0), m2 = mark_cycle(r, 2), m3 = mark_cycle(r, 3),
             m4 = mark_cycle(r, 4), md = mark_cycle(r, kMarkDone);
        bool seq_ok = r.loaded && r.released && m0 && m2 && m3 && m4 && md &&
                      *m0 < *m2 && *m2 < *m3 && *m3 < *m4 && *m4 < *md;
        checks.line(id, {}, seq_ok,
            std::string("[G3] the N=371 program's own MARK sequence (0,2,3,4,DONE) is present in "
                        "ram_log() in rising cycle order, ") + what);
    };
    grade_fixture(ntsc_n371, "FIXTURE", "NTSC run");
    grade_fixture(pal_n371, "FIXTURE-PAL", "PAL run");

    // ---- preconditions of P1 (N=371 NTSC run) ----
    {
        checks.note("1a: " + r1_note(ntsc_n371.r1, true));
        bool ok = r1_ok(ntsc_n371.r1, true);
        checks.line("1a", {}, ok,
            "[p194,p195] R1 anchor: the four refresh accesses in every line of the N=371 NTSC "
            "run are 2C, 2C and 2C apart, the same C in every line, and every candidate refresh "
            "group in the run has that same four-read shape (Figure 6-9), none discarded before "
            "grading");
    }
    {
        bool ntsc_ok = ntsc_n371.field_lines == 262 || ntsc_n371.field_lines == 263;
        checks.note("1b: NTSC capture field_lines=" + std::to_string(ntsc_n371.field_lines) +
                    ", PAL capture of the same binary field_lines=" + std::to_string(pal_n371.field_lines));
        checks.line("1b", {}, ntsc_ok,
            "[p24] R2: the NTSC capture's own refresh-group count between two vertical syncs is "
            "262 or 263 (measured " + std::to_string(ntsc_n371.field_lines) + ")");
    }
    // 1c: TxD idle before the window. NMH_MUTATE=P1-1C swaps bin_n371, so
    // ntsc_n371 (and pal_n371, built from the same binary) is the mutant.
    {
        auto m0 = mark_cycle(ntsc_n371, 0);
        long long before_count = m0 ? std::count_if(ntsc_n371.tx_changes.begin(), ntsc_n371.tx_changes.end(),
                                                      [&](uint64_t c) { return c < *m0; })
                                     : -1;
        checks.note("1c: changes-before-MARK0=" + std::to_string(before_count));
        checks.line("1c", {"FIXTURE"}, m0.has_value() && before_count == 0,
            "[p257] TxD does not change from the start of the run until MARK=0 in the N=371 "
            "NTSC run");
    }
    // 1e: the run completed (no poll ended at its bound). NMH_MUTATE=P1-1E
    // swaps bin_n371 to the missing-word-2 variant: word 3's TBE poll then
    // reaches BND_TBE, and POLL_TIMEOUT (set directly by POLLBIT,
    // ch8_defs.i) records that for every poll in the run.
    {
        auto m2 = mark_cycle(ntsc_n371, 2), m3 = mark_cycle(ntsc_n371, 3);
        auto gap = (m2 && m3) ? std::optional<int64_t>(int64_t(*m3) - int64_t(*m2)) : std::nullopt;
        bool done = mark_cycle(ntsc_n371, kMarkDone).has_value();
        checks.note("1e: MARK2->MARK3 gap=" + std::to_string(gap.value_or(-1)) + " cycles, DONE present=" +
                    std::to_string(done) + ", POLL_TIMEOUT=" + std::to_string(ntsc_n371.poll_timeout));
        checks.line("1e", {"FIXTURE"}, done && !ntsc_n371.poll_timeout,
            "[G4] MARK=DONE is present and no poll (G4's POLLBIT, any of them) ended at its "
            "own bound in the N=371 NTSC run");
    }
    // 1d: words sent back to back (N=371 only), graded after 1e. NMH_MUTATE=P1-1D swaps
    // bin_n371 to the TSRE-polling variant.
    {
        double T = 372.0 * ntsc_n371.C;
        bool ok = true;
        for (int k = 2; k <= 4; ++k) {
            auto mk = mark_cycle(ntsc_n371, uint16_t(k));
            if (!mk) { ok = false; continue; }
            double bound = double(ntsc_n371.tx_changes.empty() ? 0 : ntsc_n371.tx_changes[0]) +
                           double(10 * (k - 1) - 1) * T;
            if (!(double(*mk) < bound)) ok = false;
        }
        checks.line("1d", {"FIXTURE", "1a", "1e"}, ok,
            "[p257,p283] for k=2..4, MARK=k comes before word (k-1)'s own stop-bit change in "
            "the N=371 NTSC run");
    }

    // ---- the same per-run preconditions (R1 anchor+population, R2
    // standard, completion, TxD idle before MARK=0) for the single-word
    // runs N=0 and N=32396, on each standard. (1d does not apply: a W=1
    // run has no "next word".) ----
    auto check_run_preconditions = [&](const TxRun& r, const std::string& pfx, bool ntsc) {
        checks.note(pfx + ": " + r1_note(r.r1, ntsc));
        bool anchor_ok = r1_ok(r.r1, ntsc);
        checks.line(pfx + "-R1", {}, anchor_ok,
            "[p194,p195] R1 anchor: the four refresh accesses in every line are 2C, 2C and 2C "
            "apart, the same C in every line, and every candidate refresh group has that same "
            "four-read shape (Figure 6-9), none discarded before grading");
        bool std_ok = ntsc ? (r.field_lines == 262 || r.field_lines == 263)
                            : (r.field_lines == 312 || r.field_lines == 313);
        checks.line(pfx + "-R2", {}, std_ok,
            "[p24] R2: the run's own refresh-group count (" + std::to_string(r.field_lines) +
            ") matches the standard selected");
        auto m0 = mark_cycle(r, 0), md = mark_cycle(r, kMarkDone);
        checks.line(pfx + "-DONE", {}, m0 && md && *m0 < *md && !r.poll_timeout,
            "[G3,G4] MARK=0 and MARK=DONE are present in rising order and no poll (G4's POLLBIT, any "
            "of them) ended at its own bound");
        bool idle_ok = m0.has_value() &&
                       std::none_of(r.tx_changes.begin(), r.tx_changes.end(),
                                    [&](uint64_t c) { return c < *m0; });
        checks.line(pfx + "-IDLE", {pfx + "-DONE"}, idle_ok,
            "[p257] TxD does not change from the start of the run until MARK=0");
    };
    check_run_preconditions(ntsc_n0, "P1N0", true);
    check_run_preconditions(ntsc_n32396, "P1N32396", true);

    auto run_pre_ids = [](const std::string& pfx) {
        return std::vector<std::string>{pfx + "-R1", pfx + "-R2", pfx + "-DONE", pfx + "-IDLE"};
    };
    auto cat = [](std::vector<std::string> a, const std::vector<std::string>& b) {
        a.insert(a.end(), b.begin(), b.end());
        return a;
    };

    // ==== P1: On NTSC, N+1 color clocks per bit ==========================
    // NMH_MUTATE=P1-NPLUS1 swaps bin_n0/bin_n371/bin_n32396 to the N+1
    // variants, while the grids below are still graded against N=0/371/32396.
    {
        GridResult g0 = check_grid(ntsc_n0.tx_changes, ntsc_n0.C, 0, 1);
        GridResult g371 = check_grid(ntsc_n371.tx_changes, ntsc_n371.C, 371, 4);
        GridResult g32396 = check_grid(ntsc_n32396.tx_changes, ntsc_n32396.C, 32396, 1);
        checks.note("P1 grid: N=0 matched " + std::to_string(g0.matched) + "/" + std::to_string(g0.total_expected) +
                    " of " + std::to_string(g0.total_changes) + " changes, C=" + std::to_string(ntsc_n0.C));
        checks.note("P1 grid: N=371 matched " + std::to_string(g371.matched) + "/" + std::to_string(g371.total_expected) +
                    " of " + std::to_string(g371.total_changes) + " changes, C=" + std::to_string(ntsc_n371.C));
        checks.note("P1 grid: N=32396 matched " + std::to_string(g32396.matched) + "/" + std::to_string(g32396.total_expected) +
                    " of " + std::to_string(g32396.total_changes) + " changes, C=" + std::to_string(ntsc_n32396.C));
        checks.note("P1 deviation of each change from t0+k*(N+1)*C: N=0: " + grid_deviation_note(ntsc_n0.tx_changes, ntsc_n0.C, 0, 1) +
                    "; N=371: " + grid_deviation_note(ntsc_n371.tx_changes, ntsc_n371.C, 371, 4) +
                    "; N=32396: " + grid_deviation_note(ntsc_n32396.tx_changes, ntsc_n32396.C, 32396, 1) +
                    "; the observation resolution is " + std::to_string(kObsRes) + " cycle");
        const std::vector<std::string> p371 = {"1a", "1b", "1c", "1d", "1e"};
        checks.line("P1", cat(cat(p371, run_pre_ids("P1N0")), run_pre_ids("P1N32396")),
            g0.ok && g371.ok && g32396.ok,
            "[p255] \"N+1 color clocks occur ... between transmissions of output bits\": on "
            "NTSC, with SERPER=N, every TxD change lands within one cycle of t0+k*(N+1)*C for "
            "k=0..10W-1, and there is no other change, for N=0 (W=1), N=371 (W=4) and N=32396 (W=1)");
        checks.record(
            "sol:8-serper.c8 [p255]: withdrawn from the graded line above -- \"the number N\" "
            "covers every 15-bit divisor, and N=0, 371 and 32396 sample three of them, not the "
            "whole domain; the N=371 grid above is red (words 2-4 do not land within one cycle of t0+k*372*C), "
            "which stands as the counterexample the claim's every-N reading would have to survive");

        // Controls, graded on the same comparator: P1's own N=371 run is
        // red, which the AND above hides behind N=0 and N=32396's own
        // passing grids. Each is its own line.
        checks.line("P1-CTRL-N0", run_pre_ids("P1N0"), g0.ok,
            "[p255] control: N=0 (W=1) alone, same grid check");
        checks.line("P1-CTRL-N32396", run_pre_ids("P1N32396"), g32396.ok,
            "[p255] control: N=32396 (W=1) alone, same grid check");
        bool within_word_ok = check_within_word(ntsc_n371.tx_changes, ntsc_n371.C, 371, 4);
        checks.line("P1-CTRL-WITHIN", p371, within_word_ok,
            "[p255] control: the within-word intervals of the N=371 NTSC run (the "
            "9 bit-to-bit gaps inside each word, excluding the stop-to-next-start boundary) all "
            "differ from T=(N+1)*C by less than one cycle");
    }

    // ---- P2 preconditions 2a-2e: 1a-1e restated on the PAL run, and the
    // per-run preconditions of the PAL single-word runs ----
    {
        checks.note("2a: " + r1_note(pal_n371.r1, false));
        bool ok = r1_ok(pal_n371.r1, false);
        checks.line("2a", {}, ok, "[p194,p195] R1 anchor on the PAL N=371 run: the four refresh "
            "accesses in every line are 2C, 2C and 2C apart, and every candidate refresh group "
            "has that same shape (Figure 6-9), none discarded before grading");

        bool pal_ok = pal_n371.field_lines == 312 || pal_n371.field_lines == 313;
        checks.line("2b", {}, pal_ok,
            "[p24] R2: the PAL capture's own refresh-group count is 312 or 313 (measured " +
            std::to_string(pal_n371.field_lines) + ")");

        auto m0 = mark_cycle(pal_n371, 0);
        checks.line("2c", {"FIXTURE-PAL"}, m0.has_value() &&
                      std::none_of(pal_n371.tx_changes.begin(), pal_n371.tx_changes.end(),
                                   [&](uint64_t c) { return c < *m0; }),
            "[p257] TxD idle before MARK=0, PAL N=371 run");

        bool done2 = mark_cycle(pal_n371, kMarkDone).has_value();
        checks.note("2e: POLL_TIMEOUT=" + std::to_string(pal_n371.poll_timeout));
        checks.line("2e", {"FIXTURE-PAL"}, done2 && !pal_n371.poll_timeout,
            "[G4] run completed, no poll (G4's POLLBIT, any of them) ended at its own bound, "
            "PAL N=371 run");

        double T = 372.0 * pal_n371.C;
        bool d_ok = true;
        for (int k = 2; k <= 4; ++k) {
            auto mk = mark_cycle(pal_n371, uint16_t(k));
            if (!mk) { d_ok = false; continue; }
            double bound = double(pal_n371.tx_changes.empty() ? 0 : pal_n371.tx_changes[0]) +
                           double(10 * (k - 1) - 1) * T;
            if (!(double(*mk) < bound)) d_ok = false;
        }
        checks.line("2d", {"FIXTURE-PAL", "2a", "2e"}, d_ok, "[p257,p283] words sent back to back, PAL N=371 run");
    }
    check_run_preconditions(pal_n0, "P2N0", false);
    check_run_preconditions(pal_n32396, "P2N32396", false);

    // ==== P2: On PAL, the bit time is one fixed period times N+1 =========
    {
        UniformResult u0 = check_uniform(pal_n0.tx_changes, pal_n0.C, 1);
        UniformResult u371 = check_uniform(pal_n371.tx_changes, pal_n371.C, 4);
        UniformResult u32396 = check_uniform(pal_n32396.tx_changes, pal_n32396.C, 1);
        checks.note("P2 uniform grid: N=0 T_N=" + std::to_string(u0.T_N) + " ok=" + std::to_string(u0.ok) +
                    "; N=371 T_N=" + std::to_string(u371.T_N) + " ok=" + std::to_string(u371.ok) +
                    "; N=32396 T_N=" + std::to_string(u32396.T_N) + " ok=" + std::to_string(u32396.ok) +
                    "; changes on the line through the first and last change within the observation resolution: " +
                    std::to_string(u0.matched) + "/10, " + std::to_string(u371.matched) + "/40, " +
                    std::to_string(u32396.matched) + "/10");

        // (ii) the same period in every run: U_N = T_N/(N+1), pairwise tolerance.
        struct RunU { double U; int W; int N; };
        std::vector<RunU> us = {
            {u0.T_N / 1.0, 1, 0}, {u371.T_N / 372.0, 4, 371}, {u32396.T_N / 32397.0, 1, 32396}
        };
        bool pair_ok = true;
        std::string detail;
        for (size_t i = 0; i < us.size(); ++i)
            for (size_t j = i + 1; j < us.size(); ++j) {
                double tol = kObsRes * (1.0 / (double(10 * us[i].W - 1) * double(us[i].N + 1)) +
                                            1.0 / (double(10 * us[j].W - 1) * double(us[j].N + 1)));
                double diff = std::abs(us[i].U - us[j].U);
                bool ok = diff < tol;
                if (!ok) pair_ok = false;
                detail += " |U" + std::to_string(us[i].N) + "-U" + std::to_string(us[j].N) + "|=" +
                          std::to_string(diff) + "(tol " + std::to_string(tol) + ")";
            }
        checks.note("P2(ii) pairwise U comparison:" + detail);

        const std::vector<std::string> p2371 = {"2a", "2b", "2c", "2d", "2e"};
        const auto p2single = cat(run_pre_ids("P2N0"), run_pre_ids("P2N32396"));

        // Control: the (N=0, N=32396) pair alone, excluding N=371. P2 is red
        // through clause (i) at N=371 (22 of 40 changes on its line) and
        // through clause (ii) in the pair (N=371, N=32396); the pair (N=0,
        // N=32396) agrees, and this control runs the same pairwise comparison
        // on it.
        {
            double tol_pair = kObsRes * (1.0 / (double(10 * us[0].W - 1) * double(us[0].N + 1)) +
                                             1.0 / (double(10 * us[2].W - 1) * double(us[2].N + 1)));
            double diff_pair = std::abs(us[0].U - us[2].U);
            checks.line("P2-CTRL-PAIR", cat({"2a"}, p2single), diff_pair < tol_pair,
                "[p255] control: the (N=0, N=32396) pair alone agrees within "
                "the stated tolerance (diff=" + std::to_string(diff_pair) + ", tol=" +
                std::to_string(tol_pair) + ")");
        }

        // (i) and (ii) together are what establishes "N+1 times one fixed
        // period, the same for every bit and for every N" -- (i) alone
        // fits each run's own period whatever it is, and only (ii) grades
        // the N+1 relation across runs, so both are graded on one line.
        checks.line("P2", cat(p2371, p2single), u0.ok && u371.ok && u32396.ok && pair_ok,
            "[p255] \"N+1 color clocks occur ... between transmissions of output bits\": on "
            "PAL, (i) each run's own uniform grid (T_N=span/(10W-1)) accounts for every TxD "
            "change with no other change, and (ii) every pair of runs' own U_N=T_N/(N+1) agree "
            "within the stated tolerance, for N=0, N=371 and N=32396");

        // The fixed period against one PAL colour clock: the pages give 1
        // (p.255) and 0.991 (p.284), so it is measured, not graded.
        double ratio0 = us[0].U / pal_n371.C, ratio371 = us[1].U / pal_n371.C, ratio32396 = us[2].U / pal_n371.C;
        checks.note("P2 U/C: " + std::to_string(ratio0) + " (N=0), " + std::to_string(ratio371) +
                    " (N=371), " + std::to_string(ratio32396) + " (N=32396) -- p.255 predicts 1, "
                    "p.284 predicts 0.991, so it is not graded");

        checks.record(
            "sol:8-serper.c9 [p255]: withdrawn from the graded lines above -- \"the number N\" "
            "covers every 15-bit divisor, and N=0, 371 and 32396 sample three of them, not the "
            "whole domain; the N=371 pairwise comparison above is red (|U371-U32396|=" +
            std::to_string(std::abs(us[1].U - us[2].U)) + " exceeds its own tolerance), which "
            "stands as the counterexample the claim's every-N reading would have to survive");
    }

    // ==== P3: SERPER=371 on NTSC is the setting p.255 calls 9600 baud =====
    // Reuses ntsc_n371 (P1's own run); preconditions 3a-3e are 1a-1e.
    {
        GridResult g = check_grid(ntsc_n371.tx_changes, ntsc_n371.C, 371, 4);
        checks.note("P3: reusing P1's N=371 NTSC run, matched " + std::to_string(g.matched) + "/" +
                    std::to_string(g.total_expected));
        checks.line("P3", {"1a", "1b", "1c", "1d", "1e"}, g.ok,
            "[p255] \"N+1 color clocks occur ... between transmissions of output bits\": "
            "every change of the N=371 NTSC run differs from t0+k*372*C by less than one cycle for k=0..39");
        checks.record(
            "sol:8-serper.c10 [p255]: withdrawn from the graded line above -- the claim reads 371 "
            "as the proper NTSC setting for 9600 baud, but the rate = 3,579,545/372 = 9622.4 "
            "bit/s; the page's own arithmetic (3,579,545/9600-1=371.87, printed 371) makes its "
            "\"9600 baud\" actually 9622 bit/s, so 9600 itself is not graded (only the p.255 "
            "N+1 grid above is), and that grid is red at N=371 (words 2-4 do not land within one cycle of "
            "t0+k*372*C)");

        checks.record("3a-3e [p194-p257]: preconditions 1a-1e, already graded above on this same "
                       "N=371 NTSC run (P3's own stimulus and observation)");
    }

    // ==== P4: ADKCON FAST sets the disk bit cell to 2us or 4us ============
    run_p4(dir, mv, checks);
    checks.record(
        "4d DISKWRITE clause [p248]: withdrawn from the graded lines 4d-*, which grade DMAON only -- "
        "\"DISKWRITE The disk write bit (in DSKLEN) is enabled\" has no contrast here: a stimulus that "
        "makes it false is a disk-read transfer, whose fetch window exists only when a disk image is "
        "served, and the harness's fixture mounts no image and answers no sector request (nanomig.v "
        "exposes the ports). The value is printed on each 4d MEASURED line.");

    // ==== P5: the nine-bit receive setting takes eight data bits plus a
    // parity bit ============================================================
    run_p5(dir, mv, checks);

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
