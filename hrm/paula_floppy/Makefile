# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

all: test_paula_floppy

# `test` is an ALIAS, added for O67. This package answered only to
# `all: test_paula_floppy`, so a sweep running `make test` got "No rule to make
# target" and the package read as having no graded output rather than as
# having failed to run. The alias adds no recipe and changes no output. Four
# packages named their target after their module; all four answer to `make
# test` now.
.PHONY: all test test_paula_floppy clean

test: test_paula_floppy

test_paula_floppy:
	rm -rf obj_dir
	verilator --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_floppy -Wall -Wno-fatal -Wno-pinmissing -Wno-lint $(NANOMIG)/src/minimig-aga/paula_floppy.v $(NANOMIG)/src/minimig-aga/paula_floppy_fifo.v $(NANOMIG)/src/minimig-aga/paula_floppy_ebr.v $(NANOMIG)/src/minimig-aga/paula_floppy_dma_buf.v test_paula_floppy.cpp
	./obj_dir/Vpaula_floppy

clean:
	rm -rf obj_dir
