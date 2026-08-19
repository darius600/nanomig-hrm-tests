all: test_paula_floppy

test_paula_floppy:
	rm -rf obj_dir
	verilator --trace --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_floppy -Wall -Wno-fatal -Wno-pinmissing -Wno-lint ../../../src/minimig-aga/paula_floppy.v ../../../src/minimig-aga/paula_floppy_fifo.v ../../../src/minimig-aga/paula_floppy_ebr.v ../../../src/minimig-aga/paula_floppy_dma_buf.v test_paula_floppy.cpp
	./obj_dir/Vpaula_floppy

clean:
	rm -rf obj_dir
