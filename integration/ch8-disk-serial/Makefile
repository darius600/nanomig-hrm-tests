# ch8-disk-serial: serial-port timing, nine-bit receive and disk bit-cell
# rate. See SPECIFICATION.md.

TEST_CPP := test_ch8_disk_serial.cpp
PROGS    := \
  prog_tx_n0 prog_tx_n371 prog_tx_n32396 \
  prog_tx_n0_plus1 prog_tx_n371_plus1 prog_tx_n32396_plus1 \
  prog_tx_n371_1c prog_tx_n371_1d prog_tx_n371_1e \
  prog_tx_n0_brk prog_tx_n32396_brk prog_tx_n0_stall prog_tx_n32396_stall \
  prog_tx_n371_nomark3 \
  prog_fast0 prog_fast1 prog_fast1_4c prog_fast1_4d prog_fast1_4e \
  prog_fast0_4c prog_fast0_4d prog_fast0_4e prog_fast1_stall prog_fast0_stall prog_fast1_4f prog_fast0_4f \
  prog_rx_a prog_rx_b prog_rx_a_5b prog_rx_a_5c prog_rx_a_5d prog_rx_a_5e \
  prog_rx_a_altper prog_rx_b_5c prog_rx_b_5d prog_rx_b_5e \
  prog_rx_a_stall prog_rx_b_stall \
  prog_fast1_once prog_fast0_once \
  prog_tx_n0_r1 prog_tx_n371_r1 prog_tx_n32396_r1 prog_fast1_r1 prog_fast0_r1 \
  prog_rx_a_r1 prog_rx_b_r1

include ../harness/nanomig.mk
