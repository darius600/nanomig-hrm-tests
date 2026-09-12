# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

VERILATOR = verilator
# Top-level ports (clk, clk7_en, cck, reset, aen, dmaena, reg_address_in[3:1],
# data[15:0], intpen, strhor, sample[7:0], sample_okk[7:0], intreq, dmareq,
# dmas, volume[6:0]) cover most of this package's assertions. --public-flat-rw
# is added for one internal signal, `lencnt`: the port-level dmas/intreq
# signals turned out to have latching/self-clear semantics (read from the RTL
# during the build stage -- dmas is recomputed on every idle re-assertion of
# AUDxDR even without new data, and AUDxIR/intreq2 self-clear on the very next
# period-timer transition) that make them unreliable as a "one edge per
# completed AUDxLEN block" marker from outside the module. `lencnt` itself is
# unambiguous: it is the register AUDxLEN's value is loaded into and counted
# down, and a jump back up to the just-loaded value is the block boundary
# itself, not an inference from a derived pulse. A prior agent used this same
# flag successfully on this module (see hrm/ch5-audio-channel-state's own
# Makefile).
VFLAGS = --public-flat-rw --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio_channel -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
CFLAGS_EXTRA ?=

SRC = $(NANOMIG)/src/minimig-aga/paula_audio_channel.v
TEST = test_ch5_audio_channel_registers.cpp

all: test

obj_dir/Vpaula_audio_channel: $(SRC) $(TEST)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TEST) $(if $(CFLAGS_EXTRA),-CFLAGS "$(CFLAGS_EXTRA)")

test: obj_dir/Vpaula_audio_channel
	./obj_dir/Vpaula_audio_channel

clean:
	rm -rf obj_dir
