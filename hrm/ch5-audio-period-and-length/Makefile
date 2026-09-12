# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

VERILATOR = verilator
# --public-flat-rw: this package reads the period/length/volume counter
# internals directly (percnt, percntrld, percount, perfin, lencnt,
# lencntrld, lenfin, volcnt, volcntrld, audper, audvol, audlen, datbuf,
# auddat, penhi, pbufld1, silence), none of which are top-level ports.
# Same flag, same reasoning, as this chapter's sibling package
# hrm/ch5-audio-channel-state on this same fixture.
VFLAGS = --public-flat-rw --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio_channel -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
CFLAGS_EXTRA ?=

SRC = $(NANOMIG)/src/minimig-aga/paula_audio_channel.v
TEST = test_ch5_audio_period_and_length.cpp

all: test

obj_dir/Vpaula_audio_channel: $(SRC) $(TEST)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TEST) $(if $(CFLAGS_EXTRA),-CFLAGS "$(CFLAGS_EXTRA)")

test: obj_dir/Vpaula_audio_channel
	./obj_dir/Vpaula_audio_channel

clean:
	rm -rf obj_dir
