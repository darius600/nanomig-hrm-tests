# NanoMig checkout location; default is this repo's submodule.
NANOMIG ?= ../../nanomig

TOP := audio_dma_first_pass_rig
SRC := audio_dma_first_pass_rig.v \
       $(NANOMIG)/src/minimig-aga/paula_audio.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_channel.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_mixer.v \
       $(NANOMIG)/src/minimig-aga/paula_audio_volume.v \
       $(NANOMIG)/src/minimig-aga/agnus_audiodma.v
TEST_SRC := test_ch5_audio_dma_first_pass.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -Wno-UNUSED -Wno-WIDTH

.PHONY: all test build clean

all: test

build: $(TEST_BIN)

$(TEST_BIN): $(SRC) $(TEST_SRC) Makefile
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(SRC) $(TEST_SRC)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
