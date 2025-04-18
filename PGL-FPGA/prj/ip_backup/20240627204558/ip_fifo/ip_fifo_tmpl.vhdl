-- Created by IP Generator (Version 2022.2-SP4.2 build 132111)
-- Instantiation Template
--
-- Insert the following codes into your VHDL file.
--   * Change the_instance_name to your own instance name.
--   * Change the net names in the port map.


COMPONENT ip_fifo
  PORT (
    wr_clk : IN STD_LOGIC;
    wr_rst : IN STD_LOGIC;
    wr_en : IN STD_LOGIC;
    wr_data : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    wr_byte_en : IN STD_LOGIC;
    wr_full : OUT STD_LOGIC;
    wr_water_level : OUT STD_LOGIC_VECTOR(10 DOWNTO 0);
    almost_full : OUT STD_LOGIC;
    rd_clk : IN STD_LOGIC;
    rd_rst : IN STD_LOGIC;
    rd_en : IN STD_LOGIC;
    rd_data : OUT STD_LOGIC_VECTOR(7 DOWNTO 0);
    rd_empty : OUT STD_LOGIC;
    rd_water_level : OUT STD_LOGIC_VECTOR(10 DOWNTO 0);
    almost_empty : OUT STD_LOGIC
  );
END COMPONENT;


the_instance_name : ip_fifo
  PORT MAP (
    wr_clk => wr_clk,
    wr_rst => wr_rst,
    wr_en => wr_en,
    wr_data => wr_data,
    wr_byte_en => wr_byte_en,
    wr_full => wr_full,
    wr_water_level => wr_water_level,
    almost_full => almost_full,
    rd_clk => rd_clk,
    rd_rst => rd_rst,
    rd_en => rd_en,
    rd_data => rd_data,
    rd_empty => rd_empty,
    rd_water_level => rd_water_level,
    almost_empty => almost_empty
  );
