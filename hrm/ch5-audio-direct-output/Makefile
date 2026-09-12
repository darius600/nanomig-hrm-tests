# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

VERILATOR = verilator
# --public-flat-rw: this package observes the audio state machine's internal
# signals (audio_state, AUDxON, AUDxIP, AUDxIR, AUDxDAT, perfin), none of
# which are top-level ports -- a prior agent used this same flag
# successfully on this module (see hrm/ch5-audio-channel-state, and
# hrm/paula_audio_channel's own trace.vcd for precedent).
VFLAGS = --public-flat-rw --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio_channel -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
CFLAGS_EXTRA ?=

SRC = $(NANOMIG)/src/minimig-aga/paula_audio_channel.v
TEST = test_ch5_audio_direct_output.cpp

all: test

obj_dir/Vpaula_audio_channel: $(SRC) $(TEST)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TEST) $(if $(CFLAGS_EXTRA),-CFLAGS "$(CFLAGS_EXTRA)")

test: obj_dir/Vpaula_audio_channel
	./obj_dir/Vpaula_audio_channel

clean:
	rm -rf obj_dir
