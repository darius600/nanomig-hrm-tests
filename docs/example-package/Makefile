# The same Makefile every package in hrm/ uses, with the paths pointing one
# level further out because this example lives in docs/ rather than hrm/.
#
# It lives outside hrm/ deliberately: run_all.sh and check_published_counts.py
# both count the directories under hrm/, and an example is not a package.

NANOMIG ?= ../../nanomig

TOP := ciaa
SRC := $(NANOMIG)/src/minimig-aga/ciaa.v \
       $(NANOMIG)/src/minimig-aga/cia_int.v \
       $(NANOMIG)/src/minimig-aga/cia_timera.v \
       $(NANOMIG)/src/minimig-aga/cia_timerb.v \
       $(NANOMIG)/src/minimig-aga/cia_timerd.v
TEST_SRC := test_example.cpp
OBJ_DIR := obj_dir
TEST_BIN := $(OBJ_DIR)/V$(TOP)

VERILATOR ?= verilator
FLAGS := --no-timing --cc --exe --build -j 0 -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

.PHONY: all test build mutate clean

all: test

build:
	$(VERILATOR) $(FLAGS) --top-module $(TOP) $(SRC) $(TEST_SRC) -o V$(TOP)

test: build
	./$(TEST_BIN)

# Prove the assertion can go red. A green here means the expect() is not
# measuring what it claims to.
mutate: build
	MUTATE=1 ./$(TEST_BIN) || true

clean:
	rm -rf $(OBJ_DIR)
