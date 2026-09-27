# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = paula
SRC_DIR = $(NANOMIG)/src/minimig-aga
VERILOG_SOURCES = $(SRC_DIR)/paula.v $(SRC_DIR)/paula_uart.v $(SRC_DIR)/paula_intcontroller.v \
                   $(SRC_DIR)/paula_floppy.v $(SRC_DIR)/paula_floppy_fifo.v \
                   $(SRC_DIR)/paula_audio.v $(SRC_DIR)/paula_audio_channel.v \
                   $(SRC_DIR)/paula_audio_mixer.v $(SRC_DIR)/paula_audio_volume.v
VERILATOR = verilator
VERILATOR_FLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(SRC_DIR)
CPP_TEST = test_$(TOP_MODULE).cpp

.PHONY: all test clean

all: test

test: $(CPP_TEST) $(VERILOG_SOURCES)
	rm -rf obj_dir
	$(VERILATOR) $(VERILATOR_FLAGS) $(VERILOG_SOURCES) $(CPP_TEST)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
