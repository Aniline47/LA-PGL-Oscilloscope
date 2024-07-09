module state_control(
    input            sys_clk          ,
	input            sys_rst_n        ,
                                    
    input            key_run          ,
    input            key_single       ,
    input            key_force_trig   , 
    input            enc_clk_time     ,
	input            enc_dt_time      ,
    input            enc_sw_time      ,
    input            enc_clk_ampl     ,
	input            enc_dt_ampl      ,
    input            enc_sw_ampl      ,
                                      
	output reg       en_force_trig    ,
	output           state_change_flag,
	output reg[1:0]  meas_state       ,
	output key_filter_single_neg      ,
	output reg[4:0]  time_state       ,
	output reg[3:0]  ampl_state    
   );
//*****************************************************
//**判别运行状态是否改变
//*****************************************************
reg[1:0]  meas_state0;
reg[4:0]  time_state0;
reg[3:0]  ampl_state0;

assign state_change_flag = (meas_state0 != meas_state)||(time_state0 != time_state)||(ampl_state0 != ampl_state);

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        meas_state0 <= 0;
        time_state0 <= 0;
        ampl_state0 <= 0;
    end
    else begin
        meas_state0 <= meas_state;
        time_state0 <= time_state;
        ampl_state0 <= ampl_state;
	end
end

//*****************************************************
//**运行控制按钮
//*****************************************************
parameter RUN    =   2'b00;
parameter SINGLE =   2'b01;
parameter PAUSE  =   2'b10;
wire      key_filter_single     ;
wire      key_filter_run        ;
reg       key_filter_single0    ;
reg       key_filter_single1    ;
reg       key_filter_run0       ;
reg       key_filter_run1       ;
assign    key_filter_single_neg = ~key_filter_single0&&key_filter_single1;
assign    key_filter_run_neg = ~key_filter_run0&&key_filter_run1;
always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		key_filter_single0 <= 0;
        key_filter_single1 <= 0;
    end
    else begin
		key_filter_single0 <= key_filter_single;
        key_filter_single1 <= key_filter_single0;
	end
end
always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		key_filter_run0 <= 0;
        key_filter_run1 <= 0;
    end
    else begin
		key_filter_run0 <= key_filter_run;
        key_filter_run1 <= key_filter_run0;
	end
end
key_debounce u_key_debounce_single(
	.sys_rst_n    (sys_rst_n)  ,
    .sys_clk      (sys_clk)    ,

    .key          (key_single) ,       //外部输入的按键值
    .key_filter   (key_filter_single)  //按键消抖后的值
);

key_debounce u_key_debounce_run(
	.sys_rst_n    (sys_rst_n)  ,
    .sys_clk      (sys_clk)    ,

    .key          (key_run) ,       //外部输入的按键值
    .key_filter   (key_filter_run)  //按键消抖后的值
);

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		meas_state <= RUN;
    end
    else if(meas_state==RUN)begin
		if(key_filter_run_neg)
            meas_state <= PAUSE;
		else if(key_filter_single_neg)
            meas_state <= SINGLE;
		else
            meas_state <= meas_state;
    end
    else if(meas_state==SINGLE)begin
		if(key_filter_run_neg)
            meas_state <= PAUSE;
		else
            meas_state <= meas_state;
    end
    else if(meas_state==PAUSE)begin
		if(key_filter_run_neg)
            meas_state <= RUN;
		else if(key_filter_single_neg)
            meas_state <= SINGLE;
		else
            meas_state <= meas_state;
    end
    else begin
		meas_state <= meas_state;
    end
end
//*****************************************************
//**时基调节编码器
//*****************************************************
parameter TIME_STATE_MAX=5'b10100;
parameter TIME_STATE_MIN=5'b00011;
wire enc_flag_time_shun;
wire enc_flag_time_ni;

reg       enc_flag_time_shun0       ;
reg       enc_flag_time_shun1       ;
reg       enc_flag_time_ni0         ;
reg       enc_flag_time_ni1         ;

assign enc_flag_time_shun_pos = enc_flag_time_shun0&&~enc_flag_time_shun1;
assign enc_flag_time_ni_pos   = enc_flag_time_ni0&&~enc_flag_time_ni1    ;

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		enc_flag_time_shun0 <= 0;
        enc_flag_time_shun1 <= 0;
    end
    else begin
        enc_flag_time_shun0 <= enc_flag_time_shun;
        enc_flag_time_shun1 <= enc_flag_time_shun0;
	end
end

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		enc_flag_time_ni0 <= 0;
        enc_flag_time_ni1 <= 0;
    end
    else begin
        enc_flag_time_ni0 <= enc_flag_time_ni;
        enc_flag_time_ni1 <= enc_flag_time_ni0;
	end
