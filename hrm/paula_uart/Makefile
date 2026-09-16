# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

# Pass MUT=<NAME> to build one mutation variant, e.g. `make MUT=MUT_STOPBIT_LOW`.
# Unset/empty MUT builds the unmutated (baseline) binary. This package has
# no `test` target of its own -- `all: test_paula_uart` is its own
# convention, unchanged here; every variant is still built and run the
# same way (`make` / `make MUT=<NAME>`), and `test_paula_uart` is not a
# real file this recipe ever creates (the binary lands at
# obj_dir/Vpaula_uart), so make always reruns it -- no stale-binary risk.
MUT ?=
FLAGS :=
ifneq ($(MUT),)
FLAGS += -CFLAGS -D$(MUT)=1
endif

.PHONY: all test_paula_uart clean

all: test_paula_uart

test_paula_uart:
	rm -rf obj_dir
	verilator --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_uart -Wall -Wno-fatal -Wno-pinmissing -Wno-lint $(FLAGS) $(NANOMIG)/src/minimig-aga/paula_uart.v test_paula_uart.cpp
	./obj_dir/Vpaula_uart

clean:
	rm -rf obj_dir
