module fifo_rd(
    input                  sys_clk            ,  //ʱ���ź�
    input                  sys_rst_n          ,  // ��λ�ź�
    input          [2:0]   fifo_state         ,  // fifo״̬
    input                  fifo_wr_flag       ,  //ʱ���ź�
    input                  fifo_rd_flag       ,  //��λ�ź�
    input          [10:0]  fifo_rd_data_count /* synthesis PAP_MARK_DEBUG="true" */,  //FIFO��ʱ��������ݼ���
    input          [7:0]   fifo_rd_data       ,  //��FIFO����������
    input                  fifo_full          ,  //FIFO���ź�
    output    reg          fifo_rd_en         ,  //FIFO��ʹ��
    output         [7:0]   spi_data           ,  //����spi������
    output    reg  [9:0]   RD_400x2_cnt/* synthesis PAP_MARK_DEBUG="true" */
);
//wire define
assign       spi_data = fifo_rd_data;

//reg define
reg  [3:0]   delay_cnt;
parameter    delay_max  =  5;

parameter    POINT_NUM  =  400;
parameter    DATA_NUM  =  POINT_NUM*2;

parameter    FIFO_IDLE  =  3'b111;
parameter    WR_200x2   =  3'b011;
parameter    WR_RD_LOOP =  3'b001;
parameter    RD_400x2   =  3'b101;

//��fifo_rd_en���и�ֵ,FIFOд��֮��ʼ��������֮��ֹͣ��

always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)begin
        fifo_rd_en <= 1'b0;
        delay_cnt  <= 1'b0;
        RD_400x2_cnt <= 0;	
    end
    else begin
		case(fifo_state)
			FIFO_IDLE  : begin
                if(fifo_rd_data_count > 11'd1)
                    fifo_rd_en <= 1'b1;
                else if(fifo_rd_data_count == 11'd1&&fifo_rd_en == 1)
                    fifo_rd_en <= 1'b0;
                else if(fifo_rd_data_count == 11'd1&&fifo_rd_en == 0)
                    fifo_rd_en <= 1'b1;
                else
                    fifo_rd_en <= 0;
			end
			WR_200x2   : begin
				fifo_rd_en <= 1'b0;	
//                RD_400x2_cnt <= 0;	
			end
            WR_RD_LOOP : begin
                if(fifo_wr_flag)begin
                    fifo_rd_en <= 1'b1;
                    delay_cnt <= delay_cnt+1;
                end
                else if(delay_cnt>=delay_max)begin
                    fifo_rd_en <= 1'b1;
                    delay_cnt <= 1'b0;
                end
                else if(delay_cnt)begin
                    fifo_rd_en <= 1'b0;
                    delay_cnt <= delay_cnt+1;
                end
                else begin
                    fifo_rd_en <= 1'b0;
                    RD_400x2_cnt <= 0;
                end
            end
            RD_400x2   : begin
                if(fifo_rd_flag)begin
                    fifo_rd_en <= 1'b1;
                    RD_400x2_cnt <= RD_400x2_cnt+1;   
                end
                else if(RD_400x2_cnt<DATA_NUM)begin
                    fifo_rd_en <= 1'b0;  
                end
                else if(RD_400x2_cnt>=DATA_NUM)begin
                    fifo_rd_en <= 1'b0;
                    RD_400x2_cnt <= 0;
                end
            end
            default    : begin
				fifo_rd_en <= 1'b0;	
                RD_400x2_cnt <= 0;		        
			end
        endcase
    end
end

endmodule