module output_shaping(
    input          sys_clk    ,  //ϵͳʱ��
    input          sys_rst_n  ,  //ϵͳ��λ

    input          en_pulse   ,  //��������
    output   reg   gate_set      //�������20ns�ĸߵ�ƽ
);
parameter     gate_set_max = 5;
reg   [3:0]   gate_set_cnt;

//*****************************************************
//**����4us���ź�
//*****************************************************

always @(posedge sys_clk or negedge sys_rst_n )begin//���������ݲ���
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