#include <cstdio>
#include "verilated.h"
#include "Vcounter.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);

    Vcounter* top = new Vcounter;

    top->rst = 1;

    for (int i = 0; i < 10; i++) {
        top->clk = 0;
        top->eval();

        top->clk = 1;
        top->eval();

        if (i == 2) {
            top->rst = 0;
        }

        printf("count = %d\n", top->count);
    }

    delete top;
    return 0;
}