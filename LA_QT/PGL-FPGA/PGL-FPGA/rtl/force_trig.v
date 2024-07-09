module force_trig(
    input          ADC_clk      ,  //系统时钟
    input          sys_rst_n    ,  //系统复位

    input          en_force_trig,
    input          trigger      ,  //原始触发信号
    input   [4:0]  time_state   ,  //当前状态
    output         trig_flag       //整形触发信号
);

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

reg   [31:0]  force_trig_cnt;
reg   [31:0]  force_trig_max;
reg           force_trig_flag;

reg           trigger0;
reg           trigger1;
assign        trig_flag=(trigger0&&(!trigger1)||(force_trig_flag&&en_force_trig));

always @(posedge ADC_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        trigger0 <= 0;
        trigger1 <= 0;
    end
    else begin
        trigger0 <= trigger;
        trigger1 <= trigger0;
	end
end

always @(posedge ADC_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        force_trig_cnt <= 0;
        force_trig_flag <= 0;
    end
    else begin
		if(force_trig_cnt >= force_trig_max-1)begin
            force_trig_cnt <= 0;
            force_trig_flag <= 1;
        end
        else if(force_trig_cnt < force_trig_max)begin
            force_trig_cnt <= force_trig_cnt+1;
            force_trig_flag <= 0;
        end
        else begin
            force_trig_cnt <= force_trig_cnt;
            force_trig_flag <= force_trig_flag;    
        end
	end
end

always @(*)begin//上升沿数据采样
	if(!sys_rst_n)begin
        force_trig_max = 800000000;
    end
    else begin
		case(time_state)
			TIME_500ms: begin
				force_trig_max = 800000000;
			end
			TIME_200ms: begin
				force_trig_max = 320000000;
			end
			TIME_100ms: begin
				force_trig_max = 160000000;
			end
			TIME_50ms: begin
				force_trig_max =  80000000;
			end
			TIME_20ms: begin
				force_trig_max =  32000000;
			end
			TIME_10ms: begin
				force_trig_max =  16000000;
			end
			TIME_5ms: begin
				force_trig_max =   8000000;
			end
			TIME_2ms: begin
				force_trig_max =   3200000;
			end
			TIME_1ms: begin
				force_trig_max =   1600000;
			end
			TIME_500us: begin
				force_trig_max =    800000;
			end
			TIME_200us: begin
				force_trig_max =    320000;
			end
			TIME_100us: begin
				force_trig_max =    160000;
			end
			TIME_50us: begin
				force_trig_max =     80000;
			end
			TIME_20us: begin
				force_trig_max =     32000;
			end
			TIME_10us: begin
      			force_trig_max =     16000;
			end
			TIME_5us: begin
    			force_trig_max =      8000;
			end
			TIME_2us: begin
    			force_trig_max =      3200;
			end
			TIME_1us: begin
   			    force_trig_max =      1600;
			end
			TIME_500ns: begin
	   			force_trig_max =       800;
			end
			TIME_200ns: begin
	   			force_trig_max =       320;
			end
			TIME_100ns: begin
   			    force_trig_max =       160;
			end
            default: begin
                force_trig_max = 800000000;
        	end
        endcase
	end
end

endmodule