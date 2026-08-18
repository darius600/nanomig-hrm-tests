TOP_MODULE = agnus_refresh
SRC = ../../../src/minimig-aga/$(TOP_MODULE).v

test: $(TOP_MODULE)
	./obj_dir/V$(TOP_MODULE)

$(TOP_MODULE): test_$(TOP_MODULE).cpp $(SRC)
	verilator --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module $(TOP_MODULE) -Wall -Wno-fatal $(SRC) test_$(TOP_MODULE).cpp

clean:
	rm -rf obj_dir
