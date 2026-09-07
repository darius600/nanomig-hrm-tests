# Wo liegt der NanoMig-Checkout? Standard ist das Submodul in diesem Repo.
NANOMIG ?= ../../nanomig

all: test_minimig_system test_copper

test_minimig_system: test_minimig_system.cpp
	verilator --public-flat-rw --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module test_minimig_system_top -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(NANOMIG)/src/minimig-aga test_minimig_system_top.v ide_dpram.v test_minimig_system.cpp
	./obj_dir/Vtest_minimig_system_top

test_copper: test_copper.cpp
	verilator --public-flat-rw --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir_copper --top-module test_minimig_system_top -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I$(NANOMIG)/src/minimig-aga test_minimig_system_top.v ide_dpram.v test_copper.cpp
	./obj_dir_copper/Vtest_minimig_system_top

clean:
	rm -rf obj_dir
