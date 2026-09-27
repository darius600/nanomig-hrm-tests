# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

VERILATOR = verilator
VFLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

# paula_audio.v instantiates paula_audio_channel (x4) and paula_audio_mixer,
# which itself instantiates paula_audio_volume -- all four are needed for
# Verilator to elaborate this fixture, matching hrm/ch5-audio-stereo-routing/'s own
# Makefile pattern of listing every file the top module pulls in.
SRC = $(NANOMIG)/src/minimig-aga/paula_audio.v \
      $(NANOMIG)/src/minimig-aga/paula_audio_channel.v \
      $(NANOMIG)/src/minimig-aga/paula_audio_mixer.v \
      $(NANOMIG)/src/minimig-aga/paula_audio_volume.v
TEST = test_paula_audio.cpp

all: test

obj_dir/Vpaula_audio: $(SRC) $(TEST)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TEST)

test: obj_dir/Vpaula_audio
	./obj_dir/Vpaula_audio

clean:
	rm -rf obj_dir
