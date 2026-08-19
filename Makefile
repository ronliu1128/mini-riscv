TOP = alu

run:
	verilator --cc rtl/$(TOP).sv \
		--exe sim/tb_$(TOP).cpp \
		--build \
		--trace \
		--top-module $(TOP)

	./obj_dir/V$(TOP)

clean:
	rm -rf obj_dir waveform.vcd