all: test_minimig_system

test_minimig_system:
	verilator --public-flat-rw --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module test_minimig_system_top -Wall -Wno-fatal -Wno-pinmissing -Wno-lint -I../../../src/minimig-aga test_minimig_system_top.v ide_dpram.v test_minimig_system.cpp
	./obj_dir/Vtest_minimig_system_top

clean:
	rm -rf obj_dir
