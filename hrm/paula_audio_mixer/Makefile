all: test_paula_audio_mixer

test_paula_audio_mixer:
	rm -rf obj_dir
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_audio_mixer -Wall -Wno-fatal -Wno-pinmissing -Wno-lint ../../../src/minimig-aga/paula_audio_mixer.v ../../../src/minimig-aga/paula_audio_volume.v test_paula_audio_mixer.cpp
	./obj_dir/Vpaula_audio_mixer

clean:
	rm -rf obj_dir
