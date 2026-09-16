# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

TOP = agnus_copper
RTL = $(NANOMIG)/src/minimig-aga/agnus_copper.v
TEST_SRC = test_ch8-appb-copper-access-marks.cpp
OBJ_DIR = obj_dir
TEST_BIN = $(OBJ_DIR)/V$(TOP)

VERILATOR = verilator
FLAGS = --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal

MUT ?=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test clean

all: test

# rm -rf here, not only on the binary's own rule: since MUT is not a
# prerequisite of $(TEST_BIN), `make MUT=X test` would otherwise silently
# re-run a stale binary built under a different (or no) MUT and report its
# baseline result as the mutation's.
test:
	rm -rf $(OBJ_DIR)
	$(VERILATOR) $(FLAGS) --Mdir $(OBJ_DIR) --top-module $(TOP) $(RTL) $(TEST_SRC)
	./$(TEST_BIN)

clean:
	rm -rf $(OBJ_DIR)
