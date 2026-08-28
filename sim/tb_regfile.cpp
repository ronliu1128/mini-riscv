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
    bool pass = (top->rdata1 == expected1 && top->rdata2 == expected2);

    if (pass){
        printf("[PASS!] x%u=%u, x%u=%u\n", top->raddr1, top->rdata1, top->raddr2, top->rdata2);
    }
    else{
        printf("[FAIL!] x%u=%u, expected x%u=%u, x%u=%u, expected x%u=%u\n", top->raddr1, top->rdata1, top->raddr1, expected1, top->raddr2, top->rdata2, top->raddr2, expected2);
    }
    return pass;

    }

uint32_t ref_regs[32];

void ref_rst()
{
    for(int i = 0; i < 32; i++){
        ref_regs[i]=0;
    }
}

void ref_write(uint8_t addr, uint32_t data)
{
    if (addr != 0){
        ref_regs[addr]=data;
    }
}

uint32_t ref_read(uint8_t addr)
{
        if(addr == 0){
            return 0;
        }    
            return ref_regs[addr];
}

void write(Vregfile *top, uint8_t addr, uint32_t data){
    top->we = 1;
    top->waddr = addr;
    top->wdata = data;
    clock(top);
    ref_write(addr,data);
    top->we = 0;
}

void no_write(Vregfile *top, uint8_t addr, uint32_t data){
    top->we = 0;
    top->waddr = addr;
    top->wdata = data;
    clock(top);
    //ref_write(addr,data);
}

void read(Vregfile *top, uint8_t addr1, uint8_t addr2){

    top->raddr1 = addr1;
    top->raddr2 = addr2;
    top->eval();
    uint32_t expected1 = ref_read(addr1);
    uint32_t expected2 = ref_read(addr2);
    check(top, addr1, addr2, expected1, expected2);

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
    ref_rst();
    clock(top);
    top->rst = 0;
    top->eval();

    //write x3:
    write(top, 3, 125);
    //read x3:
    printf("=== Simulation Result (ˋvˊ) ===\n");
    read(top, 3, 0);

    //write x10:
    write(top, 10, 99);
    //read x3 & x10:
    read(top, 3, 10);

    //write x0:
    write(top, 0, 100);
    //read x0:   
    read(top, 0, 10);
    
    //write x10 when we = 0: 
    no_write(top, 10, 888);
    //read x10:   
    read(top, 0, 10);   

    tf->close();
    delete tf;

    delete top;
    return 0;

}