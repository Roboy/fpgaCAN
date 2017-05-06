/*
 * myoControlRegister.hpp
 *
 *  Created on: Mar 2, 2017
 *      Author: letrend
 */
#pragma once

#define MOTORS_PER_MYOCONTROL 7

#define IORD(base,reg) (*(((volatile int32_t*)base)+reg))
#define IOWR(base,reg,data) (*(((volatile int32_t*)base)+reg)=data)
#define IORD_8DIRECT(base, offset) (*(((volatile int8_t*)base)+offset))
#define IORD_16DIRECT(base, offset) (*(((volatile int16_t*)base)+(offset>>1)))
#define IORD_32DIRECT(base, offset) (*(((volatile int32_t*)base)+(offset>>2)))
#define IOWR_8DIRECT(base, offset, data) (*(((volatile int8_t*)base)+offset)=data)
#define IOWR_16DIRECT(base, offset, data) (*(((volatile int16_t*)base)+(offset>>1))=data)
#define IOWR_32DIRECT(base, offset, data) (*(((volatile int32_t*)base)+(offset>>2))=data)

#define MYO_READ_reset_myo_control(base) IORD(base, 0)
#define MYO_READ_spi_activated(base) IORD(base, 1)
#define MYO_READ_position(base,motor) IORD(base, 2+motor)
#define MYO_READ_velocity(base,motor) IORD(base, 10+motor)
#define MYO_READ_current(base,motor) IORD(base, 18+motor)
#define MYO_READ_displacement(base,motor) IORD(base, 26+motor)
#define MYO_READ_Kp(base,motor) IORD(base, 34+motor)
#define MYO_READ_Kd(base,motor) IORD(base, 42+motor)
#define MYO_READ_Ki(base,motor) IORD(base, 50+motor)
#define MYO_READ_sp(base,motor) IORD(base, 58+motor)
#define MYO_READ_forwardGain(base,motor) IORD(base, 66+motor)
#define MYO_READ_outputPosMax(base,motor) IORD(base, 74+motor)
#define MYO_READ_outputNegMax(base,motor) IORD(base, 82+motor)
#define MYO_READ_IntegralNegMax(base,motor) IORD(base, 90+motor)
#define MYO_READ_IntegralPosMax(base,motor) IORD(base, 98+motor)
#define MYO_READ_deadBand(base,motor) IORD(base, 106+motor)
#define MYO_READ_control(base,motor) IORD(base, 114+motor)
#define MYO_READ_pwmRef(base,motor) IORD(base, 122+motor)

#define MYO_WRITE_reset_myo_control(base,data) IOWR(base, 0, data)
#define MYO_WRITE_spi_activated(base,data) IOWR(base, 1, data)
#define MYO_WRITE_Kp(base,motor,data) IOWR(base, 34+motor, data)
#define MYO_WRITE_Kd(base,motor,data) IOWR(base, 42+motor, data)
#define MYO_WRITE_Ki(base,motor,data) IOWR(base, 50+motor, data)
#define MYO_WRITE_sp(base,motor,data) IOWR(base, 58+motor, data)
#define MYO_WRITE_forwardGain(base,motor,data) IOWR(base, 66+motor, data)
#define MYO_WRITE_outputPosMax(base,motor,data) IOWR(base, 74+motor, data)
#define MYO_WRITE_outputNegMax(base,motor,data) IOWR(base, 82+motor, data)
#define MYO_WRITE_IntegralNegMax(base,motor,data) IOWR(base, 90+motor, data)
#define MYO_WRITE_IntegralPosMax(base,motor,data) IOWR(base, 98+motor, data)
#define MYO_WRITE_deadBand(base,motor,data) IOWR(base, 106+motor, data)
#define MYO_WRITE_control(base,motor,data) IOWR(base, 114+motor, data)
#define MYO_WRITE_reset_controller(base,motor) IOWR(base, 130+motor, 1)
