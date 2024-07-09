module encoder_drive(
    input            sys_clk       ,
	input            sys_rst_n     ,

    input            enc_clk       ,
	input            enc_dt        ,
    input            enc_sw        ,
	output reg       enc_flag_shun ,
	output reg       enc_flag_ni
);

wire enc_clk_pos;
wire enc_clk_neg;

reg       enc_clk0       ;
reg       enc_clk1       ;

assign enc_clk_pos = enc_clk0&&~enc_clk1;
assign enc_clk_neg = ~enc_clk0&&enc_clk1;

//*****************************************************
//**
//*****************************************************

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		enc_clk0 <= 0;
        enc_clk1 <= 0;
    end
    else begin
        enc_clk0 <= enc_clk;
        enc_clk1 <= enc_clk0;
	end
end

always @(posedge sys_clk or negedge sys_rst_n )begin//上升沿数据采样
	if(!sys_rst_n)begin
		enc_flag_shun <= 0;
        enc_flag_ni <= 0;
    end
    else if(enc_clk_pos==1&&enc_dt==0)begin
		enc_flag_shun <= 1;
    end
    else if(enc_clk_neg==1&&enc_dt==1)begin
		enc_flag_shun <= 1;
    end    
    else if(enc_clk_pos==1&&enc_dt==1)begin
        enc_flag_ni <= 1;
    end
    else if(enc_clk_neg==1&&enc_dt==0)begin
        enc_flag_ni <= 1;
    end    
    else begin
		enc_flag_shun <= 0;
        enc_flag_ni <= 0;
    end
end
endmodule