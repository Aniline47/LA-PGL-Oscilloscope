module ip_fifo(
    input          sys_clk     ,  // 时钟信号
    input          sys_rst_n   ,  // 复位信号

    input          trig_flag   ,
	input          single_flag ,
	input          equi_flag   ,

    input          fifo_wr_flag,  // 写信号
    input          fifo_rd_flag,  // 读信号

    input   [7:0]  adc_data_H,    // 写入到FIFO的数据
    input   [7:0]  adc_data_L,    // 写入到FIFO的数据
    output  [7:0]  spi_data,      // 从FIFO读出的数据
    output         spi_data_ready/* synthesis PAP_MARK_DEBUG="true" */,
    input   [1:0]  meas_state,
    input   [4:0]  time_state
);

reg       trig_flag0;
reg       trig_flag1;
wire      trig_flag_pos;
assign    trig_flag_pos=trig_flag0&&(!trig_flag1)/* synthesis PAP_MARK_DEBUG="true" */;
always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        trig_flag0 <= 0;
        trig_flag1 <= 0;
    end
    else begin
        trig_flag0 <= trig_flag;
        trig_flag1 <= trig_flag0;
	end
end

//wire define
assign       spi_data = fifo_rd_data;

//wire define
wire         almost_full        ; // FIFO将满信号
wire         almost_empty       ; // FIFO将空信号
wire  [10:0] fifo_wr_data_count ; // FIFO写时钟域的数据计数
wire  [10:0] fifo_rd_data_count ; // FIFO读时钟域的数据计数

//reg define
reg  [3:0]   delay_cnt;
parameter    delay_max  =  5;
reg   [4:0]  current_state      /* synthesis PAP_MARK_DEBUG="true" */;
reg   [4:0]  next_state         /* synthesis PAP_MARK_DEBUG="true" */;
reg   [9:0]  WR_200x2_cnt       ; // FIFO写时钟域的数据计数
reg   [9:0]  RD_400x2_cnt       ; // FIFO读时钟域的数据计数
reg          fifo_wr_en         ;
reg          fifo_rd_en         ;
reg   [7:0]  fifo_wr_data       ;
wire  [7:0]  fifo_rd_data       ;

parameter    POINT_NUM  =  400;
parameter    DATA_NUM  =  POINT_NUM*2;

parameter    RUN    =   2'b00;
parameter    SINGLE =   2'b01;
parameter    PAUSE  =   2'b10;

parameter    FIFO_IDLE  =  5'b11110;
parameter    WR_200x2   =  5'b11100;
parameter    WR_RD_LOOP =  5'b11000;
parameter    RD_400x2   =  5'b11010;

parameter    DIFFERENT  =  606;//405
assign   spi_data_ready = (current_state == RD_400x2);
//*****************************************************
//**                    main code
//*****************************************************
always @(negedge sys_clk or negedge sys_rst_n )begin
	if(!sys_rst_n)
		current_state<=FIFO_IDLE;
	else
		current_state<=next_state;
end

