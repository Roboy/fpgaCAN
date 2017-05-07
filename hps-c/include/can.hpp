#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "hwlib.h"

#define Basic_can 0x00
#define Peli_can 0x80
#define Fosc_14 0x06
#define Bus_Tx_libre 0x0C

#define IORD(base,reg) (*(((volatile int32_t*)base)+reg))
#define IOWR(base,reg,data) (*(((volatile int32_t*)base)+reg)=data)
#define IORD_8DIRECT(base, offset) (*(((volatile int8_t*)base)+offset))
#define IORD_16DIRECT(base, offset) (*(((volatile int16_t*)base)+(offset>>1)))
#define IORD_32DIRECT(base, offset) (*(((volatile int32_t*)base)+(offset>>2)))
#define IOWR_8DIRECT(base, offset, data) (*(((volatile int8_t*)base)+offset)=data)
#define IOWR_16DIRECT(base, offset, data) (*(((volatile int16_t*)base)+(offset>>1))=data)
#define IOWR_32DIRECT(base, offset, data) (*(((volatile int32_t*)base)+(offset>>2))=data)

void reset_mode(void *base);

void operation_mode(void *base);

uint8_t read_mode(void *base);
uint8_t read_command(void *base);
uint8_t read_status(void *base);
uint8_t read_interrupt_enable_register(void *base);
uint8_t read_arbitraion_lost_capture_register(void *base);
uint8_t read_error_capture_register(void *base);
void read_acceptance_code_register(void *base);
void read_acceptance_mask_register(void *base);

void read_regs_can(void *base);

void send_msg(void *base, unsigned char *vector, unsigned int id, unsigned char long_bytes_msg,  unsigned char rtr);

void receive_msg(void *base, unsigned char *vector, unsigned int id, unsigned char long_bytes_msg);

void can_init(void *base);

void config_timing_reg(void *base);

void config_output(void *base, unsigned char mode, unsigned char output_pol);
