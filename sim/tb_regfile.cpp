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
    uint32_t expected2)
{
    bool pass = (top->rdata1 == expected1 && top->rdata2 == expected2);
    if (pass)
    {
        printf("[Pattern.%d PASS!] x%u=0x%08X, x%u=0x%08X\n", pattern, raddr1, top->rdata1, raddr2, top->rdata2);
    }
    else
    {
        printf("[Pattern.%d FAIL!] x%u=0x%08X, expected x%u=0x%08X, x%u=0x%08X, expected x%u=0x%08X\n", pattern, raddr1, top->rdata1, raddr1, expected1, raddr2, top->rdata2, raddr2, expected2);
    }
    return pass;
}

uint32_t ref_regs[32];

void ref_rst()
{
    for (int i = 0; i < 32; i++)
    {
        ref_regs[i] = 0;
    }
}

void ref_write(uint8_t addr, uint32_t data)
{
    if (addr != 0)
    {
        ref_regs[addr] = data;
    }
}

uint32_t ref_read(uint8_t addr)
{
    if (addr == 0)
    {
        return 0;
    }
    return ref_regs[addr];
}

void write(Vregfile *top, uint8_t addr, uint32_t data)
{
    top->we = 1;
    top->waddr = addr;
    top->wdata = data;
    clock(top);
    ref_write(addr, data);
    top->we = 0;
}

void no_write(Vregfile *top, uint8_t addr, uint32_t data)
{
    top->we = 0;
    top->waddr = addr;
    top->wdata = data;
    clock(top);
    // ref_write(addr,data);
}

bool read(Vregfile *top, int pattern, uint8_t addr1, uint8_t addr2)
{

    top->raddr1 = addr1;
    top->raddr2 = addr2;
    top->eval();
    uint32_t expected1 = ref_read(addr1);
    uint32_t expected2 = ref_read(addr2);
    // check(top, addr1, addr2, expected1, expected2);
    if (check(top, pattern, addr1, addr2, expected1, expected2))
    {
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

    std::mt19937 rng(100);
    std::uniform_int_distribution<u_int8_t> dist_addr(0, 31);
    std::uniform_int_distribution<u_int32_t> dist_data(0, UINT32_MAX);

    int patterns_direct = 50000, patterns_random = 50000;
    int check_cnt = 0, pass_cnt = 0, fail_cnt = 0;
    int write_cnt = 0, no_write_cnt = 0, read_cnt = 0;

    // TODO:
    for (int i = 0; i < patterns_direct; i++)
    {
        // reset:
        top->rst = 1;
        top->we = 0;
        ref_rst();
        clock(top);
        top->rst = 0;
        top->eval();

        // write:
        uint8_t test_add1 = dist_addr(rng);
        uint32_t test_data1 = dist_data(rng);
        uint8_t test_add2 = dist_addr(rng);
        uint32_t test_data2 = dist_data(rng);
        write(top, test_add1, test_data1);
        write(top, test_add2, test_data2);

        // read:
        if (read(top, i, test_add1, test_add2))
        {
            pass_cnt++;
        }
        else
        {
            fail_cnt++;
        }
        check_cnt++;

        // write:
        test_data1 = dist_data(rng);
        write(top, test_add1, test_data1);

        // read:
        if (read(top, i, test_add1, test_add2))
        {
            pass_cnt++;
        }
        else
        {
            fail_cnt++;
        }
        check_cnt++;

        // write:
        test_data1 = dist_data(rng);
        write(top, 0, test_data1);

        // read:
        if (read(top, i, 0, test_add2))
        {
            pass_cnt++;
        }
        else
        {
            fail_cnt++;
        }
        check_cnt++;

        // write when we = 0:
        test_data2 = dist_data(rng);
        no_write(top, test_add2, test_data2);

        // read:
        if (read(top, i, test_add1, test_add2))
        {
            pass_cnt++;
        }
        else
        {
            fail_cnt++;
        }
        check_cnt++;

        // read same address:
        if (read(top, i, test_add1, test_add1))
        {
            pass_cnt++;
        }
        else
        {
            fail_cnt++;
        }
        check_cnt++;

        // write edge case:
        test_add1 = 1;
        test_data1 = dist_data(rng);
        test_add2 = 31;
        test_data2 = dist_data(rng);
        write(top, test_add1, test_data1);
        write(top, test_add2, test_data2);

        // read edge case:
        if (read(top, i, test_add1, test_add2))
        {
            pass_cnt++;
        }
        else
        {
            fail_cnt++;
        }
        check_cnt++;
    }

    // random Transactions test:
    for (int i = patterns_direct; i < patterns_direct + patterns_random; i++)
    {
        int test_type = rng() % 5; // 0:write 1:no_write 2:read
        uint8_t test_add1 = dist_addr(rng);
        uint8_t test_add2 = dist_addr(rng);
        uint32_t test_data1 = dist_data(rng);

        switch (test_type)
        {
        case 0:
            write(top, test_add1, test_data1);
            write_cnt++;
            break;
        case 1:
            no_write(top, test_add1, test_data1);
            no_write_cnt++;
            break;
        default:
            if (read(top, i, test_add1, test_add2))
            {
                pass_cnt++;
            }
            else
            {
                fail_cnt++;
            }
            check_cnt++;
            read_cnt++;
            break;
        }
    }

    printf("=== Random Transaction Summary (ˋvˊ) ===\n");
    printf("WRITE: %d\nNO_WRITE: %d\nREAD: %d\n", write_cnt, no_write_cnt, read_cnt);

    printf("=== Simulation Result (ˋvˊ) ===\n");
    printf("Directed Patterns: %d\nRandom Transactions: %d\nRead Checks: %d\n", patterns_direct, patterns_random, check_cnt);
    printf("Passed: %d\nFailed: %d\n", pass_cnt, fail_cnt);

    if (fail_cnt == 0)
    {
        printf("All tests passed!\n");
    }
    else
    {
        printf("YOU failed, HAHA!\n");
    }

    tf->close();
    delete tf;

    delete top;
    return 0;
}