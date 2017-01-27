
//`#start header` -- edit after this line, do not edit this line
// ========================================
//
// Copyright YOUR COMPANY, THE YEAR
// All Rights Reserved
// UNPUBLISHED, LICENSED SOFTWARE.
//
// CONFIDENTIAL AND PROPRIETARY INFORMATION
// WHICH IS THE PROPERTY OF your company.
//
// ========================================
`include "cypress.v"
//`#end` -- edit above this line, do not edit this line
// Generated on 01/10/2017 at 15:56
// Component: MDIO_host_my_v
module MDIO_host_my_v (
	output wire debug,
	output wire interrupt,
	output reg mdc,
	output reg mdio_out,
	input wire clock,
	input wire mdio_in
);

//`#start body` -- edit after this line, do not edit this line

//        Your code goes here

    /**************************************************************************/
    /* Internal Signals                                                       */
    /**************************************************************************/
    reg		    send_preamb;
    reg		    busy;		    	/* Busy condition when processing a frame */ 
    reg         busy_d;             /* Busy delay reg */
    reg         data_bits;          /* Set when data bits are driving */
    reg			mdio_start_d;		/* Delay control start bit */
	wire		start_frame;		/* Start frame pulse */
	reg			end_transfer;
    wire        ack_detect;         /* Set if a ACK is detected */
    reg         f1_load;            /* Loads FIFO F1 from the datapath ALU */
  //  reg         f1_load_p;          /* Pre f1_load */
  //  reg         f1_load_pp;         /* Pre Pre f1_load */
	wire		stop_frame;		/* Stop frame pulse */
    reg         start_frame_d;      /* Delay start_frame */
  //  wire        rising_mdc;	        /* MDC rising edge detected */
  //  wire		falling_mdc;	    /* MDC falling edge detected */
  //  wire	    ta_bits;		    /* Set when turning around bits drive the MDIO */
    wire		ld_count;			/* Load the counter */
    wire		en_count;			/* Enable the counter */
    wire        synced_clock;       /* Internal clock net synchronized to bus_clk */
    wire        tc;                 /* Count7 terminal count output */
    wire	    so;			    	/* Shift out */
    wire [1:0]  ce0;		    	/* A0 == D0 */
    wire [1:0]  ce1;		    	/* A0 == D1 */
    reg  [2:0]  cs_addr;            /* Datapath control store address */
    wire [6:0]  counter;            /* Count7 counter output */
    wire [7:0]  control;		    /* MDIO component control register */
    wire [7:0]	status;				/* MDIO component status register */
    //wire        nc1, nc2, nc3, nc4; /* nc bits connected to unused datapath output flags */
	wire        nc2; /* nc bits connected to unused datapath output flags */
	wire f0_blk_stat;	/* Set to 1 if the FIFO is empty */
	wire f0_bus_stat;	/* Set to 1 if the FIFO is not full */

    
    
    /**************************************************************************/
    /* Clock Synchronization                                                  */
    /**************************************************************************/
    cy_psoc3_udb_clock_enable_v1_0 #(.sync_mode(`TRUE)) ClkSync
    (
        /* input  */    .clock_in(clock),
        /* input  */    .enable(1'b1),
        /* output */    .clock_out(synced_clock)
    );  
    
    
     
   

    /**************************************************************************/
    /* General Purpose Control and Status Registers                           */
    /**************************************************************************/
    /* MDIO Status Register */
	cy_psoc3_status #(.cy_force_order(`TRUE), .cy_md_select(8'b11111111)) MdioStatusReg (
	/* input [07:00] */ .status(status), // Status Bits
	/* input */ .reset(1'b0), // Reset from interconnect
	/* input */ .clock(synced_clock) // Clock used for registering data
	);
	assign status[7:2] = 6'b000000;
	
	/* Stick only if the device acks to the host */
	assign status[0] = ack_detect;
	
	assign status[1] = end_transfer;
	
	assign ack_detect = ~mdio_in & counter[4] & counter[3] & counter[2] & counter[1] & ~counter[0] & data_bits;
	
	
	
	
	/* MDIO Control Register */
    if ( 0 )
	begin: AsyncCtl
		cy_psoc3_control #(.cy_init_value (8'b00000000), .cy_force_order(`TRUE)) MdioControlReg
    	(
		/* output [07:00] */  .control(control)
    	);

	end
	else
	begin: SyncCtl
		cy_psoc3_control #(.cy_init_value (8'b00000000), .cy_force_order(`TRUE), 
					   	   .cy_ctrl_mode_1(8'h00), .cy_ctrl_mode_0(8'hFF)) MdioControlReg
    	(
		/* input          */  .clock(synced_clock),
		/* output [07:00] */  .control(control)
    	);	
	end

	wire mdio_enable = control[0];     	/* Component enable */
    wire mdio_start  = control[1];	   	/* Start Transmission */
    wire mdio_rw     = control[2];	   	/* Read = 1, Write = 0 */
    
