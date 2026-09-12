# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

VERILATOR = verilator
# Only top-level ports are used (clk, clk7_en, cck, reset, aen, dmaena,
# reg_address_in[3:1], data[15:0], intpen, strhor, sample[7:0],
# sample_okk[7:0], intreq, dmareq, dmas, volume[6:0]) -- all named in this
# task's own BOUNDARIES.md and reused from hrm/paula_audio_channel's own
# sibling harness, so --public-flat-rw is not needed here.
VFLAGS = --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio_channel -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
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
