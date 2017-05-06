//////////////////////////////////////////////////////////////////////
////                                                              ////
////  can_btl.v                                                   ////
////                                                              ////
////                                                              ////
////  This file is part of the CAN Protocol Controller            ////
////  http://www.opencores.org/projects/can/                      ////
////                                                              ////
////                                                              ////
////  Author(s):                                                  ////
////       Igor Mohor                                             ////
////       igorm@opencores.org                                    ////
////                                                              ////
////                                                              ////
////  All additional information is available in the README.txt   ////
////  file.                                                       ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2002, 2003 Authors                             ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//// The CAN protocol is developed by Robert Bosch GmbH and       ////
//// protected by patents. Anybody who wants to implement this    ////
//// CAN IP core on silicon has to obtain a CAN protocol license  ////
//// from Bosch.                                                  ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//
// CVS Revision History
//
// $Log: not supported by cvs2svn $
// Revision 1.11  2003/02/09 18:40:29  mohor
// Overload fixed. Hard synchronization also enabled at the last bit of
// interframe.
//
// Revision 1.10  2003/02/09 02:24:33  mohor
// Bosch license warning added. Error counters finished. Overload frames
// still need to be fixed.
//
// Revision 1.9  2003/01/31 01:13:38  mohor
// backup.
//
// Revision 1.8  2003/01/10 17:51:34  mohor
// Temporary version (backup).
//
// Revision 1.7  2003/01/08 02:10:53  mohor
// Acceptance filter added.
//
// Revision 1.6  2002/12/28 04:13:23  mohor
// Backup version.
//
// Revision 1.5  2002/12/27 00:12:52  mohor
// Header changed, testbench improved to send a frame (crc still missing).
//
// Revision 1.4  2002/12/26 01:33:05  mohor
// Tripple sampling supported.
//
// Revision 1.3  2002/12/25 23:44:16  mohor
// Commented lines removed.
//
// Revision 1.2  2002/12/25 14:17:00  mohor
// Synchronization working.
//
// Revision 1.1.1.1  2002/12/20 16:39:21  mohor
// Initial
//
//
//

// synopsys translate_off
`include "timescale.v"
// synopsys translate_on
`include "can_defines.v"

module can_btl
( 
  clk,
  rst,
  rx,

  /* Mode register */
  reset_mode,

  /* Bus Timing 0 register */
  baud_r_presc,
  sync_jump_width,

  /* Bus Timing 1 register */
  time_segment1,
  time_segment2,
  triple_sampling,

  /* Output signals from this module */
  clk_en,
  sample_point,
  sampled_bit,
  sampled_bit_q,
  tx_point,
  hard_sync,
  resync,
  
  /* Output from can_bsp module */
  rx_idle,
  transmitting,
  last_bit_of_inter
  
  
  


);

parameter Tp = 1;

input         clk;
input         rst;
input         rx;

  /* Mode register */
input         reset_mode;

/* Bus Timing 0 register */
input   [5:0] baud_r_presc;
input   [1:0] sync_jump_width;

/* Bus Timing 1 register */
input   [3:0] time_segment1;
input   [2:0] time_segment2;
input         triple_sampling;

/* Output from can_bsp module */
input         rx_idle;
input         transmitting;
input         last_bit_of_inter;

/* Output signals from this module */
output        clk_en;
output        sample_point;
output        sampled_bit;
output        sampled_bit_q;
output        tx_point;
output        hard_sync;
output        resync;



reg     [8:0] clk_cnt;
reg           clk_en;
reg           sync_blocked;
reg           resync_blocked;
reg           sampled_bit;
reg           sampled_bit_q;
reg     [7:0] quant_cnt;
reg     [3:0] delay;
reg           sync;
reg           seg1;
reg           seg2;
reg           resync_latched;
reg           sample_point;
reg     [1:0] sample;

wire          go_sync;
wire          go_seg1;
wire          go_seg2;
wire [8:0]    preset_cnt;
wire          sync_window;



