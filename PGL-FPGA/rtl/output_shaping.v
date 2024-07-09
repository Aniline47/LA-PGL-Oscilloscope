module output_shaping(
    input          sys_clk    ,  //系统时钟
    input          sys_rst_n  ,  //系统复位

    input          en_pulse   ,  //激励脉冲
    output   reg   gate_set      //输出门限20ns的高电平
);
parameter     gate_set_max = 5;
reg   [3:0]   gate_set_cnt;

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

endmodule