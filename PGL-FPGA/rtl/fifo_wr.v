module fifo_wr(
    input                  sys_clk            ,  // ʱ���ź�
    input                  sys_rst_n          ,  // ��λ�ź�
    input          [2:0]   fifo_state         ,  // fifo״̬
    input                  fifo_wr_flag       ,  // ʱ���ź�
    input                  fifo_rd_flag       ,  // ��λ�ź�
    input          [7:0]   adc_data_H         ,  // д�뵽FIFO������
    input          [7:0]   adc_data_L         ,  // д�뵽FIFO������
    input          [10:0]  fifo_wr_data_count /* synthesis PAP_MARK_DEBUG="true" */,  //FIFOдʱ��������ݼ���
    input                  fifo_empty         ,  // FIFO���ź�
    output    reg          fifo_wr_en         ,  // FIFOдʹ��
    output    reg  [9:0]   fifo_wr_data       ,  // д��FIFO������
    output    reg  [9:0]   WR_200x2_cnt/* synthesis PAP_MARK_DEBUG="true" */
);

//reg define
reg  [3:0]   delay_cnt;
parameter    delay_max  =  5;

parameter    POINT_NUM  =  400;
parameter    DATA_NUM  =  POINT_NUM*2;

parameter    FIFO_IDLE  =  3'b111;
parameter    WR_200x2   =  3'b011;
parameter    WR_RD_LOOP =  3'b001;
parameter    RD_400x2   =  3'b101;

reg  [7:0]   test_data;

always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)begin
        fifo_wr_en <= 1'b0;
        delay_cnt  <= 1'b0;
        WR_200x2_cnt <= 0;
        test_data <= 0;
    end
    else begin
		case(fifo_state)
			FIFO_IDLE  : begin
                fifo_wr_en <= 1'b0;
//                WR_200x2_cnt <= 0;
			end
			WR_200x2   : begin
                if(fifo_wr_flag)begin
                    fifo_wr_en <= 1'b1;
                    delay_cnt <= delay_cnt+1;
//                    fifo_wr_data <= adc_data_H;

                    fifo_wr_data <= test_data;
                    test_data <= test_data+1;

                    WR_200x2_cnt <= WR_200x2_cnt+1;
                end
                else if(delay_cnt>=delay_max)begin
                    fifo_wr_en <= 1'b1;
                    delay_cnt <= 1'b0;
//                    fifo_wr_data <= adc_data_L;

                    fifo_wr_data <= test_data;
                    test_data <= test_data+1;

                    WR_200x2_cnt <= WR_200x2_cnt+1;
                end
                else if(delay_cnt)begin
                    fifo_wr_en <= 1'b0;
                    delay_cnt <= delay_cnt+1;
                end
                else if(WR_200x2_cnt<DATA_NUM/2)begin
                    fifo_wr_en <= 1'b0;
                end
                else if(WR_200x2_cnt>=DATA_NUM/2)begin
                    fifo_wr_en <= 1'b0;
                    WR_200x2_cnt<= 0;
                end
			end
            WR_RD_LOOP : begin
                if(fifo_wr_flag)begin
                    fifo_wr_en <= 1'b1;
                    delay_cnt <= delay_cnt+1;
//                    fifo_wr_data <= adc_data_H;

                    fifo_wr_data <= test_data;
                    test_data <= test_data+1;

                end
                else if(delay_cnt>=delay_max)begin
                    fifo_wr_en <= 1'b1;
                    delay_cnt <= 1'b0;
//                    fifo_wr_data <= adc_data_L;

                    fifo_wr_data <= test_data;
                    test_data <= test_data+1;

                end
                else if(delay_cnt)begin
                    fifo_wr_en <= 1'b0;
                    delay_cnt <= delay_cnt+1;
                end
                else begin
                    fifo_wr_en <= 1'b0;
                    WR_200x2_cnt<= 0;
                end
            end
            RD_400x2   : begin
                fifo_wr_en <= 1'b0;
//                WR_200x2_cnt <= 0;
            end
            default: begin
                fifo_wr_en <= 1'b0;	
//                WR_200x2_cnt <= 0;			        
			end
        endcase
    end
end

endmodule