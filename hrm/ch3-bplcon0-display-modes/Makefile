NANOMIG ?= ../../nanomig
TOP_MODULE = denise
SRC = $(NANOMIG)/src/minimig-aga/$(TOP_MODULE).v

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE) > out.txt
	cat out.txt

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --trace --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal -y $(dir $(SRC)) $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir out.txt trace.vcd
