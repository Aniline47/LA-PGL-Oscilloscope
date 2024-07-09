module spi_drive(
    input            sys_clk       ,
	input            sys_rst_n     ,

	//spi interface
	input  [7:0]     spi_data      ,
	output reg       fifo_rd_flag  ,

	input            spi_cs        ,//SPI从机的片选信号，低电平有效。
	input            spi_clk       ,//主从机之间的数据同步时钟。
	input            spi_mosi      ,//数据引脚，主机输出，从机输入。
	output reg       spi_miso       //数据引脚，主机输入，从机输出。
);

parameter IDLE = 0;
parameter TRANSFORM = 1;

parameter DATA_LEN = 8;
parameter POINT_NUM  =  400;
parameter DATA_NUM  =  POINT_NUM*2;
parameter DATA_BIT_TOTAL = DATA_LEN * DATA_NUM;

//wire define
wire spi_clk_pos0;
wire spi_clk_pos2;
wire spi_clk_neg;
wire spi_cs_pos;
wire spi_cs_neg0;
wire spi_cs_neg2;

//reg define
reg[3:0]  current_state ;
reg[3:0]  next_state    ;
reg[DATA_LEN-1:0]  r_data         ;/* synthesis PAP_MARK_DEBUG="true" */
reg[DATA_LEN-1:0 ] data_buffer    ;/* synthesis PAP_MARK_DEBUG="true" */
reg[31:0] bit_cnt       ;
reg       stdone        ;
reg       spi_cs0       ;
reg       spi_cs1       ;
reg       spi_cs2       ;
reg       spi_cs3       ;
reg       spi_clk0      ;
reg       spi_clk1      ;
reg       spi_clk2      ;
reg       spi_clk3      ;

assign spi_clk_pos0 = spi_clk0&&~spi_clk1;
assign spi_clk_pos2 = spi_clk2&&~spi_clk3;
assign spi_clk_neg = ~spi_clk0&&spi_clk1;
assign spi_cs_pos = spi_cs0&&~spi_cs1;
assign spi_cs_neg0 = ~spi_cs0&&spi_cs1;
assign spi_cs_neg2 = ~spi_cs1&&spi_cs3;

//*****************************************************
//**数据交换
//*****************************************************
always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		fifo_rd_flag <= 0;
    end
    else if(spi_cs_neg0||((bit_cnt%DATA_LEN==0)&&spi_clk_pos2))begin
        fifo_rd_flag <= 1;
	end
    else begin
        fifo_rd_flag <= 0;
	end
end

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		spi_cs0 <= 0;
        spi_cs1 <= 0;
		spi_cs2 <= 0;
        spi_cs3 <= 0;
    end
    else begin
        spi_cs0 <= spi_cs;
        spi_cs1 <= spi_cs0;
        spi_cs2 <= spi_cs1;
        spi_cs3 <= spi_cs2;
	end
end

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		spi_clk0 <= 0;
        spi_clk1 <= 0;
		spi_clk2 <= 0;
        spi_clk3 <= 0;
    end
    else if(current_state==TRANSFORM)begin
        spi_clk0 <= spi_clk;
        spi_clk1 <= spi_clk0;
        spi_clk2 <= spi_clk;
        spi_clk3 <= spi_clk0;
	end

end

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		data_buffer<=0;
    end
	else if(spi_cs_neg2==1&&current_state==TRANSFORM)
        data_buffer<=spi_data;
	else if((bit_cnt%DATA_LEN==0)&&spi_clk_neg&&bit_cnt<DATA_BIT_TOTAL)begin
        data_buffer<=spi_data;
    end
	else if(current_state==TRANSFORM&&spi_clk_pos0==1)begin
        data_buffer<={data_buffer[DATA_LEN-2:0],spi_mosi};
    end
	else
		data_buffer<=data_buffer;
end

//*****************************************************
//**计数器
//*****************************************************

always @(negedge sys_clk or negedge sys_rst_n )begin//字节计数器
	if(!sys_rst_n)
		bit_cnt<=31'd0;
	else if(current_state==TRANSFORM&&spi_clk_pos0)
		bit_cnt<=bit_cnt+1'd1;
	else if(current_state==IDLE)
		bit_cnt<=1'd0;
	else
		bit_cnt<=bit_cnt;
end
	
//*****************************************************
//**状态判断
//*****************************************************
always @(negedge sys_clk or negedge sys_rst_n )begin
	if(!sys_rst_n)
		current_state<=IDLE;
	else
		current_state<=next_state;
end

always@(*)begin
    case(current_state)
        IDLE:begin
            if(spi_cs_neg0)begin
                next_state = TRANSFORM;
            end
        end
        TRANSFORM:begin
            if(spi_cs_pos||stdone)begin
                next_state = IDLE;
            end
        end
    default: next_state=IDLE;
    endcase
end

always @(posedge sys_clk or negedge sys_rst_n )begin
	if(!sys_rst_n) begin
		spi_miso<=1'b0;	
        r_data<=0;
		stdone<=1'b0;
	end
	else begin
		case(current_state)
			IDLE: begin
				spi_miso<=1'b0;
                stdone<=1'b0;
			end
            TRANSFORM: begin
                stdone<=1'b0;
                 if((bit_cnt%DATA_LEN==0)&&spi_clk_neg&&bit_cnt<DATA_BIT_TOTAL)begin//read
				    r_data<=data_buffer;
			     end
    			 if((bit_cnt%DATA_LEN==0)&&spi_clk_neg&&bit_cnt<DATA_BIT_TOTAL)begin//write
    				spi_miso<=spi_data[DATA_LEN-1];
    			 end
    			 else if(spi_clk_neg&&bit_cnt<DATA_BIT_TOTAL)begin						
    				spi_miso<=data_buffer[DATA_LEN-1];
    			 end
    			 else if(bit_cnt>=DATA_BIT_TOTAL) begin
    				spi_miso<=1'b0;
                    stdone<=1'b1;
    			 end
                 else begin
                    spi_miso<=spi_miso;
                    stdone<=stdone;
    			 end
             end
             default: begin
				 spi_miso<=spi_miso;				        
			end
         endcase
	end
end


endmodule