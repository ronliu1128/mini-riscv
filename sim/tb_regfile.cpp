#include <cstdio>

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vregfile.h"

void clock(Vregfile *top)
{
    top->clk = 0;
    top->eval();

    top->clk = 1;
    top->eval();
}

bool check(
    Vregfile *top, 
    uint8_t raddr1,
    uint8_t raddr2,
    uint32_t expected1,
    uint32_t expected2
)
{
    top->raddr1 = raddr1;
    top->raddr2 = raddr2;
    top->eval();

    bool pass = (top->rdata1 == expected1 && top->rdata2 == expected2);

    if (pass){
        printf("[PASS!] x%u=%u, x%u=%u\n", top->raddr1, top->rdata1, top->raddr2, top->rdata2);
    }
    else{
        printf("[FAIL!] x%u=%u, expected x%u=%u, x%u=%u, expected x%u=%u\n", top->raddr1, top->rdata1, top->raddr1, expected1, top->raddr2, top->rdata2, top->raddr2, expected2);
    }
    return pass;

    }

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);

    Vregfile *top = new Vregfile;

    Verilated::traceEverOn(true);

    VerilatedVcdC *tf = new VerilatedVcdC;
    top->trace(tf, 99);
    tf->open("regfile.vcd");

    //TODO:

    //reset:
    top->rst = 1;
    top->we = 0;
    clock(top);

    top->rst = 0;
    top->eval();

    //write x3:
    top->we = 1;
    top->waddr = 3;
    top->wdata = 125;
    clock(top); 

    //read x3:
    top->raddr1 = 3;
    top->eval();
    check(top, 3, 0, 125, 0);

    //write x10:
    top->we = 1;
    top->waddr = 10;
    top->wdata = 999;
    clock(top); 

    //read x3 & x10:

    top->raddr2 = 10;
    top->eval(); 
    check(top, 3, 10, 125, 999);

    //write x0:
    top->we = 1;
    top->waddr = 0;
    top->wdata = 100;
    clock(top); 

    //read x0:   
    top->raddr1 = 0;
    top->eval();
    check(top, 0, 10, 0, 999);
    
    //write x10 when we = 0:   
    top->we = 0;
    top->waddr = 10;
    top->wdata = 888;
    clock(top);   
    
    //read x10:   
    check(top, 0, 10, 0, 999);   

    //printf("=== Simulation Result (ˋvˊ) ===\n");
    //check(top, 0, 3, 0, 124);

    tf->close();
    delete tf;

    delete top;

    return 0;

}