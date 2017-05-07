#include "can.hpp"

void reset_mode(void *base)
{
    printf("Reset mode\n");
    // entering reset mode
    IOWR_32DIRECT(base,0, 0b00000001);

    IOWR_32DIRECT(base,0, 0b00011001);
    IOWR_32DIRECT(base,0, 0b00001000);
    read_mode(base);
}

void operation_mode(void *base)
{
    uint8_t temp = read_mode(base);
    temp = 0x1;
    IOWR_32DIRECT(base,0, (0xFE & temp));
    temp = read_mode(base);
    temp = 0x0;
	IOWR_32DIRECT(base,0, (0xFE & temp));
}

uint8_t read_mode(void *base){
	printf("============READING MODE REGISTER START=============\n");
	uint8_t reg = IORD(base,0);
	printf("mode: \t 0x%X \t %u \n", reg, reg);
	printf("Reset Request:               %s\n", ((reg & 0x1)?"present":"absent"));
	printf("Listen Only Mode:            %s\n", ((reg & 0x2)?"listen only":"normal"));
	printf("Self Test Mode:              %s\n", ((reg & 0x4)?"self test":"normal"));
	printf("Acceptance Filter Mode:      %s\n", ((reg & 0x8)?"single":"dual"));
	printf("Sleep Mode:                  %s\n", ((reg & 0x10)?"sleep":"wake-up"));
	printf("==========================================================\n");
	return reg;
}

uint8_t read_command(void *base){
	printf("============READING COMMAND REGISTER START=============\n");
	uint8_t reg = IORD(base,1);
	printf("command: \t 0x%X \t %u \n", reg, reg);
	printf("Transmission Request:        %s\n", ((reg & 0x1)?"present":"absent"));
	printf("Abort Transmission:          %s\n", ((reg & 0x2)?"present":"absent"));
	printf("Release Receive Buffer:      %s\n", ((reg & 0x4)?"released":"no action"));
	printf("Clear Data Overrun:          %s\n", ((reg & 0x8)?"clear":"no action"));
	printf("Self Reception Request:      %s\n", ((reg & 0x10)?"present":"absent"));
	printf("==========================================================\n");
	return reg;
}

uint8_t read_status(void *base)
{
	uint8_t reg = IORD(base,2);
    printf("============READING STATUS START=============\n");
    printf("status: \t 0x%X \t %u \n", reg, reg);
    printf("Receive Buffer Status:       %s\n", ((reg & 0x1)?"full":"empty"));
    printf("Data Overrun Status:         %s\n", ((reg & 0x2)?"overrun, a message was lost":"absent, no data overrun has occurred"));
    printf("Transmit Buffer Status:      %s\n", ((reg & 0x4)?"released, may write a message":"locked, cannot access the transmit buffer"));
    printf("Transmit Complete:           %s\n", ((reg & 0x8)?"true":"false"));
    printf("Receive Status:              %s\n", ((reg & 0x10)?"receive":"idle"));
    printf("Transmit Status:             %s\n", ((reg & 0x20)?"transmit":"idle"));
    printf("Error Status:                %s\n", ((reg & 0x40)?"error":"ok"));
    printf("Bus Status:                  %s\n", ((reg & 0x80)?"bus-off":"bus-on"));
    printf("==========================================================\n");
    return reg;
}

uint8_t read_interrupt_enable_register(void *base)
{
	uint8_t reg = IORD(base,4);
    printf("============READING INTERRUPT ENABLE REGISTER START=============\n");
    printf("interrupt_enable: \t 0x%X \t %u \n", reg, reg);
    printf("Receive Interrupt:          %s\n", ((reg & 0x1)?"enabled":"disabled"));
    printf("Transmit Interrupt:         %s\n", ((reg & 0x2)?"enabled":"disabled"));
    printf("Error Warning Interrupt:    %s\n", ((reg & 0x4)?"enabled":"disabled"));
    printf("Data Overrun Interrupt:     %s\n", ((reg & 0x8)?"enabled":"disabled"));
    printf("Wake-Up Interrupt:          %s\n", ((reg & 0x10)?"enabled":"disabled"));
    printf("Error Passive Interrupt:    %s\n", ((reg & 0x20)?"enabled":"disabled"));
    printf("Arbitration Lost Interrupt: %s\n", ((reg & 0x40)?"enabled":"disabled"));
    printf("Bus Error Interrupt:        %s\n", ((reg & 0x80)?"enabled":"disabled"));
    printf("==========================================================\n");
    return reg;
}

