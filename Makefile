# 設定模組與檔名
TOPNAME = counter
NXPRJ_PATH ?= $(shell pwd)
VERILATOR_INPUT = rtl/$(TOPNAME).v sim/tb_counter.cpp

default:
	verilator --cc --exe --build -j 0 -Wall $(VERILATOR_INPUT) --top-module $(TOPNAME)

sim: default
	./obj_dir/V$(TOPNAME)

clean:
	rm -rf obj_dir