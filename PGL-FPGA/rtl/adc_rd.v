module adc_rd(
    input          sys_clk    ,  //系统时钟
    input          sys_rst_n  ,  //系统复位

    input          trig_flag  ,  //触发信号
    input   [4:0]  time_state ,  //当前状态
    //第一路ADC

    input   [9:0]  ad_data_1  /* synthesis PAP_MARK_DEBUG="true" */,  //第一路ADC数据
    input          ad_otr_1   /* synthesis PAP_MARK_DEBUG="true" */,  //第一路ADC输入电压超过量程标志
    output  reg    Sa_clk   ,  //第一路ADC驱动时钟
    output         ad_oe_1    ,  //第一路ADC输出使能
    output  reg[7:0]  ad_data_H_1  /* synthesis PAP_MARK_DEBUG="true" */,  //第一路ADC数据
    output  reg[7:0]  ad_data_L_1  /* synthesis PAP_MARK_DEBUG="true" */,  //第一路ADC数据
    output  reg    fifo_wr_flag,
    output         equi_flag,

    //第二路ADC
    output         ad_oe_2       //第二路ADC输出使能
   );
assign             ad_oe_1 = 1'b0;
assign             ad_oe_2 = 1'b1;//失能第二路adc
reg     [9:0]      ad_data_reg_1;
reg                data_ready_1;

always @(posedge sys_clk or negedge sys_rst_n )begin
	if(!sys_rst_n)begin
        ad_data_reg_1 <= 0;
        data_ready_1 <= 0;
    end
    else if(Sa_clk)begin
        ad_data_reg_1 <= ad_data_1;
        data_ready_1 <= 1;
	end
    else begin
        data_ready_1 <= 0;
	end
end

parameter [2:0] REAL_SIGN_H=3'b000;
parameter [2:0] REAL_SIGN_L=3'b000;
parameter [2:0] EQUI_SIGN_H=3'b000;
parameter [2:0] EQUI_SIGN_L=3'b000;

//reg [9:0] test_data_1;

always @(posedge sys_clk or negedge sys_rst_n )begin
	if(!sys_rst_n)begin
        ad_data_H_1 <= 0;
        ad_data_L_1 <= 0;
        fifo_wr_flag <= 0;
//        test_data_1 <= 0;
    end
    else if(data_ready_1)begin
        ad_data_H_1<={REAL_SIGN_H[2:0],ad_data_reg_1[9:5]};
        ad_data_L_1<={REAL_SIGN_L[2:0],ad_data_reg_1[4:0]};
//        ad_data_H_1<={REAL_SIGN_H[2:0],test_data_1[9:5]};
//        ad_data_L_1<={REAL_SIGN_L[2:0],test_data_1[4:0]};
//        test_data_1 <= test_data_1+1;
        fifo_wr_flag <= 1;
	end
    else begin
        ad_data_H_1<=ad_data_H_1;
        ad_data_L_1<=ad_data_L_1;
        fifo_wr_flag <= 0;
	end
end

reg[4:0]  time_state0;

reg[23:0] clk_cnt;
reg[23:0] clk_max;

reg[23:0] dclk_cnt;
reg[23:0] dclk_max;
reg       dclk_flag;

reg[7:0]  round_cnt;
reg[7:0]  round_max;
reg       round_flag;

assign    equi_flag = round_flag;

reg[7:0]  point_cnt;
reg[7:0]  point_max;
reg       point_flag;

reg[23:0] Sa_cd_cnt;
reg[23:0] Sa_cd_max;
reg       Sa_cd_flag;

parameter ADC_F     =200000000;//200mhz
parameter REAL_F_MIN=40;//40hz
parameter REAL_F_DIV=ADC_F/REAL_F_MIN;
parameter EQUI_F_MAX=1000000;//1mhz
parameter EQUI_F_DIV=ADC_F/EQUI_F_MAX;

parameter TIME_500ms=5'b00000;
parameter TIME_200ms=5'b00001;
parameter TIME_100ms=5'b00010;
parameter TIME_50ms =5'b00011;
parameter TIME_20ms =5'b00100;
parameter TIME_10ms =5'b00101;
parameter TIME_5ms  =5'b00110;
parameter TIME_2ms  =5'b00111;
parameter TIME_1ms  =5'b01000;
parameter TIME_500us=5'b01001;
parameter TIME_200us=5'b01010;
parameter TIME_100us=5'b01011;
parameter TIME_50us =5'b01100;
parameter TIME_20us =5'b01101;//real
parameter TIME_10us =5'b01110;//equi
parameter TIME_5us  =5'b01111;
parameter TIME_2us  =5'b10000;
parameter TIME_1us  =5'b10001;
parameter TIME_500ns=5'b10010;
parameter TIME_200ns=5'b10011;
parameter TIME_100ns=5'b10100;