assign stop_frame = counter[5] & ~counter[4] & ~counter[3] & ~counter[2] & ~counter[1] & ~counter[0] & mdio_start;
	
	/**************************************************************************/
    /* Count7 block used to count bits on the MDIO frame                      */
    /**************************************************************************/
    cy_psoc3_count7 #(.cy_period(7'd61),.cy_route_ld(`TRUE),.cy_route_en(`TRUE),.cy_alt_mode(`FALSE)) MdioCounter
    (
        /*  input          */  .clock(synced_clock),
        /*  input          */  .reset(1'b0),
        /*  input          */  .load(ld_count),     /* Re-load the counter */
        /*  input          */  .enable(en_count),   	
        /*  output [06:00] */  .count(counter),
        /*  output         */  .tc(tc)
     );

    assign ld_count = ~mdio_start;// | start_frame | tc;	// Load the counter
    assign en_count = busy;// | start_frame;		// Enable the counter
	
	
	
	//assign debug = so;
	
	
	always @(posedge synced_clock)
	begin
		if ( mdc )
			mdio_start_d <= mdio_start;
		else
			mdio_start_d <= mdio_start_d;
	end 
	
	assign start_frame = mdio_start & ~mdio_start_d & mdc;
	
    always @(posedge synced_clock)
    begin
       start_frame_d <= start_frame;
    end
	
	
    /**************************************************************************/
    /* MDC Clock generation                                                   */
    /**************************************************************************/
	always @(posedge synced_clock)
	begin
		if ( ~mdio_start )
			begin
				mdc <= 1'b0;
				end_transfer <= 1'b0;				
			end
		else
		begin
			//if(~end_transfer & ~tc)
			if(~end_transfer)
				begin
					mdc <= ~mdc;
					busy <= 1'b1;
				end	
			else
				begin
					send_preamb <= 1'b0;
					mdc <= 1'b0;
				end
				
			if(tc)
				begin
					if(send_preamb)
						begin
						end_transfer <= 1'b1;
						busy <= 1'b0;
						end
					else
						begin 
							send_preamb <= 1'b1;
						end
				end
		end	
	end
	
	/**************************************************************************/
    /* Frame busy bit				                                          */
    /**************************************************************************/	

	always @(posedge synced_clock)
	begin
	    busy_d <= busy;
	end

	


    // /**************************************************************************/
    // /* MDC bus clock edge detection					     					 */
    // /**************************************************************************/
    // always @(posedge synced_clock)
    // begin
		// mdc_d <= mdc;					/* Delay MDC */
    // end
	
	// assign rising_mdc = mdc & ~mdc_d;	/* Detect rising edge of MDC */
	// assign falling_mdc = ~mdc & mdc_d;	/* Detect falling edge of MDC */

    /**************************************************************************/
    /* MDIO bus drive logic						      						  */
    /**************************************************************************/
	 always @(posedge synced_clock)
	  begin
		if(~send_preamb)
		   mdio_out <= 1'b1;
		else
			mdio_out <= so;
	  end
	
   
	// /**************************************************************************/
    // /* Frame busy bit					                                     */
    // /**************************************************************************/	
	// always @(posedge synced_clock)
	// begin
		// if ( ~mdio_enable )
			// busy <= 1'b0;
		// else if ( start_frame )
			// busy <= 1'b1;
		// else
			// busy <= busy & ~(tc & data_bits);
	// end
	

	/**************************************************************************/
    /* Data bits					                                          */
    /**************************************************************************/
	always @(posedge synced_clock)
	begin
		if ( ~busy )
			data_bits <= 1'b0;
		else
		/* It is set at the first counter tc */
			data_bits <= (data_bits | tc) & ~f1_load;
	end
	
    /**************************************************************************/
    /* Interrupt generation logic                                             */
    /**************************************************************************/
	// always @(posedge synced_clock)
	// begin
		// interrupt <= f1_load;
       // // interrupt <= mdio_start;
	// end
	//assign interrupt = tc & f1_load; 
	assign interrupt = 1'b0; 
    // /**************************************************************************/
    // /* Control Bits for the datapath                                          */
    // /**************************************************************************/

    /* Capture shift register data into F1 (receive FIFO) */
	always @(posedge synced_clock)
	begin
        f1_load <= tc & data_bits & busy;
	end
      
    // always @(posedge synced_clock)
	// begin
       // f1_load_p <= f1_load_pp;
	// end

    // always @(posedge synced_clock)
    // begin
        // f1_load_pp <= tc & data_bits & busy;
    // end

    // /* Assign the data_path control address bits */
    always @(posedge synced_clock)
    begin
		if(send_preamb)
			begin
				cs_addr <= { 1'b0, 1'b1, counter[0] & busy | tc & mdio_rw }; 
			end
		else
			begin
				cs_addr <=  1'b0;
			end
    end
	
	
	
	
	
    /**************************************************************************/
    /* 16-bit datapath configured as MDIO shift registers and parallel to    */
    /* serial interface.                                                      */
    /*                                                                        */
    /**************************************************************************/

cy_psoc3_dp16 #(.cy_dpconfig_a(
{
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM0: Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM1: Shift A0*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM2: Load F0 to A0*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM3: Load F0 to A0*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM4:                          */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM5:                          */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM6:                          */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM7:                          */
    8'hFF, 8'h00,  /*CFG9:                          */
    8'hFF, 8'hFF,  /*CFG11-10:                       Phy Address Match*/
    `SC_CMPB_A1_D1, `SC_CMPA_A0_D1, `SC_CI_B_ARITH,
    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
    `SC_SI_A_ROUTE, /*CFG13-12:                          */
    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
    1'h0, `SC_FIFO1__A0, `SC_FIFO0_BUS,
    `SC_MSB_DSBL, `SC_MSB_BIT7, `SC_MSB_NOCHN,
    `SC_FB_NOCHN, `SC_CMP1_NOCHN,
    `SC_CMP0_NOCHN, /*CFG15-14:                          */
    10'h00, `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
    `SC_FIFO__EDGE,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,
    `SC_WRK16CAT_DSBL /*CFG17-16:                          */
}
), .cy_dpconfig_b(
{
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM0: Idle*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP___SL, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM1: Shift A0*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM2: Load F0 to A0*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM3: Load F0 to A0*/
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM4:                          */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM5:                          */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM6:                          */
    `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
    `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
    `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
    `CS_CMP_SEL_CFGA, /*CFGRAM7:                          */
    8'hFF, 8'h00,  /*CFG9:                          */
    8'hFF, 8'hFF,  /*CFG11-10:                       Phy Address Match*/
    `SC_CMPB_A1_D1, `SC_CMPA_A0_D1, `SC_CI_B_ARITH,
    `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
    `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
    `SC_SI_A_CHAIN, /*CFG13-12:                          */
    `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
    1'h0, `SC_FIFO1__A0, `SC_FIFO0_BUS,
    `SC_MSB_DSBL, `SC_MSB_BIT7, `SC_MSB_NOCHN,
    `SC_FB_NOCHN, `SC_CMP1_CHNED,
    `SC_CMP0_CHNED, /*CFG15-14:                          */
    10'h00, `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,
    `SC_FIFO__EDGE,`SC_FIFO__SYNC,`SC_EXTCRC_DSBL,
    `SC_WRK16CAT_DSBL /*CFG17-16:                          */
}
)) cntrl16(
        /*  input                   */  .reset(1'b0),
        /*  input                   */  .clk(synced_clock),
        /*  input   [02:00]         */  .cs_addr(cs_addr),
        /*  input                   */  .route_si(mdio_in),/* MDIO input to the shift register */
        /*  input                   */  .route_ci(1'b0),
        /*  input                   */  .f0_load(1'b0),
        /*  input                   */  .f1_load(f1_load),/* Loads FIFO 1 from A0 */
        /*  input                   */  .d0_load(1'b0),
        /*  input                   */  .d1_load(1'b0),
        /*  output  [01:00]         */  .ce0({ce0}),   /*Accumulator 0 = Data register 0 */
        /*  output  [01:00]         */  .cl0(),
        /*  output  [01:00]         */  .z0(),
        /*  output  [01:00]         */  .ff0(),
        /*  output  [01:00]         */  .ce1({ce1}), /* Accumulator [0|1] = Data register 1 */
        /*  output  [01:00]         */  .cl1(),
        /*  output  [01:00]         */  .z1(),
        /*  output  [01:00]         */  .ff1(),
        /*  output  [01:00]         */  .ov_msb(),
        /*  output  [01:00]         */  .co_msb(),
        /*  output  [01:00]         */  .cmsb(),
        /*  output  [01:00]         */  .so({so,nc2}),	//Shift Out
        /*  output  [01:00]         */  .f0_bus_stat(f0_bus_stat),
        /*  output  [01:00]         */  .f0_blk_stat(f0_blk_stat),
        /*  output  [01:00]         */  .f1_bus_stat(),
        /*  output  [01:00]         */  .f1_blk_stat()
);
//`#end` -- edit above this line, do not edit this line
endmodule
//`#start footer` -- edit after this line, do not edit this line
//`#end` -- edit above this line, do not edit this line