uint8_t read_arbitraion_lost_capture_register(void *base){
	uint8_t reg = IORD(base,11);
	printf("============READING ARBITRATION LOST CAPTURE REGISTER START=============\n");
	printf("read_arbitraion_lost_capture: \t 0x%X \t %u \n", reg, reg);
	switch(reg){
		case 0: printf("arbitration lost in bit 1 of identifier\n"); break;
		case 1: printf("arbitration lost in bit 2 of identifier\n");break;
		case 2: printf("arbitration lost in bit 3 of identifier\n");break;
		case 3: printf("arbitration lost in bit 4 of identifier\n");break;
		case 4: printf("arbitration lost in bit 5 of identifier\n");break;
		case 5: printf("arbitration lost in bit 6 of identifier\n");break;
		case 6: printf("arbitration lost in bit 7 of identifier\n");break;
		case 7: printf("arbitration lost in bit 8 of identifier\n");break;
		case 8: printf("arbitration lost in bit 9 of identifier\n");break;
		case 9: printf("arbitration lost in bit 10 of identifier\n");break;
		case 10: printf("arbitration lost in bit 11 of identifier\n");break;
		case 11: printf("arbitration lost in bit SRTR\n");break;
		case 12: printf("arbitration lost in bit IDE\n");break;
		case 13: printf("arbitration lost in bit 12 of identifier\n");break;
		case 14: printf("arbitration lost in bit 13 of identifier\n");break;
		case 15: printf("arbitration lost in bit 14 of identifier\n");break;
		case 16: printf("arbitration lost in bit 15 of identifier\n");break;
		case 17: printf("arbitration lost in bit 16 of identifier\n");break;
		case 18: printf("arbitration lost in bit 17 of identifier\n");break;
		case 19: printf("arbitration lost in bit 18 of identifier\n");break;
		case 20: printf("arbitration lost in bit 19 of identifier\n");break;
		case 21: printf("arbitration lost in bit 20 of identifier\n");break;
		case 22: printf("arbitration lost in bit 21 of identifier\n");break;
		case 23: printf("arbitration lost in bit 22 of identifier\n");break;
		case 24: printf("arbitration lost in bit 23 of identifier\n");break;
		case 25: printf("arbitration lost in bit 24 of identifier\n");break;
		case 26: printf("arbitration lost in bit 25 of identifier\n");break;
		case 27: printf("arbitration lost in bit 26 of identifier\n");break;
		case 28: printf("arbitration lost in bit 27 of identifier\n");break;
		case 29: printf("arbitration lost in bit 28 of identifier\n");break;
		case 30: printf("arbitration lost in bit RTR\n");break;
	}

	printf("==========================================================\n");
	return reg;
}

uint8_t read_error_capture_register(void *base){
	printf("============READING ERROR CAPTURE REGISTER START=============\n");
	uint8_t reg = IORD(base,12);
	printf("error capture: \t 0x%X \t %u \n", reg, reg);
	switch(reg>>6){
	case 0: printf("bit error\n");break;
	case 1: printf("form error\n");break;
	case 2: printf("stuff error\n");break;
	case 3: printf("other type of error\n");break;
	}
	if(reg&0x20)
		printf("RX; error occurred during reception\n");
	else
		printf("TX; error occurred during transmission\n");
	switch(reg&0x1F){
	case 0b00011: printf("start of frame\n"); break;
	case 0b00010: printf("ID.28 to ID.21\n"); break;
	case 0b00110: printf("ID.20 to ID.18\n"); break;
	case 0b00100: printf("bit SRTR\n"); break;
	case 0b00101: printf("bit IDE\n"); break;
	case 0b00111: printf("ID.17 to ID.13\n"); break;
	case 0b01111: printf("ID.12 to ID.5\n"); break;
	case 0b01110: printf("ID.4 to ID.0\n"); break;
	case 0b01100: printf("bit RTR\n"); break;
	case 0b01101: printf("reserved bit 1\n"); break;
	case 0b01001: printf("reserved bit 0\n"); break;
	case 0b01011: printf("data length code\n"); break;
	case 0b01010: printf("data field\n"); break;
	case 0b01000: printf("CRC sequence\n"); break;
	case 0b11000: printf("CRC delimiter\n"); break;
	case 0b11001: printf("acknowledge slot\n"); break;
	case 0b11011: printf("acknowledge delimiter\n"); break;
	case 0b11010: printf("end of frame\n"); break;
	case 0b10010: printf("intermission\n"); break;
	case 0b10001: printf("active error flag\n"); break;
	case 0b10110: printf("passive error flag\n"); break;
	case 0b10011: printf("tolerate dominant bits\n"); break;
	case 0b10111: printf("error delimiter\n"); break;
	case 0b11100: printf("overload flag\n"); break;
	}
	printf("==========================================================\n");
	return reg;
}

