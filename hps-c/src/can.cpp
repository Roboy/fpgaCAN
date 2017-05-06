#include "can.hpp"

void reset_mode(void *base)
{
    unsigned char temp;
    printf("Reset mode\n");
    temp = read_control(base);
    IOWR_32DIRECT(base,0, (temp | 0x01));
    temp = read_control(base);
}

void op_mode(void *base)
{
    uint8_t temp = read_control(base);
    temp = 0x1;
    IOWR_32DIRECT(base,0, (0xFE & temp));
    temp = read_control(base);
    temp = 0x0;
	IOWR_32DIRECT(base,0, (0xFE & temp));
}

unsigned char read_status(void *base)
{
    unsigned char status;
    status = IORD(base,2);
    printf("============READING STATUS START=============\n");
    printf("status: \t 0x%X \t %u \n", status, status);
    printf("Receive Buffer Status:       %s\n", ((status & 0x1)?"full":"empty"));
    printf("Data Overrun Status:         %s\n", ((status & 0x2)?"overrun, a message was lost":"absent, no data overrun has occurred"));
    printf("Transmit Buffer Status:      %s\n", ((status & 0x4)?"released, may write a message":"locked, cannot access the transmit buffer"));
    printf("Transmit Complete:           %s\n", ((status & 0x8)?"true":"false"));
    printf("Receive Status:              %s\n", ((status & 0x10)?"receive":"idle"));
    printf("Transmit Status:             %s\n", ((status & 0x20)?"transmit":"idle"));
    printf("Error Status:                %s\n", ((status & 0x40)?"error":"ok"));
    printf("Bus Status:                  %s\n", ((status & 0x80)?"bus-off":"bus-on"));
    printf("============READING STATUS END=============\n");
    return(status);
}

uint8_t read_control(void *base){
	printf("============READING CONTROL START=============\n");
	uint8_t control = IORD(base,0);
	printf("control: \t 0x%X \t %u \n", control, control);
	printf("Reset Request:               %s\n", ((control & 0x1)?"present":"absent"));
	printf("Receive Interrupt Enable:    %s\n", ((control & 0x2)?"enabled":"disabled, no data overrun has occurred"));
	printf("Transmit Interrupt Enable:   %s\n", ((control & 0x4)?"enabled":"disabled"));
	printf("Error Interrupt Enable:      %s\n", ((control & 0x8)?"enabled":"disabled"));
	printf("Overrun Interrupt Enable:    %s\n", ((control & 0x10)?"enabled":"disabled"));
	printf("============READING CONTROL END=============\n");
	return control;
}

void read_regs_can(void *base)
{
    int i;
    unsigned char temp;
    for(i = 0; i < 32; i++)
          {
              temp = IORD_32DIRECT(base,i*4);
              printf("Reg %u : %X h        ",i,temp);
              printf("Reg %u: %u decimal \n",i,temp);
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

    do
    {
        status = read_status(base);
        usleep(100000);
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
    uint8_t mode = 0b00000101; // motorola
    IOWR(base,31,mode);

    config_timing_reg(base);

//    config_output(base, normal_mode, push_pull);

    op_mode(base);
    printf("end init \n");

}

void config_timing_reg(void *base)
{
	uint8_t temp = 0;
	uint8_t SJW = 0b00, baud_rate_prescalar = 1;  // synchronised jump width time is 1 x Tq
	temp = ((SJW << 6)|(1<<baud_rate_prescalar));  // Baud rate prescaler Tq set to 2*2*(1/Fcan) (i.e. 100ns)
													//which achieves 1Mbit/s with the settings below
	// bus timing 0
    IOWR(base,6,temp);
    uint8_t SEG1PH = 0b100; // Phase buffer segment 4 = 5 x Tq
    uint8_t SEG2PH = 0b010; // Phase Buffer segment 2 = 3 x Tq
    uint8_t SAM = 1; // Bus is sampled 3 times at sample point
    temp = 0;
    temp = ((SAM<<7)|(SEG2PH<<4)|SEG1PH);
    // bus timing 1
    IOWR(base,7,temp);
}

void config_output(void *base, unsigned char mode, unsigned char output_pol)
{
    unsigned char temp;

    temp= mode | output_pol;
    IOWR(base,8,temp);
}
