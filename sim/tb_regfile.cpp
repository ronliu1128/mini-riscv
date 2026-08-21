#include <cstdio>

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vregfile.h"

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);

    Vregfile *top = new Vregfile;

    Verilated::traceEverOn(true);

    VerilatedVcdC *tf = new VerilatedVcdC;
    top->trace(tf, 99);
    tf->open("regfile.vcd");

    //TODO:
    top->rst = 1;
    top->we = 0;

    top->clk = 0;
    top->eval();

    top->clk = 1;
    top->eval();

    top->rst = 0;

    top->raddr1 = 0;
    top->raddr2 = 5;
    top->eval();

    printf("=== Simulation Result (ˋvˊ) ===\n");
    printf("x0=%u\n", top->rdata1);
    printf("x5=%u\n", top->rdata2);
  
    tf->close();
    delete tf;

    delete top;

    return 0;

}