# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

all: test_paula_audio_mixer

test_paula_audio_mixer:
	rm -rf obj_dir
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio_mixer -Wall -Wno-fatal -Wno-pinmissing -Wno-lint $(NANOMIG)/src/minimig-aga/paula_audio_mixer.v $(NANOMIG)/src/minimig-aga/paula_audio_volume.v test_paula_audio_mixer.cpp
	./obj_dir/Vpaula_audio_mixer

clean:
	rm -rf obj_dir