assign preset_cnt = (baud_r_presc + 1'b1)<<1;        // (BRP+1)*2
assign hard_sync  =   (rx_idle | last_bit_of_inter)  & (~rx) & sampled_bit & (~sync_blocked) & (~transmitting);  // Hard synchronization
assign resync     =  (~rx_idle)                      & (~rx) & sampled_bit & (~sync_blocked) & (~resync_blocked) & (~transmitting);  // Re-synchronization


/* Generating general enable signal that defines baud rate. */
always @ (posedge clk or posedge rst)
begin
  if (rst)
    clk_cnt <= 0;
  else if (clk_cnt == (preset_cnt-1))
    clk_cnt <=#Tp 0;
  else
    clk_cnt <=#Tp clk_cnt + 1;
end


always @ (posedge clk or posedge rst)
begin
  if (rst)
    clk_en  <= 1'b0;
  else if (clk_cnt == (preset_cnt-1))
    clk_en  <=#Tp 1'b1;
  else
    clk_en  <=#Tp 1'b0;
end



/* Changing states */
 assign go_sync = clk_en & (seg2 & (~hard_sync) & (~resync) & ((quant_cnt == time_segment2)));
 assign go_seg1 = clk_en & (sync | hard_sync | (resync & seg2 & sync_window) | (resync_latched & sync_window));
 assign go_seg2 = clk_en & (seg1 & (~hard_sync) & (quant_cnt == (time_segment1 + delay)));



/* When early edge is detected outside of the SJW field, synchronization request is latched and performed when
   SJW is reached */
always @ (posedge clk or posedge rst)
begin
  if (rst)
    resync_latched <= 1'b0;
  else if (resync & seg2 & (~sync_window))
    resync_latched <=#Tp 1'b1;
  else if (go_seg1)
    resync_latched <= 1'b0;
end



/* Synchronization stage/segment */
always @ (posedge clk or posedge rst)
begin
  if (rst)
    sync <= 0;
  else if (go_sync)
    sync <=#Tp 1'b1;
  else if (go_seg1)
    sync <=#Tp 1'b0;
end


assign tx_point = go_sync;

/* Seg1 stage/segment (together with propagation segment which is 1 quant long) */
always @ (posedge clk or posedge rst)
begin
  if (rst)
    seg1 <= 1;
  else if (go_seg1)
    seg1 <=#Tp 1'b1;
  else if (go_seg2)
    seg1 <=#Tp 1'b0;
end


/* Seg2 stage/segment */
always @ (posedge clk or posedge rst)
begin
  if (rst)
    seg2 <= 0;
  else if (go_seg2)
    seg2 <=#Tp 1'b1;
  else if (go_sync | go_seg1)
    seg2 <=#Tp 1'b0;
end


/* Quant counter */
always @ (posedge clk or posedge rst)
begin
  if (rst)
    quant_cnt <= 0;
  else if (go_sync | go_seg1 | go_seg2)
    quant_cnt <=#Tp 0;
  else if (clk_en)
    quant_cnt <=#Tp quant_cnt + 1'b1;
end


/* When late edge is detected (in seg1 stage), stage seg1 is prolonged. */
always @ (posedge clk or posedge rst)
begin
  if (rst)
    delay <= 0;
  else if (clk_en & resync & seg1)
    delay <=#Tp (quant_cnt > sync_jump_width)? (sync_jump_width + 1) : (quant_cnt + 1);
  else if (go_sync | go_seg1)
    delay <=#Tp 0;
end


// If early edge appears within this window (in seg2 stage), phase error is fully compensated
assign sync_window = ((time_segment2 - quant_cnt) < ( sync_jump_width + 1));


// Sampling data (memorizing two samples all the time).
always @ (posedge clk or posedge rst)
begin
  if (rst)
    sample <= 2'b11;
  else if (clk_en)
    sample <= {sample[0], rx};
end


// When enabled, tripple sampling is done here.
always @ (posedge clk or posedge rst)
begin
  if (rst)
    begin
      sampled_bit <= 1;
      sampled_bit_q <= 1;
      sample_point <= 0;
    end
  else if (clk_en & (~hard_sync))
    begin
      if (seg1 & (quant_cnt == (time_segment1 + delay)))
        begin
          sample_point <=#Tp 1;
          sampled_bit_q <=#Tp sampled_bit;
          if (triple_sampling)
            sampled_bit <=#Tp (sample[0] & sample[1]) | ( sample[0] & rx) | (sample[1] & rx);
          else
            sampled_bit <=#Tp rx;
        end
    end
  else
    sample_point <=#Tp 0;
end



/* Blocking synchronization (can occur only once in a bit time) */

always @ (posedge clk or posedge rst)
begin
  if (rst)
    sync_blocked <=#Tp 1'b0;
  else if (clk_en)
    begin
      if (hard_sync | resync)
        sync_blocked <=#Tp 1'b1;
      else if (seg2 & quant_cnt == time_segment2)
        sync_blocked <=#Tp 1'b0;
    end
end


/* Blocking resynchronization until reception starts (needed because after reset mode exits we are waiting for
   end-of-frame and interframe. No resynchronization is needed meanwhile). */
always @ (posedge clk or posedge rst)
begin
  if (rst)
    resync_blocked <=#Tp 1'b1;
  else if (reset_mode)
    resync_blocked <=#Tp 1'b1;
  else if (hard_sync)
    resync_blocked <=#Tp 1'b0;
end





endmodule