void read_acceptance_code_register(void *base){
	printf("============READING ACCEPTANCE CODE REGISTER START=============\n");
	uint8_t acr0 = IORD(base,16);
	uint8_t acr1 = IORD(base,17);
	uint8_t acr2 = IORD(base,18);
	uint8_t acr3 = IORD(base,19);
	printf("acr0: \t 0x%X \t %u \n", acr0, acr0);
	printf("acr1: \t 0x%X \t %u \n", acr1, acr1);
	printf("acr2: \t 0x%X \t %u \n", acr2, acr2);
	printf("acr3: \t 0x%X \t %u \n", acr3, acr3);
	printf("==========================================================\n");
}

void read_acceptance_mask_register(void *base){
	printf("============READING ACCEPTANCE MASK REGISTER START=============\n");
	uint8_t amr0 = IORD(base,20);
	uint8_t amr1 = IORD(base,21);
	uint8_t amr2 = IORD(base,22);
	uint8_t amr3 = IORD(base,23);
	printf("amr0: \t 0x%X \t %u \n", amr0, amr0);
	printf("amr1: \t 0x%X \t %u \n", amr1, amr1);
	printf("amr2: \t 0x%X \t %u \n", amr2, amr2);
	printf("amr3: \t 0x%X \t %u \n", amr3, amr3);
	printf("==========================================================\n");
}

void read_regs_can(void *base)
{
    int i;
    unsigned char temp;
    for(i = 0; i < 31; i++){
	  temp = IORD_32DIRECT(base,i*4);
	  printf("Reg %u:\t 0x%X  \n",i,temp);
    }
}

void send_msg(void *base, unsigned char *vector, unsigned int id, unsigned char long_bytes_msg,  unsigned char rtr)
{
    unsigned char filtro_h, filtro_l, status;
    int i;

    printf("sending msg");

    status=0x00;
    filtro_h = (id >> 3);
    filtro_l = (id << 5);

    if (rtr ==1)
        filtro_l = filtro_l | 0x10;

    if(long_bytes_msg > 0x08)
        long_bytes_msg = 0x08;

    filtro_l = filtro_l | long_bytes_msg;

    IOWR(base,10,filtro_h);
    IOWR(base,11,filtro_l);

    for(i=0 ; i<long_bytes_msg ; i++)
    {
        IOWR(base,(12+i),*vector);
        vector = vector+1;
    }

    // sending request
    IOWR(base,1,0x01);

    do{
    	status = read_status(base);
	}while(status != 0x4);

    printf("Msg Sent \n");
}

void receive_msg(void *base, unsigned char *vector, unsigned int id, unsigned char long_bytes_msg){
	IOWR(base,1,0x02);
}

void can_init(void *base)
{
    printf("init can \n");

    // clock divider register
//    uint8_t mode = 0b00000101; // motorola
    uint8_t mode = 0b00000000; // intel
    IOWR(base,31,mode);

    config_timing_reg(base);

    reset_mode(base);

    // set acceptance filters
	IOWR(base,16,0b00001101);
	IOWR(base,17,0);
	IOWR(base,18,0);
	IOWR(base,19,0);
	IOWR(base,20,0);
	IOWR(base,21,0);
	IOWR(base,22,0);
	IOWR(base,23,0);
    printf("end init \n");

}

void config_timing_reg(void *base)
{
	uint8_t temp = 0;
	uint8_t SJW = 0b01, baud_rate_prescalar = 0;  // synchronised jump width time is 1 x Tq
	temp = ((SJW << 6)|(1<<baud_rate_prescalar));  // Baud rate prescaler Tq set to 2*2*(1/Fcan) (i.e. 100ns)
													//which achieves 1Mbit/s with the settings below
	// bus timing 0
	printf("setting bus timing 0: \t 0x%X\n", temp);
    IOWR(base,6,temp);
    uint8_t SEG1PH = 8; // Phase buffer segment 4 = 5 x Tq
    uint8_t SEG2PH = 2; // Phase Buffer segment 2 = 3 x Tq
    uint8_t SAM = 1; // Bus is sampled 3 times at sample point
    temp = 0;
    temp = ((SAM<<7)|(SEG2PH<<4)|SEG1PH);
    // bus timing 1
    printf("setting bus timing 1: \t 0x%X\n", temp);
    IOWR(base,7,temp);
}

void config_output(void *base, unsigned char mode, unsigned char output_pol)
{
    unsigned char temp;

    temp= mode | output_pol;
    IOWR(base,8,temp);
}
