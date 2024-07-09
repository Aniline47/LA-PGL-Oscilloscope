module divider_32(
    input              sys_clk      ,  //ϵͳʱ��
    input              sys_rst_n    ,  //ϵͳ��λ

    input              en_div_pulse ,
    input       [31:0] top_number   ,  //32λ����
    input       [31:0] botton_number,  //32λ��ĸ
    output  reg [31:0] result  //32λ���
);

parameter  div_max = 32;
reg [ 5:0] div_cnt;
reg        div_state;
reg [63:0] top_number0;  //32λ����
reg [63:0] botton_number0;  //32λ��ĸ
reg [31:0] result0; //32λ���

always @(posedge sys_clk or negedge sys_rst_n )begin//���������ݲ���
	if(!sys_rst_n)begin
        div_cnt <= 0;
        div_state <= 0;
        top_number0 <= 0;  //32λ����
        botton_number0 <= 0;  //32λ��ĸ
        result0 <= 0;
        result <= 0;
    end
    else begin
        if(en_div_pulse)begin
            div_state <= 1;
            div_cnt <= 0;
            top_number0[31:0] <= top_number;  //32λ����
            botton_number0[31:0]<= botton_number;  //32λ��ĸ
            result0 <= 0;
        end
        else if(div_cnt>=div_max)begin
            result <= result0;
            div_state <= 0;
            div_cnt <= 0;
        end
        else if(div_state)begin
            if((botton_number0<<(31-div_cnt))>top_number0)begin
                result0[31-div_cnt]<= 0;
                div_cnt <= div_cnt+1;
            end
            else if((botton_number0<<(31-div_cnt))<=top_number0)begin
                top_number0 <= top_number0-(botton_number0<<(31-div_cnt));
                result0[31-div_cnt]<= 1;
                div_cnt <= div_cnt+1;
            end
        end
            else if(!div_state)begin
            div_cnt <= 0;
            div_state <= 0;
            top_number0 <= 0;  //32λ����
            botton_number0 <= 0;  //32λ��ĸ
            result0 <= 0;
        end
	end
end

endmodule