TOP = counter

run:
	verilator --cc rtl/$(TOP).v \
		--exe sim/tb_$(TOP).cpp \
		--build \
		--trace \
		--top-module $(TOP)

	./obj_dir/V$(TOP)

clean:
	rm -rf obj_dir waveform.vcd