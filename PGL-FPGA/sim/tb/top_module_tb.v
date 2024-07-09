`timescale 1ns / 1ns        //仿真单位/仿真精度

module top_module_tb();

reg grs_n;
GTP_GRS GRS_INST( 
    .GRS_N (grs_n)
);

//parameter define
parameter  CLK_PERIOD_250m = 4;
parameter  CLK_PERIOD_50m = 20;
parameter  CLK_PERIOD_30m = 33;
parameter  CLK_PERIOD_25m = 40;
parameter  CLK_PERIOD_10m = 100;
parameter  CLK_PERIOD_5m = 200;
parameter  CLK_PERIOD_1m333= 750;
parameter  CLK_PERIOD_500k= 2000;
parameter  CLK_PERIOD_25k= 40000;
parameter  CLK_PERIOD_2k= 500000;
parameter  CLK_PERIOD_1k333= 750187;
parameter  CLK_PERIOD_25 = 40000000;
parameter  CLK_PERIOD_1 = 1000000000;
//reg define
reg           sys_clk;
reg           sys_rst_n;

reg           spi_clk;
reg           spi_cs;
reg           spi_mosi;
wire          spi_miso;

reg           key_run;
reg           key_single;

reg           enc_clk_time;
reg           enc_dt_time;
reg           enc_sw_time;

reg           enc_clk_ampl;
reg           enc_dt_ampl;
reg           enc_sw_ampl;

wire          ad_oe_1;
wire          ad_oe_2;

wire          state_change_flag;
wire[1:0]     meas_state;
wire[4:0]     time_state;
wire[2:0]     ampl_state;
wire          ad_clk_1;
reg           trigger;
reg [9:0]     adc_data_1;
//信号初始化
initial begin
    grs_n = 1'b0; 

    sys_clk <= 1'b0;
    sys_rst_n <= 1'b0;

    spi_clk <= 1'b0;
    spi_cs <= 1'b1;
    spi_mosi<=0;

    key_run <= 1;
    key_single <= 1;

    enc_clk_time <=1'b0;
    enc_dt_time <=1'b0;
    enc_sw_time <=1'b0;

    enc_clk_ampl <=1'b0;
    enc_dt_ampl <=1'b0;
    enc_sw_ampl <=1'b0;
    trigger<=0;

    adc_data_1<=10'b1111100000;

//encoder_tb
    #210
    sys_rst_n <= 1'b1;
    #5000
    grs_n = 1'b1;

//    #2000
//    key_run <= 1'b0;
//    #2000
//    key_run <= 1'b1;
//    #2000
//    key_run <= 1'b0;
//    #2000
//    key_run <= 1'b1;
//    #2000
//    key_single <= 1'b0;
//    #2000
//    key_single <= 1'b1;
//    #2000
//    key_single <= 1'b0;
//    #2000
//    key_single <= 1'b1;
//    #2000
//    key_run <= 1'b0;
//    #2000
//    key_run <= 1'b1;

//    #500
//    enc_clk_ampl <= 1'b1;
//    #50
//    enc_dt_ampl <=1'b1;
//    #200
//    enc_clk_ampl <= 1'b0;
//    #50
//    enc_dt_ampl <=1'b0;
//    #500
//    enc_clk_ampl <= 1'b1;
//    #50
//    enc_dt_ampl <=1'b1;
//    #200
//    enc_clk_ampl <= 1'b0;
//    #50
//    enc_dt_ampl <=1'b0;
//    #500
//    enc_clk_ampl <= 1'b1;
//    #50
//    enc_dt_ampl <=1'b1;
//    #200
//    enc_clk_ampl <= 1'b0;
//    #50
//    enc_dt_ampl <=1'b0;
//    #500
//    enc_clk_ampl <= 1'b1;
//    #50
//    enc_dt_ampl <=1'b1;
//    #200
//    enc_clk_ampl <= 1'b0;
//    #50
//    enc_dt_ampl <=1'b0;
//    #500
//    enc_clk_ampl <= 1'b1;
//    #50
//    enc_dt_ampl <=1'b1;
//    #200
//    enc_clk_ampl <= 1'b0;
//    #50
//    enc_dt_ampl <=1'b0;
//
//    #500
//    enc_clk_time <= 1'b1;
//    #50
//    enc_dt_time <=1'b1;
//    #200
//    enc_clk_time <= 1'b0;
//    #50
//    enc_dt_time <=1'b0;
//    #500
//    enc_clk_time <= 1'b1;
//    #50
//    enc_dt_time <=1'b1;
//    #200
//    enc_clk_time <= 1'b0;
//    #50
//    enc_dt_time <=1'b0;
//    #500
//    enc_clk_time <= 1'b1;
//    #50
//    enc_dt_time <=1'b1;
//    #200
//    enc_clk_time <= 1'b0;
//    #50
//    enc_dt_time <=1'b0;
//    #500
//    enc_clk_time <= 1'b1;
//    #50
//    enc_dt_time <=1'b1;
//    #200
//    enc_clk_time <= 1'b0;
//    #50
//    enc_dt_time <=1'b0;
//    #500
//    enc_clk_time <= 1'b1;
//    #50
//    enc_dt_time <=1'b1;
//    #200
//    enc_clk_time <= 1'b0;
//    #50
//    enc_dt_time <=1'b0;

    #1000000
    spi_cs <= 1'b0;
    #1500000
    spi_cs <= 1'b1;
    #1000000
    spi_cs <= 1'b0;
    #1500000
    spi_cs <= 1'b1;
end

//产生时钟
always #(CLK_PERIOD_50m/2) sys_clk = ~sys_clk;
always #(CLK_PERIOD_1m333/2) trigger = ~trigger;
always #(CLK_PERIOD_30m/2) spi_clk = ~spi_clk;
always #(CLK_PERIOD_25k/2) adc_data_1 = ~adc_data_1;

//例化待测设计
top_module  top_module(
    .sys_clk           (sys_clk),
    .sys_rst_n         (sys_rst_n),

    .spi_clk           (spi_clk),
    .spi_cs            (spi_cs),
    .spi_mosi          (spi_mosi),
    .spi_miso          (spi_miso),

    .key_run           (key_run),
    .key_single        (key_single),

    .enc_clk_time      (enc_clk_time),
	.enc_dt_time       (enc_dt_time),
    .enc_sw_time       (enc_sw_time),

    .enc_clk_ampl      (enc_clk_ampl),
    .enc_dt_ampl       (enc_dt_ampl),
    .enc_sw_ampl       (enc_sw_ampl),

    .ad_oe_1           (ad_oe_1),
    .ad_oe_2           (ad_oe_2),

    .trigger           (trigger),
    .adc_data_1        (adc_data_1),

    .ad_clk_1          (ad_clk_1),
    .state_change_flag (state_change_flag),
    .meas_state        (meas_state),
    .time_state        (time_state),
    .ampl_state        (ampl_state)
    );

endmodule