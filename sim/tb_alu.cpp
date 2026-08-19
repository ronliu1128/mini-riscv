#include <cstdio>
#include <cstdint>

#include "verilated.h"
//#include "verilated_vcd_c.h"
#include "Valu.h"
#include <random>

uint32_t reference_model(uint32_t a, uint32_t b, uint8_t op)
{
    switch (op) {
        case 0: return a + b;
        case 1: return a - b;
        case 2: return a & b;
        case 3: return a | b;
        case 4: return a ^ b;
        default: return 0;
    }
}

bool check(
    Valu *top, 
    uint32_t a,
    uint32_t b,
    uint8_t op,
    int patterns
)
{
    top->a = a;
    top->b = b;
    top->op = op;
    
    top->eval();

    uint32_t expected = reference_model(a, b, op);
    bool expected_zero = (expected == 0);

    if (top->result!= expected|| top->zero != expected_zero) {
        printf("[Pattern.%d FAIL] a=%u, b=%u, op=%u, result=%u, expected=%u, zero=%u, expected_zero=%u\n",patterns, a, b, op, top->result, expected, top->zero, expected_zero);
        return false;
    }
    else {
        //printf("[PASS] a=%u, b=%u, op=%u, result=%u, expected=%u, zero=%u, expected_zero=%u\n", a, b, op, top->result, expected, top->zero, expected_zero);
        return true;
    }
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    Valu *top = new Valu;

    std::mt19937 rng(12345);
    
    int patterns = 100;
    int pass_cnt = 0;
    int fail_cnt = 0;

    //Verilated::traceEverOn(true);
    //VerilatedVcdC* tf = new VerilatedVcdC;
    //top->trace(tf, 99);
    //tf->open("wave.vcd");   

    for (int i = 0; i < patterns; i++) {
        uint32_t a = rng();
        uint32_t b = rng();
        uint8_t op = rng() % 5;

        if (check(top, a, b, op, i)) {
            pass_cnt++;
        } else {
            fail_cnt++;
        }
    }

    
    printf("=== RANDOM TEST ===\n Total Tests: %d\n Passed: %d\n Failed: %d\n", patterns, pass_cnt, fail_cnt);
    if (fail_cnt == 0) {
        printf("All tests passed!\n");
    } else {
        printf("YOU failed, HAHA!\n");
    }

    delete top;

    return 0;
}

