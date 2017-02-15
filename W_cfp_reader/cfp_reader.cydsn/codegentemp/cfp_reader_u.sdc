# Component constraints for D:\pgm\Cypress_dem_board\CFP_reader\cfp_reader\W_cfp_reader\cfp_reader.cydsn\TopDesign\TopDesign.cysch
# Project: D:\pgm\Cypress_dem_board\CFP_reader\cfp_reader\W_cfp_reader\cfp_reader.cydsn\cfp_reader.cyprj
# Date: Wed, 15 Feb 2017 02:43:22 GMT
# MDIO_Interface
# Specify the path from mdio_out generated on the component clock to the input of the DFF clocked from MDC as false
expr {(!0) ?
   [set_false_path -from [get_pins {\MDIO_Interface:mdio_o\/q}] -to [get_pins {\MDIO_Interface:bMDIO:mdio_reg\/main_0}]] : {}}
