all: test_paula_uart

test_paula_uart:
	rm -rf obj_dir
	verilator --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_uart -Wall -Wno-fatal -Wno-pinmissing -Wno-lint ../../../src/minimig-aga/paula_uart.v test_paula_uart.cpp
	./obj_dir/Vpaula_uart

clean:
	rm -rf obj_dir
