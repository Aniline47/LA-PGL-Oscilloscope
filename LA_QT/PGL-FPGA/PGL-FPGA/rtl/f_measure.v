module f_measure(
    input              sys_clk      ,  //系统时钟
    input              sys_rst_n    ,  //系统复位

    input              trigger      ,  //原始触发信号
    output      [31:0] f_st_re_div_32  /* synthesis PAP_MARK_DEBUG="true" */
);
//*****************************************************
//**触发信号整形转化为脉冲
//*****************************************************
reg           trigger0;
reg           trigger1;
assign        trig_flag = trigger0&&(!trigger1);

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        trigger0 <= 0;
        trigger1 <= 0;
    end
    else begin
        trigger0 <= trigger;
        trigger1 <= trigger0;
	end
end

parameter     F_STANDARD = 250000000;//250mhz标准信号
parameter     en_pulse_max = 125000000;//0.5s刷新一次
parameter     gate_set_max = 62500000;//最小测4Hz
reg   [63:0]  frequence_64/* synthesis PAP_MARK_DEBUG="true" */;
reg           en_div_pulse;
reg           en_div_pulse_delay;
reg   [31:0]  trig_cnt;
reg           en_pulse;//每4ms产生一个激励脉冲
reg   [31:0]  en_pulse_cnt;
reg           gate_set;//设置门限4us的高电平
reg   [31:0]  gate_set_cnt;
reg           gate_real;//实际的高电平
reg   [31:0]  gate_real_cnt;

//*****************************************************
//**每4ms产生一个激励脉冲
//*****************************************************

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        en_pulse_cnt <= 0;
        en_pulse <= 0;
    end
    else begin
        if(en_pulse_cnt >= en_pulse_max-1)begin
            en_pulse_cnt <= 0;
            if((gate_set_cnt <= 0)&&(gate_real_cnt <= 0)&&(trig_cnt <= 0))begin
                en_pulse <= 1;
            end
        end
        else if(en_pulse_cnt < en_pulse_max-1)begin
            en_pulse_cnt <= en_pulse_cnt+1;
            en_pulse <= 0;
        end
	end
end

//*****************************************************
//**生成4us门信号
//*****************************************************

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        gate_set_cnt <= 0;
        gate_set <= 0;
    end
    else begin
        if((en_pulse||gate_set))begin
            if(gate_set_cnt >= gate_set_max)begin
                gate_set_cnt <= 0;
                gate_set <= 0;
            end
            else if(gate_set_cnt < gate_set_max)begin
                gate_set_cnt <= gate_set_cnt+1;
                gate_set <= 1;
            end
        end
	end
end

//*****************************************************
//**生成实际门信号
//*****************************************************

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        gate_real <= 0;
    end
    else begin
        if(trig_flag&&gate_set)begin
            gate_real <= 1;
        end 
        else if(trig_flag&&~gate_set)begin
            gate_real <= 0;
        end
	end
end

//*****************************************************
//**实际门信号期间计数
//*****************************************************

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        gate_real_cnt <= 0;
        trig_cnt <= 0;
        frequence_64 <= 0;
        en_div_pulse <= 0;
    end
    else begin
        if(gate_real)begin
            gate_real_cnt <= gate_real_cnt+1;
            if(trig_flag)begin
                trig_cnt <= trig_cnt+1;
            end
        end 
        else if(gate_real_cnt&&trig_cnt)begin
            frequence_64 <= {trig_cnt[31:0],gate_real_cnt[31:0]};
            en_div_pulse <= 1;
            gate_real_cnt <= 0;
            trig_cnt <= 0;
        end
        else begin
            frequence_64 <= frequence_64;
            en_div_pulse <= 0;
            gate_real_cnt <= 0;
            trig_cnt <= 0;
        end
	end
end

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
        en_div_pulse_delay <= 0;
    end
    else begin
        en_div_pulse_delay <= en_div_pulse;
	end
end

//*****************************************************
//**f_st_re_div_32计算
//*****************************************************

divider_32 u_divider_32(
    .sys_clk                  (sys_clk), // 时钟信号
    .sys_rst_n                (sys_rst_n), // 复位信号

    .en_div_pulse             (en_div_pulse_delay),
    .top_number               (frequence_64[31: 0]), //32位分子(大)
    .botton_number            (frequence_64[63:32]), //32位分母(小)
    .result                   (f_st_re_div_32)//32位结果
);

endmodule