# Where is the NanoMig checkout? Default is the submodule in this repo.
NANOMIG ?= ../../nanomig

# Pass MUT=<NAME> to build one mutation variant, e.g. `make MUT=MUT_STOPBIT_LOW`.
# Unset/empty MUT builds the unmutated (baseline) binary. Every variant is
# built and run the same way (`make` / `make MUT=<NAME>`), and
# `test_paula_uart` is not a real file this recipe ever creates (the binary
# lands at obj_dir/Vpaula_uart), so make always reruns it -- no stale-binary
# risk.
MUT ?=
FLAGS :=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test test_paula_uart clean

all: test_paula_uart

# `test` is an ALIAS, added for O67. This package answered only to
# `all: test_paula_uart`, so a sweep running `make test` got "No rule to make
# target" and the package read as having no graded output rather than as
# having failed to run. The alias adds no recipe and changes no output. Four
# packages named their target after their module; all four answer to `make
# test` now.
test: test_paula_uart

test_paula_uart:
	rm -rf obj_dir
	verilator --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_uart -Wall -Wno-fatal -Wno-pinmissing -Wno-lint $(FLAGS) $(NANOMIG)/src/minimig-aga/paula_uart.v test_paula_uart.cpp
	./obj_dir/Vpaula_uart

clean:
	rm -rf obj_dir
