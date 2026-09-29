# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

TOP_MODULE = paula
SRC_DIR = $(NANOMIG)/src/minimig-aga
# paula.v instantiates paula_uart, paula_intcontroller, paula_floppy and
# paula_audio directly; paula_audio in turn instantiates four
# paula_audio_channel instances and one paula_audio_mixer, which itself
# instantiates four paula_audio_volume instances. All seven submodule
# files are listed here.
VERILOG_SOURCES = $(SRC_DIR)/paula.v \
                   $(SRC_DIR)/paula_uart.v \
                   $(SRC_DIR)/paula_intcontroller.v \
                   $(SRC_DIR)/paula_floppy.v \
                   $(SRC_DIR)/paula_audio.v \
                   $(SRC_DIR)/paula_audio_channel.v \
                   $(SRC_DIR)/paula_audio_mixer.v \
                   $(SRC_DIR)/paula_audio_volume.v
VERILATOR = verilator
VERILATOR_FLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -Wno-pinmissing -Wno-lint
VERILATOR_FLAGS += -I$(SRC_DIR)
CPP_TEST = test_$(TOP_MODULE).cpp

# MUT=<NAME> replays one guarded mutation (see the "// MUTATION <NAME> ..."
# lines in test_paula.cpp's own header) by defining MUT_<NAME>=1 for the
# build, reproducing the exact stimulus/oracle change that assertion's log
# entry describes. obj_dir is always removed before building (below), so
# no separate `make clean` is needed when switching MUT values.
MUTFLAGS := $(if $(MUT),-CFLAGS -DMUT_$(MUT)=1,)

.PHONY: all test clean

all: test

test: $(CPP_TEST) $(VERILOG_SOURCES)
	rm -rf obj_dir
	$(VERILATOR) $(VERILATOR_FLAGS) $(MUTFLAGS) $(VERILOG_SOURCES) $(CPP_TEST)
	./obj_dir/V$(TOP_MODULE)

clean:
	rm -rf obj_dir
