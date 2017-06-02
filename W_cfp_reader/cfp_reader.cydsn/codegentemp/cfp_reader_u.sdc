# Component constraints for D:\PROJECT\pgm\Cypress_dem_board\CFP_reader\cfp_reader\W_cfp_reader\cfp_reader.cydsn\TopDesign\TopDesign.cysch
# Project: D:\PROJECT\pgm\Cypress_dem_board\CFP_reader\cfp_reader\W_cfp_reader\cfp_reader.cydsn\cfp_reader.cyprj
# Date: Fri, 02 Jun 2017 03:12:45 GMT
# MDIO_Interface
# Specify the path from mdio_out generated on the component clock to the input of the DFF clocked from MDC as false
expr {(!0) ?
   [set_false_path -from [get_pins {\MDIO_Interface:mdio_o\/q}] -to [get_pins {\MDIO_Interface:bMDIO:mdio_reg\/main_0}]] : {}}
