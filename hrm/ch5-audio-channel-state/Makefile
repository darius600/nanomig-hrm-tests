# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

VERILATOR = verilator
# --public-flat-rw: this package observes the audio state machine itself
# (audio_state, dmasen, lencntrld, lencount, lenfin, perfin, percnt, ...),
# none of which are top-level ports -- a prior agent used this same flag
# successfully on this module (see hrm/paula_audio_channel's own trace.vcd
# for precedent; this package needs read/write access, not just a trace).
VFLAGS = --public-flat-rw --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio_channel -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
CFLAGS_EXTRA ?=

SRC = $(NANOMIG)/src/minimig-aga/paula_audio_channel.v
TEST = test_ch5_audio_channel_state.cpp

all: test

obj_dir/Vpaula_audio_channel: $(SRC) $(TEST)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TEST) $(if $(CFLAGS_EXTRA),-CFLAGS "$(CFLAGS_EXTRA)")

test: obj_dir/Vpaula_audio_channel
	./obj_dir/Vpaula_audio_channel

clean:
	rm -rf obj_dir
