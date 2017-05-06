//////////////////////////////////////////////////////////////////////
////                                                              ////
////  can_fifo.v                                                  ////
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
// Revision 1.15  2003/06/11 09:37:05  mohor
// overrun and length_info fifos are initialized at the end of reset.
//
// Revision 1.14  2003/03/05 15:02:30  mohor
// Xilinx RAM added.
//
// Revision 1.13  2003/03/01 22:53:33  mohor
// Actel APA ram supported.
//
// Revision 1.12  2003/02/19 14:44:03  mohor
// CAN core finished. Host interface added. Registers finished.
// Synchronization to the wishbone finished.
//
// Revision 1.11  2003/02/14 20:17:01  mohor
// Several registers added. Not finished, yet.
//
// Revision 1.10  2003/02/11 00:56:06  mohor
// Wishbone interface added.
//
// Revision 1.9  2003/02/09 02:24:33  mohor
// Bosch license warning added. Error counters finished. Overload frames
// still need to be fixed.
//
// Revision 1.8  2003/01/31 01:13:38  mohor
// backup.
//
// Revision 1.7  2003/01/17 17:44:31  mohor
// Fifo corrected to be synthesizable.
//
// Revision 1.6  2003/01/15 13:16:47  mohor
// When a frame with "remote request" is received, no data is stored
// to fifo, just the frame information (identifier, ...). Data length
// that is stored is the received data length and not the actual data
// length that is stored to fifo.
//
// Revision 1.5  2003/01/14 17:25:09  mohor
// Addresses corrected to decimal values (previously hex).
//
// Revision 1.4  2003/01/14 12:19:35  mohor
// rx_fifo is now working.
//
// Revision 1.3  2003/01/09 21:54:45  mohor
// rx fifo added. Not 100 % verified, yet.
//
// Revision 1.2  2003/01/09 14:46:58  mohor
// Temporary files (backup).
//
// Revision 1.1  2003/01/08 02:10:55  mohor
// Acceptance filter added.
//
//
//
//

// synopsys translate_off
`include "timescale.v"
// synopsys translate_on
`include "can_defines.v"

module can_fifo
( 
  clk,
  rst,

  wr,

  data_in,
  addr,
  data_out,
  fifo_selected,

  reset_mode,
  release_buffer,
  extended_mode,
  overrun,
  info_empty,
  info_cnt

);

parameter Tp = 1;

input         clk;
input         rst;
input         wr;
input   [7:0] data_in;
input   [7:0] addr;
input         reset_mode;
input         release_buffer;
input         extended_mode;
input         fifo_selected;

output  [7:0] data_out;
output        overrun;
output        info_empty;
output  [6:0] info_cnt;

`ifdef ACTEL_APA_RAM
`else
`ifdef XILINX_RAM
`else
  reg     [7:0] fifo [0:63];
  reg     [3:0] length_fifo[0:63];
  reg           overrun_info[0:63];
`endif
`endif

reg     [5:0] rd_pointer;
reg     [5:0] wr_pointer;
reg     [5:0] read_address;
reg     [5:0] wr_info_pointer;
reg     [5:0] rd_info_pointer;
reg           wr_q;
reg     [3:0] len_cnt;
reg     [6:0] fifo_cnt;
reg     [6:0] info_cnt;
reg           latch_overrun;
reg           initialize_memories;

wire    [3:0] length_info;
wire          write_length_info;
wire          fifo_empty;
wire          fifo_full;
wire          info_full;

assign write_length_info = (~wr) & wr_q;

// Delayed write signal
always @ (posedge clk or posedge rst)
begin
  if (rst)
    wr_q <= 0;
  else if (reset_mode)
    wr_q <=#Tp 0;
  else
    wr_q <=#Tp wr;
end


// length counter
always @ (posedge clk or posedge rst)
begin
  if (rst)
    len_cnt <= 0;
  else if (reset_mode | write_length_info)
    len_cnt <=#Tp 1'b0;
  else if (wr & (~fifo_full))
    len_cnt <=#Tp len_cnt + 1'b1;
end