//*****************************************************
//**
//*****************************************************

always @(posedge sys_clk or negedge sys_rst_n )begin
	if(!sys_rst_n) begin
        clk_cnt<=0;
        clk_max<=0;
        dclk_cnt<=0;
        dclk_max<=0;
        dclk_flag<=0;
        round_cnt<=0;
        round_max<=0;
        round_flag<=0;
        point_cnt<=0;
        point_max<=0;
        point_flag<=0;
        Sa_cd_cnt<=0;
        Sa_cd_max<=0;
        Sa_cd_flag<=0;
	end
	else if(time_state0!=time_state) begin
        time_state0<=time_state;
        clk_cnt<=0;
        clk_max<=0;
        dclk_cnt<=0;
        dclk_max<=0;
        dclk_flag<=0;
        round_cnt<=0;
        round_max<=0;
        round_flag<=0;
        point_cnt<=0;
        point_max<=0;
        point_flag<=0;
        Sa_cd_cnt<=0;
        Sa_cd_max<=0;
        Sa_cd_flag<=0;
	end
	else begin
        time_state0<=time_state;
		case(time_state)
			TIME_500ms: begin
				clk_max<=REAL_F_DIV/1-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_200ms: begin
				clk_max<=REAL_F_DIV/2.5-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_100ms: begin
				clk_max<=REAL_F_DIV/5-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_50ms: begin
				clk_max<=REAL_F_DIV/10-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_20ms: begin
				clk_max<=REAL_F_DIV/25-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_10ms: begin
				clk_max<=REAL_F_DIV/50-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_5ms: begin
				clk_max<=REAL_F_DIV/100-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_2ms: begin
				clk_max<=REAL_F_DIV/250-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_1ms: begin
				clk_max<=REAL_F_DIV/500-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_500us: begin
				clk_max<=REAL_F_DIV/1000-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_200us: begin
				clk_max<=REAL_F_DIV/2500-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_100us: begin
				clk_max<=REAL_F_DIV/5000-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_50us: begin
				clk_max<=REAL_F_DIV/10000-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_20us: begin
				clk_max<=REAL_F_DIV/25000-1;
                if(clk_cnt<clk_max)begin
                    clk_cnt<=clk_cnt+1;
                    Sa_clk<=0;
                end
                else if(clk_cnt==clk_max)begin
                    clk_cnt<=0;
                    Sa_clk<=1;
                end
			end
			TIME_10us: begin
      			dclk_max<=(EQUI_F_DIV/2)*round_cnt;
    			round_max<=2;
    			point_max<=200;
    			Sa_cd_max<=EQUI_F_DIV;
                if(round_flag)begin
                    round_cnt<=0;
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                    round_flag<=0;
                end
                if(trig_flag&&point_flag)begin
        		    point_flag<=0;
                    dclk_flag<=0;
                end
                if(point_flag)begin
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                end
                else if(dclk_flag==0)begin
                    if(dclk_max==0)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt>=dclk_max)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt<dclk_max)begin
                        dclk_cnt<=dclk_cnt+1;
                    end
                end
                else if(dclk_flag==1)begin
                    if((round_cnt==round_max-1)&&(point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_flag<=1;
                        point_flag<=1;
                    end
                    else if((point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_cnt<=round_cnt+1;
                        point_cnt<=0;
                        Sa_cd_cnt<=0;
                        point_flag<=1;
                    end
                    else if(Sa_cd_cnt==Sa_cd_max-1)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=0;
                    end
                    else if(Sa_cd_cnt==0)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt+1;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=1;
                    end
                    else if((round_cnt<=round_max-1)&&(point_cnt<=point_max)&&(Sa_cd_cnt<=Sa_cd_max-1))begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=0;
                    end
                    else begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt;
                        Sa_clk<=Sa_clk;
                    end
                end
			end
			TIME_5us: begin
    			dclk_max<=(EQUI_F_DIV/4)*round_cnt;
    			round_max<=4;
    			point_max<=100;
    			Sa_cd_max<=EQUI_F_DIV;
                if(round_flag)begin
                    round_cnt<=0;
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                    round_flag<=0;
                end
                if(trig_flag&&point_flag)begin
        		    point_flag<=0;
                    dclk_flag<=0;
                end
                if(point_flag)begin
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                end
                else if(dclk_flag==0)begin
                    if(dclk_max==0)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt>=dclk_max)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt<dclk_max)begin
                        dclk_cnt<=dclk_cnt+1;
                    end
                end
                else if(dclk_flag==1)begin
                    if((round_cnt==round_max-1)&&(point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_flag<=1;
                        point_flag<=1;
                    end
                    else if((point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_cnt<=round_cnt+1;
                        point_cnt<=0;
                        Sa_cd_cnt<=0;
                        point_flag<=1;
                    end
                    else if(Sa_cd_cnt==Sa_cd_max-1)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=0;
                    end
                    else if(Sa_cd_cnt==0)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt+1;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=1;
                    end
                    else if((round_cnt<=round_max-1)&&(point_cnt<=point_max)&&(Sa_cd_cnt<=Sa_cd_max-1))begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=0;
                    end
                    else begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt;
                        Sa_clk<=Sa_clk;
                    end
                end
			end
			TIME_2us: begin
    			dclk_max<=(EQUI_F_DIV/10)*round_cnt;
    			round_max<=10;
    			point_max<=40;
    			Sa_cd_max<=EQUI_F_DIV;
                if(round_flag)begin
                    round_cnt<=0;
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                    round_flag<=0;
                end
                if(trig_flag&&point_flag)begin
        		    point_flag<=0;
                    dclk_flag<=0;
                end
                if(point_flag)begin
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                end
                else if(dclk_flag==0)begin
                    if(dclk_max==0)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt>=dclk_max)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt<dclk_max)begin
                        dclk_cnt<=dclk_cnt+1;
                    end
                end
                else if(dclk_flag==1)begin
                    if((round_cnt==round_max-1)&&(point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_flag<=1;
                        point_flag<=1;
                    end
                    else if((point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_cnt<=round_cnt+1;
                        point_cnt<=0;
                        Sa_cd_cnt<=0;
                        point_flag<=1;
                    end
                    else if(Sa_cd_cnt==Sa_cd_max-1)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=0;
                    end
                    else if(Sa_cd_cnt==0)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt+1;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=1;
                    end
                    else if((round_cnt<=round_max-1)&&(point_cnt<=point_max)&&(Sa_cd_cnt<=Sa_cd_max-1))begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=0;
                    end
                    else begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt;
                        Sa_clk<=Sa_clk;
                    end
                end
			end
			TIME_1us: begin
   			    dclk_max<=(EQUI_F_DIV/20)*round_cnt;
    			round_max<=20;
    			point_max<=20;
    			Sa_cd_max<=EQUI_F_DIV;
                if(round_flag)begin
                    round_cnt<=0;
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                    round_flag<=0;
                end
                if(trig_flag&&point_flag)begin
        		    point_flag<=0;
                    dclk_flag<=0;
                end
                if(point_flag)begin
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                end
                else if(dclk_flag==0)begin
                    if(dclk_max==0)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt>=dclk_max)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt<dclk_max)begin
                        dclk_cnt<=dclk_cnt+1;
                    end
                end
                else if(dclk_flag==1)begin
                    if((round_cnt==round_max-1)&&(point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_flag<=1;
                        point_flag<=1;
                    end
                    else if((point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_cnt<=round_cnt+1;
                        point_cnt<=0;
                        Sa_cd_cnt<=0;
                        point_flag<=1;
                    end
                    else if(Sa_cd_cnt==Sa_cd_max-1)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=0;
                    end
                    else if(Sa_cd_cnt==0)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt+1;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=1;
                    end
                    else if((round_cnt<=round_max-1)&&(point_cnt<=point_max)&&(Sa_cd_cnt<=Sa_cd_max-1))begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=0;
                    end
                    else begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt;
                        Sa_clk<=Sa_clk;
                    end
                end
			end
			TIME_500ns: begin
	   			dclk_max<=(EQUI_F_DIV/40)*round_cnt;
    			round_max<=40;
    			point_max<=10;
    			Sa_cd_max<=EQUI_F_DIV;
                if(round_flag)begin
                    round_cnt<=0;
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                    round_flag<=0;
                end
                if(trig_flag&&point_flag)begin
        		    point_flag<=0;
                    dclk_flag<=0;
                end
                if(point_flag)begin
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                end
                else if(dclk_flag==0)begin
                    if(dclk_max==0)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt>=dclk_max)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt<dclk_max)begin
                        dclk_cnt<=dclk_cnt+1;
                    end
                end
                else if(dclk_flag==1)begin
                    if((round_cnt==round_max-1)&&(point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_flag<=1;
                        point_flag<=1;
                    end
                    else if((point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_cnt<=round_cnt+1;
                        point_cnt<=0;
                        Sa_cd_cnt<=0;
                        point_flag<=1;
                    end
                    else if(Sa_cd_cnt==Sa_cd_max-1)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=0;
                    end
                    else if(Sa_cd_cnt==0)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt+1;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=1;
                    end
                    else if((round_cnt<=round_max-1)&&(point_cnt<=point_max)&&(Sa_cd_cnt<=Sa_cd_max-1))begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=0;
                    end
                    else begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt;
                        Sa_clk<=Sa_clk;
                    end
                end
			end
			TIME_200ns: begin
	   			dclk_max<=(EQUI_F_DIV/100)*round_cnt;
    			round_max<=100;
    			point_max<=4;
    			Sa_cd_max<=EQUI_F_DIV;
                if(round_flag)begin
                    round_cnt<=0;
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                    round_flag<=0;
                end
                if(trig_flag&&point_flag)begin
        		    point_flag<=0;
                    dclk_flag<=0;
                end
                if(point_flag)begin
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                end
                else if(dclk_flag==0)begin
                    if(dclk_max==0)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt>=dclk_max)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt<dclk_max)begin
                        dclk_cnt<=dclk_cnt+1;
                    end
                end
                else if(dclk_flag==1)begin
                    if((round_cnt==round_max-1)&&(point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_flag<=1;
                        point_flag<=1;
                    end
                    else if((point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_cnt<=round_cnt+1;
                        point_cnt<=0;
                        Sa_cd_cnt<=0;
                        point_flag<=1;
                    end
                    else if(Sa_cd_cnt==Sa_cd_max-1)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=0;
                    end
                    else if(Sa_cd_cnt==0)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt+1;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=1;
                    end
                    else if((round_cnt<=round_max-1)&&(point_cnt<=point_max)&&(Sa_cd_cnt<=Sa_cd_max-1))begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=0;
                    end
                    else begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt;
                        Sa_clk<=Sa_clk;
                    end
                end
			end
			TIME_100ns: begin
   			    dclk_max<=(EQUI_F_DIV/200)*round_cnt;
    			round_max<=200;
    			point_max<=2;
    			Sa_cd_max<=EQUI_F_DIV;
                if(round_flag)begin
                    round_cnt<=0;
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                    round_flag<=0;
                end
                if(trig_flag&&point_flag)begin
        		    point_flag<=0;
                    dclk_flag<=0;
                end
                if(point_flag)begin
                    point_cnt<=0;
                    Sa_cd_cnt<=0;
                end
                else if(dclk_flag==0)begin
                    if(dclk_max==0)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt>=dclk_max)begin
                        dclk_cnt<=0;
                        dclk_flag<=1;
                    end
                    else if(dclk_cnt<dclk_max)begin
                        dclk_cnt<=dclk_cnt+1;
                    end
                end
                else if(dclk_flag==1)begin
                    if((round_cnt==round_max-1)&&(point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_flag<=1;
                        point_flag<=1;
                    end
                    else if((point_cnt==point_max)&&(Sa_cd_cnt==Sa_cd_max-1))begin
                        round_cnt<=round_cnt+1;
                        point_cnt<=0;
                        Sa_cd_cnt<=0;
                        point_flag<=1;
                    end
                    else if(Sa_cd_cnt==Sa_cd_max-1)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=0;
                    end
                    else if(Sa_cd_cnt==0)begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt+1;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=1;
                    end
                    else if((round_cnt<=round_max-1)&&(point_cnt<=point_max)&&(Sa_cd_cnt<=Sa_cd_max-1))begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt+1;
                        Sa_clk<=0;
                    end
                    else begin
                        round_cnt<=round_cnt;
                        point_cnt<=point_cnt;
                        Sa_cd_cnt<=Sa_cd_cnt;
                        Sa_clk<=Sa_clk;
                    end
                end
			end
            default: begin
                clk_cnt<=0;
                clk_max<=0;
                dclk_cnt<=0;
                dclk_max<=0;
                dclk_flag<=0;
                round_cnt<=0;
                round_max<=0;
                round_flag<=0;
                point_cnt<=0;
                point_max<=0;
                point_flag<=0;
                Sa_cd_cnt<=0;
                Sa_cd_max<=0;
                Sa_cd_flag<=0;				        
        	end
         endcase
	end
end

endmodule