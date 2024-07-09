module key_debounce(
    input        sys_clk   ,
    input        sys_rst_n ,

    input        key       ,   //�ⲿ����İ���ֵ
    output  reg  key_filter    //�����������ֵ
);

//parameter define
parameter  CNT_MAX = 20'd5_000_000;    //����ʱ��=CNT_MAX/��Ƶ(250_000_000)(s)

//reg define
reg [19:0] cnt ;
reg        key_d0;            //�������ź��ӳ�һ��ʱ������
reg        key_d1;            //�������ź��ӳ�����ʱ������

//*****************************************************
//**                    main code
//*****************************************************

//�԰����˿ڵ������ӳ�����ʱ������
always @ (posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n) begin
        key_d0 <= 1'b1;
        key_d1 <= 1'b1;
    end
    else begin
        key_d0 <= key;
        key_d1 <= key_d0;
    end 
end

//����ֵ����
always @ (posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n) 
        cnt <= 20'd0;
    else begin
        if(key_d1 != key_d0)    //��⵽����״̬�����仯
            cnt <= CNT_MAX;     //�򽫼�������Ϊ20'd100_0000��
                                //����ʱ100_0000 * 20ns(1s/50MHz) = 20ms
        else begin              //�����ǰ����ֵ��ǰһ������ֵһ����������û�з����仯
            if(cnt > 20'd0)     //��������ݼ���0
                cnt <= cnt - 1'b1;  
            else
                cnt <= 20'd0;
        end
    end
end

//������������յİ���ֵ�ͳ�ȥ
always @ (posedge sys_clk or negedge sys_rst_n) begin
    if(!sys_rst_n)
        key_filter <= 1'b1;
	//�ڼ������ݼ���1ʱ�ͳ�����ֵ
    else if(cnt == 20'd1) 
		key_filter <= key_d1;
    else
		key_filter <= key_filter;
end

endmodule