// wr_info_pointer
always @ (posedge clk or posedge rst)
begin
  if (rst)
    wr_info_pointer <= 0;
  else if (write_length_info & (~info_full) | initialize_memories)
    wr_info_pointer <=#Tp wr_info_pointer + 1'b1;
  else if (reset_mode)
    wr_info_pointer <=#Tp 0;
end



// rd_info_pointer
always @ (posedge clk or posedge rst)
begin
  if (rst)
    rd_info_pointer <= 0;
  else if (reset_mode)
    rd_info_pointer <=#Tp 0;
  else if (release_buffer & (~fifo_empty))
    rd_info_pointer <=#Tp rd_info_pointer + 1'b1;
end


// rd_pointer
always @ (posedge clk or posedge rst)
begin
  if (rst)
    rd_pointer <= 0;
  else if (release_buffer & (~fifo_empty))
    rd_pointer <=#Tp rd_pointer + length_info;
  else if (reset_mode)
    rd_pointer <=#Tp 0;
end


// wr_pointer
always @ (posedge clk or posedge rst)
begin
  if (rst)
    wr_pointer <= 0;
  else if (wr & (~fifo_full))
    wr_pointer <=#Tp wr_pointer + 1'b1;
  else if (reset_mode)
    wr_pointer <=#Tp 0;
end


// latch_overrun
always @ (posedge clk or posedge rst)
begin
  if (rst)
    latch_overrun <= 0;
  else if (reset_mode | write_length_info)
    latch_overrun <=#Tp 0;
  else if (wr & fifo_full)
    latch_overrun <=#Tp 1'b1;
end


// Counting data in fifo
always @ (posedge clk or posedge rst)
begin
  if (rst)
    fifo_cnt <= 0;
  else if (wr & (~release_buffer) & (~fifo_full))
    fifo_cnt <=#Tp fifo_cnt + 1'b1;
  else if ((~wr) & release_buffer & (~fifo_empty))
    fifo_cnt <=#Tp fifo_cnt - length_info;
  else if (wr & release_buffer & (~fifo_full) & (~fifo_empty))
    fifo_cnt <=#Tp fifo_cnt - length_info + 1'b1;
  else if (reset_mode)
    fifo_cnt <=#Tp 0;
end

assign fifo_full = fifo_cnt == 64;
assign fifo_empty = fifo_cnt == 0;


// Counting data in length_fifo and overrun_info fifo
always @ (posedge clk or posedge rst)
begin
  if (rst)
    info_cnt <= 0;
  else if (write_length_info ^ release_buffer)
    begin
      if (release_buffer & (~info_empty))
        info_cnt <=#Tp info_cnt - 1'b1;
      else if (write_length_info & (~info_full))
        info_cnt <=#Tp info_cnt + 1'b1;
    end
end
        
assign info_full = info_cnt == 64;
assign info_empty = info_cnt == 0;


