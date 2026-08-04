#include <cstdio>
#include "verilated.h"
#include "Vcounter.h"
#include "verilated_vcd_c.h"   // <-- 新增

vluint64_t sim_time = 0;

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vcounter* top = new Vcounter;

    Verilated::traceEverOn(true);

    VerilatedVcdC* tf = new VerilatedVcdC;

    top->trace(tf, 99);

    tf->open("wave.vcd");

    top->rst = 1;

    for (int i = 0; i < 10; i++) {
        top->clk = 0;
        top->eval();

        tf->dump(sim_time);
        sim_time += 5;

        top->clk = 1;
        top->eval();

        tf->dump(sim_time);
        sim_time += 5;

        if (i == 2) {
            top->rst = 0;
        }

        printf("count = %d\n", top->count);
    }
    
    tf->close();
    delete tf;
    delete top;
    return 0;
}