always@(*)begin
    case(current_state)
        FIFO_IDLE  :begin
            if(fifo_rd_data_count>=1)begin
                next_state = FIFO_IDLE;
            end
            else if(meas_state==RUN&&time_state<=5'b01101)begin
                next_state = WR_200x2;
            end
            else if(meas_state==RUN&&time_state>5'b01101&&equi_flag)begin
                next_state = WR_200x2;
            end
            else if(meas_state==SINGLE&&single_flag)begin
                next_state = WR_200x2;
            end
        end
        WR_200x2   :begin
            if(time_state<=5'b01101&&WR_200x2_cnt>=DATA_NUM/2)begin
                next_state = WR_RD_LOOP;
            end
            else if(fifo_wr_data_count<=DIFFERENT&&time_state>5'b01101&&WR_200x2_cnt>=DATA_NUM/2)begin
                next_state = WR_200x2;
            end
            else if(fifo_wr_data_count>DIFFERENT&&time_state>5'b01101&&WR_200x2_cnt>=DATA_NUM/2)begin
                next_state = RD_400x2;
            end
        end
        WR_RD_LOOP :begin
            if(trig_flag_pos&&fifo_wr_data_count<=DIFFERENT)begin
                next_state = WR_200x2;
            end
            else if(trig_flag_pos&&fifo_wr_data_count>DIFFERENT)begin
                next_state = RD_400x2;
            end
        end
        RD_400x2   :begin
            if(RD_400x2_cnt>=DATA_NUM)begin
                next_state = FIFO_IDLE ;
            end
        end
        default: next_state = FIFO_IDLE;
    endcase
end

//reg define
reg  [7:0]   test_data;

always @(posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)begin
        delay_cnt  <= 1'b0;
        fifo_wr_en <= 1'b0;
        WR_200x2_cnt <= 0;
        fifo_rd_en <= 1'b0;
        RD_400x2_cnt <= 0;
        test_data <= 0;
    end
    else begin
		case(current_state)
			FIFO_IDLE  : begin
                if(fifo_rd_data_count > 11'd1)begin
                    fifo_wr_en <= 1'b0;
                    fifo_rd_en <= 1'b1;
                end
                else if(fifo_rd_data_count == 11'd1&&fifo_rd_en == 1)begin
                    fifo_wr_en <= 1'b0;
                    fifo_rd_en <= 1'b0;
                end
                else if(fifo_rd_data_count == 11'd1&&fifo_rd_en == 0)begin
                    fifo_wr_en <= 1'b0;
                    fifo_rd_en <= 1'b1;
                end
                else begin
                    fifo_wr_en <= 1'b0;
                    fifo_rd_en <= 0;
                end
			end
			WR_200x2   : begin
                if(fifo_wr_flag)begin
                    fifo_wr_en <= 1'b1;
				    fifo_rd_en <= 1'b0;	
                    delay_cnt <= delay_cnt+1;

                    fifo_wr_data <= adc_data_H;

//                    fifo_wr_data <= test_data;
//                    test_data <= test_data+1;

                    WR_200x2_cnt <= WR_200x2_cnt+1;
                end
                else if(delay_cnt>=delay_max)begin
                    fifo_wr_en <= 1'b1;
				    fifo_rd_en <= 1'b0;	
                    delay_cnt <= 1'b0;

                    fifo_wr_data <= adc_data_L;

//                    fifo_wr_data <= test_data;
//                    test_data <= test_data+1;

                    WR_200x2_cnt <= WR_200x2_cnt+1;
                end
                else if(delay_cnt)begin
                    fifo_wr_en <= 1'b0;
				    fifo_rd_en <= 1'b0;	
                    delay_cnt <= delay_cnt+1;
                end
                else if(WR_200x2_cnt<DATA_NUM/2)begin
                    fifo_wr_en <= 1'b0;
				    fifo_rd_en <= 1'b0;	
                end
                else if(WR_200x2_cnt>=DATA_NUM/2)begin
                    fifo_wr_en <= 1'b0;
				    fifo_rd_en <= 1'b0;	
                    WR_200x2_cnt<= 0;
                end
			end
            WR_RD_LOOP : begin
                if(fifo_wr_flag)begin
                    fifo_wr_en <= 1'b1;
				    fifo_rd_en <= 1'b1;	
                    delay_cnt <= delay_cnt+1;

                    fifo_wr_data <= adc_data_H;

//                    fifo_wr_data <= test_data;
//                    test_data <= test_data+1;

                end
                else if(delay_cnt>=delay_max)begin
                    fifo_wr_en <= 1'b1;
				    fifo_rd_en <= 1'b1;	
                    delay_cnt <= 1'b0;

                    fifo_wr_data <= adc_data_L;

//                    fifo_wr_data <= test_data;
//                    test_data <= test_data+1;

                end
                else if(delay_cnt)begin
                    fifo_wr_en <= 1'b0;
				    fifo_rd_en <= 1'b0;	
                    delay_cnt <= delay_cnt+1;
                end
                else begin
                    fifo_wr_en <= 1'b0;
				    fifo_rd_en <= 1'b0;	
                    WR_200x2_cnt<= 0;
                end
            end
            RD_400x2   : begin
                if(fifo_rd_flag)begin
                    fifo_wr_en <= 1'b0;
                    fifo_rd_en <= 1'b1;
                    RD_400x2_cnt <= RD_400x2_cnt+1;   
                end
                else if(RD_400x2_cnt<DATA_NUM)begin
                    fifo_wr_en <= 1'b0;
                    fifo_rd_en <= 1'b0;  
                end
                else if(RD_400x2_cnt>=DATA_NUM)begin
                    fifo_wr_en <= 1'b0;
                    fifo_rd_en <= 1'b0;
                    RD_400x2_cnt <= 0;
                end
            end
            default: begin
                fifo_wr_en <= 1'b0;	
				fifo_rd_en <= 1'b0;	
                WR_200x2_cnt <= 0;
                RD_400x2_cnt <= 0;			        
			end
        endcase
    end
end

//例化FIFO IP核
async_fifo u_async_fifo (
  .wr_clk            (sys_clk           ), // input
  .wr_rst            (~sys_rst_n        ), // input
  .wr_en             (fifo_wr_en        ), // input
  .wr_data           (fifo_wr_data      ), // input [7:0]
  .wr_full           (fifo_full         ), // output
  .wr_water_level    (fifo_wr_data_count), // output [8:0]
  .almost_full       (almost_full       ), // output
  .rd_clk            (sys_clk           ), // input
  .rd_rst            (~sys_rst_n        ), // input
  .rd_en             (fifo_rd_en        ), // input
  .rd_data           (fifo_rd_data      ), // output [7:0]
  .rd_empty          (fifo_empty        ), // output
  .rd_water_level    (fifo_rd_data_count), // output [8:0]
  .almost_empty      (almost_empty      )  // output
);

////例化写FIFO模块
//fifo_wr  u_fifo_wr(
//    .sys_clk             (sys_clk           ), // 写时钟
//    .sys_rst_n           (sys_rst_n         ), // 复位信号
//    .fifo_state          (current_state     ), // fifo状态
//    .fifo_wr_flag        (fifo_wr_flag      ), // 时钟信号
//    .fifo_rd_flag        (fifo_rd_flag      ), // 复位信号
//    .adc_data_H          (adc_data_H        ), // 写入到FIFO的数据
//    .adc_data_L          (adc_data_L        ), // 写入到FIFO的数据
//    .fifo_wr_data_count  (fifo_wr_data_count), // FIFO写时钟域的数据计数
//    .fifo_wr_en          (fifo_wr_en        ), // fifo写请求
//    .fifo_wr_data        (fifo_wr_data      ), // 写入FIFO的数据
//    .fifo_empty          (fifo_empty        ), // fifo空信号
//    .WR_200x2_cnt        (WR_200x2_cnt)
//);
//
////例化读FIFO模块
//fifo_rd  u_fifo_rd(
//    .sys_clk             (sys_clk           ), // 读时钟
//    .sys_rst_n           (sys_rst_n         ), // 复位信号
//    .fifo_state          (current_state     ), // fifo状态
//    .fifo_wr_flag        (fifo_wr_flag      ), // 时钟信号
//    .fifo_rd_flag        (fifo_rd_flag      ), // 复位信号
//    .fifo_rd_data_count  (fifo_rd_data_count), // FIFO读时钟域的数据计数
//    .fifo_rd_en          (fifo_rd_en        ), // fifo读请求
//    .fifo_rd_data        (fifo_rd_data      ), // 从FIFO输出的数据
//    .fifo_full           (fifo_full         ), // fifo满信号
//    .spi_data            (spi_data          ),
//    .RD_400x2_cnt        (RD_400x2_cnt)
//);

endmodule 