// Selecting which address will be used for reading data from rx fifo
always @ (extended_mode or rd_pointer or addr)
begin
  if (extended_mode)      // extended mode
    begin
      read_address <= rd_pointer + (addr - 8'd16);
    end
  else                    // normal mode
    begin
      read_address <= rd_pointer + (addr - 8'd20);
    end
end


always @ (posedge clk or posedge rst)
begin
  if (rst)
    initialize_memories <= 1;
  else if (&wr_info_pointer)
    initialize_memories <=#Tp 1'b0;
end


`ifdef ACTEL_APA_RAM
  actel_ram_64x8_sync fifo
  (
    .DO      (data_out),
    .RCLOCK  (clk),
    .WCLOCK  (clk),
    .DI      (data_in),
    .PO      (),                       // parity not used
    .WRB     (~(wr & (~fifo_full))),
    .RDB     (~fifo_selected),
    .WADDR   (wr_pointer),
    .RADDR   (read_address)
  );


  actel_ram_64x4_sync info_fifo
  (
    .DO      (length_info),
    .RCLOCK  (clk),
    .WCLOCK  (clk),
    .DI      (len_cnt & {4{~initialize_memories}}),
    .PO      (),                       // parity not used
    .WRB     (~(write_length_info & (~info_full) | initialize_memories)),
    .RDB     (1'b0),                   // always enabled
    .WADDR   (wr_info_pointer),
    .RADDR   (rd_info_pointer)
  );


  actel_ram_64x1_sync overrun_fifo
  (
    .DO      (overrun),
    .RCLOCK  (clk),
    .WCLOCK  (clk),
    .DI      ((latch_overrun | (wr & fifo_full)) & (~initialize_memories)),
    .PO      (),                       // parity not used
    .WRB     (~(write_length_info & (~info_full) | initialize_memories)),
    .RDB     (1'b0),                   // always enabled
    .WADDR   (wr_info_pointer),
    .RADDR   (rd_info_pointer)
  );
`else
`ifdef XILINX_RAM

/*
  ram_64x8_sync fifo
  (
    .addra(wr_pointer),
    .addrb(read_address),
    .clka(clk),
    .clkb(clk),
    .dina(data_in),
    .doutb(data_out),
    .wea(wr & (~fifo_full))
  );
*/

  RAMB4_S8_S8 fifo
  (
    .DOA(),
    .DOB(data_out),
    .ADDRA({3'h0, wr_pointer}),
    .CLKA(clk),
    .DIA(data_in),
    .ENA(1'b1),
    .RSTA(1'b0),
    .WEA(wr & (~fifo_full)),
    .ADDRB({3'h0, read_address}),
    .CLKB(clk),
    .DIB(8'h0),
    .ENB(1'b1),
    .RSTB(1'b0),
    .WEB(1'b0)
  );



/*
  ram_64x4_sync info_fifo
  (
    .addra(wr_info_pointer),
    .addrb(rd_info_pointer),
    .clka(clk),
    .clkb(clk),
    .dina(len_cnt),
    .doutb(length_info),
    .wea(write_length_info & (~info_full))
  );
*/
  RAMB4_S4_S4 info_fifo
  (
    .DOA(),
    .DOB(length_info),
    .ADDRA({4'h0, wr_info_pointer}),
    .CLKA(clk),
    .DIA(len_cnt & {4{~initialize_memories}}),
    .ENA(1'b1),
    .RSTA(1'b0),
    .WEA(write_length_info & (~info_full) | initialize_memories),
    .ADDRB({4'h0, rd_info_pointer}),
    .CLKB(clk),
    .DIB(4'h0),
    .ENB(1'b1),
    .RSTB(1'b0),
    .WEB(1'b0)
  );

/*
  ram_64x1_sync overrun_fifo
  (
    .addra(wr_info_pointer),
    .addrb(rd_info_pointer),
    .clka(clk),
    .clkb(clk),
    .dina(latch_overrun | (wr & fifo_full)),
    .doutb(overrun),
    .wea(write_length_info & (~info_full))
  );
*/


  RAMB4_S1_S1 overrun_fifo
  (
    .DOA(),
    .DOB(overrun),
    .ADDRA({6'h0, wr_info_pointer}),
    .CLKA(clk),
    .DIA((latch_overrun | (wr & fifo_full)) & (~initialize_memories)),
    .ENA(1'b1),
    .RSTA(1'b0),
    .WEA(write_length_info & (~info_full) | initialize_memories),
    .ADDRB({6'h0, rd_info_pointer}),
    .CLKB(clk),
    .DIB(1'h0),
    .ENB(1'b1),
    .RSTB(1'b0),
    .WEB(1'b0)
  );


`else
  // writing data to fifo
  always @ (posedge clk)
  begin
    if (wr & (~fifo_full))
      fifo[wr_pointer] <=#Tp data_in;
  end

  // reading from fifo
  assign data_out = fifo[read_address];


  // writing length_fifo
  always @ (posedge clk)
  begin
    if (write_length_info & (~info_full) | initialize_memories)
      length_fifo[wr_info_pointer] <=#Tp len_cnt & {4{~initialize_memories}};
  end


  // reading length_fifo
  assign length_info = length_fifo[rd_info_pointer];

  // overrun_info
  always @ (posedge clk)
  begin
    if (write_length_info & (~info_full) | initialize_memories)
      overrun_info[wr_info_pointer] <=#Tp (latch_overrun | (wr & fifo_full)) & (~initialize_memories);
  end
  
  
  // reading overrun
  assign overrun = overrun_info[rd_info_pointer];


`endif
`endif





endmodule
