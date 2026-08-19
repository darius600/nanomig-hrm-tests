VERILATOR = verilator
VFLAGS = --no-timing --cc --exe --build -j 0 --Mdir obj_dir --top-module paula_intcontroller -Wall -Wno-fatal -Wno-pinmissing -Wno-lint

SRC = ../../../src/minimig-aga/paula_intcontroller.v
TEST = test_paula_intcontroller.cpp

all: test

obj_dir/Vpaula_intcontroller: $(SRC) $(TEST)
	rm -rf obj_dir
	$(VERILATOR) $(VFLAGS) $(SRC) $(TEST)

test: obj_dir/Vpaula_intcontroller
	./obj_dir/Vpaula_intcontroller

clean:
	rm -rf obj_dir