end

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		time_state <= 5'b01101;
    end
    else if(enc_flag_time_shun_pos==1&&time_state<TIME_STATE_MAX)begin
		time_state <= time_state+1;
    end
    else if(enc_flag_time_ni_pos==1&&time_state>TIME_STATE_MIN)begin
        time_state <= time_state-1;
    end
    else if(time_state>TIME_STATE_MAX)begin
		time_state <= TIME_STATE_MAX;
    end
    else if(time_state<TIME_STATE_MIN)begin
        time_state <= TIME_STATE_MIN;
    end
    else begin
		time_state <= time_state;
    end
end

encoder_drive u_encoder_drive_time(
	.sys_rst_n    (sys_rst_n),
    .sys_clk      (sys_clk),

    .enc_clk      (enc_clk_time),
	.enc_dt       (enc_dt_time),
    .enc_sw       (enc_sw_time),
	.enc_flag_shun(enc_flag_time_shun),
	.enc_flag_ni  (enc_flag_time_ni)
);
//*****************************************************
//**幅度调节编码器
//*****************************************************
parameter AMPL_STATE_MAX=4'b1000;
parameter AMPL_STATE_MIN=4'b0001;
wire enc_flag_ampl_shun;
wire enc_flag_ampl_ni;

reg enc_flag_ampl_shun0 ;
reg enc_flag_ampl_shun1 ;
reg enc_flag_ampl_ni0 ;
reg enc_flag_ampl_ni1 ;

assign enc_flag_ampl_shun_pos = enc_flag_ampl_shun0&&~enc_flag_ampl_shun1;
assign enc_flag_ampl_ni_pos = enc_flag_ampl_ni0&&~enc_flag_ampl_ni1 ;

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
    if(!sys_rst_n)begin
        enc_flag_ampl_shun0 <= 0;
        enc_flag_ampl_shun1 <= 0;
    end
    else begin
        enc_flag_ampl_shun0 <= enc_flag_ampl_shun;
        enc_flag_ampl_shun1 <= enc_flag_ampl_shun0;
    end
end

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
    if(!sys_rst_n)begin
        enc_flag_ampl_ni0 <= 0;
        enc_flag_ampl_ni1 <= 0;
    end
    else begin
        enc_flag_ampl_ni0 <= enc_flag_ampl_ni;
        enc_flag_ampl_ni1 <= enc_flag_ampl_ni0;
    end
end

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
    if(!sys_rst_n)begin
        ampl_state <= AMPL_STATE_MIN;
    end
    else if(enc_flag_ampl_shun_pos==1&&ampl_state<AMPL_STATE_MAX)begin
        ampl_state <= ampl_state+1;
    end
    else if(enc_flag_ampl_ni_pos==1&&ampl_state>AMPL_STATE_MIN)begin
        ampl_state <= ampl_state-1;
    end
    else if(ampl_state>AMPL_STATE_MAX)begin
        ampl_state <= AMPL_STATE_MAX;
    end
    else if(ampl_state<AMPL_STATE_MIN)begin
        ampl_state <= AMPL_STATE_MIN;
    end
    else begin
        ampl_state <= ampl_state;
    end
end

encoder_drive u_encoder_drive_ampl(
    .sys_rst_n (sys_rst_n),
    .sys_clk (sys_clk),
    
    .enc_clk (enc_clk_ampl),
    .enc_dt (enc_dt_ampl),
    .enc_sw (enc_sw_ampl),
    .enc_flag_shun(enc_flag_ampl_shun),
    .enc_flag_ni (enc_flag_ampl_ni)
);
//*****************************************************
//**强制触发按钮
//*****************************************************
wire      key_filter_force_trig ;
reg       key_filter_force_trig0;
reg       key_filter_force_trig1;
assign    key_filter_force_trig_neg = ~key_filter_force_trig0&&key_filter_force_trig1;
always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		key_filter_force_trig0 <= 0;
        key_filter_force_trig1 <= 0;
    end
    else begin
		key_filter_force_trig0 <= key_filter_force_trig;
        key_filter_force_trig1 <= key_filter_force_trig0;
	end
end

key_debounce u_key_debounce_force_trig(
	.sys_rst_n    (sys_rst_n)  ,
    .sys_clk      (sys_clk)    ,

    .key          (key_force_trig) ,       //外部输入的按键值
    .key_filter   (key_filter_force_trig)  //按键消抖后的值
);

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		en_force_trig <= 0;
    end
    else if(meas_state==RUN)begin
		if(key_filter_force_trig_neg)
            en_force_trig <= ~en_force_trig;
		else
            en_force_trig <= en_force_trig;
    end
    else if(meas_state==SINGLE)begin
		en_force_trig <= 0;
    end
    else if(meas_state==PAUSE)begin
		if(key_filter_force_trig_neg)
            en_force_trig <= ~en_force_trig;
		else
            en_force_trig <= en_force_trig;
    end
    else begin
		en_force_trig <= en_force_trig;
    end
end

endmodule