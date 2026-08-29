#include <cstdio>

#include "verilated.h"
#include "verilated_vcd_c.h"
#include "Vregfile.h"
#include <random>

void clock(Vregfile *top)
{
    top->clk = 0;
    top->eval();

    top->clk = 1;
    top->eval();
}

bool check(
    Vregfile *top, 
    int pattern,
    uint8_t raddr1,
    uint8_t raddr2,
    uint32_t expected1,
    uint32_t expected2
)
{
    bool pass = (top->rdata1 == expected1 && top->rdata2 == expected2);
    if (pass){
        printf("[Pattern.%d PASS!] x%u=0x%08X, x%u=0x%08X\n", pattern, raddr1, top->rdata1, raddr2, top->rdata2);
    }
    else{
        printf("[Pattern.%d FAIL!] x%u=0x%08X, expected x%u=0x%08X, x%u=0x%08X, expected x%u=0x%08X\n", pattern, raddr1, top->rdata1, raddr1, expected1, raddr2, top->rdata2, raddr2, expected2);
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

bool read(Vregfile *top, int pattern, uint8_t addr1, uint8_t addr2){

    top->raddr1 = addr1;
    top->raddr2 = addr2;
    top->eval();
    uint32_t expected1 = ref_read(addr1);
    uint32_t expected2 = ref_read(addr2);
    //check(top, addr1, addr2, expected1, expected2);
    if (check(top, pattern, addr1, addr2, expected1, expected2)) {
        return true;
    } 
    return false;
}

int main(int argc, char **argv)
{
    Verilated::commandArgs(argc, argv);

    Vregfile *top = new Vregfile;

    Verilated::traceEverOn(true);

    VerilatedVcdC *tf = new VerilatedVcdC;
    top->trace(tf, 99);
    tf->open("regfile.vcd");

    std::mt19937 rng(1);
    int patterns = 50000;
    int pass_cnt = 0;
    int fail_cnt = 0;

    //TODO:
    for (int i = 0; i < patterns; i++) {
    //reset:
    top->rst = 1;
    top->we = 0;
    ref_rst();
    clock(top);
    top->rst = 0;
    top->eval();

    //write:
    uint8_t test_add1 = rng() % 32;
    uint32_t test_data1 = static_cast<uint32_t>(rng());
    uint8_t test_add2 = rng() % 32;
    uint32_t test_data2 = static_cast<uint32_t>(rng());      
    write(top, test_add1, test_data1);
    write(top, test_add2, test_data2);
    //read:
    //read(top, test_add1, test_add2);
    if (read(top, i, test_add1, test_add2)) {
        pass_cnt++;
    } else {
        fail_cnt++;
    }

    //write:
    test_data1 = static_cast<uint32_t>(rng());
    write(top, test_add1, test_data1);
    //read:
    if (read(top, i, test_add1, test_add2)) {
        pass_cnt++;
    } else {
        fail_cnt++;
    }

    //write:
    test_data1 = static_cast<uint32_t>(rng());    
    write(top, 0, test_data1);
    //read:   
    if (read(top, i, 0, test_add2)) {
        pass_cnt++;
    } else {
        fail_cnt++;
    }
    
    //write when we = 0: 
    test_data2 = static_cast<uint32_t>(rng());  
    no_write(top, test_add2, test_data2);
    //read:   
    if (read(top, i, test_add1, test_add2)) {
        pass_cnt++;
    } else {
        fail_cnt++;
    }   
    }

    printf("=== Simulation Result (ˋvˊ) ===\n Total Tests: %d\n Passed: %d\n Failed: %d\n", patterns, pass_cnt, fail_cnt);
    if (fail_cnt == 0) {
        printf("All tests passed!\n");
    } else {
        printf("YOU failed, HAHA!\n");
    }
    
    tf->close();
    delete tf;

    delete top;
    return 0;

}