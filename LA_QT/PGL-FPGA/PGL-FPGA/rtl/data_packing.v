module data_packing(
    input          sys_clk    ,  //系统时钟
    input          sys_rst_n  ,  //系统复位
    //第一路ADC
    input   [4:0]  current_state_1,//当前状态
    input   [9:0]  ad_data_reg_1  /* synthesis PAP_MARK_DEBUG="true" */,  //第一路ADC数据
    input          ad_otr_1   /* synthesis PAP_MARK_DEBUG="true" */,  //第一路ADC输入电压超过量程标志
    input          data_ready_1,
    output  reg[7:0]  ad_data_H_1  /* synthesis PAP_MARK_DEBUG="true" */,  //第一路ADC数据
    output  reg[7:0]  ad_data_L_1  /* synthesis PAP_MARK_DEBUG="true" */,  //第一路ADC数据
    output  reg    fifo_wr_flag
   );

parameter [2:0] REAL_SIGN_H=3'b000;
parameter [2:0] REAL_SIGN_L=3'b000;
parameter [2:0] EQUI_SIGN_H=3'b000;
parameter [2:0] EQUI_SIGN_L=3'b000;

reg [9:0] test_data;

reg         data_ready_10;
reg         data_ready_11;
assign      data_ready_1_pos = data_ready_10&&(!data_ready_11);
assign      data_ready_1_neg = (!data_ready_10)&&data_ready_11;

always @(posedge sys_clk or negedge sys_rst_n )begin
	if(!sys_rst_n)begin
        data_ready_10 <= 0;
        data_ready_11 <= 0;
    end
    else begin
        data_ready_10 <= data_ready_1;
        data_ready_11 <= data_ready_10;
	end
end

always @(posedge sys_clk or negedge sys_rst_n )begin
	if(!sys_rst_n)begin
        ad_data_H_1 <= 0;
        ad_data_L_1 <= 0;
        fifo_wr_flag <= 0;
        test_data <= 0;
    end
    else if(data_ready_1_pos)begin
        ad_data_H_1<={REAL_SIGN_H[2:0],ad_data_reg_1[9:5]};
        ad_data_L_1<={REAL_SIGN_L[2:0],ad_data_reg_1[4:0]};
//        ad_data_H_1<={REAL_SIGN_H[2:0],test_data[9:5]};
//        ad_data_L_1<={REAL_SIGN_L[2:0],test_data[4:0]};
//        test_data <= test_data+1;
        fifo_wr_flag <= 1;
	end
    else begin
        ad_data_H_1<=ad_data_H_1;
        ad_data_L_1<=ad_data_L_1;
        fifo_wr_flag <= 0;
	end
end

endmodule