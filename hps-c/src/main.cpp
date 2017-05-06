#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"
#include "can.hpp"
#include <limits.h>

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )

int main(int argc, char *argv[]) {

	void *virtual_base;
	int fd;
	int loop_count;
	int led_direction = 0;
	int led_mask = 0x01;
	void *h2p_lw_led_addr, *h2p_lw_can_addr;

	// map the address space for the LED registers into user space so we can interact with them.
	// we'll actually map in the entire CSR span of the HPS since we want to access various registers within that span

	if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return( 1 );
	}

	virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, HW_REGS_BASE );

	if( virtual_base == MAP_FAILED ) {
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return( 1 );
	}
	
	h2p_lw_led_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + LED_PIO_BASE ) & ( unsigned long)( HW_REGS_MASK ) );
	h2p_lw_can_addr=virtual_base + ( ( unsigned long  )( ALT_LWFPGASLVS_OFST + CAN_0_BASE ) & ( unsigned long)( HW_REGS_MASK ) );


	printf("Start CAN operation:\n");
	      unsigned int temp;

	      read_status(h2p_lw_can_addr);
//	      reset_mode(h2p_lw_can_addr);
//	      read_status(h2p_lw_can_addr);
//	      op_mode(h2p_lw_can_addr);
//	      read_status(h2p_lw_can_addr);
//
	      can_init(h2p_lw_can_addr);
//	      read_status(h2p_lw_can_addr);
//
//	      uint8_t can_tx[8];
//	      can_tx[0]= 0x0F;
//	      can_tx[1]= 0x0E;
//	      can_tx[2]= 0x0D;
//	      can_tx[3]= 0x0C;
//	      can_tx[4]= 0x0B;
//	      can_tx[5]= 0x0A;
//	      can_tx[6]= 0xF1;
//	      can_tx[7]= 0xF7;
//
//	      reset_mode(h2p_lw_can_addr);
//	      read_regs_can(h2p_lw_can_addr);
//	      op_mode(h2p_lw_can_addr);
//	      read_regs_can(h2p_lw_can_addr);
//
//	      send_msg(h2p_lw_can_addr, can_tx, 0x0001, 0x08, 0);
//	      read_regs_can(h2p_lw_can_addr);
//	      read_status(h2p_lw_can_addr);

//	unsigned int temp;
//	  int i = 0;
//	  printf("Control Register \n");
//	  temp = IORD(h2p_lw_can_addr,0x0000);
//	  printf("%u : R %u \n",i,temp);
//	  printf("Goint to OP mode \n");
//	   IOWR_32DIRECT(h2p_lw_can_addr,0,32);
//	   temp = IORD_32DIRECT(h2p_lw_can_addr,0x0000);
//	   printf("%u : R %u \n",i,temp);
//	  /*printf("Goint to reset mode \n");
//	  IOWR_32DIRECT(h2p_lw_can_addr,0,32);
//	  temp = IORD_32DIRECT(h2p_lw_can_addr,0x0000);
//	  printf("%u : R %u \n",i,temp); */
//	  printf("Double check \n");
//	  temp = IORD(h2p_lw_can_addr,0x0000);
//	  printf("%u : R %u \n",i,temp);
//	  printf("Read Clock Divider Reg \n");
//	  i=31;
//	  temp = IORD(h2p_lw_can_addr,31);
//	  printf("%u : R %u \n",i,temp);
//	  printf("Set it to extended mode \n");
//	  IOWR(h2p_lw_can_addr,31,128);
//	  temp = IORD(h2p_lw_can_addr,31);
//	  printf("%u : R %u \n",i,temp);
//	  printf("And back \n");
//	  IOWR(h2p_lw_can_addr,31,0);
//	    temp = IORD(h2p_lw_can_addr,31);
//	    printf("%u : R %u \n",i,temp);
//	  for(i = 1; i < 32; i++) {
//		  IOWR_32DIRECT(h2p_lw_can_addr,i*4,0);
//		  temp = IORD_32DIRECT(h2p_lw_can_addr,i*4);
//		  printf("%u: R %u \n",i,temp);
//	  }
//	  printf("Goint to reset mode \n");
//	  IOWR_32DIRECT(h2p_lw_can_addr,0,33);
//	  temp = IORD_32DIRECT(h2p_lw_can_addr,0x0000);
//	  printf("%u : R %u \n",i,temp);
//	  printf("Double check \n");
//	  temp = IORD(h2p_lw_can_addr,0x0000);
//	  for(i = 1; i < 32; i++) {
//	  	  IOWR_32DIRECT(h2p_lw_can_addr,i*4,0);
//	  	  temp = IORD_32DIRECT(h2p_lw_can_addr,i*4);
//	  	  printf("%u: R %u \n",i,temp);
//	    }
//	  printf("\nDONE");

	// clean up our memory mapping and exit
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

	return( 0 );
}
