#include <cstdio>
#include <cstdint>

#include "verilated.h"
//#include "verilated_vcd_c.h"
#include "Valu.h"

void check(
    Valu *top, 
    uint32_t a,
    uint32_t b,
    uint8_t op,
    uint32_t expected,
    bool expected_zero
)
{
    top->a = a;
    top->b = b;
    top->op = op;
    
    top->eval();

    if (top->result!= expected || top->zero != expected_zero) {
        printf("[FAIL] op=%u, result=%u, expected=%u, zero=%u, expected_zero=%u\n", op, top->result, expected, top->zero, expected_zero);
    }
    else {
        printf("[PASS] op=%u, result=%u, expected=%u, zero=%u, expected_zero=%u\n", op, top->result, expected, top->zero, expected_zero);
    }
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    Valu *top = new Valu;

    //Verilated::traceEverOn(true);

    //VerilatedVcdC* tf = new VerilatedVcdC;

    //top->trace(tf, 99);

    //tf->open("wave.vcd");   

    check(top, 10, 3, 0, 13, false);
    check(top, 10, 3, 1, 7, false);
    check(top, 10, 3, 2, 2, false);
    check(top, 10, 3, 3, 11, false);
    check(top, 10, 3, 4, 9, false);

    delete top;

    return 0;
}

