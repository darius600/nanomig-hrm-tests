# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

VERILATOR = verilator
VFLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio_volume -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

SRC = $(NANOMIG)/src/minimig-aga/paula_audio_volume.v
TEST = test_paula_audio_volume.cpp

all: test

obj_dir/Vpaula_audio_volume: $(SRC) $(TEST)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TEST)

test: obj_dir/Vpaula_audio_volume
	./obj_dir/Vpaula_audio_volume

clean:
	rm -rf obj_dir
