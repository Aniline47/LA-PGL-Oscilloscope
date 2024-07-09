module top_module(
    //�����Դ�
    input                     sys_clk          ,//
    input                     sys_rst_n        ,//
    //��ť
    input                     key_force_trig   ,//P3
    input                     key_run          ,//P4
    input                     key_single       ,//L6
    input                     key_auto         ,//M5  //TODO
    //������
    input                     enc_clk_time     ,//U1
	input                     enc_dt_time      ,//U2
    input                     enc_sw_time      ,//P1
    input                     enc_clk_ampl     ,//M1
	input                     enc_dt_ampl      ,//M3
    input                     enc_sw_ampl      ,//T1
    //�����ź�
    input                     trigger          ,//P2
    output                    Sa_hold          ,//T2
    //���ͨ��
	input                     spi_cs           ,//P7
	input                     spi_clk          ,//N6
	input                     spi_mosi         ,//T5
	output                    spi_miso         ,//R5

	output                    en_force_trig    ,//P8
    output                    spi_data_ready   ,//N7
	output                    state_change_flag,//R8
	output       [1:0]        meas_state       ,//T8 V13
    output       [4:0]        time_state       ,//U13 T6 M11 T12 U8
    output       [3:0]        ampl_state       ,//V6 N11 V12 V8 
    //adcģ��
    input        [9:0]        adc_data_1  /* synthesis PAP_MARK_DEBUG="true" */,  //��һ·ADC����
    input                     ad_otr_1         ,//
    output                    ad_oe_1          ,//
    output                    ad_clk_1         ,//
    output                    ad_oe_2           //
);

wire clk_250m;
wire clk_200m;
wire clk_500m/* synthesis PAP_MARK_DEBUG="true" */;

wire [7:0]  ad_data_H_1;
wire [7:0]  ad_data_L_1;
wire        ad_clk_1_raw;
wire [7:0]  spi_data;
wire fifo_wr_flag;
wire fifo_rd_flag;

wire trig_flag;
wire single_flag;
wire equi_flag;

wire f_st_re_div_32;

assign Sa_hold = ad_clk_1;

output_shaping u_output_shaping(
    .sys_clk                  (clk_200m), // ʱ���ź�
    .sys_rst_n                (sys_rst_n), // ��λ�ź�

    .en_pulse                 (ad_clk_1_raw),  //��������
    .gate_set                 (ad_clk_1)//�������20ns�ĸߵ�ƽ
);

f_measure u_f_measure(
    .sys_clk                  (clk_200m), // ʱ���ź�
    .sys_rst_n                (sys_rst_n), // ��λ�ź�

    .trigger                  (trigger),  //ԭʼ�����ź�
    .f_st_re_div_32           (f_st_re_div_32)
);

force_trig u_force_trig(
    .ADC_clk                  (clk_200m), // ʱ���ź�
    .sys_rst_n                (sys_rst_n), // ��λ�ź�

    .en_force_trig            (en_force_trig),
    .trigger                  (trigger),  //ԭʼ�����ź�
    .time_state               (time_state),  //��ǰ״̬
    .trig_flag                (trig_flag)  //���δ����ź�
);

ip_fifo u_ip_fifo(
    .sys_clk                  (clk_200m), // ʱ���ź�
    .sys_rst_n                (sys_rst_n), // ��λ�ź�
                              
    .trig_flag                (trig_flag),
	.single_flag              (single_flag),
    .equi_flag                (equi_flag),

    .fifo_wr_flag             (fifo_wr_flag), // ʱ���ź�
    .fifo_rd_flag             (fifo_rd_flag), // ��λ�ź�
                              
    .adc_data_H               (ad_data_H_1), // д�뵽FIFO������
    .adc_data_L               (ad_data_L_1), // д�뵽FIFO������
    .spi_data                 (spi_data),// ��FIFO����������
    .spi_data_ready           (spi_data_ready),// ��FIFO����������

    .meas_state               (meas_state),   
    .time_state               (time_state)
);

adc_rd u_adc_rd(
    .sys_clk                  (clk_200m),
	.sys_rst_n                (sys_rst_n),
                              
    .trig_flag                (trig_flag),//��ǰ״̬
    .time_state               (time_state),//��ǰ״̬
    //��һ·ADC
    .ad_data_1                (adc_data_1)/* synthesis PAP_MARK_DEBUG="true" */,  //��һ·ADC����
    .ad_otr_1                 (ad_otr_1  )/* synthesis PAP_MARK_DEBUG="true" */,  //��һ·ADC�����ѹ�������̱�־
    .Sa_clk                   (ad_clk_1_raw),   //��һ·ADC����ʱ��
    .ad_oe_1                  (ad_oe_1),  //��һ·ADC���ʹ��
    .ad_data_H_1              (ad_data_H_1)/* synthesis PAP_MARK_DEBUG="true" */,  //��һ·ADC���ݸ�λ
    .ad_data_L_1              (ad_data_L_1)/* synthesis PAP_MARK_DEBUG="true" */,  //��һ·ADC���ݵ�λ
    .fifo_wr_flag             (fifo_wr_flag),
    .equi_flag                (equi_flag),

    //�ڶ�·ADC
    .ad_oe_2                  (ad_oe_2)//�ڶ�·ADC���ʹ��
);

spi_drive u_spi_drive(
	.sys_rst_n                (sys_rst_n),
    .sys_clk                  (clk_200m) ,

	.spi_data                 (spi_data),
	.fifo_rd_flag             (fifo_rd_flag),
	//spi interface
	.spi_cs                   (spi_cs)   ,//SPI�ӻ���Ƭѡ�źţ��͵�ƽ��Ч��
	.spi_clk                  (spi_clk)  ,//���ӻ�֮�������ͬ��ʱ�ӡ�
	.spi_mosi                 (spi_mosi) ,//�������ţ�����������ӻ����롣
	.spi_miso                 (spi_miso)  //�������ţ��������룬�ӻ������
);

state_control u_state_control(
    .sys_clk                  (clk_200m),
    .sys_rst_n                (sys_rst_n),

    .key_run                  (key_run),
    .key_single               (key_single),
    .key_force_trig           (key_force_trig),

    .enc_clk_time             (enc_clk_time),//ʱ��
	.enc_dt_time              (enc_dt_time),
    .enc_sw_time              (enc_sw_time),

    .enc_clk_ampl             (enc_clk_ampl),//����
	.enc_dt_ampl              (enc_dt_ampl),
    .enc_sw_ampl              (enc_sw_ampl),

	.en_force_trig            (en_force_trig),
    .state_change_flag        (state_change_flag),
	.meas_state               (meas_state),
	.key_filter_single_neg    (single_flag),
	.time_state               (time_state),
	.ampl_state               (ampl_state)
);

pll_clk u_pll_clk (
    .clkin1                   (sys_clk )       , // input
    .pll_lock                 (pll_lock )      , // output
    .clkout0                  (clk_250m )      , // output
    .clkout1                  (clk_200m)       , // output
    .clkout2                  (clk_500m)         // output
);

endmodule