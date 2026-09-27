# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

all: test_paula_audio_mixer

# `test` is an ALIAS for the module-named target, so a sweep running `make
# test` builds and runs this package instead of getting "No rule to make
# target" and reading it as having no graded output rather than as having
# failed to run. The alias adds no recipe and changes no output.
.PHONY: all test test_paula_audio_mixer clean

test: test_paula_audio_mixer

test_paula_audio_mixer:
	rm -rf obj_dir
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio_mixer -Wall -Wno-fatal -Wno-pinmissing -Wno-lint $(NANOMIG)/src/minimig-aga/paula_audio_mixer.v $(NANOMIG)/src/minimig-aga/paula_audio_volume.v test_paula_audio_mixer.cpp
	./obj_dir/Vpaula_audio_mixer

clean:
	rm -rf obj_dir
