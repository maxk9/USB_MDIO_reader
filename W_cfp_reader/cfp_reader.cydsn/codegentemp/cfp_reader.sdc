# THIS FILE IS AUTOMATICALLY GENERATED
# Project: D:\pgm\Cypress_dem_board\CFP_reader\cfp_reader\W_cfp_reader\cfp_reader.cydsn\cfp_reader.cyprj
# Date: Tue, 31 Jan 2017 10:35:34 GMT
#set_units -time ns
create_clock -name {CyXTAL} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/xtal}]]
create_clock -name {CyMASTER_CLK} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/clk_sync}]]
create_generated_clock -name {CyBUS_CLK} -source [get_pins {ClockBlock/clk_sync}] -edges {1 2 3} [list [get_pins {ClockBlock/clk_bus_glb}]]
create_generated_clock -name {Clock_1} -source [get_pins {ClockBlock/clk_sync}] -edges {1 25 49} [list [get_pins {ClockBlock/dclk_glb_0}]]
create_generated_clock -name {Clock_3} -source [get_pins {ClockBlock/clk_sync}] -edges {1 49 97} [list [get_pins {ClockBlock/dclk_glb_1}]]
create_generated_clock -name {Clock_2} -source [get_pins {ClockBlock/clk_sync}] -edges {1 801 1599} -nominal_period 33333.333333333328 [list [get_pins {ClockBlock/dclk_glb_2}]]
create_clock -name {CyILO} -period 10000 -waveform {0 5000} [list [get_pins {ClockBlock/ilo}] [get_pins {ClockBlock/clk_100k}] [get_pins {ClockBlock/clk_1k}] [get_pins {ClockBlock/clk_32k}]]
create_clock -name {CyPLL_OUT} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/pllout}]]
create_clock -name {CyIMO} -period 41.666666666666664 -waveform {0 20.8333333333333} [list [get_pins {ClockBlock/imo}]]


# Component constraints for D:\pgm\Cypress_dem_board\CFP_reader\cfp_reader\W_cfp_reader\cfp_reader.cydsn\TopDesign\TopDesign.cysch
# Project: D:\pgm\Cypress_dem_board\CFP_reader\cfp_reader\W_cfp_reader\cfp_reader.cydsn\cfp_reader.cyprj
# Date: Tue, 31 Jan 2017 10:35:23 GMT
# MDIO_Interface
# Specify the path from mdio_out generated on the component clock to the input of the DFF clocked from MDC as false
expr {(!0) ?
   [set_false_path -from [get_pins {\MDIO_Interface:mdio_o\/q}] -to [get_pins {\MDIO_Interface:bMDIO:mdio_reg\/main_0}]] : {}}
