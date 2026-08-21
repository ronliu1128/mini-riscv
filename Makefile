#define variable
TOP = regfile
RTL_DIR = rtl
SIM_DIR = sim
BUILD_DIR = obj_dir
VCD_FILE = $(TOP).vcd

all: run

run:
	verilator -Wall --trace --cc $(RTL_DIR)/$(TOP).sv \
		--exe $(SIM_DIR)/tb_$(TOP).cpp \
		--top-module $(TOP) \
		--Mdir $(BUILD_DIR) \
		--build -j

	./$(BUILD_DIR)/V$(TOP)

clean:
	rm -rf $(BUILD_DIR) $(VCD_FILE)

.PHONY: all run clean