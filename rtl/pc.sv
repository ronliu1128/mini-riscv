module pc(
    input   logic           clk,
    input   logic           rst,
    
    output  logic   [31:0]  pc
);

logic [31:0] next_pc;
//logic [31:0]inst;

always_ff @(posedge clk) begin 
    if (rst)
        pc <= 32'd0;
    else
        pc <= next_pc;
end

always_comb begin 
    next_pc = pc + 32'd4;  
end

endmodule
